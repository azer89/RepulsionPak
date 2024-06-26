
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "SelfIntersectionFixer.h"

#include "clipper.hpp"

// http:_//doc.cgal.org/latest/Sweep_line_2/index.html

#include <iostream>

// Computing intersection points among curves using the sweep line.
/*#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>
#include <list>

typedef CGAL::Quotient<CGAL::MP_Float>                  NT;
typedef CGAL::Cartesian<NT>                             Kernel;
typedef Kernel::Point_2                                 Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
typedef Traits_2::Curve_2                               Segment_2;*/

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <iostream>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point;
typedef CGAL::Polygon_2<K> Polygon_2;

SelfIntersectionFixer::SelfIntersectionFixer()
{
}

SelfIntersectionFixer::~SelfIntersectionFixer()
{
}

bool SelfIntersectionFixer::IsSimple(std::vector<AVector> poly)
{
	//Point points[] = { Point(0, 0), Point(5.1, 0), Point(1, 1), Point(0.5, 6) };
	std::vector<Point> points;
	for (int a = 0; a < poly.size(); a++)
	{
		points.push_back(Point(poly[a].x, poly[a].y));
	}

	Polygon_2 pgn(points.begin(), points.end());
	// check if the polygon is simple.
	bool isSimple = pgn.is_simple();
	//std::cout << "The polygon is " <<
	//	(isSimple ? "" : "not ") << "simple." << std::endl;
	// check if the polygon is convex
	//std::cout << "The polygon is " <<
	//	(pgn.is_convex() ? "" : "not ") << "convex." << std::endl;
	return isSimple;
}

void SelfIntersectionFixer::FixSelfIntersection1(std::vector<AVector> oldPoly, std::vector<AVector>& newPoly)
{
	float cScaling = 1e10; // because clipper only uses integer
	ClipperLib::Path oldCPoly;
	for (int a = 0; a < oldPoly.size(); a++)
	{
		oldCPoly << ClipperLib::IntPoint(oldPoly[a].x * cScaling, oldPoly[a].y * cScaling);
	}

	ClipperLib::Paths newCPolys;
	ClipperLib::SimplifyPolygon(oldCPoly, newCPolys);

	/*std::vector<std::vector<AVector>> newPolys;
	for (int a = 0; a < newCPolys.size(); a++)
	{
		std::vector<AVector> pol;
		for (int b = 0; b < newCPolys[a].size(); b++)
		{
			AVector pt(newCPolys[a][b].X / cScaling, newCPolys[a][b].Y / cScaling);
			pol.push_back(pt);
		}
	}*/
	// find the largest
	float maxArea = std::numeric_limits<float>::min();
	ClipperLib::Path newCPoly;
	for (int a = 0; a < newCPolys.size(); a++)
	{
		float polyArea = std::abs(ClipperLib::Area(newCPolys[a]));
		if (polyArea > maxArea)
		{
			newCPoly = newCPolys[a];
			maxArea = polyArea;
		}
	}

	for (int b = 0; b < newCPoly.size(); b++)
	{
		AVector pt(newCPoly[b].X / cScaling, newCPoly[b].Y / cScaling);
		newPoly.push_back(pt);
	}
}