

/* ---------- ShapeRadiusMatching V2  ---------- */

/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef PATH_IO_H
#define PATH_IO_H

#include "ArtData.h"
#include "VFRegion.h"
//#include "ABlob.h"
//#include "LRFunctions.h"
#include "OpenCVWrapper.h"
#include "AGraph.h"

#include <vector>
#include <cstring>

class PathIO
{
public:
	PathIO();
	~PathIO();

	// force data
	void SaveData(std::vector < std::vector<float> > forceData, std::string filename);

	// graph
	void SaveAGraph(AGraph, std::string filename);
	AGraph LoadAGraph(std::string filename);

	// art
	void SaveArtData(std::vector<ArtData> artDataArray, std::string filename);
	std::vector<ArtData> LoadArtData(std::string filename);

	void SaveNormSDF2CSV(std::vector<float> distArray, std::string filename);
	void SaveSDF2CSV(std::vector<float> distArray, std::string filename);

	// pair functions
	//std::vector<LRFunctions> LoadPairFunctions(std::string filename);
	//void SavePairFunctions(std::vector<LRFunctions> pfs, std::string filename);

	// regions
	void SaveRegions(std::vector<VFRegion> regions, std::string filename); // save to a file	
	void SavePath(std::ofstream* f, APath aPath, int no_region); // [region number] [is_closed_path] [path_type] x0 y0 x1 y1 x2 y2...	
	std::vector<VFRegion> LoadRegions(std::string filename); // load from a file

	// blobs
	//std::vector<ABlob> LoadBlobs(std::string filename);
	//void SaveBlobs(std::vector<ABlob> blobs, std::string filename);	

	// 

	// matching
	//std::vector<int> LoadIndices(std::string filename);
	//void SaveIndices(std::vector<int> indices, std::string filename);

	// CV_32FC1
	//CVImg LoadFloatImage(std::string filename);
	//void SaveFloatImage(CVImg img, std::string filename);

	// useful things
	bool DoesFileExist(std::string filename);

	void SaveInfo(std::string filename,
		          float simulation_time,
				  float fill_ratio,
				  float fill_rms,
				  float deformation_value,
				  float avg_skin_offset,
				  int num_elements,
				  int num_frame,
				  int num_triangle_edge,
				  int num_aux_edge,
				  int num_points,
				  int num_triangles,
				  int seed);

};

#endif