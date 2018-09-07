
/* ---------- ShapeRadiusMatching V2  ---------- */

#ifndef __Self_Intersection_Fixer_h__
#define __Self_Intersection_Fixer_h__

#include "AVector.h"
#include <vector>

class SelfIntersectionFixer
{
public:
	SelfIntersectionFixer();
	~SelfIntersectionFixer();

	bool IsSimple(std::vector<AVector> poly);

	void FixSelfIntersection1(std::vector<AVector> oldPoly, std::vector<AVector>& newPoly);

	// newPoly will be the largest subset of the old poly
	//bool FixSelfIntersection1(std::vector<AVector> oldPoly, std::vector<AVector>& newPoly);

private:

};



#endif