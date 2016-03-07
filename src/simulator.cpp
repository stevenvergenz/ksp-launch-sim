#include "simulator.h"

Simulator::Simulator(SimulationConfig* config) : abort(false), config(config)
{

}

Simulator::~Simulator()
{

}

void Simulator::run()
{
	PriorityQueue fringe;
	QList<SimFrame*> terminalFrames;

	SimFrame* bestFrame = nullptr;
	double bestScore = 100000;

	// set up initial state
	SimFrame* frame = new SimFrame();
	frame->config = config;
	frame->currentStage = 0;
	frame->currentMass = config->stages[0].totalMass;

	frame->time = 0.0;
	frame->position = glm::dvec2(0.0, config->body.radius);
	frame->velocity = glm::dvec2(2*PI*config->body.radius/config->body.rotationalPeriod, 0.0);
	frame->orientation = glm::dvec2(0, 1);
	frame->throttle = 0.0;
	frame->deltaVSpent = 0.0;

	frame->prev = nullptr;
	frame->_refCount = 0;

	fringe.push(frame, evaluateFrame(frame));

	while(!abort && !fringe.isEmpty())
	{
		double score;
		frame = fringe.pop(&score);
		emit update(frame, score);

		auto orbit = frame->orbit();
		bool goalMet =
			fabs(orbit.x - frame->config->body.radius - frame->config->goal.desiredApoapsis) < 1000
			&& fabs(orbit.y - frame->config->body.radius - frame->config->goal.desiredPeriapsis) < 1000;

		// update latest best path
		if(goalMet && score < bestScore){
			bestFrame = frame;
			bestScore = score;
		}

		// check for terminal conditions
		if(frame->time > frame->config->params.duration
			|| frame->deltaVVac() == 0
			|| glm::length(frame->position) < frame->config->body.radius
			|| goalMet
		){
			terminalFrames.push_back(frame);
			continue;
		}

		// enumerate state space

		// what if I do nothing?
		SimFrame* next = computeNextFrame(frame, glm::dvec2(0,1), 0);
		next->orientation = glm::dvec2(0,1);
		next->throttle = 0;
		next->prev = frame;
		frame->_refCount++;
		next->_refCount = 0;
		fringe.push( next, frame->deltaVSpent + evaluateFrame(next) );

		// for each orientation
		for(double angle=0.0; angle < 2*PI; angle += frame->config->params.radialStep)
		{
			glm::dvec2 orientation(sin(angle), cos(angle));

			// for each throttle position
			for(double throttle=frame->config->params.throttleStep; throttle<1.0; throttle+=frame->config->params.throttleStep)
			{
				// get resulting state
				next = computeNextFrame(frame, orientation, throttle);
				next->orientation = orientation;
				next->throttle = throttle;
				next->prev = frame;
				frame->_refCount++;
				next->_refCount = 0;
				fringe.push( next, frame->deltaVSpent + evaluateFrame(next) );
			}
		}

	}

	// clean up suboptimal paths
	for(int i=0; i<terminalFrames.length(); i++)
		if(terminalFrames[i] != bestFrame)
			SimFrame::freeLeaves(terminalFrames[i]);


	emit done(bestFrame);
}



SimFrame* Simulator::computeNextFrame(SimFrame *prevFrame, glm::dvec2 orientation, double throttle)
{
	SimulationConfig* config = prevFrame->config;
	double dt = config->params.timeResolution;

	// initialize new frame
	SimFrame* curFrame = new SimFrame();
	curFrame->config = prevFrame->config;
	curFrame->time = prevFrame->time + dt;
	curFrame->currentStage = prevFrame->currentStage;

	// compute atmo drag (estimate)
	glm::dvec2 rotationDirection = glm::normalize(glm::dvec2(prevFrame->position.y, -prevFrame->position.x));
	glm::dvec2 airspeed = prevFrame->velocity - 2*PI*glm::length(prevFrame->position)/config->body.rotationalPeriod * rotationDirection;
	double pressure = config->body.surfacePressure * pow(E, -(glm::length(prevFrame->position)-config->body.radius)/config->body.scaleHeight);
	if(pressure <= 0.000001*config->body.surfacePressure)
		pressure = 0;
	else if(pressure > config->body.surfacePressure)
		pressure = config->body.surfacePressure;

	double density = pressure / (R_CONST * tempFromHeight(glm::length(prevFrame->position)-config->body.radius));
	glm::dvec2 drag = 0.5 * density * pow(glm::length(airspeed), 2.0) * config->stages[curFrame->currentStage].dragCoefficient
		* config->stages[curFrame->currentStage].crossSectionalArea / prevFrame->currentMass * -glm::normalize(prevFrame->velocity);

	// update engine characteristics in atmo
	double atmoThrust = (pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].maxThrustAtmo
		+ (1 - pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].maxThrustVac;
	double atmoIsp = (pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].IspAtmo
		+ (1 - pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].IspVac;

	// update orientation
	curFrame->orientation = glm::length(orientation) == 1 ? orientation : prevFrame->orientation;

	// update throttle
	double engineThrust = 1000.0 * atmoThrust * throttle;

	// compute thrust, update fuel status
	glm::dvec2 thrustAccel;
	double fuelNeeded = engineThrust / (9.82*atmoIsp) * dt;

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
			* 9.82 * atmoIsp
			/ engineThrust;
		double stageAccel = engineThrust/prevFrame->currentMass * oldStageDuration/dt;
		curFrame->currentMass = config->stages[curFrame->currentStage].dryMass;

		// change stages if available, and keep burning
		if(curFrame->currentStage < config->stageCount-1)
		{
			curFrame->currentStage++;

			// update engine characteristics in atmo
			atmoThrust = (pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].maxThrustAtmo
				+ (1 - pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].maxThrustVac;
			atmoIsp = (pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].IspAtmo
				+ (1 - pressure/config->body.surfacePressure) * config->stages[curFrame->currentStage].IspVac;

			engineThrust = 1000.0 * atmoThrust * throttle;
			stageAccel += engineThrust / config->stages[curFrame->currentStage].totalMass * (dt - oldStageDuration)/dt;
			curFrame->currentMass = config->stages[curFrame->currentStage].totalMass -
				engineThrust / (9.82*atmoIsp) * (dt - oldStageDuration);
		}

		thrustAccel = stageAccel * curFrame->orientation;
	}
	//printf("Thrust accel: %lf %lf\n", thrustAccel.x, thrustAccel.y);

	// compute gravitational acceleration
	glm::dvec2 g = G * config->body.mass / pow(glm::length(prevFrame->position), 2) * -glm::normalize(prevFrame->position);

	// apply forces
	curFrame->velocity = (g+thrustAccel+drag)*dt + prevFrame->velocity;
	curFrame->position = 0.5*(g+thrustAccel+drag)*pow(dt,2) + prevFrame->velocity*dt + prevFrame->position;
	curFrame->deltaVSpent = prevFrame->deltaVSpent + prevFrame->deltaVVac() - curFrame->deltaVVac();
	return curFrame;
}


double Simulator::evaluateFrame(SimFrame *frame)
{
	glm::dvec2 orbit = frame->orbit();
	double mu = G * frame->config->body.mass;

	double actualV1, neededV1, deltaV1;
	double actualV2, neededV2, deltaV2;
	double neededEnergy, newRadius, oppositeRadius;

	// rename goal apo/peri based on current estimated apoapsis
	if(orbit.x > frame->config->goal.desiredPeriapsis){
		newRadius = frame->config->body.radius + frame->config->goal.desiredPeriapsis;
		oppositeRadius = frame->config->body.radius + frame->config->goal.desiredApoapsis;
	}
	else {
		newRadius = frame->config->body.radius + frame->config->goal.desiredApoapsis;
		oppositeRadius = frame->config->body.radius + frame->config->goal.desiredPeriapsis;
	}

	// compute delta-v to raise orbit to goal from current apoapsis
	double h = glm::length( glm::cross(glm::dvec3(frame->position, 0.0), glm::dvec3(frame->velocity, 0.0)) );
	actualV1 = h / orbit.x;
	neededEnergy = -mu / (orbit.x + newRadius);
	neededV1 = sqrt( 2*neededEnergy + 2*mu/orbit.x );
	deltaV1 = fabs(neededV1 - actualV1);

	// compute delta-v to raise old apoapsis to goal from opposite side
	actualV2 = sqrt( 2*neededEnergy	+ 2*mu/newRadius );
	neededEnergy = -mu / (newRadius + oppositeRadius);
	neededV2 = sqrt( 2*neededEnergy + 2*mu/newRadius );
	deltaV2 = fabs(neededV2 - actualV2);

	return deltaV1 + deltaV2;
}

double Simulator::tempFromHeight(double height)
{
	double geoHeight = 7963.75 * height / (6371 + 1.25*height);

	if(geoHeight < 10000)
		return 290 - (75/10000)*height;
	else if(geoHeight < 20000)
		return 215;
	else if(geoHeight < 46000)
		return 215 + (55/26000)*height;
	else if(geoHeight < 51000)
		return 270;
	else if(geoHeight < 71000)
		return 270 - (55/20000)*height;
	else if(geoHeight < 85000)
		return 215 - (20/14000)*height;
	else
		return 195;
}
