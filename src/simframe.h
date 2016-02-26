#ifndef SIMFRAME_H
#define SIMFRAME_H

#include <cmath>

#include "configs.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"

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
	glm::dvec2 position;
	glm::dvec2 velocity;
	glm::dvec2 orientation;

	SimFrame* next;
	SimFrame* prev;
};

#endif // SIMFRAME_H
