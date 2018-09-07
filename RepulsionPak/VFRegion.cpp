
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "VFRegion.h"
#include "UtilityFunctions.h"
#include "SystemParams.h"

#ifndef VFD_NROSY
#define VFD_NROSY 1.0f
#endif


/*
================================================================================
================================================================================
*/
VFRegion::VFRegion()// :
	//_dDomainRect(0.0f, 0.0f, 1.0f, 1.0f) // the bounding box
{
}

/*
================================================================================
================================================================================
*/
/*void VFRegion::AddConstraintsFromPaths(std::vector<APath> paths)
{
	float scalingFactor = SystemParams::_downscaleFactor;

	for (int a = 0; a < paths.size(); a++)
	{
		if (paths[a].pathType == FIELD_PATH)
		{
			std::vector<AVector> points = paths[a].points;
			for (int b = 0; b < points.size(); b++)
			{
				AVector pt1;
				AVector pt2;
				if (b == points.size() - 1)
				{
					pt1 = points[b - 1];
					pt2 = points[b];

					// we want to measure the angle
					AVector vecDir = pt2 - pt1;
					AVector xDir(1, 0);
					float perpDot = xDir.x * vecDir.y - xDir.y * vecDir.x;
					float rot = (float)atan2(perpDot, xDir.Dot(vecDir));

					addConstraintPoint(VFD::float2(pt2.x * scalingFactor, pt2.y * scalingFactor), rot, 1.0f);
				}
				else
				{
					pt1 = points[b];
					pt2 = points[b + 1];

					// we want to measure the angle
					AVector vecDir = pt2 - pt1;
					AVector xDir(1, 0);
					float perpDot = xDir.x * vecDir.y - xDir.y * vecDir.x;
					float rot = (float)atan2(perpDot, xDir.Dot(vecDir));

					addConstraintPoint(VFD::float2(pt1.x * scalingFactor, pt1.y * scalingFactor),
						rot, 1.0f);
				}
			}
		}
	}
}
*/

/*
================================================================================
================================================================================
*/
/*void VFRegion::Solve()
{
	// triangulation
	float fMaxTriangleArea = (15.0 / 1024.0)*(15.0 / 1024.0);
	float fMinTriangleArea = (0.05 / 1024.0)*(0.05 / 1024.0);

	// ===== Add constraints ====
	AddConstraintsFromPaths(this->_fieldPaths);

	/// now build the field
	float fError[4] = { 1.0e-3f, 1.0e-3f, 1.0e-3f, 1.0e-3f };
	_vfdField.build(fMaxTriangleArea, fMinTriangleArea, 4, _dDomainRect, _vfdConstraints, false);
}
*/

/*
================================================================================
================================================================================
*/
//int VFRegion::addConstraintPoint(VFD::Point2D& position, float fAngle, float fInfluence)
//{
//	return addConstraintPoint(position, (position + VFD::float2(0.0f, 0.01f)), fAngle, fInfluence);
//}

/*
================================================================================
================================================================================
*/
/*int VFRegion::addConstraintPoint(VFD::Point2D& position, VFD::Point2D& nextVector, float fAngle, float fInfluence)
{
	VFD::float2 e0(getROSYfromVector(1.0f, nextVector - position));

	VFD::FieldPoint2D p(position.x, position.y);
	p.setValue(0, e0.x);
	p.setValue(1, e0.y);

	p.setInfluence(fInfluence);
	_vfdConstraints.addFieldPoint(p);

	setConstraintPointAngle(_vfdConstraints.getNumFieldPoints() - 1, fAngle);

	return _vfdConstraints.getNumFieldPoints() - 1;
}*/

/*
================================================================================
================================================================================
*/
/*void VFRegion::setConstraintPointAngle(int nIndex, float fAngle)
{
	VFD::FieldPoint2D& p = _vfdConstraints.getFieldPoint(nIndex);
	VFD::float2 repField;
	VFD::SmoothField::tensorFieldToRepresentationField(1, fAngle, getConstraintPointLength(nIndex), repField);
	p.setValue(0, repField.x);
	p.setValue(1, repField.y);
}*/

/*
================================================================================
================================================================================
*/
/*float VFRegion::getConstraintPointLength(int nIndex)
{
	VFD::FieldPoint2D& p = _vfdConstraints.getFieldPoint(nIndex);
	VFD::float2 repField(p.getValue(0), p.getValue(1)), tensor;
	VFD::SmoothField::representationFieldToTensorField(1.0, repField, tensor);
	return tensor.length();
}*/

/*
================================================================================
================================================================================
*/
/*int VFRegion::addConstraintPoint(VFD::Point2D& position, const VFD::Point2D& p0, const VFD::Point2D& p1, const VFD::Point2D& p2, float fInfluence)
{
	VFD::float2 e0(getROSYfromVector(1.0f, p1 - p0));
	VFD::float2 e1(getROSYfromVector(1.0f, p2 - p1));

	VFD::FieldPoint2D p(position.x, position.y);
	p.setValue(0, e0.x);
	p.setValue(1, e0.y);
	p.setValue(2, e1.x);
	p.setValue(3, e1.y);

	p.setInfluence(fInfluence);
	_vfdConstraints.addFieldPoint(p);
	return _vfdConstraints.getNumFieldPoints() - 1;
}*/

/*
================================================================================
================================================================================
*/
/*VFD::float2 VFRegion::getROSYfromVector(float fNRosy, const VFD::float2& v)
{
	VFD::float2 tensor(v);
	float fLengthSqr(tensor.length());
	if (fLengthSqr < 1.0e-12)
		return VFD::float2(0.0f); // singularity
	tensor.normalize();
	if (tensor.y < 0.0f)
		tensor = -tensor;
	float fAngle = CORE::acos(tensor.x);
	fAngle *= fNRosy;
	tensor.x = CORE::cos(fAngle);
	tensor.y = CORE::sin(fAngle);
	tensor *= CORE::sqrt(fLengthSqr);
	return tensor;
}*/

/*
================================================================================
================================================================================
*/
/*void VFRegion::getDirectionfFromNRosy(const VFD::FieldPoint2D& fieldPoint, int nFieldIndex0, int nFieldIndex1, CORE::float2& dir)
{
	VFD::Point2D tensor(fieldPoint.getValue(0), fieldPoint.getValue(1));
	VFD::SmoothField::representationFieldToTensorField((1.0f / VFD_NROSY), tensor, dir);
}*/

/*
================================================================================
This is a very important function
================================================================================
*/
/*AVector VFRegion::GetDirectionInterpolation(AVector pos)
{
	CORE::float2 dir = _vfdField.getFieldValueFloat2(VFD::float2(pos.x, pos.y));
	return AVector(dir.x, dir.y);
}*/
