
#ifndef AGRAPH_H
#define AGRAPH_H

#include "AMass.h"
#include "ABary.h"
#include "AnIndexedLine.h"
#include "AnIdxTriangle.h"
#include "UtilityFunctions.h"
#include "OpenCVWrapper.h"

#include "PAD.h"

#include <map>
#include <vector>
#include <algorithm>

#include <limits>

// bins
//	first: which graph
//	second: edge index
typedef std::pair<int, int> ABinItem;

typedef std::vector<AnIdxTriangle> ArtTri;


struct AGraph // should be renamed to elements
{
public:

	// pad debug delete me
	//std::vector<AVector> _padPoints;
	//std::vector<ABary>	_padBarys;
	//std::vector<int>	_padTriIdxs;

	// centroid
	AVector _centroid; // center of mass
	ABary	_centroidBary;
	int		_centroidTriIdx;

	// for orientation force
	std::vector<AVector> _normFromCentroidArray;
	float _angleVal;
	std::vector<AVector> _rotateArray;

	// for shrinking
	//int _shrinking_state;
	//float _transition_time_counter;
	//float _transition_time;

	std::vector<ArtTri> _artTris;

	//std::vector<int> _fColors; // foreground colors
	//std::vector<int> _bColors; // background colors

	std::vector<MyColor> _fColors; // foreground colors
	std::vector<MyColor> _bColors; // background colors

	// shape matching
	bool _isMatched;
	int _matchedVertexIdx;
	AVector _matchedContainerPt;

	// PAD visualization delete me
	AVector _matchedPoint;	
	int _triMatchedIdx;
	ABary _matchedBary;

	bool _bigOne; // main elements, not small elements

	bool _isRigid; //

	//float _scale;
	float _oriSkiOffset;

	float _currentArea;
	float _oriArea;
	float _scale;                         // scaling of the element
	float _sumVelocity;                   // sum of the velocities of the masses
	//float _maxEdgeLengthFactor;           // growth limit
	bool  _isGrowing;                     // is currently growing ?
	
	int   _id;                            // index of this element
	int   _ori_id;

	std::vector<AMass>   _massList;       // list of the masses

	//std::map<int, AMass> _massMap;      // we're not using MST,  only use this for kruskal
	//std::vector<AVector> _oriMassPos;   // 
	//std::vector<AnIndexedLine> _edges;  // we're not using MST

	int   _skinPointNum;  // number of boundary points
	float _averageEdgeLength; // average of the length of the edges
	float _oriAvgEdgeLength;  // original average of the edge length, for calculating stopping
	//float _minEdgeLength;

	std::vector<AnIndexedLine> _skinIdxEdges;  // indices of the boundary points	 
	std::vector<AVector>       _skin;       // boundary points. temporary data, always updated every step

	// PAD and friends
	PADCalculator _padCalc;
	float _skin_length;
	std::vector<bool> _rdpFlags_skin;
	std::vector<AVector> _resampled_skin;
	std::vector<AVector> _smooth_skin;


	// embed vector graphics
	std::vector<std::vector<AVector>> _uniuniArts;
	std::vector<std::vector<int>>     _uniuniArts2Triangles;
	std::vector<std::vector<ABary>>   _uniuniBaryCoords;  

	std::vector<AVector>              _uniArt;
	std::vector<int>                  _uniArt2Triangles;
	std::vector<ABary>                _uniBaryCoords;

	std::vector<std::vector<AVector>> _tesselateArts;
	std::vector<std::vector<int>>     _tesselateArts2Triangles; // mapping vector graphics to triangles
	std::vector<std::vector<ABary>>   _tesselateBaryCoords;     // barycentric coordinates


	std::vector<std::vector<AVector>> _arts;           // vector graphics
	std::vector<std::vector<int>>     _arts2Triangles; // mapping vector graphics to triangles
	std::vector<std::vector<ABary>>   _baryCoords;     // barycentric coordinates

	std::vector<AnIdxTriangle> _triangles; // triangulation
	std::vector<AnIndexedLine> _triEdges;  // for edge forces

	std::vector<AnIndexedLine> _auxiliaryEdges;  // for edge forces

	std::vector<AnIndexedLine> _negSpaceEdges;  // for edge forces

	// mapping from _triEdges to _triangles
	std::vector<std::vector<int>> _edgeToTri;

	// for self-intersection relax
	std::vector<int> _randommm_indices;

	OpenCVWrapper _cvWrapper;
	
	AGraph();

	float DistanceToBoundary(AVector pt) ;

	// need to call RecalculateArts
	bool InsideArts(AVector pt) const;

	// need to call RecalculateArts
	float DistToArts(AVector pt) const;

	void CalculateCentroid();

	void CalculateVecToCentroidArray();

	//void CalculateVecToCentroidArray2();

	float GetArea();

	// calculate the sum
	// of velocities of the masses
	void CalculateSumVelocity();
	
	// calculate std::vector<std::vector<ABary>> _baryCoords
	void ComputeBarycentric();

	void ComputeTesselateBarycentric();
	
	void TesselateArts();

	//
	void CalculateOriAvgEdgeLength();

	// should be called after UpdateBoundaryAndAvgEdgeLength
	void SetMatchedPoint(AVector e_pt, AVector c_pt);

	// should be called after UpdateBoundaryAndAvgEdgeLength	
	void CalculatePAD();

	void CalculateSmoothSkin();

	void CalculatePADBary();

	//
	void UpdateBoundaryAndAvgEdgeLength();

	//
	AVector FindClosestPtOnEdges(int idx, AVector pt);

	//
	void SelfIntersectionFlagging();	

	//
	void SelfIntersectionRelax();

	//
	void CalculateBoundaryEdges();

	//
	void DrawAsSelected();

	//
	void Draw();

	//
	void Move(AVector vec);

	//
	void Translate(AVector vec);

	//
	void ReflectXAxis();

	//
	void Rotate(float radValue);

	//
	void Scale(float scaleFactor);
	
	//
	void ConvertMassMapToList();

	void RemoveShortNegSpaceEdges();

	//
	//void CalculateNNEdges();	

	//
	bool CanGrow();

	//
	//void Grow2(float growth_scale_iter, std::vector<AGraph>& allGraphs, float dt);

	//void InitShrinking();

	//
	void Grow(float growth_scale_iter, std::vector<AGraph>& allGraphs,  float dt);	

	//
	void ComputeFoldingForces();	

	//
	AVector FlipVertex(AVector l1, AVector l2, AVector pt);

	//
	void MSTEdgeRelax();

	// NN EDGES
	void RecalculateEdgeLengths();

	//
	void RecalculateUniUniArts();

	//
	void RecalculateArts();

	// triangle edges
	void RecalculateTriangleEdgeLengths();

	// triangle edges
	void CalculateTriangleEdges();

	// create bending springs
	void CreateBendingSprings();

	//
	int GetUnsharedVertexIndex(AnIdxTriangle tri, AnIndexedLine edge);

	//
	bool TryToAddTriangleEdge(AnIndexedLine anEdge, int triIndex);	

	// triangle edges
	int FindTriangleEdge(AnIndexedLine anEdge);

	void SolveForNegativeSPaceSprings();

	// triangle edges
	void SolveForTriangleSprings();

	void SolveForNoise(/*std::vector<std::vector<AVector>> perlinMap*/);

};

#endif