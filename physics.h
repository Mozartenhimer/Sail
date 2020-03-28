#pragma once
#include <vector>

#include "Geometry.h"


struct BoundingBox {
	// axis aligned bounding box
	olc::vr2d topLeft;
	olc::vr2d bottomRight;
	bool checkOverLap(BoundingBox const & BB){
		if (bottomRight.y > BB.topLeft.y || BB.bottomRight.y > topLeft.y)
			return false;
		if (bottomRight.x < BB.topLeft.x || BB.bottomRight.x < topLeft.x)
			return false;

		return true; 
	}
};
class Enviroment;
class RigidBody {
	friend class RigidBody;
	static int baseID;
public:
	int id = -1;
	std::string name;
	BoundingBox bounds;
	Real inertiaMoment;
	Real mass;
	//! Kinematic State
	olc::vr2d pos;
	olc::vr2d vel;
	olc::vr2d acc;
	olc::vr2d posPrev;
	olc::vr2d velPrev;
public:
	Real rot; // Positive from X-axis CCW (or anticlockwise for you red coa`ts.)
	Real rotDot; 
	Real rotDotDot;
	Real rotPrev;
	Real rotDotPrev;
public:
	inline void stashKinematicState()
	{
		rotPrev = rot; rotDotPrev = rotDot;
		posPrev = pos; velPrev = vel;
	}

	Real frictionCoeff;
	Real restitutionCoeff;
	//! Forces & Moments at this time step. (body frame)
	
	olc::vr2d externalForces;
	Real externalMoments;
	//! Fixed or not fixed for now.
	enum {
		NOFLAGS = 0,
		FIXED = 1 << 0,
	};
private:
	char flags = 0;
public: // Boolean Statemodifiers
	inline void setFixed(bool fixation) { if (fixation) flags |= FIXED; else flags ^= FIXED;}
	inline bool isFixed() { return flags & FIXED; }
public:
	//! Coordinates with respect to center of mass.
	std::vector<Line> lines_b;
	std::vector<SegmentedCurve> curves_b;
	std::vector<Circle> circles_b;
	

	std::vector<Line> lines_w;
	std::vector<SegmentedCurve> curves_w;
	std::vector<Circle> circles_w;
public:	
	olc::Mat2d body2world; // rotation matrix. Should of gone homogeneous... oh well
	RigidBody();
	RigidBody(Real Mass, Real Izz);
	
	// Add geometry in body frame
	void addLine(const Line &);
	void addSegmentedCurve(const SegmentedCurve &);
	void addCircle(const Circle &);
	
	// apply force in body frame
	inline void applyForce_b(olc::vr2d Force) {
		externalForces += body2world*Force;
	};
	// Apply force in world frame
	inline void applyForce_w(olc::vr2d Force) {
		externalForces += Force;
	};
	inline void applyMoment(Real Moment) {
		externalMoments += Moment;
	};
	inline void zeroExternals() {
		externalForces = olc::vr2d();
		externalMoments = Real();
	};

	void updateMeshInWorld();
	void propogateKinematics(Real timeStep);
	bool collidesWith(RigidBody  & otherBody,std::vector<Contact> & frameCollisions);
	bool collidesWith(Enviroment & env, std::vector<Contact> & frameCollisions);
	std::vector<Contact> frameContacts;
private:
	void circleToWorld(Circle & C);
	void lineToWorld(Line & L);
	void segmentedCurveToWorld(SegmentedCurve & C);

public:
	

};
class Enviroment {
public:
	static Enviroment LoadFromSVG(std::string Filename);
	std::vector<RigidBody> bodies;


};
class Pricipia {

public:
	Real minTimeStep = 10.0f;
	std::vector<RigidBody*> bodies;
	
	int PropgateState(Real timeStep); 
	
	Pricipia();
	inline void addBody(RigidBody * newBody) { bodies.push_back(newBody);};
	inline void addEnviroment(Enviroment * env) {
		for (auto & B : env->bodies)
			bodies.push_back(&B);
	};
	
};
