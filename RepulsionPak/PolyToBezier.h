
/* ---------- RepulsionPak  ---------- */

#ifndef __Poly_To_Bezier__
#define __Poly_To_Bezier__

// conversion from polygon to cubic bezier

#include "BezierSmoothing.h"
#include "AQuad.h" // for bezier start/end points and also two control points
#include <vector>


class PolyToBezier
{
public:
	static std::vector<AVector> ConvertPoly(std::vector<AVector> poly)
	{
		float smooth_value = 0.1;
		std::vector<AVector> simplePoly = BezierSmoothing::SmoothingRDP(poly);

		std::vector<AQuad> quads = Convert(simplePoly);
		std::vector<AVector> newPoly;
		for (int a = 0; a < quads.size(); a++)
		{
			if (a == 0) { newPoly.push_back(quads[0].p0); }

			std::vector<AVector> ptpt;
			BezierSmoothing::DeCasteljau(ptpt, quads[a].p0, quads[a].p1, quads[a].p2, quads[a].p3, smooth_value);
			newPoly.insert(newPoly.end(), ptpt.begin() + 1, ptpt.end());
		}

		return newPoly;
	}

	// http_://www.antigrain.com/research/bezier_interpolation/
	static std::vector<AQuad> Convert(std::vector<AVector> poly)
	{
		int sz = poly.size();
		std::vector<AQuad> bezierCurves;
		for (int a = 0; a < poly.size() - 1; a++)
		{
			AVector p0; // start
			AVector p1; // control point 1
			AVector p2; // control point 2
			AVector p3; // end

			AVector pPrev;
			AVector pNext;

			p0 = poly[a];
			p3 = poly[a + 1];

			if (a == 0) { pPrev = poly[sz - 1]; }
			else { pPrev = poly[a - 1]; }

			if (a == sz - 2) { pNext = poly[0]; }
			else { pNext = poly[a + 2]; }

			CurveToBezier(pPrev, p0, p3, pNext, p1, p2);

			AQuad aQuad(p0, p1, p2, p3);

			bezierCurves.push_back(aQuad);
		}
		return bezierCurves;
	}

	static void CurveToBezier(AVector p0, AVector p1, AVector p2, AVector p3, AVector& cp0, AVector& cp1)
	{
		float t_smooth_factor = 0.5;

		double xc1 = (p0.x + p1.x) / 2.0;		double yc1 = (p0.y + p1.y) / 2.0;
		double xc2 = (p1.x + p2.x) / 2.0;		double yc2 = (p1.y + p2.y) / 2.0;
		double xc3 = (p2.x + p3.x) / 2.0;		double yc3 = (p2.y + p3.y) / 2.0;

		double len1 = sqrt((p1.x - p0.x) * (p1.x - p0.x) + (p1.y - p0.y) * (p1.y - p0.y));
		double len2 = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
		double len3 = sqrt((p3.x - p2.x) * (p3.x - p2.x) + (p3.y - p2.y) * (p3.y - p2.y));

		double k1 = len1 / (len1 + len2);		double k2 = len2 / (len2 + len3);

		double xm1 = xc1 + (xc2 - xc1) * k1;	double ym1 = yc1 + (yc2 - yc1) * k1;
		double xm2 = xc2 + (xc3 - xc2) * k2;	double ym2 = yc2 + (yc3 - yc2) * k2;

		// Resulting control points. Here smooth_value is mentioned
		// above coefficient K whose value should be in range [0...1].
		cp0.x = xm1 + (xc2 - xm1) * t_smooth_factor + p1.x - xm1;
		cp0.y = ym1 + (yc2 - ym1) * t_smooth_factor + p1.y - ym1;

		cp1.x = xm2 + (xc2 - xm2) * t_smooth_factor + p2.x - xm2;
		cp1.y = ym2 + (yc2 - ym2) * t_smooth_factor + p2.y - ym2;
	}
};

#endif