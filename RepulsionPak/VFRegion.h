
/* ---------- RepulsionPak  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef AREGION_H
#define AREGION_H

#include "APath.h"
#include "UtilityFunctions.h"
//#include "VecFieldDesigner.h"

#include "ClipperWrapper.h"

#include "AnIdxTriangle.h"
#include "CGALTriangulation.h"

#include "SelfIntersectionFixer.h"

#include <vector>

#include "MyColor.h"

struct VFRegion
{
public:
	std::string _name;

	std::vector<int>  _boundaryFColors; // foreground colors
	std::vector<int>  _boundaryBColors; // background colors

	std::vector<MyColor>  _boundaryFColorsRGB; // foreground colors (RGB mode)
	std::vector<MyColor>  _boundaryBColorsRGB; // background colors (RGB mode)

	// streamlines and skeletons
	std::vector<std::vector<AVector>> _streamLines;
	std::vector<std::vector<AVector>> _skeletonLines;

	// paths
	std::vector<APath> _boundaryPaths;
	std::vector<int>   _boundaryBWFlags;
	std::vector<APath> _focalPaths;
	std::vector<APath> _fieldPaths;
	std::vector<APath> _boundaryMSTPaths; // Physics-Pak

	std::vector<std::vector<AVector>> _holes;

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

	/* for triangulation, see function Triangulate() */
	CGALTriangulation _triangulator;

public:
	// Adobe VectorFieldDesign
	//CORE::Rectf           _dDomainRect; // bounding box

private:
	// Adobe VectorFieldDesign
	//VFD::SmoothField      _vfdField;
	//VFD::Constraints      _vfdConstraints;

public:
	// constructor
	VFRegion();

	void FixSelfIntersection()
	{
		// fix self intersection
		SelfIntersectionFixer  siFixer;

		std::vector<std::vector<AVector>> polyPoundaries = GetBoundaries();
		for (int a = 0; a < polyPoundaries.size(); a++)
		{
			bool isSimple = siFixer.IsSimple(polyPoundaries[a]);
			if (!isSimple)
			{
				//std::cout << "fix self intersection - ";
				std::vector<AVector> newPoly;
				siFixer.FixSelfIntersection1(polyPoundaries[a], newPoly);

				if (newPoly.size() <= 2) { std::cerr << "self intersection NOT FIXED because poly is ZERO\n"; }


				isSimple = siFixer.IsSimple(newPoly);
				//_simpleBoundaries.push_back(newPoly);
				if (isSimple)
				{
					std::cout << "FIXED\n";
				}
				else { std::cerr << "self intersection NOT FIXED\n"; }

				polyPoundaries[a] = newPoly;
			}
		} // iterate polyBoundaries

		SetBoundaries(polyPoundaries);
	}

	AVector GetCenterSkeleton()
	{
		AVector pt1 = _skeletonLines[0][0];
		AVector pt2 = _skeletonLines[0][1];
		return  pt1 + ((pt2 - pt1).Norm() * 0.5f * pt1.Distance(pt2));
	}

	void TranslateBoundaries(AVector transVec)
	{
		for (int a = 0; a < _boundaryPaths.size(); a++)
		{
			_boundaryPaths[a] = _boundaryPaths[a].Translate(transVec);
		}
	}

	void RotateBoundaries(float radAngle)
	{
		for (int a = 0; a < _boundaryPaths.size(); a++)
		{
			_boundaryPaths[a] = _boundaryPaths[a].Rotate(radAngle);
		}
	}

	VFRegion Scale(float scaleFactor)
	{
		/*
		// streamlines and skeletons
		std::vector<std::vector<AVector>> _streamLines;
		std::vector<std::vector<AVector>> _skeletonLines;

		// paths
		std::vector<APath> _boundaryPaths;
		std::vector<APath> _focalPaths;
		std::vector<APath> _fieldPaths;
		*/

		VFRegion scaledRegion;

		std::vector<std::vector<AVector>> streamLines;
		std::vector<std::vector<AVector>> skeletonLines;

		std::vector<APath> boundaryPaths;
		std::vector<APath> focalPaths;
		std::vector<APath> fieldPaths;

		for (int a = 0; a < _streamLines.size(); a++)
		{
			std::vector<AVector> streamline;
			for (int b = 0; b < _streamLines[a].size(); b++)
			{
				streamline.push_back(_streamLines[a][b] * scaleFactor);
			}
			streamLines.push_back(streamline);
		}

		for (int a = 0; a < _skeletonLines.size(); a++)
		{
			std::vector<AVector> skeletonline;
			for (int b = 0; b < _skeletonLines[a].size(); b++)
			{
				skeletonline.push_back(_skeletonLines[a][b] * scaleFactor);
			}
			skeletonLines.push_back(skeletonline);
		}

		for (int a = 0; a < _boundaryPaths.size(); a++)
		{
			boundaryPaths.push_back(_boundaryPaths[a].Scale(scaleFactor));
		}

		for (int a = 0; a < _focalPaths.size(); a++)
		{
			focalPaths.push_back(_focalPaths[a].Scale(scaleFactor));
		}

		for (int a = 0; a < _fieldPaths.size(); a++)
		{
			fieldPaths.push_back(_fieldPaths[a].Scale(scaleFactor));
		}

		scaledRegion._streamLines = streamLines;
		scaledRegion._skeletonLines = skeletonLines;

		scaledRegion._boundaryPaths = boundaryPaths;
		scaledRegion._focalPaths = focalPaths;
		scaledRegion._fieldPaths = fieldPaths;

		return scaledRegion;
	}

public:

	// MST Boundaries
	void SetMSTBoundaries(std::vector<std::vector<AVector>> boundaries)
	{
		this->_boundaryMSTPaths.clear(); // delete old one
		for (int a = 0; a < boundaries.size(); a++)
		{
			APath p;
			p.points = boundaries[a];
			p.isClosed = true;
			this->_boundaryMSTPaths.push_back(p);
		}
	}

	// MST boundaries
	std::vector<std::vector<AVector>> GetMSTBoundaries()
	{
		std::vector<std::vector<AVector>> myMSTBoundaries;
		for (int a = 0; a < this->_boundaryMSTPaths.size(); a++)
		{
			myMSTBoundaries.push_back(this->_boundaryMSTPaths[a].points);
		}
		return myMSTBoundaries;
	}

	// boundaries
	void ResampleBoundaries(int N)
	{
		for (int a = 0; a < _boundaryPaths.size(); a++)
		{
			std::vector<AVector> oriCurve = _boundaryPaths[a].points;
			UtilityFunctions::UniformResampleClosed(oriCurve, this->_boundaryPaths[a].points, N);
		}
	}

	// boundaries
	void MakeBoundariesClockwise()
	{
		//std::vector<std::vector<AVector>> boundaries = GetBoundaries(); // new boundary		
		SetBoundaries(ClipperWrapper::MakeClockwise(GetBoundaries()));
		SetMSTBoundaries(ClipperWrapper::MakeClockwise(GetMSTBoundaries()));
	}

	// boundaries
	std::vector<std::vector<AVector>> GetBoundaries()
	{
		std::vector<std::vector<AVector>> myBoundaries;
		for (int a = 0; a < this->_boundaryPaths.size(); a++)
		{
			myBoundaries.push_back(this->_boundaryPaths[a].points);
		}
		return myBoundaries;
	}

	// boundaries
	void SetBoundaries(std::vector<std::vector<AVector>> boundaries)
	{
		this->_boundaryPaths.clear(); // delete old one
		for (int a = 0; a < boundaries.size(); a++)
		{
			APath p;
			p.points = boundaries[a];
			p.isClosed = true;
			this->_boundaryPaths.push_back(p);
		}
	}

	// boundaries
	std::vector<std::vector<AVector>> GetFocalBoundaries()
	{
		std::vector<std::vector<AVector>> myFocals;
		for (int a = 0; a < this->_focalPaths.size(); a++)
		{
			myFocals.push_back(this->_focalPaths[a].points);
		}
		return myFocals;
	}

	// field
	std::vector<std::vector<AVector>> GetFields()
	{
		std::vector<std::vector<AVector>> myFields;
		for (int a = 0; a < this->_fieldPaths.size(); a++)
		{
			myFields.push_back(this->_fieldPaths[a].points);
		}
		return myFields;
	}

public:
	// ===== Adobe VectorFieldDesign =====
	//void Solve();

	// ===== Adobe VectorFieldDesign =====
	//AVector GetDirectionInterpolation(AVector pos);

private:
	// ===== Adobe VectorFieldDesign =====
	//void  setConstraintPointAngle(int nIndex, float fAngle);
	//float getConstraintPointLength(int nIndex);

	// ===== Adobe VectorFieldDesign =====
	//void  AddConstraintsFromPaths(std::vector<APath> paths);
	//int   addConstraintPoint(VFD::Point2D& position, float fAngle, float fInfluence);
	//int   addConstraintPoint(VFD::Point2D& position, VFD::Point2D& nextVector, float fAngle, float fInfluence);
	//int   addConstraintPoint(VFD::Point2D& position, const VFD::Point2D& p0, const VFD::Point2D& p1, const VFD::Point2D& p2, float fInfluence);

	// ===== Adobe VectorFieldDesign =====
	//VFD::float2 getROSYfromVector(float fNRosy, const VFD::float2& v);
	//void        getDirectionfFromNRosy(const VFD::FieldPoint2D& fieldPoint, int nFieldIndex0, int nFieldIndex1, CORE::float2& dir);


public:
	static std::vector<std::vector<AVector>> ConvertPathsToVectors(std::vector<APath> paths)
	{
		std::vector<std::vector<AVector>> myVectors;
		for (int a = 0; a < paths.size(); a++)
		{
			myVectors.push_back(paths[a].points);
		}
		return myVectors;
	}

	void Triangulate()
	{
		_triPoints.clear();
		_triangles.clear();

		std::vector<std::vector<AVector>> points = GetBoundaries();
		for (int a = 0; a < points.size(); a++)
		{
			std::vector<AVector> verts;
			std::vector<AnIdxTriangle> tris;
			_triangulator.Triangulate(points[a], verts, tris);
			_triPoints.push_back(verts);
			_triangles.push_back(tris);
		}
	}
};

#endif