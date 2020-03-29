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
	sailPivot = {0.0f,0.5f};
	rudder = ConstructRudder();
	rudderPivot = { 0.0f,-1.0f };
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
	sailAngle = sailMaxAngle;
	for (auto & point : sail_w.points)
	{
		point = body2world * (Mat2d(sailAngle)*(point)+sailPivot) + body.pos;
	}
	Pencil::Draw(sail_w);
};



RigidBody Ship::ConstructBody()
{
	RigidBody B;
	// Front
	Line tempLeft = { { 0.0f,1.0f },{ 0.5f,0.0f } };
	Line tempRight = { { 0.0f,1.0f },{ -0.5f,0.0f } };
	
	B.addLine(tempLeft);
	B.addLine(tempRight);
	// Mid
	tempLeft = {  { 0.5f,0.0f },{ 0.5f,-1.0f } };
	tempRight = { { -0.5f,0.0f },{ -0.5f,-1.0f } };
	B.addLine(tempLeft);
	B.addLine(tempRight);
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
	B.points.push_back({ 0.0f,0.05f }); 
	B.points.push_back({ 0.0f,-0.2f });

	return B;
};

SegmentedCurve Ship::ConstructSail()
{
	SegmentedCurve B;
	B.points.push_back({ 0.0f,0.0f });
	B.points.push_back({ 0.0f,-0.8f });
	return B;

}
