/**
* Struct for quad data
*
*
* Author: Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
* Version: 2014
*
*
*/

#ifndef __A_QUAD_H__
#define __A_QUAD_H__

#include "AVector.h"

//namespace CVSystem
//{
	// Representation of quad with four vertices
struct AQuad
{
public:
	// start point
	AVector p0;

	// first control point
	AVector p1;

	// second control point
	AVector p2;

	// end point
	AVector p3;

	// constructor
	AQuad(AVector p0, AVector p1, AVector p2, AVector p3)
	{
		this->p0 = p0;
		this->p1 = p1;
		this->p2 = p2;
		this->p3 = p3;
	}


	// scaling
	AQuad Resize(double val)
	{
		AQuad newQ(this->p0.Resize(val), this->p1.Resize(val), this->p2.Resize(val), this->p3.Resize(val));
		return newQ;
	}
};
//}
#endif