
# include "BezierSmoothing.h"

#include "UtilityFunctions.h"

#include "CurveFitting.h"
#include "CurveRDP.h"

// TEST

int   rdp_point_min = 3;
float rdp_epsilon = 10; // 2.5
float subdivide_limit = 0.5f;

std::vector<AVector> BezierSmoothing::SmoothingRDP(std::vector<AVector> poly)
{
	std::vector<AVector> newPoly;

	std::vector<bool> pFlag; // not used
	std::vector<AVector> pPolySimple;
	RDPSimplification(poly, pPolySimple, pFlag);

	return pPolySimple;
}

std::vector<AVector> BezierSmoothing::SmoothingRDPB(std::vector<AVector> poly)
{
	std::vector<AVector> poly2;
	UtilityFunctions::UniformResample(poly, poly2, 0.02f);

	std::vector<AVector> newPoly;

	std::vector<bool> pFlag;
	std::vector<AVector> pPolySimple;
	RDPSimplification(poly2, pPolySimple, pFlag);
	std::vector<AQuad> quads = GetBezierQuadLS(poly2, pFlag);

	for (int a = 0; a < quads.size(); a++)
	{
		if (a == 0) { newPoly.push_back(quads[0].p0); }

		std::vector<AVector> ptpt;
		DeCasteljau(ptpt, quads[a].p0, quads[a].p1, quads[a].p2, quads[a].p3, subdivide_limit);
		newPoly.insert(newPoly.end(), ptpt.begin() + 1, ptpt.end());
	}

	return newPoly;
}

void BezierSmoothing::RDPSimplification(std::vector<AVector> poly, std::vector<AVector>& rdpPoly, std::vector<bool>& bFlag)
{
	bFlag = std::vector<bool>(poly.size());
	for (size_t a = 0; a < poly.size(); a++)  { bFlag[a] = false; }
	bFlag[0] = true;
	bFlag[poly.size() - 1] = true;

	CurveRDP::RDPFlags(bFlag, poly, rdp_epsilon, 0, poly.size() - 1, rdp_point_min);
	for (size_t a = 0; a < poly.size(); a++)
	{
		if (bFlag[a]) rdpPoly.push_back(poly[a]);
	}

}


std::vector<AQuad> BezierSmoothing::GetBezierQuadLS(std::vector<AVector> poly, std::vector<bool> rdpFlags)
{
	std::vector<AQuad> quads;

	// Begin to sample points
	std::vector<std::vector<AVector>> rdpPoly;
	rdpPoly.push_back(std::vector<AVector>());

	for (size_t a = 0; a < poly.size(); a++)
	{
		int idx = rdpPoly.size() - 1;
		rdpPoly[idx].push_back(poly[a]);
		if (rdpFlags[a] && a != 0 && a != poly.size() - 1)
		{
			rdpPoly.push_back(std::vector<AVector>());
			rdpPoly[idx + 1].push_back(poly[a]);
		}
	}

	for (size_t a = 0; a < rdpPoly.size(); a++)
	{
		std::vector<AVector> segm = rdpPoly[a];

		// RESUBDIVIDE
		//if (segm.size() < 5) { Resubdivide(segm); }

		std::vector<AVector> fourPoints;
		if (CurveFitting::PointstoBezier(segm, fourPoints))
		{
			quads.push_back(AQuad(segm[0], fourPoints[1], fourPoints[2], segm[segm.size() - 1]));
		}
		else {}
	}
	return quads;
}

void BezierSmoothing::DeCasteljau(std::vector<AVector>& poly, AVector p0, AVector p1, AVector p2, AVector p3, double subdivide_limit)
{
	if (p0.Distance(p3) <= subdivide_limit)
	{
		poly.push_back(p0);
	}
	else
	{
		double splitParam = 0.5;	// split into two equal parts

		double x0 = p0.x; double y0 = p0.y;
		double x1 = p1.x; double y1 = p1.y;
		double x2 = p2.x; double y2 = p2.y;
		double x3 = p3.x; double y3 = p3.y;

		double x01 = (x1 - x0) * splitParam + x0;		double x12 = (x2 - x1) * splitParam + x1;		double x23 = (x3 - x2) * splitParam + x2;
		double y01 = (y1 - y0) * splitParam + y0;		double y12 = (y2 - y1) * splitParam + y1;		double y23 = (y3 - y2) * splitParam + y2;

		double x012 = (x12 - x01) * splitParam + x01;	double x123 = (x23 - x12) * splitParam + x12;
		double y012 = (y12 - y01) * splitParam + y01;	double y123 = (y23 - y12) * splitParam + y12;

		double x0123 = (x123 - x012) * splitParam + x012;
		double y0123 = (y123 - y012) * splitParam + y012;

		DeCasteljau(poly, AVector(x0, y0), AVector(x01, y01), AVector(x012, y012), AVector(x0123, y0123), subdivide_limit);
		DeCasteljau(poly, AVector(x0123, y0123), AVector(x123, y123), AVector(x23, y23), AVector(x3, y3), subdivide_limit);
	}
}