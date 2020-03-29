#include "pch.h"
#include "crafts.h"
#include <string>
#include <tuple>
#include <vector>
#include "pencil.h"
using namespace olc;

void Ship::init() {
	body = ConstructBody();
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

	// Circle with two lines
	// Rotate everythin in body coords
	Pencil::Draw(body);
	
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

