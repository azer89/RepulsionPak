
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