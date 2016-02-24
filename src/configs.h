#ifndef CONFIGS
#define CONFIGS

#include "vec2.h"

struct SimulationConfig
{
	struct ShipInfo
	{
		double mass; // kg
		double maxThrust; // kN
	} ship;

	struct BodyInfo
	{
		double surfaceGravity; // m/s2
		double radius; // m
		double revolutionSpeed; // m/s at surface
	} body;

	struct GoalInfo
	{
		double desiredApoapsis; // m from surface
		double desiredPeriapsis; // m from surface
	} goal;

	struct SimInfo
	{
		double duration; // s
		double timeResolution; // s
	} params;

	SimulationConfig()
	{
		ship.mass = 1;

		body.surfaceGravity = 9.8;
		body.radius = 100;
		body.revolutionSpeed = 10;

		goal.desiredApoapsis = 100;
		goal.desiredPeriapsis = 100;

		params.duration = 100;
		params.timeResolution = 0.1;
	}

	SimulationConfig(ShipInfo ship, BodyInfo body, GoalInfo goal, SimInfo params)
		: ship(ship), body(body), goal(goal), params(params)
	{

	}
};

struct SimFrame
{
	SimulationConfig* config;
	double time;

	Vec2 position;
	Vec2 velocity;
	Vec2 orientation;

	SimFrame* next;
	SimFrame* prev;
};

namespace KerbolSystem
{
	extern SimulationConfig::BodyInfo Kerbin;
}

#endif // CONFIGS

