

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

struct AnElement;

// An object to represent a mass
class AMass
{
private:

public:
	CollissionGrid* _cGrid;

	bool _isGrowing;
	float _distToBoundary;
	bool _isFolded;
		

	float   _mass; 
	AVector _pos;
	AVector _velocity;

	inline AVector GetPos() const
	{
		return _pos;
	}

	std::vector<AnIdxTriangle> _triangles;
	
	float _closestDist;
	int _n_closest_elems;
	
	std::vector<AVector> _closestPoints;
	int _closestPt_actual_sz; // reserve for avoiding push_back
	int _closestPt_fill_sz;   // reserve for avoiding push_back

	std::vector<AVector> _closestPeaks;
	//int _closestOtherGraphIdx; // graph index

	float _avgEdgeLength;
	bool  _isInside;
	int   _idx;
	
	std::vector<AnIndexedLine>  _triEdges; // edges from triangulation
	std::unordered_map<int, int> _neighbors;

	std::vector<int> _segmentIndices;
	std::vector<AVector> _lineSgment;

public:

	void CalculateIndicesOfLineSegment(const int& numSkin);
	void UpdateLineSegment(const std::vector<AMass>& otherMasses);
	
public:
	
	AVector _edgeForce;
	AVector _repulsionForce;
	AVector _boundaryForce;
	AVector _overlapForce;	
	AVector _rotationForce;
	AVector _selfIntersectForce;

	// Constructor
	AMass();

	// Constructor
	AMass(const float& x, const float& y);

	// Constructor
	AMass(const AVector& pos);

	void CallMeFromConstructor();

	// true if accepted, false if rejected
	bool TryToAddTriangleEdge(AnIndexedLine anEdge, const std::vector<AMass>& otherMasses);

	bool FindTriEdge(const AnIndexedLine& anEdge);

	bool IsNeighbor(int idx);

	bool IsInsideTriangle(const AVector& pt, const std::vector<AMass>& otherMasses);	

	void Grow(const float& growth_scale_iter, const float& dt);

	void Draw();

	void DrawForce();

	void Init(); // reset force to zero

	void Simulate(float dt);

	void Solve(const int& massNumber,
				const AnElement& parentGraph,
				const std::vector<std::vector<AVector>>& boundaries,
				const std::vector<std::vector<AVector>>& holes,
				const std::vector<std::vector<AVector>>& focalOffsets);

	void GetClosestPoints2(const int& parentGraphIndex);


};

#endif