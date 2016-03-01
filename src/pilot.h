#ifndef PILOT_H
#define PILOT_H

#include <cmath>

#include "glm/glm.hpp"
#include "glm/vec2.hpp"

#include "simframe.h"
#include "simulator.h"

// the result of a pilot's actions
struct PilotAction
{
	glm::dvec2 orientation;
	double throttle;

	double score;
};

const PilotAction NULL_ACTION = {{1,0}, 0, 0};

// base class for all pilots
class Pilot
{
public:
	virtual PilotAction getCourse(SimFrame* frame) = 0;

private:
	virtual double evaluateFrame(SimFrame* frame) = 0;
};


// pilot that only goes straight
class NullPilot : public Pilot
{
public:
	NullPilot();
	PilotAction getCourse(SimFrame *frame);

private:
	double evaluateFrame(SimFrame *frame);
};


// pilot that tries to get to orbit
class OrbitalPilot : public Pilot
{
public:
	OrbitalPilot();
	PilotAction getCourse(SimFrame *frame, int depth);
	PilotAction getCourse(SimFrame *frame);

private:
	double initialDv;

	double evaluateFrame(SimFrame *frame);
	double evaluateAction(SimFrame* frame, PilotAction action, int depth);
};

#endif // PILOT_H
