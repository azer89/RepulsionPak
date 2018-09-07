
#include "RigidRegistration.h"
#include "ARectangle.h"
#include "MySVGRenderer.h"

#include "OpenCVWrapper.h"

#include "ColorPalette.h"

#include <Eigen/Dense>

RigidRegistration::RigidRegistration()
{
	float imgScale = 1.0f;
	_pngImg.CreateColorImage(SystemParams::_upscaleFactor * imgScale);
	_pngImg.SetColorImageToWhite();
}

RigidRegistration::~RigidRegistration()
{
}

AVector RigidRegistration::GetCentroid(const std::vector<AVector>& points)
{
	AVector centroidPt(0, 0);
	for (unsigned int a = 0; a < points.size(); a++)
		{ centroidPt += points[a]; }
	centroidPt /= ((float)points.size());
	return centroidPt;
}

void RigidRegistration::SaveOriGraph(AGraph g2, int numGraph2)
{
	// ===== get points =====
	std::vector<AVector> points2;
	for (unsigned a = 0; a < g2._massList.size(); a++)
		{ points2.push_back(g2._massList[a]._pos); }

	// ===== calculate centroid =====
	AVector centroid2 = GetCentroid(points2);

	// ===== calculate bb scale =====
	ARectangle bb = UtilityFunctions::GetBoundingBox(points2);
	float l = bb.witdh;
	if (bb.height > bb.witdh) { l = bb.height; }
	float scaleVal = 1.0f / l;

	// ===== bring the centroid to (0, 0) and shrink to unit bb =====
	for (unsigned a = 0; a < points2.size(); a++)
	{
		points2[a] -= centroid2;
		points2[a] *= scaleVal;
	}

	// ===== bring the centroid to (0, 0) and shrink to unit bb =====
	g2.RecalculateArts();
	std::vector<std::vector<AVector>> arts2 = g2._arts;
	for (unsigned int a = 0; a < arts2.size(); a++)
	{
		for (unsigned int b = 0; b < arts2[a].size(); b++)
		{
			arts2[a][b] -= centroid2;
			arts2[a][b] *= scaleVal;
		}
	}

	// SAVE!!!
	_oriArts.push_back(arts2);
	_artFColors.push_back(g2._fColors);
	_artBColors.push_back(g2._bColors);
	_deformedArts.push_back(std::vector<GraphArt>());

	// ===== drawing =====
	// DON't DELETE
	/*std::stringstream ss1;
	ss1 << "element_" << numGraph2;
	_pngImg.SetColorImageToWhite();
	float imgScale = 1.0f;
	float halfVal = imgScale * SystemParams::_upscaleFactor / 2.0f;
	float polyScale = 400;
	for (unsigned int a = 0; a < arts2.size(); a++)
	{
		for (unsigned int b = 0; b < arts2[a].size(); b++)
		{
			arts2[a][b] *= polyScale;
			arts2[a][b] += AVector(halfVal, halfVal);
		}
	}
	float lineThickness = 2.0f;
	for (int a = arts2.size() - 1; a >= 0; a--)
	{
		int fIdx = g2._fColors[a];
		int bIdx = g2._bColors[a];

		//background
		if (bIdx >= 0) { _cvWrapper.DrawFilledPoly(_pngImg, arts2[a], ColorPalette::_palette_01[bIdx], imgScale); }

		// foreground
		if (fIdx >= 0) { _cvWrapper.DrawPolyOnCVImage(_pngImg._img, arts2[a], ColorPalette::_palette_01[fIdx], true, lineThickness, imgScale); }
	
	}
	_pngImg.SaveImage(SystemParams::_save_folder + "ORI_ELEMENTS\\" + ss1.str() + ".png");
	MySVGRenderer::SaveArts(SystemParams::_save_folder + "ORI_ELEMENTS\\" + ss1.str() + ".svg", arts2, g2._fColors, g2._bColors);*/
}

float RigidRegistration::CalculateDistance(AGraph g1_deformed,  // deformed
	                                       AGraph g2_ori,  // original
										   int numGraph1,
										   int numGraph2)
{
	// get points
	std::vector<AVector> points1_deformed;// deformed
	for (unsigned a = 0; a < g1_deformed._massList.size(); a++)
	//for (unsigned a = 0; a < g1._boundaryPointNum; a++)
		{ points1_deformed.push_back(g1_deformed._massList[a]._pos); }// deformed

	// get points
	std::vector<AVector> points2_ori;
	for (unsigned a = 0; a < g2_ori._massList.size(); a++)
	//for (unsigned a = 0; a < g2._boundaryPointNum; a++)
		{ points2_ori.push_back(g2_ori._massList[a]._pos); }

	// move to center
	AVector centroid1 = GetCentroid(points1_deformed);
	AVector centroid2 = GetCentroid(points2_ori);

	// scale
	ARectangle bb_ori = UtilityFunctions::GetBoundingBox(points2_ori);
	float l_ori = bb_ori.witdh;
	if (bb_ori.height > bb_ori.witdh) { l_ori = bb_ori.height; }
	float scaleVal = 1.0f / l_ori;

	for (unsigned a = 0; a < points1_deformed.size(); a++)
	{ 
		points1_deformed[a] -= centroid1;// deformed
		points2_ori[a] -= centroid2;

		points1_deformed[a] *= scaleVal; // deformed
		points2_ori[a] *= scaleVal;
	}

	// arts	
	g1_deformed.RecalculateArts();
	std::vector<std::vector<AVector>> arts1 = g1_deformed._arts;
	for (unsigned int a = 0; a < arts1.size(); a++)
	{
		for (unsigned int b = 0; b < arts1[a].size(); b++)
		{
			arts1[a][b] -= centroid1;
			arts1[a][b] *= scaleVal;
		}
	}

	return CalculateDistance(points1_deformed, // deformed
		                     points2_ori, 
							 arts1, 
							 g1_deformed._fColors,      // deformed
							 g1_deformed._bColors,      // deformed
							 g1_deformed._skinPointNum, // deformed
							 numGraph1,
							 numGraph2);
}

void RigidRegistration::SaveArtToSVG(std::vector<AVector> rot,
					                 AVector trans, 
					                 std::vector<std::vector<AVector>> arts, 
					                 int numGraph)
{

}

float RigidRegistration::CalculateDistance(std::vector<AVector> points1,  // deformed
	                                       std::vector<AVector> points2,  // original
										   std::vector<std::vector<AVector>> arts, // deformed
										   std::vector<MyColor> fColors,
										   std::vector<MyColor> bColors,
										   int boundaryPointNum,  // deformed
										   int numGraph1,
										   int numGraph2)
{
	// step 1 
	// compute centroids
	AVector centroid1 = GetCentroid(points1);
	AVector centroid2 = GetCentroid(points2);
	
	// step 2
	// compute centered vectors
	std::vector<AVector> ct_points1;
	std::vector<AVector> ct_points2;
	for (unsigned int a = 0; a < points1.size(); a++)
	{
		ct_points1.push_back(points1[a] - centroid1);
		ct_points2.push_back(points2[a] - centroid2);
	}	

	// step 3
	// 3a) create X and Y matrices
	Eigen::MatrixXf X(2, points1.size());
	Eigen::MatrixXf Y(2, points1.size());
	for (unsigned int a = 0; a < points1.size(); a++)
	{
		X(0, a) = ct_points1[a].x;
		X(1, a) = ct_points1[a].y;

		Y(0, a) = ct_points2[a].x;
		Y(1, a) = ct_points2[a].y;
	}

	// 3b) compute the covariance matrix S = X Y^T
	Eigen::MatrixXf S = X * Y.transpose();

	//Eigen::JacobiSVD<Eigen::MatrixXf> svd(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
	Eigen::JacobiSVD<Eigen::MatrixXf> svd(S, Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::MatrixXf U = svd.matrixU();
	Eigen::MatrixXf V = svd.matrixV();

	// diff
	//Eigen::MatrixXf Cp = svd.matrixU() * svd.singularValues().asDiagonal() * svd.matrixV().transpose();
	//Eigen::MatrixXf diff = Cp - S;
	//cout << "diff: " << diff.array().abs().sum() << "\n";

	// step 4 rotation matrix
	Eigen::MatrixXf M = Eigen::MatrixXf::Identity(2, 2);
	float det = (V * U.transpose()).determinant();
	M(1, 1) = det;

	Eigen::MatrixXf R = V * M * U.transpose(); // this is the rotation matrix

	// step 5 translation
	AVector t = centroid2 - AVector(R(0, 0) * centroid1.x + R(0, 1) * centroid1.y, 
		                            R(1, 0) * centroid1.x + R(1, 1) * centroid1.y);

	// lets register points1
	for (unsigned a = 0; a < points1.size(); a++)
	{
		// rotate
		points1[a]  = AVector(R(0, 0) * points1[a].x + R(0, 1) * points1[a].y,
			                  R(1, 0) * points1[a].x + R(1, 1) * points1[a].y);
		// translate
		points1[a] += t;
	}

	float dist = 0;
	for (unsigned a = 0; a < points1.size(); a++)
		{ dist += points1[a].Distance(points2[a]); }
	
	// ----------------------------------------
	// debug drawing (START)
	for (unsigned int a = 0; a < arts.size(); a++)
	{
		for (unsigned int b = 0; b < arts[a].size(); b++)
		{
			arts[a][b] = AVector(R(0, 0) * arts[a][b].x + R(0, 1) * arts[a][b].y,
			                     R(1, 0) * arts[a][b].x + R(1, 1) * arts[a][b].y);
			arts[a][b] += t;
		}
	}

	// save the arts here
	// numGraph2 is the index of the original
	_deformedArts[numGraph2].push_back(arts);

	/*std::stringstream ss1;
	ss1 << "element_" << numGraph1;
	_pngImg.SetColorImageToWhite();
	float imgScale = 1.0f;
	float lineThickness = 2.0f;
	std::vector<AVector> points1_skin(points1.begin(), points1.begin() + boundaryPointNum);
	std::vector<AVector> points2_skin(points2.begin(), points2.begin() + boundaryPointNum);

	float halfVal = imgScale * SystemParams::_upscaleFactor / 2.0f;
	float polyScale = 400;

	for (unsigned int a = 0; a < points1_skin.size(); a++)
	{
		points1_skin[a] *= polyScale;
		points2_skin[a] *= polyScale;
		points1_skin[a] += AVector(halfVal, halfVal);
		points2_skin[a] += AVector(halfVal, halfVal);
	}
	for (unsigned int a = 0; a < arts.size(); a++)
	{
		for (unsigned int b = 0; b < arts[a].size(); b++)
		{
			arts[a][b] *= polyScale;
			arts[a][b] += AVector(halfVal, halfVal);
		}
	}
	//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, bbPoly,       MyColor(200, 200, 200), true, 1);
	
	
	//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, points1_skin, MyColor(200, 200, 200), true, lineThickness * 4.0f);
	//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, points2_skin, MyColor(242, 128, 170), true, lineThickness);
	//for (unsigned int a = 0; a < arts.size(); a++)
	for (int a = arts.size() - 1; a >= 0; a--)
	{
		int fIdx = fColors[a];
		int bIdx = bColors[a];

		//background
		if (bIdx >= 0)
		{
			_cvWrapper.DrawFilledPoly(_pngImg, arts[a], ColorPalette::_palette_01[bIdx], imgScale);
		}

		// foreground
		if (fIdx >= 0)
		{
			_cvWrapper.DrawPolyOnCVImage(_pngImg._img, arts[a], ColorPalette::_palette_01[fIdx], true, lineThickness, imgScale);
		}

		//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, arts[a], MyColor(0, 0, 170), true, lineThickness);
	}
	_pngImg.SaveImage(SystemParams::_save_folder + "DEFORMED_ELEMENTS\\" + ss1.str() + ".png");
	MySVGRenderer::SaveArts(SystemParams::_save_folder + "DEFORMED_ELEMENTS\\" + ss1.str() + ".svg", arts, fColors, bColors);
	// debug drawing (END)
	// ----------------------------------------
	*/
	// distance
	return dist;
}

void RigidRegistration::TranslateAndScale(GraphArt& arts, float scaleVal, AVector transVector)
{
	for (unsigned int a = 0; a < arts.size(); a++)
	{
		for (unsigned int b = 0; b < arts[a].size(); b++)
		{
			arts[a][b] *= scaleVal;
			arts[a][b] += transVector;
		}
	}
}

void RigidRegistration::Clear()
{
	for (unsigned int a = 0; a < _deformedArts.size(); a++)
	{
		_deformedArts[a].clear();
	}
}

void RigidRegistration::LineUpArts()
{
	float bb_size = 20.0f;
	float y_gap = 10.0f;
	float x_gap = 5.0f;

	std::vector<GraphArt> oArts = _oriArts;
	std::vector<std::vector<GraphArt>> dArts = _deformedArts;

	// ===== original art ===== 
	for (unsigned int a = 0; a < oArts.size(); a++)
	{
		float y_pos = a * (bb_size + y_gap);
		TranslateAndScale(oArts[a], bb_size, AVector(0, y_pos));
	}

	// ===== deformed art ===== 
	float x_start = bb_size + x_gap;
	for (unsigned int a = 0; a < dArts.size(); a++)
	{
		float y_pos = a * (bb_size + y_gap);
		for (unsigned int b = 0; b < dArts[a].size(); b++)
		{
			float x_pos = x_start + (b * (bb_size + x_gap));
			TranslateAndScale(dArts[a][b], bb_size, AVector(x_pos, y_pos));
		}
	}

	// save svg
	MySVGRenderer::LineUpArts(SystemParams::_save_folder + "DEFORMED_ELEMENTS\\line_up.svg",
							  oArts,
							  dArts,
							  _artFColors,
							  _artBColors);
}
