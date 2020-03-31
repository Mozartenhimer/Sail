#include "pch.h"
#include "crafts.h"
#include <string>
#include <tuple>
#include <vector>
#include "pencil.h"
#include "vectorExtendedPGE.hpp"
using namespace olc;

void Ship::init() {
	body = ConstructBody();
	sail = ConstructSail();
	sailPivot = { 0.5f,0.0f};
	rudder = ConstructRudder();
	rudderPivot = { -1.0f ,0.0f};
};

Ship::Ship() {
	init();
};

Ship::Ship(olc::vf2d position, olc::vf2d velocity) {
	init();
	body.pos = position;
	body.vel = velocity;
};

void Ship::updateState(float timeStep) {
	olc::Mat2d body2world(body.rot);
	//APPLY SAIL FORCE HEERE
	// body.applyForce_b(getCurrentThrustForce());
	
}


void Ship::Draw() {
	Pencil::Draw(body);
	
	olc::Mat2d body2world(body.rot);
	rudder_w = rudder;
	for (auto & point : rudder_w.points) 
	{
		point = body2world*(Mat2d(rudderAngle)*(point)+rudderPivot) + body.pos;
	}
	Pencil::Draw(rudder_w);
	sail_w = sail;
	for (auto & point : sail_w.points)
	{
		point = body2world * (Mat2d(sailAngleFromCenterline)*(point)+sailPivot) + body.pos;
	}
	Pencil::Draw(sail_w);
};


void Ship::applyEnviromentForces(olc::vf2d wind,olc::vf2d current) {
	olc::vf2d appearantWind = body.vel - wind;
	float windSpeed = appearantWind.mag();
	// WRAP THIS STUFF UP INTO A FOIL class
	float windAngle = angle(appearantWind);
	
	sailAngleFromCenterline = windAngle-getHeading();	



	// clamp sail to controls
	if (sailAngleFromCenterline > sailSlackAngle)
	{
		sailAngleFromCenterline = sailSlackAngle;
	}
	if (sailAngleFromCenterline < -sailSlackAngle) {
		sailAngleFromCenterline = -sailSlackAngle;
	}


	float sailAngleWorld = getHeading() + sailAngleFromCenterline;
	float aoa = sailAngleWorld - windAngle;
	dbg(appearantWind); dbg(windSpeed);
	
	olc::vf2d normalSailForce = 
		Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(.0f, 1.0f)*sailFoil.normalForce(windSpeed, aoa);
	Pencil::AddArrow(body.pos, normalSailForce);
	olc::vf2d axialSailForce =
		Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(1.0f,0.0f)*sailFoil.axialForce(windSpeed, aoa);
	Pencil::AddArrow(body.pos, axialSailForce);
	body.applyForce_w(normalSailForce);

}

RigidBody Ship::ConstructBody()
{
	RigidBody B;
	// Front
	Line tempUp = { { 1.0f,0.0f },{ 0.0f,0.5f } };
	Line tempLow = { { 1.0f ,0.0f},{ 0.0f,-0.5f } };
	
	B.addLine(tempUp);
	B.addLine(tempLow);
	// Mid
	tempUp = { { 0.0f,0.5f },{ -1.0f ,0.5f } };
	tempLow = { { 0.0f,-0.5f },{ -1.0f,-0.5f } };
	B.addLine(tempUp);
	B.addLine(tempLow);
	//Circle Capsule;
	//Capsule.radius = 1.0f;
	//B.addCircle(Capsule);

	B.inertiaMoment = INFINITY;
	return B;
}

SegmentedCurve Ship::ConstructRudder()
{
	SegmentedCurve B;
	// In relative to rudder pivot points.
	B.points.push_back({ 0.05f ,0.0f});
	B.points.push_back({ -0.2f, 0.0f});

	return B;
};

SegmentedCurve Ship::ConstructSail()
{
	SegmentedCurve B;
	B.points.push_back({ 0.0f,0.0f });
	B.points.push_back({ -0.8f, 0.0f});
	return B;

}
