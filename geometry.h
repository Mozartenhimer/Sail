#pragma once
#include "vectorExtendedPGE.hpp"
#include "globals.h"
class VectorShape {
	/*std::vector<vr2d> points;
	VectorShape();*/
protected:
public:
	std::string name = "";
	olc::Pixel color = olc::WHITE;
	VectorShape();
};
struct Contact {
	Contact() : status(NO_INTERSECT), mag(0) {};
	enum {
		NO_INTERSECT = 0,
		INTERSECT,
		COINCIDENT,
		PARALLEL
	} status;
	//! @brief Intersection Point
	olc::vr2d IP;
	//! @brief Normalized Displacemnt Vector
	olc::vr2d ND;
	//! @brief Displacement Magnitude to Statically resolve Collision.
	Real mag;
	//!
	olc::vr2d displacement() { return mag * ND; };

};

class Line : public VectorShape {
private:
	union {
		olc::vr2d p1v;
		olc::vr2d * p1p;
	};
	union {
		olc::vr2d p2v;
		olc::vr2d * p2p;
	};
	// If false, p1v and p2v are actually float vectors
	
	char flags; // true if it's using pointers to point at the points.
	enum {
		INDIRECT = 1,
		// Where forward is from p1 to  p2
		INSIDE_RIGHT = 1 << 1, // otherwise inside left
		// todo IMPLEMENT & sidedness
	};
public:
	
	Line();
	Line(const Line & parent);
	//Line(std::initializer_list<olc::vr2d>);
	Line(olc::vr2d * p1, olc::vr2d * p2);
	Line(olc::vr2d   p1, olc::vr2d   p2);
	// Copy constructors
	//Line(Line & );
	//! Also accepts a NULL pointer if you don't need an intersectionPoint.
	Contact Intersects(Line & L);
	olc::vr2d vector();
	olc::vr2d normVector();
	void inline makeOwn() {
		if (flags | INDIRECT) {
			flags ^= INDIRECT;
			olc::vr2d p1 = *p1p;
			olc::vr2d p2 = *p2p;
			p1v = p1;
			p2v = p2;
		}
	};
	bool inline isIndirect() const
	{
		return flags & INDIRECT;
	}
	bool inline isInsideRight() const  { return flags | INSIDE_RIGHT; }
	void inline setInsideRight(bool setInsideRight) 
	{ // GOOD ANSWER https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
		if (setInsideRight) flags |= INSIDE_RIGHT;
		else flags &= ~INSIDE_RIGHT;
	};
	inline Real Length() {
		return (p1() - p2()).mag();
	}
	olc::vr2d & p1();
	olc::vr2d & p2();
	
};


class SegmentedCurve : public VectorShape {
public:
	std::vector<olc::vr2d>  points;
	char type;
	enum { CLOSED, OPEN };
	SegmentedCurve();
	SegmentedCurve(std::string filename);
	Contact Intersects(Line & L);

	static std::vector<SegmentedCurve> LoadFromSVG(std::string filename);
	Line getLineSegment(size_t idx);

};

class Circle : public VectorShape {
public:
	Real radius;
	olc::vr2d  pos;

	Circle();
	Circle(olc::Pixel color_);
	Contact Intersects(Line & L);
	Contact Intersects(Circle & C);
	Contact Intersects(SegmentedCurve & C);

	

};





