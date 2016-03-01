#ifndef SIMFRAME_H
#define SIMFRAME_H

#include <cmath>

#include "configs.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


class SimFrame
{
public:
	SimFrame();
	double deltaV() const;
	glm::dvec2 orbit() const;

	SimulationConfig* config;
	double time;

	int currentStage;
	double currentMass;
	glm::dvec2 position;
	glm::dvec2 velocity;

	glm::dvec2 orientation;
	double throttle;

	double score;
	SimFrame* prev;
};

#endif // SIMFRAME_H
