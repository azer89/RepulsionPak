/* ---------- ShapeRadiusMatching V2  ---------- */

#include "NANOFLANNWrapper.h"
#include "PointCloud.h"
#include "AVector.h"
#include "ALine.h"

using namespace nanoflann;

NANOFLANNWrapper::NANOFLANNWrapper() : _pointKDTree(0), _leaf_max_size(10)
{
}

NANOFLANNWrapper::~NANOFLANNWrapper()
{
	if (_pointKDTree) delete _pointKDTree;
}

/*void NANOFLANNWrapper::CreateDjikstraKDTree()
{
	int numData = 0;
	for (int a = 0; a < _djikstraData.size(); a++)
		{ numData += _djikstraData[a].size(); }
	_pointCloud.pts.resize(numData);

	int iter = 0;
	for (int a = 0; a < _djikstraData.size(); a++)
	{
		for (int b = 0; b < _djikstraData[a].size(); b++)
		{
			AVector pt = _djikstraData[a][b];
			_pointCloud.pts[iter].x = pt.x;
			_pointCloud.pts[iter].y = pt.y;
			_pointCloud.pts[iter].info1 = a; // store the index
			_pointCloud.pts[iter].info2 = b; // store the index
			iter++;
		}
	}

	_pointKDTree = new PointKDTree(2, _pointCloud, KDTreeSingleIndexAdaptorParams(_leaf_max_size));
	_pointKDTree->buildIndex();
}*/

void NANOFLANNWrapper::CreatePointKDTree()
{
	// how many point?
	int numPt = _pointData.size();

	_pointCloud.pts.resize(numPt);
	for (size_t a = 0; a < _pointData.size(); a++)
	{
		AVector pt = _pointData[a];
		_pointCloud.pts[a].x = pt.x;
		_pointCloud.pts[a].y = pt.y;
		_pointCloud.pts[a].info1 = a; // store the index
	}

	// Create point tree
	_pointKDTree = new PointKDTree(2 /*dim*/, _pointCloud, KDTreeSingleIndexAdaptorParams(_leaf_max_size));
	_pointKDTree->buildIndex();
}

void NANOFLANNWrapper::CreatePointWithInfoKDTree()
{
	// how many point?
	int numPt = _pointData.size();

	_pointCloud.pts.resize(numPt);
	for (size_t a = 0; a < _pointData.size(); a++)
	{
		AVector pt = _pointData[a];
		_pointCloud.pts[a].x = pt.x;
		_pointCloud.pts[a].y = pt.y;

		_pointCloud.pts[a].info1 = _pointInfo1[a];  // info 1
		_pointCloud.pts[a].info2 = _pointInfo2[a];  // info 2
	}

	// Create point tree
	_pointKDTree = new PointKDTree(2 /*dim*/, _pointCloud, KDTreeSingleIndexAdaptorParams(_leaf_max_size));
	_pointKDTree->buildIndex();
}

/*void NANOFLANNWrapper::CreateLineKDTree()
{
	// how many lines?
	int numPt = _lineData.size();

	_pointCloud.pts.resize(numPt);
	for (size_t a = 0; a < _lineData.size(); a++)
	{
		AVector pt = _lineData[a].GetMiddlePoint();
		_pointCloud.pts[a].x = pt.x;
		_pointCloud.pts[a].y = pt.y;
		_pointCloud.pts[a].info1 = a; // store the index
	}

	// Create point tree
	_pointKDTree = new PointKDTree(2, _pointCloud, KDTreeSingleIndexAdaptorParams(_leaf_max_size));
	_pointKDTree->buildIndex();
}*/

void NANOFLANNWrapper::SetPointData(std::vector<AVector> myData)
{
	this->_pointData = myData;
}

void NANOFLANNWrapper::SetPointDataWithInfo(std::vector<AVector> myData, std::vector<int> info1, std::vector<int> info2)
{
	this->_pointData = myData;
	this->_pointInfo1 = info1;
	this->_pointInfo2 = info2;
}

//void NANOFLANNWrapper::SetDjikstraData(std::vector<std::vector<AVector>> myGraph)
//{
//	this->_djikstraData = myGraph;
//}

//void NANOFLANNWrapper::SetLineData(std::vector<ALine> myData)
//{
//	this->_lineData = myData;
//}

void NANOFLANNWrapper::AppendPointData(std::vector<AVector> myData)
{
	this->_pointData.insert(this->_pointData.end(), myData.begin(), myData.end());
}

//void NANOFLANNWrapper::AppendLineData(std::vector<ALine> myData)
//{
//	this->_lineData.insert(this->_lineData.end(), myData.begin(), myData.end());
//}

std::vector<AVector> NANOFLANNWrapper::GetClosestPoints(AVector pt, int num_query)
{
	float query_pt[2] = { pt.x, pt.y };
	std::vector<size_t> ret_index(num_query);
	std::vector<float> out_dist_sqr(num_query);

	nanoflann::KNNResultSet<float> resultSet(num_query);
	resultSet.init(&ret_index[0], &out_dist_sqr[0]);
	_pointKDTree->findNeighbors(resultSet, &query_pt[0], nanoflann::SearchParams(10, 1e-8));

	std::vector<AVector> retVectors;

	for (int a = 0; a < num_query; a++)
	{
		AVector retPt(_pointCloud.pts[ret_index[a]].x, _pointCloud.pts[ret_index[a]].y);
		retVectors.push_back(retPt);
	}

	return retVectors;
}

std::vector<int> NANOFLANNWrapper::GetClosestIndices(AVector pt, int num_query)
{
	float query_pt[2] = { pt.x, pt.y };
	std::vector<size_t> ret_index(num_query);
	std::vector<float> out_dist_sqr(num_query);

	_pointKDTree->knnSearch(&query_pt[0], num_query, &ret_index[0], &out_dist_sqr[0]);
	std::vector<int> retIndices;

	for (int a = 0; a < num_query; a++)
	{
		retIndices.push_back(_pointCloud.pts[ret_index[a]].info1);
	}

	return retIndices;
}

std::vector<std::pair<int, int>> NANOFLANNWrapper::GetClosestPairIndices(AVector pt, int num_query)
{
	float query_pt[2] = { pt.x, pt.y };
	std::vector<size_t> ret_index(num_query);
	std::vector<float>  out_dist_sqr(num_query);

	_pointKDTree->knnSearch(&query_pt[0], num_query, &ret_index[0], &out_dist_sqr[0]);
	std::vector<std::pair<int, int>> retIndices;

	for (int a = 0; a < num_query; a++)
	{
		retIndices.push_back(std::pair<int, int>(_pointCloud.pts[ret_index[a]].info1, _pointCloud.pts[ret_index[a]].info2));
	}

	return retIndices;
}

//float NANOFLANNWrapper::DistancePolygonTest(AVector pt, int neighbor)
//{
//	return pt.Distance(PointPolygonTest(pt, neighbor));
//}

/*int NANOFLANNWrapper::IndexPolygonTest(AVector pt, int neighbor)
{
	// two neighbors
	//int n = 2;
	std::vector<int> retIndices = GetClosestIndices(pt, neighbor);
	float dist = std::numeric_limits<float>::max();
	//AVector closestPt = pt;
	int retIdx = -1;
	for (int a = 0; a < neighbor; a++)
	{
		int idx = retIndices[a];
		ALine ln = _lineData[idx];
		AVector vec = UtilityFunctions::ClosestPtAtFiniteLine(ln.GetPointA(), ln.GetPointB(), pt);
		float d = vec.Distance(pt);
		if (d < dist)
		{
			dist = d;
			retIdx = retIndices[a];
		}
	}
	//if (idx == -1) { std::cout << "error\n"; }
	return retIdx;
}*/

/*AVector NANOFLANNWrapper::PointPolygonTest(AVector pt, int neighbor)
{
	// two neighbors
	//int neighbor = 2;
	std::vector<int> retIndices = GetClosestIndices(pt, neighbor);
	float dist = std::numeric_limits<float>::max();
	AVector closestPt = pt;
	for (int a = 0; a < neighbor; a++)
	{
		int idx = retIndices[a];
		ALine ln = _lineData[idx];
		AVector vec = UtilityFunctions::ClosestPtAtFiniteLine(ln.GetPointA(), ln.GetPointB(), pt);
		float d = vec.Distance(pt);
		if (d < dist)
		{
			dist = d;
			closestPt = vec;
		}
	}

	return closestPt;
}*/

std::vector<ALine> NANOFLANNWrapper::ConvertVectorsToLines(std::vector<std::vector<AVector>> polylines)
{
	std::vector<ALine> lines;
	for (int a = 0; a < polylines.size(); a++)
	{
		for (int b = 0; b < polylines[a].size() - 1; b++)
		{
			AVector pt1 = polylines[a][b];
			AVector pt2 = polylines[a][b + 1];

			ALine ln(pt1, pt2);
			lines.push_back(ln);
		}
	}
	return lines;
}

std::vector<ALine> NANOFLANNWrapper::ConvertVectorsToLines(std::vector<AVector> polyline)
{
	std::vector<ALine> lines;
	for (int a = 0; a < polyline.size() - 1; a++)
	{
		AVector pt1 = polyline[a];
		AVector pt2 = polyline[a + 1];

		ALine ln(pt1, pt2);
		lines.push_back(ln);
	}
	return lines;
}