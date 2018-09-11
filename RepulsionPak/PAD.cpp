
#include "PAD.h"

#include "UtilityFunctions.h"

#include "glew.h"
#include "freeglut.h"
#include <algorithm>

#include "SystemParams.h"

//float PADCalculator::_pad_step = 2.0f;

PADCalculator::PADCalculator()
{
	_clickedIdx = -1;
	_matchedIdx = -1;
	_isElement = false;
}

PADCalculator::~PADCalculator()
{
}

void PADCalculator::ComputePAD(std::vector<AVector> aShape, std::vector<bool> rdpFlags)
{
	//std::vector<AVector> sampledShape = aShape;

	//UtilityFunctions::UniformResampleClosed(aShape, sampledShape, SystemParams::_pad_step);
	//UtilityFunctions::UniformResampleClosed(aShape, sampledShape, SystemParams::_pad_step_int);

	/*bool notDone = true;
	while (notDone)
	{
		int sz = sampledShape.size();
		if (sampledShape[sz - 1].Distance(sampledShape[0]) < SystemParams::_pad_step)
		{
			sampledShape.pop_back();
		}
		else
		{
			notDone = false;
		}
	}*/

	this->_aShape = aShape;

	// clockwise or counter clockwise
	if (UtilityFunctions::IsClockwise(_aShape)) { std::cout << "clockwise !!!\n"; }
	else { std::cout << "---------- counter-clockwise !!! ----------\n"; }

	int sampled_sz = aShape.size();

	std::vector<float> curve_x, curve_y;
	std::vector<float> dx, dy;  // first derivatives
	std::vector<float> ddx, ddy; // second derivatives

	std::vector<float> ks; // curvature

	for (unsigned int a = 0; a < sampled_sz; a++)
	{
		curve_x.push_back(_aShape[a].x);
		curve_y.push_back(_aShape[a].y);
	} 

	GetFirstDeriv(curve_x, dx);
	GetFirstDeriv(curve_y, dy);

	GetSecondDeriv(curve_x, ddx);
	GetSecondDeriv(curve_y, ddy);

	GetCurvature(dx,
				dy,
				ddx,
				ddy,
				ks);

	//for (unsigned int a = 0; a < sampledShape.size(); a++)
	//{
	//	std::cout << ks[a] << "   ";
	//}
	//std::cout << "PAD done\n";
	_max_curvature = -1000000000000;
	_min_curvature =  1000000000000;
	for (unsigned int a = 0; a < sampled_sz; a++)
	{
		if (ks[a] > _max_curvature) { _max_curvature = ks[a]; }
		if (ks[a] < _min_curvature) { _min_curvature = ks[a]; }
	}

	// curvature
	_curvature = ks;

	// PAD descriptor
	_descriptors = std::vector<PADDescriptor>(_aShape.size());
	for (unsigned int a = 0; a < rdpFlags.size(); a++)
	{
		if (rdpFlags[a])
		{
			_rdpIndices.push_back(a); // index only
			_descriptors[a] = ComputePADatPoint3(a); // compute descriptor
		}
	}

	// sort
	/*_sorted_descriptors = _descriptors;

	// sort
	std::sort(_sorted_descriptors.begin(), _sorted_descriptors.end(), [](const PADDescriptor &x,
		const PADDescriptor &y)
	{ return x._total_length < y._total_length; });

	for (int a = 0; a < 10; a++)
	{
		std::cout << _sorted_descriptors[a]._total_length << "\n";
	}*/
	DetectSalientParts();

	//std::cout << "PAD done!\n";

	//float param_gap = 1.0f / (float)SystemParams::_pad_step_int;
	//float param_gap = 1.0f / (float)SystemParams::_resample_num;;
	//float param_gap = 1.0f;
	
	//
	/*int ctr = 0;
	float sumk = 0.0f;
	_pyramid_indices.push_back(0);
	for (unsigned int a = 0; a < sampled_sz; a++)
	{
		if (ctr == SystemParams::_pad_level) { break; }

		
		int b = a + 1;
		if (b == sampled_sz) { b == 0; }
		// integral of absolute curvature
		// formula from:
		//     Data point selection for piecewise linear curve approximation
		sumk += (std::abs(ks[a]) + std::abs(ks[b])) * 0.5 * param_gap;

		if (sumk >= std::pow(2.0f, ctr) * SystemParams::_pad_delta)
		{
			_pyramid_indices.push_back(a);
			//std::cout << a << "  ";
			ctr++;
		}
	}*/

	// calculate absolute curvature
	/*float total_abs_k = 0;
	for (unsigned int a = 0; a < sampledShape.size(); a++)
	{
		int b = a + 1;
		if (a == sampledShape.size() - 1) b = 0;

		total_abs_k += (std::abs(ks[a]) + std::abs(ks[b])) * PADCalculator::_pad_step * 0.5;

		std::cout << total_abs_k << "   ";
	}*/
}

bool PADCalculator::IsGoodAngle(const PADDescriptor& desc)
{
	int prevIdx = desc._start_index - 1;
	int curIdx = desc._start_index;
	int nextIdx = desc._start_index + 1;

	if (prevIdx < 0) { prevIdx = _aShape.size() - 1; }
	if (nextIdx >= _aShape.size()) { nextIdx = 0; }

	AVector vec1 = (_aShape[curIdx] - _aShape[prevIdx]).Norm();
	AVector vec2 = (_aShape[nextIdx] - _aShape[curIdx]).Norm();

	//return true;
	float angleVal = UtilityFunctions::Angle2D(vec1.x, vec1.y, vec2.x, vec2.y);
	if (angleVal >= 0)
	{
		if (angleVal < SystemParams::_angle_cap) { return false; }

		//std::cout << "angleVal: " << angleVal << "\n";

		return true;
	}

	return false;
}

bool PADCalculator::IsConvex(const PADDescriptor& desc)
{
	int prevIdx = desc._start_index - 1;
	int curIdx  = desc._start_index;
	int nextIdx = desc._start_index + 1;

	if (prevIdx < 0) { prevIdx = _aShape.size() - 1; }
	if (nextIdx >= _aShape.size()) { nextIdx = 0; }
	
	AVector vec1 = (_aShape[curIdx] - _aShape[prevIdx]).Norm();
	AVector vec2 = (_aShape[nextIdx] - _aShape[curIdx]).Norm();

	//return true;
	float angleVal = UtilityFunctions::Angle2D(vec1.x, vec1.y, vec2.x, vec2.y);
	if (angleVal >= 0)
	{
		//if (angleVal > 1.0472) { return false; }
		return true;
	}

	return false;
}

void PADCalculator::DetectSalientParts()
{
	// sort
	//_sorted_descriptors = _descriptors;
	std::vector<PADDescriptor> temp_descriptors = _descriptors;

	// sort
	std::sort(temp_descriptors.begin(), temp_descriptors.end(), [](const PADDescriptor &x,
		const PADDescriptor &y)
	{ return x._total_length < y._total_length; });

	float salient_gap = SystemParams::_container_salient_gap; 

	// ---------- element ----------
	if (_isElement)
	{
		//salient_gap = SystemParams::_element_salient_gap;
		int sz = temp_descriptors.size();
		if (IsConvex(temp_descriptors[0])) { _sorted_descriptors.push_back(temp_descriptors[0]); }
		for (int a = 1; a < sz; a++)
		{
			//if (temp_descriptors[a]._total_curvature < 0) { continue; }
			//if (temp_descriptors[a]._length_ratio > SystemParams::_desc_max_length_ratio) { continue; }

			AVector pt = _aShape[temp_descriptors[a]._start_index];
			if (/*DistToDescriptors(_sorted_descriptors, pt) > salient_gap &&*/ IsGoodAngle(temp_descriptors[a]) )
			{
				_sorted_descriptors.push_back(temp_descriptors[a]);
			}
		}

		return;
	}

	// ---------- container ----------
	int sz = temp_descriptors.size();
	if (IsConvex(temp_descriptors[0])) { _sorted_descriptors.push_back(temp_descriptors[0]); }
	for (int a = 1; a < sz; a++)
	{
		//if (temp_descriptors[a]._total_curvature < 0) { continue; }
		//if (temp_descriptors[a]._length_ratio > SystemParams::_desc_max_length_ratio) { continue; }

		AVector pt = _aShape[temp_descriptors[a]._start_index];
		if (DistToDescriptors(_sorted_descriptors, pt) > salient_gap &&
			IsGoodAngle(temp_descriptors[a] ))
		{
			_sorted_descriptors.push_back(temp_descriptors[a]);
		}
	}
}

float PADCalculator::DistToDescriptors(std::vector<PADDescriptor> descs, AVector pt)
{
	int sz = descs.size();
	float d = 100000000;
	for (unsigned int a = 0; a < sz; a++)
	{
		AVector pt2 = _aShape[descs[a]._start_index];
		float dist = pt2.DistanceSquared(pt);
		if (dist < d) 
		{
			d = dist;
		}
	}
	return d;
}

// use this
PADDescriptor PADCalculator::ComputePADatPoint3(int idx)
{
	PADDescriptor padDesc;
	padDesc.Init();

	int sz = _aShape.size(); // num of sampled points of container boundary
	float param_gap = 1.0f; // arclength gap between two sampled points

	float total_abs_k  = 0;    // total of absolute curvature
	float total_length = 0;    // length of curve
	int   level_ctr    = 0;    // iter for pad level
	int   prev_idx     = idx;  // previous point index
	float goal_length  = (level_ctr + 1) * SystemParams::_pad_delta;

	float total_k = 0; // curvature
	total_k += _curvature[prev_idx]; // curvature

									 // left	
	for (int cur_idx = idx - 1; cur_idx != idx; cur_idx--) // this will stop if goes back to where we start
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop if reach highest pad level
		if (cur_idx < 0) { cur_idx = sz - 1; }
		if (cur_idx == idx) { std::cout << " <<< "; break; }

		total_k += _curvature[cur_idx]; // curvature
		total_abs_k += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += SystemParams::_resample_gap_float;
		if (total_abs_k >= goal_length)
		{
			padDesc._left_indices[level_ctr] = cur_idx;
			padDesc._left_lengths[level_ctr] = total_length; // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;      // update
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	// right
	total_abs_k  = 0;  // reset
	total_length = 0;  // reset
	level_ctr    = 0;  // reset
	goal_length  = (level_ctr + 1) * SystemParams::_pad_delta; // reset
	prev_idx     = idx;
	for (int cur_idx = idx + 1; cur_idx != idx; cur_idx++)
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop
		if (cur_idx == sz) { cur_idx = 0; } // move
		if (cur_idx == idx) { std::cout << " >>> "; break; }

		total_k += _curvature[cur_idx]; // curvature
		total_abs_k += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += SystemParams::_resample_gap_float;
		if (total_abs_k >= goal_length)
		{
			padDesc._right_indices[level_ctr] = cur_idx;
			padDesc._right_lengths[level_ctr] = total_length;  // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	padDesc._start_index = idx;
	padDesc.PostProcess();
	padDesc._length_ratio = padDesc._total_length / UtilityFunctions::CurveLengthClosed(_aShape);
	padDesc._total_curvature = total_k; // curvature

	return padDesc;
}

// use this
PADDescriptor PADCalculator::ComputePADatPoint2(int idx)
{
	PADDescriptor padDesc;
	padDesc.Init();

	int sz = _aShape.size(); // num of sampled points of container boundary
	float param_gap = 1.0f; // arclength gap between two sampled points
	
	float total_abs_k  = 0;    // total of absolute curvature
	float total_length = 0;    // length of curve
	int   level_ctr    = 0;    // iter for pad level
	int   prev_idx     = idx;  // previous point index
	float goal_length  = (level_ctr + 1) * SystemParams::_pad_delta;

	float total_k = 0; // curvature
	total_k      += _curvature[prev_idx]; // curvature
	
	// left	
	for (int cur_idx = idx - 1; cur_idx != idx; cur_idx--) // this will stop if goes back to where we start
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop if reach highest pad level
		if (cur_idx < 0)    { cur_idx = sz - 1; }
		if (cur_idx == idx) { std::cout << " ~~~ "; break; }

		total_k      += _curvature[cur_idx]; // curvature
		total_abs_k  += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += _aShape[cur_idx].Distance(_aShape[prev_idx]);
		if (total_abs_k >= goal_length)
		{
			padDesc._left_indices[level_ctr] = cur_idx;
			padDesc._left_lengths[level_ctr] = total_length; // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;      // update
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	// right
	total_abs_k  = 0;  // reset
	total_length = 0;  // reset
	level_ctr    = 0;  // reset
	goal_length  = (level_ctr + 1) * SystemParams::_pad_delta; // reset
	prev_idx     = idx;
	for (int cur_idx = idx + 1; cur_idx != idx; cur_idx++)
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop
		if (cur_idx == sz)  { cur_idx = 0; } // move
		if (cur_idx == idx) { std::cout << " ~~~ "; break; }

		total_k      += _curvature[cur_idx]; // curvature
		total_abs_k  += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += _aShape[cur_idx].Distance(_aShape[prev_idx]);
		if (total_abs_k >= goal_length)
		{
			padDesc._right_indices[level_ctr] = cur_idx;
			padDesc._right_lengths[level_ctr] = total_length;  // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	padDesc._start_index = idx;
	padDesc.PostProcess();
	padDesc._length_ratio = padDesc._total_length / UtilityFunctions::CurveLengthClosed(_aShape);
	padDesc._total_curvature = total_k; // curvature

	return padDesc;
}

// don't use this
PADDescriptor PADCalculator::ComputePADatPoint(int idx)
{
	PADDescriptor padDesc;
	padDesc.Init();

	int sz = _aShape.size(); // num of sampled points of container boundary
	float param_gap = 1.0f; // arclength gap between two sampled points

	float total_abs_k  = 0;    // total of absolute curvature
	float total_length = 0;    // length of curve
	int   level_ctr    = 0;    // iter for pad level
	int   prev_idx     = idx;  // previous point index
	//float goal_length  = (level_ctr + 1) * SystemParams::_pad_delta;
	float goal_length = std::pow(2.0f, level_ctr) * SystemParams::_pad_delta;

	float total_k = 0; // curvature
	total_k += _curvature[prev_idx]; // curvature

									 // left	
	for (int cur_idx = idx - 1; cur_idx != idx; cur_idx--) // this will stop if goes back to where we start
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop if reach highest pad level
		if (cur_idx < 0) { cur_idx = sz - 1; }
		if (cur_idx == idx) { std::cout << " <<< "; break; }

		total_k += _curvature[cur_idx]; // curvature
		total_abs_k += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += SystemParams::_resample_gap_float;
		if (total_abs_k >= goal_length)
		{
			padDesc._left_indices[level_ctr] = cur_idx;
			padDesc._left_lengths[level_ctr] = total_length; // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;      // update
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	// right
	total_abs_k  = 0;  // reset
	total_length = 0;  // reset
	level_ctr    = 0;  // reset
	goal_length  = (level_ctr + 1) * SystemParams::_pad_delta; // reset
	goal_length  = goal_length = std::pow(2.0f, level_ctr) * SystemParams::_pad_delta;
	prev_idx     = idx;
	for (int cur_idx = idx + 1; cur_idx != idx; cur_idx++)
	{
		if (level_ctr == SystemParams::_pad_level) { break; } // stop
		if (cur_idx == sz) { cur_idx = 0; } // move
		if (cur_idx == idx) { std::cout << " >>> "; break; }

		total_k += _curvature[cur_idx]; // curvature
		total_abs_k += (std::abs(_curvature[cur_idx]) + std::abs(_curvature[prev_idx])) * 0.5 * param_gap; // trapezium area
		total_length += SystemParams::_resample_gap_float;
		if (total_abs_k >= goal_length)
		{
			padDesc._right_indices[level_ctr] = cur_idx;
			padDesc._right_lengths[level_ctr] = total_length;  // this is gap length !!!

			total_length = 0; // reset
			level_ctr++;
			goal_length = (level_ctr + 1) * SystemParams::_pad_delta; // update
		}
		prev_idx = cur_idx;
	}

	padDesc._start_index = idx;
	padDesc.PostProcess();
	padDesc._length_ratio = padDesc._total_length / UtilityFunctions::CurveLengthClosed(_aShape);
	padDesc._total_curvature = total_k; // curvature

	return padDesc;
}

void PADCalculator::FindClickedPoint(float x, float y)
{
	// clicking is disabled
	AVector pt(x, y);
	float minDist = std::numeric_limits<float>::max();

	std::cout << "PADCalculator::FindClickedMass\n";

	for (unsigned int a = 0; a < _rdpIndices.size(); a++)
	{

		float d = this->_aShape[_rdpIndices[a]].DistanceSquared(pt);
		if (d < minDist)
		{
			minDist = d;
			_clickedIdx = _rdpIndices[a];
		}
	}

	if (minDist > 10000.0f) { _clickedIdx = -1; }
	else
	{
		std::cout << "_clickedIdx = " << _clickedIdx << "\n";
		_descriptors[_clickedIdx].Print();
		std::cout << "\n";
	}
}

void PADCalculator::CalculateBarycentric(std::vector<std::vector<AVector>> actualTriangles)
{
	for (unsigned int a = 0; a < _aShape.size(); a++)
	{
		int triIdx = -1;
		ABary bary;
		for (unsigned int c = 0; c < actualTriangles.size(); c++)
		{
			if (UtilityFunctions::InsidePolygon(actualTriangles[c], _aShape[a].x, _aShape[a].y))
			{
				triIdx = c;
				break;
			}
		}
		if (triIdx == -1)
		{
			//std::cout << "u error !!!\n";

			triIdx = -1;
			float dist = 100000000;
			for (unsigned int c = 0; c < actualTriangles.size(); c++)
			{
				float d = UtilityFunctions::DistanceToClosedCurve(actualTriangles[c], _aShape[a]);
				if (d < dist)
				{
					dist = d;
					triIdx = c;
				}
			}
		}
		//else

		//{
		bary = UtilityFunctions::Barycentric(_aShape[a],
			actualTriangles[triIdx][0],
			actualTriangles[triIdx][1],
			actualTriangles[triIdx][2]);
		//}
		_aShapeBarys.push_back(bary);
		_aShapeTriIdxs.push_back(triIdx);
	}
}

void PADCalculator::Draw2()
{
	//glLineWidth(0.5f);
	glColor3f(0.8, 0.8, 0.8);
	//glBegin(GL_LINES);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _aShape.size(); a++)
	{
		//int b = a + 1;
		//if (b >= _aShape.size()) { b = 0; }

		// color 1
		glVertex2f(_aShape[a].x, _aShape[a].y);
		//glVertex2f(_aShape[b].x, _aShape[b].y);
	}
	glEnd();
	

	if (_matchedIdx >= 0)
	{
		//std::cout << ".";

		int sz = _aShape.size();
		PADDescriptor desc = _sorted_descriptors[_matchedIdx];
		int startIdx = desc._start_index;
		int level_ctr = 0;

		// left
		glLineWidth(1.0f);
		glBegin(GL_LINES);
	
		int prev_idx = startIdx;
		for (int cur_idx = startIdx - 1; ; cur_idx--)
		{
			if (level_ctr == SystemParams::_pad_level) { break; }
			if (cur_idx < 0) { cur_idx = sz - 1; }
			if(cur_idx == startIdx) { break; }


			//float otherCol = 1.0f - (float)level_ctr / (float)SystemParams::_pad_level;
			//if (otherCol > 0.999) { otherCol = 0.9; }
			//glColor3f(otherCol, otherCol, 1.0f);
			glColor3f(1.0, 0.5, 0.5f);
			glVertex2f(_aShape[prev_idx].x, _aShape[prev_idx].y);
			glVertex2f(_aShape[cur_idx].x, _aShape[cur_idx].y);

			if (cur_idx == desc._left_indices[level_ctr])
			{
				level_ctr++;
			}

			prev_idx = cur_idx;
		}

		glEnd();

		// draw right
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		prev_idx = startIdx;
		level_ctr = 0;
		for (int cur_idx = startIdx + 1; ; cur_idx++)
		{
			if (level_ctr == SystemParams::_pad_level) { break; }
			if (cur_idx == sz) { cur_idx = 0; }
			if (cur_idx == startIdx) { break; }

			//float otherCol = 1.0f - (float)level_ctr / (float)SystemParams::_pad_level;
			//if (otherCol > 0.999) { otherCol = 0.9; }
			//glColor3f(otherCol, otherCol, 1.0f);
			glColor3f(0.5, 1.0, 0.5f);
			glVertex2f(_aShape[prev_idx].x, _aShape[prev_idx].y);
			glVertex2f(_aShape[cur_idx].x, _aShape[cur_idx].y);

			if (cur_idx == desc._right_indices[level_ctr])
			{
				level_ctr++;
			}

			prev_idx = cur_idx;
		}

		glEnd();


		// ---------- start point ----------
		glColor3f(0, 0, 1);
		glPointSize(2.0);
		glBegin(GL_POINTS);
		glVertex2f(_aShape[startIdx].x, _aShape[startIdx].y);
		glEnd();


		glColor3f(0, 0, 1);
		glPointSize(2.0);
		glBegin(GL_POINTS);
		for (int a = 0; a < SystemParams::_pad_level; a++)
		{
			int l_idx = desc._left_indices[a];
			int r_idx = desc._right_indices[a];

			glVertex2f(_aShape[l_idx].x, _aShape[l_idx].y);
			glVertex2f(_aShape[r_idx].x, _aShape[r_idx].y);
		}
		glEnd();
	}
}

void PADCalculator::Draw()
{
	if (!SystemParams::_show_shape_matching) { return; }

	// ---------- clicked ----------
	if (_clickedIdx >= 0)
	{
		int sz = _aShape.size();
		PADDescriptor desc = _descriptors[_clickedIdx];

		int level_ctr = 0;

		// ---------- draw left part ----------
		glLineWidth(3.0f);
		glBegin(GL_LINES);
		int prev_idx = _clickedIdx;
		for (int cur_idx = _clickedIdx - 1; ; cur_idx--)
		{
			if (level_ctr == SystemParams::_pad_level) { break; }
			if (cur_idx < 0) { cur_idx = sz - 1; }			

			float otherCol = 1.0f - (float)level_ctr / (float)SystemParams::_pad_level;
			if (otherCol > 0.999) { otherCol = 0.9; }
			glColor3f(otherCol, otherCol, 1.0f);
			glVertex2f(_aShape[prev_idx].x, _aShape[prev_idx].y);
			glVertex2f(_aShape[cur_idx].x, _aShape[cur_idx].y);

			if (cur_idx == desc._left_indices[level_ctr])
				{ level_ctr++; }

			prev_idx = cur_idx;
		}

		glEnd();

		// ---------- draw right part ----------
		glLineWidth(3.0f);
		glBegin(GL_LINES);
		prev_idx = _clickedIdx;
		level_ctr = 0;
		for (int cur_idx = _clickedIdx + 1; ; cur_idx++)
		{
			if (level_ctr == SystemParams::_pad_level) { break; }
			if (cur_idx == sz) { cur_idx = 0; }

			float otherCol = 1.0f - (float)level_ctr / (float)SystemParams::_pad_level;
			if (otherCol > 0.999) { otherCol = 0.9; }
			glColor3f(otherCol, otherCol, 1.0f);
			glVertex2f(_aShape[prev_idx].x, _aShape[prev_idx].y);
			glVertex2f(_aShape[cur_idx].x,  _aShape[cur_idx].y);

			if (cur_idx == desc._right_indices[level_ctr])
				{ level_ctr++; }

			prev_idx = cur_idx;
		}

		glEnd();

		// ---------- clicked point ----------
		glColor3f(1, 0.2, 0.2);
		glPointSize(3.0);
		glBegin(GL_POINTS);
		glVertex2f(_aShape[_clickedIdx].x, _aShape[_clickedIdx].y);
		glEnd();
		
		// ---------- clicked points on left and right ----------
		glColor3f(0.1, 0.4, 0.1);
		glPointSize(3.0);
		glBegin(GL_POINTS);
		for (int a = 0; a < SystemParams::_pad_level; a++)
		{
			int l_idx = _descriptors[_clickedIdx]._left_indices[a];
			int r_idx = _descriptors[_clickedIdx]._right_indices[a];

			glVertex2f(_aShape[l_idx].x, _aShape[l_idx].y);
			glVertex2f(_aShape[r_idx].x, _aShape[r_idx].y);
		}
		glEnd();
	}

	// ---------- draw descriptor points on the sorted array ----------
	glColor3f(0, 0.4, 0);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < _sorted_descriptors.size(); i++)
	{
		int _idx = _sorted_descriptors[i]._start_index;
		glVertex2f(_aShape[_idx].x, _aShape[_idx].y);
	}
	glEnd();
	
	/*glLineWidth(3.0f);
	glColor3f(0.0, 0.0, 0);
	glBegin(GL_LINES);
	for (unsigned int a = 0; a < _aShape.size(); a++)
	{
		int b = a + 1;
		if (b >= _aShape.size()) { b = 0; }

		// color 1
		glVertex2f(_aShape[a].x, _aShape[a].y);
		glVertex2f(_aShape[b].x, _aShape[b].y);
	}
	glEnd();
	*/

	/*glLineWidth(5.0f);
	glBegin(GL_LINES);
	int pyramid_sz = _pyramid_indices.size();
	for (unsigned int a = 0; a < pyramid_sz - 1; a++)
	{
		int idx0 = _pyramid_indices[a];
		int idx1 = _pyramid_indices[a + 1];
		float otherCol = 1.0f - (float)a / (float)pyramid_sz;
		glColor3f(otherCol, otherCol, 1.0f);
		for (unsigned int b = idx0; b < idx1; b++)
		{
			glVertex2f(_aShape[b].x, _aShape[b].y);
			glVertex2f(_aShape[b+1].x, _aShape[b+1].y);
		}
	}

	glEnd();*/
}

// input output
void PADCalculator::GetFirstDeriv(std::vector<float> inputCurve, std::vector<float>& dCurve)
{
	int sz = inputCurve.size();
	for (unsigned int a = 0; a < sz; a++)
	{
		// current 
		float i = inputCurve[a];
		
		// next
		float i_1 = inputCurve[0];
		if (a != sz - 1) { i_1 = inputCurve[a + 1]; }

		dCurve.push_back(i_1 - i); // first derivative
	}
}

void PADCalculator::GetCurvature(std::vector<float> dx,
								std::vector<float> dy,
								std::vector<float> ddx,
								std::vector<float> ddy,
								std::vector<float>& ks)
{
	// en.wikipedia.org/wiki/Curvature#Local_expressions

	int sz = dx.size();
	for (unsigned int a = 0; a < sz; a++)
	{
		float nom = dx[a] * ddy[a] - dy[a] * ddx[a];
		
		float denom = dx[a] * dx[a] + dy[a] * dy[a];
		denom = std::pow(denom, 1.5f);

		ks.push_back(nom / denom);
	}
}

// input output
void PADCalculator::GetSecondDeriv(std::vector<float> inputCurve, std::vector<float>& ddCurve)
{
	int sz = inputCurve.size();
	for (unsigned int a = 0; a < sz; a++)
	{
		float i   = inputCurve[a];
		float i_1 = i; // next
		float i_2 = i; // next next

		if (a == sz - 1)
		{
			i_1 = inputCurve[0];
			i_2 = inputCurve[1];
		}
		else if (a == sz - 2)
		{
			i_1 = inputCurve[sz - 1];
			i_2 = inputCurve[0];
		}
		else
		{
			i_1 = inputCurve[a + 1];
			i_2 = inputCurve[a + 2];
		}

		// en.wikipedia.org/wiki/Finite_difference#Higher-order_differences
		// 2nd order forward
		ddCurve.push_back(i_2 - (2.0f * i_1) + i ); // second derivative
	}
}