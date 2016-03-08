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

	// sim info
	SimulationConfig* config;
	double time;

	// rocket status
	int currentStage;
	double currentMass;

	// flight characteristics
	glm::dvec2 position;
	glm::dvec2 velocity;
	glm::dvec2 orientation;
	double throttle;

	// fuel expenditure tracking
	double deltaVSpent;

	// path-finding info
	double score;
	double bestForgotten;
	SimFrame* prev;
	int _refCount;

	// derived values
	double deltaVVac() const;
	double deltaVAtmo() const;
	glm::dvec2 orbit() const;
};

#endif // SIMFRAME_H
