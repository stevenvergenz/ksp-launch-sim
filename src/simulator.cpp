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
	prevFrame->orientation = Vec2(0, 1.0).normalized();
	prevFrame->position = Vec2(0.0, config->body.radius);
	prevFrame->velocity = Vec2(config->body.revolutionSpeed, 0.0);
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
		Vec2 thrustAccel;
		double fuelNeeded = engineThrust / (9.82*config->stages[curFrame->currentStage].Isp) * dt;
		//printf("Fuel needed: %lf\n", fuelNeeded);

		// have fuel
		if(fuelNeeded < prevFrame->currentMass - config->stages[curFrame->currentStage].dryMass)
		{
			//printf("Fuel OK\n");
			thrustAccel = engineThrust / prevFrame->currentMass * dt * curFrame->orientation;
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
			double stageAccel = engineThrust/prevFrame->currentMass * oldStageDuration;
			curFrame->currentMass = config->stages[curFrame->currentStage].dryMass;

			/*if(oldStageDuration == 0)
				printf("Fuel depleted\n");
			else
				printf("Fuel depleting\n");*/

			// change stages if available, and keep burning
			if(curFrame->currentStage < config->stageCount-1)
			{
				curFrame->currentStage++;
				engineThrust = 1000.0 * config->stages[curFrame->currentStage].maxThrust;
				stageAccel += engineThrust / config->stages[curFrame->currentStage].totalMass * (dt - oldStageDuration);
				curFrame->currentMass = config->stages[curFrame->currentStage].totalMass -
					engineThrust / (9.82*config->stages[curFrame->currentStage].Isp) * (dt - oldStageDuration);
			}

			thrustAccel = stageAccel * curFrame->orientation;
		}
		//printf("Thrust accel: %lf %lf\n", thrustAccel.x(), thrustAccel.y());

		// compute gravitational force
		double accel = config->body.surfaceGravity * pow(config->body.radius/prevFrame->position.mag(), 2);
		Vec2 g = accel * dt * -prevFrame->position.normalized();

		// apply forces
		curFrame->velocity = (g+thrustAccel)*dt + prevFrame->velocity;
		curFrame->position = 0.5*(g+thrustAccel)*pow(dt,2) + prevFrame->velocity*dt + prevFrame->position;

		// post update
		emit update(curFrame);

		// new becomes old
		prevFrame = curFrame;

		// NOTE: Frames are not freed as the sim progresses.
		// Let the event listener handle that
	}
	// while rocket is above the ground and the sim hasn't timed out
	while(prevFrame->position.mag() > config->body.radius && prevFrame->time < config->params.duration);

	emit done();
}

const SimFrame* Simulator::getLastFlightData()
{
	return lastFlight;
}
