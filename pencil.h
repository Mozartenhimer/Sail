#pragma once
#include "vectorExtendedPGE.hpp"
#include <vector>
#include "physics.h"
#include "geometry.h"
struct Arrow {
	olc::vf2d tailLocation;
	olc::vf2d vector;
	olc::Pixel color;
	float scale;
	void Draw();
};
class Pencil {
	// Singleton
	// Trying to make this a buffered Draw, so that debug information can be requested to be drawn not every frame
	// Allowing for simulation pausing.
public:
	static olc::VxOLCPGE * host;
	Pencil() = delete;
	static std::vector<Arrow> arrows;
	/*Pencil(olc::VxOLCPGE * Renderer) {
		host = Renderer;
	};*/
	static inline void Draw(Line & L, olc::Pixel Color = 0) {
		if (Color == 0) {
			Color = L.color;
		}
		host->DrawLine(host->toScreen(L.p1()), host->toScreen(L.p2()), Color);
	};
	static inline void Draw(Circle & C, olc::Pixel Color = 0)
	{
		if (Color == 0) {
			Color = C.color;
		}
		host->DrawCircle(host->toScreen(C.pos), (int)host->toScreen(C.radius), Color);
	}
	static inline void Draw(SegmentedCurve & S, olc::Pixel Color = 0)
	{
		if (Color == 0) {
			Color = S.color;
		}

		for (int i = 0; i < (int)S.points.size() - 1; i++) {

			Line L = { S.points.at(i),S.points.at(i + 1) };
			host->DrawLine(host->toScreen(L.p1()),
				host->toScreen(L.p2()));
		}

		if (S.type == S.CLOSED && S.points.size() > 2) {// draw last line
			Line L = { S.points.at(0),S.points[S.points.size() - 1] };
			host->DrawLine(host->toScreen(L.p2()), host->toScreen(L.p1()), Color);
		}; 
	}

	static inline void Draw(RigidBody & B, olc::Pixel Color = 0)
	{
		for (auto & C : B.circles_w) {
			Draw(C);
		}
		for (auto & S : B.curves_w) {
			Draw(S);
		}
		for (auto & L : B.lines_w) {
			Draw(L);
		}
	}
	static void AddArrow(olc::vf2d tailLocation, olc::vf2d vector, float scale = 1.0f, olc::Pixel color = olc::Pixel(255, 150, 150));
	static inline void DrawNow() {
		for (auto & A : arrows) {
			A.Draw();
		}
	};
	static inline void clearDrawingQueue() {
		arrows.clear();
	};

};
