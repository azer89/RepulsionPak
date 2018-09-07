
/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef ART_DATA_H
#define ART_DATA_H

#include "AVector.h"
#include "AnIdxTriangle.h"
#include "CGALTriangulation.h"
#include "UtilityFunctions.h"
#include "SelfIntersectionFixer.h"
#include "CurveRDP.h"
//#include "OpenCVWrapper.h"

// creating hull
#include "SystemParams.h"
//#include "LRFunctions.h"
#include "ClipperWrapper.h"

#include <vector>
#include <limits>

struct ArtData
{
public:
	bool _isFocalElement; // cannot be deformed
	bool _isNegativeElement;

	int _regionNumber;    // reference to a region
	int _blobNumber;	  // reference to a blob
	int _ornamentNumber;  // reference to an ornament

	// non intersecting boundaries are simple boundaries
	// the original _boundaries can have self-intersection
	// Warning: these boundaries may be scaled down !
	std::vector<std::vector<AVector>> _simpleBoundaries; 
	std::vector<std::vector<AVector>> _boundaries;	// boundaries of ornaments
	std::vector<int>  _boundaryBWFlags;

	std::vector<std::vector<AVector>> _vvsPt;		// reference point at the streamline
	std::vector<std::vector<AVector>> _vvDir;		// L/R direction
	std::vector<std::vector<int>>	  _vvIdx;		// L/R index, 0 is left, 1 is right
	std::vector<std::vector<float>>   _vvNormDist;	// normalized location at the streamline 0..1
	std::vector<std::vector<float>>   _vvDist;		// original distance from ornament point to skeleton (unscaled)
	std::vector<std::vector<float>>   _vvNewDist;   // actual distance from ornament point to skeleton

	//LRFunctions _notNormFunctions;

	/* triangulation data 
	   these are vector of vector because they support multiple boundaries

	// example code :
		aRegion.Triangulate();
		std::vector<std::vector<AVector>> triPoints       = aRegion._triPoints;
		std::vector<std::vector<AnIdxTriangle>> triangles = aRegion._triangles;
		for (int a = 0; a < triangles.size(); a++)
		{
			for (int b = 0; b < triangles[a].size(); b++)
			{
				int idx0 = triangles[a][b].idx0; int idx1 = triangles[a][b].idx1; int idx2 = triangles[a][b].idx2;
				AVector pt0 = triPoints[a][idx0]; AVector pt1 = triPoints[a][idx1]; AVector pt2 = triPoints[a][idx2];
				...
			}
		}
	*/
	std::vector<std::vector<AVector>>       _triPoints; // vertices
	std::vector<std::vector<AnIdxTriangle>> _triangles; // indexed triangles
	float _triLeft, _triTop, _triBottom, _triRight;    // bounding box of the triangles

	/* for triangulation, see function Triangulate() */
	CGALTriangulation _triangulator;
	

public:
	// constructor #1
	ArtData()
	{
		this->_isFocalElement = false;
		this->_isNegativeElement = false;
	}

	// constructor #2
	ArtData(std::vector<std::vector<AVector>> boundaries, int regionNumber, bool isFocalElement)
	{
		this->_boundaries        = boundaries;
		this->_regionNumber      = regionNumber;
		this->_isFocalElement    = isFocalElement;

		this->_isNegativeElement = false;
	}

	/*float GetArea()
	{
		OpenCVWrapper cvWrapper;
		float oArea = 0;
		for (int a = 0; a < _boundaries.size(); a++)
		{
			oArea += cvWrapper.GetArea(_boundaries[a]);
		}
		return oArea;
	}*/

	// gap value is that you allow gap to other elements
	void CalculateNotNormFunctions(std::vector<AVector> streamline)
	{
		// resample the streamline so that # points == function length
		/*std::vector<AVector> resampledAxis;
		UtilityFunctions::UniformResample(streamline, resampledAxis, SystemParams::_function_length);

		// calculate left and right vectors
		std::vector<AVector> lDirs; std::vector<AVector> rDirs; // lists
		UtilityFunctions::GetListsOfLeftRightNormals(resampledAxis, lDirs, rDirs); // calculate

		for (int a = 0; a < resampledAxis.size(); a++)
		{
			// extended left right vectors
			AVector lPt = resampledAxis[a] + lDirs[a] * SystemParams::_max_lr_ray_length * 1.5; // hack
			AVector rPt = resampledAxis[a] + rDirs[a] * SystemParams::_max_lr_ray_length * 1.5; // hack

			// intersection points
			//AVector l1 = ClipperWrapper::FindTheClosestIntersection(ALine(resampledAxis[a], lPt), _boundaries);
			//AVector r1 = ClipperWrapper::FindTheClosestIntersection(ALine(resampledAxis[a], rPt), _boundaries);
			AVector l1 = ClipperWrapper::FindTheFarthestIntersection(ALine(resampledAxis[a], lPt), _boundaries);  // should be the farthest
			AVector r1 = ClipperWrapper::FindTheFarthestIntersection(ALine(resampledAxis[a], rPt), _boundaries);  // should be the farthest

			if (!l1.IsInvalid())  {  _notNormFunctions._leftFunction.push_back(resampledAxis[a].Distance(l1)); }
			else { _notNormFunctions._leftFunction.push_back(0); }
			if (!r1.IsInvalid()) { _notNormFunctions._rightFunction.push_back(resampledAxis[a].Distance(r1)); }
			else { _notNormFunctions._rightFunction.push_back(0); }
		}*/
	}

	void Triangulate()
	{
		// clean data
		_triPoints.clear(); 
		_triangles.clear();

		for (unsigned int a = 0; a < _simpleBoundaries.size(); a++)
		{
			std::vector<AVector> verts;      // output
			std::vector<AnIdxTriangle> tris; // output
			_triangulator.Triangulate(_simpleBoundaries[a], verts, tris);
			_triPoints.push_back(verts);
			_triangles.push_back(tris);
		}

		// there's a bug that this doesnot produce any triangle, focal ornaments maybe?
		//std::cout << "v _triangles size : " << _triangles.size() << "; ";
		//std::cout << "v _triPoints size : " << _triPoints.size() << "\n";

		TriangleBoundingBox();
	}


	// calculate the boundingbox of the triangles
	void TriangleBoundingBox()
	{
		_triLeft   = std::numeric_limits<float>::max();
		_triTop    = std::numeric_limits<float>::max();
		_triBottom = std::numeric_limits<float>::min();
		_triRight  = std::numeric_limits<float>::min();

		for (int a = 0; a < _triPoints.size(); a++)
		{
			for (int b = 0; b < _triPoints[a].size(); b++)
			{
				AVector pt = _triPoints[a][b];
				if (pt.x < _triLeft)   { _triLeft  = pt.x; }
				if (pt.y < _triTop)    { _triTop   = pt.y; }
				if (pt.y > _triBottom) { _triBottom = pt.y; }
				if (pt.x > _triRight)  { _triRight  = pt.x; }
			}
		}
	}

	void FixSelfIntersection()
	{
		// do DRP algorithm first so that the output is guaranteed non self-intersecting
		std::vector<std::vector<AVector>> rdpBoundaries;
		for (unsigned int a = 0; a < _boundaries.size(); a++)
		{
			std::vector<AVector> rdpBoundary;
			CurveRDP::SimplifyRDP(_boundaries[a], rdpBoundary, 0.1); // epsilon param

			if (rdpBoundary.size() > 2) // bug, polygon reduced to nothingness
			{
				rdpBoundaries.push_back(rdpBoundary);
			}
		}

		// fix self intersection
		SelfIntersectionFixer  siFixer;
		for (unsigned int a = 0; a < rdpBoundaries.size(); a++)
		{
			bool isSimple = siFixer.IsSimple(rdpBoundaries[a]);
			if (!isSimple)
			{
				//std::cout << "fix self intersection - ";
				std::vector<AVector> newPoly;
				siFixer.FixSelfIntersection1(rdpBoundaries[a], newPoly);

				if (newPoly.size() <= 2)
				{
					continue;
					std::cerr << "self intersection NOT FIXED because poly is ZERO\n";
				}


				bool isSimple = siFixer.IsSimple(newPoly);
				_simpleBoundaries.push_back(newPoly);
				if (isSimple)
				{
					//std::cout << "FIXED\n";
				}
				else { std::cerr << "self intersection NOT FIXED\n"; }
			}
			else
			{
				_simpleBoundaries.push_back(rdpBoundaries[a]);
			}
		}

	} // FixSelfIntersection

	// scaling _simpleBoundaries 
	void ScaleSimpleBoundaries(float scaleFactor)
	{
		for (unsigned int a = 0; a < _simpleBoundaries.size(); a++)
		{
			for (unsigned int b = 0; b < _simpleBoundaries[a].size(); b++)
				{ _simpleBoundaries[a][b] *= scaleFactor; }
		}
	}
};

#endif