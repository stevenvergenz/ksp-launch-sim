#include "pilot.h"

/*********************************
 * Null pilot implementation
 *********************************/

NullPilot::NullPilot()
{

}

PilotAction NullPilot::getCourse(SimFrame *frame)
{
	PilotAction action = {frame->orientation, 1.0, 0.0};
	return action;
}

double NullPilot::evaluateFrame(SimFrame *frame)
{
	return 0.0;
}


/*********************************
 * Orbital pilot implementation
 *********************************/

OrbitalPilot::OrbitalPilot()
{

}

PilotAction OrbitalPilot::getCourse(SimFrame *frame)
{
	return getCourse(frame, frame->config->params.searchDepth);
}

PilotAction OrbitalPilot::getCourse(SimFrame *frame, int depth)
{
	PilotAction bestAction;
	bestAction.score = evaluateAction(frame, NULL_ACTION, depth);
	if(frame->deltaV() == 0)
		return bestAction;

	//double horizonAngle = atan2( frame->position.x, frame->position.y) + PI/2;

	// search state space for optimal throttle value
	//double throttle = 1.0;
	for(double throttle = frame->config->params.throttleStep; throttle <= 1.0; throttle += frame->config->params.throttleStep)
	{
		// search state space for optimal orientation
		for(double angle = 0.0; angle < 2*PI; angle += frame->config->params.radialStep)
		{
			glm::dvec2 orientation( sin(angle), cos(angle) );

			PilotAction a = {orientation, throttle, 0.0};

			// anticipate consequences
			a.score = evaluateAction(frame, a, depth);

			// select best outcome
			if(frame->time == 0 && depth == frame->config->params.searchDepth){
				double angle = acos( glm::dot(a.orientation, glm::dvec2(frame->position.y, -frame->position.x))
					/ glm::length(frame->position) ) * 180/PI;

				printf("testing %.2lf, %.2lf -> %lf against %lf\n", a.throttle, angle, a.score, bestAction.score);
			}
			if(a.score < bestAction.score){
				if(frame->time == 0 && depth == frame->config->params.searchDepth)
					printf("new best score: %lf\n", a.score);
				bestAction = a;
			}
		}

	}

	if(depth == frame->config->params.searchDepth){
		double angle = acos( glm::dot(bestAction.orientation, glm::dvec2(frame->position.y, -frame->position.x))
			/ glm::length(frame->position) ) * 180/PI;
		printf("[%.2lf] %.2lf, %.2lf -> %lf\n", frame->time, bestAction.throttle, angle, bestAction.score);
	}

	return bestAction;
}

double OrbitalPilot::evaluateAction(SimFrame* frame, PilotAction action, int depth)
{
	SimFrame* nextFrame = Simulator::computeNextFrame(frame, action.orientation, action.throttle);

	double score;
	if(depth <= 0){
		score = evaluateFrame(nextFrame);
	}
	else
	{
		PilotAction nextAction = getCourse(nextFrame, depth-1);
		score = nextAction.score;
	}

	delete nextFrame;
	return score;
}

double OrbitalPilot::evaluateFrame(SimFrame *frame)
{
	glm::dvec2 orbit = frame->orbit();
	double apoapsisDifference = fabs(frame->config->goal.desiredApoapsis + frame->config->body.radius - orbit.x);
	double periapsisDifference = fabs(frame->config->goal.desiredPeriapsis + frame->config->body.radius - orbit.y);

	double pressure = frame->config->body.surfacePressure * pow(E, -(glm::length(frame->position)-frame->config->body.radius)/frame->config->body.scaleHeight);
	if(pressure <= 0.000001*frame->config->body.surfacePressure)
		pressure = 0;
	else if(pressure > frame->config->body.surfacePressure)
		pressure = frame->config->body.surfacePressure;
	double atmospherePenalty = pressure;

	// smaller value, closer to goal
	double score;
	score =
		(1 * apoapsisDifference + 1 * periapsisDifference)// / (frame->deltaV())
		//+ 10000 * crashPenalty
		+ 8 * atmospherePenalty
		- 2 * frame->deltaV()
		;

	return score;
}

