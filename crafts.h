#pragma once
#include "vectorExtendedPGE.hpp"
#include "physics.h"
#include "thruster.hpp"
#include "globals.h"


inline float wrapAngle(float angle) {
	angle = fmod(angle+M_PI, (float)2*M_PI);
	if (angle < 0)
		angle += 2 * M_PI;
	angle -= M_PI;
	return angle;
}
class Foil {
public:
	Foil() {};
	//! \brief Normal force
	float area = 1.0f;
	float fluidDensity = 1.0f; // kg/m^3
	float linearRegion = (float)1.0*M_PI / 180;
	float CaMult = 0.15f;
	float CnMult = 1.0f;
	float lastN = NAN;
	float lastA = NAN;
	inline float Cn(float aoa) {
		// linear region 


		// Latex:
		// \cos\left(x - p\right) ^ { 2 }\cdot\sin\left(\left(x\right) ^ { E }\right) ^ { 2 }+x\cdot0.4
		
		float sign = 1.0f;
		if (aoa < 0) {
			sign = -1.0f;
			aoa = -aoa;
		}
		if (aoa < linearRegion) {
			return sign*aoa * 8.6;
		}

		const float & x = aoa;
		const float p = -0.3141592653589793f;
		const float E = 0.22f;
		const float draggy = 0.1f;
		const float constant = 1.0f;
		const float cosTerm = pow(cos(x - p), 2);

		const float sinTerm = pow(cos(pow(x, E)), 2);

		return sign*(cosTerm*sinTerm + x * draggy)*constant;
	};
	//! \brief Axial force
	inline float Ca(float aoa)
	{
		return cosf(aoa);
	};
	//inline olc::vf2d force(olc::vf2d fluidVel, float aoa) {
	//	//! \brief returns in frame of foil.
	//	// 
	//	const float dynPress = 0.5*fluidDensity*fluidVel.mag2();
	//	olc::vf2d force = olc::vf2d(-Ca(aoa), Cn(aoa))*area*dynPress;
	//	return force;

	//}
	inline float normalForce(float fluidVel, float aoa) {
		//! \brief returns in frame of foil.
		const float dynPress = 0.5*fluidDensity*pow(fluidVel,2);
		lastN =  Cn(aoa)*area*dynPress*CnMult;
		return lastN;
	}
	
	inline float axialForce(float fluidVel, float aoa) {
		const float dynPress = 0.5*fluidDensity*pow(fluidVel, 2);
		lastA = (Ca(aoa)*area*dynPress*CaMult);
		return lastA;
	}
};



class Ship {
public:
	// All angles from center line
	float sailSlackAngle = 0.0f;
	float sailLimitAngle = (float)M_PI / 2.0f;
	float sailAngleFromCenterline = 0.0;
	float minSailSlackAngle = (float)180 / M_PI;
	float sailAOA;
	olc::vf2d appearantWind;
	inline void setSail(float angle)
	{
		if (angle < sailLimitAngle)
			sailSlackAngle = angle;
		else angle = sailLimitAngle;
		if (angle < 0)
			sailSlackAngle = 0;
	};
	inline float getSailSlackAngle() { return sailSlackAngle; };

public:
	inline float getHeading() {
		return wrapAngle(body.rot);
	};
	float rudderAngle = 0.0f;
	float maxRudder = (float)M_PI / 8.0f;
	inline void setRudder(float angle) { if (abs(angle) <= maxRudder) rudderAngle = angle; };
	inline float getRudder() { return rudderAngle; };

	RigidBody body;
	Foil keelFoil;

	olc::vf2d rudderPivot;
	SegmentedCurve rudder;
	SegmentedCurve rudder_w;
	Foil rudderFoil;

	olc::vf2d sailPivot;
	SegmentedCurve sail;
	SegmentedCurve sail_w;
	Foil sailFoil;
	olc::VxOLCPGE * host;


	Ship();
	void init();
	Ship(olc::vf2d position, olc::vf2d velocity);
	void Draw();

	//! Updates thrust forces, fuel levels, stuff like that.
	void updateState(float timeStep);

	//! Get the current throttle from the main engine

	void applyEnviromentForces(olc::vf2d wind, olc::vf2d current = { 0,0 });

	static RigidBody ConstructBody();
	SegmentedCurve ConstructRudder();
	SegmentedCurve ConstructSail();
	void updateSailShape(double missionElapsedTime);
	static float rudderRateCurve(float speed);
};
