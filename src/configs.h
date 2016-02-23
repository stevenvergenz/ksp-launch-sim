#ifndef CONFIGS
#define CONFIGS

struct SimulationConfig
{
	struct ShipInfo
	{
		double mass; // kg
	} ship;

	struct BodyInfo
	{
		double mass; // kg
		double radius; // m
		double revolutionSpeed; // m/s at surface
	} body;

	struct GoalInfo
	{
		double desiredApoapsis; // m from surface
		double desiredPeriapsis; // m from surface
	} goal;

	int duration; // s

	SimulationConfig()
	{
		ship.mass = 1;

		body.mass = 100;
		body.radius = 100;
		body.revolutionSpeed = 10;

		goal.desiredApoapsis = 100;
		goal.desiredPeriapsis = 100;

		duration = 10;
	}

	SimulationConfig(ShipInfo ship, BodyInfo body, GoalInfo goal)
		: ship(ship), body(body), goal(goal)
	{

	}
};

namespace KerbolSystem
{
	static SimulationConfig::BodyInfo Kerbin = {5.2915793e22, 600000, 174.94};
}

#endif // CONFIGS

