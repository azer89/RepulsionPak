
/* ---------- ShapeRadiusMatching V2  ---------- */

#ifndef ALINE_H
#define ALINE_H

#include "AVector.h"

#include <limits>
#include <cmath>

/**
* Reza Adhitya Saputra
* radhitya@uwaterloo.ca
* February 2016
*/

/**
* A struct to represent:
*     1. A Finite Lines
*     2. A Ray with a start point and a direction
*/
struct ALine
{
public:
	float XA;	float YA;	// start
	float XB;	float YB;	// end

	// Constructor #1
	ALine()
	{
		this->XA = -1;	this->YA = -1;
		this->XB = -1;	this->YB = -1;
	}

	// Constructor #2
	ALine(float XA, float YA, float XB, float YB)
	{
		this->XA = XA;	this->YA = YA;
		this->XB = XB;	this->YB = YB;
	}

	ALine(AVector v1, AVector v2)
	{
		this->XA = v1.x;	this->YA = v1.y;
		this->XB = v2.x;	this->YB = v2.y;
	}


	ALine Resize(float val)
	{
		ALine newL;

		newL.XA = this->XA * val;
		newL.YA = this->YA * val;

		newL.XB = this->XB * val;
		newL.YB = this->YB * val;

		return newL;
	}

	// Uninitialized ?
	bool Invalid()
	{
		if (((int)XA) == -1 && ((int)YA) == -1 && ((int)XB) == -1 && ((int)YB) == -1) return true;
		return false;
	}

	AVector GetMiddlePoint()
	{
		AVector startPt(XA, YA);
		AVector endPt(XB, YB);
		return startPt + (endPt - startPt) * 0.5f;
	}

	void SetPointA(AVector pt)
	{
		XA = pt.x;
		YA = pt.y;
	}

	void SetPointB(AVector pt)
	{
		XB = pt.x;
		YB = pt.y;
	}

	// Start point
	AVector GetPointA() { return AVector(XA, YA); }

	// End point
	AVector GetPointB() { return AVector(XB, YB); }

	// Direction of the vector
	AVector Direction() { return AVector(XB - XA, YB - YA); }

	float Magnitude()
	{
		AVector dir = Direction();
		return sqrt(dir.x * dir.x + dir.y * dir.y);
	}

	bool LiesHere(AVector vec)
	{
		float det = (XB - XA) * (vec.y - YA) - (YB - YA) * (vec.x - XA);
		if (det > -1e-8 && det < 1e-8) return true;
		return false;
	}

	float Length()
	{
		return AVector(XA, YA).Distance(AVector(XB, YB));
	}

	// returns:
	//	 1 : same direction
	//  -1 : opposite direction
	//   0 : other else
	int HasSameDirection(ALine otherLine)
	{
		float mag1 = Magnitude();
		float mag2 = otherLine.Magnitude();

		AVector dir1 = Direction();
		AVector dir2 = otherLine.Direction();

		float a_dot_b = dir1.Dot(dir2);
		float a_b_mag = mag1 *  mag2;

		float addValue = a_dot_b + a_b_mag;
		if (addValue > -std::numeric_limits<float>::epsilon() && addValue < std::numeric_limits<float>::epsilon()) { return -1; }

		float subsValue = a_dot_b - a_b_mag;

		if (subsValue > -std::numeric_limits<float>::epsilon() && subsValue < std::numeric_limits<float>::epsilon()) { return 1; }

		return 0;
	}
};

#endif // ALINE_H