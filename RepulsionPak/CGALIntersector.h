
#ifndef __CGAL_Intersector__
#define __CGAL_Intersector__

#include "AVector.h"
#include "ALine.h"
#include <vector>

class CGALIntersector
{
public:
	static AVector FindTheClosestIntersection(ALine line, std::vector<AVector> shape);
};
#endif