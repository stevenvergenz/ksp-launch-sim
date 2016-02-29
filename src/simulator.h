#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QRunnable>
#include <cmath>
#include <cstdio>

#define PI 3.141592653589
#define E 2.71828183
#define R_CONST 287.053

#include "glm/glm.hpp"
#include "glm/vec2.hpp"

#include "configs.h"
#include "simframe.h"
#include "pilot.h"


class Simulator : public QObject, public QRunnable
{
	Q_OBJECT

public:
	Simulator(SimulationConfig* config);
	~Simulator();

	void run();

	static SimFrame* computeNextFrame(SimFrame* prevFrame, glm::dvec2 orientation = glm::dvec2(), double throttle = 1.0);

signals:
	void start(const SimFrame* initFrame);
	void update(const SimFrame* updateFrame);
	void done();

public slots:

private:
	SimulationConfig* config;
	SimFrame* lastFlight;

	static double tempFromHeight(double height);
};

#endif // SIMULATOR_H
