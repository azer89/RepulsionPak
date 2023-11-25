
/**
* http_://jimherold.com/2012/04/20/least-squares-bezier-fit/
*
* Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
* Version: 2014
*
*/

#include "CurveFitting.h"
#include "UtilityFunctions.h"

bool CurveFitting::PointstoBezier(std::vector<AVector> points, std::vector<AVector>& P)
{
	bool retVal = true;
	P = std::vector<AVector>(4);

	Eigen::MatrixXd M(4, 4);
	M << -1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0;

	Eigen::MatrixXd MInv = M.inverse();

	Eigen::MatrixXd U = GetU(points);
	Eigen::MatrixXd UT = U.transpose();

	Eigen::MatrixXd  X = GetX(points);
	Eigen::MatrixXd  Y = GetY(points);

	Eigen::MatrixXd A = UT * U;

	if (A.determinant() == 0)
	{
		std::cout << "A determinant is zero\n";
		retVal = false;
	}

	Eigen::MatrixXd B = A.inverse();
	Eigen::MatrixXd C = MInv * B;
	Eigen::MatrixXd D = C * UT;
	Eigen::MatrixXd E = D * X;
	Eigen::MatrixXd F = D * Y;

	for (int i = 0; i < 4; i++)
	{
		double x = E(i, 0);
		double y = F(i, 0);

		AVector p(x, y);
		P[i] = p;
	}
	return retVal;
}

Eigen::MatrixXd CurveFitting::GetX(std::vector<AVector> points)
{
	Eigen::MatrixXd X(points.size(), 1);

	for (size_t i = 0; i < points.size(); i++)
		X.row(i)(0) = points[i].x;

	return X;
}

Eigen::MatrixXd CurveFitting::GetY(std::vector<AVector> points)
{
	Eigen::MatrixXd Y(points.size(), 1);

	for (size_t i = 0; i < points.size(); i++)
		Y.row(i)(0) = points[i].y;

	return Y;
}

Eigen::MatrixXd CurveFitting::GetU(std::vector<AVector> points)
{
	std::vector<double> npls = NormalizedPathLengths(points);

	Eigen::MatrixXd U(npls.size(), 4);

	for (size_t i = 0; i < npls.size(); i++)
	{
		U.row(i)(0) = pow(npls[i], 3);
		U.row(i)(1) = pow(npls[i], 2);
		U.row(i)(2) = pow(npls[i], 1);
		U.row(i)(3) = pow(npls[i], 0);
	}

	return U;
}

std::vector<double> CurveFitting::NormalizedPathLengths(std::vector<AVector> points)
{
	std::vector<double> pathLength(points.size());
	std::vector<double> zpl(points.size());

	pathLength[0] = 0;

	for (size_t i = 1; i < points.size(); i++)
	{
		AVector p1 = points[i];
		AVector p2 = points[i - 1];
		double distance = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
		pathLength[i] += pathLength[i - 1] + distance;
	}


	for (size_t i = 0; i < zpl.size(); i++)
	{
		zpl[i] = pathLength[i] / pathLength[pathLength.size() - 1];
	}

	return zpl;
}

void CurveFitting::DeCasteljau(std::vector<AVector>& poly, AVector p0, AVector p1, AVector p2, AVector p3, double subdivide_limit)
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

std::vector<AVector> CurveFitting::GetBezier(std::vector<AVector> points, float minOffset)
{
	std::vector<AVector> poly2;
	UtilityFunctions::UniformResample(points, poly2, 0.2f);

	std::vector<AVector> aQuad;
	PointstoBezier(poly2, aQuad);
	std::vector<AVector> newPoly;
	DeCasteljau(newPoly, aQuad[0], aQuad[1], aQuad[2], aQuad[3], minOffset);
	std::cout << "cubic fitting curve size: " << newPoly.size() << "\n";
	return newPoly;
}