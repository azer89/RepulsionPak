
#ifndef __PAD__
#define __PAD__

#include <iostream>
#include <vector>

#include "AVector.h"
#include "ABary.h"

struct PADDescriptor
{
	/*
	For matching:
		> positive curvature only
		> arclength can be checked by left_lengths[max_level] + right_lengths[max_level]
		> sharp corner has arclength 0.05 ~ 0.1
	*/

public:
	int _start_index;

	float _total_curvature;
	float _total_length;
	float _length_ratio; // length ratio to container length

	std::vector<int>   _left_indices;
	std::vector<int>   _right_indices;

	std::vector<float> _left_lengths;
	std::vector<float> _right_lengths;

	std::vector<float> _left_norms;
	std::vector<float> _right_norms;

public:
	PADDescriptor()
	{
		_total_curvature = 0;
		_total_length = 0;
		_length_ratio = 0;
		_start_index = -1;
	}

	// concat to a single vector
	std::vector<float> GetDescriptor()
	{
		std::vector<float> desc;
		desc.insert(desc.end(), _left_norms.begin(), _left_norms.end());
		desc.insert(desc.end(), _right_norms.begin(), _right_norms.end());
		return desc;
	}

	void Init()
	{
		for (int a = 0; a < SystemParams::_pad_level; a++)
		{
			_left_indices.push_back(0);  // init
			_left_lengths.push_back(0);  // init
			_right_indices.push_back(0); // init
			_right_lengths.push_back(0); // init
		}
	}

	float DistanceSquared(const PADDescriptor& otherDesc)
	{
		float d = 0;

		for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			float l_d = _left_norms[a] - otherDesc._left_norms[a];
			d += (l_d * l_d);


			float r_d = _right_norms[a] - otherDesc._right_norms[a];
			d += (r_d * r_d);
		}

		return d;
	}

	void PostProcess()
	{
		//// pad version
		//float l_len = _left_lengths[SystemParams::_pad_level - 1];
		//float r_len = _right_lengths[SystemParams::_pad_level - 1];
		//_total_length = l_len + r_len;		
		/*for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			float two_pow = std::pow(2.0, a);
			// normalize left
			_left_norms.push_back(_left_lengths[a] / (two_pow * l_len));
			// normalize right
			_right_norms.push_back(_right_lengths[a] / (two_pow * r_len));
		}*/


		//muh version !!!
		_total_length = 0;
		for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			_total_length += _left_lengths[a] + _right_lengths[a];
		}
		/*
		for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			float two_pow = std::pow(2.0, a);

			// normalize left
			_left_norms.push_back(_left_lengths[a] / _total_length);
			//_left_norms.push_back(_left_lengths[a]); // not normalized

			// normalize right
			_right_norms.push_back(_right_lengths[a] / _total_length);
			//_right_norms.push_back(_right_lengths[a]); // not normalized
		}*/

		// another version
		float vec_length = 0;
		for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			vec_length += (_left_lengths[a] * _left_lengths[a]) + (_right_lengths[a] * _right_lengths[a]);
		}

		vec_length = std::sqrt(vec_length);

		for (unsigned int a = 0; a < SystemParams::_pad_level; a++)
		{
			_left_norms.push_back(_left_lengths[a] / vec_length);
			_right_norms.push_back(_right_lengths[a] / vec_length);
		}
	}

	int GetMostLeftIndex()
	{
		int plv = SystemParams::_pad_level;
		return _left_indices[plv - 1];
	}

	int GetMostRightIndex()
	{
		int plv = SystemParams::_pad_level;
		return _right_indices[plv - 1];
	}

	void Print()
	{
		std::cout << "total_curvature=" << _total_curvature << "\n";
		std::cout << "_total_length=" << _total_length << "\n";
		std::cout << "_length_ratio=" << _length_ratio << "\n";
		for (int a = 0; a < SystemParams::_pad_level; a++)
		{
			//std::cout << "left_index="   << _left_indices[a] << " "; 
			//std::cout << "left_length="  << _left_lengths[a] << " "; 
			std::cout << "left_norm=" << _left_norms[a] << " ";
			//std::cout << "right_index="  << _right_indices[a] << " ";
			//std::cout << "right_length=" << _right_lengths[a] << " ";
			std::cout << "right_norm=" << _right_norms[a] << "\n";
		}
		std::cout << "\n";
	}
};


class PADCalculator
{
public:
	PADCalculator();
	~PADCalculator();

	void DetectSalientParts();
	bool IsConvex(const PADDescriptor& desc);
	bool IsGoodAngle(const PADDescriptor& desc);
	float DistToDescriptors(std::vector<PADDescriptor> descs, AVector pt);
	void ComputePAD(std::vector<AVector> aShape, std::vector<bool> rdpFlags);
	PADDescriptor ComputePADatPoint(int idx);
	PADDescriptor ComputePADatPoint2(int idx);
	PADDescriptor ComputePADatPoint3(int idx);

	// input output
	void GetFirstDeriv(std::vector<float> inputCurve, std::vector<float>& dCurve);

	// input output
	void GetSecondDeriv(std::vector<float> inputCurve, std::vector<float>& ddCurve);

	// input input output
	void GetCurvature(std::vector<float>  dx,
		std::vector<float>  dy,
		std::vector<float>  ddx,
		std::vector<float>  ddy,
		std::vector<float>& ks);

	//void PADCalculator::FindClickedPoint(float x, float y);

	void Draw();
	void Draw2();

	void CalculateBarycentric(std::vector<std::vector<AVector>> actualTriangles);
	//void UpdateAShapeWithBary();


public:
	int  _clickedIdx;
	int  _matchedIdx;
	bool _isElement;

	std::vector<int> _rdpIndices;  // index only for extrema points

	// these are not sorted
	std::vector<PADDescriptor> _descriptors; // pad descriptor for each point

	// these are sorted
	std::vector<PADDescriptor> _sorted_descriptors; // pad descriptor for each point

	std::vector<AVector> _aShape; // the shape we want to measure
	std::vector<float>   _curvature; // curvature of the shame
	std::vector<ABary>	 _aShapeBarys;
	std::vector<int>	 _aShapeTriIdxs;
	//std::vector<int> _pyramid_indices;

	float _max_curvature;
	float _min_curvature;


};

#endif
