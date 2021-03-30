
/* ---------- RepulsionPak  ---------- */

#include "UtilityFUnctions.h"

#include "AVector.h"
#include "ALine.h"
#include "ARectangle.h"

#include <sstream>

#define PI 3.14159265359
#define PI2 6.28318530718

bool UtilityFunctions::IsLeft(AVector a, AVector b, AVector c)
{
	return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}


// https_://www.gamedev.net/topic/295943-is-this-a-better-point-in-triangle-test-2d/
float UtilityFunctions::TSign(AVector p1, AVector p2, AVector p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

// https_://www.gamedev.net/topic/295943-is-this-a-better-point-in-triangle-test-2d/
bool UtilityFunctions::PointInTriangle(AVector A, AVector B, AVector C, AVector P)
{
	bool b1, b2, b3;

	b1 = TSign(P, A, B) > 0.0f;
	b2 = TSign(P, B, C) > 0.0f;
	b3 = TSign(P, C, A) > 0.0f;

	return ((b1 == b2) && (b2 == b3));

	//bool b0 = (AVector(P.x - A.x, P.y - A.y) * AVector(A.y - B.y, B.x - A.x) > 0);
	//bool b1 = (AVector(P.x - B.x, P.y - B.y) * AVector(B.y - C.y, C.x - B.x) > 0);
	//bool b2 = (AVector(P.x - C.x, P.y - C.y) * AVector(C.y - A.y, A.x - C.x) > 0);
	//return (b0 == b1 && b1 == b2);
}


/*================================================================================
================================================================================*/
void UtilityFunctions::GetLeftRightPoints(AVector p1, AVector p2, float offset, AVector* pA, AVector* pB)
{
	AVector dir = (p2 - p1).Norm();
	dir *= offset;

	// because we have inverted y-axis...
	AVector dLeft(-dir.y, dir.x);  // this is right
	AVector dRIght(dir.y, -dir.x); // this is left

	*pA = p1 + dLeft;
	*pB = p1 + dRIght;
}

/*================================================================================
================================================================================*/
void UtilityFunctions::GetLeftRightDirections(AVector p1, AVector p2, AVector* dirA, AVector* dirB)
{
	AVector dir = (p2 - p1).Norm();

	*dirA = AVector(-dir.y, dir.x);
	*dirB = AVector(dir.y, -dir.x);
}

/*================================================================================
Don't use this, this has a bug!
================================================================================*/
float UtilityFunctions::AreaOfPolygon(const std::vector<AVector>& polyline)
{
	std::cout << "FUCK FUCKING FUCK! DONT USE THIS\n";
	float area = 0;
	/*polyline.push_back(polyline[0]);	
	for (unsigned int i = 0; i < polyline.size() - 1; i++)
	{
		area += std::abs((polyline[i].x * polyline[i + 1].y) - (polyline[i + 1].x * polyline[i].y));
	}
	area /= 2.0f;*/

	return area;
}

/*================================================================================
================================================================================*/
float UtilityFunctions::DistanceToFiniteLine(AVector v, AVector w, AVector p)
{
	float machine_eps = std::numeric_limits<float>::epsilon();

	// Return minimum distance between line segment vw and point p
	float l2 = v.DistanceSquared(w);					   // i.e. |w-v|^2 -  avoid a sqrt
	if (l2 > -machine_eps && l2 < machine_eps) return p.Distance(v);   // v == w case

	// Consider the line extending the segment, parameterized as v + t (w - v).
	// We find projection of point p onto the line. 
	// It falls where t = [(p-v) . (w-v)] / |w-v|^2
	float t = (p - v).Dot(w - v) / l2;

	if (t < 0.0)	  { return  p.Distance(v); }  // Beyond the 'v' end of the segment
	else if (t > 1.0) { return  p.Distance(w); }  // Beyond the 'w' end of the segment
	AVector projection = v + (w - v) * t;         // Projection falls on the segment
	return p.Distance(projection);
}

float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;                       // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck? 
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
										   //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}



/*================================================================================
================================================================================*/
AVector UtilityFunctions::ClosestPtAtFiniteLine(AVector v, AVector w, AVector p)
{
	//float machine_eps = std::numeric_limits<float>::epsilon();
	float machine_eps = 0.0001;
	float l2 = v.Distance(w);	
	if (l2 > -machine_eps && l2 < machine_eps) { return v; } 
	float t = (p - v).Dot(w - v) / l2;
	
	//float inv_l2 = Q_rsqrt(v.DistanceSquared(w));
	//if (inv_l2 > 100000) { return v; }
	//float t = (p - v).Dot(w - v) * inv_l2;


	if (t < 0.0)	  { return  v; }
	else if (t > 1.0) { return  w; } 
	return v + (w - v) * t;  // projection
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::GetPointInterpolationFromStart(std::vector<AVector> curve, float val)
{
	float curveLength = CurveLength(curve);
	float interpLength = val * curveLength;

	float err = 1e-2; // I don't like this
	if (interpLength < err) { return curve[0]; }
	if (std::abs(interpLength - curveLength) < err) { return curve[curve.size() - 1]; }

	float sumLength = 0.0f;
	for (unsigned int a = 1; a < curve.size(); a++)
	{
		AVector prevPoint = curve[a - 1];
		AVector curPoint = curve[a];
		float dist = prevPoint.Distance(curPoint);
		AVector dir = (curPoint - prevPoint).Norm();

		// check whether it falls in between
		float diff = (sumLength + dist) - interpLength;

		if (diff > 0) { return prevPoint + dir * (dist - diff); }
		else { sumLength += dist; }
	}
	std::cout << "can't find\n";
	return AVector(); // can't find
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::GetIndicesInBetween(std::vector<AVector> curve, float val, int& index1, int& index2)
{
	float curveLength = CurveLength(curve);
	float interpLength = val * curveLength;

	float err = 1e-2; // I don't like this
	if (interpLength < err)
	{
		index1 = 0;
		index2 = 1;
		return;
	}
	if (std::abs(interpLength - curveLength) < err)
	{
		index1 = curve.size() - 2;
		index2 = curve.size() - 1;
		return;
	}

	float sumLength = 0.0f;
	for (unsigned int a = 1; a < curve.size(); a++)
	{
		AVector prevPoint = curve[a - 1];
		AVector curPoint = curve[a];
		float dist = prevPoint.Distance(curPoint);
		AVector dir = (curPoint - prevPoint).Norm();

		// check whether it falls in between
		float diff = (sumLength + dist) - interpLength;
		if (diff > 0)
		{
			index1 = a - 1;
			index2 = a;
			return;
		}
		else { sumLength += dist; }
	}
	std::cout << "can't find\n";
}

/*
================================================================================
================================================================================
*/
std::vector<AVector> UtilityFunctions::GetCurveSubset(std::vector<AVector> curve, int N, float startVal, float endVal)
{
	// doesn't work ???
	//float eps = std::numeric_limits<float>::epsilon();
	//if (startVal < eps && endVal < eps) { return curve; }

	float curveLength = CurveLength(curve);
	float frontLength = startVal * curveLength;
	float backLength = endVal * curveLength;

	// cut front
	std::reverse(curve.begin(), curve.end()); // reverse, iterate backward
	float sumLength = 0.0f;
	for (int a = curve.size() - 1; a > 0; a--)
	{
		AVector leftPoint = curve[a - 1];            // previous point
		AVector curPoint = curve[a];                 // current point
		float dist = curPoint.Distance(leftPoint);   // distance to the left
		AVector dir = (leftPoint - curPoint).Norm(); // direction to the left

		// check whether it falls in between
		float diff = (sumLength + dist) - frontLength;

		curve.pop_back(); // always pop the current point
		if (diff > 0)
		{
			curve.push_back(curPoint + dir * (dist - diff));
			break;
		}
		else
		{
			sumLength += dist;
		}
	}
	std::reverse(curve.begin(), curve.end());

	// cut back (code duplication)
	sumLength = 0.0f;
	for (int a = curve.size() - 1; a > 0; a--)
	{
		AVector leftPoint = curve[a - 1];            // previous point
		AVector curPoint = curve[a];                 // current point
		float dist = curPoint.Distance(leftPoint);   // distance to the left
		AVector dir = (leftPoint - curPoint).Norm(); // direction to the left

		// check whether it falls in between
		float diff = (sumLength + dist) - backLength;

		curve.pop_back(); // always pop the current point
		if (diff > 0)
		{
			curve.push_back(curPoint + dir * (dist - diff));
			break;
		}
		else
		{
			sumLength += dist;
		}
	}

	std::vector<AVector> resampled;
	UniformResample(curve, resampled, N);

	return resampled;
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToClosedCurve(std::vector<AVector> polyline, AVector p)
{
	polyline.push_back(polyline[0]); // because loop
	return UtilityFunctions::DistanceToPolyline(polyline, p);
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToClosedCurves(const std::vector<std::vector<AVector>>& polylines, AVector p)
{
	float minDist = std::numeric_limits<float>::max();
	for (unsigned int a = 0; a < polylines.size(); a++)
	{
		float d = UtilityFunctions::DistanceToClosedCurve(polylines[a], p);
		if (d < minDist)
		{
			minDist = d;
		}
	}
	return minDist;
}

/*================================================================================
paulbourke.net/geometry/pointlineplane/
================================================================================*/
inline float DistSquared(const AVector& p, const AVector& other)
{
	float xDist = p.x - other.x;
	float yDist = p.y - other.y;
	return xDist * xDist + yDist * yDist;
}


AVector UtilityFunctions::ClosestPtAtFiniteLine2(const AVector& lnStart, const AVector& lnEnd, const AVector& pt)
{
	//float LineMag;
	//float U;
	//XYZ Intersection;

	float dx = lnEnd.x - lnStart.x;
	float dy = lnEnd.y - lnStart.y;

	float lineMagSq = DistSquared(lnStart, lnEnd); //lnStart.DistanceSquared(lnEnd);

	float u = (((pt.x - lnStart.x) * dx) +
		((pt.y - lnStart.y) * dy)) /
		lineMagSq;

	if (u < 0.0f) { return lnStart; }
	else if (u > 1.0f) { return lnEnd; }

	return AVector(lnStart.x + u * dx, lnStart.y + u * dy);
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::GetClosestPtOnClosedCurve(const std::vector<AVector>& polyline, const AVector& p)
{
	float dist = 10000000000;
	AVector closestPt;
	AVector pt;
	float d;
	int p_size = polyline.size();
	for (unsigned int a = 1; a < p_size; a++)
	{
		pt = ClosestPtAtFiniteLine2(polyline[a - 1], polyline[a], p);
		d = DistSquared(p, pt); // p.DistanceSquared(pt);
		if (d < dist)
		{
			dist = d;
			closestPt = pt;
		}
	}

	{ // first and last point
		pt = ClosestPtAtFiniteLine2(polyline[p_size - 1], polyline[0], p);
		d = DistSquared(p, pt); //p.DistanceSquared(pt);
		if (d < dist)
		{
			dist = d;
			closestPt = pt;
		}
	}

	return closestPt;
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToPolyline(const std::vector<AVector>& polyline, AVector p)
{
	float dist = std::numeric_limits<float>::max();
	for (unsigned int a = 1; a < polyline.size(); a++)
	{
		float d = DistanceToFiniteLine(polyline[a - 1], polyline[a], p);
		if (d < dist) { dist = d; }
	}
	return dist;
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToPolylines(const std::vector<std::vector<AVector>>& polylines, AVector p)
{
	float dist = std::numeric_limits<float>::max();

	for (unsigned int a = 0; a < polylines.size(); a++)
	{
		float d = DistanceToPolyline(polylines[a], p);
		if (d < dist) { dist = d; }
	}

	return dist;
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToBunchOfPoints(const std::vector<AVector>& points, AVector p)
{
	float dist = std::numeric_limits<float>::max();
	for (unsigned int b = 0; b < points.size(); b++)
	{
		float d = p.DistanceSquared(points[b]);
		if (d < dist) { dist = d; }
	}
	return std::sqrt(dist);
}

/*================================================================================
================================================================================*/
AVector UtilityFunctions::ClosestPoint(const std::vector<AVector>& points, AVector p)
{
	AVector retPt;
	float dist = std::numeric_limits<float>::max();
	for (unsigned int a = 0; a < points.size(); a++)
	{
		float d = p.DistanceSquared(points[a]); // squared
		if (d < dist)
		{
			dist = d;
			retPt = points[a];
		}
	}
	return retPt;
}

/*
================================================================================
================================================================================
*/
float UtilityFunctions::DistanceToBunchOfPoints(const std::vector<std::vector<AVector>>& points, AVector p)
{
	float dist = std::numeric_limits<float>::max();
	for (unsigned int a = 0; a < points.size(); a++)
	{
		for (unsigned int b = 0; b < points[a].size(); b++)
		{
			float d = p.DistanceSquared(points[a][b]);
			if (d < dist) { dist = d; }
		}
	}
	return std::sqrt(dist);
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::GetClosestPtOnPolyline(const std::vector<AVector>& polyline, AVector p)
{
	float dist = std::numeric_limits<float>::max();
	AVector closestPt;
	for (unsigned int a = 1; a < polyline.size(); a++)
	{
		//float d = DistanceToFiniteLine(polyline[a - 1], polyline[a], p);
		AVector pt = ClosestPtAtFiniteLine(polyline[a - 1], polyline[a], p);
		float d = p.DistanceSquared(pt); // squared
		if (d < dist) 
		{ 
			dist = d; 
			closestPt = pt;
		}
	}
	return closestPt;
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::GetListsOfLeftRightNormals(const std::vector<AVector>& curve, std::vector<AVector>& lDirs, std::vector<AVector>& rDirs)
{
	for (unsigned int a = 0; a < curve.size(); a++)
	{
		AVector lDir;  AVector rDir;
		if (a != 0 && a < curve.size() - 1) { UtilityFunctions::GetLeftRightNormals(curve[a - 1], curve[a], curve[a + 1], &lDir, &rDir); }
		else if (a == 0) { UtilityFunctions::GetLeftRightNormals(AVector(), curve[a], curve[a + 1], &lDir, &rDir); }
		else if (a == curve.size() - 1) { UtilityFunctions::GetLeftRightNormals(curve[a - 1], curve[a], AVector(), &lDir, &rDir); }
		lDirs.push_back(lDir);
		rDirs.push_back(rDir);
	}
}

/*================================================================================
================================================================================*/
std::vector<AVector> UtilityFunctions::TrimPoly(const std::vector<AVector>& poly, const std::vector<std::vector<AVector>>& shapes)
{
	//std::cout << "poly size before trimming: " << poly.size() << "\n";

	std::vector<AVector> longestPoly;
	int longestSz = 0;
	int sz = poly.size();
	for (unsigned int a = 0; a < sz - 1; a++)
	{
		if (InsidePolygons(shapes, poly[a].x, poly[a].y)) { continue; }
		std::vector<AVector> candidatePoly;
		candidatePoly.push_back(poly[a]);
		for (unsigned int b = a + 1; b < sz; b++)
		{
			if (InsidePolygons(shapes, poly[b].x, poly[b].y)) { break; }
			candidatePoly.push_back(poly[b]);
		}
		if (candidatePoly.size() > longestSz)
		{
			longestSz = candidatePoly.size();
			longestPoly = candidatePoly;

		}
	}
	if (longestPoly.size() < 2)
	{
		//longestPoly = poly;
	}
	std::cout << "poly size after trimming: " << longestPoly.size() << "\n";
	return longestPoly;
}

/*================================================================================
================================================================================*/
std::vector<AVector> UtilityFunctions::GetInterPolatedLRDirs(float normDist, std::vector<AVector> lDirs, std::vector<AVector> rDirs, int functionLength)
{
	std::vector<AVector> dirs(2);

	float sz = functionLength;
	int index1 = std::floor(sz * normDist);
	int index2 = std::ceil(sz * normDist);

	if (index1 < 0 || index2 < 0)
	{
		dirs[0] = lDirs[0];
		dirs[1] = rDirs[0];
		return dirs;
	}
	else if (index1 >= functionLength || index2 >= functionLength)
	{
		dirs[0] = lDirs[functionLength - 1];
		dirs[1] = rDirs[functionLength - 1];
		return dirs;
	}

	AVector lDir1 = lDirs[index1];
	AVector lDir2 = lDirs[index2];

	AVector rDir1 = rDirs[index1];
	AVector rDir2 = rDirs[index2];

	dirs[0] = (lDir1 + lDir2) / 2.0f;
	dirs[1] = (rDir1 + rDir2) / 2.0f;

	return dirs;
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::GetLeftRightNormals(AVector prev, AVector cur, AVector next, AVector* leftNormal, AVector* rightNormal)
{
	if (!prev.IsInvalid() && !next.IsInvalid())
	{
		AVector dir1 = (cur - prev).Norm();
		AVector dir2 = (next - cur).Norm();
		AVector dir3 = (dir1 + dir2) / 2.0f;

		*leftNormal = AVector(-dir3.y, dir3.x); // left
		*rightNormal = AVector(dir3.y, -dir3.x); // right
	}
	else if (prev.IsInvalid())
	{
		AVector dir = (next - cur).Norm();
		*leftNormal = AVector(-dir.y, dir.x); // left
		*rightNormal = AVector(dir.y, -dir.x); // right
	}
	else if (next.IsInvalid())
	{
		AVector dir = (cur - prev).Norm();
		*leftNormal = AVector(-dir.y, dir.x); // left
		*rightNormal = AVector(dir.y, -dir.x); // right
	}
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::GetLeftNormals(AVector prev, AVector cur, AVector next, AVector* leftNormal)
{
	if (!prev.IsInvalid() && !next.IsInvalid())
	{
		AVector dir1 = (cur - prev).Norm();
		AVector dir2 = (next - cur).Norm();
		AVector dir3 = (dir1 + dir2) / 2.0f;

		*leftNormal = AVector(-dir3.y, dir3.x); // left
	}
	else if (prev.IsInvalid())
	{
		AVector dir = (next - cur).Norm();
		*leftNormal = AVector(-dir.y, dir.x); // left
	}
	else if (next.IsInvalid())
	{
		AVector dir = (cur - prev).Norm();
		*leftNormal = AVector(-dir.y, dir.x); // left
	}
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::GetRightNormals(AVector prev, AVector cur, AVector next, AVector* rightNormal)
{
	if (!prev.IsInvalid() && !next.IsInvalid())
	{
		AVector dir1 = (cur - prev).Norm();
		AVector dir2 = (next - cur).Norm();
		AVector dir3 = (dir1 + dir2) / 2.0f;
		*rightNormal = AVector(dir3.y, -dir3.x); // right
	}
	else if (prev.IsInvalid())
	{
		AVector dir = (next - cur).Norm();
		*rightNormal = AVector(dir.y, -dir.x); // right
	}
	else if (next.IsInvalid())
	{
		AVector dir = (cur - prev).Norm();
		*rightNormal = AVector(dir.y, -dir.x); // right
	}
}

/*ARectangle UtilityFunctions::GetBoundingBox(std::vector<AVector> contour)
{
	float xMin = std::numeric_limits<float>::max();
	float xMax = std::numeric_limits<float>::min();
	float yMin = std::numeric_limits<float>::max();
	float yMax = std::numeric_limits<float>::min();

	for (int a = 0; a < contour.size(); a++)
	{
		AVector pt = contour[a];

		if (pt.x > xMax) { xMax = pt.x; }
		if (pt.x < xMin) { xMin = pt.x; }
		if (pt.y > yMax) { yMax = pt.y; }
		if (pt.y < yMin) { yMin = pt.y; }
	}

	ARectangle bb(AVector(xMin, yMin), xMax - xMin, yMax - yMin);
	return bb;
}*/

float UtilityFunctions::CurveLengthClosed(std::vector<AVector> curves)
{
	curves.push_back(curves[curves.size() - 1]);
	return CurveLength(curves);
}

/*
================================================================================
================================================================================
*/
// Todo: implement closed curves
float UtilityFunctions::CurveLength(std::vector<AVector> curves)
{
	float length = 0.0;
	for (size_t a = 1; a < curves.size(); a++) { length += curves[a].Distance(curves[a - 1]); }
	return length;
}

/*
================================================================================
================================================================================
*/
bool UtilityFunctions::InsidePolygons(const std::vector<std::vector<AVector>>& polygons, float px, float py)
{
	for (unsigned int a = 0; a < polygons.size(); a++)
	{
		if (InsidePolygon(polygons[a], px, py)) { return true; }
	}
	return false;
}

/*
================================================================================
================================================================================
*/
bool UtilityFunctions::InsidePolygon(const std::vector<AVector>& polygon, float px, float py)
{
	// http_:_//alienryderflex_._com/polygon/
	int poly_sz = polygon.size();
	
	bool  oddNodes = false;

	unsigned int   i;
	unsigned int j = poly_sz - 1;
	for (i = 0; i < poly_sz; i++)
	{
		if ((polygon[i].y < py && polygon[j].y >= py ||
			polygon[j].y < py && polygon[i].y >= py)
			&& (polygon[i].x <= px || polygon[j].x <= px))
		{
			oddNodes ^= (polygon[i].x + (py - polygon[i].y) / (polygon[j].y - polygon[i].y) * (polygon[j].x - polygon[i].x) < px);
		}
		j = i;
	}

	return oddNodes;
}


bool UtilityFunctions::IsClockwise(std::vector<AVector> polygon)
{
	double sumValue = 0;
	for (size_t a = 0; a < polygon.size(); a++)
	{
		AVector curPt = polygon[a];
		AVector nextPt;
		if (a == polygon.size() - 1)
		{
			nextPt = polygon[0];
		}
		else
		{
			nextPt = polygon[a + 1];
		}

		// sumValue += ((x2 - x1) * (y2 + y1));
		sumValue += ((nextPt.x - curPt.x) * (nextPt.y + curPt.y));
	}

	if (sumValue >= 0) { return false; }

	return true;
}

/*
================================================================================
Return the angle between two vectors on a plane
The angle is from vector 1 to vector 2, positive anticlockwise
The result is between -pi -> pi
================================================================================
*/
float UtilityFunctions::Angle2D(float x1, float y1, float x2, float y2)
{
	// atan2(vector.y, vector.x) = the angle between the vector and the X axis

	float dtheta, theta1, theta2;

	theta1 = atan2(y1, x1);
	theta2 = atan2(y2, x2);
	dtheta = theta2 - theta1;

	while (dtheta > PI)
	{
		dtheta -= PI2;
	}

	while (dtheta < -PI)
	{
		dtheta += PI2;
	}

	return dtheta;
}

void UtilityFunctions::UniformResampleWithConvexHull(std::vector<AVector> oriCurve, 
	                                                 std::vector<AVector> convexHull, 
													 std::vector<AVector>& resampleCurve, 
													 float resampleGap)
{
	float eps = 1e-5;
	std::vector<int> hullIndices;
	for (int a = 0; a < oriCurve.size(); a++)
	{
		if (UtilityFunctions::DistanceToBunchOfPoints(convexHull, oriCurve[a]) < eps)
			{ hullIndices.push_back(a); }
	}

	
	for (int a = 0; a < hullIndices.size(); a++)
	{
		// make loop
		int pairA = hullIndices[a];
		int pairB;
		if (a == hullIndices.size() - 1) { pairB = hullIndices[0]; }
		else { pairB = hullIndices[a + 1]; }

		// need to add the first one
		//if (a == 0) { resampleCurve.push_back(oriCurve[pairA]); }

		// if less than gap
		float dist = oriCurve[pairA].Distance(oriCurve[pairB]);
		if (dist < resampleGap)
		{
			resampleCurve.push_back(oriCurve[pairB]);
			continue;
		}

		// subset
		//std::vector<AVector> subsetCurve(oriCurve.begin() + pairA, oriCurve.begin() + pairB);
		std::vector<AVector> subsetCurve;
		if (pairA > pairB)
		{
			subsetCurve.insert(subsetCurve.end(), oriCurve.begin() + pairB, oriCurve.end());
			subsetCurve.insert(subsetCurve.end(), oriCurve.begin(), oriCurve.begin() + pairA);
		}
		else
		{
			subsetCurve.insert(subsetCurve.end(), oriCurve.begin() + pairA, oriCurve.begin() + pairB);
		}
		
		//std::cout << "\n\npair A B " << pairA << " " << pairB << "\n";
		//std::cout << "dist " << dist << "\n";
		//std::cout << "gap  " << resampleGap << "\n";
		std::vector<AVector> rCurve;
		UtilityFunctions::UniformResampleNoFirstPoint(subsetCurve, rCurve, resampleGap);
		resampleCurve.insert(resampleCurve.end(), rCurve.begin(), rCurve.end());

		std::cout << "resample size " << resampleCurve.size() << "\n";
		std::cout << "\n";
	}
}

void UtilityFunctions::UniformResampleNoFirstPoint(std::vector<AVector> oriCurve, 
	                                               std::vector<AVector>& resampleCurve, 
												   float resampleGap)
{
	resampleCurve.clear();
	//std::cout << "oriCurve size " << oriCurve.size() << "\n";
	float curveLength = CurveLength(oriCurve);

	int segmentNum = (int)(std::round(curveLength / resampleGap)); // rounding
	resampleGap = curveLength / (float)segmentNum;

	int iter = 0;
	double sumDist = 0.0;
	//resampleCurve.push_back(oriCurve[0]); // NoFirstPoint
	while (iter < oriCurve.size() - 1)
	{
		double currentDist = oriCurve[iter].Distance(oriCurve[iter + 1]);
		sumDist += currentDist;

		if (sumDist > resampleGap)
		{
			double vectorLength = currentDist - (sumDist - resampleGap);
			AVector pt1 = oriCurve[iter];
			AVector pt2 = oriCurve[iter + 1];
			AVector directionVector = (pt2 - pt1).Norm();

			AVector newPoint1 = pt1 + directionVector * vectorLength;
			resampleCurve.push_back(newPoint1);

			sumDist = currentDist - vectorLength;

			while (sumDist - resampleGap > 1e-8)
			{
				AVector insertPt2 = resampleCurve[resampleCurve.size() - 1] + directionVector * resampleGap;
				resampleCurve.push_back(insertPt2);
				sumDist -= resampleGap;
			}
		}

		iter++;

	}

	// bug
	if (resampleCurve.size() == 0)
	{
		AVector lastPt = oriCurve[oriCurve.size() - 1];
		resampleCurve.push_back(lastPt);
		return; // end
	}

	//std::cout << "resample size " << resampleCurve.size() << "\n";
	float eps = std::numeric_limits<float>::epsilon();
	AVector lastPt = oriCurve[oriCurve.size() - 1];
	if (resampleCurve[resampleCurve.size() - 1].Distance(lastPt) > (resampleGap - eps)) { resampleCurve.push_back(lastPt); }
}

/*
================================================================================
================================================================================
*/
// only for open curve
void UtilityFunctions::UniformResample(std::vector<AVector> oriCurve, 
	                                   std::vector<AVector>& resampleCurve, 
									   float resampleGap)
{
	resampleCurve.clear();
	float curveLength = CurveLength(oriCurve);

	int segmentNum = (int)(std::round(curveLength / resampleGap)); // rounding
	resampleGap = curveLength / (float)segmentNum;

	int iter = 0;
	float sumDist = 0.0;
	resampleCurve.push_back(oriCurve[0]);
	while (iter < oriCurve.size() - 1)
	{
		float currentDist = oriCurve[iter].Distance(oriCurve[iter + 1]);
		sumDist += currentDist;

		if (sumDist > resampleGap)
		{
			float vectorLength = currentDist - (sumDist - resampleGap);
			AVector pt1 = oriCurve[iter];
			AVector pt2 = oriCurve[iter + 1];
			AVector directionVector = (pt2 - pt1).Norm();

			AVector newPoint1 = pt1 + directionVector * vectorLength;
			resampleCurve.push_back(newPoint1);

			sumDist = currentDist - vectorLength;

			while (sumDist - resampleGap > 1e-8)
			{
				AVector insertPt2 = resampleCurve[resampleCurve.size() - 1] + directionVector * resampleGap;
				resampleCurve.push_back(insertPt2);
				sumDist -= resampleGap;
			}
		}

		iter++;

	}

	

	float eps = std::numeric_limits<float>::epsilon();
	AVector lastPt = oriCurve[oriCurve.size() - 1];
	if (resampleCurve[resampleCurve.size() - 1].Distance(lastPt) > (resampleGap - eps)) { resampleCurve.push_back(lastPt); }

	//resampleCurve.push_back(oriCurve[oriCurve.size() - 1]);
}



/*
================================================================================
================================================================================
*/
void UtilityFunctions::UniformResample(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, int N)
{
	resampleCurve.clear();
	float curveLength = CurveLength(oriCurve);
	float resampleLength = curveLength / (float)(N - 1);

	//int i = 0;
	int iter = 0;
	float sumDist = 0.0;
	resampleCurve.push_back(oriCurve[0]);
	while (resampleCurve.size() < (N - 1))
	{
		float currentDist = oriCurve[iter].Distance(oriCurve[iter + 1]);
		sumDist += currentDist;

		if (sumDist > resampleLength)
		{
			float vectorLength = currentDist - (sumDist - resampleLength);
			AVector pt1 = oriCurve[iter];
			AVector pt2 = oriCurve[iter + 1];
			AVector directionVector = (pt2 - pt1).Norm();

			AVector newPoint1 = pt1 + directionVector * vectorLength;
			resampleCurve.push_back(newPoint1);

			sumDist = currentDist - vectorLength;

			while (sumDist - resampleLength > 1e-8)
			{
				AVector insertPt2 = resampleCurve[resampleCurve.size() - 1] + directionVector * resampleLength;
				resampleCurve.push_back(insertPt2);
				sumDist -= resampleLength;
			}
		}
		iter++;
	}
	resampleCurve.push_back(oriCurve[oriCurve.size() - 1]); // N - 1

	// bug 
	if (resampleCurve.size() > N)
	{
		resampleCurve.pop_back();
	}
}

void UtilityFunctions::UniformResampleClosed(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, float resampleGap)
{
	float l = CurveLengthClosed(oriCurve);
	int N = l / resampleGap;
	if (N < 20)
	{
		resampleCurve = oriCurve;
	}
	else
	{
		UniformResampleClosed(oriCurve, resampleCurve, N);
	}
}

/*
================================================================================
================================================================================
*/
void UtilityFunctions::UniformResampleClosed(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, int N)
{
	// very ugly code
	AVector startPt = oriCurve[0];
	AVector endPt = oriCurve[oriCurve.size() - 1];
	if (startPt.Distance(endPt) > 0.01)
	{
		oriCurve.push_back(oriCurve[0]);
	}
	
	return UtilityFunctions::UniformResample(oriCurve, resampleCurve, N);
}

/*
================================================================================
================================================================================
*/
std::vector<AVector> UtilityFunctions::UniformResampleALine(AVector pt1, AVector pt2, int N)
{
	AVector skelDir = (pt2 - pt1).Norm();
	float skelLength = pt1.Distance(pt2);
	std::vector<AVector> resampledLine;
	float floatDelta = skelLength / (float)N;
	for (float a = 0; a < skelLength; a += floatDelta)
		{ resampledLine.push_back(pt1 + skelDir * a); }

	// todo : fix this
	if (resampledLine.size() > N)
	{
		while (resampledLine.size() > N) { resampledLine.pop_back(); }
	}
	return resampledLine;
}

/*
================================================================================
================================================================================
*/
/*
std::vector<AVector> UtilityFunctions::Trim(std::vector<AVector> curve, float trimValue)
{
	std::vector<AVector> newCurve;
}*/

/*
================================================================================
================================================================================
*/
bool UtilityFunctions::HasEnding(std::string const &fullString, std::string const &ending)
{
	if (fullString.length() >= ending.length())
	{
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	}
	else
	{
		return false;
	}
}

/*
================================================================================
================================================================================
*/
// split string
std::vector<std::string>& UtilityFunctions::Split(const std::string &s, char delim, std::vector<std::string> &elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

/*
================================================================================
================================================================================
*/
// split string
std::vector<std::string> UtilityFunctions::Split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	UtilityFunctions::Split(s, delim, elems);
	return elems;
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::GetBoundingBoxCenter(std::vector<AVector> boundary)
{
	ARectangle rect = GetBoundingBox(boundary);
	return rect.GetCenter();
}

/*
================================================================================
================================================================================
*/
ARectangle UtilityFunctions::GetBoundingBox(std::vector<AVector> boundary)
{
	/*std::vector<cv::Point2f> newBoundary;
	for (int a = 0; a < boundary.size(); a++)
	{ newBoundary.push_back(cv::Point2f(boundary[a].x, boundary[a].y)); }
	cv::Rect bb = cv::boundingRect(newBoundary);
	return ARectangle(AVector(bb.x, bb.y), bb.width, bb.height);*/

	float xMax = std::numeric_limits<float>::min();
	float yMax = std::numeric_limits<float>::min();
	float xMin = std::numeric_limits<float>::max();
	float yMin = std::numeric_limits<float>::max();

	for (unsigned int a = 0; a < boundary.size(); a++)
	{
		AVector pt = boundary[a];

		if (pt.x > xMax) { xMax = pt.x; }
		if (pt.y > yMax) { yMax = pt.y; }
		if (pt.x < xMin) { xMin = pt.x; }
		if (pt.y < yMin) { yMin = pt.y; }
	}

	return ARectangle(AVector(xMin, yMin), xMax - xMin, yMax - yMin);
}

/*
================================================================================
================================================================================
*/
ARectangle UtilityFunctions::GetBoundingBox(std::vector<std::vector<AVector>> boundaries)
{
	float xMax = std::numeric_limits<float>::min();
	float yMax = std::numeric_limits<float>::min();
	float xMin = std::numeric_limits<float>::max();
	float yMin = std::numeric_limits<float>::max();

	for (unsigned int a = 0; a < boundaries.size(); a++)
	{
		for (unsigned int b = 0; b < boundaries[a].size(); b++)
		{
			AVector pt = boundaries[a][b];

			if (pt.x > xMax) { xMax = pt.x; }
			if (pt.y > yMax) { yMax = pt.y; }
			if (pt.x < xMin) { xMin = pt.x; }
			if (pt.y < yMin) { yMin = pt.y; }
		}
	}

	return ARectangle(AVector(xMin, yMin), xMax - xMin, yMax - yMin);
}

/*
================================================================================
================================================================================
*/
std::vector<AVector> UtilityFunctions::TranslatePoly(std::vector<AVector> poly, float x, float y)
{
	std::vector<AVector> newPoly;
	for (unsigned int a = 0; a < poly.size(); a++)
	{
		newPoly.push_back(poly[a] + AVector(x, y));
	}
	return newPoly;
}

/*
================================================================================
================================================================================
*/
std::vector<AVector> UtilityFunctions::MovePoly(std::vector<AVector> poly, AVector oldCenter, AVector newCenter)
{
	AVector offsetVector = newCenter - oldCenter;
	return TranslatePoly(poly, offsetVector.x, offsetVector.y);
}

/*
================================================================================
================================================================================
*/
std::vector<std::vector<AVector>> UtilityFunctions::PathsToVectorAVector(std::vector<APath> paths)
{
	std::vector<std::vector<AVector>> boundaries;
	for (unsigned int a = 0; a < paths.size(); a++)
		{ boundaries.push_back(paths[a].points); }
	return boundaries;
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::Rotate(AVector pt, AVector centerPt, float rad)
{
	pt -= centerPt;
	pt = UtilityFunctions::Rotate(pt, rad);
	pt += centerPt;
	return pt;
}

/*
================================================================================
================================================================================
*/
AVector UtilityFunctions::Rotate(AVector pt, float rad)
{
	float cs = cos(rad);
	float sn = sin(rad);

	float x = pt.x * cs - pt.y * sn;
	float y = pt.x * sn + pt.y * cs;

	return AVector(x, y);
}

/*
================================================================================
make an intersection point shorter
================================================================================
*/
AVector UtilityFunctions::CutIntersectionPoint(AVector pt, AVector intersectPt, float offsetValue)
{
	float dist = pt.Distance(intersectPt);
	AVector dir = (intersectPt - pt).Norm();
	dist -= offsetValue;
	if (dist < std::numeric_limits<float>::epsilon()) { return pt; }
	//if (dist < std::numeric_limits<float>::epsilon()) { return intersectPt; } // failed

	return pt + dir * dist;

}

// barycentric
ABary UtilityFunctions::Barycentric(AVector p, AVector A, AVector B, AVector C)
{
	ABary bary;

	AVector v0 = B - A;
	AVector v1 = C - A;
	AVector v2 = p - A;
	float d00   = v0.Dot(v0);
	float d01   = v0.Dot(v1);
	float d11   = v1.Dot(v1);
	float d20   = v2.Dot(v0);
	float d21   = v2.Dot(v1);
	float denom = d00 * d11 - d01 * d01;
	bary._v     = (d11 * d20 - d01 * d21) / denom;
	bary._w     = (d00 * d21 - d01 * d20) / denom;
	bary._u     = 1.0 - bary._v - bary._w;

	//if (bary._v < 0 || bary._v > 1.0) { std::cout << "bary._v : " << bary._v << "\n"; }
	//if (bary._w < 0 || bary._w > 1.0) { std::cout << "bary._w : " << bary._w << "\n"; }
	//if (bary._u < 0 || bary._u > 1.0) { std::cout << "bary._u : " << bary._u << "\n"; }

	return bary;
}

int UtilityFunctions::GetIndexFromIntList(const std::vector<int>& aList, int elem)
{
	for (unsigned int a = 0; a < aList.size(); a++)
	{
		if (elem == aList[a]) { return a; }
	}

	return -1;
}


