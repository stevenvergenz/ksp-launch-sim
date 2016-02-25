#ifndef SIMFRAME_H
#define SIMFRAME_H

#include <cmath>

#include "configs.h"
#include "vec2.h"

class SimFrame
{
public:
	SimFrame();
	double deltaV();
	double apoapsis();
	double periapsis();

	SimulationConfig* config;
	double time;

	int currentStage;
	double currentMass;
	Vec2 position;
	Vec2 velocity;
	Vec2 orientation;

	SimFrame* next;
	SimFrame* prev;
};

#endif // SIMFRAME_H
