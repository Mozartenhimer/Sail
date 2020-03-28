#pragma once
#include "vectorExtendedPGE.hpp"
#include "Geometry.h"
#define _USE_MATH_DEFINES
#include <math.h>
typedef  float Real;
struct State {
	//Pressure, temperature, density, Machnumber
	Real p, T, rho,M;
};

class streamLine
{
public:
	// Setup nozzel exit state.
	float intialMassFlow; //kg*m/s
	float maxLength;
	streamLine * leftNeighbor;
	streamLine * rightNeighbor;

	streamLine();
	~streamLine();
};
class Plume
{
	// Is a bunc of streamlines
	Real gamma = 1.4f;
public:
	Plume();
	~Plume();
	Real getShockAngle(Real theta, Real  M1);
	
};
/* 

// From: https://en.wikipedia.org/wiki/Oblique_shock
// The theta-beta-Mach equations
// beta = Shock angle from relative to previous streamline
// theta = corner angle
// M1   = Upstream Mach no
tan(theta) = 2 * (1 / tan(beta))*(M1 ^ 2 * pow(sin(beta), 2) - 1) / (M1*M1*gamma + cos(2 * beta) + 2)
// Max Deflection Angle (before going to bow shock)
M2 = 1 / (sin(beta - theta))
		 *sqrt(
		     (1+(gamma-1)/2*M1*M1*pow(sin(beta),2))
	        / (gamma*M1*M1*pow(sin(beta),2) - (gamma-1)/2)
		 );
// From: https://www.grc.nasa.gov/www/k-12/airplane/detach.html
// Across the normal shock wave the Mach number decreases to a value specified as M1 :
M2 ^ 2 = ((gamma - 1) * M1 ^ 2 + 2) / (2 * gamma * M1 ^ 2 - (gamma - 1))

// Look at :
// https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19660007268.pdf
// https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/20110014613.pdf

*/