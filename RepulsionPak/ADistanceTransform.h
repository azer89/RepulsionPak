
#ifndef A_DISTANCE_TRANSFORM_H
#define A_DISTANCE_TRANSFORM_H

#include "OpenCVWrapper.h"
#include "AGraph.h"
#include "AVector.h"
#include "CollissionGrid.h"
#include <vector>

class ADistanceTransform
{
public:
	ADistanceTransform(//const std::vector<AGraph>& graphs, 
		               const std::vector<std::vector<AVector>>& containers,
					   const std::vector<std::vector<AVector>>& holes,
					   const std::vector<std::vector<AVector>>& focals,
					   float scale = 2.0);//B
	//ADistanceTransform(//const std::vector<std::vector<AVector>> graphs,
	//	               const std::vector<std::vector<AVector>>& containers,
	//				   const std::vector<std::vector<AVector>>& focals,
	//				   float scale = 2);//B
	~ADistanceTransform();

	void AddGraph(const AGraph& aGraph);
	void AddGraph(const std::vector<AVector>& graphs);

	void UpdateBoundaries(const std::vector<AGraph>& graphs);

	// use this
	//void CalculateFillWoSkin(CollissionGrid* cGrid, float& fill_percentage, int numIter, bool saveImage = false);

	// use this
	//void CalculateSDFWoSkin(CollissionGrid* cGrid, int numIter, bool saveImage = false);
	
	// uhhhh...
	void CalculateFill(CollissionGrid* cGrid, float& fill_ratio, int numIter, bool saveImage = false);

	// uhhhh...
	void CalculateSDF1(CollissionGrid* cGrid, int numIter, bool saveImage = false); // for manual packing
	void CalculateSDF2(const std::vector<AGraph>& graphs, CollissionGrid* cGrid, int numIter, bool saveImage = false); // for graphs (_arts)
	

	void CalculatePeaks();
	// soon to be deprecated
	/*void CalculateDistanceTransform2(CollissionGrid* cGrid, 
									 AVector& peakPos,
									 float& maxDist,
									 float& fill_percentage,
									 std::vector<AVector>& peaks,
									 int numIter);*/

public :
	void DebugOverlapMask(std::vector<int> overlapMask, CVImg thinningImage, std::string imageName);

	CVImg VoronoiSkeleton(CollissionGrid* cGrid, int numIter);
	CVImg SkeletonDistance(std::vector<int> overlapMask, std::string imageName);
	CVImg SkeletonDraw(std::string imageName);
	void DebugDistanceImage(CVImg thinningImage, std::string imageName);
	void DebugDistanceImage(std::string imageName);
	
public:
	// manaul thinning skeletons
	std::vector<std::vector<AVector>> _manualSkeletons;

	std::vector<AVector> _peaks;

	float _maxDist;

private:
	//float _avgSkinThickness;

	float _scale;
	int _sz;

	OpenCVWrapper _cvWrapper;

	std::vector<float> _containerDistArray;
	std::vector<float> _distArray;
	//cv::Mat _containerDistImage;
	//cv::Mat _distImage; // initialize once, updated every step 
	
	CVImg _fill_img_template;

	//std::vector<AVector> _distImagez; // initialize once
	std::vector<std::vector<cv::Point2f>> _graphBoundaries; // initialize once, updated every step

	std::vector<MyColor> _voronoiColors; // for voronoi skeletons
	std::vector<CVImg>   _voronoiBWImages; // for voronoi skeletons
	std::vector<std::vector<AVector>> _containers;  // for voronoi skeletons

	

	float _container_size;	
	//CVImg _intImg;
};

#endif