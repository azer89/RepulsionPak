
#include "CGALIntersector.h"

//read this
// http://stackoverflow.com/questions/26246381/how-to-calculate-the-intersection-of-a-line-segment-and-circle-with-cgal

// Computing intersection points among curves using the sweep line.
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>

#include <list>
#include <iostream>

typedef CGAL::Quotient<CGAL::MP_Float>                  NT;
typedef CGAL::Cartesian<NT>                             Kernel;
typedef Kernel::Point_2                                 Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
typedef Traits_2::Curve_2                               Segment_2;

AVector CGALIntersector::FindTheClosestIntersection(ALine line, std::vector<AVector> shape)
{
	std::vector<Segment_2> segments;
	int sz = shape.size();
	for (int a = 0; a < sz - 1; a++)
	{
		segments.push_back(Segment_2(Point_2(shape[a].x, shape[a].y), Point_2(shape[a + 1].x, shape[a + 1].y)));
	}
	segments.push_back(Segment_2(Point_2(shape[sz - 1].x, shape[sz - 1].y), Point_2(shape[0].x, shape[0].y)));

	segments.push_back(Segment_2(Point_2(line.XA, line.YA), Point_2(line.XB, line.YB)));

	std::vector<Point_2> pts;
	CGAL::compute_intersection_points(segments.begin(), segments.end(), std::back_inserter(pts));

	float dist = std::numeric_limits<float>::max();
	AVector intersectPt;
	for (int a = 0; a < pts.size(); a++)
	{
		float x = CGAL::to_double(pts[a].x());
		float y = CGAL::to_double(pts[a].y());
		AVector pt(x, y);
		float d = line.GetPointA().Distance(pt);
		if (d < dist)
		{
			dist = d;
			intersectPt = pt;
		}
	}
	return intersectPt;
}