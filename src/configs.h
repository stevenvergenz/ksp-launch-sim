
#ifndef CONSTS
#define CONSTS
#define PI 3.141592653589
#define E 2.71828183
#define R_CONST 287.053
#define G 6.67408e-11
#endif

#ifndef CONFIGS
#define CONFIGS

struct SimulationConfig
{
	struct StageInfo
	{
		double dryMass; // kg
		double totalMass; // kg
		double maxThrustAtmo; // kN
		double maxThrustVac; // kN
		double IspAtmo; // s
		double IspVac; // s

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
		int searchDepth;
		double radialStep; // radians
		double throttleStep; // (0,1)
	} params;

};


namespace KerbolSystem
{
	extern SimulationConfig::BodyInfo Kerbin;
}

#endif // CONFIGS

