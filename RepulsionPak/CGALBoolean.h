
#ifndef __CGAL_Boolean__
#define __CGAL_Boolean__

#include "AVector.h"
#include <vector>

class CGALBoolean
{
public:
	static std::vector<AVector> UnionOperationWithGrowing(std::vector<std::vector<AVector>>& polys);
};
#endif