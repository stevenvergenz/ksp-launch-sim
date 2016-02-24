#include "simulator.h"

Simulator::Simulator(SimulationConfig* config) : config(config), lastFlight(nullptr)
{

}

Simulator::~Simulator()
{
	// delete loose sim memory
	if(lastFlight != nullptr){
		SimFrame* i = lastFlight;
		while(i != nullptr){
			SimFrame* tmp = i->next;
			delete i;
			i = tmp;
		}
	}
}

void Simulator::run()
{
	// clean up last flight
	SimFrame* i = lastFlight;
	while(i != nullptr){
		SimFrame* tmp = i->next;
		delete i;
		i = tmp;
	}

	// set up initial state
	SimFrame* prevFrame = new SimFrame();
	prevFrame->config = config;
	prevFrame->time = 0.0;
	prevFrame->orientation = Vec2(0.0, 1.0);
	prevFrame->position = Vec2(0.0, config->body.radius);
	prevFrame->velocity = Vec2(config->body.revolutionSpeed+100, 100.0);

	prevFrame->prev = nullptr;
	prevFrame->next = nullptr;

	double dt = config->params.timeResolution;

	emit start(prevFrame);

	// start simulation loop
	do
	{
		// initialize new frame
		SimFrame* nextFrame = new SimFrame();
		nextFrame->config = config;
		nextFrame->time = prevFrame->time + dt;
		nextFrame->orientation = prevFrame->orientation;

		prevFrame->next = nextFrame;
		nextFrame->prev = prevFrame;
		nextFrame->next = nullptr;

		// compute gravitational force
		double accel = config->body.surfaceGravity * pow(config->body.radius/prevFrame->position.mag(), 2);
		Vec2 g = accel * -prevFrame->position.normalized();

		// apply forces
		nextFrame->velocity = g*dt + prevFrame->velocity;
		nextFrame->position = 0.5*g*pow(dt,2) + prevFrame->velocity*dt + prevFrame->position;

		// post update
		emit update(nextFrame);

		// new becomes old
		prevFrame = nextFrame;

		// NOTE: Frames are not freed as the sim progresses.
		// Let the event listener handle that
	}
	// while rocket is above the ground and the sim hasn't timed out
	while(prevFrame->position.mag() > config->body.radius && prevFrame->time < config->params.duration);

	emit done();
}

const SimFrame* Simulator::getLastFlightData()
{
	return lastFlight;
}
