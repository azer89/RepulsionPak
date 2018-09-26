#ifndef CONTAINER_WORKER_H
#define CONTAINER_WORKER_H

#include "PAD.h"
#include "CollissionGrid.h"
#include "VFRegion.h"
#include "AGraph.h"

#include "nanoflann.hpp"
#include "KDTreeVectorOfVectorsAdaptor.h"

#include "OpenCVWrapper.h"

typedef std::vector<std::vector<float> > my_vector_of_vectors_t;
typedef KDTreeVectorOfVectorsAdaptor< my_vector_of_vectors_t, float >  my_kd_tree_t;

// first index is which element, the second index 
typedef std::vector<std::pair<int, int> > my_vector_of_info_t;

class ContainerWorker
{
public:
	ContainerWorker();
	~ContainerWorker();

	void LoadContainer(CollissionGrid* cGrid);

	void CreatePoints(std::vector<AVector>  ornamentBoundary, // container boundary
					  float img_length,
		              std::vector<AVector>& randomPoints,
					  int& boundaryPointNum);

	void Draw();

	bool IsGraphInside(const AGraph& g);

	std::vector<AGraph> PlacementWithMatching(std::vector<AGraph>& oriGraphs);
	std::vector<AGraph> PlacementWithMatching2(std::vector<AGraph>& oriGraphs);
	void PlacementWithMatching3(std::vector<AGraph>& oriGraphs, std::vector<AGraph>& matchedGraphs, std::vector<bool>& oriGraphsFlags);
	
	void DrawDebug(const std::vector<AVector>& uniArt, 
					const std::vector<AVector>& l_poly,
					const std::vector<AVector>& r_poly,
		           AVector dockPoint, 
					const std::vector<AVector>& l_poly_container,
					const std::vector<AVector>& r_poly_container,
		           float scoreVal, 
		           float insideScoreVal, 
				   float onScoreVal,
		           float outsideScoreVal,
					const std::vector<AVector>& insidePoints,
		const std::vector<AVector>& outsidePoints,
		const std::vector<AVector>& onPoints);

	float GetScore(const std::vector<AVector>& uniArt, 
				   const std::vector<AVector>& l_poly,
		           const std::vector<AVector>& r_poly,
		           AVector dockPoint,
					const std::vector<AVector>& l_poly_container,
					const std::vector<AVector>& r_poly_container );
	void  GetLRSegments(PADDescriptor desc, std::vector<AVector> poly, std::vector<AVector>& l_poly, std::vector<AVector>& r_poly);

	// KNN
	void PrepareKNN(std::vector<AGraph>& oriGraphs);
	my_vector_of_info_t GetNN(std::vector<float> queryPt);

public:

	// debug
	int _debug_counter;
	CVImg _debugImg;
	OpenCVWrapper       _cvWrapper;

	float _container_length; // 1

	//std::vector<AVector> _score_debug;


	PADCalculator      _padCalc; 

	std::vector<std::vector<std::vector<AVector>>> _focals;   
	std::vector<std::vector<AVector>> _offsetFocalBoundaries; 

	// this is old code with VF
	std::vector<VFRegion> _container_vf_region;              
	std::vector<std::vector<AVector>> _container_boundaries; // original container
	std::vector<std::vector<AVector>> _holes;               
	//std::vector<std::vector<AVector>> _tempBoundaries;

	std::vector<AVector> _randomPositions;  // for elements

	// ramer douglas peucker
	std::vector<bool> _rdpFlags;  // 1
	std::vector<AVector> _rdp_container;   // 1 // simplified container

	// resampled container
	std::vector<AVector> _resampled_container;  // 1

	// CSS smoothing
	std::vector<AVector> _smooth_container;   // 1 // smooth container

	// for KNN
	my_vector_of_vectors_t _descriptors;
	my_vector_of_info_t _descriptor_info;
	my_kd_tree_t* _mat_index;
	
};

#endif
