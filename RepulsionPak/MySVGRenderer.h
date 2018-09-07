
/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef MY_SVG_RENDERER
#define MY_SVG_RENDERER

#include "AVector.h"
#include "VFRegion.h"
//#include "VFDRegionWrapper.h"
#include "ArtData.h"
//#include "ABlob.h"
//#include "LRFunctions.h"

#include "AGraph.h"
#include "MyColor.h"

#include <vector>

typedef std::vector<std::vector<AVector>> GraphArt;
typedef std::vector<MyColor> ArtColors;


class MySVGRenderer
{
public:
	//std::vector<GraphArt> oArts;
	//std::vector<std::vector<GraphArt>> dArts;
	static void LineUpArts(std::string filename,
		                   std::vector<GraphArt> oArts,
						   std::vector<std::vector<GraphArt>> dArts,
						   std::vector<ArtColors> fColors,
						   std::vector<ArtColors> bColors);

	static void SaveArts(std::string filename, 
		                 std::vector<std::vector<AVector>> arts, 
						 std::vector<int> fColors, 
						 std::vector<int> bColors);

	static void SaveElementsAndSkins(std::string filename, 
		                             std::vector<AGraph> graphs,
									 const std::vector<std::vector<std::vector<AVector>>>& focals,
									 const std::vector<std::vector<AVector>>& offsetFocalBoundaries,
									 std::vector<std::vector<AVector>> boundaries);

	static void SaveTriangles(std::string filename, std::vector<AGraph> graphs);

	// physics pak
	static void SaveGraphElements(std::string filename, std::vector<AGraph> graphs);
	 
	//std::vector<std::vector<std::vector<AVector>>> _focals;
	//std::vector<std::vector<AVector>> _offsetFocalBoundaries;
	static void SaveOrnamentsToSVG(std::string filename, 
		                           const std::vector<ArtData>& ornaments, 
								   std::vector<VFRegion> regions,
								   bool saveSimple = false);

	//static void SaveSimpleBoundariesToSVG(std::string filename, std::vector<ArtData> ornaments);

	static void SaveTrianglesToSVG(std::string filename, std::vector<ArtData> ornaments);

	//static void SaveStreamlinesToSVG(std::string filename, std::vector<ABlob> blobs);

	//static void SaveBlobToSVG(std::string           filename, 
	//	                      std::vector<ABlob>    blobs, 
	//						  std::vector<VFRegion> regions,
	//						  std::vector<AVector>  combinedBoundary);

	// general debugging
	static void SaveShapesToSVG(std::string filename, std::vector<std::vector<AVector>> shapes);

	static void SaveDijkstraDebugImageLOL(std::string filename, 
		                                  std::vector<std::vector<AVector>> myGraph, 
										  std::vector<AVector> blobBoundary, 
										  std::vector<AVector> regionBoundary,
										  std::vector<AVector> cubicCurve,
										  std::vector<AVector> shortestPath,
										  std::vector<AVector> oldStreamline,
										  AVector startPt,
										  AVector endPt);
	//static void SaveLRFunctions(std::string filename, LRFunctions func);
	static void SaveLRFunctions(std::string filename, std::vector<ALine> intersectList);
};

#endif