#ifndef CONFIGS
#define CONFIGS


struct SimulationConfig
{
	struct StageInfo
	{
		StageInfo()
			: dryMass(0), totalMass(0), maxThrust(0), Isp(0)
		{}
		StageInfo(double dry, double total, double thrust, double Isp)
			: dryMass(dry), totalMass(total), maxThrust(thrust), Isp(Isp)
		{}

		double dryMass; // kg
		double totalMass; // kg
		double maxThrust; // kN
		double Isp; // s
	};
	StageInfo stages[10];
	int stageCount;

	struct BodyInfo
	{
		double mass; // kg
		double radius; // m
		double rotationalPeriod; // s
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
};


namespace KerbolSystem
{
	extern SimulationConfig::BodyInfo Kerbin;
}

#endif // CONFIGS

