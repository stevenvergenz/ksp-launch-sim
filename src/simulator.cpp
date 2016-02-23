#include "simulator.h"

Simulator::Simulator(SimulationConfig config) : config(config)
{

}

void Simulator::run()
{
	emit start("Starting sim");
	emit update("Updating sim");
	emit done("Finished sim");
}
