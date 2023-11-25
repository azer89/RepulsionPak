#ifndef ___Bezier_Smoothing___
#define ___Bezier_Smoothing___

#include <vector>

#include "AVector.h"
#include "AQuad.h"



class BezierSmoothing
{
public:
	static std::vector<AVector> SmoothingRDP(std::vector<AVector> poly);
	static std::vector<AVector> SmoothingRDPB(std::vector<AVector> poly);
	static void RDPSimplification(std::vector<AVector> poly, std::vector<AVector>& rdpPoly, std::vector<bool>& bFlag);
	static std::vector<AQuad> GetBezierQuadLS(std::vector<AVector> poly, std::vector<bool> rdpFlags);
	static void DeCasteljau(std::vector<AVector>& poly, AVector p0, AVector p1, AVector p2, AVector p3, double subdivide_limit);

};

#endif