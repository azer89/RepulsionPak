
#include "AMass.h"
#include "AnElement.h"
#include "AVector.h"
#include "StuffWorker.h"
#include "SystemParams.h"

#include "glew.h"
#include "freeglut.h"

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
	this->_pos = AVector(0, 0);
	this->_idx = -1;
	//this->_cellIdx = -1;

	CallMeFromConstructor();
}

// Constructor
AMass::AMass(const float& x, const float& y)
{
	//this->_m = 0;             // mass is always one
	this->_pos = AVector(x, y);
	this->_idx = -1;

	CallMeFromConstructor();
}

// Constructor
AMass::AMass(const AVector& pos)
{
	//this->_m     = 0;             // mass is always one
	this->_pos = pos;
	this->_idx = -1;

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
	//_prevForce = AVector(0, 0);

	this->_avgEdgeLength = 0.0f;
	this->_isInside = false;

	this->_edgeForce = AVector(0, 0);
	this->_repulsionForce = AVector(0, 0);
	this->_boundaryForce = AVector(0, 0);
	this->_overlapForce = AVector(0, 0);
	this->_rotationForce = AVector(0, 0);
	this->_selfIntersectForce = AVector(0, 0);
}

bool AMass::IsInsideTriangle(const AVector& pt, const std::vector<AMass>& otherMasses)
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
		anEdge.SetDist(pt1.Distance(pt2));

		// add triangle edge
		if (anEdge._index0 != _idx) { anEdge.Swap(); }
		_triEdges.push_back(anEdge);

		// neighbor map
		_neighbors.insert({ anEdge._index1, anEdge._index1 });
	}
	return false;
}

bool AMass::FindTriEdge(const AnIndexedLine& anEdge)
{
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		if (_triEdges[a]._index0 == anEdge._index0 && _triEdges[a]._index1 == anEdge._index1) { return true; }
		if (_triEdges[a]._index1 == anEdge._index0 && _triEdges[a]._index0 == anEdge._index1) { return true; }
	}
	return false;
}

bool AMass::IsNeighbor(int idx)
{
	auto search = _neighbors.find(idx);
	return (search != _neighbors.end());
}

void AMass::GetClosestPoints2(const int& parentGraphIndex)
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
			AVector bForce = dirDist * k_boundary;
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
	this->_edgeForce.SetZero();
	this->_repulsionForce.SetZero();
	this->_boundaryForce.SetZero();
	this->_overlapForce.SetZero();
	this->_rotationForce.SetZero();
	this->_selfIntersectForce.SetZero();
}

/*
Oiler method
*/
void AMass::Simulate(float dt/*, float dampingVal*/)
{
	// oiler
	_velocity += ((/*_attractionForce + */ _edgeForce +
		_repulsionForce +
		_boundaryForce +
		_overlapForce +
		//_noiseForce     + 
		_rotationForce +
		_selfIntersectForce) * dt);
	float len = _velocity.Length();

	float capVal = SystemParams::_velocity_cap * dt;

	if (len > capVal)
	{
		_velocity = _velocity.Norm() * capVal;
	}

	_pos = _pos + _velocity * dt;

}

void AMass::Grow(const float& growth_scale_iter, const float& dt)
{
	this->_mass += growth_scale_iter * dt;


	// not used
	//_distToMidPoint = _oriDistToMidPoint * _mass;

	//for (unsigned int a = 0; a < _edges.size(); a++) { _edges[a].MakeLonger(growth_scale_iter, dt); }
	//for (unsigned int a = 0; a < _angleConnectors.size(); a++) { _angleConnectors[a].MakeLonger(growth_scale_iter, dt); }
	for (unsigned int a = 0; a < _triEdges.size(); a++) { _triEdges[a].MakeLonger(growth_scale_iter, dt); }
	//for (unsigned int a = 0; a < _nnEdges.size();   a++) {  _nnEdges[a].MakeLonger(   growth_scale_iter, dt);  }
	//for (unsigned int a = 0; a < _moreEdges.size(); a++) {  _moreEdges[a].MakeLonger( growth_scale_iter, dt);  }
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

void AMass::CalculateIndicesOfLineSegment(const int& numSkin)
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