

#ifndef MASS_H
#define MASS_H

#include "AVector.h"

#include "AngleConnector.h"
#include "AnIdxTriangle.h"
#include "AnIndexedLine.h"
#include "UtilityFunctions.h"

//#include "AGraph.h"

#include "NANOFLANNWrapper.h"
#include "QTObject.h"
#include "Quadtree.h"
#include "CollissionGrid.h"

#include <vector>
#include <algorithm>
#include <unordered_map>

struct AGraph;

// An object to represent a mass
class AMass
{
private:

public:
	CollissionGrid* _cGrid;

	bool _isGrowing;

	float _distToBoundary;

	bool _isFolded;

	//bool _selfIntersect;
	//int _cellIdx;

	float   _mass;     // 
	AVector _pos;	  // current
	AVector _velocity;
	//AVector _prevPos; // previous

	std::vector<AnIdxTriangle> _triangles;
	//std::vector<AVector>       _triCenters;

	//AVector _angleVel;  
	//AVector _edgeVel;
	//AVector _repulsionVel;
	//AVector _boundaryVel;

	// closest point from other graph
	//float   _closestOtherDist;
	float                _closestDist;
	std::vector<int>     _closestGraphIndices;


	std::vector<AVector> _closestPoints;
	int _closestPt_actual_sz; // reserve for avoiding push_back
	int _closestPt_fill_sz;   // reserve for avoiding push_back

	std::vector<AVector> _closestPeaks;
	//int                  _closestOtherGraphIdx; // graph index

	float _avgEdgeLength;
	bool  _isInside;
	int   _idx;

	// not used
	//float _oriDistToMidPoint; // original distance to the midpoint of facing edge
	//float _distToMidPoint;    // distance to the midpoint of facing edge

	//std::vector<AnIndexedLine>  _moreEdges;

	//std::vector<AnIndexedLine>  _edges;
	//std::vector<AnIndexedLine>  _nnEdges;         // n nearest neighbors for attaching springs, currently disabled
	//std::vector<AngleConnector> _angleConnectors;

	std::vector<AnIndexedLine>  _triEdges; // edges from triangulation
	std::unordered_map<int, int> _neighbors;

public:
	std::vector<int> _segmentIndices;
	std::vector<AVector> _lineSgment;

	void CalculateIndicesOfLineSegment(int numSkin);
	void UpdateLineSegment(const std::vector<AMass>& otherMasses);


public:
	//AVector _attractionForce;
	//AVector _angleForce;
	AVector _edgeForce;
	AVector _repulsionForce;
	AVector _boundaryForce;
	AVector _overlapForce;
	//AVector _noiseForce;
	AVector _rotationForce;
	AVector _selfIntersectForce;
	//AVector _foldingForce;

	AVector _prevForce;

	// Constructor
	AMass();

	// Constructor
	AMass(float x, float y);

	// Constructor
	AMass(AVector pos);

	void CallMeFromConstructor();

	// true if accepted, false if rejected
	bool TryToAddTriangleEdge(AnIndexedLine anEdge, const std::vector<AMass>& otherMasses);

	//bool FindEdge(AnIndexedLine anEdge);
	bool FindTriEdge(AnIndexedLine anEdge);

	//void AddEdge(   AnIndexedLine anEdge);	

	//void SortEdges(const std::vector<AMass>& otherMasses);

	bool IsNeighbor(int idx);

	bool IsInsideTriangle(AVector pt, const std::vector<AMass>& otherMasses);

	
	
	//AVector GetClosestPointOnEdges(std::vector<AMass> otherMasses,
	//	                           AVector pt, // query point 
	//						       int ptIdx); // query index, make sure it's not neighbor

	void RecalculateEdgeLengths(const std::vector<AMass>& otherMasses);

	void Grow(float growth_scale_iter, float dt);

	void Draw();

	void DrawForce();

	void CalculateNNEdges(const std::vector<AMass>& otherMasses, int numBoundaryPoint);	

	void CalculateFoldingForce(const std::vector<AMass>& otherMasses);

	void Solve(const int& massNumber,
		       AGraph& parentGraph,
			   //const std::vector<AGraph>& allGraphs,
			   //CollissionGrid* cGrid,
			   const std::vector<std::vector<AVector>>& boundaries,
			   const std::vector<std::vector<AVector>>& holes,
			   const std::vector<std::vector<AVector>>& focalOffsets);

	//void IntersectionRelax(int parentGraphIndex, std::vector<AGraph>& allGraphs);

	//void AngleRelax(std::vector<AMass>& otherMasses);

	void VerletSyncEdgeLengths(const std::vector<AMass>& otherMasses); // currently not used

	void CollissionRelax(int parentGraphIndex, std::vector<AGraph>& allGraphs);

	//void VerletRelax(std::vector<AMass>& otherMasses); // move this to AGraph

	void GetClosestPoints2(int parentGraphIndex
		                     /*std::vector<AGraph>& allGraphs,*/
							 //std::vector<AVector> peaks,
		                     /*CollissionGrid* cGrid*/);

	//void GetClosestPoints(int parentGraphIndex,
	//	                     std::vector<AGraph>& allGraphs,
	//						 std::vector<AVector> peaks);

	void Init(); // reset force to zero

	void Simulate(float dt/*, float dampingVal*/);

	void UpdatePos(float dt, float dtdt_half); // dtdt_half = dt * dt * 0.5
	void UpdateVelocity(float dt_half, float dampingVal);
};

#endif