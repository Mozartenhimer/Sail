#include "pch.h"
#include "pencil.h"
olc::VxOLCPGE * Pencil::host;
std::vector<Arrow> Pencil::arrows;
void Arrow::Draw() {
	// TODO Draw Arrow proper.
	olc::vi2d loc1 = Pencil::host->toScreen(tailLocation);
	olc::vi2d loc2 = Pencil::host->toScreen(tailLocation + vector * scale);
	Pencil::host->DrawLine(loc1, loc2, color);
	Pencil::host->DrawCircle(Pencil::host->toScreen(tailLocation), 2, color);
}

// Add arrows to a queue of arrows to be rendered until the queue of arrows is cleared
void Pencil::AddArrow(olc::vf2d tailLocation, olc::vf2d vector, float scale, olc::Pixel color) {

	// Cap the magnitude to prevent slowdow
	if (vector.mag2()*scale > host->screenDims.mag2())
	{
		vector /= host->screenDims.mag2()/scale;
	}

	Arrow A;
	A.tailLocation = tailLocation;
	A.vector = vector;
	A.scale = scale;
	A.color = color;
	
	arrows.push_back(A);
}
