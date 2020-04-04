#include "pch.h"
#include "globals.h"
#include "hud.h"
#include "Geometry.h"
#include "plume.hpp"
#include "thruster.hpp"
#include "crafts.h"
#include "physics.h"
#include "pencil.h"
/*
 TODO:
	Make wind persisent state of ship object.
 Wind field shown in background
Sail, rudder and keel forces
Sail & rudder rendering
Sound effects
	- Wind
	-Water
	-
Demonstrate physics working
Game-if-fication
	-Race?
	-Item Collection?
	-Chill?

Wake effects
Let peole know
	_ Friends
	_- Friends/Family with sailboats.
*/


using namespace olc;
olc::VxOLCPGE * AsteroidFont::host;
bool debugOverlay = true;
// Rand stuff is stolen from OLC Proc gen video
static uint32_t nProcGen = 0; // Seed 
uint32_t rnd()
{
	nProcGen += 0xe120fc15;
	uint64_t tmp;
	tmp = (uint64_t)nProcGen * 0x4a39b70d;
	uint32_t m1 = (uint32_t)((tmp >> 32) ^ tmp);
	tmp = (uint64_t)m1 * 0x12fad5c9;
	uint32_t m2 = (uint32_t)((tmp >> 32) ^ tmp);
	return m2;
}

double rndDouble(double min, double max)
{
	return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
}

float rndFloat(float min, float max) // This is mine, but the rndDouble is Javid's....
{
	return ((float)rnd() / (float)(0x7FFFFFFF)) * (max - min) + min;
}

int rndInt(int min, int max)
{
	return (rnd() % (max - min)) + min;
}
// end stolen Javid code.
olc::vf2d wind(olc::vf2d position) // Time implied
{
	// Return the velocity vector of the wind.
	return { -3.0f,0.0f };
}
struct Camera {
	// It's actually a orhogonal 2d sping mass damper
	float omega_nom = (float)M_PI*20.0f;
	float zeta_nom = 1.0f;
	olc::vf2d anchorPos; // In world space
	olc::vf2d anchorVel; // In world space
	olc::vf2d pos; // In world space
	olc::vf2d vel; // In wolrd Space
	inline void updateState(olc::vf2d const & anchorPos_, olc::vf2d const & anchorVel_, float timeStep) {
		anchorPos = anchorPos_;
		anchorVel = anchorVel_;
		olc::vf2d deltaVel = vel - anchorVel;
		float zeta;
		float omega;
		if (deltaVel.mag2() > pow(15.0f, 2)) { // soft/hard landing camera shake
			zeta = 0.05f;
			omega = omega_nom;
			//dbg(deltaVel.mag());
		}
		else {
			zeta = zeta_nom;
			omega = omega_nom;
		}
		olc::vf2d delta = pos - anchorPos;
		olc::vf2d acc = -1.0f*(2.0f*zeta*omega* deltaVel + omega * omega*delta);
		vel += acc * timeStep;
		pos += vel * timeStep;
	};
};	

class Lando : public VxOLCPGE
{
public:
	HUD hud;
	RigidBody testObj;
	Pricipia physicsEngine; 

	Enviroment env;
	Circle test;
	Ship ship;
	Line flowTestLine;
	//Line shock;
	float nominalScreenHeight;
	Real shockAngle;
	Line mouseLine;
	Plume plume;
	Camera cam; 
	
	float frameMaxDt; //!< The maximum simulation time that can go by between frames. Will cause a slow down if 1/fElapsedTime less  than this
	float timeMultiplier =1.0f; //!< Set to one to no effect. Less than one causes a slow down
	float frameDelay = 0.0f; //!< Delay between game time updates. Useful for debugging.
	
private:
	double previousFrameTime = -1.0; //!< Previous realTime that the frame was rendered. See float frameDelay.
	double realTime = 0.0; //!< Real Time Clock;
	double missionElapsedTime = 0.0; //!< Game time
	bool simRunning = true; //!< Basically game pause.
	float throttleRate = 1.0f / 5.0f; // 1 over seconds from 0 to full throttle;

public:
	Lando() : hud(this)
	{
		Pencil::host = this;
		ship.host = this;
		sAppName = "Lando";
		
	}
	//! Reset game to as if the executable just started
	void ResetGameState() {
		missionElapsedTime = 0;

		ship.body.pos = olc::vf2d(0.0f, 0.0f);
		ship.body.vel = olc::vf2d(0.0f, 0.0f);
		ship.body.rot = 0.0f;
		ship.body.rotDot = 0.0f;
		cam.anchorPos = cam.pos = ship.body.pos;
		cam.anchorVel = cam.vel = ship.body.vel;
		cameraPos = cam.pos;
		// Testing
		ship.body.rot    = 0.0f;
		ship.body.rotDot = 0.0f;
		ship.body.inertiaMoment = INFINITY;
		//! Updates the positions of the bodies.
		physicsEngine.PropgateState(0.0f);
	};
public:

	void DrawBackground() {
		Pixel backgroundColor(10, 10, 50);
		Clear(backgroundColor);
		// TODO improve performance at large areas.
		// Proceduredurally generate stars.
		// Divide the screen up 
		constexpr float blockSize= 9.0f;
		vf2d TL = screenTopLeft();
		float X = floor(TL.x/blockSize)*blockSize- blockSize;
		for (int i = 0; X < TL.x + screenDims.x + blockSize; i++) {
			float Y = ceil(TL.y / blockSize)*blockSize +blockSize;
			for (int j = 0; Y > TL.y - screenDims.y - blockSize; ) {
				uint32_t blockID = (uint16_t)(X / blockSize) << 16 | (uint16_t)(Y / blockSize);
				nProcGen = blockID;
				int nStars = rndInt(1, 15);
				for (int k = 0; k < nStars; k++) {
					//int brightness = rndInt(50,200);
					float intensity = rndFloat(0.001f, 2.0f);
					// modulate brightness based on scale.
					int brightness = (int)fmin(255.0f,intensity * pow(getPixelsPerMeter(), 1.5f));
					float x = rndFloat(0.0f, blockSize);
					float y = rndFloat(0.0f, blockSize);
					olc::vf2d starPos( X + x,Y - y );
					Pixel star = Pixel(brightness, brightness, brightness, 255);
					Pixel rendered = clampedPixel(backgroundColor,star);
					Draw(toScreen(starPos), rendered);
				}
				//FillCircle(toScreen({ X,Y }), 10,RED);
				Y -= blockSize;
			}
			X += blockSize;
		}
	};

public:
	bool OnUserCreate() override
	{
		
		
				
		// Maybe this is a good solution? You could do post processing. but it seems a little clunky.
		Thruster::Renderer = this;
		hud.host = this;
		AsteroidFont::host = this;

		hud.anchor = { 0,400 };
		
		nominalScreenHeight = screenDims.y;
		
		
		// Below is permanent stuff
		physicsEngine.bodies.push_back(&ship.body);
		physicsEngine.addEnviroment(&env);
		
		
		frameMaxDt = 1/200.0f;
		timeMultiplier = 1.0f;
		screenOffset.x = ScreenWidth() / 2;
		screenOffset.y = ScreenHeight() / 2;

		

		frameDelay = 1.0f;
		ResetGameState();
		
		return true;
	}
public:
	bool OnEveryFrame(float fElapsedTime) override
	{
		
		realTime += fElapsedTime;
		

		//! This variable is used for all physics simulations. fElapsed time is used for user input.
		float frameTimeStep = fElapsedTime* timeMultiplier;
		// Cause an intential slow down of the game to enforce maximum game dt
		if (frameTimeStep > (frameMaxDt)) {
			frameTimeStep = frameMaxDt;
		}
		// the timeMultiplier has an effect.

		
		olc::vf2d mousePos(toWorld(olc::vi2d(GetMouseX(), GetMouseY())));
		olc::vi2d mouseLoopBack(toScreen(mousePos));
		DrawBackground();
		
		DrawDebugLine(std::to_string(missionElapsedTime));
		
		DrawDebugLine("pX:" + std::to_string(ship.body.pos.x) + " pY:" + std::to_string(ship.body.pos.y));
		DrawDebugLine("vX:" + std::to_string(ship.body.vel.x) + " vY:" + std::to_string(ship.body.vel.y));
		DrawDebugLine("aX:" + std::to_string(ship.body.acc.x) + " aY:" + std::to_string(ship.body.acc.y));
		DrawDebugLine("Rudder:" + std::to_string(ship.rudderAngle*180/M_PI));
		
		// USER INPUT
		
		//if (GetKey(olc::Key::SHIFT).bHeld) throttleRateModifer = 10.0f;
		//if (GetKey(olc::Key::CTRL).bHeld) throttleRateModifer = 0.05f;
		//
		// ---- Debug Controls
		if (GetKey(Key::TAB).bPressed) simRunning = !simRunning;
		if (GetKey(Key::ENTER).bPressed)  ship.body.setFixed(!ship.body.isFixed());
		if (GetKey(Key::R).bPressed)  ResetGameState();
		
		//----- Ship Controls
		
		// Rudder
		float rudderRate = 5.0f;
		if (GetKey(olc::Key::A).bHeld) { ship.setRudder(ship.getRudder() - rudderRate * frameTimeStep);}
		if (GetKey(olc::Key::D).bHeld) { ship.setRudder(ship.getRudder() + rudderRate * frameTimeStep);}
		float turnRate = 2.0f;
		if (GetKey(olc::Key::Q).bHeld) { ship.body.rot += turnRate * frameTimeStep; }
		if (GetKey(olc::Key::E).bHeld) { ship.body.rot -= turnRate * frameTimeStep; }
		// Sail
		float sailRate = 1.0f;
		if (GetKey(olc::Key::W).bHeld) { ship.setSail(ship.getSailSlackAngle() + sailRate * frameTimeStep); }
		if (GetKey(olc::Key::S).bHeld) { ship.setSail(ship.getSailSlackAngle() - sailRate * frameTimeStep); }


		//----- Camera Controls
		constexpr float camMoveRate = 1000.0f; // m/s
		if (GetKey(olc::Key::UP).bHeld) {    cameraPos.y += camMoveRate/getPixelsPerMeter() * fElapsedTime; }
		if (GetKey(olc::Key::DOWN).bHeld) {  cameraPos.y -= camMoveRate/getPixelsPerMeter() * fElapsedTime; }
		if (GetKey(olc::Key::RIGHT).bHeld) { cameraPos.x += camMoveRate/getPixelsPerMeter() * fElapsedTime; }
		if (GetKey(olc::Key::LEFT).bHeld) {  cameraPos.x -= camMoveRate/getPixelsPerMeter() * fElapsedTime; }
		//Zoom 													
		float zoomRate = 2.0f;
		if (GetKey(olc::Key::K1).bHeld) {
			nominalScreenHeight *= (1.0f + fElapsedTime * zoomRate);
			setScreenHeightMeters(nominalScreenHeight);
		}
		if (GetKey(olc::Key::K2).bHeld) { 
			nominalScreenHeight *= (1.0f - fElapsedTime * zoomRate);
			setScreenHeightMeters(nominalScreenHeight);
		}


		// Determine whether the sim should continue this frame.
		bool simThisFrame = simRunning;
		// Allow space to pause simulation, and do one frame at a time
		if (simRunning && GetKey(Key::SPACE).bPressed) {
			simThisFrame = false;
			simRunning = false;
		} else if (simRunning || GetKey(Key::SPACE).bPressed) {
			simThisFrame = true;
		}

		if (simThisFrame) {
			Pencil::clearDrawingQueue();
			float dt = frameTimeStep;
			if (GetKey(Key::CTRL).bHeld) dt = -dt; // Run backwards
			// Camera crap

			olc::vr2d displacement = ship.body.vel*0.1f;
			Real frameBorder = (Real)2;
			olc::vr2d maxMovement = { -(screenDims.x / 2 - frameBorder),(screenDims.y / 2 - frameBorder) };
			displacement = BoxClamp(displacement, maxMovement, -maxMovement);
			olc::vf2d camTarget = ship.body.pos + displacement;
			cam.updateState(camTarget, ship.body.vel, dt);
			cameraPos = cam.pos;
			ship.applyEnviromentForces(wind(ship.body.pos));
			ship.updateState(dt);

			missionElapsedTime += dt;
			// Test physics
			physicsEngine.PropgateState(dt);
			// Prototype HUD
			hud.getShipState(ship);
		}

		if (GetMouse(1).bPressed) {
			cameraPos = mousePos;
		}
		if (GetMouse(1).bHeld) {
			setScreenHeightMeters(nominalScreenHeight*0.1f);
		}
		else if (GetMouse(1).bReleased)
		{
			setScreenHeightMeters(nominalScreenHeight);
		}
		DrawDebugLine("Delta (M):" + std::to_string((cam.vel - cam.anchorVel).mag()) + "Vy:" + std::to_string(cam.vel.y));
		// DRAWING 

		//debug
		Draw(mouseLoopBack);

		// Actual
		ship.Draw();

		// Draw Centroidsqe
		//hud.Draw();
		DrawDebugLine("Sail Slack Angle:" + std::to_string(ship.sailSlackAngle* 180 / M_PI));
		DrawDebugLine("Sail     Angle:" + std::to_string(ship.sailAngleFromCenterline * 180 / M_PI));
		DrawDebugLine("Heading       :" + std::to_string(ship.getHeading() * 180 / M_PI));
		Pencil::DrawNow();
		
		return true;
	}
};


int main(){
	Lando bedStead;

	if (bedStead.Construct(500, 500, 2, 2)) {
		bedStead.setScreenHeightMeters(10.0f);
		bedStead.Start();
	}
	return 0;
}

