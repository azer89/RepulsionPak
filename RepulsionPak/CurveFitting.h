
/**
* Bezier curve fitting using least square (http_://jimherold.com/2012/04/20/least-squares-bezier-fit/)
*
*
* Author: Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
* Version: 2014
*
*
*/

#ifndef __Curve_Fitting__
#define __Curve_Fitting__

#include <vector>
#include <Eigen/Dense>
#include "AVector.h"

//namespace CVSystem
//{
	class CurveFitting
	{
	public:
		static std::vector<AVector> GetBezier(std::vector<AVector> points, float minOffset);

		// fit bezier curve to a set of points
		static bool PointstoBezier(std::vector<AVector> points, std::vector<AVector>& P);

		static void DeCasteljau(std::vector<AVector>& poly, AVector p0, AVector p1, AVector p2, AVector p3, double subdivide_limit);

	private:
		// calculate U matrix
		static Eigen::MatrixXd GetU(std::vector<AVector> points);

		// calculate X matrix
		static Eigen::MatrixXd GetX(std::vector<AVector> points);

		// calculate Y matrix
		static Eigen::MatrixXd GetY(std::vector<AVector> points);

		// normalize point set
		static std::vector<double> NormalizedPathLengths(std::vector<AVector> points);
	};
//}

#endif