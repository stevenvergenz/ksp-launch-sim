#ifndef SIMFRAME_H
#define SIMFRAME_H

#include <cmath>
#include <cstring>

#include "configs.h"
#include "glm/glm.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"


class SimFrame
{
public:
	SimFrame();
	SimFrame* clone();

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

	// derived values
	double deltaVVac() const;
	double deltaVAtmo() const;
	glm::dvec2 orbit() const;

	// path-finding info
	double score;
	SimFrame* prev;

	// child management
	double bestForgotten;
	SimFrame** next;
	int nextCount;
	int nextConsideredIndex;
	unsigned char nextBuffered[128];
};

#endif // SIMFRAME_H
