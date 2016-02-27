#ifndef CONFIGS
#define CONFIGS


struct SimulationConfig
{
	struct StageInfo
	{
		StageInfo()
			: dryMass(0), totalMass(0), maxThrust(0), Isp(0)
		{}
		StageInfo(double dry, double total, double thrust, double Isp, double area, double drag)
			: dryMass(dry), totalMass(total), maxThrust(thrust), Isp(Isp), crossSectionalArea(area), dragCoefficient(drag)
		{}

		double dryMass; // kg
		double totalMass; // kg
		double maxThrust; // kN
		double Isp; // s

		double crossSectionalArea; // m2
		double dragCoefficient; // dimensionless
	};
	StageInfo stages[10];
	int stageCount;

	struct BodyInfo
	{
		double mass; // kg
		double radius; // m
		double rotationalPeriod; // s

		double surfacePressure; // Pa
		double scaleHeight; // m
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

