#pragma once
#include "vectorExtendedPGE.hpp"
#include "physics.h"
#include "thruster.hpp"
#include "globals.h"
class Ship {
public:
	float sailSlackAngle = 0.0f;
	float sailMaxAngle = (float)M_PI / 2.0f;
	float sailAngle = 0.0;
	
	inline void setSail(float angle) { if (abs(angle) <= sailMaxAngle) sailSlackAngle = angle; };
	inline float getSailSlackAngle() { return sailSlackAngle; };
	
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
	olc::VxOLCPGE * host;

	Ship();
	void init();
	Ship(olc::vf2d position, olc::vf2d velocity);
	void Draw();

	//! Updates thrust forces, fuel levels, stuff like that.
	void updateState(float timeStep);
	
	//! Get the current throttle from the main engine
	


	static RigidBody ConstructBody();
	SegmentedCurve ConstructRudder();
	SegmentedCurve ConstructSail();
};