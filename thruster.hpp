#pragma once
#include "vectorExtendedPGE.hpp"

#include "plume.hpp"
class Thruster
{
public:
	static olc::VxOLCPGE * Renderer;
	float * fuelTank = nullptr;
	float maxThrust = 392.3f*2.0f*10.0f; // 2x weight
	float specificImpulse = 2000.0f;
	olc::vf2d gimbalLocation = {0.0f,-5.0f};
	float rotationAngle = (float)M_PI_2; // Angle from positive X which the thrust value acts.
	float gimbalLimit = (float)M_PI/6;
	
	float gimbalAngle;
	float throttle;
	
	// TODO PLUME
	Plume plume;
public:
	Thruster();
	Thruster(olc::VxOLCPGE * Renderer_);
	~Thruster();
	void setThrottle(float throttle_); // 
	float getThrottle() const;
	void setGimbal(float gimbalAngle_); //Radians
	float getGimbalAngle();
	olc::vf2d getThrust  (); // N
	float     getMoment();// N*m
	float	  getMdot(); // kgs
	void      Draw(olc::vf2d const & hostPosition,olc::Mat2d const & body2world);
	

};

