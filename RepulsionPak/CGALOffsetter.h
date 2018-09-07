
#ifndef __CGAL_Offsetter__
#define __CGAL_Offsetter__

#include "AVector.h"
#include <vector>

class CGALOffsetter
{
public:
	// https_://github.com/CGAL/cgal/blob/master/Minkowski_sum_2/examples/Minkowski_sum_2/bops_circular.h
	// http_://doc.cgal.org/latest/Minkowski_sum_2/index.html#Chapter_2D_Minkowski_Sums
	static std::vector<AVector> OffsettingP(std::vector<AVector> somePoly, float offsetVal);
};

#endif