#include "pch.h"
#include "physics.h"
#include "globals.h"
#include "dbg.h"
#include "pencil.h"
int RigidBody::baseID = 0;
RigidBody::RigidBody()
{
	id = baseID++;
	mass = (Real)1;
	inertiaMoment = (Real)10;
	bounds.topLeft = { (Real)0.5,(Real)0.5 };
	bounds.bottomRight = { (Real)-0.5,(Real)-0.5 };
}

RigidBody::RigidBody(Real Mass, Real Izz)
{
	id = baseID++;
	this->mass = Mass;
	this->inertiaMoment = Izz;
}

void RigidBody::addLine(const Line & L )
{
	//TODO Update Bounnding Box
	lines_b.push_back(L);
	Line caboose;
	lineToWorld(caboose);
	lines_w.push_back(caboose);
}

void RigidBody::addSegmentedCurve(const SegmentedCurve & SC)
{
	//TODO Update Bounnding Box
	curves_b.push_back(SC);
	SegmentedCurve caboose;
	segmentedCurveToWorld(caboose);
	curves_w.push_back(caboose);
}

void RigidBody::addCircle(const Circle & C)
{
	//TODO Update Bounnding Box
	circles_b.push_back(C);
	Circle caboose;
	circleToWorld(caboose);
	circles_w.push_back(caboose);
}

bool RigidBody::collidesWith(RigidBody & otherBody, std::vector<Contact>& contacts)
{
	// Append to vector the collisions we see
	// Prevent from colliding with self
	if (&otherBody == this) return false;
	//if (!bounds.checkOverLap(otherBody.bounds)) return false;
	
	
	// For each line in Rigid body 
	for (auto & L  : lines_w) {
		
		for (auto & O : otherBody.lines_w) {
			// If there's a contact, push_back on the frameCollisions.
			Contact C = L.Intersects(O);
			if (C.status == C.INTERSECT) contacts.push_back(C);
		}
		for (auto & O : otherBody.curves_w) {
			Contact C = O.Intersects(L);
			if (C.status == C.INTERSECT) contacts.push_back(C);
		}
		for (auto & O : otherBody.circles_w) {
			// If there's a contact, push_back on the frameCollisions.
			Contact C = O.Intersects(L);
			if (C.status == C.INTERSECT) contacts.push_back(C);
		}
	}

	for (auto & C : circles_w) {
		
	}
	for (auto & C : curves_w) {

	}
	return false;
}

bool RigidBody::collidesWith(Enviroment & env, std::vector<Contact>& frameCollisions)
{
	bool hit = false;
	
	for (auto & B : env.bodies) {
		hit |= collidesWith(B, frameCollisions);
	}
	
	return hit;
}

void RigidBody::circleToWorld(Circle & C) {
	// Rotate then translate
	C.pos = body2world*C.pos + pos;
}

void  RigidBody::lineToWorld(Line & L) {
	L.p1() = body2world * L.p1() + pos;
	L.p2() = body2world * L.p2() + pos;
}

void RigidBody::segmentedCurveToWorld(SegmentedCurve  & SC) {
	for (auto & P : SC.points)
		P = body2world * P + pos;
		
}
void RigidBody::updateMeshInWorld() {
	//TODO Update Bounnding Box
	body2world = olc::Mat2d(rot);
	for (size_t i = 0; i < circles_b.size(); i ++) {
		circles_w[i] = circles_b[i];
		circleToWorld(circles_w[i]);
	}
	for (size_t i = 0; i < lines_b.size(); i++) {
		lines_w[i] = lines_b[i];
		lineToWorld(lines_w[i]);
	}
	for (size_t i = 0; i < curves_b.size(); i++) {
		curves_w[i] = curves_b[i];
		segmentedCurveToWorld(curves_w[i]);
	}
}

void RigidBody::propogateKinematics(Real timeStep)
{
	acc = (externalForces)/
		(mass);  // m/s^2

	// Translation
	vel += acc * timeStep;
	pos += vel * timeStep;

	rotDotDot = externalMoments / inertiaMoment;
	//  Rotation

	rotDot += rotDotDot * timeStep;
	rot += rotDot * timeStep;
	auto B = this;


}


int Pricipia::PropgateState(Real timeStep)
{
	Real dt = timeStep;
	int n = 1;
	int collisions = 0;
	// Subdivide timesteps if needed
	if (dt > minTimeStep)
	{
		n = (int)ceil(timeStep / minTimeStep);
		dt = timeStep / ((Real)n);
	}
	
	for (int i = 0; i < n; i++) { // Do timesteps
		
		// These value for if it colli
		
		
		// Propogate state
		for (auto & B : bodies) {
			if (B->isFixed()) continue;
				
			B->updateMeshInWorld();
			B->stashKinematicState();
			B->propogateKinematics(timeStep);
		}
		std::vector<Contact> frameContacts;
	
		// Collision checking Pass
		for (auto & B : bodies) {
	
 			// if (B->isFixed()) continue;
			B->updateMeshInWorld();
			for (auto & check : bodies) {

				// No checky against self
				if (&check == &B) continue;
				// Contact 
				B->collidesWith(*check, B->frameContacts);
			}
		}
		collisions = (int)frameContacts.size();
		// Resolve with contacts
		for (auto & B : bodies) {
			for(auto & C : B->frameContacts){
				
				B->pos += C.displacement(); // TODO: ? How would you do this with rotations included?

				// Collide force must stop it in the normal line direction (for a resistution of zero)
				// The Collide force must also halt rotation into the object. 
				// However, tangent to the line, it should be governed by a friction coeff
				constexpr float restitution = 0.0f; // Todo use body 

				olc::vr2d vHit = B->body2world * (C.IP.cross(-B->rotDot)) + B->velPrev;
				Pencil::AddArrow(C.IP, vHit, 0.5f, olc::RED);
			
				
				
			

				olc::vr2d collideNormalForce = -vHit.dot(C.ND) / timeStep * C.ND;
			

				B->applyForce_w(collideNormalForce);

				
								
				olc::vr2d r =  C.IP- B->pos;
				// Calculate the tangential velocity and normalize, and take negative to get direction vector
				//olc::vr2d v_tan = -r.cross(B->rotDot);
				olc::vf2d v_tan = B->body2world * (-1.0f*C.IP.cross(B->rotDot)) + B->velPrev;
				
				
				// Make a force on the body to halt the rotation exactly
				olc::vr2d collideRotForce = B->rotDot*B->inertiaMoment*(-v_tan.norm()) / timeStep;
				if (collideRotForce.dot(v_tan) > 0) collideRotForce *= -1.0;
				Real collideMoment = -collideRotForce.cross(r);
				
				Pencil::AddArrow(C.IP, collideRotForce, 0.01f, olc::Pixel(150, 200, 0));
				Pencil::AddArrow(C.IP, v_tan, 5, olc::BLUE);
				dbg("===============");
				dbg(r);
				dbg(C.IP);
				dbg(B->rotDot);
				dbg(v_tan);
				dbg(collideMoment);
				//Pencil::AddArrow(C.IP, v_t);
				B->applyMoment(collideMoment);
				
				//dbg(collideMoment);
				// recompute the states 	
				B->propogateKinematics(timeStep);
				B->frameContacts.clear();
			}
			B->zeroExternals();
		}


	}
		

	return collisions; // number of timesteps
}

Pricipia::Pricipia()
{
}

Enviroment Enviroment::LoadFromSVG(std::string filename)
{
	Enviroment output;
	std::vector<SegmentedCurve> Shapes = SegmentedCurve::LoadFromSVG(filename);
	// For each shape, create a rigid body
	for (auto & S : Shapes) {
		// Find Centroid 
		// From Here:https://en.wikipedia.org/wiki/Centroid#Of_a_polygon 
		olc::vr2d Centroid;

		// Dumb centroid
		for (auto & P : S.points) {
			Centroid += P;
		}
		Centroid /= (float)S.points.size();
		if(false){
			//Smart Centroid that doesn't seem to work
			Real Area = (Real)0;
			std::vector<olc::vr2d> & P = S.points;
			for (size_t i = 0; i < S.points.size() - 1; i++) {
				Area += (P[i].x*P[i + 1].y - P[i + 1].x*P[i].y);
				Centroid.x += (P[i].x + P[i + 1].x)*(P[i].x*P[i + 1].y - P[i + 1].x*P[i].y);
				Centroid.y += (P[i].y + P[i + 1].y)*(P[i].x*P[i + 1].y - P[i + 1].x*P[i].y);
			}
			Area /= (Real)2;
			dbg(Area);
			Centroid /= (6 * Area);
			// Make the body have a local coordinate system.
		}
		for (auto & P  : S.points) {
			P -= Centroid;
		}
	
		RigidBody caboose;
		caboose.addSegmentedCurve(S);
		caboose.setFixed(true);
		caboose.pos = Centroid;
		output.bodies.push_back(caboose);
	}


	return output;
}
