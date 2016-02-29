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

OrbitalPilot::OrbitalPilot(int searchDepth, double radialIncrement, double throttleIncrement)
	: searchDepth(searchDepth), radialIncrement(radialIncrement), throttleIncrement(throttleIncrement)
{

}

PilotAction OrbitalPilot::getCourse(SimFrame *frame)
{
	return getCourse(frame, searchDepth);
}

PilotAction OrbitalPilot::getCourse(SimFrame *frame, int depth)
{
	PilotAction bestAction;
	bestAction.score = evaluateAction(frame, NULL_ACTION, depth);

	// search state space for optimal throttle value
	for(double throttle = throttleIncrement; throttle <= 1.0; throttle += throttleIncrement)
	{
		// search state space for optimal orientation
		for(double angle = 0.0; angle < 2*PI; angle += radialIncrement)
		{
			glm::dvec2 orientation( sin(angle), cos(angle) );

			PilotAction a = {orientation, throttle, 0.0};

			// anticipate consequences
			a.score = evaluateAction(frame, a, depth);

			// select best outcome
			if(a.score < bestAction.score){
				bestAction = a;
			}
		}

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

	double apoapsisPct = apoapsisDifference / (frame->config->goal.desiredApoapsis + frame->config->body.radius);
	double periapsisPct = periapsisDifference / (frame->config->goal.desiredPeriapsis + frame->config->body.radius);

	double crashPenalty = glm::length(frame->position) < frame->config->body.radius ? 1 : 0;

	double dvPct = frame->deltaV() / initialDv;

	// smaller value, closer to goal
	return 1 * apoapsisPct
		+ 1 * periapsisPct
		+ 100 * crashPenalty
		//- 1 * dvPct
		;
}

