
/**
 * Curve smoothing using gaussian kernel
 *
 *
 * Author: Reza Adhitya Saputra (reza.adhitya.saputra@gmail.com)
 * Version: 2014
 *
 *
 */

#ifndef __CSS_Smoothing__
#define __CSS_Smoothing__

#include <vector>
#include "AVector.h"
#include "OpenCVWrapper.h"
 //#include "SystemParams.h"



 // split curves to x and y components
template<typename T, typename V>
void PolyLineSplit(const std::vector<T>& pl, std::vector<V>& contourx, std::vector<V>& contoury)
{
	contourx.resize(pl.size());
	contoury.resize(pl.size());

	for (size_t j = 0; j < pl.size(); j++)
	{
		contourx[j] = pl[j].x;
		contoury[j] = pl[j].y;
	}
}

// merge x and y components to a single curve
template<typename T, typename V>
void PolyLineMerge(std::vector<T>& pl, const std::vector<V>& contourx, const std::vector<V>& contoury)
{
	assert(contourx.size() == contoury.size());
	pl.resize(contourx.size());
	for (size_t j = 0; j < contourx.size(); j++)
	{
		pl[j].x = contourx[j];
		pl[j].y = contoury[j];
	}
}

//namespace CVSystem
//{
	// Class for gaussian refinement
	// // this could cause shortening
class CSSSmoothing
{
public:
	CSSSmoothing();
	~CSSSmoothing();

	// old version
	void Init1(float sigma);
	void SmoothCurve1(std::vector<AVector>& curve);

	// improved version, need opencv ?
	void Init2();
	void SmoothCurve2(std::vector<AVector>& curve, cv::Mat distance_img);

private:

	// gaussian kernel
	std::vector<double> _g;

	// first derivative
	std::vector<double> _dg;

	// second derivative
	std::vector<double> _d2g;

	// gaussian sigma
	double _sigma;

	int _M;

	int _min_affinity;

private:

	bool ShouldDoSmoothing(std::vector<AVector> oriCurve, int n, cv::Mat distance_img);

	// 1st and 2nd derivative of 1D gaussian 
	void getGaussianDerivs(double sigma, int M, std::vector<double>& gaussian, std::vector<double>& dg, std::vector<double>& d2g);

	void getGaussian(double sigma, int M, std::vector<double>& gaussian);

	// 1st and 2nd derivative of smoothed curve point 
	void getdX(std::vector<double> x,
		int n,
		double sigma,
		double& gx,
		double& dgx,
		double& d2gx,
		std::vector<double> g,
		std::vector<double> dg,
		std::vector<double> d2g,
		bool isOpen = false);

	void getdXY(std::vector<AVector> x,
		int n,
		double sigma,
		AVector& gx,
		std::vector<double> g);


	// 0th, 1st and 2nd derivatives of whole smoothed curve
	void getdXcurve(std::vector<double> x,
		double sigma,
		std::vector<double>& gx,
		std::vector<double>& dx,
		std::vector<double>& d2x,
		std::vector<double> g,
		std::vector<double> dg,
		std::vector<double> d2g,
		bool isOpen = false);

	void getdXYcurve(std::vector<AVector> x,
		double sigma,
		std::vector<AVector>& gx,
		std::vector<double> g,
		cv::Mat distance_img);
};
//}

#endif