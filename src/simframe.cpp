#include "simframe.h"

SimFrame::SimFrame() : config(nullptr), prev(nullptr)
{

}

double SimFrame::deltaV() const
{
	double pressure = config->body.surfacePressure * pow(E, -(glm::length(position)-config->body.radius)/config->body.scaleHeight);
	if(pressure <= 0.000001*config->body.surfacePressure)
		pressure = 0;
	else if(pressure > config->body.surfacePressure)
		pressure = config->body.surfacePressure;

	double atmoIsp = (pressure/config->body.surfacePressure) * config->stages[currentStage].IspAtmo
		+ (1 - pressure/config->body.surfacePressure) * config->stages[currentStage].IspVac;

	return atmoIsp * 9.82 * log(currentMass / config->stages[currentStage].dryMass);
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
