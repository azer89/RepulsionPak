
#include "ContainerWorker.h"

#include "SystemParams.h"
#include "AVector.h"
#include "UtilityFunctions.h"
#include "PoissonGenerator.h"
#include "PathIO.h"
#include "CurveRDP.h"
#include "CSSSmoothing.h"



#include "glew.h"
#include "freeglut.h"

#include <vector>


ContainerWorker::ContainerWorker()
{
	_mat_index = 0;

	_debug_counter = 0;
	_debugImg.CreateColorImage(SystemParams::_upscaleFactor * 2.0f);

}

ContainerWorker::~ContainerWorker()
{
	if (_mat_index) delete _mat_index;
}

void ContainerWorker::LoadContainer(CollissionGrid* cGrid)
{
	// ---------- load regions ----------
	PathIO pathIO;
	//_container_vf_region = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_artName + ".path");
	_container_vf_region = pathIO.LoadRegions(SystemParams::_container_file);

	std::cout << "region size: " << _container_vf_region.size() << "\n";

	for (unsigned int a = 0; a < _container_vf_region.size(); a++)
	{
		// ===== MULTIPLE CONTAINERS =====
		_container_vf_region[a].MakeBoundariesClockwise();
		std::vector<std::vector<AVector>> bs = _container_vf_region[a].GetBoundaries();
		_container_boundaries.insert(_container_boundaries.end(), bs.begin(), bs.end());

		/*for (unsigned int b = 0; b < bs.size(); b++)
		{
			if (!UtilityFunctions::IsClockwise(bs[a]))
			{
				std::reverse(bs[b].begin(), bs[b].end());
				std::cout << "boundary is counter clockwise!\n";
			}
			_container_boundaries.push_back(bs[b]);
		}*/
	}

	_container_length = UtilityFunctions::CurveLengthClosed(_container_boundaries[0]); // 1
	std::cout << "_container_length=" << _container_length << "\n";

	OpenCVWrapper cvWRap;
	std::cout << "____container area = " << cvWRap.GetAreaOriented(_container_boundaries[0]) << "\n";
	
	
	// ---------- resampled ----------
	//SystemParams::_resample_gap_float = _container_length / (float)SystemParams::_resample_num;
	//UtilityFunctions::UniformResampleClosed(_container_boundaries[0], _resampled_container, SystemParams::_resample_gap_float); // 1
	//std::cout << "container resampled gap: " << SystemParams::_resample_gap_float << "\n";
	UtilityFunctions::UniformResampleClosed(_container_boundaries[0], _resampled_container, SystemParams::_resample_gap_float); // 1

	// ---------- gaussian smoothing ----------
	CSSSmoothing css;
	css.Init1(SystemParams::_gaussian_smoothing);
	_smooth_container = _resampled_container;
	css.SmoothCurve1(_smooth_container);

	// ---------- RDP (not used) ----------
	_rdpFlags = std::vector<bool>(_resampled_container.size());
	for (unsigned int a = 0; a < _rdpFlags.size(); a++) { _rdpFlags[a] = true; }
	//CurveRDP::SimplifyRDPFlags(_resampled_container, _rdpFlags, SystemParams::_rdp_epsilon);
	//CurveRDP::SimplifyRDP(_resampled_container, _rdp_container, SystemParams::_rdp_epsilon);

	// ---------- PAD ----------
	_padCalc.ComputePAD(_smooth_container, _rdpFlags);
		
	// ---------- holes ----------
	for (unsigned int a = 0; a < _container_vf_region.size(); a++)
	{
		std::vector<std::vector<AVector>> hs = _container_vf_region[a]._holes;
		_holes.insert(_holes.end(), hs.begin(), hs.end());
	}

	std::cout << "number of boundaries: " << _container_boundaries.size() << "\n";
	std::cout << "number of holes: "      << _holes.size() << "\n";

	for (unsigned int a = 0; a < _container_vf_region.size(); a++)
	{
		if (_container_vf_region[a]._focalPaths.size() > 0)
		{
			// focals
			std::vector<std::vector<AVector>> focal = _container_vf_region[a].GetFocalBoundaries();
			_focals.push_back(focal);

			// offset
			std::vector<AVector> myOffsetBoundary = ClipperWrapper::RoundOffsettingPP(focal, SystemParams::_focal_offset)[0];
			_offsetFocalBoundaries.push_back(myOffsetBoundary);
		}
	}

	std::cout << "_focals size: " << _focals.size() << "\n";
	std::cout << "_offsetFocalBoundaries size: " << _offsetFocalBoundaries.size() << "\n";

	// uncomment me
	int numPoints = SystemParams::_num_element_pos;
	PoissonGenerator::DefaultPRNG PRNG;
	//if (SystemParams::_seed > 0)
	//{
	PRNG = PoissonGenerator::DefaultPRNG(SystemParams::_seed);
	//}
	const auto points = PoissonGenerator::GeneratePoissonPoints(numPoints, PRNG);

	// ---------- iterate points ----------
	float offst = std::sqrt(2.0f) * SystemParams::_upscaleFactor;
	float shiftOffst = 0.5f * (offst - SystemParams::_upscaleFactor);
	for (auto i = points.begin(); i != points.end(); i++)
	{
		AVector pt((i->x * offst) - shiftOffst, (i->y * offst) - shiftOffst);

		// ===== MULTIPLE CONTAINERS =====
		//bool isInside = UtilityFunctions::InsidePolygon(_boundaries[0], pt); // 1 UNCOMMENT THIS FOR HOLES
		bool isInside = UtilityFunctions::InsidePolygons(_container_boundaries, pt.x, pt.y);     // 2 UNCOMMENT THIS FOR MULTIPLE BOUNDARIES

		// ===== MULTIPLE CONTAINERS =====
		// 1  UNCOMMENT THESE FOR HOLES
		for (int bb = 0; bb < _holes.size() && isInside; bb++)
			{ isInside = !UtilityFunctions::InsidePolygon(_holes[bb], pt.x, pt.y); }

		//for (int bb = 0; bb < _offsetFocalBoundaries.size() && isInside; bb++)
		//{ isInside = !UtilityFunctions::InsidePolygon(_offsetFocalBoundaries[bb], pt); }

		float d = UtilityFunctions::DistanceToBunchOfPoints(_randomPositions, pt);

		if (isInside && d > 30) 
			{ _randomPositions.push_back(pt); }
	}

	if (!SystemParams::_do_shape_matching && SystemParams::_num_element_pos_limit < _randomPositions.size())
	{
		while (_randomPositions.size() != SystemParams::_num_element_pos_limit)
		{
			std::mt19937 g(SystemParams::_seed);
			std::shuffle(_randomPositions.begin(), _randomPositions.end(), g);
			_randomPositions.erase(_randomPositions.begin());
		}
	}

	cGrid->AnalyzeContainer(_container_boundaries, _holes, _offsetFocalBoundaries);
}

bool ContainerWorker::IsGraphInside(const AGraph& g)
{
	for (unsigned int a = 0; a < g._padCalc._sorted_descriptors.size(); a++)
	{
		AVector pt = g._padCalc._aShape[g._padCalc._sorted_descriptors[a]._start_index];
		if (!UtilityFunctions::InsidePolygon(this->_container_boundaries[0], pt.x, pt.y))
		{
			float d = UtilityFunctions::DistanceToClosedCurve(this->_container_boundaries[0], pt);
			//std::cout << d << ">";
			if (d > 5) { return false; }
		}
	}
	return true;
}

void ContainerWorker::PlacementWithMatching3(std::vector<AGraph>& oriGraphs, std::vector<AGraph>& matchedGraphs, std::vector<bool>& oriGraphsFlags)
{
	//std::vector<AGraph> matchedGraphs;
	

	// KNN
	PrepareKNN(oriGraphs);

	// write some code here !!!
	int desc_sz = _padCalc._sorted_descriptors.size();
	for (unsigned int i = 0; i < desc_sz; i++) // loop container descriptors
	{
		// init
		float d = -100000000000;
		
		int best_b_idx = -1;
		int best_c_idx = -1;

		// debug delete me
		std::vector<AVector> l_poly_container;
		std::vector<AVector> r_poly_container;
		GetLRSegments(_padCalc._sorted_descriptors[i], _padCalc._aShape, l_poly_container, r_poly_container);

		// point on container
		//AVector c_pt = _smooth_container[_padCalc._sorted_descriptors[i]._start_index]; // docking
		AVector c_pt = _resampled_container[_padCalc._sorted_descriptors[i]._start_index]; // docking

		// rotation
		int c_l_idx = _padCalc._sorted_descriptors[i].GetMostLeftIndex();
		int c_r_idx = _padCalc._sorted_descriptors[i].GetMostRightIndex();
		//AVector cA = _resampled_container[c_l_idx]; 
		//AVector cB = _resampled_container[c_r_idx];
		AVector cA = c_pt.DirectionTo(_resampled_container[c_l_idx]).Norm();
		AVector cB = c_pt.DirectionTo(_resampled_container[c_r_idx]).Norm();
		AVector cVector = (cB - cA).Norm();

		// scaling
		float s_c = _padCalc._sorted_descriptors[i]._total_length;

		AGraph yay_g; // the choice !

		my_vector_of_info_t indices = GetNN(_padCalc._sorted_descriptors[i].GetDescriptor());
		std::cout << "indices size = " << indices.size() << "\n";
		for(unsigned int ii = 0; ii < indices.size(); ii++)
		{
			int idx1 = indices[ii].first;
			int idx2 = indices[ii].second;

			// non-repeating
			if (oriGraphsFlags[idx1]) continue;

			AGraph candidate_g = oriGraphs[idx1];
			candidate_g.CalculatePADBary();

			// docking
			int e_start_idx = candidate_g._padCalc._sorted_descriptors[idx2]._start_index;
			//AVector e_pt = candidate_g._smooth_skin[e_start_idx];
			AVector e_pt = candidate_g._resampled_skin[e_start_idx];
			candidate_g.SetMatchedPoint(e_pt, c_pt);

			// scaling
			//float s_e = candidate_g._padCalc._sorted_descriptors[idx2]._total_length;
			//candidate_g.Scale(s_c / s_e);
			//candidate_g.Scale(SystemParams::_element_initial_scale);

			// rotation
			int e_l_idx = candidate_g._padCalc._sorted_descriptors[idx2].GetMostLeftIndex();
			int e_r_idx = candidate_g._padCalc._sorted_descriptors[idx2].GetMostRightIndex();

			AVector eA = e_pt.DirectionTo(candidate_g._resampled_skin[e_l_idx]).Norm();
			AVector eB = e_pt.DirectionTo(candidate_g._resampled_skin[e_r_idx]).Norm();
			/*AVector eA = candidate_g._resampled_skin[e_l_idx];
			AVector eB = candidate_g._resampled_skin[e_r_idx];*/
			AVector eVector = (eB - eA).Norm();
			float rotation_value = UtilityFunctions::Angle2D(eVector.x, eVector.y, cVector.x, cVector.y);
			candidate_g.Rotate(rotation_value); // rotation

			// move to a point (not identical to translation)
			candidate_g.Move(c_pt);

			// translation 
			candidate_g.RecalculateArts(); // before translate
			candidate_g.Translate(c_pt - candidate_g._matchedPoint); // should recalculate first
			candidate_g.RecalculateArts();

			// debug delete me
			std::vector<AVector> l_poly;
			std::vector<AVector> r_poly;
			GetLRSegments(candidate_g._padCalc._sorted_descriptors[idx2], candidate_g._padCalc._aShape, l_poly, r_poly);

			

			float dist = GetScore(candidate_g._uniArt, l_poly, r_poly, c_pt, l_poly_container, r_poly_container);
			if (dist > d)
			{
				best_b_idx = idx1;
				best_c_idx = idx2;
				d = dist;
				yay_g = candidate_g;
			}
		}
		//}

		if (best_b_idx == -1 || best_c_idx == -1) { continue; }

		float c_ratio = _padCalc._sorted_descriptors[i]._length_ratio;
		float e_ratio = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx]._length_ratio;
		std::cout << "c_ratio = " << c_ratio << " , e_ratio = " << e_ratio << "\n";

		yay_g.Scale(SystemParams::_element_initial_scale);
		yay_g.Move(c_pt);
		yay_g.Translate(c_pt - yay_g._massList[yay_g._matchedVertexIdx]._pos);
		
		yay_g.RecalculateEdgeLengths();
		yay_g.RecalculateTriangleEdgeLengths();
		yay_g.UpdateBoundaryAndAvgEdgeLength();
		yay_g._ori_id = best_b_idx; // keep track the original graph
		yay_g._bigOne = true;
		yay_g._padCalc._matchedIdx = best_c_idx;

		oriGraphsFlags[best_b_idx] = true;

		matchedGraphs.push_back(yay_g);

		//std::vector<AVector> resampleArt;
		//UtilityFunctions::UniformResampleClosed(yay_g._uniArt, resampleArt, SystemParams::_score_resample_num);
		//_score_debug.insert(_score_debug.end(), resampleArt.begin(), resampleArt.end());

		std::cout << "score func: " << d << "\n";
	}// loop container descriptors

	 // remove random points
	for (int a = _randomPositions.size() - 1; a >= 0; a--)
	{
		float dist = 100000000;
		for (unsigned int b = 0; b < matchedGraphs.size(); b++)
		{
			float d = matchedGraphs[b].DistanceToBoundary(_randomPositions[a]);
			if (d < dist) { dist = d; }
		}

		if (dist < SystemParams::_random_point_gap)
		{
			_randomPositions.erase(_randomPositions.begin() + a);
		}
	}

	int num_pos_limit = SystemParams::_num_element_pos_limit;
	if (num_pos_limit < _randomPositions.size())
	{
		while (_randomPositions.size() != num_pos_limit)
		{
			std::mt19937 g(SystemParams::_seed);
			std::shuffle(_randomPositions.begin(), _randomPositions.end(), g);
			_randomPositions.erase(_randomPositions.begin());
		}
	}

	//return matchedGraphs;
}



my_vector_of_info_t ContainerWorker::GetNN(std::vector<float> queryPt)
{
	my_vector_of_info_t returnArray;

	// do a knn search
	const size_t num_results = SystemParams::_num_nn;
	std::vector<size_t>   ret_indexes(num_results);
	std::vector<double>   out_dists_sqr(num_results);

	nanoflann::KNNResultSet<double> resultSet(num_results);

	resultSet.init(&ret_indexes[0], &out_dists_sqr[0]);
	_mat_index->index->findNeighbors(resultSet, &queryPt[0], nanoflann::SearchParams(10));

	for (size_t i = 0; i < num_results; i++)
	{
		//std::cout << "ret_index[" << i << "]=" << ret_indexes[i] << " out_dist_sqr=" << out_dists_sqr[i] << std::endl;
		returnArray.push_back(_descriptor_info[ret_indexes[i]] );
	}

	return returnArray;
}

void ContainerWorker::PrepareKNN(std::vector<AGraph>& oriGraphs)
{
	//typedef std::vector<std::vector<float> > my_vector_of_vectors_t;
	// first index is which element, the second index 
	//typedef std::vector<std::pair<int, int> > my_vector_of_info_t;

	//my_vector_of_vectors_t _descriptors;
	//my_vector_of_info_t _descriptor_info;

	int dim = SystemParams::_pad_level * 2;

	int g_sz = oriGraphs.size();
	for (unsigned int a = 0; a < g_sz; a++) // loop graphs
	{
		int g_d_sz = oriGraphs[a]._padCalc._sorted_descriptors.size();
		for (unsigned int b = 0; b < g_d_sz; b++) // loop a graph's descriptors
		{
			_descriptors.push_back(oriGraphs[a]._padCalc._sorted_descriptors[b].GetDescriptor() );
			_descriptor_info.push_back(std::pair<int, int>(a, b));
		}
	}

	_mat_index = new my_kd_tree_t(dim /*dim*/, _descriptors, 10 /* max leaf */);
	_mat_index->index->buildIndex();
}

std::vector<AGraph> ContainerWorker::PlacementWithMatching2(std::vector<AGraph>& oriGraphs)
{
	std::vector<AGraph> matchedGraphs;

	// write some code here !!!
	int desc_sz = _padCalc._sorted_descriptors.size();
	for (unsigned int a = 0; a < desc_sz; a++) // loop container descriptors
	{
		// init
		float d        = -100000000000;
		int best_b_idx = -1;
		int best_c_idx = -1;
		int g_sz       = oriGraphs.size();
		
		// point on container
		AVector c_pt = _smooth_container[_padCalc._sorted_descriptors[a]._start_index]; // docking

		// rotation
		int c_l_idx     = _padCalc._sorted_descriptors[a].GetMostLeftIndex();
		int c_r_idx     = _padCalc._sorted_descriptors[a].GetMostRightIndex();
		AVector cA      = _smooth_container[c_l_idx]; AVector cB = _smooth_container[c_r_idx];
		AVector cVector = (cB - cA).Norm();

		AGraph yay_g; // the choice !

		for (unsigned int b = 0; b < g_sz; b++) // loop graphs
		{
			int g_d_sz = oriGraphs[b]._padCalc._sorted_descriptors.size();
			for (unsigned int c = 0; c < g_d_sz; c++) // loop a graph's descriptors
			{
				// need better score function
				//float dist = oriGraphs[b]._padCalc._sorted_descriptors[c].DistanceSquared(_padCalc._sorted_descriptors[a]);

				//if (dist < d)
				{
					AGraph candidate_g = oriGraphs[b];
					candidate_g.CalculatePADBary();

					// docking
					int e_start_idx = candidate_g._padCalc._sorted_descriptors[c]._start_index;
					AVector e_pt    = candidate_g._smooth_skin[e_start_idx];
					candidate_g.SetMatchedPoint(e_pt, c_pt); 

					// scaling
					candidate_g.Scale(SystemParams::_element_initial_scale); 

					// rotation
					int e_l_idx = candidate_g._padCalc._sorted_descriptors[c].GetMostLeftIndex();
					int e_r_idx = candidate_g._padCalc._sorted_descriptors[c].GetMostRightIndex();
					AVector eA  = candidate_g._smooth_skin[e_l_idx];
					AVector eB  = candidate_g._smooth_skin[e_r_idx];
					AVector eVector = (eB - eA).Norm();
					float rotation_value = UtilityFunctions::Angle2D(eVector.x, eVector.y, cVector.x, cVector.y);
					candidate_g.Rotate(rotation_value); // rotation

					// move to a point (not identical to translation)
					candidate_g.Move(c_pt);             

					// translation 
					candidate_g.RecalculateArts(); // before translate
					candidate_g.Translate(c_pt - candidate_g._matchedPoint); // should recalculate first
					candidate_g.RecalculateArts();

					// need better score function
					//if (IsGraphInside(candidate_g)) // are you the one?
					//{
					//float dist = GetScore(candidate_g._uniArt, c_pt);
					float dist = 1234567879;
					if(dist > d)
					{
						best_b_idx = b;
						best_c_idx = c;
						d = dist; // need better score function
						yay_g = candidate_g;
					}
				}
			}// loop a graph's descriptors
		}// loop graphs

		if (best_b_idx == -1 || best_c_idx == -1) { continue; }
				
		float c_ratio = _padCalc._sorted_descriptors[a]._length_ratio;
		float e_ratio = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx]._length_ratio;
		std::cout << "c_ratio = " << c_ratio << " , e_ratio = " << e_ratio << "\n";

		yay_g.RecalculateEdgeLengths();
		yay_g.RecalculateTriangleEdgeLengths();
		yay_g.UpdateBoundaryAndAvgEdgeLength();
		yay_g._ori_id = best_b_idx; // keep track the original graph
		yay_g._bigOne = true;
		yay_g._padCalc._matchedIdx = best_c_idx;
		
		matchedGraphs.push_back(yay_g);

		//std::vector<AVector> resampleArt;
		//UtilityFunctions::UniformResampleClosed(yay_g._uniArt, resampleArt, SystemParams::_score_resample_num);
		//_score_debug.insert(_score_debug.end(), resampleArt.begin(), resampleArt.end());

		std::cout << "score func: " << d << "\n";
	}// loop container descriptors

	// remove random points
	for (int a = _randomPositions.size() - 1; a >= 0; a--)
	{
		float dist = 100000000;
		for (unsigned int b = 0; b < matchedGraphs.size(); b++)
		{
			float d = matchedGraphs[b].DistanceToBoundary(_randomPositions[a]);
			if (d < dist) { dist = d; }
		}

		if (dist < SystemParams::_random_point_gap)
		{
			_randomPositions.erase(_randomPositions.begin() + a);
		}
	}

	return matchedGraphs;
}

void ContainerWorker::DrawDebug(const std::vector<AVector>& uniArt,
								const std::vector<AVector>& l_poly,
								const std::vector<AVector>& r_poly,
								AVector dockPoint,
							const std::vector<AVector>& l_poly_container,
							const std::vector<AVector>& r_poly_container,
								float scoreVal,
								float insideScoreVal,
							    float onScoreVal,
								float outsideScoreVal,
	const std::vector<AVector>& insidePoints,
	const std::vector<AVector>& outsidePoints,
	const std::vector<AVector>& onPoints)
{
	float scale_val = 2.0f;

	std::stringstream ss1;
	ss1 << "matching_" << _debug_counter++;
	_debugImg.SetColorImageToWhite();
	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, this->_container_boundaries[0], MyColor(200, 200, 200), true, 1, scale_val);
	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, l_poly_container, MyColor(0, 0, 255), false, 1, scale_val);
	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, r_poly_container, MyColor(255, 0, 0), false, 1, scale_val);

	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, uniArt, MyColor(100, 100, 100), true, 1, scale_val);

	for (unsigned int a = 0; a < insidePoints.size(); a++)
	{
		AVector pt = insidePoints[a];
		_cvWrapper.DrawCircle(_debugImg._img, pt * scale_val, MyColor(150, 255, 150), 1);
	}
	for (unsigned int a = 0; a < outsidePoints.size(); a++)
	{
		AVector pt = outsidePoints[a];
		_cvWrapper.DrawCircle(_debugImg._img, pt * scale_val, MyColor(255, 150, 150), 1);
	}
	for (unsigned int a = 0; a < onPoints.size(); a++)
	{
		AVector pt = onPoints[a];
		_cvWrapper.DrawCircle(_debugImg._img, pt * scale_val, MyColor(255, 255, 150), 1);

	}

	_cvWrapper.DrawCircle(_debugImg._img, dockPoint * scale_val, MyColor(0, 200, 0), 3);
	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, l_poly, MyColor(0, 0, 255), false, 1, scale_val);
	_cvWrapper.DrawPolyOnCVImage(_debugImg._img, r_poly, MyColor(255, 0, 0), false, 1, scale_val);

	std::stringstream ss2;
	ss2 << "Total score: " << scoreVal;
	_cvWrapper.PutText(_debugImg._img, ss2.str(), AVector(10, 30), MyColor(0, 150, 0));

	ss2.str("");
	ss2 << "Inside score: " << insideScoreVal ;
	_cvWrapper.PutText(_debugImg._img, ss2.str(), AVector(10, 50), MyColor(0, 150, 0));

	ss2.str("");
	ss2 << "On score: " << onScoreVal;
	_cvWrapper.PutText(_debugImg._img, ss2.str(), AVector(10, 70), MyColor(0, 150, 0));

	ss2.str("");
	ss2 << "Outside score: " << outsideScoreVal;
	_cvWrapper.PutText(_debugImg._img, ss2.str(), AVector(10, 90), MyColor(0, 150, 0));

	

	_debugImg.SaveImage(SystemParams::_output_folder + "DEBUG\\" + ss1.str() + ".png");
	
}

float ContainerWorker::GetScore(const std::vector<AVector>& uniArt,
						const std::vector<AVector>& l_poly,
						const std::vector<AVector>& r_poly,
						AVector dockPoint,
						const std::vector<AVector>& l_poly_container,
						const std::vector<AVector>& r_poly_container)
{
	std::vector<AVector> resampleArt;
	UtilityFunctions::UniformResampleClosed(uniArt, resampleArt, SystemParams::_resample_gap_float);

	//_score_debug.insert(_score_debug.end(), resampleArt.begin(), resampleArt.end());

	std::vector<AVector> cBoundary = this->_container_boundaries[0];

	std::vector<float> dists; // negative is inside // positive is outside
	for (unsigned int a = 0; a < resampleArt.size(); a++)
	{
		float d = UtilityFunctions::DistanceToClosedCurve(cBoundary, resampleArt[a]);
		if (UtilityFunctions::InsidePolygon(cBoundary, resampleArt[a].x, resampleArt[a].y))
		{
			d = -d; 
			// negative is inside
		}
		dists.push_back(d);
	}

	// find the most negative
	/*float minVal = 100000000000000;
	for (unsigned int a = 0; a < resampleArt.size(); a++)
	{
		if (dists[a] < minVal) { minVal = dists[a]; }
	}
	float minValPos = -minVal;
	std::cout << "minValPos= " << minValPos << "\n";
	*/
	float minValPos = 353;

	float scoreVal = 0;
	float insideScore = 0;
	float outsideScore = 0;
	float onScoreVal = 0;

	std::vector<AVector> insidePoints;
	std::vector<AVector> outsidePoints;
	std::vector<AVector> onPoints;

	for (unsigned int a = 0; a < resampleArt.size(); a++)
	{
		if (std::abs(dists[a]) <= 1.0f)
		{
			scoreVal += 1;
			onScoreVal += 1;
			onPoints.push_back(resampleArt[a]);
		}
		else if (dists[a] < 0)
		{
			float denomVal = std::pow(minValPos, 2) * SystemParams::_beta_inside;
			float val = std::exp(-1.0f * (dists[a] * dists[a]) / denomVal);
			scoreVal += val;
			insideScore += val;
			insidePoints.push_back(resampleArt[a]);
		}
		
		else
		{
			float val = -SystemParams::_alpha_outside * dists[a];
			scoreVal += val;
			outsideScore += val;
			outsidePoints.push_back(resampleArt[a]);
		}
	}

	DrawDebug(uniArt, l_poly, r_poly, dockPoint, l_poly_container, r_poly_container, scoreVal, insideScore, onScoreVal, outsideScore, insidePoints, outsidePoints, onPoints);

	return scoreVal;
}

std::vector<AGraph> ContainerWorker::PlacementWithMatching(std::vector<AGraph>& oriGraphs)
{
	std::vector<AGraph> matchedGraphs;

	// write some code here !!!
	int desc_sz = _padCalc._sorted_descriptors.size();
	for (unsigned int a = 0; a < desc_sz; a++) // loop container descriptors
	{
		float d = 100000000000;
		int best_b_idx = -1;
		int best_c_idx = -1;
		int g_sz = oriGraphs.size();
		for (unsigned int b = 0; b < g_sz; b++) // loop graphs
		{
			int g_d_sz = oriGraphs[b]._padCalc._sorted_descriptors.size();
			for (unsigned int c = 0; c < g_d_sz; c++) // loop a graph's descriptors
			{
				float dist = oriGraphs[b]._padCalc._sorted_descriptors[c].DistanceSquared(_padCalc._sorted_descriptors[a]);
				//dist /= oriGraphs[b]._padCalc._sorted_descriptors[c]._length_ratio;
				if (dist < d)
				{
					best_b_idx = b;
					best_c_idx = c;
					d = dist;
				}
			}// loop a graph's descriptors
		}// loop graphs

		if (best_b_idx == -1 || best_c_idx == -1) { continue; }

		// docking
		AVector c_pt = _smooth_container[_padCalc._sorted_descriptors[a]._start_index];
		int e_start_idx = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx]._start_index;
		AVector e_pt = oriGraphs[best_b_idx]._smooth_skin[e_start_idx];

		//ALine cLine;
		int c_l_idx = _padCalc._sorted_descriptors[a].GetMostLeftIndex();
		int c_r_idx = _padCalc._sorted_descriptors[a].GetMostRightIndex();
		AVector cA = _smooth_container[c_l_idx];
		AVector cB = _smooth_container[c_r_idx];
		AVector cVector = (cB - cA).Norm();

		//ALine eLine;
		int e_l_idx = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx].GetMostLeftIndex();
		int e_r_idx = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx].GetMostRightIndex();
		AVector eA =  oriGraphs[best_b_idx]._smooth_skin[e_l_idx];
		AVector eB =  oriGraphs[best_b_idx]._smooth_skin[e_r_idx];
		AVector eVector = (eB - eA).Norm();
		float rotation_value = UtilityFunctions::Angle2D(eVector.x, eVector.y, cVector.x, cVector.y);


		//AVector translation_vec = c_pt - e_pt;
		//float scale_vec = _padCalc._sorted_descriptors[a]._total_length / oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx]._total_length ;
		//std::cout << "scale_vec: " << scale_vec << "\n";
		float c_ratio = _padCalc._sorted_descriptors[a]._length_ratio;
		float e_ratio = oriGraphs[best_b_idx]._padCalc._sorted_descriptors[best_c_idx]._length_ratio;
		std::cout << "c_ratio = " << c_ratio << " , e_ratio = " << e_ratio << "\n";


		AGraph g = oriGraphs[best_b_idx]; // prev
		g.CalculatePADBary();			  // prev
		g._isMatched = true;              // prev
		g.SetMatchedPoint(e_pt, c_pt);          // next
		g.Scale(SystemParams::_element_initial_scale); // prev

		g.Rotate(rotation_value); // during
		g.Move(c_pt);             // during
		
		g.RecalculateArts(); // before translate
		g.Translate(c_pt - g._matchedPoint);
		
		g.RecalculateEdgeLengths();
		g.RecalculateTriangleEdgeLengths();
		g.UpdateBoundaryAndAvgEdgeLength();
		g._ori_id = best_b_idx; // keep track the original graph
		g._bigOne = true;
		g._padCalc._matchedIdx = best_c_idx;


		matchedGraphs.push_back(g);
	}// loop container descriptors

	return matchedGraphs;
}

void ContainerWorker::CreatePoints(std::vector<AVector> ornamentBoundary,
									float img_length,
									std::vector<AVector>& randomPoints,
									int& boundaryPointNum)
{
	// how many points
	float fVal = img_length / SystemParams::_upscaleFactor;
	fVal *= fVal;
	int numPoints = SystemParams::_sampling_num * fVal;
	float resamplingGap = std::sqrt(float(numPoints)) / float(numPoints) * img_length;

	std::vector<AVector> resampledBoundary;

	//if (SystemParams::_manual_element_skin)
	//{
	//	resampledBoundary = ornamentBoundary;
	//	resamplingGap *= 0.5;
	//}
	//else
	//{
	ornamentBoundary.push_back(ornamentBoundary[0]); // closed sampling
	float rGap = (float)(resamplingGap * SystemParams::_boundary_sampling_factor);
	UtilityFunctions::UniformResample(ornamentBoundary, resampledBoundary, rGap);
	// bug !!! nasty code
	if (resampledBoundary[resampledBoundary.size() - 1].Distance(resampledBoundary[0]) < rGap * 0.5) // r gap
	{
		resampledBoundary.pop_back();
	}
	//}


	PoissonGenerator::DefaultPRNG PRNG;
	if (SystemParams::_seed > 0)
	{
		PRNG = PoissonGenerator::DefaultPRNG(SystemParams::_seed);
	}
	const auto points = PoissonGenerator::GeneratePoissonPoints(numPoints, PRNG);

	randomPoints.insert(randomPoints.begin(), resampledBoundary.begin(), resampledBoundary.end());
	boundaryPointNum = resampledBoundary.size();

	float sc = img_length * std::sqrt(2.0f);
	float ofVal = 0.5f * (sc - img_length);
	//float ofVal = 0;
	// ---------- iterate points ----------
	for (auto i = points.begin(); i != points.end(); i++)
	{
		float x = (i->x * sc) - ofVal;
		float y = (i->y * sc) - ofVal;
		AVector pt(x, y);

		if (UtilityFunctions::InsidePolygon(ornamentBoundary, pt.x, pt.y))
		{
			float d = UtilityFunctions::DistanceToClosedCurve(resampledBoundary, pt);
			//float d2 = UtilityFunctions::DistanceToBunchOfPoints(randomPoints, pt);
			if (d > resamplingGap/* && d2 > 50*/)
			{
				randomPoints.push_back(pt);
			}
			//AVector cPt = knn->GetClosestPoints(pt, 1)[0];
			//if (cPt.Distance(pt) > resamplingGap)
			//	{ randomPoints.push_back(pt); }
		}
	}

	//delete knn;
}

// reference
void ContainerWorker::GetLRSegments(PADDescriptor desc, std::vector<AVector> poly, std::vector<AVector>& l_poly, std::vector<AVector>& r_poly)
{
	int level_ctr = 0;
	int sz = poly.size();

	// ----------  left part ----------	
	l_poly.push_back(poly[desc._start_index]);
	for (int cur_idx = desc._start_index - 1; ; cur_idx--)
	{
		if (level_ctr == SystemParams::_pad_level) { break; }
		if (cur_idx < 0) { cur_idx = sz - 1; }
		
		l_poly.push_back(poly[cur_idx]);

		if (cur_idx == desc._left_indices[level_ctr])
		{
			level_ctr++;
		}
	}


	// ----------  right part ----------	
	level_ctr = 0;
	r_poly.push_back(poly[desc._start_index]);
	for (int cur_idx = desc._start_index + 1; ; cur_idx++)
	{
		if (level_ctr == SystemParams::_pad_level) { break; }
		if (cur_idx == sz) { cur_idx = 0; }
		
		r_poly.push_back(poly[cur_idx]);

		if (cur_idx == desc._right_indices[level_ctr])
		{
			level_ctr++;
		}
	}
}

void ContainerWorker::Draw()
{
	// ---------- draw boundary ----------
	if (SystemParams::_show_boundary)
	{
		glColor3f(0.615686275, 0.611764706, 0.619607843);
		for (unsigned int i = 0; i < _container_boundaries.size(); i++)
		{
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			int con_sz = _container_boundaries[i].size();
			int b = con_sz - 1;
			for (unsigned int a = 0; a < con_sz; a++)
			{
				glVertex2f(_container_boundaries[i][b].x, _container_boundaries[i][b].y);
				glVertex2f(_container_boundaries[i][a].x, _container_boundaries[i][a].y);				

				b = a;
			}
			glEnd();
		}
		// ---------- draw holes ----------
		for (unsigned int i = 0; i < _holes.size(); i++)
		{
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			int h_size = _holes[i].size();
			int b = h_size - 1;
			for (unsigned int a = 0; a < h_size; a++)
			{
				glVertex2f(_holes[i][b].x, _holes[i][b].y);
				glVertex2f(_holes[i][a].x, _holes[i][a].y);				

				b = a;
			}
			glEnd();
		}

		// ---------- draw something strange ----------
		/*glColor3f(0.615686275, 0.611764706, 0.619607843);
		for (int i = 0; i < _squareContainer.size(); i++)
		{
			glLineWidth(1.0f);
			glBegin(GL_LINES);
			for (unsigned int a = 0; a < _squareContainer[i].size(); a++)
			{
				int b = a + 1;
				if (b >= _squareContainer[i].size()) { b = 0; }

				glVertex2f(_squareContainer[i][a].x, _squareContainer[i][a].y);
				glVertex2f(_squareContainer[i][b].x, _squareContainer[i][b].y);
			}
			glEnd();
		}*/		
	}

	// ---------- draw simplified boundary ----------
	/*glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (unsigned int a = 0; a < _rdp_container.size(); a++)
	{
		int b = a + 1;
		if (b >= _rdp_container.size()) { b = 0; }

		glVertex2f(_rdp_container[a].x, _rdp_container[a].y);
		glVertex2f(_rdp_container[b].x, _rdp_container[b].y);
	}
	glEnd();*/

	// ---------- draw simplified boundary (color coded) ----------
	/*glColor3f(1, 0, 1);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _rdpFlags.size(); a++)
	{
		if (_rdpFlags[a])
		{
			AVector pt = _smooth_container[a];
			glVertex2f(pt.x, pt.y);
		}
		//AVector pt = _aDTransform->_peaks[a];
		//glVertex2f(pt.x, pt.y);
	}
	glEnd(); */

	// ---------- draw smooth boundary ----------
	/*glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (unsigned int a = 0; a < _smooth_container.size(); a++)
	{
		int b = a + 1;
		if (b >= _smooth_container.size()) { b = 0; }

		glVertex2f(_smooth_container[a].x, _smooth_container[a].y);
		glVertex2f(_smooth_container[b].x, _smooth_container[b].y);
	}
	glEnd();*/
	

	// ---------- draw resampled points ----------
	if (SystemParams::_show_shape_matching)
	{
		glPointSize(1.0f);
		glColor3f(1, 0.5, 0.5);
		glBegin(GL_POINTS);
		for (unsigned int a = 0; a < _smooth_container.size(); a++)
		{
			glVertex2f(_smooth_container[a].x, _smooth_container[a].y);
		}
		glEnd();
	}

	// ---------- pyramid of arclength descriptor ----------
	_padCalc.Draw();

	/*if (SystemParams::_show_shape_matching)
	{
		glPointSize(1.0f);
		glColor3f(0, 0.0, 1.0);
		glBegin(GL_POINTS);
		for (unsigned int a = 0; a < _score_debug.size(); a++)
		{
			glVertex2f(_score_debug[a].x, _score_debug[a].y);
		}
		glEnd();
	}*/


	/*glColor3f(1.0, 0.0, 0.0);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	glVertex2f(_resampled_container[0].x, _resampled_container[0].y);
	glEnd();*/

}