#pragma once
#include "vectorExtendedPGE.hpp"
#include "crafts.h"
#include "AsteroidsFont.h"
class AsteroidFont {
public:
	static olc::VxOLCPGE * host;
	static inline void DrawLine(olc::vi2d location, const std::string & Line, olc::Pixel color = olc::DARK_GREEN, float scale = 1.0f) {
		// TODO: Normalized Scaling.
		constexpr int width = 12;
		constexpr int tableOffset = 'P' - '0';// yea...
		std::string Line2 = Line;
		std::transform(Line2.begin(), Line2.end(), Line2.begin(), ::toupper);
		// Loop through characters
		for (int i = 0; i < Line.length(); i++) {
			asteroids_char_t cmprssd = asteroids_font[Line[i] - tableOffset];
			olc::vi2d start, end;
			auto extract = [&](uint8_t packed) {
				uint8_t Y = packed & ~0xF0;
				uint8_t X = packed >> 4;
				olc::vi2d characterLocation = location 
					+ olc::vi2d((int)(width + (float)i * (float)width*scale), 0);
				olc::vi2d point = (olc::vi2d)(olc::vf2d((float)X, -(float)Y)*scale);
				return point + characterLocation;
			};

			int pidx = 0;
			uint8_t P = cmprssd.points[pidx++];
			start = extract(P);

			/// Loop through Points in the Character
			while (true) {
				if (P == FONT_LAST) break;
				P = cmprssd.points[pidx++];

				if (P == FONT_LAST)
					break;
				else if (P == FONT_UP) {
					start = extract(cmprssd.points[pidx++]);
					continue;
				}
				end = extract(P);

				//dbg(start);

				host->DrawLine(start, end, olc::GREEN);
				start = end;
				if (pidx >= 8) break;
			}
		}
	}
};
class HUD {
public:
	 olc::VxOLCPGE * host;
	 HUD() {};
	 HUD(olc::VxOLCPGE * _host) : host(_host) {
		 anchor = {0.0f,0.8f}; 

	 };
	// Wants;
	// Would love to have each field have an auto derivative with it.
	// Delta V remainging
	// Hovertime remaining
	// velocity vector
	// acceleration Plot (this isn't easy)
	// Acceleration
	// Throttle
	// Mini map
	 
	//.......
	// Data Flow: 
	// 1. Populate "frame" with data.
	// 2. Draw Data
	olc::vf2d anchor; // Normalized Screen Coordinates
	//struct {
		float deltaVRemaining;
		float hoverTime;
		float throttle;
		olc::vf2d pos;
		olc::vf2d vel;
		olc::vf2d acc;
		float rot;
		float rotDot;
		float rotDotDot;
	//} shipState;
	
	inline void Draw() {
		// TODO: some sort of tile.
		DrawLine({ 0,0 }, "X:" + std::to_string(pos.x));
		DrawLine({ 0.1f,0.1f}, "HEADS UP DISPLAY");
	}
	
	inline olc::vi2d toScreen(olc::vf2d normPosition) {
		olc::vi2d screenDims = { host->ScreenWidth(),host->ScreenHeight() };
		return olc::vi2d { (int)((float)screenDims.x*normPosition.x), (int)((float)screenDims.y*normPosition.y) } + anchor;
	}

	inline void DrawLine(olc::vf2d location, std::string Line, olc::Pixel color = olc::DARK_GREEN, float scale = 1.0f)
	{
		AsteroidFont::DrawLine(toScreen(location), Line, color, scale);
	}

	inline void getShipState(Ship & ship) {
		pos                  = ship.body.pos;
		vel                  = ship.body.vel;
		acc                  = ship.body.acc;
		rot                  = ship.body.rot;
		rotDot               = ship.body.rotDot;
		rotDotDot = ship.body.rotDotDot;
	}

};
