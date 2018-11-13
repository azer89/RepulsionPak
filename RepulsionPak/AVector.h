
/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef AVECTOR_H
#define AVECTOR_H

#include <limits>
#include <cmath>
#include <iostream> // for abs (?)

#include <vector>

#include "SystemParams.h"

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
struct AVector
{
public:
	// x
	float x;

	// y
	float y;

	float radAngle;

	// what is this?
	float aCertainInfo;

	// Default constructor
	AVector()
	{
		this->x = -1;
		this->y = -1;
		this->radAngle = 0;
	}

	// Constructor
	AVector(float x, float y)
	{
		this->x = x;
		this->y = y;
		this->radAngle = 0;
	}

	// Scale a point
	AVector Resize(float val)
	{
		AVector newP;
		newP.x = this->x * val;
		newP.y = this->y * val;
		return newP;
	}

	void SetInvalid()
	{
		this->x = -1;
		this->y = -1;
	}

	bool IsBad()
	{
		//if (Length() > 1000000) { std::cout << "huh?\n"; return true; }

		return std::isinf(x) || std::isnan(x) || std::isinf(y) || std::isnan(y);
	}

	// if a point is (-1, -1)
	bool IsInvalid()
	{
		if (((int)x) == -1 && ((int)y) == -1)
		{
			return true;
		}
		return false;
	}

	// Normalize
	AVector Norm() // get the unit vector
	{
		float vlength = std::sqrt(x * x + y * y);

		//if (vlength == 0) { std::cout << "div by zero duh\n"; }
		if (vlength == 0) { return AVector(0, 0); }

		return AVector(this->x / vlength, this->y / vlength);
	}

	// Euclidean distance
	float Distance(const AVector& other)
	{
		float xDist = x - other.x;
		float yDist = y - other.y;
		return std::sqrt(xDist * xDist + yDist * yDist);
	}

	// Euclidean distance
	float Distance(float otherX, float otherY)
	{
		float xDist = x - otherX;
		float yDist = y - otherY;
		return std::sqrt(xDist * xDist + yDist * yDist);
	}

	// squared euclidean distance
	float DistanceSquared(const AVector& other)
	{
		float xDist = x - other.x;
		float yDist = y - other.y;
		return (xDist * xDist + yDist * yDist);
	}

	// squared euclidean distance
	float DistanceSquared(float otherX, float otherY)
	{
		float xDist = x - otherX;
		float yDist = y - otherY;
		return (xDist * xDist + yDist * yDist);
	}

	// operator overloading
	AVector operator+ (const AVector& other) { return AVector(x + other.x, y + other.y); }

	// operator overloading
	AVector operator- (const AVector& other) { return AVector(x - other.x, y - other.y); }
	bool operator== (const AVector& other)
	{
		return (abs(this->x - other.x) < std::numeric_limits<float>::epsilon() && abs(this->y - other.y) < std::numeric_limits<float>::epsilon());
	}

	// operator overloading
	bool operator!= (const AVector& other)
	{
		return (abs(this->x - other.x) > std::numeric_limits<float>::epsilon() || abs(this->y - other.y) > std::numeric_limits<float>::epsilon());
	}

	// operator overloading
	AVector operator+= (const AVector& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	// operator overloading
	AVector operator-= (const AVector& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	// operator overloading
	AVector operator/ (const float& val) { return AVector(x / val, y / val); }

	// operator overloading
	AVector operator* (const float& val) { return AVector(x * val, y * val); }

	// operator overloading
	AVector operator*= (const float& val)
	{
		x *= val;
		y *= val;
		return *this;
	}

	// operator overloading
	AVector operator/= (const float& val)
	{
		x /= val;
		y /= val;
		return *this;
	}

	// length of a vector
	float Length() { return sqrt(x * x + y * y); }

	// squared length of a vector
	float LengthSquared() { return x * x + y * y; }

	// dot product
	float Dot(const AVector& otherVector) { return x * otherVector.x + y * otherVector.y; }

	// cross product
	AVector Cross(const AVector& otherVector)
	{
		//u x v = u.x * v.y - u.y * v.x
		return AVector(x * otherVector.y, y * otherVector.x);
	}

	// linear dependency test
	bool IsLinearDependent(const AVector& otherVector)
	{
		float det = (this->x * otherVector.y) - (this->y * otherVector.x);
		if (det > -std::numeric_limits<float>::epsilon() && det < std::numeric_limits<float>::epsilon()) { return true; }
		return false;
	}

	// angle direction
	// not normalized
	AVector DirectionTo(const AVector& otherVector)
	{
		return AVector(otherVector.x - this->x, otherVector.y - this->y);
	}

	bool IsOut()
	{
		return (x < 0 || x > SystemParams::_upscaleFactor || y < 0 || y > SystemParams::_upscaleFactor);
	}

	void Print()
	{
		std::cout << "(" << x << ", " << y << ")\n";
	}
};

typedef std::vector<std::vector<AVector>> GraphArt;

#endif // AVECTOR_H