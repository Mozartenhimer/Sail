#include "pch.h"
#include "crafts.h"
#include <string>
#include <tuple>
#include <vector>
#include "pencil.h"
using namespace olc;

void Ship::init() {
	body = ConstructBody();
	void updateBodyInWorld();
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
	
	Line tempLeg = { { 0.8f,-0.5f },{ 1.3f,-2.0f } };
	B.addLine(tempLeg);
	tempLeg = { { -0.8f,-0.5f },{ -1.3f,-2.0f } };
	B.addLine(tempLeg);
	
	Circle Capsule;
	Capsule.radius = 1.0f;
	B.addCircle(Capsule);

	B.inertiaMoment = INFINITY;
	return B;
}

