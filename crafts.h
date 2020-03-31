#pragma once
#include "vectorExtendedPGE.hpp"
#include "physics.h"
#include "thruster.hpp"
#include "globals.h"


class Foil {
public:
	Foil() {};
	//! \brief Normal force
	float area = 1.0f;
	float fluidDensity = 1.0f; // kg/m^3
	inline float Cn(float aoa) {
		// Latex:
		// \cos\left(x - p\right) ^ { 2 }\cdot\sin\left(\left(x\right) ^ { E }\right) ^ { 2 }+x\cdot0.4
		bool negate = false;
		if (aoa < 0) {
			negate = true;
			aoa = -aoa;
		}
		const float & x = aoa;
		const float p = -0.3141592653589793f;
		const float E = 0.22f;
		const float draggy = 0.1f;
		const float constant = 1.0f;
		const float cosTerm = pow(cos(x - p), 2);

		const float sinTerm = pow(cos(pow(x, E)), 2);
		if (negate)
			return -(cosTerm*sinTerm + x * draggy)*constant;
		else
			return  (cosTerm*sinTerm + x * draggy)*constant;
	};
	//! \brief Axial force
	inline float Ca(float aoa)
	{
		//Latiex: y\ =C+\cos\left(2x\right)\cdot C
		const float C = 0.09;
		const float & x = aoa;
		return pow(sinf(aoa), 2) + 0.1;
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
		return Cn(aoa)*area*dynPress;


	}

	inline float axialForce(float fluidVel, float aoa) {
		//! \brief returns in frame of foil.
		// 
		return -2.0f;

	}
};



class Ship {
public:
	float sailSlackAngle = 0.0f;;
	float sailLimitAngle = (float)M_PI / 2.0f;
	float sailAngleFromCenterline = 0.0;

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
		float heading = body.rot;
		heading = fmod(heading, M_PI * 2);
		return heading;
	};
	float rudderAngle = 0.0f;
	float maxRudder = (float)M_PI / 2.0f;
	inline void setRudder(float angle) { if (abs(angle) <= maxRudder) rudderAngle = angle; };
	inline float getRudder() { return rudderAngle; };

	RigidBody body;
	olc::vf2d rudderPivot;
	SegmentedCurve rudder;
	SegmentedCurve rudder_w;
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
};
