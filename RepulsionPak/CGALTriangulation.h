
// ---------- ShapeRadiusMatching V2  ----------

#ifndef __CGAL_Triangulation_h__
#define __CGAL_Triangulation_h__

#include "AVector.h"
#include "ALine.h"
#include "AnIdxTriangle.h"

#include <vector>

class CGALTriangulation
{
public:
	CGALTriangulation();
	~CGALTriangulation();

	/*std::vector<ALine>*/ 
	void Triangulate(std::vector<AVector> boundary, std::vector<AVector>& vertices, std::vector<AnIdxTriangle>& triangles);
};

#endif