#include "simframe.h"

SimFrame::SimFrame() : config(nullptr), next(nullptr), prev(nullptr)
{

}

double SimFrame::deltaV()
{
	return config->stages[currentStage].Isp * 9.82
		* log(currentMass / config->stages[currentStage].dryMass);
}

double SimFrame::apoapsis()
{
	//double angularMomentum =
	return 0.0;
}

double SimFrame::periapsis()
{
	return 0.0;
}
