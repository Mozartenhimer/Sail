#include "Geometry.h"
#include "nanosvg.h"
#include "dbg.h"
using namespace olc;
// Current Intersection compreh
// Cicle <> Line
// Polygon <> Line  = Line <> Npoly lines
// Line <> Line
// Polygon <> Polygon = N^2 LIne <> line



VectorShape::VectorShape() {};
Contact reduceContacts(const Line & L, const std::vector<Contact> & touches) {
	//TODO
	if (touches.size() > 0) return touches.front();
	return Contact();
};

Contact reduceContacts(const Circle & C, const std::vector<Contact> & touches) {
	// TODO
	if (touches.size() > 0) return touches.front();
	return Contact();
};
Line::Line()
{
	flags = 0b00;
	p1v = olc::vr2d();
	p2v = olc::vr2d();
}
Line::Line(const Line & parent)
{
	flags = parent.flags;
	flags &= ~INDIRECT;
	if (parent.isIndirect()){
		p1v = *parent.p1p;
		p2v = *parent.p2p;
	}
	else {
		p1v = parent.p1v;
		p2v = parent.p2v;
	}
}

olc::vr2d & Line::p1()
{
	if (isIndirect()) {
		return *p1p;
	}
	else
		return p1v;
}
olc::vr2d & Line::p2()
{
	if (isIndirect()) {
		return *p2p;
	}
	else
		return p2v;
}
Line::Line(olc::vr2d p1, olc::vr2d p2)
{
	flags  = 0;
	p1v = p1;
	p2v = p2;
}
Line::Line(olc::vr2d * p1, olc::vr2d * p2)
{
	flags = INDIRECT;
	p1p = p1; 
	p2p = p2;
}



Contact Line::Intersects(Line & L) { 
	Contact C; 
	
	olc::vr2d IP;
	olc::vr2d * intersectionPoint= &IP;
	olc::vr2d NAD;
	olc::vr2d * normalDir = &NAD;
	float mag;
	float * normalDisp = &mag;


	// From Wikipedia
	//https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
	vf2d const & p3 = L.p1(); // To match wiki
	vf2d const  & p4 = L.p2();
	olc::vr2d ND= this->normVector().perp();
	// if The Same line...
	if ((p1().x - p2().x)*(p3.y - p4.y) - (p1().y - p2().y)*(p3.x - p4.x) == 0.0f) {
		C.IP = p1();
		Contact C;
		C.Contact::COINCIDENT;
		return C;
	}
	
	olc::Mat2d Top(p1().x - p3.x, p3.x - p4.x,
		           p1().y - p3.y, p3.y - p4.y);
	olc::Mat2d Bot(p1().x - p2().x, p3.x - p4.x,
		           p1().y - p2().y, p3.y - p4.y);
	// t = normalized this line coordinate
	float t = Top.det() / Bot.det();

	Top = olc::Mat2d(p1().x - p2().x, p1().x - p3.x,
				     p1().y - p2().y, p1().y - p3.y);
	// u = normalized argument L line coordinate
	float u = - Top.det() / Bot.det();

	C.IP.x = (p1().x + t * (p2().x - p1().x));
	C.IP.y = (p1().y + t * (p2().y - p1().y));
	
	if ((t >= 0 && t <= 1.0) && (u >= 0 && u <= 1))
		C.status = Contact::INTERSECT;
	else
		C.status = Contact::NO_INTERSECT;

	// Polygons are defined in counter clockwise orientation.

	C.ND = ND;
	// Find the normalDisplacement
	olc::vr2d in, out ;
	if (u > 0.5f) {
		in = p4; // TODO look at this assumption, use normal methods for polygons
		out = p3;
	} else {
		in = p3;
		out = p4;
	}

	// Find intruding Length
	//olc::vr2d poke = *intersectionPoint - in;
	olc::vr2d pokingDistance = C.IP - in;
	//if (normalDisp != nullptr) *normalDisp = (*normalDir).dot(poke);
	C.mag = C.ND.dot(pokingDistance);
	return C;
}


olc::vr2d Line::vector() 
{
	return (p2() -p1());
}

olc::vr2d Line::normVector()
{
	return vector().norm();
}

Contact SegmentedCurve::Intersects(Line & L)
{
	// TODO Change this to only output one combined collision
	std::vector<Contact> contacts;
	//dbg(this->name);
	for (size_t i = 0; i < points.size(); i++) // Line in shape
	{
		Line test = getLineSegment(i);
		Contact C = test.Intersects(L);
		if (C.status) contacts.push_back(C);
			
	}
	// Resolve Collision
	if (contacts.size() == 0)
		return Contact();
	return reduceContacts(L, contacts); // TODO
}


std::vector<SegmentedCurve> SegmentedCurve::LoadFromSVG(std::string filename)
{
	// Convention is to have the file be created in inches, and 1 inch correspond to a meter
	// Load
	//  Currently assumes one shape.
	// TODO: Make this function load a rigid body as multiple shapes.  Going to need to move.
	//       I think it's at point for being able to do that quite smoothly.
	std::vector<SegmentedCurve> shapes;
	struct NSVGimage* image;
	image = nsvgParseFromFile(filename.c_str(), "in", 1);
	printf("size: %f x %f\n", image->width, image->height);
	// Use..
	int i = 0;
	size_t nShapes = 0;
	size_t nPaths = 0;
	for (NSVGshape * shape = image->shapes; shape != NULL; shape = shape->next) {
		nShapes++;
		shapes.emplace_back(SegmentedCurve());
		SegmentedCurve & currentShape = shapes.back();
		for (NSVGpath * path = shape->paths; path != NULL; path = path->next) {
			nPaths++;
			size_t j;
			size_t pushed = 0;
			float oldSlope = NAN;
			for (j = 0; j < path->npts; j++) {
				vr2d newPoint = { *(path->pts + 2 * j + 0),-*(path->pts + 2 * j + 1) };

				if (j < 1) {
					currentShape.points.push_back(newPoint);
					pushed++;
				}
				else {// Inkscape likes to put additional colinear points in. So remove them.
					float slope = (newPoint.y - currentShape.points.back().y) /
						(newPoint.x - currentShape.points.back().x);
					if (abs(slope - oldSlope)< 0.00001f) { // colinear
						currentShape.points.back() = newPoint;
					}
					else { // not colienar
						currentShape.points.push_back(newPoint);
						pushed++;
					}
					oldSlope = slope;
				}

			}

			if ((currentShape.points.front() - currentShape.points.back()).mag2() < 0.00001) {
				currentShape.points.pop_back();
				currentShape.type = CLOSED;
			}
			else {
				currentShape.type = OPEN;
			}
		}

	}

	nsvgDelete(image);
	dbg(nShapes);
	dbg(nPaths);
	
	// Delete
	return shapes;
}
Line  SegmentedCurve::getLineSegment(size_t idx)
{
	//Line ret = Line(&points[idx], &points[idx + 1]);
	if (idx == points.size()-1)
		return Line(&points[idx], &points[0]);
	else
		return  Line(&points[idx], &points[idx + 1]);
}

SegmentedCurve::SegmentedCurve(std::string filename)
{LoadFromSVG(filename);}

SegmentedCurve::SegmentedCurve()
{
	type = OPEN;
}
// Circle Stuff


Circle::Circle()
{
	radius = 1.0f;
	pos = vr2d();
}

Circle::Circle(olc::Pixel color_)
{
	*this = Circle();
	color = color_;
}


Contact Circle::Intersects(Line & L)
{
	Contact C;
	// Using
	// https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Line_defined_by_two_points
	// Matching notation
	vr2d & p1=L.p1();
	vr2d & p2=L.p2();
	vr2d & p0 = pos;
	// TODO: Square instead of sqrt optimize
	//TODO I don't think this is accounting for the finite line ness.
	Real distance = abs((p2.y - p1.y)*p0.x - (p2.x - p1.x)*p0.y + p2.x*-p1.y - p2.y*p1.x) /
		(p2-p1).mag(); 
	if (distance <= radius) {
		// Try the cross produce
		vr2d para = L.normVector();
		vr2d test = pos - p1;
		if (para.cross(test) > (Real)0)
		{
			//*normalDir = para.perp();
			C.ND = para.perp();
		}
		else
		{
			C.ND = -para.perp();
		}

		
		
	}
	

	return C;

	
}
