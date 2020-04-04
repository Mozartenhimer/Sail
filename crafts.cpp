#include "pch.h"7
#include "crafts.h"
#include <string>
#include <tuple>
#include <vector>
#include "pencil.h"
#include "vectorExtendedPGE.hpp"
using namespace olc;

Ship::Ship() {
	init();
};

void Ship::init() {
	body = ConstructBody();
	sail = ConstructSail();
	sailPivot = { 0.5f,0.0f};
	rudder = ConstructRudder();
	rudderPivot = { -1.0f ,0.0f};
	keelFoil.area = 0.1f;
	keelFoil.fluidDensity = 1000.0f;

};



Ship::Ship(olc::vf2d position, olc::vf2d velocity) {
	init();
	body.pos = position;
	body.vel = velocity;
};

void Ship::updateState(float timeStep) {
	olc::Mat2d body2world(body.rot);

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
	Pencil::DrawDebugLine("APwind:" + std::to_string(appearantWind.x) + " Y:" + std::to_string(appearantWind.y));
	Pencil::DrawDebugLine("Wind:" + std::to_string(wind.x) + " Y:" + std::to_string(wind.y));

	float windSpeed = appearantWind.mag();
	float windAngle = angle(appearantWind);
		
	sailAngleFromCenterline = windAngle-getHeading();	

	// clamp sail to controls
	if (sailAngleFromCenterline > sailSlackAngle){
		sailAngleFromCenterline = sailSlackAngle;
	}
	if (sailAngleFromCenterline < -sailSlackAngle) {
		sailAngleFromCenterline = -sailSlackAngle;
	}
	float sailAngleWorld = getHeading() + sailAngleFromCenterline;
	float sailAOA = sailAngleWorld - windAngle;
	
	
	olc::vf2d normalSailForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(.0f, 1.0f)*sailFoil.normalForce(windSpeed, sailAOA);
	//Pencil::AddArrow(body.pos, normalSailForce);
	olc::vf2d axialSailForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(1.0f,0.0f)*sailFoil.axialForce(windSpeed, sailAOA);
	// Jank sign correctness
	// Flip sign if drag opposes appearent wind
	if (axialSailForce.dot(appearantWind) > 0) {
		axialSailForce *= -1.0f;
	};
	//Pencil::AddArrow(body.pos, axialSailForce);
	//Pencil::AddArrow(body.pos, -appearantWind, 0.5f, olc::Pixel(255, 0, 0));
	body.applyForce_w(normalSailForce);
	body.applyForce_w(axialSailForce);

	// Compute keel forces
	current += body.vel;
	float keelAOA = getHeading() - angle(current) ;
	olc::vf2d normalKeelForce = Mat2d(getHeading())*olc::vf2d(0.0f, 1.0f)*keelFoil.normalForce(current.mag(), keelAOA);
	body.applyForce_w(normalKeelForce);
	Pencil::AddArrow(body.pos, normalKeelForce, 1.0f, olc::Pixel(200, 200, 200));
	Pencil::DrawDebugLine("keelAOA:" + std::to_string(keelAOA));
	/*
	if (body.vel.mag() < 10.0f) {
		body.applyForce_w(olc::vr2d(1.0f, 0.0f));
	}
	*/





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
