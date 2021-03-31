
#ifndef __Collission_Grid__
#define __Collission_Grid__

#include <vector>

#include "SystemParams.h"
#include "ASquare.h"
#include "AVector.h"

#include "ThreadPool.h"

#include <cmath>

// to read:

// http_://gamedev.stackexchange.com/questions/72030/using-uniform-grids-for-collision-detection-efficient-way-to-keep-track-of-wha

typedef std::vector<int> GraphIndices;

class CollissionGrid
{
public:
	CollissionGrid();

	CollissionGrid(float cellSize);

	~CollissionGrid();

	void GetCellPosition(int& xPos, int& yPos, float x, float y);

	//void InsertAPoint(float x, float y, int info1, int info2, float nx, float ny);

	void InsertAPoint(float x, float y, int info1, int info2);

	void GetGraphIndices1(float x, float y, std::vector<int>& closestGraphIndices); // ADistanceTransform

	void GetGraphIndices1B(float x, float y, std::vector<int>& closestGraphIndices); // ADistanceTransform

	//void GetGraphIndices2(float x, float y, int parentGraphIndex, std::vector<int>& closestGraphIndices);

	std::vector<int>* GetGraphIndicesPtr(float x, float y, int parentGraphIndex);

	void GetGraphIndices2B(float x, float y, int parentGraphIndex, std::vector<int>& closestGraphIndices);

	void GetData(float x, float y, int parentGraphIndex, std::vector<AVector>& closestPts, std::vector<int>& closestGraphIndices);

	void GetClosestPoints(float x, float y, std::vector<AVector>& closestPts);

	std::vector<AnObject*> GetObjects(float x, float y);

	void MovePoints();

	void Draw();

	void AnalyzeContainer(const std::vector<std::vector<AVector>>&  boundaries, 
		const std::vector<std::vector<AVector>>&  holes,
		                  const std::vector<std::vector<AVector>>& offsetFocalBoundaries);

	void PrecomputeGraphIndices();
	//void PrecomputeGraphIndices2();

	//void PrecomputeData_Prepare_Threads();
	void PrecomputeGraphIndices_ThreadTask(int startIdx, int endIdx);

	bool NearBoundary(float x, float y);

	
	//void PrecomputeData_Thread(int startIdx, int endIdx);

public:
	//std::vector<GraphIndices> _graphIndexArray;

	int _numColumn;
	float _maxLength;

	std::vector<AnObject*> _objects;

	std::vector<ASquare*> _squares;

	//ThreadPool* _my_threadpool;
};

#endif