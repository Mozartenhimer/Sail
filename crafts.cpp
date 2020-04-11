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
	rudderFoil.area = 0.03f;
	rudderFoil.fluidDensity = 1000.0f;
	
	keelFoil.area = 0.07f;
	keelFoil.fluidDensity = 1000.0f;
	
	body.mass = 10.0f;

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

inline void magCheck(olc::vf2d V){
	if (V.mag() > 50) {
		dbg(V);
		__debugbreak;
	}
}

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



	sailAOA = wrapAngle(sailAOA);
	Pencil::DrawDebugLine("sailAOA" + std::to_string(sailAOA));

	olc::vf2d normalSailForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(.0f, 1.0f)*sailFoil.normalForce(windSpeed, sailAOA);
	Pencil::DrawDebugLine("SailNormalForce" + std::to_string(normalSailForce.mag()));
	
	//Pencil::AddArrow(body.pos, normalSailForce);
	olc::vf2d axialSailForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(1.0f,0.0f)*sailFoil.axialForce(windSpeed, sailAOA);
	


	// Jank sign correctness
	// Flip sign if drag opposes appearent wind
	if (axialSailForce.dot(appearantWind) > 0) {
		axialSailForce *= -1.0f;
	};
	Pencil::AddArrow(body.pos + Mat2d(getHeading())*sailPivot, axialSailForce);
	Pencil::AddArrow(body.pos + Mat2d(getHeading())*sailPivot, normalSailForce);
	Pencil::AddArrow(body.pos + Mat2d(getHeading())*sailPivot, -appearantWind, 0.1f, olc::Pixel(255, 0, 0));
	body.applyForce_w(normalSailForce);
	body.applyForce_w(axialSailForce);

	// Compute keel forces
	current += body.vel;
	float keelAOA = wrapAngle(getHeading() - angle(current));



	olc::vf2d normalKeelForce = Mat2d(getHeading())*olc::vf2d(0.0f, 1.0f)*keelFoil.normalForce(current.mag(), keelAOA);
	olc::vf2d axialKeelForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(1.0f, 0.0f)*sailFoil.axialForce(windSpeed, sailAOA);
	float NormDot = normalKeelForce.dot(current);
	
	body.applyForce_w(normalKeelForce);
	//body.applyForce_w(axialKeelForce);
	float AxDot = axialKeelForce.dot(current);
	
	if(NormDot)
	Pencil::AddArrow(body.pos, normalKeelForce, 1.0f, olc::Pixel(200, 200, 200));
	//Pencil::DrawDebugLine("keelAOA:" + std::to_string(keelAOA));

	Pencil::log(sailAOA);
	Pencil::log(keelAOA);
	Pencil::log(normalSailForce.mag());
	Pencil::log(axialSailForce.mag());
	Pencil::log(normalKeelForce.mag());
	Pencil::log(axialKeelForce.mag());
	Pencil::log(body.vel.x);
	Pencil::log(body.vel.y);
	Pencil::log(AxDot);
	Pencil::log(NormDot);

	/*
	if (body.vel.mag() < 10.0f) {
		body.applyForce_w(olc::vr2d(1.0f, 0.0f));
	}
	*/
	// Rudder
	olc::vf2d RudderAdditionalVel = Mat2d((double)getHeading())*(-rudderPivot.cross(body.rotDot));
	
	olc::vf2d rudderCurrent = current + RudderAdditionalVel;
	
	Pencil::AddArrow(body.pos + Mat2d(getHeading())*rudderPivot, RudderAdditionalVel,1.0f,olc::Pixel(100,100,100));
	;

	// This does not take into account added rudder current due to 
	float rudderAngle_w = getHeading() + rudderAngle;
	float rudderAOA = wrapAngle(rudderAngle_w - angle(rudderCurrent));
	Pencil::DrawDebugLine("Rudder AOA:" + std::to_string(rudderAOA));
	olc::vf2d normalRudderForce = Mat2d(rudderAngle_w)*olc::vf2d(.0f, 1.0f)*rudderFoil.normalForce(rudderCurrent.mag(), rudderAOA);
	float momentComponent = (Mat2d(getHeading())*olc::vf2d(0.0, 1.0f)).dot(normalRudderForce);
	Pencil::DrawDebugLine("Rudder Moment Component:" + std::to_string(momentComponent));

	float rudderMoment = -momentComponent*rudderPivot.mag();
	
	Pencil::DrawDebugLine("Rudder Moment:" + std::to_string(rudderMoment));
	Pencil::DrawDebugLine("Rudder Normal Force" + std::to_string(normalRudderForce.mag()));
	Pencil::AddArrow(body.pos + Mat2d(getHeading())*rudderPivot, normalRudderForce,0.1f);
	
	//`olc::vf2d axialSailForce = Mat2d(sailAngleFromCenterline + getHeading())*olc::vf2d(1.0f, 0.0f)*sailFoil.axialForce(windSpeed, sailAOA);



	//// Jank sign correctness
	//// Flip sign if drag opposes appearent wind
	
	//Pencil::AddArrow(body.pos, axialSailForce);
	//Pencil::AddArrow(body.pos, normalSailForce);
	//Pencil::AddArrow(body.pos, -appearantWind, 0.1f, olc::Pixel(255, 0, 0));
	//body.applyForce_w(normalSailForce);
	//body.applyForce_w(axialSailForce);
	//body.applyForce_w(normalRudderForce);
	
	body.applyMoment(rudderMoment);
	
	// Apply rotational damping
    double dampFactor = 0.1f;
	Pencil::DrawDebugLine("BodyRotDot" + std::to_string(body.rotDot));
	body.applyMoment(-pow(body.rotDot, 2)*dampFactor);
	

	Pencil::DrawDebugLine("-------------");
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
	float sailLength = 0.8f;
	for ( int i = 0; i <= 10 ; i++){
		float dist = (float)i / 10.0f*sailLength;
		B.points.push_back({ -dist,0.0f });
	}
	return B;

}

void Ship::updateSailShape(double missionElapsedTime) {

	float wavelength = 0.2f;
	float waveSpeed = 5.f;

	for (auto & point : sail.points ) {
		float phase = fmod(missionElapsedTime*waveSpeed, 2 * M_PI);
		point.y = 0.1f*sin(point.x/wavelength+phase)*point.x;
	}
	
}