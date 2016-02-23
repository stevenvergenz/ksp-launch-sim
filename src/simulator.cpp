#include "simulator.h"

Simulator::Simulator(SimulationConfig config) : config(config)
{

}

void Simulator::run()
{
	SimFrame* prevFrame = new SimFrame();
	prevFrame->time = 0.0;
	prevFrame->orientation = Vec2(0.0, 1.0);
	prevFrame->position = Vec2(0.0, config.body.radius+1.0);
	prevFrame->velocity = Vec2(config.body.revolutionSpeed, 0.0);

	SimFrame* nextFrame = new SimFrame();

	double dt = config.params.timeResolution;

	emit start(QString("[T+%1s] %2m @ %3m/s")
		.arg(prevFrame->time, 0, 'f', 2)
		.arg(prevFrame->position.mag()-config.body.radius)
		.arg(prevFrame->velocity.mag())
	);

	do
	{
		// initialize frame
		nextFrame->time = prevFrame->time + dt;
		nextFrame->orientation = prevFrame->orientation;

		// compute gravitational force
		double accel = config.body.surfaceGravity * pow(config.body.radius/prevFrame->position.mag(), 2);
		Vec2 g = accel * -prevFrame->position.normalized();

		// apply forces
		nextFrame->velocity = g*dt + prevFrame->velocity;
		nextFrame->position = 0.5*g*pow(dt,2) + prevFrame->velocity*dt + prevFrame->position;

		// post update
		QString updateMsg = QString("[T+%1s] %2m @ %3m/s")
			.arg(nextFrame->time, 0, 'f', 2)
			.arg(nextFrame->position.mag()-config.body.radius)
			.arg(nextFrame->velocity.mag());
		emit update(updateMsg);

		// swap frames
		SimFrame* temp = prevFrame;
		prevFrame = nextFrame;
		nextFrame = temp;
	}
	while(prevFrame->position.mag() > config.body.radius && prevFrame->time < config.params.duration);

	emit done("Finished sim");

	// clean up memory
	delete prevFrame;
	delete nextFrame;
}
