#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QRunnable>
#include <cmath>
#include <cstdio>
#include <cassert>

#include "glm/glm.hpp"
#include "glm/vec2.hpp"

#include "configs.h"
#include "simframe.h"
#include "priorityqueue.h"


class Simulator : public QObject, public QRunnable
{
	Q_OBJECT

public:
	Simulator(SimulationConfig* config);
	~Simulator();

	bool abort;
	void run();

signals:
	void update(const SimFrame* updateFrame);
	void done(const SimFrame* bestResult);

private:
	SimulationConfig* config;

	static SimFrame* computeNextFrame(SimFrame* prevFrame, glm::dvec2 orientation = glm::dvec2(), double throttle = 1.0);
	static double evaluateFrame(SimFrame* frame);
	static double tempFromHeight(double height);
};

#endif // SIMULATOR_H
