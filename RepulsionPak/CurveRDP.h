
/* ---------- ShapeRadiusMatching V2  ---------- */

#ifndef __Curve_RDP__
#define __Curve_RDP__

#include <vector>

#include "AVector.h"

//namespace CVSystem
//{
// class for Ramer-Douglas-Peucker
class CurveRDP
{
public:
	// distance a point to a line
	static float PerpendicularDistance(AVector p, AVector p1, AVector p2);

	// RDP flags
	// don't use this...
	static void RDPFlags(std::vector<bool>& pointFlags, std::vector<AVector> points, double epsilon, int startIndex, int endIndex, double rdp_point_min);
	
	
	static void SimplifyRDPFlags(std::vector<AVector>& oldCurves, std::vector<bool>& curves_flags, float epsilon);
	static void SimplifyRDPFlagsRecursive(std::vector<AVector>& oldCurves, std::vector<bool>& curves_flags, float epsilon, int startIndex, int endIndex);

	// RDP partitioning
	static void SimplifyRDP(std::vector<AVector>& oldCurves, std::vector<AVector>& newCurves, float epsilon);

	static void SimplifyRDP(std::vector<std::vector<AVector>>& oldPolys, std::vector<std::vector<AVector>>& newPolys, float epsilon);

	// RDP partitioning
	static void SimplifyRDPRecursive(std::vector<AVector>& oldCurves, std::vector<AVector>& newCurves, float epsilon, int startIndex, int endIndex);
};
//}

#endif