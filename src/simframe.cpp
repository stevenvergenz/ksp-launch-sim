#include "simframe.h"

SimFrame::SimFrame() : config(nullptr), next(nullptr), prev(nullptr)
{

}

double SimFrame::deltaV() const
{
	return config->stages[currentStage].Isp * 9.82
		* log(currentMass / config->stages[currentStage].dryMass);
}

glm::dvec2 SimFrame::orbit() const
{
	double angularMomentum = glm::length( glm::cross(glm::dvec3(position,0.0), glm::dvec3(velocity,0.0)) );
	double mu = G * config->body.mass;
	double param = pow(angularMomentum, 2) / mu;
	double energy = pow(glm::length(velocity),2) / 2 - mu/glm::length(position);
	double eccentricity = sqrt( 1 + 2*energy*pow(angularMomentum,2) / pow(mu,2) );
	return glm::dvec2(param / (1-eccentricity), param / (1+eccentricity));
}
