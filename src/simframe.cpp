#include "simframe.h"

SimFrame::SimFrame() : config(nullptr), deltaVSpent(0), score(INFINITY), bestForgotten(INFINITY), prev(nullptr), _refCount(0)
{

}

double SimFrame::deltaVVac() const
{
	double deltaV = 0;
	for(int i=currentStage; i<config->stageCount; i++)
	{
		double atmoIsp = config->stages[i].IspVac;
		double mass = i == currentStage ? currentMass : config->stages[i].totalMass;
		deltaV += atmoIsp * 9.82 * log(mass / config->stages[i].dryMass);
	}
	return deltaV;
}

double SimFrame::deltaVAtmo() const
{
	double deltaV = 0;
	for(int i=currentStage; i<config->stageCount; i++)
	{
		double atmoIsp = config->stages[i].IspAtmo;
		double mass = i == currentStage ? currentMass : config->stages[i].totalMass;
		deltaV += atmoIsp * 9.82 * log(mass / config->stages[i].dryMass);
	}
	return deltaV;
}

glm::dvec2 SimFrame::orbit() const
{
	double h = glm::length( glm::cross(glm::dvec3(position,0.0), glm::dvec3(velocity,0.0)) ); // specific angular momentum
	double mu = G * config->body.mass;
	double param = pow(h, 2) / mu;
	double energy = pow(glm::length(velocity),2) / 2 - mu/glm::length(position);
	double eccentricity = sqrt( 1 + 2*energy*pow(h,2) / pow(mu,2) );

	// {x: apoapsis, y: periapsis}
	return glm::dvec2(param / (1-eccentricity), param / (1+eccentricity));
}

void SimFrame::freeLeaves(SimFrame* node)
{
	if(node != nullptr && node->_refCount == 0){
		SimFrame* prev = node->prev;
		delete node;
		if(prev != nullptr)
			prev->_refCount--;
		SimFrame::freeLeaves(prev);
	}
}
