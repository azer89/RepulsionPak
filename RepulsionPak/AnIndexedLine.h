/**
*
* Line representation with index
*
* Author: Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
* Version: 2014
*
*
*/

#ifndef ANINDEXEDLINE_H
#define ANINDEXEDLINE_H

#include "SystemParams.h"

//namespace CVSystem
//{
struct AnIndexedLine
{
public:	
	int _index0; // start index
	
	int _index1; // end index

	//float _maxDist; // limiting the growth

	bool  _canGrow;

	float _angle;

private:
	float _scale;

	float _dist;

	float _oriDist;

public:
	void SetDist(float d){ _dist = d; _oriDist = _dist; }
	float GetDist() const { return _dist; }
	float GetScale() const{ return _scale; }

public:

	// Constructor
	AnIndexedLine(int index0, int index1)
	{
		this->_scale = 1.0f;

		this->_index0 = index0;
		this->_index1 = index1;
		this->_dist = 0.0f;
		this->_angle = 0.0f;

		//this->_maxDist = this->_dist * SystemParams::_growth_scale;
		this->_canGrow = true;
	}


	// Constructor
	AnIndexedLine(int index0, int index1, float dist)
	{
		this->_scale = 1.0f;

		this->_index0 = index0;
		this->_index1 = index1;
		this->_dist = dist;
		this->_angle = 0.0f;

		//this->_maxDist = this->_dist * SystemParams::_growth_scale;
		this->_canGrow = true;
	}

	// Construction without parameters
	/*AnIndexedLine()
	{
		this->_index0 = -1;
		this->_index1 = -1;
		this->_dist = 0;
		this->_angle = 0.0f;

		this->_maxDist = 0;
		this->_canGrow = true;
	}*/

	void MakeLonger(float growth_scale_iter, float dt)
	{
		_scale += growth_scale_iter * dt;
		_dist = _oriDist * _scale;

		//this->_dist *= (1.0f + (growth_scale_iter * dt)); // grow a bit
		//this->_dist += growth_scale_iter * dt;
	}

	// http_://stackoverflow.com/questions/3647331/how-to-swap-two-numbers-without-using-temp-variables-or-arithmetic-operations
	void Swap()
	{
		_index0 ^= _index1;
		_index1 ^= _index0;
		_index0 ^= _index1;
	}
};
//}

#endif