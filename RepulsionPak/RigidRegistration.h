
#ifndef __Rigid_Registration__
#define __Rigid_Registration__

#include "OpenCVWrapper.h"
#include "AVector.h"
#include "AGraph.h"

#include "MyColor.h"

typedef std::vector<std::vector<AVector>> GraphArt;
typedef std::vector<MyColor> ArtColors;

/*

This note is an implementation of a note:

LEAST-SQUARES RIGID MOTION USING SVD
Olga Sorkine-Hornung and Michael Rabinovich

*/
class RigidRegistration
{
public:
	RigidRegistration();
	~RigidRegistration();

	void SaveOriGraph(AGraph g2, int numGraph2);

	float CalculateDistance(AGraph g1, AGraph g2, int numGraph1, int numGraph2);

	float CalculateDistance(std::vector<AVector> points1, 
		                    std::vector<AVector> points2, 
							std::vector<std::vector<AVector>> arts,
							std::vector<MyColor> fColors,
							std::vector<MyColor> bColors,
							int boundaryPointNum, 
							int numGraph1,
							int numGraph2);

	void SaveArtToSVG(std::vector<AVector> rot, 
					  AVector trans, 
					  std::vector<std::vector<AVector>> arts, 
					  int numGraph);

	AVector GetCentroid(const std::vector<AVector>& points);


	void LineUpArts();

	void TranslateAndScale(GraphArt& arts, float scaleVal, AVector transVector);

	void Clear();

public:
	OpenCVWrapper _cvWrapper;
	CVImg         _pngImg;

private:
	std::vector<GraphArt> _oriArts;
	std::vector<ArtColors> _artFColors;
	std::vector<ArtColors> _artBColors;
	std::vector<std::vector<GraphArt>> _deformedArts;
};

#endif