
#ifndef __CGAL_Triangulation_2_h__
#define __CGAL_Triangulation_2_h__

#include "AVector.h"
#include "ALine.h"
#include "AnIdxTriangle.h"

#include <vector>

class CGALTriangulation2
{
public:
	CGALTriangulation2();
	~CGALTriangulation2();

	void Triangulate(std::vector<AVector> boundary, std::vector<AnIdxTriangle>& triangles);
};

#endif