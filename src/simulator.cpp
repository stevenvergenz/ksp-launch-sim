#include "simulator.h"

Simulator::Simulator(SimulationConfig* config) : abort(false), config(config)
{

}

Simulator::~Simulator()
{

}

/*void Simulator::run()
{
	PriorityQueue fringe;
	double spentDV = 0, forwardDV = 100000;

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
	frame->score = evaluateFrame(frame);

	fringe.push(frame);

	while(!abort && !fringe.isEmpty())
	{
		frame = fringe.pop();

		auto orbit = frame->orbit();
		bool goalMet =
			fabs(orbit.x - frame->config->body.radius - frame->config->goal.desiredApoapsis) < 1000
			&& fabs(orbit.y - frame->config->body.radius - frame->config->goal.desiredPeriapsis) < 1000;

		// update latest best path
		double dvToGo = 0;
		if(frame->deltaVSpent > spentDV && (dvToGo = evaluateFrame(frame)) < forwardDV){
			printf("Best score updated\n");
			spentDV = frame->deltaVSpent;
			forwardDV = dvToGo;

			SimFrame* copy = new SimFrame();
			memcpy(copy, frame, sizeof(SimFrame));
			emit update(copy);
		}

		// check for terminal conditions
		if(frame->time > frame->config->params.duration
			|| frame->deltaVVac() < evaluateFrame(frame)
			|| glm::length(frame->position) < frame->config->body.radius
		){
			SimFrame::freeLeaves(frame);
			continue;
		}
		else if(goalMet){
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
		next->score = next->deltaVSpent + evaluateFrame(next);
		fringe.push(next);

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
				next->score = next->deltaVSpent + evaluateFrame(next);

				fringe.push(next);
			}
		}

	}

	// clean up suboptimal paths
	SimFrame* bestFrame;;
	for(int i=0; i<terminalFrames.length(); i++)
	{
		if(terminalFrames[i]->score < bestFrame->score){
			SimFrame::freeLeaves(bestFrame);
			bestFrame = terminalFrames[i];
		}
		else
			SimFrame::freeLeaves(terminalFrames[i]);
	}

	emit done(bestFrame);
}*/

void Simulator::run()
{
	PriorityQueue queue;
	unsigned int processCount;

	// set up initial state
	SimFrame* frame = new SimFrame();
	frame->config = config;
	frame->currentStage = 0;
	frame->currentMass = config->stages[0].totalMass;

	frame->time = 0.0;
	frame->position = glm::dvec2(0.0, config->body.radius+80000);
	frame->velocity = glm::dvec2(2278.9/*2*PI*config->body.radius/config->body.rotationalPeriod*/, 0.0);
	frame->orientation = glm::dvec2(0, 1);
	frame->throttle = 0.0;
	frame->score = evaluateFrame(frame);

	printf("%lf x %lf\n", frame->orbit().x, frame->orbit().y);
	queue.push(frame);

	// while frames in buffer
	while( !abort && !queue.isEmpty() )
	{
		// get deepest, cheapest frame with at least one unexplored successor
		frame = queue.getBest();
		//emit update(frame->clone());

		auto orbit = frame->orbit();
		bool goalMet =
			fabs(orbit.x - frame->config->body.radius - frame->config->goal.desiredApoapsis) < 1000
			&& fabs(orbit.y - frame->config->body.radius - frame->config->goal.desiredPeriapsis) < 1000;

		// update latest best path

		if(++processCount % 1000 == 0){
			SimFrame* copy = new SimFrame();
			memcpy(copy, frame, sizeof(SimFrame));
			emit update(copy);
		}

		// if frame is goal node
		if(goalMet){
			// break and return
			break;
		}
		// else if no successors possible, or entire buffer taken by frame ancestors
		else if(
			// timeout
			frame->time > frame->config->params.duration
			// wasted too much fuel
			|| frame->deltaVVac() < evaluateFrame(frame)
			// crashed
			|| glm::length(frame->position) < frame->config->body.radius
			// entire buffer taken by frame ancestors (unlikely)
			|| frame->time / frame->config->params.timeResolution + 1 >= PriorityQueue::CAPACITY
		){
			// update frame score to infinity and propagate
			frame->score = std::numeric_limits<double>::max();
			if(frame->prev->nextConsideredIndex == frame->prev->nextCount)
				propagateScore(frame->prev);
			queue.reprioritize();
			continue;
		}

		// if no successor list
		if( frame->next == nullptr)
		{
			// generate successors for frame

			frame->nextCount = 2*PI/frame->config->params.radialStep * 1/frame->config->params.throttleStep + 1;
			frame->next = new SimFrame*[frame->nextCount];
			int i = 0;

			// what if I do nothing?
			SimFrame* next = computeNextFrame(frame, glm::dvec2(0,1), 0);
			next->orientation = glm::dvec2(0,1);
			next->throttle = 0;
			next->prev = frame;
			next->score = next->deltaVSpent + evaluateFrame(next);
			frame->next[i++] = next;

			// for each orientation
			for(double angle=0.0; angle < 2*PI; angle += frame->config->params.radialStep)
			{
				glm::dvec2 orientation(sin(angle), cos(angle));

				// for each throttle position
				for(double throttle = frame->config->params.throttleStep; throttle <= 1.0; throttle += frame->config->params.throttleStep)
				{
					// get resulting state
					next = computeNextFrame(frame, orientation, throttle);
					next->orientation = orientation;
					next->throttle = throttle;
					next->prev = frame;
					next->score = next->deltaVSpent + evaluateFrame(next);

					frame->next[i++] = next;
				}
			}

		}

		// add some unbuffered successor to buffer
		SimFrame* successor;
		bool lastSuccessorAdded = false;
		if(frame->nextConsideredIndex < frame->nextCount){
			successor = frame->next[frame->nextConsideredIndex++];
			if(frame->nextConsideredIndex == frame->nextCount)
				lastSuccessorAdded = true;
		}
		else {
			// loop through children to find the first unbuffered successor
			successor = nullptr;
			for(int i=0; i<frame->nextCount; i++){
				if(frame->nextBuffered[i/8] ^ (1 << (i%8))){
					successor = frame->next[i];
					break;
				}
			}
		}

		// if out of memory
		SimFrame* trash = queue.push(successor);
		if(trash != nullptr)
		{
			// store forgotten frame score in parent
			trash->prev->bestForgotten = trash->score;

			// forget lowest priority frame
			if(trash->next != nullptr)
			{
				delete[] trash->next;
				trash->next = nullptr;
				trash->nextCount = 0;
				trash->nextConsideredIndex = 0;
				trash->bestForgotten = INFINITY;
				for(int i=0; i<128; i++){
					trash->nextBuffered[i] = '\0';
				}
			}
		}

		// if last successor just added
		if(lastSuccessorAdded)
		{
			propagateScore(frame);
			// revalidate buffer
			queue.reprioritize();
		}
	}

	// copy best path to new flight
	SimFrame* head = new SimFrame();
	SimFrame* cur = head;
	while(cur != nullptr)
	{
		cur->config = frame->config;
		cur->time = frame->time;
		cur->currentMass = frame->currentMass;
		cur->currentStage = frame->currentStage;
		cur->position = frame->position;
		cur->velocity = frame->velocity;
		cur->orientation = frame->orientation;
		cur->throttle = frame->throttle;
		cur->deltaVSpent = frame->deltaVSpent;

		if(frame->prev != nullptr){
			cur->prev = new SimFrame();
		}
		else {
			cur->prev = nullptr;
		}
		cur = cur->prev;
		frame = frame->prev;
	}
	emit done(head);
}

void Simulator::propagateScore(SimFrame *frame)
{
	while (frame != nullptr && frame->next != nullptr)
	{
		// update score with min(cheapest successor, bestForgotten)
		SimFrame* minChild = frame->next[0];
		for(int i=1; i<frame->nextCount; i++){
			if(frame->next[i]->score < minChild->score)
				minChild = frame->next[i];
		}

		double bestScore;
		if(minChild->score < frame->bestForgotten){
			bestScore = minChild->score;
		}
		else {
			bestScore = frame->bestForgotten;
		}

		if(bestScore != frame->score){
			frame->score = bestScore;
			frame = frame->prev;
		}
		else {
			frame = nullptr;
		}
	}
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
