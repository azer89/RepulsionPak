
/* ---------- ShapeRadiusMatching V4  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef ABARY_H
#define ABARY_H

/**
* Reza Adhitya Saputra
* radhitya@uwaterloo.ca
* February 2016
*/

/**
* A struct to represent:
*     1. A Point
*     2. A Vector (direction only)
*/
struct ABary
{
public:
	float _u;

	float _v;

	float _w;

	ABary()
	{
		this->_u = 0;
		this->_v = 0;
		this->_w = 0;
	}

	ABary(float u, float v, float w)
	{
		this->_u = u;
		this->_v = v;
		this->_w = w;
	}

	bool IsValid()
	{
		return (_u > 0 && _v > 0 && _w > 0);
	}
};



#endif // ABARY_H