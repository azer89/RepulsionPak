
#include "AMass.h"
#include "AnElement.h"

#include "AVector.h"

#include "NANOFLANNWrapper.h"
#include "SystemParams.h"

#include "glew.h"
#include "freeglut.h"

#include "StuffWorker.h"


#define _USE_MATH_DEFINES
#include <math.h> 


// sorting edges
//bool CompareByAngle(const AnIndexedLine &a, const AnIndexedLine &b)
//{ return a._angle < b._angle; }

// sorting angle connectors
//bool CompareByAngle2(const AngleConnector &a, const AngleConnector &b)
//{ return a._angle < b._angle; }


// Constructor
AMass::AMass()
{
	//this->_m     = 0;             // mass is always one
	this->_pos   = AVector(0, 0);
	this->_idx   = -1;
	//this->_cellIdx = -1;

	CallMeFromConstructor();
}

// Constructor
AMass::AMass(float x, float y)
{
	//this->_m = 0;             // mass is always one
	this->_pos = AVector(x, y);
	this->_idx = -1;

	CallMeFromConstructor();
}

// Constructor
AMass::AMass(AVector pos)
{
	//this->_m     = 0;             // mass is always one
	this->_pos   = pos;
	this->_idx   = -1;

	CallMeFromConstructor();
}

void AMass::CallMeFromConstructor()
{
	// hard parameter
	_closestPt_fill_sz = 0;
	_closestPt_actual_sz = 50;
	_closestPoints = std::vector <AVector>(_closestPt_actual_sz);

	_isGrowing = false;

	_distToBoundary = 0.0f;

	_isFolded = false;
	//_selfIntersect = false;

	_mass = 1.0f;

	//this->_prevPos = this->_pos; // del
	_velocity = AVector(0, 0);
	_prevForce = AVector(0, 0);

	this->_avgEdgeLength = 0.0f;
	//this->_closestOtherGraphIdx = -1;
	//this->_closestSelfIdx = -1;
	this->_isInside = false;

	//this->_angleVel = AVector(0, 0);
	//this->_edgeVel = AVector(0, 0);
	//this->_repulsionVel = AVector(0, 0);
	//this->_boundaryVel = AVector(0, 0);

	//this->_attractionForce = AVector(0, 0);
	this->_edgeForce      = AVector(0, 0);
	this->_repulsionForce = AVector(0, 0);
	this->_boundaryForce  = AVector(0, 0);
	this->_overlapForce   = AVector(0, 0);
	//this->_noiseForce     = AVector(0, 0);
	this->_rotationForce = AVector(0, 0);
	this->_selfIntersectForce = AVector(0, 0);
	//this->_foldingForce = AVector(0, 0);

	
}

bool AMass::IsInsideTriangle(AVector pt, const std::vector<AMass>& otherMasses)
{
	for (unsigned a = 0; a < _triangles.size(); a++)
	{
		AVector t1 = otherMasses[_triangles[a].idx0]._pos;
		AVector t2 = otherMasses[_triangles[a].idx1]._pos;
		AVector t3 = otherMasses[_triangles[a].idx2]._pos;

		ABary bary = UtilityFunctions::Barycentric(pt, t1, t2, t3);

		if (bary.IsValid()) { return true; }

		//if (UtilityFunctions::PointInTriangle(t1, t2, t3, pt)) { return true; }
	}
	return false;
}


// true if accepted, false if rejected
bool AMass::TryToAddTriangleEdge(AnIndexedLine anEdge, const std::vector<AMass>& otherMasses)
{
	if (!FindTriEdge(anEdge))
	{
		// add edge
		// calculate distance
		AVector pt1 = otherMasses[anEdge._index0]._pos;
		AVector pt2 = otherMasses[anEdge._index1]._pos;
		anEdge.SetDist( pt1.Distance(pt2) ); 

		// add triangle edge
		if (anEdge._index0 != _idx) { anEdge.Swap(); }
		_triEdges.push_back(anEdge);

		// neighbor map
		_neighbors.insert({ anEdge._index1, anEdge._index1 });
	}
	return false;
}

bool AMass::FindTriEdge(AnIndexedLine anEdge)
{
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		if (_triEdges[a]._index0 == anEdge._index0 && _triEdges[a]._index1 == anEdge._index1) { return true; }
		if (_triEdges[a]._index1 == anEdge._index0 && _triEdges[a]._index0 == anEdge._index1) { return true; }
	}
	return false;
}

/*bool AMass::FindEdge(AnIndexedLine anEdge)
{
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
		if (_edges[a]._index0 == anEdge._index0 && _edges[a]._index1 == anEdge._index1) { return true; }
		if (_edges[a]._index1 == anEdge._index0 && _edges[a]._index0 == anEdge._index1) { return true; }
	}
	return false;
}*/

bool AMass::IsNeighbor(int idx)
{
	auto search = _neighbors.find(idx);
	return (search != _neighbors.end());
}

/*AVector AMass::GetClosestPointOnEdges(std::vector<AMass> otherMasses,
	                                  AVector pt, 
							          int ptIdx)
{
	float minDist = std::numeric_limits<float>::max();
	AVector closestPt(-1, -1);
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
		if (_edges[a]._index1 == ptIdx) { continue; } // make sure not neighbors

		AVector cPt = UtilityFunctions::ClosestPtAtFiniteLine(_pos, otherMasses[_edges[a]._index1]._pos, pt);
		float d = pt.Distance(cPt);
		if (d < minDist)
		{
			closestPt = cPt;
			minDist = d;
		}
	}
	return closestPt;
}*/

/*void AMass::AddEdge(AnIndexedLine anEdge)
{
	if (anEdge._index0 != _idx) { anEdge.Swap(); }
	_edges.push_back(anEdge);
}*/

/*void AMass::SortEdges(const std::vector<AMass>& otherMasses)
{
	AVector xVec(1, 0); // unit vector
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
		AVector p1 = this->_pos;
		AVector p2 = otherMasses[_edges[a]._index1]._pos;
		AVector dir = p1.DirectionTo(p2).Norm();
		_edges[a]._angle = UtilityFunctions::Angle2D(xVec.x, xVec.y, dir.x, dir.y);
		if (_edges[a]._angle < 0) { _edges[a]._angle = (M_PI * 2.0f) + _edges[a]._angle; }
	}
	std::sort(_edges.begin(), _edges.end(), CompareByAngle);
}*/

void AMass::Grow(float growth_scale_iter, float dt)
{
	this->_mass += growth_scale_iter * dt;

	
	// not used
	//_distToMidPoint = _oriDistToMidPoint * _mass;

	//for (unsigned int a = 0; a < _edges.size(); a++) { _edges[a].MakeLonger(growth_scale_iter, dt); }
	//for (unsigned int a = 0; a < _angleConnectors.size(); a++) { _angleConnectors[a].MakeLonger(growth_scale_iter, dt); }
	for (unsigned int a = 0; a < _triEdges.size();  a++) {  _triEdges[a].MakeLonger(  growth_scale_iter, dt);  }
	//for (unsigned int a = 0; a < _nnEdges.size();   a++) {  _nnEdges[a].MakeLonger(   growth_scale_iter, dt);  }
	//for (unsigned int a = 0; a < _moreEdges.size(); a++) {  _moreEdges[a].MakeLonger( growth_scale_iter, dt);  }
}

// NN EDGES
// should only be called when cloning a graph that is scaled or moved
void AMass::RecalculateEdgeLengths(const std::vector<AMass>& otherMasses)
{
}

/*void AMass::RecalculateEdgeLengths(std::vector<AMass>& otherMasses, float growFactor)
{
	for (int a = 0; a < _edges.size(); a++)
	{
		AVector otherPt = otherMasses[_edges[a]._index1]._pos;
		_edges[a]._dist = _pos.Distance(otherPt) * growFactor;
	}

	for (int a = 0; a < _nnEdges.size(); a++)
	{
		AVector otherPt = otherMasses[_nnEdges[a]._index1]._pos;
		_nnEdges[a]._dist = _pos.Distance(otherPt) * growFactor;
	}
}*/

void AMass::CalculateNNEdges(const std::vector<AMass>& otherMasses, int numBoundaryPoint)
{
	/*
	///////////////////////////////////////////////////
	if (this->_idx >= numBoundaryPoint) { return; }

	std::vector<AVector> points;
	for (unsigned int a = numBoundaryPoint; a < otherMasses.size(); a++)
		{ points.push_back(otherMasses[a]._pos); }

	NANOFLANNWrapper* knnStuff = new NANOFLANNWrapper();
	knnStuff->_leaf_max_size = 4;
	knnStuff->SetPointData(points);
	knnStuff->CreatePointKDTree();

	int neighborSize = 5;
	std::vector<int> neighborIndices = knnStuff->GetClosestIndices(_pos, neighborSize);

	for (unsigned int a = 0; a < neighborIndices.size(); a++)
	{
		if (IsNeighbor(neighborIndices[a])) { continue; }
		if (_nnEdges.size() >= 2) { break; }

		int newIdx = numBoundaryPoint + neighborIndices[a];
		AnIndexedLine anEdge(this->_idx, newIdx,
			this->_pos.Distance(otherMasses[newIdx]._pos));
		_nnEdges.push_back(anEdge);
	}
	///////////////////////////////////////////////////
	*/

	//std::cout << "num NN " << _nnEdges.size() << "\n";

	// knn
	/*std::vector<AVector> points;
	for (unsigned int a = 0; a < otherMasses.size(); a++)
		{ points.push_back(otherMasses[a]._pos); }

	NANOFLANNWrapper* knnStuff = new NANOFLANNWrapper();
	knnStuff->_leaf_max_size = 4;
	knnStuff->SetPointData(points);
	knnStuff->CreatePointKDTree();

	int neighborSize = SystemParams::_neighbor_size + 8;
	if (neighborSize > otherMasses.size()) { neighborSize = otherMasses.size(); }
	std::vector<int> neighborIndices = knnStuff->GetClosestIndices(_pos, neighborSize);

	for (unsigned int a = 0; a < neighborIndices.size(); a++)
	//for (int a = 0; a < otherMasses.size(); a++)
	{
		if (neighborIndices[a] == _idx) { continue;  }
		if (IsNeighbor(neighborIndices[a])) { continue; }
		if (_nnEdges.size() >= SystemParams::_neighbor_size) { break; }

		AnIndexedLine anEdge(this->_idx, neighborIndices[a], 
			                 this->_pos.Distance(otherMasses[neighborIndices[a]]._pos));

		_nnEdges.push_back(anEdge);
	}

	//std::cout << "num NN " << _nnEdges.size() << "\n";

	//if (_idx < numBoundaryPoint)
	{
		for (unsigned int a = 0; a < neighborIndices.size(); a++)
		{
			if (neighborIndices[a] == _idx) { continue; }
			if (IsNeighbor(neighborIndices[a])) { continue; }
			if (_moreEdges.size() >= SystemParams::_neighbor_size) { break; }

			AnIndexedLine anEdge(this->_idx, neighborIndices[a],
				this->_pos.Distance(otherMasses[neighborIndices[a]]._pos));

			_moreEdges.push_back(anEdge);
		}
	}*/
}

void AMass::GetClosestPoints2(int parentGraphIndex)
{
	if (parentGraphIndex < 0 || parentGraphIndex >= StuffWorker::_graphs.size()) { return; }
	if (this->_idx >= StuffWorker::_graphs[parentGraphIndex]._skinPointNum) { return; } // uncomment me

	

	//this->_closestGraphIndices.clear();
	this->_closestPt_fill_sz = 0;
	this->_isInside = false;           // "inside" flag
	this->_n_closest_elems = 0;
	
	//StuffWorker::_cGrid->GetGraphIndices2B(_pos.x, _pos.y, parentGraphIndex, _closestGraphIndices);
	GraphIndices* _closestGraphIndices;
	_closestGraphIndices = StuffWorker::_cGrid->GetGraphIndicesPtr(_pos.x, _pos.y, parentGraphIndex);

	this->_n_closest_elems = _closestGraphIndices->size();
	
	if (_closestGraphIndices->size() > 0)
	{
		std::vector<bool> insideGraphFlags;
		int sz = _closestGraphIndices->size();
		for (unsigned int a = 0; a < sz; a++)
		{
			// fake!!!
			if ((*_closestGraphIndices)[a] == parentGraphIndex) { insideGraphFlags.push_back(true); continue; }

			if (UtilityFunctions::InsidePolygon(StuffWorker::_graphs[(*_closestGraphIndices)[a]]._skin, _pos.x, _pos.y))
			{
				insideGraphFlags.push_back(true);
				_isInside = true;
				continue; // can be more than one
			}
			else { insideGraphFlags.push_back(false); }
		}

		// closest pts
		int sz2 = sz;
		if (sz2 > _closestPt_actual_sz) { sz2 = _closestPt_actual_sz; }
		for (unsigned int a = 0; a < sz2; a++)
		{
			if (insideGraphFlags[a]) { continue; }

			AVector pt = UtilityFunctions::GetClosestPtOnClosedCurve(StuffWorker::_graphs[(*_closestGraphIndices)[a]]._skin, _pos);
			_closestPoints[_closestPt_fill_sz] = pt;
			_closestPt_fill_sz++;
			//if (pt.IsInvalid()) { std::cout << "."; }
			//_closestPoints.push_back(pt);
		}
	}

	// this is used in AGraph
	_closestDist = std::numeric_limits<float>::max();
	for (unsigned int a = 0; a < _closestPt_fill_sz; a++)
	{
		float d = _closestPoints[a].DistanceSquared(_pos); // SQUARED!!!
		if (d < _closestDist)
		{
			_closestDist = d;
		}
	}
	_closestDist = std::sqrt(_closestDist); // SQRT
}

//void AMass::GetClosestPoints(int parentGraphIndex,
//		                     std::vector<AGraph>& allGraphs,
//							 std::vector<AVector> peaks)
//{
//	if (parentGraphIndex < 0 || parentGraphIndex >= allGraphs.size()) { return; }
//	//if (this->_idx >= allGraphs[parentGraphIndex]._boundaryPointNum) { return; } // uncomment me
//
//	this->_closestGraphIndices.clear();
//	this->_closestPoints.clear();
//	this->_isInside = false;           // "inside" flag
//
//	std::vector<AnObject*> qtObjects = _cGrid->GetObjects(_pos.x, _pos.y);
//
//	float minDist = std::numeric_limits<float>::max();
//	for (unsigned int a = 0; a < qtObjects.size(); a++)
//	{
//		// boundary only, from other graphs
//		if (qtObjects[a]->_info1 != parentGraphIndex)
//		{
//			_closestPoints.push_back(AVector(qtObjects[a]->_x, qtObjects[a]->_y));
//			if (UtilityFunctions::GetIndexFromIntList(_closestGraphIndices, qtObjects[a]->_info1) == -1)
//			{
//				_closestGraphIndices.push_back(qtObjects[a]->_info1);
//			}
//		}
//	}
//
//	if (_closestGraphIndices.size() > 0)
//	{
//		// isinside
//		//int insideGraphIdx = -1;
//		//std::vector<int> insideGraphIndices;
//		//std::vector<bool> insideGraphFlags;
//		for (unsigned int a = 0; a < _closestGraphIndices.size(); a++)
//		{
//			if (UtilityFunctions::InsidePolygon(allGraphs[_closestGraphIndices[a]]._skin, _pos))
//			{
//				//insideGraphFlags.push_back(true);
//				//insideGraphIdx = a;
//				_isInside = true;
//				//allGraphs[_closestGraphIndices[a]]._isGrowing = false; // important !!!
//				break; // can be more than one
//			}
//			else
//			{
//				//insideGraphFlags.push_back(false);
//			}
//		}
//
//		// closest pts
//		/*for (unsigned int a = 0; a < _closestGraphIndices.size(); a++)
//		{
//			if (insideGraphFlags[a]) { continue; }
//
//			AVector pt = UtilityFunctions::GetClosestPtOnClosedCurve(allGraphs[_closestGraphIndices[a]]._boundary, _pos);
//			_closestPoints.push_back(pt);
//		}*/
//	}
//
//	//std::cout << _closestPoints.size() << "\n";
//
//	/*_closestDist = std::numeric_limits<float>::max();
//	for (unsigned int a = 0; a < _closestPoints.size(); a++)
//	{
//		float d = _closestPoints[a].Distance(_pos);
//		if (d < _closestDist)
//		{
//			//_closestOtherGraphIdx = _closestGraphIndices[a];
//			_closestDist = d;
//		}
//	}*/
//
//	//std::cout << _closestDist << "\n";
//
//	// uncomment !!!
//	/*if (SystemParams::_activate_attraction_force)
//	{
//		_closestPeaks.clear();
//		for (unsigned int a = 0; a < peaks.size(); a++)
//		{
//			float d = peaks[a].Distance(_pos);
//			if (d < SystemParams::_peak_dist_stop * 4.0f)
//			{
//				_closestPeaks.push_back(peaks[a]);
//			}
//		}
//	}*/
//}

/*void AMass::CollissionRelax(int parentGraphIndex, std::vector<AnElement>& allGraphs)
{
	this->_isInside = false;

	int idx = -1;
	for (unsigned int a = 0; a < _closestGraphIndices.size(); a++)
	{
		if (UtilityFunctions::InsidePolygon(allGraphs[_closestGraphIndices[a]]._skin, _pos.x, _pos.y))
		{
			idx = _closestGraphIndices[a];
			_isInside = true;			
			break;
		}
	}

	// is inside
	if (idx >= 0)
	{
		AVector pt = UtilityFunctions::GetClosestPtOnClosedCurve(allGraphs[idx]._skin, _pos);
		//_prevPos = pt; // delete
		//_velocity = AVector(0, 0);
		_pos = pt;
	}

}*/

/*AVector AMass::GetClosestPointOnAnEdge(int parentGraphIndex,
		                            const std::vector<AGraph>& allGraphs,
		                            Quadtree* qt)
{
	_closestPt = AVector(-1, -1);
	_isInside = false;

	if (parentGraphIndex < 0 || parentGraphIndex >= allGraphs.size()) { return _closestPt; }

	int closestPointInfoIndex = -1;
	//int numCheck = 5;
	
	//std::vector<std::pair<int, int>> pointInfo = knn->GetClosestPairIndices(_pos, numCheck);
	std::vector<QTObject*> qtObjects = qt->GetObjectsAt(_pos.x, _pos.y);

	float minDist = std::numeric_limits<float>::max();
	//for (int a = 0; a < numCheck; a++)
	for (int a = 0; a < qtObjects.size(); a++)
	{
		if (qtObjects[a]->info1 != parentGraphIndex)
		{
			//AnIndexedLine anEdge = allGraphs[pointInfo[a].first]._edges[pointInfo[a].second];
			AnIndexedLine anEdge = allGraphs[qtObjects[a]->info1]._boundaryEdges[qtObjects[a]->info2];
			AVector pt1 = allGraphs[qtObjects[a]->info1]._massList[anEdge._index0]._pos;
			AVector pt2 = allGraphs[qtObjects[a]->info1]._massList[anEdge._index1]._pos;
			AVector cPt = UtilityFunctions::ClosestPtAtFiniteLine(pt1, pt2, _pos);

			float d = cPt.Distance(_pos); // distance to current pos
			if (d < minDist)
			{
				_closestPt = cPt;
				minDist = d;
				closestPointInfoIndex = a;
			}
		}
	}

	if (!_closestPt.IsInvalid())
	{
		_isInside = UtilityFunctions::InsidePolygon(allGraphs[qtObjects[closestPointInfoIndex]->info1]._boundary, _pos);
	}
}*/

/*AVector AMass::GetClosestPointOnAnEdge(int parentGraphIndex,
		                               const std::vector<AGraph>& allGraphs,
		                               NANOFLANNWrapper* knn)
{
	_closestPt = AVector(-1, -1);
	_isInside = false;

	if (parentGraphIndex < 0 || parentGraphIndex >= allGraphs.size()) { return _closestPt; }

	_closestGraphIdx = -1;
	int closestPointInfoIndex = -1;
	int numCheck = 5;
	std::vector<std::pair<int, int>> pointInfo = knn->GetClosestPairIndices(_pos, numCheck);

	float minDist = std::numeric_limits<float>::max();
	for (int a = 0; a < numCheck; a++)
	{
		if (pointInfo[a].first != parentGraphIndex)
		{
			//AnIndexedLine anEdge = allGraphs[pointInfo[a].first]._edges[pointInfo[a].second];
			AnIndexedLine anEdge = allGraphs[pointInfo[a].first]._boundaryEdges[pointInfo[a].second];
			AVector pt1 = allGraphs[pointInfo[a].first]._massList[anEdge._index0]._pos;
			AVector pt2 = allGraphs[pointInfo[a].first]._massList[anEdge._index1]._pos;
			AVector cPt = UtilityFunctions::ClosestPtAtFiniteLine(pt1, pt2, _pos);

			float d = cPt.Distance(_pos); // distance to current pos
			if (d < minDist)
			{
				_closestPt = cPt;
				minDist = d;
				closestPointInfoIndex = a;
			}
		}
	}

	if (!_closestPt.IsInvalid())
	{
		_isInside = UtilityFunctions::InsidePolygon(allGraphs[pointInfo[closestPointInfoIndex].first]._boundary, _pos);
		_closestGraphIdx = pointInfo[closestPointInfoIndex].first;
	}	
}*/

// this is not sync'ed
void AMass::VerletSyncEdgeLengths(const std::vector<AMass>& otherMasses)
{
	/*for (int a = 0; a < _edges.size(); a++) // n neighbors
	{
		AVector pt1 = otherMasses[_edges[a]._index0]._pos;
		AVector pt2 = otherMasses[_edges[a]._index1]._pos;
		_edges[a]._dist = pt1.Distance(pt2);
	}*/
}


//void AMass::VerletRelax(std::vector<AMass>& otherMasses)
//{
//	for (unsigned int a = 0; a < _angleConnectors.size(); a++)
//	{
		/*AVector pt1    = otherMasses[_angleConnectors[a]._index0]._pos;
		AVector pt2    = otherMasses[_angleConnectors[a]._index1]._pos;
		float dist     = pt1.Distance(pt2);
		
		float distDiff = dist - _angleConnectors[a]._dist;

		float distDiffHalf = distDiff * 0.5f;

		AVector dir    = pt1.DirectionTo(pt2).Norm();

		// assume you want to make them closer
		otherMasses[_angleConnectors[a]._index0]._pos += dir * distDiffHalf;
		otherMasses[_angleConnectors[a]._index1]._pos -= dir * distDiffHalf;

		// assume you want to make them closer
		otherMasses[_angleConnectors[a]._index0]._prevPos += dir * distDiffHalf;
		otherMasses[_angleConnectors[a]._index1]._prevPos -= dir * distDiffHalf;*/
//	}
//}

//void AMass::AngleRelax(std::vector<AMass>& otherMasses)
//{
//	std::vector<int> randomIndices;
//	for (unsigned int a = 0; a < _angleConnectors.size(); a++) { randomIndices.push_back(a); }
//
//	std::random_shuffle(randomIndices.begin(), randomIndices.end());
//
//	for (unsigned int a = 0; a < _angleConnectors.size(); a++)
//	{
//
//		AngleConnector ac = _angleConnectors[randomIndices[a]];
//
//
//		if (otherMasses[ac._index0]._isInside || otherMasses[ac._index1]._isInside) { continue; }
//
//		AVector pt0 = otherMasses[ac._index0]._pos;
//		AVector pt1 = otherMasses[ac._index1]._pos;
//		AVector dirA = _pos.DirectionTo(pt0).Norm(); // left point
//		AVector dirB = _pos.DirectionTo(pt1).Norm(); // right point
//		float angle = UtilityFunctions::Angle2D(dirA.x, dirA.y, dirB.x, dirB.y);
//		if (angle < 0) { angle = (M_PI * 2.0f) + angle; }
//
//		float radThreshold = 0.523599; // 30 degree
//		float angleDif = angle - ac._angle;
//
//		if (std::abs(angleDif) < radThreshold) { continue; }
//
//		// assume we need to decrease
//		float radVal = angleDif - radThreshold;
//		if (angleDif < 0) { radVal = angleDif + radThreshold; }
//
//		float numEdges = otherMasses[ac._index0]._edges.size() + otherMasses[ac._index1]._edges.size();
//		float a0 = ((float)otherMasses[ac._index1]._edges.size()) / numEdges;
//		float a1 = 1.0 - a0;
//
//		AVector pt0_prime = UtilityFunctions::Rotate(pt0, _pos, radVal * a0);
//		AVector pt1_prime = UtilityFunctions::Rotate(pt1, _pos, -radVal * a1);
//		otherMasses[ac._index0]._pos = pt0_prime;
//		//otherMasses[ac._index0]._prevPos = pt0_prime; // delete
//		otherMasses[ac._index0]._velocity = AVector(0, 0);
//		otherMasses[ac._index1]._pos = pt1_prime;
//		//otherMasses[ac._index1]._prevPos = pt1_prime; // delete
//		otherMasses[ac._index1]._velocity = AVector(0, 0);
//
//		/*if (angle > M_PI && angle < M_PI + (M_PI / 2.0f))
//		{
//			// 180 degree upper bound 
//			float halfRad = (M_PI - angle) / 2.0f;
//			AVector pt0_prime = UtilityFunctions::Rotate(pt0, _pos,  halfRad);
//			AVector pt1_prime = UtilityFunctions::Rotate(pt1, _pos, -halfRad);
//
//			otherMasses[ac._index0]._pos = pt0_prime;
//			otherMasses[ac._index0]._prevPos = pt0_prime;
//			otherMasses[ac._index1]._pos = pt1_prime;
//			otherMasses[ac._index1]._prevPos = pt1_prime;
//		}
//		else if(angle > M_PI + (M_PI / 2.0f))
//		{
//			// 10 degree lower bound
//			float halfRad = ((M_PI * 2.0f + 0.174533) - angle) / 2.0f;
//			AVector pt0_prime = UtilityFunctions::Rotate(pt0, _pos, -halfRad);
//			AVector pt1_prime = UtilityFunctions::Rotate(pt1, _pos,  halfRad);
//
//			otherMasses[ac._index0]._pos = pt0_prime;
//			otherMasses[ac._index0]._prevPos = pt0_prime;
//			otherMasses[ac._index1]._pos = pt1_prime;
//			otherMasses[ac._index1]._prevPos = pt1_prime;
//		}*/
//
//	}
//}

void AMass::CalculateFoldingForce(const std::vector<AMass>& otherMasse)
{
	/*

	// zero velocity
	//_prevPos = _pos; // delete
	//_velocity = AVector(0, 0);

	float k_folding = SystemParams::_k_folding;
	//std::cout << k_folding << "\n";
	
	for (unsigned int a = 0; a < _nnEdges.size(); a++) // n neighbors
	{
		AVector otherPt = otherMasse[_nnEdges[a]._index1]._pos;
		float dist = _pos.Distance(otherPt);

		// assume we need to make the dist shorter
		AVector dir = _pos.DirectionTo(otherPt).Norm();
		AVector fForce = (dir * k_folding *  (dist - _nnEdges[a].GetDist()));
		if (!fForce.IsBad()) // check if NaN or Inf
		{
			this->_foldingForce += fForce;
		}
	}

	*/
}


void AMass::Solve(const int& massNumber,
	              const AnElement& parentGraph,
				  const std::vector<std::vector<AVector>>& boundaries,
				  const std::vector<std::vector<AVector>>& holes,
				  const std::vector<std::vector<AVector>>& focalOffsets)
{

	if (massNumber < parentGraph._skinPointNum)
	{
		if (_n_closest_elems > 0)
		{
			if (_isInside)
			{
				// ---------- OVERLAP FORCE ----------
				AVector sumO(0, 0);
				AVector ctrPt;
				AVector dir;
				for (unsigned int a = 0; a < _triangles.size(); a++)
				{
					ctrPt = (parentGraph._massList[_triangles[a].idx0].GetPos() +        // triangle vertex
						               parentGraph._massList[_triangles[a].idx1].GetPos() +        // triangle vertex
									   parentGraph._massList[_triangles[a].idx2].GetPos()) / 3.0f; // triangle vertex

					dir = _pos.DirectionTo(ctrPt);
					sumO += dir;
				}
				sumO *= SystemParams::_k_overlap;
				if (!sumO.IsBad()) { this->_overlapForce += sumO; }
			}
			else
			{
				// ---------- REPULSION FORCE ----------
				AVector sumR(0, 0);
				AVector dir;
				for (int a = 0; a < _closestPt_fill_sz; a++)
				{
					dir = _closestPoints[a].DirectionTo(_pos); // direction, normalized
					float dist = dir.Length(); // distance
					sumR += (dir.Norm() / (SystemParams::_repulsion_soft_factor + std::pow(dist, 2)));
				}
				sumR *= SystemParams::_k_repulsion;
				if (!sumR.IsBad()) { this->_repulsionForce += sumR; }

			}
		}
	}

	bool nearBoundary = _cGrid->NearBoundary(_pos.x, _pos.y);

	if (nearBoundary)
	{
		// ---------- BOUNDARY FORCE ----------
		float k_boundary = SystemParams::_k_boundary;
		// ===== MULTIPLE CONTAINERS =====
		if (!UtilityFunctions::InsidePolygons(boundaries, _pos.x, _pos.y))
		//if (!UtilityFunctions::InsidePolygon(boundaries[0], _pos)) // only consider when it is outside
		{
			AVector cPt;
			float ddd = std::numeric_limits<float>::max();
			AVector ccc;
			for (unsigned a = 0; a < boundaries.size(); a++)
			{
				ccc = UtilityFunctions::GetClosestPtOnClosedCurve(boundaries[a], _pos);
				float distdist = ccc.Distance(_pos);
				if (distdist < ddd)
				{
					ddd = distdist;
					cPt = ccc;
				}
			}
			//UtilityFunctions::GetClosestPtOnClosedCurve(boundaries, _pos); // expensive 
			AVector dirDist = _pos.DirectionTo(cPt); // not normalized
			AVector bForce  = dirDist * k_boundary;
			if (!bForce.IsBad()) { this->_boundaryForce += bForce; }  // apply
		}
	
		// ===== MULTIPLE CONTAINERS =====
		// focal
		/*for (int a = 0; a < focalOffsets.size(); a++)
		{
			AVector sumO(0, 0);
			if (UtilityFunctions::InsidePolygon(focalOffsets[a], _pos))
			{
				for (unsigned int b = 0; b < _triangles.size(); b++)
				{
					AVector ctrPt = (parentGraph._massList[_triangles[b].idx0]._pos +        // triangle vertex
						parentGraph._massList[_triangles[b].idx1]._pos +        // triangle vertex
						parentGraph._massList[_triangles[b].idx2]._pos) / 3.0f; // triangle vertex

					AVector dir = _pos.DirectionTo(ctrPt);
					sumO += dir;
				}
			}
			sumO *= SystemParams::_k_overlap;
			if (!sumO.IsBad()) { this->_boundaryForce += sumO; }
		}*/

		// holes
		if (UtilityFunctions::InsidePolygons(holes, _pos.x, _pos.y))
		{
			AVector cPt;
			float ddd = std::numeric_limits<float>::max();
			AVector ccc;
			for (unsigned a = 0; a < holes.size(); a++)
			{
				ccc = UtilityFunctions::GetClosestPtOnClosedCurve(holes[a], _pos);
				float distdist = ccc.Distance(_pos);
				if (distdist < ddd)
				{
					ddd = distdist;
					cPt = ccc;
				}
			}
			//UtilityFunctions::GetClosestPtOnClosedCurve(boundaries, _pos); // expensive 
			AVector dirDist = _pos.DirectionTo(cPt); // not normalized
			AVector bForce = dirDist * k_boundary;
			if (!bForce.IsBad()) { this->_boundaryForce += bForce; }  // apply
		}
	}
}

/*
*/
void AMass::Init()
{
	//_attractionForce = AVector(0, 0);
	this->_edgeForce      = AVector(0, 0);
	this->_repulsionForce = AVector(0, 0);
	this->_boundaryForce  = AVector(0, 0);
	this->_overlapForce   = AVector(0, 0);
	//this->_noiseForce     = AVector(0, 0);
	this->_rotationForce = AVector(0, 0);
	this->_selfIntersectForce = AVector(0, 0);
	//_foldingForce   = AVector(0, 0);
}

/*
verlet
*/
void AMass::UpdatePos(float dt, float dtdt_half)
{
	// dtdt_half = dt * dt * 0.5
	_pos += _velocity * dt + _prevForce * dtdt_half;
}

/*
verlet

dt_half = dt * 0.5
*/ 
void AMass::UpdateVelocity(float dt_half, float dampingVal)
{
	AVector curForce = _edgeForce + _repulsionForce + _boundaryForce + _overlapForce /*+ _noiseForce*/ + _rotationForce + _selfIntersectForce;
	//curForce /= this->_mass; // DISABLING MASS

	_velocity += (_prevForce + curForce) * dt_half;
	if (_velocity.Length() < dampingVal) { _velocity = AVector(0, 0); }
	else { _velocity -= _velocity.Norm() * dampingVal; }

	_prevForce = curForce;
}

/*
Oiler method
*/
void AMass::Simulate(float dt/*, float dampingVal*/)
{
	
	// oiler
	_velocity += ((/*_attractionForce + */ _edgeForce      + 
		                                   _repulsionForce + 
		                                   _boundaryForce  + 
		                                   _overlapForce   + 
		                                   //_noiseForce     + 
		                                   _rotationForce  + 
		                                   _selfIntersectForce) * dt);
	float len = _velocity.Length();

	float capVal = SystemParams::_velocity_cap * dt; 
	
	if (len > capVal)
	{
		_velocity = _velocity.Norm() * capVal;
	}

	_pos = _pos + _velocity * dt;

}

// I believe this is never used
void AMass::DrawForce()
{
	/*AVector allForces = _attractionForce + _edgeForce + _boundaryForce + _repulsionForce;

	// draw angle force
	int forceScale = 0.1f;
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 1.0);
	glVertex2f(_pos.x, _pos.y);
	glColor3f(1.0, 0.0, 0.0);
	glVertex2f(_pos.x + (allForces.x * forceScale), _pos.y + (allForces.y * forceScale));
	//glVertex2f(_pos.x + (_angleVel.x * forceScale), _pos.y + (_angleVel.y * forceScale));
	glEnd();*/
	
}

void AMass::Draw()
{	
	/*
	glLineWidth(0.5f);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	
	glVertex2f(_pos.x, _pos.y);
	glVertex2f(_lineSgment[0].x, _lineSgment[0].y);

	glVertex2f(_pos.x, _pos.y);
	glVertex2f(_lineSgment[2].x, _lineSgment[2].y);

	glEnd();
	*/

	/*glLineWidth(0.5f);
	glBegin(GL_LINES);
	for (unsigned int a = 0; a < _nnEdges.size(); a++)
	{
		AVector pt = otherMasses[_nnEdges[a]._index1]._pos;

		glColor3f(0.0, 0.0, 0.0);
		glVertex2f(_pos.x, _pos.y);

		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(pt.x, pt.y);
	}
	glEnd();
	*/
}

//std::vector<int> _segmentIndices;
//std::vector<AVector> _lineSgment;

void AMass::CalculateIndicesOfLineSegment(int numSkin)
{
	_segmentIndices = std::vector<int>(3);
	_segmentIndices[1] = _idx;
	_segmentIndices[0] = _idx - 1;
	_segmentIndices[2] = _idx + 1;
	if (_segmentIndices[0] < 0) { _segmentIndices[0] = numSkin - 1; }
	if (_segmentIndices[2] == numSkin) { _segmentIndices[2] = 0; }

	_lineSgment = std::vector<AVector>(3);
}

void AMass::UpdateLineSegment(const std::vector<AMass>& otherMasses)
{
	_lineSgment[0] = otherMasses[_segmentIndices[0]]._pos;
	_lineSgment[1] = _pos;
	_lineSgment[2] = otherMasses[_segmentIndices[2]]._pos;
}