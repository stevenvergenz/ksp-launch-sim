#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QRunnable>
#include <cmath>
#include <cstdio>
#include "glm/glm.hpp"
#include "glm/vec2.hpp"

#include "configs.h"
#include "simframe.h"


class Simulator : public QObject, public QRunnable
{
	Q_OBJECT

public:
	Simulator(SimulationConfig* config);
	~Simulator();

	void run();
	const SimFrame* getLastFlightData();

signals:
	void start(const SimFrame* initFrame);
	void update(const SimFrame* updateFrame);
	void done();

public slots:

private:
	SimulationConfig* config;
	SimFrame* lastFlight;
};

#endif // SIMULATOR_H
