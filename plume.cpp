#include "pch.h"
#include "plume.hpp"

#include <vector>

// TODO: Shock Shock interaction equations/
// setup flowfield
// 
using namespace std;
// Used for finding theta_max
// from: http://www.pdas.com/maxramp2.xml
// for gamma = 1.4

Real interpolate(const vector<Real> &xData, const vector<Real> &yData, Real x, bool extrapolate = false);
const std::vector<Real> maxThetaMach = { 1.050f,1.100f,1.200f,1.300f,1.400f,
                                         1.600f,1.800f,2.000f,2.500f,3.000f,
	                                     4.000f,5.000f,6.000f,8.000f,10.000f};
const std::vector<Real> maxThetaTable = {0.0097403f,0.0264446f,0.0688391f,0.1162752f,0.1645352f,
                                         0.2557175f,0.3348112f,0.4009638f,0.5200634f,0.5946937f,
                                         0.6767315f,0.7176386f,0.7407139f,0.7642938f,0.7754327f};
											


Plume::Plume()
{
}


Plume::~Plume()
{
}

streamLine::streamLine()
{
	intialMassFlow = 1.0f;
	maxLength = 2.0f;
}

streamLine::~streamLine()
{
}

Real Plume::getShockAngle(Real theta, Real  M1) {
	if (theta == 0.0f) return 0;
	
	/*Got psuedo code from:
     https://www.codesansar.com/numerical-methods/secant-method-pseudocode.htm */
	// Could also be approximated by 5th order polynomial:
	// y = 9E-05x5 - 0.0031x4 + 0.0432x3 - 0.3028x2 + 1.0767x - 0.8477 
	// R² = 0.9995
	Real maxObliqueShockTurn = interpolate(maxThetaMach, maxThetaTable, M1);
	if (theta > maxObliqueShockTurn) return (Real)M_PI/2;
	 //	2. Define function as f(x)
	auto f = [&](Real beta)	{return  (2 / tan(beta))*(M1*M1 * pow(sin(beta), 2) - 1) /
		                    (M1*M1*(gamma + cos(2 * beta)) + 2)    - tan(theta); };

	//	3. Input:
	//    a.Initial guess x0, x1
	// TODO Could improve intial guess with the hypersonic limit one.
	// These should be bounds on the 
	Real x0 = theta; 
	Real x1 = maxObliqueShockTurn;
	//	b.Tolerable Error e
	constexpr Real tol = 5.0f;
	//	c.Maximum Iteration N
	int N = 3;
	Real x2;
	//	4. Initialize iteration counter step =1
	int step = 0;
	Real Residual;

	//	5. Do
	do {
		//	If f(x0) = f(x1)
		if (f(x0) == f(x1)) return 0;
		//	Print "Mathematical Error"
		//	Stop
		//	End 
		//	x2 = x1 - (x1 - x0) * f(x1) / (f(x1) - f(x0))
		x2 = x1 - (x1 - x0) * f(x1) / (f(x1) - f(x0));
		//	x0 = x1
		x0 = x1;
		//	x1 = x2
		x1 = x2;
		//	step = step + 1
		step++;
		//	If step > N
		if (step > N) return x2;
			//	Print "Not Convergent"
			//	Stop
			//	End If
			//	While abs f(x2) > e
		Residual = abs(f(x2));
	} while (fabs(x0-x1) > tol);
	//dbg(Residual);
	//dbg(step);

	//	6. Print root as x2
	return x2;
}

Real interpolate(const vector<Real>   &xData, const vector<Real>  & yData, Real x, bool extrapolate)
{ 
	
	// from :http://www.cplusplus.com/forum/general/216928/
	size_t size = xData.size();

	size_t i = 0;        // find left end of interval for interpolation
	if (x >= xData[size - 2]) // special case: beyond right end
	{
		i = size - 2;
	}
	else
	{
		while (x > xData[i + 1]) i++;
	}
	Real xL = xData[i], yL = yData[i], xR = xData[i + 1], yR = yData[i + 1];      // points on either side (unless beyond ends)
	if (!extrapolate)                                                         // if beyond ends of array and not extrapolating
	{
		if (x < xL) yR = yL;
		if (x > xR) yL = yR;
	}

	Real dydx = (yR - yL) / (xR - xL);                                    // gradient

	return yL + dydx * (x - xL);                                              // linear interpolation
}