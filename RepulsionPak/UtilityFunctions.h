
/* ---------- RepulsionPak  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include <vector>
#include "APath.h"
#include "ABary.h"

// forward declaration
struct AVector;
struct ALine;
struct ARectangle;


class UtilityFunctions
{
public:
	//static AVector PolygonCentroid();

	static float TSign(AVector p1, AVector p2, AVector p3);
	static bool PointInTriangle(AVector A, AVector B, AVector C, AVector P);

	// Distance to line
	static float  DistanceToFiniteLine(AVector v, AVector w, AVector p);
	static AVector ClosestPtAtFiniteLine2(const AVector& v, const AVector& w, const AVector& p); // pbourke algorithm
	static AVector ClosestPtAtFiniteLine(AVector v, AVector w, AVector p);

	// Distance to points
	static float   DistanceToBunchOfPoints(const std::vector<std::vector<AVector>>& points, AVector p);
	static float   DistanceToBunchOfPoints(const std::vector<AVector>& points, AVector p);

	// points
	static AVector ClosestPoint(const std::vector<AVector>& points, AVector p);

	// Distance to polylines
	static float   DistanceToPolyline(const std::vector<AVector>& polyline, AVector p);
	static float   DistanceToPolylines(const std::vector<std::vector<AVector>>& polylines, AVector p);
	static AVector GetClosestPtOnPolyline(const std::vector<AVector>& polyline, AVector p);
	//static AVector GetClosestPtOnClosedPolyline(std::vector<AVector> polyline, AVector p);
	static float   DistanceToClosedCurve(std::vector<AVector> polyline, AVector p);
	static float   DistanceToClosedCurves(const std::vector<std::vector<AVector>>& polylines, AVector p);
	static AVector GetClosestPtOnClosedCurve(const std::vector<AVector>& polyline, const AVector& p);

	// Left Right
	static void GetLeftRightPoints(AVector p1, AVector p2, float offset, AVector* pA, AVector* pB);
	static void GetLeftRightDirections(AVector p1, AVector p2, AVector* dirA, AVector* dirB);
	static void GetLeftRightNormals(AVector prev, AVector cur, AVector next, AVector* leftNormal, AVector* rightNormal);
	static void GetLeftNormals(AVector prev, AVector cur, AVector next, AVector* leftNormal);
	static void GetRightNormals(AVector prev, AVector cur, AVector next, AVector* rightNormal);
	static void GetListsOfLeftRightNormals(const std::vector<AVector>& curve, std::vector<AVector>& lDirs, std::vector<AVector>& rDirs);
	static std::vector<AVector> GetInterPolatedLRDirs(float normDist, std::vector<AVector> lDirs, std::vector<AVector> rDirs, int functionLength);

	// eliminate intersection of ornament tips with the others, get the longest poly
	static std::vector<AVector> TrimPoly(const std::vector<AVector>& poly, const std::vector<std::vector<AVector>>& shapes);

	// Todo: implement closed curves
	static float CurveLength(std::vector<AVector> curves);
	static float CurveLengthClosed(std::vector<AVector> curves);
	static bool InsidePolygon(const std::vector<AVector>& polygon, float px, float py);
	static bool InsidePolygons(const std::vector<std::vector<AVector>>& polygons, float px, float py);
	static float Angle2D(float x1, float y1, float x2, float y2);

	// curve orientation
	static bool IsClockwise(std::vector<AVector> polygon);

	// this only resample open curve
	static void                 UniformResampleWithConvexHull(std::vector<AVector> oriCurve,
		std::vector<AVector> convexHull,
		std::vector<AVector>& resampleCurve,
		float resampleGap);
	static void                 UniformResampleNoFirstPoint(std::vector<AVector> oriCurve,
		std::vector<AVector>& resampleCurve,
		float resampleGap);


	static void                 UniformResample(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, float resampleGap);
	static void                 UniformResample(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, int N);
	static void                 UniformResampleClosed(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, int N);
	static void                 UniformResampleClosed(std::vector<AVector> oriCurve, std::vector<AVector>& resampleCurve, float resampleGap);
	static std::vector<AVector> UniformResampleALine(AVector pt1, AVector pt2, int N);
	static AVector              GetPointInterpolationFromStart(std::vector<AVector> curve, float val); // val should be 0..1
	static void                 GetIndicesInBetween(std::vector<AVector> curve, float val, int& index1, int& index2);
	static std::vector<AVector> GetCurveSubset(std::vector<AVector> curve, int N, float startVal, float endVal);
	//static std::vector<AVector> Trim(std::vector<AVector> curve, float trimValue);

	// string
	static bool HasEnding(std::string const& fullString, std::string const& ending);
	static std::vector<std::string>& Split(const std::string& s, char delim, std::vector<std::string>& elems);
	static std::vector<std::string> Split(const std::string& s, char delim);

	// bounding box
	static ARectangle GetBoundingBox(std::vector<AVector> boundary);
	static ARectangle GetBoundingBox(std::vector<std::vector<AVector>> boundaries);
	static AVector GetBoundingBoxCenter(std::vector<AVector> boundary);

	// translation
	static std::vector<AVector> TranslatePoly(std::vector<AVector> poly, float x, float y);
	static std::vector<AVector> MovePoly(std::vector<AVector> poly, AVector oldCenter, AVector newCenter);

	// conversion
	static std::vector<std::vector<AVector>> PathsToVectorAVector(std::vector<APath> paths);

	static AVector Rotate(AVector pt, AVector centerPt, float rad);

	// rotate point
	static AVector Rotate(AVector pt, float rad);

	// make an intersection point shorter
	static AVector CutIntersectionPoint(AVector pt, AVector intersectPt, float offsetValue);

	// barycentric
	static ABary Barycentric(AVector p, AVector A, AVector B, AVector C);

	static int GetIndexFromIntList(const std::vector<int>& aList, int elem);

	// c is the query, a-b is a line
	static bool IsLeft(AVector a, AVector b, AVector c);

	static AVector ProjectToALine(AVector a, AVector b, AVector p)
	{
		AVector ap = p - a;
		AVector ab = b - a;
		return a + ab * ap.Dot(ab) / ab.Dot(ab);


	}
};

#endif 
