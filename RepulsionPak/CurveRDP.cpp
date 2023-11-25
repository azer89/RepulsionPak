
/* ---------- ShapeRadiusMatching V2  ---------- */

/**
*
* Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
* Version: 2014
*
*/

#include "CurveRDP.h"
#include <limits>

float CurveRDP::PerpendicularDistance(AVector p, AVector p1, AVector p2)
{
	// if start and end point are on the same x the distance is the difference in X.
	float result;
	if (abs(p1.x - p2.x) < std::numeric_limits<float>::epsilon()) { result = abs(p.x - p1.x); }
	else
	{
		float slope = (p2.y - p1.y) / (p2.x - p1.x);
		float intercept = p1.y - (slope * p1.x);
		result = abs(slope * p.x - p.y + intercept) / sqrt(pow(slope, 2) + 1);
	}

	return result;
}

void CurveRDP::SimplifyRDPFlags(std::vector<AVector>& oldCurves, std::vector<bool>& curves_flags, float epsilon)
{
	for (unsigned int a = 0; a < oldCurves.size(); a++)
	{
		curves_flags[a] = false;
	}
	curves_flags[0] = true;
	SimplifyRDPFlagsRecursive(oldCurves, curves_flags, epsilon, 0, oldCurves.size() - 1);
	curves_flags[curves_flags.size() - 1] = false;
}



void CurveRDP::SimplifyRDP(std::vector<AVector>& oldCurves, std::vector<AVector>& newCurves, float epsilon)
{
	newCurves.clear();
	newCurves.push_back(oldCurves[0]);
	SimplifyRDPRecursive(oldCurves, newCurves, epsilon, 0, oldCurves.size() - 1);
	newCurves.push_back(oldCurves[oldCurves.size() - 1]);
}

void CurveRDP::SimplifyRDP(std::vector<std::vector<AVector>>& oldPolys, std::vector<std::vector<AVector>>& newPolys, float epsilon)
{
	for (int a = 0; a < oldPolys.size(); a++)
	{
		std::vector<AVector> newPoly;
		SimplifyRDP(oldPolys[a], newPoly, epsilon);
		newPolys.push_back(newPoly);
	}
}

void CurveRDP::SimplifyRDPFlagsRecursive(std::vector<AVector>& oldCurves, std::vector<bool>& curves_flags, float epsilon, int startIndex, int endIndex)
{
	AVector firstPoint = oldCurves[startIndex];
	AVector lastPoint = oldCurves[endIndex];

	int index = -1;
	float dist = std::numeric_limits<float>::min();
	for (int i = startIndex + 1; i < endIndex; i++)
	{
		float cDist = PerpendicularDistance(oldCurves[i], firstPoint, lastPoint);
		if (cDist > dist)
		{
			dist = cDist;
			index = i;
		}
	}

	if (index != -1 && dist > epsilon)
	{
		SimplifyRDPFlagsRecursive(oldCurves, curves_flags, epsilon, startIndex, index);
		curves_flags[index] = true;
		//newCurves.push_back(oldCurves[index]);
		SimplifyRDPFlagsRecursive(oldCurves, curves_flags, epsilon, index, endIndex);
	}
}

void CurveRDP::SimplifyRDPRecursive(std::vector<AVector>& oldCurves, std::vector<AVector>& newCurves, float epsilon, int startIndex, int endIndex)
{
	AVector firstPoint = oldCurves[startIndex];
	AVector lastPoint = oldCurves[endIndex];

	int index = -1;
	float dist = std::numeric_limits<float>::min();
	for (int i = startIndex + 1; i < endIndex; i++)
	{
		float cDist = PerpendicularDistance(oldCurves[i], firstPoint, lastPoint);
		if (cDist > dist)
		{
			dist = cDist;
			index = i;
		}
	}

	if (index != -1 && dist > epsilon)
	{
		SimplifyRDPRecursive(oldCurves, newCurves, epsilon, startIndex, index);
		newCurves.push_back(oldCurves[index]);
		SimplifyRDPRecursive(oldCurves, newCurves, epsilon, index, endIndex);
	}
}

void CurveRDP::RDPFlags(std::vector<bool>& flags,
	std::vector<AVector> points,
	double epsilon,
	int startIndex,
	int endIndex,
	double rdp_point_min)
{
	//if (endIndex - startIndex < rdp_point_min)
	//	{ return; }

	AVector firstPoint = points[startIndex];
	AVector lastPoint = points[endIndex];

	int index = -1;
	double dist = DBL_MIN;
	for (int i = startIndex + 1; i < endIndex; i++)
	{
		double cDist = PerpendicularDistance(points[i], firstPoint, lastPoint);
		if (cDist > dist)
		{
			dist = cDist;
			index = i;
		}
	}

	if (dist > epsilon)
	{
		if (index - startIndex >= rdp_point_min && endIndex - index >= rdp_point_min)
		{
			// Todo: wrong recursive sequence
			flags[index] = true;
			RDPFlags(flags, points, epsilon, startIndex, index, rdp_point_min);
			RDPFlags(flags, points, epsilon, index, endIndex, rdp_point_min);
		}

		//flags[index] = true;
		//RDP(flags, points, epsilon, startIndex, index, rdp_point_min);
		//RDP(flags, points, epsilon, index, endIndex  , rdp_point_min);
	}
}