
/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef CLIPPER_WRAPPER_H
#define CLIPPER_WRAPPER_H

#include "AVector.h"
#include "ALine.h"

//#include "AGraph.h"
//#include "VFRegion.h"

struct VFRegion;
struct AGraph;

#include <vector>

class ClipperWrapper
{
public:
	ClipperWrapper();
	~ClipperWrapper();

	// intersections using clipperlib
	// find the effect of changing orientation with the intersections obtained
	static AVector FindTheFarthestIntersection(ALine line, std::vector<AVector> shape/*, bool reverseShape = false*/);  // from endpoint	
	static AVector FindTheFarthestIntersection(ALine line, std::vector<std::vector<AVector>> boundaries);

	static AVector FindTheClosestIntersection(ALine line, std::vector<AVector> shape);  // from start
	static AVector FindTheClosestIntersection(ALine line, std::vector<std::vector<AVector>> shapes);

	static AVector ClosestIntersectionWithOpenPoly(std::vector<AVector> poly, std::vector<std::vector<AVector>> shapes);

	static AVector FindTheClosestIntersection2(ALine line, std::vector<AVector> shape);  // from start
	static std::vector<std::vector<AVector>> BlobBoundaryOffsetting(std::vector<AVector> blobBoundary, float offsetVal);

	// orientation
	static bool IsClockwise(std::vector<AVector> polygon);
	static std::vector<std::vector<AVector>> MakeClockwise(std::vector<std::vector<AVector>> shapes);
	static std::vector<AVector> MakeClockwise(std::vector<AVector> shape);

	// offsetting 
	static std::vector<std::vector<AVector>> RoundOffsettingP(  std::vector<AVector> polygon,                float offsetVal);  // closed poly	
	static std::vector<std::vector<AVector>> RoundOffsettingPP( std::vector<std::vector<AVector >> polygons, float offsetVal);  // closed polys
	static std::vector<std::vector<AVector>> MiterOffsettingP(  std::vector<AVector> polygon,                float offsetVal, float miterLimit);  // closed poly	
	static std::vector<std::vector<AVector>> MiterOffsettingPP( std::vector<std::vector<AVector >> polygons, float offsetVal, float miterLimit);  // closed polys

	// experiment 
	static void ClippingContainer(const std::vector<AVector>& container, 
		                          const std::vector<std::vector<AVector >>& skins,
								  std::vector<std::vector<AVector>>& outPolys,
								  std::vector<bool>& orientationFlags);

	static std::vector<std::vector<AVector>> GetUniPolys(std::vector<std::vector<AVector >> polygons);  // closed poly	

	static float CalculateFill(const std::vector<AVector>& container, const std::vector<AGraph>& graphs);
	static float CalculateFill(const std::vector<AVector>& container, const std::vector<std::vector<AVector >>& graphs);

	static void CalculateSCP(const std::vector<AVector>& container, const std::vector<AGraph>& graphs);

	// offsetting
	static std::vector<std::vector<AVector>> OffsettingL(std::vector<AVector> someLine, float offsetVal); 

	// positive and negative offsetting
	static std::vector<std::vector<AVector>> MiterPNOffsettingP(std::vector<AVector> polygon, float offsetVal, float miterLimit);                 // closed poly	
	static std::vector<std::vector<AVector>> MiterPNOffsettingPP(std::vector<std::vector<AVector >> polygons, float offsetVal, float miterLimit); // closed polys

	// XOR followed by intersection
	static std::vector<std::vector<AVector>> XORIntersectionOperations(std::vector<AVector> targetShape, std::vector<std::vector<AVector>> clippingShapes);

	//static std::vector<AVector> UnionOperation(std::vector<std::vector<AVector>> polygons);

	// Union Operation
	static std::vector<AVector> UnionOperation(std::vector<std::vector<AVector>> polys);
	static std::vector<AVector> UnionOperation(std::vector<AVector> poly1, std::vector<AVector> poly2);

	static std::vector<AVector> UnionOperationWithGrowing(std::vector<std::vector<AVector>>& polys);

	// get poly with the largest area
	static std::vector<AVector> GetLargestPoly(std::vector<std::vector<AVector>> polys);

private:
	//OpenCVWrapper _cvWrapper;
};

#endif




