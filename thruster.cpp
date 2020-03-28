#include "pch.h"
#include "thruster.hpp"
using namespace olc;

olc::VxOLCPGE * Thruster::Renderer;
Thruster::Thruster()
{
}
Thruster::Thruster(olc::VxOLCPGE * Renderer_) {
	Renderer = Renderer_;
}

Thruster::~Thruster()
{
}

void Thruster::setThrottle(float throttle_)
{
	if (*fuelTank > 0.0f) {
		throttle = throttle_;
	} else {
		throttle = 0.0f;
		return;
	}

	if (throttle > 1.0f)  throttle = 1.0f;
	else if (throttle <  0.0f) throttle = 0.0f;
}

float Thruster::getThrottle() const
{
	return throttle;
}

void Thruster::setGimbal(float gimbalAngle_)
{
	
	if (gimbalAngle_ > gimbalLimit) gimbalAngle = gimbalLimit;
	else if (gimbalAngle_ < -gimbalLimit) gimbalAngle = -gimbalLimit;
	else gimbalAngle = gimbalAngle_;

}

float Thruster::getGimbalAngle()
{
	return gimbalAngle;
}

olc::vf2d Thruster::getThrust()
{
	olc::Mat2d bodyR(rotationAngle+gimbalAngle);
	float force = throttle * maxThrust;
	olc::vf2d line = { 1.0f, 0.0f };
	return (bodyR * line)*force;
}

float Thruster::getMoment()
{
	olc::Mat2d gimbalR(gimbalAngle+ rotationAngle);
	olc::vf2d F = { 1.0f, 0.0f };
	F = F*(throttle * maxThrust);
	
	return gimbalLocation.cross(gimbalR*F);;
}

float Thruster::getMdot()
{
	return  throttle * maxThrust/ (9.80665f*specificImpulse);
}

void Thruster::Draw(olc::vf2d const & pos, olc::Mat2d const & body2world)
{
	// Draw Plume (for debug / fun)

		float scale = maxThrust / 2000.0f;
		float nozScale = 0.2f;
		float plumeLength = getThrottle()*scale;
		// Rotate the plume about rotation axis
		Mat2d nozzle2body(getGimbalAngle() + rotationAngle - (float)M_PI/2.0f);


	if (getThrottle() > 0) {
		// plume drawn from gimbal
		vf2d plumeEnd_noz = { 0.2f*scale*getThrottle(),-plumeLength };
		vf2d plumeEnd_noz_m = MirrorX(plumeEnd_noz);
		// Translate plume coordinates to body start
		vf2d plumeEnd_body = nozzle2body * plumeEnd_noz + gimbalLocation;
		vf2d plumeEnd_body_m = nozzle2body * plumeEnd_noz_m + gimbalLocation;
		// Rotate plume coordinates with body2world
		vi2d plumeStart_screen = Renderer->toScreen(body2world*gimbalLocation + pos);
		vi2d plumeEnd_screen = Renderer->toScreen(body2world*plumeEnd_body + pos);
		vi2d plumeEnd_screen_m = Renderer->toScreen(body2world*plumeEnd_body_m + pos);
		Renderer->DrawTriangle(plumeStart_screen, plumeEnd_screen_m, plumeEnd_screen, RED);
	}

	// Draw Thruster itself
	vf2d nozEnd_noz = { 0.3f*scale*nozScale,-scale * nozScale };
	vf2d nozEnd_noz_m = MirrorX(nozEnd_noz);
	// Nozzle in body space
	vf2d nozEnd_body = nozzle2body * nozEnd_noz + gimbalLocation;
	vf2d nozEnd_body_m = nozzle2body * nozEnd_noz_m + gimbalLocation;

	vi2d nozStart_screen = Renderer->toScreen(body2world*gimbalLocation + pos);
	vi2d nozEnd_screen = Renderer->toScreen(body2world*nozEnd_body + pos);
	vi2d nozEnd_screen_m = Renderer->toScreen(body2world*nozEnd_body_m + pos);

	//Renderer->DrawTriangle(nozStart_screen, nozEnd_screen, nozEnd_screen_m, WHITE);
	Renderer->DrawLine(nozStart_screen, nozEnd_screen, Pixel(200,200,200));
	Renderer->DrawLine(nozStart_screen, nozEnd_screen_m, Pixel(200, 200, 200));
}
