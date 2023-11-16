
#include "AnElement.h"
#include "ARectangle.h"

#include "ColorPalette.h"

#include "CGALTriangulation2.h"

#include "StuffWorker.h"

#include "CSSSmoothing.h"

//#include "OpenCVWrapper.h"

#include <random>
#include <algorithm>

#define PI 3.14159265359
#define PI2 6.28318530718

//std::vector<AMass> AGraph::_massList = std::vector<AMass>();

// constructir
AnElement::AnElement()
{
	this->_isMatched = false;
	this->_oriSkiOffset = 0;
	this->_oriArea = 0;
	this->_scale = 1.0f;
	this->_id = -1;
	this->_ori_id = -1;
	this->_isGrowing = true;
	this->_averageEdgeLength = 0;
	//this->_minEdgeLength = 0;
	this->_oriAvgEdgeLength = 0;
	this->_sumVelocity = 0;
	this->_bigOne = true; // main elements, not small elements
	this->_isRigid = false;
}

//AnElement& AnElement::operator=(const AnElement& other)
//{
//	if (this != &other) // not a self-assignment
//	{
//		/*if (size != other.size) // resource cannot be reused
//		{
//			data.reset(new int[other.size]);
//			size = other.size;
//		}
//		std::copy(&other.data[0], &other.data[0] + size, &data[0]);*/
//
//
//
//	}
//	return *this;
//}


float AnElement::DistanceToBoundary(AVector pt)
{
	return UtilityFunctions::DistanceToClosedCurve(_uniArt, pt);
}

void AnElement::CalculateCentroid()
{
	//std::vector<AMass>::const_iterator first_iter = _massList.begin();
	//std::vector<AMass>::const_iterator last_iter = _massList.begin() + _skinPointNum;
	//vector<T> newVec(first, last);
	std::vector<AVector> boundary_test;
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		boundary_test.push_back(_massList[a]._pos);
	}
	this->_centroid = _cvWrapper.GetCenter(boundary_test);
}

void AnElement::CalculateVecToCentroidArray()
{
	//
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		_normFromCentroidArray.push_back((_massList[a]._pos - _centroid).Norm());
		_rotateArray.push_back(AVector(0, 0));
	}
}

float AnElement::GetArea()
{
	return _cvWrapper.GetArea(this->_skin);
}

void AnElement::CalculateSumVelocity()
{
	_sumVelocity = 0;
	for (unsigned int b = 0; b < _massList.size(); b++)
	{
		_sumVelocity += _massList[b]._velocity.Length();
	}
}

void AnElement::TesselateArts()
{

	for (int a = 0; a < _arts.size(); a++)
	{
		std::vector<AVector> ggg;
		UtilityFunctions::UniformResampleClosed(_arts[a], ggg, 2.0f);
		_tesselateArts.push_back(ggg);
	}

	CGALTriangulation2 cTri;

	for (int a = 0; a < _tesselateArts.size(); a++)
	{
		ArtTri aT;
		cTri.Triangulate(_tesselateArts[a], aT);
		_artTris.push_back(aT);
	}
}

void AnElement::ComputeTesselateBarycentric()
{

}

void AnElement::CalculatePADBary()
{
	// calculate triangles
	std::vector<std::vector<AVector>> actualTriangles;
	for (unsigned int c = 0; c < _triangles.size(); c++)
	{
		std::vector<AVector> tri(3);
		tri[0] = _massList[_triangles[c].idx0]._pos;
		tri[1] = _massList[_triangles[c].idx1]._pos;
		tri[2] = _massList[_triangles[c].idx2]._pos;
		actualTriangles.push_back(tri);
	}
	_padCalc.CalculateBarycentric(actualTriangles);
}

void AnElement::ComputeBarycentric()
{
	// calculate triangles
	std::vector<std::vector<AVector>> actualTriangles;
	for (unsigned int c = 0; c < _triangles.size(); c++)
	{
		std::vector<AVector> tri(3);
		tri[0] = _massList[_triangles[c].idx0]._pos;
		tri[1] = _massList[_triangles[c].idx1]._pos;
		tri[2] = _massList[_triangles[c].idx2]._pos;
		actualTriangles.push_back(tri);
	}

	//  ================================================  
	// arts
	_arts2Triangles.clear();
	_baryCoords.clear();
	for (unsigned int a = 0; a < _arts.size(); a++)
	{
		std::vector<int> a2t;
		std::vector<ABary> bCoords;
		for (unsigned int b = 0; b < _arts[a].size(); b++)
		{
			int triIdx = -1;
			ABary bary;
			for (unsigned int c = 0; c < _triangles.size(); c++)
			{
				if (UtilityFunctions::InsidePolygon(actualTriangles[c], _arts[a][b].x, _arts[a][b].y))
				{
					triIdx = c;
					break;
				}
			}

			if (triIdx == -1)
			{
				std::cout << "art error !!!\n";

				triIdx = -1;
				float dist = 100000000;
				for (unsigned int c = 0; c < _triangles.size(); c++)
				{
					float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _arts[a][b]);
					if (d < dist)
					{
						dist = d;
						triIdx = c;
					}
				}
			}

			//else
			{
				bary = UtilityFunctions::Barycentric(_arts[a][b],
					actualTriangles[triIdx][0],
					actualTriangles[triIdx][1],
					actualTriangles[triIdx][2]);
			}
			bCoords.push_back(bary);
			a2t.push_back(triIdx);
		}
		_baryCoords.push_back(bCoords);
		_arts2Triangles.push_back(a2t);
	}

	// ============================================================== 
	// centroid
	_centroidTriIdx = -1;
	for (unsigned int c = 0; c < _triangles.size(); c++)
	{
		if (UtilityFunctions::InsidePolygon(actualTriangles[c], _centroid.x, _centroid.y))
		{
			_centroidTriIdx = c;
			break;
		}
	}

	if (_centroidTriIdx == -1)
	{
		std::cout << "centroid bary error!!!\n";
		_centroidTriIdx = -1;
		float dist = 100000000;
		for (unsigned int c = 0; c < _triangles.size(); c++)
		{
			float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], AVector(_centroid.x, _centroid.y));
			if (d < dist)
			{
				dist = d;
				_centroidTriIdx = c;
			}
		}
	}

	_centroidBary = UtilityFunctions::Barycentric(_centroid,
		actualTriangles[_centroidTriIdx][0],
		actualTriangles[_centroidTriIdx][1],
		actualTriangles[_centroidTriIdx][2]);


	// ============================================================== 
	// tesselate art
	_tesselateArts2Triangles.clear();
	_tesselateBaryCoords.clear();
	for (unsigned int a = 0; a < _tesselateArts.size(); a++)
	{
		std::vector<int> a2t;
		std::vector<ABary> bCoords;
		for (unsigned int b = 0; b < _tesselateArts[a].size(); b++)
		{
			int triIdx = -1;
			ABary bary;
			for (unsigned int c = 0; c < _triangles.size(); c++)
			{
				if (UtilityFunctions::InsidePolygon(actualTriangles[c], _tesselateArts[a][b].x, _tesselateArts[a][b].y))
				{
					triIdx = c;
					break;
				}
			}

			if (triIdx == -1)
			{
				std::cout << "tesselate error !!!\n";

				triIdx = -1;
				float dist = 100000000;
				for (unsigned int c = 0; c < _triangles.size(); c++)
				{
					float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _tesselateArts[a][b]);
					if (d < dist)
					{
						dist = d;
						triIdx = c;
					}
				}
			}

			//else
			{
				bary = UtilityFunctions::Barycentric(_tesselateArts[a][b],
					actualTriangles[triIdx][0],
					actualTriangles[triIdx][1],
					actualTriangles[triIdx][2]);
			}
			bCoords.push_back(bary);
			a2t.push_back(triIdx);
		}
		_tesselateBaryCoords.push_back(bCoords);
		_tesselateArts2Triangles.push_back(a2t);
	}

	// ============================================================== 
	// uni art
	_uniArt2Triangles.clear();
	_uniBaryCoords.clear();
	for (unsigned int a = 0; a < _uniArt.size(); a++)
	{
		int triIdx = -1;
		ABary bary;
		for (unsigned int c = 0; c < _triangles.size(); c++)
		{
			if (UtilityFunctions::InsidePolygon(actualTriangles[c], _uniArt[a].x, _uniArt[a].y))
			{
				triIdx = c;
				break;
			}
		}
		if (triIdx == -1)
		{
			std::cout << "uniArt err! ";

			triIdx = -1;
			float dist = 100000000;
			for (unsigned int c = 0; c < _triangles.size(); c++)
			{
				float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _uniArt[a]);
				if (d < dist)
				{
					dist = d;
					triIdx = c;
				}
			}
		}
		//else

		//{
		bary = UtilityFunctions::Barycentric(_uniArt[a],
			actualTriangles[triIdx][0],
			actualTriangles[triIdx][1],
			actualTriangles[triIdx][2]);
		//}
		_uniBaryCoords.push_back(bary);
		_uniArt2Triangles.push_back(triIdx);
	}


	//  ================================================  
	// uni uni arts
	_uniuniArts2Triangles.clear();
	_uniuniBaryCoords.clear();
	for (unsigned int a = 0; a < _uniuniArts.size(); a++)
	{
		std::vector<int> a2t;
		std::vector<ABary> bCoords;
		for (unsigned int b = 0; b < _uniuniArts[a].size(); b++)
		{
			int triIdx = -1;
			ABary bary;
			for (unsigned int c = 0; c < _triangles.size(); c++)
			{
				if (UtilityFunctions::InsidePolygon(actualTriangles[c], _uniuniArts[a][b].x, _uniuniArts[a][b].y))
				{
					triIdx = c;
					break;
				}
			}

			if (triIdx == -1)
			{
				std::cout << "art error !!!\n";

				triIdx = -1;
				float dist = 100000000;
				for (unsigned int c = 0; c < _triangles.size(); c++)
				{
					float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _uniuniArts[a][b]);
					if (d < dist)
					{
						dist = d;
						triIdx = c;
					}
				}
			}

			//else
			{
				bary = UtilityFunctions::Barycentric(_uniuniArts[a][b],
					actualTriangles[triIdx][0],
					actualTriangles[triIdx][1],
					actualTriangles[triIdx][2]);
			}
			bCoords.push_back(bary);
			a2t.push_back(triIdx);
		}
		_uniuniBaryCoords.push_back(bCoords);
		_uniuniArts2Triangles.push_back(a2t);
	}
}

void AnElement::CalculateOriAvgEdgeLength()
{
	_oriAvgEdgeLength = 0;
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		_oriAvgEdgeLength += _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
	}
	_oriAvgEdgeLength /= (float)_triEdges.size();

	/*for (unsigned int a = 0; a < _edges.size(); a++)
		{ _oriAvgEdgeLength += _massList[_edges[a]._index0]._pos.Distance(_massList[_edges[a]._index1]._pos); }
	_oriAvgEdgeLength /= (float)_edges.size();*/
}

void AnElement::RemoveShortNegSpaceEdges()
{
	float avgEdgeLength = 0;
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		avgEdgeLength += _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
	}
	avgEdgeLength /= (float)_triEdges.size();

	//std::cout << "  avg edge length : " << _averageEdgeLength << "\n";
	for (int a = _negSpaceEdges.size() - 1; a >= 0; a--)
	{
		AVector pt1 = _massList[_negSpaceEdges[a]._index0]._pos;
		AVector pt2 = _massList[_negSpaceEdges[a]._index1]._pos;

		//std::cout << avgEdgeLength << "  " << pt1.Distance(pt2) << "\n";

		if (pt1.Distance(pt2) < avgEdgeLength * SystemParams::_self_intersection_threshold)
		{
			_negSpaceEdges.erase(_negSpaceEdges.begin() + a);
		}
	}
}

void AnElement::CalculateSmoothSkin()
{
	// ----------  ----------
	UtilityFunctions::UniformResampleClosed(_uniArt, _resampled_skin, SystemParams::_resample_gap_float);

	// ---------- gaussian smoothing ----------
	CSSSmoothing css;
	css.Init1(SystemParams::_gaussian_smoothing_element);
	_smooth_skin = _resampled_skin;
	css.SmoothCurve1(_smooth_skin);
}

void AnElement::CalculatePAD()
{
	// PAD and friends
	_skin_length = UtilityFunctions::CurveLengthClosed(_skin); // 1

	// ---------- resampled ----------
	UtilityFunctions::UniformResampleClosed(_uniArt, _resampled_skin, SystemParams::_resample_gap_float); // 1
	//float gap = SystemParams::_resample_gap_float * 5;
	//UtilityFunctions::UniformResampleClosed(_uniArt, _resampled_skin, gap);
	//std::cout << "element resampled gap: " << gap << "\n";

	// ---------- gaussian smoothing ----------
	CSSSmoothing css;
	css.Init1(SystemParams::_gaussian_smoothing_element);
	_smooth_skin = _resampled_skin;
	css.SmoothCurve1(_smooth_skin);

	// ---------- RDP (not used) ----------
	_rdpFlags_skin = std::vector<bool>(_resampled_skin.size());
	for (unsigned int a = 0; a < _rdpFlags_skin.size(); a++) { _rdpFlags_skin[a] = true; }
	//CurveRDP::SimplifyRDPFlags(_resampled_container, _rdpFlags, SystemParams::_rdp_epsilon);
	//CurveRDP::SimplifyRDP(_resampled_container, _rdp_container, SystemParams::_rdp_epsilon);

	// ---------- PAD ----------
	_padCalc._isElement = true;
	_padCalc.ComputePAD(_smooth_skin, _rdpFlags_skin);

	/*
	std::vector<ABary>	_padBarys;
	std::vector<int>	_padTriIdxs;
	*/
	// pad debug delete me
	// calculate triangles

	/*std::vector<std::vector<AVector>> actualTriangles;
	for (unsigned int c = 0; c < _triangles.size(); c++)
	{
		std::vector<AVector> tri(3);
		tri[0] = _massList[_triangles[c].idx0]._pos;
		tri[1] = _massList[_triangles[c].idx1]._pos;
		tri[2] = _massList[_triangles[c].idx2]._pos;
		actualTriangles.push_back(tri);
	}

	for (unsigned int a = 0; a < _padCalc._sorted_descriptors.size(); a++)
	{
		AVector pt = _padCalc._aShape[_padCalc._sorted_descriptors[a]._start_index ];
		_padPoints.push_back(pt); //////

		int triIdx = -1;
		ABary bary;
		for (unsigned int c = 0; c < actualTriangles.size(); c++)
		{
			if (UtilityFunctions::InsidePolygon(actualTriangles[c], pt.x, pt.y))
			{
				triIdx = c;
				break;
			}
		}

		if (triIdx == -1)
		{
			std::cout << "error !!!\n";
			triIdx = -1;
			float dist = 100000000;
			for (unsigned int c = 0; c < actualTriangles.size(); c++)
			{
				float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], pt);
				if (d < dist)
				{
					dist = d;
					triIdx = c;
				}
			}
		}

		bary = UtilityFunctions::Barycentric(pt,
			actualTriangles[triIdx][0],
			actualTriangles[triIdx][1],
			actualTriangles[triIdx][2]);

		_padBarys.push_back(bary);
		_padTriIdxs.push_back(triIdx);

	}*/


	//std::cout << "PAD element\n";
}

void AnElement::SetMatchedPoint(AVector e_pt, AVector c_pt)
{
	_isMatched = true;
	_matchedPoint = e_pt;
	_matchedContainerPt = c_pt;

	float d = INT_MAX;
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		float dist = _massList[a]._pos.DistanceSquared(_matchedPoint);
		if (dist < d)
		{
			d = dist;
			_matchedVertexIdx = a;
		}
	}

	std::vector<std::vector<AVector>> actualTriangles;
	for (unsigned int c = 0; c < _triangles.size(); c++)
	{
		std::vector<AVector> tri(3);
		tri[0] = _massList[_triangles[c].idx0]._pos;
		tri[1] = _massList[_triangles[c].idx1]._pos;
		tri[2] = _massList[_triangles[c].idx2]._pos;
		actualTriangles.push_back(tri);
	}

	_triMatchedIdx = -1;
	for (unsigned int c = 0; c < actualTriangles.size(); c++)
	{
		if (UtilityFunctions::InsidePolygon(actualTriangles[c], _matchedPoint.x, _matchedPoint.y))
		{
			_triMatchedIdx = c;
			break;
		}
	}

	if (_triMatchedIdx == -1)
	{
		//std::cout << "error !!!\n";

		_triMatchedIdx = -1;
		float dist = INT_MAX;
		for (unsigned int c = 0; c < actualTriangles.size(); c++)
		{
			float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _matchedPoint);
			if (d < dist)
			{
				dist = d;
				_triMatchedIdx = c;
			}
		}
	}

	_matchedBary = UtilityFunctions::Barycentric(_matchedPoint,
		actualTriangles[_triMatchedIdx][0],
		actualTriangles[_triMatchedIdx][1],
		actualTriangles[_triMatchedIdx][2]);

}

void AnElement::UpdateBoundaryAndAvgEdgeLength()
{
	_randommm_indices = std::vector<int>(_skinPointNum);
	for (unsigned int a = 0; a < _skinPointNum; a++) { _randommm_indices[a] = a; }

	if (_skin.size() == 0)
	{
		// this is needed for Inside/Outside calculation
		for (unsigned int a = 0; a < _skinPointNum; a++)
		{
			_skin.push_back(_massList[a]._pos);
		}
	}

	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		_skin[a].x = _massList[a]._pos.x;
		_skin[a].y = _massList[a]._pos.y;
	}

	// calculate average edge length
	_averageEdgeLength = 0;
	int trsz = _triEdges.size();
	for (unsigned int a = 0; a < trsz; a++)
	{
		_averageEdgeLength += _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
	}
	_averageEdgeLength /= (float)trsz;
}

AVector AnElement::FindClosestPtOnEdges(int idx, AVector pt)
{
	float minDist = std::numeric_limits<float>::max();
	AVector closestPt;

	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		if (_triEdges[a]._index0 == idx || _triEdges[a]._index1 == idx) { continue; }

		AVector pt1 = _massList[_triEdges[a]._index0]._pos;
		AVector pt2 = _massList[_triEdges[a]._index1]._pos;
		AVector cPt = UtilityFunctions::ClosestPtAtFiniteLine(pt1, pt2, pt);
		float d = cPt.Distance(pt);
		if (d < minDist)
		{
			minDist = d;
			closestPt = cPt;
		}
	}
	return closestPt;
}

void AnElement::SelfIntersectionFlagging()
{
	/*
	for (unsigned int a = 0; a < _boundaryPointNum; a++)
	{
		//int idx1 = randomIndices[a];
		int idx1 = a;
		_massList[idx1]._selfIntersect = false;

		if (_massList[idx1]._isFolded) { continue; }

		for (unsigned int b = 0; b < _massList.size(); b++)
			//for (unsigned int b = 0; b < _boundaryPointNum; b++)
		{
			int idx2 = b;

			if (idx1 == idx2) { continue; }

			if (_massList[idx1].IsNeighbor(idx2)) { continue; }

			if (_massList[idx2].IsInsideTriangle(_massList[idx1]._pos, _massList))
			{
				//std::cout << "@";
				_massList[idx1]._selfIntersect = true;
				break;
			}
		}
	}*/
}

void AnElement::SelfIntersectionRelax()
{
	//std::vector<int> randomIndices;
	//for (unsigned int a = 0; a < _massList.size(); a++) { randomIndices.push_back(a); }
	for (unsigned int a = 0; a < _skinPointNum; a++) { _randommm_indices[a] = a; }

	{
		int rand_num = rand();
		std::mt19937 g(rand_num);
		std::shuffle(_randommm_indices.begin(), _randommm_indices.end(), g);
	}

	float lThreshold = SystemParams::_self_intersection_threshold;
	float distMin = _averageEdgeLength * lThreshold;
	float distMinSq = distMin * distMin;

	//for (unsigned int a = 0; a < _massList.size() - 1; a++)
	for (unsigned int a = 0; a < _skinPointNum - 1; a++)
	{
		int idx1 = _randommm_indices[a];

		//int iterGap = 3;
		//for (unsigned int b = a + iterGap; b < _skinPointNum; b++)
		AVector pt1, pt2, v1, v2, dir;
		for (unsigned int b = a + 1; b < _skinPointNum; b++)
		{

			int idx2 = _randommm_indices[b];

			if (_massList[idx1].IsNeighbor(idx2)) { continue; }


			pt1 = _massList[idx1]._pos;
			pt2 = _massList[idx2]._pos;

			// ballpark
			//if (pt1.Distance(pt2) < distMin * 5.0f)
			if (pt1.DistanceSquared(pt2) < distMinSq * 25.0f)
			{
				// 1 to 2
				v1 = UtilityFunctions::GetClosestPtOnPolyline(_massList[idx2]._lineSgment, pt1);
				//float d1 = pt1.Distance(v1);
				float d1Sq = pt1.DistanceSquared(v1);
				//if (d1 < distMin)
				if (d1Sq < distMinSq)
				{
					dir = v1.DirectionTo(pt1).Norm();
					float offst = distMin - std::sqrt(d1Sq);
					// BUG ???
					if (offst < 10) { _massList[idx1]._pos += dir * offst; }
					else { std::cerr << "error\n"; }
				}

				// 2 to 1
				v2 = UtilityFunctions::GetClosestPtOnPolyline(_massList[idx1]._lineSgment, pt2);
				//float d2 = pt2.Distance(v2);
				float d2Sq = pt2.Distance(v2);
				//if (d2 < distMin)
				if (d2Sq < distMinSq)
				{
					dir = v2.DirectionTo(pt2).Norm();
					float offst = distMin - std::sqrt(d2Sq);
					// BUG ???
					if (offst < 10) { _massList[idx2]._pos += dir * offst; }
					else { std::cerr << "error\n"; }
				}
			}

		}
	}
}

void AnElement::CalculateBoundaryEdges()
{
	for (unsigned int a = 0; a < _skinPointNum - 1; a++)
	{
		float dist = _massList[a]._pos.Distance(_massList[a + 1]._pos);
		AnIndexedLine anEdge(a, a + 1, dist); // dist isn't important anyway ...
		_skinIdxEdges.push_back(anEdge);
	}

	// last and first
	{
		float dist = _massList[_skinPointNum - 1]._pos.Distance(_massList[0]._pos);
		AnIndexedLine anEdge(_skinPointNum - 1, 0, dist); // dist isn't important anyway ...
		_skinIdxEdges.push_back(anEdge);
	}
}

void AnElement::DrawAsSelected()
{
	if (_isGrowing) { glColor3f(0, 0, 1); }
	else { glColor3f(0, 0, 0); }
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (unsigned int b = 0; b < _skinIdxEdges.size(); b++)
	{
		AVector p1 = _massList[_skinIdxEdges[b]._index0]._pos;
		AVector p2 = _massList[_skinIdxEdges[b]._index1]._pos;
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	}
	glEnd();
}

void AnElement::Draw()
{
	/*for (unsigned int a = 0; a < _massList.size(); a++)
	{
		if (_massList[a]._foldingForce.Length() > 0)
		{
			_massList[a].Draw(_massList);
		}
	}*/

	/*glColor3f(0.0, 0.0, 1.0);
	glPointSize(3.0f);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _boundaryPointNum; a++)
	{
		AVector p1 = _massList[a]._pos;
		glVertex2f(p1.x, p1.y);
	}
	glEnd();*/

	/*glColor3f(0.0, 1.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		if (_massList[a]._selfIntersect)
		{
			AVector p1 = _massList[a]._pos;
			glVertex2f(p1.x, p1.y);
		}
	}
	glEnd();*/

	/*glColor3f(1.0, 0.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		if (_massList[a]._isFolded)
		{
			AVector p1 = _massList[a]._pos;
			glVertex2f(p1.x, p1.y);
		}
	}
	glEnd();*/

	//for (int a = 0; a < _graphs.size(); a++)
	//{
	// show inside
	/*for (int a = 0; a < _massList.size(); a++)
	{
		if (_massList[a]._isInside)
		{
			glColor3f(0, 0, 1);
			glPointSize(5.0);
			glBegin(GL_POINTS);
			glVertex2f(_massList[a]._pos.x, _massList[a]._pos.y);
			glEnd();
		}
	}*/


	// MST
	//if (SystemParams::_show_mst)
	//{
	// draw prev pos
	/*glColor3f(1.0, 0.0, 0.0);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	for (int a = 0; a < _massList.size(); a++)
	{
		AVector p1 = _massList[a]._prevPos;
		glVertex2f(p1.x, p1.y);
	}
	glEnd();*/

	/*glColor3f(0.5, 0.5, 1.0);

	for (unsigned int b = 0; b < _edges.size(); b++)
	{
		AVector p1 = _massList[_edges[b]._index0]._pos;
		AVector p2 = _massList[_edges[b]._index1]._pos;
		glLineWidth(2.0f);
		glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();
	}*/

	//glColor3f(0.0, 0.0, 1.0);
	//glPointSize(3.0f);
	//glBegin(GL_POINTS);
	//for (unsigned int a = 0; a < _massList.size(); a++)
	//{
	//	AVector p1 = _massList[a]._pos;
	//	glVertex2f(p1.x, p1.y);
	//}
	//glEnd();
	//}

	// draw force
	//for (unsigned int a = 0; a < _massList.size(); a++)
	//{
	//	_massList[a].DrawForce();
	//}

	if (SystemParams::_show_elements)
	{
		/*glColor3f(0.22745098, 0.635294118, 0.858823529);
		glLineWidth(1.0f);
		glBegin(GL_LINES);

		for (unsigned int a = 0; a < _arts.size(); a++)
		{
			for (unsigned int b = 0; b < _arts[a].size(); b++)
			{
				int c = b + 1;
				if (c >= _arts[a].size()) { c = 0; }
				glVertex2f(_arts[a][b].x, _arts[a][b].y);
				glVertex2f(_arts[a][c].x, _arts[a][c].y);
			}
		}
		glEnd();*/


		// background		
		glBegin(GL_TRIANGLES);
		float avgThres = _averageEdgeLength * 10.0f;
		for (int a = _artTris.size() - 1; a >= 0; a--)
		{
			MyColor bCol = _bColors[a];
			if (bCol.IsValid())
			{
				float r = bCol._r;	float g = bCol._g;	float b = bCol._b;
				r /= 255.0;	g /= 255.0;	b /= 255.0;
				glColor3f(r, g, b);

				for (unsigned int b = 0; b < _artTris[a].size(); b++)
				{
					AVector v1 = _tesselateArts[a][_artTris[a][b].idx0];
					AVector v2 = _tesselateArts[a][_artTris[a][b].idx1];
					AVector v3 = _tesselateArts[a][_artTris[a][b].idx2];

					if (v1.IsOut() || v2.IsOut() || v3.IsOut()) { continue; }
					if (v1.Distance(v2) > avgThres || v2.Distance(v3) > avgThres || v1.Distance(v3) > avgThres) { continue; }

					glVertex2f(v1.x, v1.y);
					glVertex2f(v2.x, v2.y);
					glVertex2f(v3.x, v3.y);
				}
			}
		}
		glEnd();

		// foreground
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < _arts.size(); a++)
		{
			MyColor fCol = _fColors[a];
			if (fCol.IsValid())
			{
				float r = fCol._r; float g = fCol._g; float b = fCol._b;
				r /= 255.0;	g /= 255.0;	b /= 255.0;
				glColor3f(r, g, b);

				for (unsigned int b = 0; b < _arts[a].size(); b++)
				{
					int c = b + 1;
					if (c >= _arts[a].size()) { c = 0; }
					glVertex2f(_arts[a][b].x, _arts[a][b].y);
					glVertex2f(_arts[a][c].x, _arts[a][c].y);
				}
			}
		}
		glEnd();
	}

	// draw closest pairs
	if (SystemParams::_show_closest_pairs)
	{
		// all pairs
		glColor3f(0.98, 0.75, 0.176);
		glLineWidth(0.01f);
		glBegin(GL_LINES);

		for (unsigned int a = 0; a < _massList.size(); a++)
		{
			for (int b = 0; b < _massList[a]._closestPt_fill_sz; b++)
			{
				glVertex2f(_massList[a]._pos.x, _massList[a]._pos.y);
				glVertex2f(_massList[a]._closestPoints[b].x, _massList[a]._closestPoints[b].y);
			}
		}
		glEnd();
	}

	if (SystemParams::_show_bending_springs)
	{
		glColor3f(0.956862745, 0.48627451, 0.125490196);
		glLineWidth(0.5f);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < _auxiliaryEdges.size(); a++)
		{
			AVector pt1 = _massList[_auxiliaryEdges[a]._index0]._pos;
			AVector pt2 = _massList[_auxiliaryEdges[a]._index1]._pos;
			glVertex2f(pt1.x, pt1.y);
			glVertex2f(pt2.x, pt2.y);
		}
		glEnd();
	}

	if (SystemParams::_show_triangles)
	{
		glColor3f(0.22745098, 0.635294118, 0.858823529);
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < _triangles.size(); a++)
		{
			AVector pt1 = _massList[_triangles[a].idx0]._pos;
			AVector pt2 = _massList[_triangles[a].idx1]._pos;
			AVector pt3 = _massList[_triangles[a].idx2]._pos;

			glVertex2f(pt1.x, pt1.y);
			glVertex2f(pt2.x, pt2.y);

			glVertex2f(pt2.x, pt2.y);
			glVertex2f(pt3.x, pt3.y);

			glVertex2f(pt3.x, pt3.y);
			glVertex2f(pt1.x, pt1.y);
		}
		glEnd();

		glColor3f(0.956862745, 0.48627451, 0.125490196);
		glLineWidth(0.5f);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < _negSpaceEdges.size(); a++)
		{
			AVector pt1 = _massList[_negSpaceEdges[a]._index0]._pos;
			AVector pt2 = _massList[_negSpaceEdges[a]._index1]._pos;

			glVertex2f(pt1.x, pt1.y);
			glVertex2f(pt2.x, pt2.y);
		}
		glEnd();
	}

	if (SystemParams::_show_uni_art)
	{
		// UNIART!!!!
		/*glColor3f(1, 0, 0);
		glLineWidth(0.5f);
		glBegin(GL_LINES);

		for (unsigned int a = 0; a < _uniArt.size(); a++)
		{
			int c = a + 1;
			if (c >= _uniArt.size()) { c = 0; }
			glVertex2f(_uniArt[a].x, _uniArt[a].y);
			glVertex2f(_uniArt[c].x, _uniArt[c].y);
		}
		glEnd();*/

		glLineWidth(1.0f);
		glColor3f(255, 0, 0);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < _uniuniArts.size(); a++)
		{
			//MyColor fCol = _fColors[a];
			//if (fCol.IsValid())
			{
				//float r = fCol._r; float g = fCol._g; float b = fCol._b;
				//r /= 255.0;	g /= 255.0;	b /= 255.0;
				//glColor3f(r, g, b);

				for (unsigned int b = 0; b < _uniuniArts[a].size(); b++)
				{
					int c = b + 1;
					if (c >= _uniuniArts[a].size()) { c = 0; }
					glVertex2f(_uniuniArts[a][b].x, _uniuniArts[a][b].y);
					glVertex2f(_uniuniArts[a][c].x, _uniuniArts[a][c].y);
				}
			}
		}
		glEnd();
	}

	if (SystemParams::_show_element_boundary)
	{
		// draw boundary
		glLineWidth(1.0f);

		if (_isGrowing) { glColor3f(0.949019608, 0.501960784, 0.666666667); }
		else { glColor3f(0, 0, 0); }

		if (_isMatched) { glColor3f(0, 0, 1); }

		glBegin(GL_LINES);
		for (unsigned b = 1; b < _skinPointNum; b++)
		{
			glVertex2f(_skin[b - 1].x, _skin[b - 1].y);
			glVertex2f(_skin[b].x, _skin[b].y);
		}
		glVertex2f(_skin[_skinPointNum - 1].x, _skin[_skinPointNum - 1].y);
		glVertex2f(_skin[0].x, _skin[0].y);
		glEnd();





		//for (unsigned int b = 0; b < _boundaryPointNum; b++)
		//{
		//	_massList[b].Draw(_massList);
		//}

		/*glColor3f(1.0, 0.0, 0.0);
		glPointSize(3.0f);
		glBegin(GL_POINTS);
		for (unsigned int a = 0; a < _massList.size(); a++)
		{
		if (!_massList[a]._isGrowing)
		{
		AVector p1 = _massList[a]._pos;
		glVertex2f(p1.x, p1.y);
		}
		}
		glEnd();*/


	}

	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		_massList[a].Draw();
	}

	if (SystemParams::_show_shape_matching)
	{
		/*glColor3f(0, 0.0, 0.5);
		glPointSize(1.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < _padPoints.size(); i++)
		{
			//int l_idx = _descriptors[_clickedIdx]._left_indices[a];
			//int _idx = _sorted_descriptors[i]._start_index;
			glVertex2f(_padPoints[i].x, _padPoints[i].y);
			//glVertex2f(_aShape[r_idx].x, _aShape[r_idx].y);
		}
		glEnd();*/


		if (_isMatched)
		{
			//std::cout << "#";
			_padCalc.Draw2();

			glColor3f(1.0, 0.0, 0.0);
			glPointSize(3.0f);
			glBegin(GL_POINTS);
			glVertex2f(_massList[_matchedVertexIdx]._pos.x, _massList[_matchedVertexIdx]._pos.y);
			glEnd();
		}
	}

	/*glColor3f(1.0, 0.0, 0.0);
	glPointSize(5.0f);
	glBegin(GL_POINTS);

	glVertex2f(_centroid.x, _centroid.y);

	glEnd();*/

	// gggg
	/*{ glColor3f(0, 0, 0); }
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		AVector p1 = _massList[a]._pos;
		AVector p2 = p1 + (_rotateArray[a] * 10.0f);

		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
	}
	glEnd();*/

}

void AnElement::Move(AVector vec)
{
	UpdateBoundaryAndAvgEdgeLength();
	ARectangle bb = UtilityFunctions::GetBoundingBox(_skin);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos -= bb.topleft;
	}

	AVector centerMass(0, 0);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		centerMass += _massList[a]._pos;
	}
	centerMass /= (float)_massList.size();

	//std::cout << "centermass x: " << centerMass.x << ", y: " << centerMass.y << "\n";

	AVector offsetVec = vec - centerMass;
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos += offsetVec;
	}

	// original
	//_oriMassPos.clear();
	//for (unsigned int a = 0; a < _massList.size(); a++) { _oriMassPos.push_back(_massList[a]._pos); }
}

void AnElement::Translate(AVector vec)
{
	for (int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos += vec;
	}
}

void AnElement::ReflectXAxis()
{
	// get center of mass
	AVector centerPos(0, 0);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		centerPos += _massList[a]._pos;
	}
	centerPos /= (float)_massList.size();

	for (int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos.x -= centerPos.x;
		_massList[a]._pos.x = -_massList[a]._pos.x;
		_massList[a]._pos.x += centerPos.x;
	}
}

void AnElement::Rotate(float radValue)
{
	// get center of mass
	AVector centerPos(0, 0);
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		centerPos += _massList[a]._pos;
	}
	centerPos /= (float)_massList.size();

	for (int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos -= centerPos;
		_massList[a]._pos = UtilityFunctions::Rotate(_massList[a]._pos, radValue);
		_massList[a]._pos += centerPos;
		//_massList[a]._prevPos = _massList[a]._pos; // delete
		//_massList[a]._velocity = AVector(0, 0);
	}
}

void AnElement::Scale(float scaleFactor)
{
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._pos *= scaleFactor;
	}
}

/*void AssignEdgesToMasses()
{
	// add edges
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
		AnIndexedLine anEdge = _edges[a];
		_massList[anEdge._index0].AddEdge(anEdge);
		_massList[anEdge._index1].AddEdge(anEdge);
	}

	// sort neigbours
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a].SortEdges(_massList);
	}
}*/

void AnElement::ConvertMassMapToList() // we don't use MST anymore
{
	// this one is automatically sorted
	/*std::map<int, AMass>::iterator it;
	for (it = _massMap.begin(); it != _massMap.end(); ++it)
	{
		_massList.push_back((*it).second);
	}*/
}

//void AGraph::CalculateNNEdges()
//{
//	for (unsigned int a = 0; a < _massList.size(); a++)
//	{
//		_massList[a].CalculateNNEdges(_massList, _skinPointNum);
//	}
//}

// we don't use MST anymore
/*bool ContainMass(int massIndex)
{
	return _massMap.find(massIndex) != _massMap.end();
}*/

// we don't use MST anymore
/*void Concat(AGraph anotherGraph)
{
	_massMap.insert(anotherGraph._massMap.begin(), anotherGraph._massMap.end());
	_edges.insert(_edges.end(), anotherGraph._edges.begin(), anotherGraph._edges.end());
}*/

bool AnElement::CanGrow()
{
	float minDist = std::numeric_limits<float>::max(); // very large
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		if (_massList[a]._isInside) { return false; }

		//if (!_massList[a]._closestOtherPt.IsInvalid())
		//{
		//float dist = _massList[a]._pos.Distance(_massList[a]._closestOtherPt);
		float dist = _massList[a]._closestDist;
		if (dist < SystemParams::_growth_min_dist) { return false; }
		//if (dist < minDist)
		//{
		//	minDist = dist;
		//}
		//}
	}

	/*if (minDist < SystemParams::_growth_min_dist)
	{
		return false;
	}*/
	return true;
}

//void AGraph::Grow2(float growth_scale_iter, std::vector<AGraph>& allGraphs, float dt)
//{
//	//if (_averageEdgeLength > _oriAvgEdgeLength * _maxEdgeLengthFactor) { _isGrowing = false; }
//	if (_currentArea > (_oriArea * SystemParams::_max_growth))
//	{
//		_isGrowing = false;
//	}
//	else
//	{
//		for (unsigned int a = 0; a < _skinPointNum && _isGrowing; a++)
//		{
//			if (_massList[a]._closestPoints.size() > 0)
//			{
//				if (_massList[a]._isInside)  { _isGrowing = false; break; }
//				if (_massList[a]._closestDist < SystemParams::_growth_min_dist) { _isGrowing = false; break; }
//			}
//		}
//	}
//
//	for (unsigned int a = 0; a < _massList.size(); a++)
//	{
//		_massList[a]._isGrowing = false;
//	}
//
//	if (_isGrowing)
//	{
//		this->_scale += growth_scale_iter * dt;
//		for (unsigned int a = 0; a < _massList.size(); a++)
//		{
//			if (_massList[a]._mass < this->_scale)
//			{
//				_massList[a].Grow(growth_scale_iter, dt);
//				_massList[a]._isGrowing = true;
//			}
//		}
//		for (unsigned int a = 0; a < _triEdges.size(); a++)
//		{
//			if (_triEdges[a].GetScale() < this->_scale)
//			{
//				_triEdges[a].MakeLonger(growth_scale_iter, dt);
//			}
//		}
//	}
//	else
//	{
//		for (unsigned int a = 0; a < _triEdges.size(); a++)
//		{
//			int idx1 = _triEdges[a]._index0;
//			int idx2 = _triEdges[a]._index1;
//
//			if (idx1 >= _skinPointNum && idx2 >= _skinPointNum) { continue; }
//
//			bool nope1 = false;
//			bool nope2 = false;
//
//			if (_massList[idx1]._closestPoints.size() > 0)
//			{
//				if (_massList[idx1]._isInside) { nope1 = true; }
//				if (_massList[idx1]._closestDist < SystemParams::_growth_min_dist) { nope1 = true; }
//			}
//			if (_massList[idx2]._closestPoints.size() > 0)
//			{
//				if (_massList[idx2]._isInside) { nope2 = true; }
//				if (_massList[idx2]._closestDist < SystemParams::_growth_min_dist) { nope2 = true; }
//			}
//
//			if (!nope1 && !nope2 && _triEdges[a].GetScale() < (this->_scale * SystemParams::_grow_grow_grow))
//			{
//				_massList[idx1]._isGrowing = true;
//				_massList[idx2]._isGrowing = true;
//				_triEdges[a].MakeLonger(growth_scale_iter, dt);
//			}
//		}
//
//		for (unsigned int a = 0; a < _massList.size(); a++)
//		{
//			if (_massList[a]._isGrowing) { _massList[a].Grow(growth_scale_iter, dt); }
//		}
//	}
//}

//void AGraph::InitShrinking()
//{
//	_transition_time = SystemParams::_shrink_transition_time;
//	_transition_time_counter = rand() % (int)_transition_time;
//}

void AnElement::Grow(float growth_scale_iter, std::vector<AnElement>& allGraphs, float dt)
{
	for (unsigned int a = 0; a < _skinPointNum && _isGrowing; a++)
	{
		if (_massList[a]._closestPoints.size() > 0)
		{
			if (_massList[a]._isInside) { _isGrowing = false; break; }

			if (_massList[a]._closestDist < SystemParams::_growth_min_dist)
			{
				_isGrowing = false;
				break;
			}
		}
	}

	if (!_bigOne)
	{
		growth_scale_iter *= 2;
	}


	if (!_isGrowing/* && _shrinking_state > 0*/) { return; }

	this->_scale += /*_shrinking_state */ growth_scale_iter * dt; // scaling factor


	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a].Grow(/* _shrinking_state * */growth_scale_iter, dt);
	}

	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		_triEdges[a].MakeLonger(/* _shrinking_state * */ growth_scale_iter, dt);
	}

	for (unsigned int a = 0; a < _auxiliaryEdges.size(); a++)
	{
		_auxiliaryEdges[a].MakeLonger(/* _shrinking_state * */ growth_scale_iter, dt);
	}
}

void AnElement::ComputeFoldingForces()
{
	std::vector<bool> massFlags;
	//for (unsigned int a = 0; a < _boundaryPointNum; a++)
	for (unsigned int a = 0; a < _massList.size(); a++)
	{
		_massList[a]._isFolded = false;
	}
	//{ massFlags.push_back(false); }

	for (unsigned int a = 0; a < _triangles.size(); a++)
	{
		AnIdxTriangle tri = _triangles[a];
		AVector pt1 = _massList[tri.idx0]._pos;
		AVector pt2 = _massList[tri.idx1]._pos;
		AVector pt3 = _massList[tri.idx2]._pos;

		// 1 - 23
		if (/*tri.idx0 < _boundaryPointNum &&*/ !UtilityFunctions::IsLeft(pt2, pt3, pt1))
		{
			_massList[tri.idx0]._isFolded = true;
			//massFlags[tri.idx0] = true;
		}

		// 2 - 31
		if (/*tri.idx1 < _boundaryPointNum &&*/ !UtilityFunctions::IsLeft(pt3, pt1, pt2))
		{
			_massList[tri.idx1]._isFolded = true;
			//massFlags[tri.idx1] = true;
		}

		// 3 - 12
		if (/*tri.idx2 < _boundaryPointNum &&*/ !UtilityFunctions::IsLeft(pt1, pt2, pt3))
		{
			_massList[tri.idx2]._isFolded = true;
			//massFlags[tri.idx2] = true;
		}
	}

	//for (unsigned int a = 0; a < _boundaryPointNum; a++)
	//for (unsigned int a = 0; a < _massList.size(); a++)
	//{
	//	//if (massFlags[a])
	//	if (_massList[a]._isFolded)
	//	{
	//		_massList[a].CalculateFoldingForce(_massList);
	//	}
	//}
}

/*void TriangleRelax()
{
	std::vector<int> randomIndices;
	for (unsigned int a = 0; a < _triangles.size(); a++) { randomIndices.push_back(a); }
	std::random_shuffle(randomIndices.begin(), randomIndices.end());

	for (unsigned int a = 0; a < _triangles.size(); a++)
	{
		AnIdxTriangle tri = _triangles[randomIndices[a]];
		AVector pt1 = _massList[tri.idx0]._pos;
		AVector pt2 = _massList[tri.idx1]._pos;
		AVector pt3 = _massList[tri.idx2]._pos;

		// 1 - 23
		if (!UtilityFunctions::IsLeft(pt2, pt3, pt1) &&
		!_massList[tri.idx0]._isInside)// &&
		//!_massList[tri.idx0]._idx < _boundaryPointNum)
		{
			//std::cout << "1\n";
			_massList[tri.idx0]._pos = FlipVertex(pt2, pt3, pt1);
			_massList[tri.idx0]._velocity = AVector(0, 0);
			//_massList[tri.idx0]._prevPos = _massList[tri.idx0]._pos; // delete

		}

		// 2 - 31
		if (!UtilityFunctions::IsLeft(pt3, pt1, pt2) &&
		!_massList[tri.idx1]._isInside)// &&
		//!_massList[tri.idx1]._idx < _boundaryPointNum)
		{
			//std::cout << "2\n";
			_massList[tri.idx1]._pos = FlipVertex(pt3, pt1, pt2);
			_massList[tri.idx1]._velocity = AVector(0, 0);
			//_massList[tri.idx1]._prevPos = _massList[tri.idx1]._pos; // delete

		}

		// 3 - 12
		if (!UtilityFunctions::IsLeft(pt1, pt2, pt3) &&
		!_massList[tri.idx2]._isInside)// &&
		//!_massList[tri.idx2]._idx < _boundaryPointNum)
		{
			//std::cout << "3\n";
			_massList[tri.idx2]._pos = FlipVertex(pt1, pt2, pt3);
			_massList[tri.idx2]._velocity = AVector(0, 0);
			//_massList[tri.idx2]._prevPos = _massList[tri.idx2]._pos; // delete

		}
	}
}*/

AVector AnElement::FlipVertex(AVector l1, AVector l2, AVector pt)
{
	AVector dir = l1.DirectionTo(l2).Norm();
	AVector leftDir(dir.y, -dir.x);
	AVector closestPt = UtilityFunctions::ProjectToALine(l1, l2, pt);
	//AVector closestPt = UtilityFunctions::ClosestPtAtFiniteLine(l1, l2, pt); // should be infinite line
	//AVector closestPt = l1 + dir * 0.5 * l1.Distance(l2);
	return closestPt + leftDir * 0.5f;
	//return closestPt;
}

void AnElement::MSTEdgeRelax()
{
	/*std::vector<int> randomIndices;
	for (unsigned int a = 0; a < _edges.size(); a++) { randomIndices.push_back(a); }
	std::random_shuffle(randomIndices.begin(), randomIndices.end());

	// recalculate edges
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
	int idx = randomIndices[a];

	AVector pt1 = _massList[_edges[idx]._index0]._pos;
	AVector pt2 = _massList[_edges[idx]._index1]._pos;
	float dist = pt1.Distance(pt2);
	float distDiffHalf = (dist - _edges[idx]._dist) * 0.5f;

	AVector dir = pt1.DirectionTo(pt2).Norm();

	bool isInside1 = _massList[_edges[idx]._index0]._isInside;
	bool isInside2 = _massList[_edges[idx]._index1]._isInside;

	if (isInside1 && !isInside2)
	{
	_massList[_edges[idx]._index1]._pos -= dir * distDiffHalf * 2.0f;
	_massList[_edges[idx]._index1]._prevPos = _massList[_edges[idx]._index1]._pos;
	}
	else if (!isInside1 && isInside2)
	{
	_massList[_edges[idx]._index0]._pos += dir * distDiffHalf * 2.0f;
	_massList[_edges[idx]._index0]._prevPos = _massList[_edges[idx]._index0]._pos;
	}
	else
	{
	_massList[_edges[idx]._index0]._pos += dir * distDiffHalf;
	_massList[_edges[idx]._index1]._pos -= dir * distDiffHalf;

	_massList[_edges[idx]._index0]._prevPos = _massList[_edges[idx]._index0]._pos;
	_massList[_edges[idx]._index1]._prevPos = _massList[_edges[idx]._index1]._pos;
	}


	}

	// sync edges
	//for (int a = 0; a < _massList.size(); a++)
	//{
	//	_massList[a].VerletSyncEdgeLengths(_massList);
	//}

	// recalculate angle connectors
	for (int a = 0; a < _massList.size(); a++)
	{ _massList[a].VerletRelax(_massList); }*/
}

// NN EDGES
void AnElement::RecalculateEdgeLengths()
{
	// mass list
	//for (unsigned int a = 0; a < _massList.size(); a++)
	//{
	//	_massList[a].RecalculateEdgeLengths(_massList);
	//}

	// edges
	/*for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
	float dist = _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
	_triEdges[a].SetDist(dist);
	}*/

	// boundary edges needs no update
}

// need to call RecalculateArts
bool AnElement::InsideArts(AVector pt) const
{
	// true if inside
	return UtilityFunctions::InsidePolygons(_uniuniArts, pt.x, pt.y);
}

// need to call RecalculateArts
float AnElement::DistToArts(AVector pt) const
{
	return UtilityFunctions::DistanceToClosedCurves(_uniuniArts, pt);
}

void AnElement::RecalculateUniUniArts()
{
	AnIdxTriangle tri(0, 0, 0);
	ABary bary(0, 0, 0);

	int uniuni_art_sz = _uniuniArts.size();
	for (unsigned int a = 0; a < uniuni_art_sz; a++)
	{
		int art_sz_2 = _uniuniArts[a].size();
		for (unsigned int b = 0; b < art_sz_2; b++)
		{
			//int idx = _arts2Triangles[a][b];
			tri = _triangles[_uniuniArts2Triangles[a][b]];
			//AVector pt1 = _massList[tri.idx0]._pos;
			//AVector pt2 = _massList[tri.idx1]._pos;
			//AVector pt3 = _massList[tri.idx2]._pos;
			bary = _uniuniBaryCoords[a][b];
			_uniuniArts[a][b] = _massList[tri.idx0]._pos * bary._u +
				_massList[tri.idx1]._pos * bary._v +
				_massList[tri.idx2]._pos * bary._w;
		}
	}
}

void AnElement::RecalculateArts()
{
	AnIdxTriangle tri(0, 0, 0);
	ABary bary(0, 0, 0);

	// centroid
	tri = _triangles[_centroidTriIdx];
	_centroid = _massList[tri.idx0]._pos * _centroidBary._u +
		_massList[tri.idx1]._pos * _centroidBary._v +
		_massList[tri.idx2]._pos * _centroidBary._w;


	int art_sz = _arts.size();
	for (unsigned int a = 0; a < art_sz; a++)
	{
		int art_sz_2 = _arts[a].size();
		for (unsigned int b = 0; b < art_sz_2; b++)
		{
			//int idx = _arts2Triangles[a][b];
			tri = _triangles[_arts2Triangles[a][b]];
			//AVector pt1 = _massList[tri.idx0]._pos;
			//AVector pt2 = _massList[tri.idx1]._pos;
			//AVector pt3 = _massList[tri.idx2]._pos;
			bary = _baryCoords[a][b];
			_arts[a][b] = _massList[tri.idx0]._pos * bary._u +
				_massList[tri.idx1]._pos * bary._v +
				_massList[tri.idx2]._pos * bary._w;
		}
	}

	int uniuni_art_sz = _uniuniArts.size();
	for (unsigned int a = 0; a < uniuni_art_sz; a++)
	{
		int art_sz_2 = _uniuniArts[a].size();
		for (unsigned int b = 0; b < art_sz_2; b++)
		{
			//int idx = _arts2Triangles[a][b];
			tri = _triangles[_uniuniArts2Triangles[a][b]];
			//AVector pt1 = _massList[tri.idx0]._pos;
			//AVector pt2 = _massList[tri.idx1]._pos;
			//AVector pt3 = _massList[tri.idx2]._pos;
			bary = _uniuniBaryCoords[a][b];
			_uniuniArts[a][b] = _massList[tri.idx0]._pos * bary._u +
				_massList[tri.idx1]._pos * bary._v +
				_massList[tri.idx2]._pos * bary._w;
		}
	}

	int tess_sz = _tesselateArts.size();
	for (unsigned int a = 0; a < tess_sz; a++)
	{
		int tess_sz_2 = _tesselateArts[a].size();
		for (unsigned int b = 0; b < tess_sz_2; b++)
		{
			//int idx = _arts2Triangles[a][b];
			tri = _triangles[_tesselateArts2Triangles[a][b]];
			//AVector pt1 = _massList[tri.idx0]._pos;
			//AVector pt2 = _massList[tri.idx1]._pos;
			//AVector pt3 = _massList[tri.idx2]._pos;
			bary = _tesselateBaryCoords[a][b];
			_tesselateArts[a][b] = _massList[tri.idx0]._pos * bary._u +
				_massList[tri.idx1]._pos * bary._v +
				_massList[tri.idx2]._pos * bary._w;
		}
	}

	int uni_sz = _uniArt.size();
	for (unsigned int a = 0; a < uni_sz; a++)
	{
		tri = _triangles[_uniArt2Triangles[a]];
		bary = _uniBaryCoords[a];
		_uniArt[a] = _massList[tri.idx0]._pos * bary._u +
			_massList[tri.idx1]._pos * bary._v +
			_massList[tri.idx2]._pos * bary._w;
	}

	/*for (unsigned int a = 0; a < _padPoints.size(); a++)
	{
		//int idx = _uniArt2Triangles[a];
		tri = _triangles[_padTriIdxs[a]];
		//AVector pt1 = _massList[tri.idx0]._pos;
		//AVector pt2 = _massList[tri.idx1]._pos;
		//AVector pt3 = _massList[tri.idx2]._pos;
		bary = _padBarys[a];
		_padPoints[a] = _massList[tri.idx0]._pos * bary._u +
			_massList[tri.idx1]._pos * bary._v +
			_massList[tri.idx2]._pos * bary._w;
	}*/

	if (_isMatched)
	{
		tri = _triangles[_triMatchedIdx];
		bary = _matchedBary;
		_matchedPoint = _massList[tri.idx0]._pos * bary._u +
			_massList[tri.idx1]._pos * bary._v +
			_massList[tri.idx2]._pos * bary._w;


		int aShape_sz = _padCalc._aShape.size();
		for (unsigned int a = 0; a < aShape_sz; a++)
		{
			tri = _triangles[_padCalc._aShapeTriIdxs[a]];
			bary = _padCalc._aShapeBarys[a];
			_padCalc._aShape[a] = _massList[tri.idx0]._pos * bary._u +
				_massList[tri.idx1]._pos * bary._v +
				_massList[tri.idx2]._pos * bary._w;
		}
	}
}

// triangle edges
void AnElement::RecalculateTriangleEdgeLengths()
{
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		//_triEdges[a]._dist = _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
		float dist = _massList[_triEdges[a]._index0]._pos.Distance(_massList[_triEdges[a]._index1]._pos);
		_triEdges[a].SetDist(dist);
	}

	for (unsigned int a = 0; a < _auxiliaryEdges.size(); a++)
	{
		float dist = _massList[_auxiliaryEdges[a]._index0]._pos.Distance(_massList[_auxiliaryEdges[a]._index1]._pos);
		_auxiliaryEdges[a].SetDist(dist);
	}

	// not used
	// auxiliary edges
	/*for (unsigned int a = 0; a < _boundaryPointNum; a++)
	{
		// reinforce
		if (_massList[a]._triEdges.size() == 2)
		{
			AVector pt0 = _massList[_massList[a]._triEdges[0]._index1]._pos;
			AVector pt1 = _massList[_massList[a]._triEdges[1]._index1]._pos;
			AVector midPt = (pt0 + pt1) / 2.0f;

			AVector curPt = _massList[a]._pos;
			float dist = curPt.Distance(midPt);
			_massList[a]._distToMidPoint = dist;  // current dist (maybe not needed)
			_massList[a]._oriDistToMidPoint = dist;  // ori dist
		}
	}*/
}

int AnElement::GetUnsharedVertexIndex(AnIdxTriangle tri, AnIndexedLine edge)
{
	if (tri.idx0 != edge._index0 && tri.idx0 != edge._index1) { return tri.idx0; }

	if (tri.idx1 != edge._index0 && tri.idx1 != edge._index1) { return tri.idx1; }

	if (tri.idx2 != edge._index0 && tri.idx2 != edge._index1) { return tri.idx2; }

	return -1;
}

//
void AnElement::CreateBendingSprings()
{
	for (unsigned a = 0; a < _edgeToTri.size(); a++)
	{
		if (_edgeToTri[a].size() != 2) { continue; }

		int idx1 = GetUnsharedVertexIndex(_triangles[_edgeToTri[a][0]], _triEdges[a]);
		if (idx1 < 0) { continue; }

		int idx2 = GetUnsharedVertexIndex(_triangles[_edgeToTri[a][1]], _triEdges[a]);
		if (idx2 < 0) { continue; }

		AnIndexedLine anEdge(idx1, idx2);
		AVector pt1 = _massList[idx1]._pos;
		AVector pt2 = _massList[idx2]._pos;
		//anEdge._dist = pt1.Distance(pt2);
		float d = pt1.Distance(pt2);
		anEdge.SetDist(d);

		// push to edge list
		_auxiliaryEdges.push_back(anEdge);
	}
}

// triangle edges
void AnElement::CalculateTriangleEdges()
{
	// triangle edge springs
	for (unsigned int a = 0; a < _triangles.size(); a++)
	{
		int idx0 = _triangles[a].idx0;
		int idx1 = _triangles[a].idx1;
		int idx2 = _triangles[a].idx2;

		// 0 - 1
		TryToAddTriangleEdge(AnIndexedLine(idx0, idx1), a);

		// 1 - 2
		TryToAddTriangleEdge(AnIndexedLine(idx1, idx2), a);

		// 2 - 0
		TryToAddTriangleEdge(AnIndexedLine(idx2, idx0), a);
	}

	// create bending springs
	CreateBendingSprings();

	// debug mapping
	/*for (unsigned a = 0; a < _edgeToTri.size(); a++)
	{
		for (unsigned b = 0; b < _edgeToTri[a].size(); b++)
		{ std::cout << _edgeToTri[a][b] << " "; }
		std::cout << "\n";
	}
	std::cout << "\n";*/

	// assign triangles to masses
	for (unsigned int a = 0; a < _triangles.size(); a++)
	{
		int idx0 = _triangles[a].idx0;
		int idx1 = _triangles[a].idx1;
		int idx2 = _triangles[a].idx2;

		_massList[idx0]._triangles.push_back(_triangles[a]);
		_massList[idx1]._triangles.push_back(_triangles[a]);
		_massList[idx2]._triangles.push_back(_triangles[a]);

		/*AVector ctrPt = ( _massList[_triangles[a].idx0]._pos +
						  _massList[_triangles[a].idx1]._pos +
						  _massList[_triangles[a].idx2]._pos) / 3.0f;
		_massList[idx0]._triCenters.push_back(ctrPt);
		_massList[idx1]._triCenters.push_back(ctrPt);
		_massList[idx2]._triCenters.push_back(ctrPt);*/
	}

	// add to masses
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		int idx0 = _triEdges[a]._index0;
		int idx1 = _triEdges[a]._index1;

		_massList[idx0].TryToAddTriangleEdge(_triEdges[a], _massList);
		_massList[idx1].TryToAddTriangleEdge(_triEdges[a], _massList);
	}

	// NOT USED
	// GO TO RecalculateTriangleEdgeLengths()
	// auxiliary edges
	/*for (unsigned int a = 0; a < _boundaryPointNum; a++)
	{
		// reinforce
		if (_massList[a]._triEdges.size() == 2)
		{
			AVector pt0   = _massList[_massList[a]._triEdges[0]._index1]._pos;
			AVector pt1   = _massList[_massList[a]._triEdges[1]._index1]._pos;
			AVector midPt = (pt0 + pt1) / 2.0f;

			AVector curPt = _massList[a]._pos;
			float dist    = curPt.Distance(midPt);
			_massList[a]._distToMidPoint    = dist;  // current dist (maybe not needed)
			_massList[a]._oriDistToMidPoint = dist;  // ori dist
		}
	}*/
}

bool AnElement::TryToAddTriangleEdge(AnIndexedLine anEdge, int triIndex)
{
	int edgeIndex = FindTriangleEdge(anEdge);
	//if (!FindTriangleEdge(anEdge)/* && !FindEdge(anEdge)*/)
	if (edgeIndex < 0)
	{
		AVector pt1 = _massList[anEdge._index0]._pos;
		AVector pt2 = _massList[anEdge._index1]._pos;
		//anEdge._dist = pt1.Distance(pt2);
		float d = pt1.Distance(pt2);
		anEdge.SetDist(d);

		// push to edge list
		_triEdges.push_back(anEdge);

		// push to edge-to-triangle list
		std::vector<int> indices;
		indices.push_back(triIndex);
		_edgeToTri.push_back(indices);

		return true;
	}

	// push to edge-to-triangle list
	_edgeToTri[edgeIndex].push_back(triIndex);

	return false;
}


// triangle edges
int AnElement::FindTriangleEdge(AnIndexedLine anEdge)
{
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		if (_triEdges[a]._index0 == anEdge._index0 &&
			_triEdges[a]._index1 == anEdge._index1)
		{
			return a;
		}

		if (_triEdges[a]._index1 == anEdge._index0 &&
			_triEdges[a]._index0 == anEdge._index1)
		{
			return a;
		}

	}

	return -1;
}

// we're not using MST anymore
/*bool FindEdge(AnIndexedLine anEdge)
{
	for (unsigned int a = 0; a < _edges.size(); a++)
	{
		if (_edges[a]._index0 == anEdge._index0 &&
		_edges[a]._index1 == anEdge._index1) {
		return true;
		}
		if (_edges[a]._index1 == anEdge._index0 &&
		_edges[a]._index0 == anEdge._index1) {
			return true;
		}
	}
	return false;
}*/

float clip(int n, int lower, int upper)
{
	return std::max(lower, std::min(n, upper));
}

void  AnElement::SolveForNoise(/*std::vector<std::vector<AVector>> perlinMap*/)
{
	//if (_transition_time > 0.001)
	//{
	//	//std::cout << "#";
	//	float k_noise = SystemParams::_k_noise;
	//	for (unsigned int a = 0; a < _massList.size(); a++)
	//	{
	//		AVector pos = _massList[a]._pos;
	//		int x = clip(pos.x, 0, 500);
	//		int y = clip(pos.y, 0, 500);
	//		_massList[a]._noiseForce += StuffWorker::_perlinMap[x][y] * k_noise;
	//	}
	//}


}

void AnElement::SolveForNegativeSPaceSprings()
{
	float k_edge = SystemParams::_k_neg_space_edge;

	AVector pt0;
	AVector pt1;
	AVector dir;
	AVector eForce;
	for (unsigned int a = 0; a < _negSpaceEdges.size(); a++)
	{
		int idx0 = _negSpaceEdges[a]._index0;
		int idx1 = _negSpaceEdges[a]._index1;

		pt0 = _massList[idx0]._pos;
		pt1 = _massList[idx1]._pos;

		float dist = pt0.Distance(pt1);
		//float threshold = _negSpaceEdges[a].GetDist() * SystemParams::_self_intersection_threshold;
		float threshold = _averageEdgeLength * SystemParams::_self_intersection_threshold;
		if (dist < threshold)
		{
			// assume we need to make the dist longer
			dir = pt1.DirectionTo(pt0).Norm();
			float diff = threshold - dist;
			eForce = (dir * k_edge * diff);

			if (!eForce.IsBad())
			{
				_massList[idx0]._selfIntersectForce += eForce;	// _massList[idx0]._distToBoundary;
				_massList[idx1]._selfIntersectForce -= eForce;	// _massList[idx1]._distToBoundary;
			}
		}

	}
}

// triangle edges
void AnElement::SolveForTriangleSprings()
{
	// param
	float k_edge = SystemParams::_k_edge/* * SystemParams::_k_edge_dynamic*/;

	if (!_bigOne)
	{
		k_edge *= SystemParams::_k_edge_small_factor;
	}

	//if (this->_isRigid)
	//{
	//	k_edge = 1000;
	//}

	AVector pt0;
	AVector pt1;
	AVector dir;
	AVector eForce;

	// triangle edge springs
	for (unsigned int a = 0; a < _triEdges.size(); a++)
	{
		int idx0 = _triEdges[a]._index0;
		int idx1 = _triEdges[a]._index1;

		pt0 = _massList[idx0]._pos;
		pt1 = _massList[idx1]._pos;

		float dist = pt0.Distance(pt1);

		// assume we need to make the dist shorter
		dir = pt0.DirectionTo(pt1).Norm();
		float   oriDist = _triEdges[a].GetDist();
		float signVal = 1;
		float diff = dist - oriDist;

		if (diff < 0) { signVal = -1; }

		eForce = (dir * k_edge * signVal * diff * diff);
		//eForce = (dir * k_edge *  diff);
		// ORI
		//AVector eForce = (dir * k_edge *  (dist - oriDist));

		//std::cout << (dist - oriDist) << " ";

		if (!eForce.IsBad())
		{
			_massList[idx0]._edgeForce += eForce;	// _massList[idx0]._distToBoundary;
			_massList[idx1]._edgeForce -= eForce;	// _massList[idx1]._distToBoundary;
		}
	}


	// auxiliary edge springs
	for (unsigned int a = 0; a < _auxiliaryEdges.size(); a++)
	{
		int idx0 = _auxiliaryEdges[a]._index0;
		int idx1 = _auxiliaryEdges[a]._index1;

		pt0 = _massList[idx0]._pos;
		pt1 = _massList[idx1]._pos;

		float dist = pt0.Distance(pt1);

		float signVal = 1;
		dir = pt0.DirectionTo(pt1).Norm();
		float   oriDist = _auxiliaryEdges[a].GetDist();
		float diff = dist - oriDist;
		if (diff < 0) { signVal = -1; }
		eForce = (dir * k_edge * signVal * diff * diff);
		// ORI
		/*// assume we need to make the dist shorter
		AVector eForce = (dir * k_edge *  (dist - oriDist));*/

		//std::cout << (dist - oriDist) << " ";

		if (!eForce.IsBad())
		{
			_massList[idx0]._edgeForce += eForce;	// _massList[idx0]._distToBoundary;
			_massList[idx1]._edgeForce -= eForce;	// _massList[idx1]._distToBoundary;
		}
	}

	// dock
	if (_isMatched)
	{

		AVector dir = _massList[_matchedVertexIdx]._pos.DirectionTo(_matchedContainerPt);
		float dist = dir.Length();
		dir = dir.Norm();
		AVector eForce = (dir * SystemParams::_k_dock * dist);
		if (!eForce.IsBad())
		{
			_massList[_matchedVertexIdx]._edgeForce += eForce;	// _massList[idx0]._distToBoundary;
		}

	}

	// rotation
	float eps_rot = 3.14 * 0.001;

	float xPosNorm = PI * -_centroid.x / SystemParams::_upscaleFactor;
	float angleValAvg = 0;
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		//AVector targetVector = UtilityFunctions::Rotate(_normFromCentroidArray[a], AVector(0, 0), xPosNorm);
		AVector targetVector(0, -1);
		AVector curNorm = (_massList[a]._pos - _centroid).Norm();
		float angleVal = UtilityFunctions::Angle2D(curNorm.x, curNorm.y, targetVector.x, targetVector.y);
		angleValAvg += angleVal;
	}
	angleValAvg /= (float)_skinPointNum;
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{

		if (std::abs(angleValAvg) > eps_rot)
		{
			AVector curNorm = (_massList[a]._pos - _centroid).Norm();

			if (angleValAvg > 0)
			{
				// anticlockwise
				AVector dRIght(-curNorm.y, curNorm.x); // this is left
				_rotateArray[a] = dRIght;
			}
			else
			{
				AVector dLeft(curNorm.y, -curNorm.x);  // this is right
				_rotateArray[a] = dLeft;
			}
		}
		else
		{
			_rotateArray[a] = AVector(0, 0);
			//angleValAvg = 0;
		}

		AVector rForce = _rotateArray[a] * SystemParams::_k_rotate;
		if (!rForce.IsBad())
		{
			_massList[a]._rotationForce += rForce;	// _massList[idx0]._distToBoundary;
		}
	}
	/*float eps_rot = 3.14 * 0.01;
	AVector curNorm;
	AVector rForce;
	for (unsigned int a = 0; a < _skinPointNum; a++)
	{
		curNorm = (_massList[a]._pos - _centroid).Norm();
		float angleVal = UtilityFunctions::Angle2D(curNorm.x, curNorm.y, _normFromCentroidArray[a].x, _normFromCentroidArray[a].y);

		if (std::abs( angleVal ) > eps_rot)
		{
			if (angleVal > 0)
			{
				// anticlockwise

				AVector dRIght(-curNorm.y, curNorm.x); // this is left
				_rotateArray[a] = dRIght;
			}
			else
			{
				AVector dLeft(curNorm.y, -curNorm.x);  // this is right
				_rotateArray[a] = dLeft;
			}
		}
		else
		{
			_rotateArray[a] = AVector(0, 0);
			angleVal = 0;
		}

		rForce =  _rotateArray[a] * std::abs(angleVal) * SystemParams::_k_rotate;
		if (!rForce.IsBad())
		{
			_massList[a]._rotationForce += rForce;	// _massList[idx0]._distToBoundary;
		}
	}*/
}
