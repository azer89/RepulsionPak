
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "ClipperWrapper.h"
#include "OpenCVWrapper.h"

#include "ClipperWrapper.h"
#include "AGraph.h"

#include "clipper.hpp"

/*
================================================================================
================================================================================
*/
ClipperWrapper::ClipperWrapper()
{
}

/*
================================================================================
================================================================================
*/
ClipperWrapper::~ClipperWrapper()
{
}

/*
================================================================================
generate negative shapes using clipper
the way it works, a XOR operation is performed, then followed by intersection operation
================================================================================
*/
std::vector<std::vector<AVector>> ClipperWrapper::XORIntersectionOperations(std::vector<AVector> targetShape, std::vector<std::vector<AVector>> clippingShapes)
{
	float cScaling = 1e10;

	ClipperLib::Path cTargetShape;
	ClipperLib::Paths cClippingShapes(clippingShapes.size());
	ClipperLib::PolyTree sol1;

	// the clipped shape
	for (int a = 0; a < targetShape.size(); a++)
		{ cTargetShape << ClipperLib::IntPoint(targetShape[a].x * cScaling, targetShape[a].y * cScaling); }

	// shapes that clip another shape
	for (int a = 0; a < clippingShapes.size(); a++)
	{
		for (int b = 0; b < clippingShapes[a].size(); b++)
		{ cClippingShapes[a] << ClipperLib::IntPoint(clippingShapes[a][b].x * cScaling, clippingShapes[a][b].y * cScaling); }
	}
	
	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath( cTargetShape,    ClipperLib::ptClip,    true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape

	// XOR 
	myClipper1.Execute( ClipperLib::ctXor, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	ClipperLib::PolyTree sol2;
	ClipperLib::Clipper myClipper2;
	myClipper2.AddPath(cTargetShape, ClipperLib::ptClip, true);
	myClipper2.AddPaths(pSol1, ClipperLib::ptSubject, true);

	// Intersection
	myClipper2.Execute(ClipperLib::ctIntersection, sol2, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol2;
	ClipperLib::PolyTreeToPaths(sol2, pSol2);

	std::vector<std::vector<AVector>> outPolys; // return list

	for (int a = 0; a < pSol2.size(); a++)
	{
		std::vector<AVector> poly;
		for (int b = 0; b < pSol2[a].size(); b++)
		{
			AVector iPt(pSol2[a][b].X / cScaling, pSol2[a][b].Y / cScaling); // scaling down
			poly.push_back(iPt);
		}
		outPolys.push_back(poly);
	}
	std::cout << "polys.size " << outPolys.size() << "\n";
	return outPolys;
}

std::vector<AVector> ClipperWrapper::UnionOperation(std::vector<AVector> poly1, std::vector<AVector> poly2)
{
	float cScaling = 1e10;

	ClipperLib::Path cTargetShape;
	ClipperLib::Path cClippingShape;
	ClipperLib::PolyTree sol1;

	// the clipped shape
	for (int a = 0; a < poly1.size(); a++)
	{
		cTargetShape << ClipperLib::IntPoint(poly1[a].x * cScaling,
			                                 poly1[a].y * cScaling);
	}

	// shape that clips another shape
	for (int a = 0; a < poly2.size(); a++)
	{
		cClippingShape << ClipperLib::IntPoint(poly2[a].x * cScaling,
			                                   poly2[a].y * cScaling);
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape,   ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPath(cClippingShape, ClipperLib::ptSubject, true); // shape that clip another shape

	// Union 
	myClipper1.Execute(ClipperLib::ctUnion, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	std::vector<std::vector<AVector>> outPolys; // return list	

	for (int a = 0; a < pSol1.size(); a++)
	{
		std::vector<AVector> poly;
		for (int b = 0; b < pSol1[a].size(); b++)
		{
			AVector iPt(pSol1[a][b].X / cScaling, pSol1[a][b].Y / cScaling); // scaling down
			poly.push_back(iPt);
		}
		outPolys.push_back(poly);
	}

	std::cout << "outPolys.size() " << outPolys.size() << "\n";
	if (outPolys.size() == 1) { return outPolys[0]; }

	return GetLargestPoly(outPolys);
}

std::vector<AVector> ClipperWrapper::UnionOperation(std::vector<std::vector<AVector>> polys)
{
	if (polys.size() == 1) { return polys[0]; }

	float cScaling = 1e10;

	ClipperLib::Path cTargetShape;
	ClipperLib::Paths cClippingShapes(polys.size() - 1);
	ClipperLib::PolyTree sol1;

	// the clipped shape
	for (int a = 0; a < polys[0].size(); a++)
		{ cTargetShape << ClipperLib::IntPoint(polys[0][a].x * cScaling, 
		                                       polys[0][a].y * cScaling); }

	// shapes that clip another shape
	for (int a = 1; a < polys.size(); a++)
	{
		for (int b = 0; b < polys[a].size(); b++)
		{ 
			int x = polys[a][b].x * cScaling;
			int y = polys[a][b].y * cScaling;
			int idx = a - 1; // because total shape minus one
			cClippingShapes[idx] << ClipperLib::IntPoint(x, y);
		}
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape, ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape

	// Union 
	myClipper1.Execute(ClipperLib::ctUnion, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	std::vector<std::vector<AVector>> outPolys; // return list	

	for (int a = 0; a < pSol1.size(); a++)
	{
		std::vector<AVector> poly;
		for (int b = 0; b < pSol1[a].size(); b++)
		{
			AVector iPt(pSol1[a][b].X / cScaling, pSol1[a][b].Y / cScaling); // scaling down
			poly.push_back(iPt);
		}
		outPolys.push_back(poly);
	}

	std::cout << "outPolys.size() " << outPolys.size() << "\n";
	if (outPolys.size() == 1) { return outPolys[0]; }

	return GetLargestPoly(outPolys);
}

/*
================================================================================
================================================================================
*/
std::vector<AVector> ClipperWrapper::UnionOperationWithGrowing(std::vector<std::vector<AVector>>& polys)
{
	for (int a = 0; a < polys.size(); a++)
	{ polys[a] = GetLargestPoly( MiterOffsettingP(polys[a], 2, 2) ); }

	std::vector<AVector> P1;
	for (int a = 0; a < polys[0].size(); a++)
		{ P1.push_back(AVector(polys[0][a].x, polys[0][a].y)); }

	//for (int a = 1; a < 4; a++)
	for (int a = 1; a < polys.size(); a++)
	{
		std::vector<AVector> P2 = polys[a];
		std::vector<AVector> unionPoly = UnionOperation(P1, P2);
		P1 = unionPoly;
	}
	return P1;
	//return UnionOperation(polys);
}

/*
================================================================================
================================================================================
*/
AVector ClipperWrapper::FindTheFarthestIntersection(ALine line, std::vector<AVector> shape/*, bool reverseShape*/)
{
	AVector endPt = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	AVector intersectPt;

	float cScaling = 1e10; // because clipper only uses integer

	ClipperLib::Path cShape;
	ClipperLib::Path cLine;
	ClipperLib::PolyTree sol;

	// find the effect of changing orientation with the intersections obtained
	//if (reverseShape) { std::reverse(shape.begin(), shape.end()); }

	for (int a = 0; a < shape.size(); a++)
		{ cShape << ClipperLib::IntPoint(shape[a].x * cScaling, shape[a].y * cScaling); }

	cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPath(cShape, ClipperLib::ptClip, true);    // closed shape should be the clipper
	myClipper.AddPath(cLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	//if (pSol.size() > 1)
	//std::cout << "============== pSol: " << pSol.size() << " ==============\n";

	float dist = std::numeric_limits<float>::max();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			float d = endPt.Distance(iPt);
			if (d < dist)
			{
				dist = d;
				intersectPt = iPt;
			}
		}
	}
	return intersectPt;
}


/*
================================================================================
================================================================================
*/
AVector ClipperWrapper::FindTheFarthestIntersection(ALine line, std::vector<std::vector<AVector>> boundaries)
{
	int boundaryNumber = boundaries.size();
	ClipperLib::Paths cShapes(boundaryNumber);
	ClipperLib::Path cLine;
	ClipperLib::PolyTree sol;

	AVector endPt = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	//AVector startPt = line.GetPointA();
	AVector intersectPt;
	float cScaling = 1e15; // because clipper only uses integer

	for (int a = 0; a < boundaryNumber; a++)
	{
		std::vector<AVector> shape = boundaries[a];
		for (int i = 0; i < shape.size(); i++)
			{ cShapes[a] << ClipperLib::IntPoint(shape[i].x * cScaling, shape[i].y * cScaling); }
	}

	cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPaths(cShapes, ClipperLib::ptClip, true);    // closed shape should be the clipper
	myClipper.AddPath(cLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	float dist = std::numeric_limits<float>::max();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			float d = endPt.Distance(iPt);
			if (d < dist)
			{
				dist = d;
				intersectPt = iPt;
			}
		}
	}

	return intersectPt;

}

/*
================================================================================
this function is unstable
================================================================================
*/
AVector ClipperWrapper::FindTheClosestIntersection(ALine line, std::vector<std::vector<AVector>> shapes)
{
	AVector endPt = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	AVector startPt = line.GetPointA();
	AVector intersectPt;

	float cScaling = 1e15; // because clipper only uses integer

	ClipperLib::Paths cShapes(shapes.size());
	ClipperLib::Path cLine;
	ClipperLib::PolyTree sol;

	for (int a = 0; a < shapes.size(); a++)
	{
		for (int i = 0; i < shapes[a].size(); i++)
		{ cShapes[a] << ClipperLib::IntPoint(shapes[a][i].x * cScaling, shapes[a][i].y * cScaling); }
	}

	cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPaths(cShapes, ClipperLib::ptClip, true);  // closed shapes should be the clipper
	myClipper.AddPath(cLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	float dist = std::numeric_limits<float>::min();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			float d1 = endPt.Distance(iPt);
			float d2 = startPt.Distance(iPt);

			if (d1 > dist && d2 > 1e-3)	// hardcoded, need to be fixed
			{
				dist = d1;
				intersectPt = iPt;
			}
		}
	}

	return intersectPt;
}

AVector ClipperWrapper::ClosestIntersectionWithOpenPoly(std::vector<AVector> poly, std::vector<std::vector<AVector>> shapes)
{
	//AVector endPt = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	//AVector startPt = line.GetPointA();


	AVector intersectPt;

	/*
	float cScaling = 1e15; // because clipper only uses integer

	ClipperLib::Paths cShapes(shapes.size());
	ClipperLib::Path cPolyLine;
	ClipperLib::PolyTree sol;

	for (int a = 0; a < shapes.size(); a++)
	{
		for (int i = 0; i < shapes[a].size(); i++)
		{ cShapes[a] << ClipperLib::IntPoint(shapes[a][i].x * cScaling, shapes[a][i].y * cScaling); }
	}

	//cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPaths(cShapes, ClipperLib::ptClip, true);  // closed shapes should be the clipper
	myClipper.AddPath(cPolyLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	float dist = std::numeric_limits<float>::min();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			// this is wrong...
			float d1 = endPt.Distance(iPt); // end
			float d2 = startPt.Distance(iPt); // start

			if (d1 > dist && d2 > 1.5f)	// hardcoded, need to be fixed
			{
				dist = d1;
				intersectPt = iPt;
			}
		}
	}
	*/

	return intersectPt;
}

AVector ClipperWrapper::FindTheClosestIntersection2(ALine line, std::vector<AVector> shape)
{
	SelfIntersectionFixer  siFixer;
	std::vector<AVector> newPoly;
	siFixer.FixSelfIntersection1(shape, newPoly);

	std::vector<std::vector<AVector>> offsetPolys = BlobBoundaryOffsetting(newPoly, 1);
	if (offsetPolys.size() != 2)
	{
		std::cerr << "FindTheClosestIntersection2: don't get two offset polys\n";
	}

	std::vector<AVector> bigPoly    = offsetPolys[0];
	std::vector<AVector> smallPoly  = offsetPolys[1];

	OpenCVWrapper cvWrapper;
	if (cvWrapper.GetArea(bigPoly) < cvWrapper.GetArea(smallPoly))
	{
		bigPoly   = offsetPolys[1];
		smallPoly = offsetPolys[0];
	}

	AVector endPt = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	AVector startPt = line.GetPointA();
	AVector intersectPt;

	float cScaling = 1e15; // because clipper only uses integer

	ClipperLib::Path cShape;
	ClipperLib::Path cLine;
	ClipperLib::PolyTree sol;

	for (int i = 0; i < shape.size(); i++)
	{
		cShape << ClipperLib::IntPoint(shape[i].x * cScaling, shape[i].y * cScaling);
	}

	cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPath(cShape, ClipperLib::ptClip, true);    // closed shape should be the clipper
	myClipper.AddPath(cLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	float dist = std::numeric_limits<float>::min();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			float d1 = endPt.Distance(iPt);
			//float d2 = startPt.Distance(iPt);

			if (d1 > dist && UtilityFunctions::InsidePolygon(bigPoly, iPt.x, iPt.y) && !UtilityFunctions::InsidePolygon(smallPoly, iPt.x, iPt.y))	// hardcoded, need to be fixed
			{
				dist = d1;
				intersectPt = iPt;
			}
		}
	}

	return intersectPt;
}

/*
================================================================================
================================================================================
*/
std::vector<std::vector<AVector>> ClipperWrapper::BlobBoundaryOffsetting(std::vector<AVector> blobBoundary, float offsetVal)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.ArcTolerance = 0.25f * cScaling;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;
	for (int i = 0; i < blobBoundary.size(); i++)
		{ subj << ClipperLib::IntPoint(blobBoundary[i].x * cScaling, blobBoundary[i].y * cScaling); }

	cOffset.AddPath(subj, ClipperLib::jtRound, ClipperLib::etClosedLine);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}
		offPolys.push_back(offPoly);
	}

	return offPolys;
}

/*
================================================================================
this function is unstable
================================================================================
*/
AVector ClipperWrapper::FindTheClosestIntersection(ALine line, std::vector<AVector> shape) // from start
{
	AVector endPt   = line.GetPointB(); // the endpoint, because clipper thinks a closed path is also a *filled* path
	AVector startPt = line.GetPointA();
	AVector intersectPt;

	float cScaling = 1e15; // because clipper only uses integer

	ClipperLib::Path cShape;
	ClipperLib::Path cLine;
	ClipperLib::PolyTree sol;

	for (int i = 0; i < shape.size(); i++)
		{ cShape << ClipperLib::IntPoint(shape[i].x * cScaling, shape[i].y * cScaling); }

	cLine << ClipperLib::IntPoint(line.XA * cScaling, line.YA * cScaling) << ClipperLib::IntPoint(line.XB * cScaling, line.YB * cScaling);

	ClipperLib::Clipper myClipper;
	myClipper.AddPath(cShape, ClipperLib::ptClip, true);    // closed shape should be the clipper
	myClipper.AddPath(cLine, ClipperLib::ptSubject, false); // line must be subject
	myClipper.Execute(ClipperLib::ctIntersection, sol, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

	ClipperLib::Paths pSol;
	ClipperLib::PolyTreeToPaths(sol, pSol);

	//float eps = std::numeric_limits<float>::epsilon();

	float dist = std::numeric_limits<float>::min();
	for (int a = 0; a < pSol.size(); a++)
	{
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;

			float d1 = endPt.Distance(iPt);
			float d2 = startPt.Distance(iPt);
			//float d3 = UtilityFunctions::DistanceToClosedCurve(shape, iPt);

			/*if (d2 < 1.5 && d2 > 0)
			{
				std::cout << "wewewe " << d2 << "\n";
			}*/

			// last time I checked d2 is usually xxx e-5
			if (d1 > dist && d2 > 1e-3)	// hardcoded, need to be fixed
			//if (d1 > dist && d3 < 1.0)
			{
				dist = d1;
				intersectPt = iPt;


			}
		}
	}

	return intersectPt;
}

// 
std::vector<std::vector<AVector>> ClipperWrapper::GetUniPolys(std::vector<std::vector<AVector >> polygons)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.ArcTolerance = 0.25 * cScaling;

	ClipperLib::Paths subjs(polygons.size());
	ClipperLib::Paths pSol;
	for (int i = 0; i < polygons.size(); i++)
	{
		for (int a = 0; a < polygons[i].size(); a++)
		{
			subjs[i] << ClipperLib::IntPoint(polygons[i][a].x * cScaling, polygons[i][a].y * cScaling);
		}
	}

	cOffset.AddPaths(subjs, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
	cOffset.Execute(pSol, 0);

	std::vector<std::vector<AVector>>  offPolys;

	std::vector<AVector> largestPoly;
	float largestArea = -1000;
	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}

		offPolys.push_back(offPoly);

	}
	//offPolys.push_back(largestPoly);

	return offPolys;
}

// ROUND
std::vector<std::vector<AVector>> ClipperWrapper::RoundOffsettingPP(std::vector<std::vector<AVector >> polygons, 
	                                                                float offsetVal)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.ArcTolerance = 0.25 * cScaling;

	ClipperLib::Paths subjs(polygons.size());
	ClipperLib::Paths pSol;
	for (int i = 0; i < polygons.size(); i++)
	{
		for (int a = 0; a < polygons[i].size(); a++)
			{ subjs[i] << ClipperLib::IntPoint(polygons[i][a].x * cScaling, polygons[i][a].y * cScaling); }
	}

	cOffset.AddPaths(subjs, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	std::vector<AVector> largestPoly;
	float largestArea = -1000;
	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}

		OpenCVWrapper cvWrapper;
		float pArea = cvWrapper.GetArea(offPoly); // check area size
		if (pArea > largestArea)
		{
			largestPoly = offPoly;
			largestArea = pArea;
		}
	}
	offPolys.push_back(largestPoly);

	return offPolys;
}

// to do: dublicate code
float ClipperWrapper::CalculateFill(const std::vector<AVector>& container, const std::vector<std::vector<AVector >>& graphs)
{
	float cScaling = 1e10;

	ClipperLib::Path cTargetShape;
	ClipperLib::Paths cClippingShapes(graphs.size());
	ClipperLib::PolyTree sol1;

	// the clipped shape
	for (int a = 0; a < container.size(); a++)
	{
		cTargetShape << ClipperLib::IntPoint(container[a].x * cScaling, container[a].y * cScaling);
	}


	// shapes that clip another shape
	for (int a = 0; a < graphs.size(); a++)
	{
		for (int b = 0; b < graphs[a].size(); b++)
		{
			cClippingShapes[a] << ClipperLib::IntPoint(graphs[a][b].x * cScaling, graphs[a][b].y * cScaling);
		}
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape, ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape


	myClipper1.Execute(ClipperLib::ctIntersection, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);


	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	float containerArea = ClipperLib::Area(cTargetShape);
	float elementArea = 0;


	for (int a = 0; a < pSol1.size(); a++)
	{
		elementArea += ClipperLib::Area(pSol1[a]);
	}

	return elementArea / containerArea;
}

void ClipperWrapper::CalculateSCP(const std::vector<AVector>& container, const std::vector<AGraph>& graphs)
{
	float cScaling = 1e10;

	ClipperLib::Path cTargetShape;
	ClipperLib::Paths cClippingShapes(graphs.size());
	ClipperLib::PolyTree sol1;

	// the clipped shape
	for (int a = 0; a < container.size(); a++)
	{
		cTargetShape << ClipperLib::IntPoint(container[a].x * cScaling, container[a].y * cScaling);
	}


	// shapes that clip another shape
	for (int a = 0; a < graphs.size(); a++)
	{
		for (int b = 0; b < graphs[a]._uniArt.size(); b++)
		{
			cClippingShapes[a] << ClipperLib::IntPoint(graphs[a]._uniArt[b].x * cScaling, graphs[a]._uniArt[b].y * cScaling);
		}
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape, ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape


	myClipper1.Execute(ClipperLib::ctIntersection, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);


	// this is the negative space
	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);
}

float ClipperWrapper::CalculateFill(const std::vector<AVector>& container, const std::vector<AGraph>& graphs)
{
	 float cScaling = 1e10;

	 // calculate how many arts
	 int numArt = 0;
	 for (int a = 0; a < graphs.size(); a++)
	 {
		 numArt += graphs[a]._arts.size();
	 }

	 ClipperLib::Path cTargetShape;
	 ClipperLib::Paths cClippingShapes(numArt);
	 ClipperLib::PolyTree sol1;

	 // the clipped shape
	for (int a = 0; a < container.size(); a++)
		{ cTargetShape << ClipperLib::IntPoint(container[a].x * cScaling, container[a].y * cScaling); }


	// shapes that clip another shape
	int artIter = 0;
	for (int a = 0; a < graphs.size(); a++)
	{
		//for (int b = 0; b < graphs[a]._uniArt.size(); b++)
		//{ cClippingShapes[a] << ClipperLib::IntPoint(graphs[a]._uniArt[b].x * cScaling, graphs[a]._uniArt[b].y * cScaling); }

		for (int b = 0; b < graphs[a]._arts.size(); b++)
		{
			for (int c = 0; c < graphs[a]._arts[b].size(); c++)
			{
				cClippingShapes[artIter] << ClipperLib::IntPoint(graphs[a]._arts[b][c].x * cScaling, graphs[a]._arts[b][c].y * cScaling);
			}
			artIter++;
		}
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape, ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape


	myClipper1.Execute(ClipperLib::ctIntersection, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);


	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	float containerArea = ClipperLib::Area(cTargetShape);
	float elementArea = 0;


	for (int a = 0; a < pSol1.size(); a++)
	{
		elementArea += ClipperLib::Area(pSol1[a]);		
	}

	return elementArea / containerArea;
}



void ClipperWrapper::ClippingContainer(const std::vector<AVector>& container, 
	                                   const std::vector<std::vector<AVector >>& skins,
	                                   std::vector<std::vector<AVector>>& outPolys,
								       std::vector<bool>& orientationFlags)
{
	 float cScaling = 1e10;

	 ClipperLib::Path cTargetShape;
	 ClipperLib::Paths cClippingShapes(skins.size());
	 ClipperLib::PolyTree sol1;

	 // the clipped shape
	for (int a = 0; a < container.size(); a++)
		{ cTargetShape << ClipperLib::IntPoint(container[a].x * cScaling, container[a].y * cScaling); }


	// shapes that clip another shape
	for (int a = 0; a < skins.size(); a++)
	{
		for (int b = 0; b < skins[a].size(); b++)
		{ cClippingShapes[a] << ClipperLib::IntPoint(skins[a][b].x * cScaling, skins[a][b].y * cScaling); }
	}

	ClipperLib::Clipper myClipper1;
	myClipper1.AddPath(cTargetShape, ClipperLib::ptClip, true); // the clipped shape
	myClipper1.AddPaths(cClippingShapes, ClipperLib::ptSubject, true); // shapes that clip another shape


	myClipper1.Execute(ClipperLib::ctIntersection, sol1, ClipperLib::pftNonZero, ClipperLib::pftNonZero);


	ClipperLib::Paths pSol1;
	ClipperLib::PolyTreeToPaths(sol1, pSol1);

	//std::vector<std::vector<AVector>> outPolys; // return list

	for (int a = 0; a < pSol1.size(); a++)
	{
		std::vector<AVector> poly;
		for (int b = 0; b < pSol1[a].size(); b++)
		{
			AVector iPt(pSol1[a][b].X / cScaling, pSol1[a][b].Y / cScaling); // scaling down
			poly.push_back(iPt);
		}
		outPolys.push_back(poly);

		if (ClipperLib::Area(pSol1[a]) >= 0)
		{
			orientationFlags.push_back(true); // clockwise
		}
		else
		{
			orientationFlags.push_back(false); // not
		}
	}
	std::cout << "polys.size " << outPolys.size() << "\n";
	//return outPolys;
 }

// ROUND
std::vector<std::vector<AVector>>  ClipperWrapper::RoundOffsettingP(std::vector<AVector> polygon, 
	                                                                float offsetVal)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.ArcTolerance = 0.25f * cScaling;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;
	for (int i = 0; i < polygon.size(); i++)
		{ subj << ClipperLib::IntPoint(polygon[i].x * cScaling, polygon[i].y * cScaling); }

	cOffset.AddPath(subj, ClipperLib::jtRound, ClipperLib::etClosedPolygon);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}
		offPolys.push_back(offPoly);
	}

	return offPolys;
}

// MITER
std::vector<std::vector<AVector>> ClipperWrapper::MiterOffsettingPP(std::vector<std::vector<AVector >> polygons, 
	                                                           float offsetVal, 
															   float miterLimit)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.MiterLimit = miterLimit;

	ClipperLib::Paths subjs(polygons.size());
	ClipperLib::Paths pSol;
	for (int i = 0; i < polygons.size(); i++)
	{
		for (int a = 0; a < polygons[i].size(); a++)
			{ subjs[i] << ClipperLib::IntPoint(polygons[i][a].x * cScaling, polygons[i][a].y * cScaling); }
	}

	cOffset.AddPaths(subjs, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	std::vector<AVector> largestPoly;
	float largestArea = -1000;
	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}

		OpenCVWrapper cvWrapper;
		float pArea = cvWrapper.GetArea(offPoly); // check area size
		if (pArea > largestArea)
		{
			largestPoly = offPoly;
			largestArea = pArea;
		}
	}
	offPolys.push_back(largestPoly);

	return offPolys;
}

// MITER
std::vector<std::vector<AVector>>  ClipperWrapper::MiterOffsettingP(std::vector<AVector> polygon, 
	                                                                float offsetVal, 
															        float miterLimit)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.MiterLimit = miterLimit;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;
	for (int i = 0; i < polygon.size(); i++)
		{ subj << ClipperLib::IntPoint(polygon[i].x * cScaling, polygon[i].y * cScaling); }

	cOffset.AddPath(subj, ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}
		offPolys.push_back(offPoly);
	}

	return offPolys;
}

std::vector<std::vector<AVector>> ClipperWrapper::MiterPNOffsettingP(std::vector<AVector> polygon, 
	                                                                 float offsetVal, 
																	 float miterLimit)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.MiterLimit = miterLimit;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;

	std::vector<AVector> newPolygon = GetLargestPoly(MiterOffsettingP(polygon, offsetVal * 3.0, miterLimit * 2.0));

	return MiterOffsettingP(newPolygon, -offsetVal * 2.0, miterLimit);
}

std::vector<std::vector<AVector>> ClipperWrapper::MiterPNOffsettingPP(std::vector<std::vector<AVector >> polygons, 
	                                                                  float offsetVal, 
																	  float miterLimit)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;
	cOffset.MiterLimit = miterLimit;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;

	std::vector<std::vector<AVector>> newPolygons = MiterOffsettingPP(polygons, offsetVal * 3.0, miterLimit * 2.0);

	// return MiterOffsettingP(  UnionOp(newPolygons), -offsetVal, miterLimit);
	return MiterOffsettingPP(newPolygons, -offsetVal * 2.0, miterLimit);
}

std::vector<AVector> ClipperWrapper::GetLargestPoly(std::vector<std::vector<AVector>> polys)
{
	float biggestArea = std::numeric_limits<float>::min();
	std::vector<AVector> biggestPoly;
	OpenCVWrapper cvWrapper;
	for (int a = 0; a < polys.size(); a++)
	{
		float pArea = cvWrapper.GetArea(polys[a]);
		if (pArea > biggestArea)
		{
			biggestArea = pArea;
			biggestPoly = polys[a];
		}

	}
	return biggestPoly;
}

std::vector<std::vector<AVector>> ClipperWrapper::OffsettingL(std::vector<AVector> someLine, float offsetVal)
{
	float cScaling = 1e10;
	ClipperLib::ClipperOffset cOffset;

	//cOffset.MiterLimit = miterLimit * cScaling;

	ClipperLib::Path subj;
	ClipperLib::Paths pSol;
	for (int i = 0; i < someLine.size(); i++)
		{ subj << ClipperLib::IntPoint(someLine[i].x * cScaling, someLine[i].y * cScaling); }

	// http_://www.angusj.com/delphi/clipper/documentation/Docs/Units/ClipperLib/Types/EndType.htm
	cOffset.AddPath(subj, ClipperLib::jtSquare, ClipperLib::etOpenButt);
	cOffset.Execute(pSol, offsetVal * cScaling);

	std::vector<std::vector<AVector>>  offPolys;

	for (int a = 0; a < pSol.size(); a++)
	{
		std::vector<AVector> offPoly;
		for (int b = 0; b < pSol[a].size(); b++)
		{
			AVector iPt(pSol[a][b].X, pSol[a][b].Y);
			iPt /= cScaling;
			offPoly.push_back(iPt);
		}
		offPolys.push_back(offPoly);
	}

	return offPolys;
}

bool ClipperWrapper::IsClockwise(std::vector<AVector> polygon)
{
	float sumValue = 0;
	for (size_t a = 0; a < polygon.size(); a++)
	{
		AVector curPt = polygon[a];
		AVector nextPt;
		if (a == polygon.size() - 1)
		{
			nextPt = polygon[0];
		}
		else
		{
			nextPt = polygon[a + 1];
		}
		sumValue += ((nextPt.x - curPt.x) * (nextPt.y + curPt.y));
	}

	if (sumValue >= 0) { return false; }

	return true;
}

std::vector<AVector> ClipperWrapper::MakeClockwise(std::vector<AVector> shape)
{
	if (ClipperWrapper::IsClockwise(shape))
	{
		return shape;
	}

	std::vector<AVector> revShape = shape;
	std::reverse(revShape.begin(), revShape.end());
	return revShape;
}

std::vector<std::vector<AVector>> ClipperWrapper::MakeClockwise(std::vector<std::vector<AVector>> shapes)
{
	// should be clockwise
	std::vector<std::vector<AVector>> orientedShapes;
	for (int a = 0; a < shapes.size(); a++)
	{
		if (ClipperWrapper::IsClockwise(shapes[a]))
		{
			orientedShapes.push_back(shapes[a]);
		}
		else
		{
			std::vector<AVector> revShape = shapes[a];
			std::reverse(revShape.begin(), revShape.end());
			orientedShapes.push_back(revShape);
		}
	}
	return orientedShapes;
}
