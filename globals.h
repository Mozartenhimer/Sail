#pragma once
#include "vectorExtendedPGE.hpp"
namespace g {
	constexpr  float g = 9.80665f; // for physics calculations m/s^2
	constexpr float earth_g = 9.80665f; //m/s^2
	const  olc::vf2d down(0.0f, -1.0f);
	constexpr float g_factor = 0.01f;
	extern bool debugOverlay;

}


