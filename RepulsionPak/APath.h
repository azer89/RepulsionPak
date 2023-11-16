
/* ---------- ShapeRadiusMatching V2  ---------- */

#ifndef APATH_H
#define APATH_H

#include "AVector.h"

#include <vector>
#include <limits>
#include <cmath>

/**
* Reza Adhitya Saputra
* radhitya@uwaterloo.ca
* March 2016
*/

enum PathType
{
	BOUNDARY_PATH = 0, // boundary of the target regions
	DETAIL_PATH = 1, // salient details of the target regions
	FOCAL_PATH = 2, // focal points of the target region, pretty much similar to DETAIL_PATH
	FIELD_PATH = 3, // paths that influence the vector field
	SEED_PATH = 4, // (not used anymore) if you want to specify the starting lines of the streamlines
	STREAM_LINE = 5, // streamline
	SKELETON_LINE = 6,
	HOLE = 7,
	//BOUNDARY_MST_PATH   = 8,    // for physics-pack
};

struct APath
{
public:
	std::vector<AVector> points;
	bool isClosed;
	PathType pathType;

	// Constructor #1
	APath()
	{
		this->isClosed = false;
	}

	// Constructor #2
	APath(bool isClosed)
	{
		this->isClosed = isClosed;
	}

	// don't delete :(
	void FlipOrder()
	{
		std::reverse(points.begin(), points.end());
	}

	APath Scale(float scaleFactor)
	{
		APath newPath;
		newPath.isClosed = this->isClosed;
		newPath.pathType = pathType;

		for (int a = 0; a < this->points.size(); a++)
		{
			newPath.points.push_back(this->points[a] * scaleFactor);
		}

		return newPath;
	}

	APath Rotate(float radAngle)
	{
		APath newPath;
		newPath.isClosed = this->isClosed;
		newPath.pathType = pathType;

		for (int a = 0; a < this->points.size(); a++)
		{
			AVector pt = this->points[a];
			AVector newPt(pt.x * std::cos(radAngle) - pt.y * std::sin(radAngle),
				pt.x * std::sin(radAngle) + pt.y * std::cos(radAngle));
			newPath.points.push_back(newPt);
		}

		return newPath;
	}

	APath Translate(AVector transVec)
	{
		APath newPath;
		newPath.isClosed = this->isClosed;
		newPath.pathType = pathType;

		for (int a = 0; a < this->points.size(); a++)
		{
			newPath.points.push_back(this->points[a] + transVec);
		}

		return newPath;
	}
};

#endif // APATH_H
