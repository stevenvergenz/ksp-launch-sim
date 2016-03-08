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
	double deltaVVac() const;
	double deltaVAtmo() const;
	glm::dvec2 orbit() const;

	SimulationConfig* config;
	double time;

	int currentStage;
	double currentMass;
	glm::dvec2 position;
	glm::dvec2 velocity;

	glm::dvec2 orientation;
	double throttle;

	double deltaVSpent;

	double score;
	double bestForgotten;
	SimFrame* prev;
	int _refCount;

	static void freeLeaves(SimFrame* frame);
};

#endif // SIMFRAME_H
