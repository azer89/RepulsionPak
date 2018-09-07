
/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
August 2016
================================================================================
*/

#ifndef FLANN_PROXY_H
#define FLANN_PROXY_H

/*
this is a proxy of nanoflann library that has BSD license,
nanoflann builds a kd-tree which is really useful for KNN.

for more information about the library go to this github repo:
https://github.com/jlblancoc/nanoflann
*/

#include "nanoflann.hpp"
#include "AVector.h"
#include "ALine.h"
#include "PointCloud.h"

#include <vector>

using namespace nanoflann;

// 
typedef KDTreeSingleIndexAdaptor< L2_Simple_Adaptor<float, PointCloud<float> >,
	PointCloud<float>,
	2 /*dim*/>
	PointKDTree;

class NANOFLANNWrapper
{
public:
	NANOFLANNWrapper();
	~NANOFLANNWrapper();

	// set data
	void SetPointData(std::vector<AVector> myData);
	void SetPointDataWithInfo(std::vector<AVector> myData, std::vector<int> info1, std::vector<int> info2);

	//void SetLineData(std::vector<ALine>   myData);
	void AppendPointData(std::vector<AVector> myData);
	//void SetDjikstraData(std::vector<std::vector<AVector>> myGraph);
	//void AppendLineData(std::vector<ALine>   myData);

	// query
	std::vector<AVector> GetClosestPoints(  AVector pt, int num_query);
	std::vector<int>     GetClosestIndices( AVector pt, int num_query);

	std::vector<std::pair<int, int>> GetClosestPairIndices(AVector pt, int num_query);

	// prepare kd-tree
	void CreatePointKDTree();
	void CreatePointWithInfoKDTree();
	//void CreateLineKDTree();
	//void CreateDjikstraKDTree();

	//AVector PointPolygonTest(    AVector pt, int neighbor = 2);
	//float   DistancePolygonTest( AVector pt, int neighbor = 2);
	//int     IndexPolygonTest(    AVector pt, int neighbor = 2);

	std::vector<ALine> ConvertVectorsToLines(std::vector<std::vector<AVector>> polylines);
	std::vector<ALine> ConvertVectorsToLines(std::vector<AVector> polyline);

public:
	//std::vector<std::vector<AVector>> _djikstraData;

	std::vector<AVector> _pointData;
	std::vector<int> _pointInfo1;
	std::vector<int> _pointInfo2;

	//std::vector<ALine>   _lineData;

	PointKDTree* 	  _pointKDTree;
	PointCloud<float> _pointCloud;

	int _leaf_max_size;
};

#endif