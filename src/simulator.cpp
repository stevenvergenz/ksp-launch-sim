#include "simulator.h"

Simulator::Simulator(SimulationConfig* config) : config(config), lastFlight(nullptr)
{

}

Simulator::~Simulator()
{
	// delete loose sim memory
	if(lastFlight != nullptr){
		SimFrame* i = lastFlight;
		while(i != nullptr){
			SimFrame* tmp = i->next;
			delete i;
			i = tmp;
		}
	}
}

void Simulator::run()
{
	// clean up last flight
	SimFrame* i = lastFlight;
	while(i != nullptr){
		SimFrame* tmp = i->next;
		delete i;
		i = tmp;
	}

	// set up initial state
	SimFrame* prevFrame = new SimFrame();
	prevFrame->config = config;
	prevFrame->time = 0.0;
	prevFrame->orientation = glm::normalize(glm::dvec2(0.0, 1.0));
	prevFrame->position = glm::dvec2(0.0, config->body.radius);
	prevFrame->velocity = glm::dvec2(2*PI*config->body.radius/config->body.rotationalPeriod, 0.0);
	prevFrame->currentStage = 0;
	prevFrame->currentMass = config->stages[0].totalMass;

	prevFrame->prev = nullptr;
	prevFrame->next = nullptr;

	double dt = config->params.timeResolution;

	emit start(prevFrame);

	// start simulation loop
	do
	{
		// initialize new frame
		SimFrame* curFrame = new SimFrame();
		curFrame->config = config;
		curFrame->time = prevFrame->time + dt;
		curFrame->currentStage = prevFrame->currentStage;

		prevFrame->next = curFrame;
		curFrame->prev = prevFrame;

		// update orientation
		curFrame->orientation = prevFrame->orientation;

		// update throttle
		double engineThrust = 1000.0 * config->stages[curFrame->currentStage].maxThrust;

		// compute thrust, update fuel status
		glm::dvec2 thrustAccel;
		double fuelNeeded = engineThrust / (9.82*config->stages[curFrame->currentStage].Isp) * dt;

		// have fuel
		if(fuelNeeded < prevFrame->currentMass - config->stages[curFrame->currentStage].dryMass)
		{
			thrustAccel = engineThrust / prevFrame->currentMass * curFrame->orientation;
			curFrame->currentMass = prevFrame->currentMass - fuelNeeded;
		}
		// out of fuel
		else
		{
			// milk last fuel out of current stage
			double oldStageDuration =
				(prevFrame->currentMass - config->stages[curFrame->currentStage].dryMass)
				* 9.82 * config->stages[curFrame->currentStage].Isp
				/ engineThrust;
			double stageAccel = engineThrust/prevFrame->currentMass * oldStageDuration/dt;
			curFrame->currentMass = config->stages[curFrame->currentStage].dryMass;

			// change stages if available, and keep burning
			if(curFrame->currentStage < config->stageCount-1)
			{
				curFrame->currentStage++;
				engineThrust = 1000.0 * config->stages[curFrame->currentStage].maxThrust;
				stageAccel += engineThrust / config->stages[curFrame->currentStage].totalMass * (dt - oldStageDuration)/dt;
				curFrame->currentMass = config->stages[curFrame->currentStage].totalMass -
					engineThrust / (9.82*config->stages[curFrame->currentStage].Isp) * (dt - oldStageDuration);
			}

			thrustAccel = stageAccel * curFrame->orientation;
		}
		//printf("Thrust accel: %lf %lf\n", thrustAccel.x, thrustAccel.y);

		// compute drag (estimate)
		glm::dvec2 rotationDirection = glm::normalize(glm::dvec2(prevFrame->position.y, -prevFrame->position.x));
		glm::dvec2 airspeed = prevFrame->velocity - 2*PI*glm::length(prevFrame->position)/config->body.rotationalPeriod * rotationDirection;
		glm::dvec2 drag = 0.0001 * pow( glm::length(airspeed), 2.0) * -glm::normalize(prevFrame->velocity);

		// compute gravitational acceleration
		glm::dvec2 g = G * config->body.mass / pow(glm::length(prevFrame->position), 2) * -glm::normalize(prevFrame->position);

		// apply forces
		curFrame->velocity = (g+thrustAccel+drag)*dt + prevFrame->velocity;
		curFrame->position = 0.5*(g+thrustAccel+drag)*pow(dt,2) + prevFrame->velocity*dt + prevFrame->position;

		// post update
		emit update(curFrame);

		// new becomes old
		prevFrame = curFrame;

		// NOTE: Frames are not freed as the sim progresses.
		// Let the event listener handle that
	}
	// while rocket is above the ground and the sim hasn't timed out
	while(glm::length(prevFrame->position) > config->body.radius && prevFrame->time < config->params.duration);

	emit done();
}

const SimFrame* Simulator::getLastFlightData()
{
	return lastFlight;
}
