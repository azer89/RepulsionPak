
#include "CGALOffsetter.h"

#include <boost/timer.hpp>
#include <CGAL/Gps_traits_2.h>
#include <CGAL/offset_polygon_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/CORE_algebraic_number_traits.h>
#include <CGAL/Arr_conic_traits_2.h>
#include <CGAL/Arrangement_2.h>

// exact
typedef CGAL::CORE_algebraic_number_traits            Nt_traits;
typedef Nt_traits::Rational                           Rational;
typedef CGAL::Cartesian<Rational>                     Rat_kernel;
typedef Rat_kernel::Point_2                           Rat_point;
typedef Rat_kernel::Segment_2                         Rat_segment;
typedef Rat_kernel::Circle_2                          Rat_circle;
typedef Nt_traits::Algebraic                          Algebraic;
typedef CGAL::Cartesian<Algebraic>                    Alg_kernel;

// exact
typedef CGAL::Arr_conic_traits_2<Rat_kernel, Alg_kernel, Nt_traits>
Traits;
typedef Traits::Point_2                               Point;
typedef Traits::Curve_2                               Conic_arc;
typedef Traits::X_monotone_curve_2                    X_monotone_conic_arc;
typedef CGAL::Arrangement_2<Traits>                   Arrangement;

// exact
typedef CGAL::Polygon_2<Rat_kernel>             Polygon_2;
typedef CGAL::Gps_traits_2<Traits>              Gps_traits;
typedef Gps_traits::Polygon_with_holes_2        Offset_polygon_with_holes_2;

// approximate
/*#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/General_polygon_set_2.h>
#include <boost/timer.hpp>
#include <CGAL/basic.h>
#include <CGAL/approximated_offset_2.h>*/

// approximate
/*typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Gps_circle_segment_traits_2<Kernel>         Traits;

// approximate
typedef CGAL::General_polygon_set_2<Traits>               Polygon_set_2;
typedef Traits::Polygon_2                                 Polygon_2;
typedef Traits::Polygon_with_holes_2                      Polygon_with_holes_2;
typedef Kernel::Point_2                                   Point;
typedef CGAL::Polygon_2<Kernel>                           Linear_polygon;*/

//std::vector<AVector> CGALOffsetter::OffsettingP(std::vector<AVector> somePoly, float offsetVal)
//{
//	// exact version
//	std::vector<Rat_point> points; // change Rat_point to Point 
//	for (int a = 0; a < somePoly.size(); a++)
//		{ points.push_back(Rat_point(somePoly[a].x, somePoly[a].y)); }
//
//	Polygon_2  P(points.begin(), points.end());
//	Traits traits;
//	Offset_polygon_with_holes_2 offset = CGAL::offset_polygon_2(P, 5, traits);
//
//	std::vector<AVector> offsetPoly;	
//
//	typedef Offset_polygon_with_holes_2::Polygon_2::Curve_const_iterator  PIter;
//	for (PIter iter = offset.outer_boundary().curves_begin(); iter != offset.outer_boundary().curves_end(); iter++)
//	{
//		float x = CGAL::to_double(iter->left().x());
//		float y = CGAL::to_double(iter->left().y());
//		offsetPoly.push_back(AVector(x, y));
//	}
//	std::cout << "done\n";
//
//	return offsetPoly;
//
//	// this one is approximate version
//
//	/*Linear_polygon P(points.begin(), points.end());
//	Polygon_with_holes_2 offset = CGAL::approximated_offset_2(P, 5, 0.00001);
//	std::vector<AVector> offsetPoly;
//	typedef Polygon_2::Curve_const_iterator  PIter;
//	for (PIter iter = offset.outer_boundary().curves_begin(); iter != offset.outer_boundary().curves_end(); iter++)
//	{
//		float x = CGAL::to_double(iter->left().x());
//		float y = CGAL::to_double(iter->left().y());
//		offsetPoly.push_back(AVector(x, y));
//	}
//	return offsetPoly;*/
//}