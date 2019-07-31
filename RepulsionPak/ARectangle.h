
/* ---------- RepulsionPak  ---------- */

#ifndef ARECTANGLE_H
#define ARECTANGLE_H

#include "AVector.h"

/**
* Reza Adhitya Saputra
* radhitya@uwaterloo.ca
* June 2016
*/

struct ARectangle
{
public:
	AVector topleft;
	float witdh;
	float height;

	ARectangle()
	{

	}

	ARectangle(AVector topleft, float witdh, float height)
	{
		this->topleft = topleft;
		this->witdh = witdh;
		this->height = height;
	}

	AVector GetCenter()
	{
		return AVector(topleft.x + witdh / 2.0f, topleft.y + height / 2.0f);
	}
};



#endif 