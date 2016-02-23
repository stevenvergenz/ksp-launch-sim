#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QRunnable>
#include <cmath>

#include "configs.h"
#include "vec2.h"

class Simulator : public QObject, public QRunnable
{
	Q_OBJECT

public:
	Simulator(SimulationConfig config);

	void run();

signals:
	void start(const QString& msg);
	void update(const QString& msg);
	void done(const QString& msg);

public slots:

private:
	SimulationConfig config;
};

#endif // SIMULATOR_H
