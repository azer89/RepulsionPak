
/* ---------- RepulsionPak  ---------- */

#ifndef STUFF_WORKER_H
#define STUFF_WORKER_H

#include "AVector.h"
#include "VFRegion.h"
#include "PathIO.h"
#include "UtilityFunctions.h"
#include "SystemParams.h"

#include "AnElement.h"

#include "ContainerWorker.h"

#include "ADistanceTransform.h"
#include "CollissionGrid.h"
#include "OpenCVWrapper.h"
#include "RigidRegistration.h"
//#include "NANOFLANNWrapper.h"
//#include "Quadtree.h"
//#include "QTObject.h"

// read directory
#include "dirent.h"

#include "FastNoise.h"

#include "PAD.h"

#include "ATimerStat.h"
#include "ThreadPool.h"
//#include <atomic>
//#include <mutex>

// bins
//	first: which graph
//	second: edge index
//typedef std::pair<int, int> ABinItem;

class StuffWorker
{
public:
	// constructor
	StuffWorker();

	// destructor
	~StuffWorker();

	// initializing static variables
	//static void InitStatic();

	std::vector<std::string> LoadFiles(std::string directoryPath); // read directory
	void LoadOrnaments(); // ornaments

	//void LoadContainer(); // container boundary
	/*void CreatePoints(std::vector<AVector>  ornamentBoundary, // container boundary
					  float img_length,
					  std::vector<AVector>& randomPoints,
					  int& boundaryPointNum);*/

					  // pyramid of arclength descriptor
					  //void CalculatePAD();

	void ProcessOrnaments(); 	          // skin and triangulate. Create copies
	void SkinAndTriangulateOrnaments();   // is called from ProcessOrnaments()  	
	void ProcessSmallOrnaments1();        // is called from ProcessOrnaments()	
	void ProcessFocalOrnaments();
	void AddNewSmallElements();

	AnElement ProcessAnOrnament(AnElement oriGraph, AVector pos, float scale, int graphID/*, float maxEdgeLengthFactor*/);

	void FindClickedMass(float x, float y);
	void DragClickedMass(float x, float y);
	void DragClickedGraph(float x, float y);

	void DrawClickedMass();

	void Draw();
	//void DrawBins();

	// Send tasks to threadpool, see AlmostAllYourShit()
	void UpdateCollisionGrid_PrepareThreadPool();
	void Final_PrepareThreadPool(float dt);

	// A task for a single thread
	void Final_ThreadTask(float dt, int startIdx, int endIdx);

	// ------------ physics simulation ------------
	void Finall_ThreadPass(float dt); // Init(), Solve(), Simulate() all combined in a single pass to threadpool
	void Operate(float dt);
	void CalculateThings(float dt);

	void Init();              // not used anymore
	void Solve();             // not used anymore	
	void Simulate(float dt);  // not used anymore
	// --------------------------------------------


	void UpdatePosition(float dt); // verlet
	void UpdateVelocity(float dt); // verlet

	//void SaveGraphToPNG(AGraph oriGraph, std::string saveFolder, int numGraph, float initScale = 1.0f);

	// IO
	void SaveGraphs(); // _rr->LineUpArts();
	void SaveSVG(int frameCounter);
	void SavePNG(int frameCounter);
	void CalculateSDF(int numIter, bool saveImage = false);
	void CalculateFillAndRMS();
	void SaveDataToCSV();

	void RecreateDistanceTransform(float scale);

	AVector GetPerlinVector(int x, int y);
	void ComputePerlinMap(int t);

public:

	//int _num_vertex;


	//float _man_neg_ratio;

	//AVector start_ln;
	//AVector end_ln;
	//AVector rand_pt;
	//AVector pt_on_line;
	ContainerWorker* _containerWorker;
	ADistanceTransform* _aDTransform;
	RigidRegistration* _rr;
	CVImg               _pngImg;
	OpenCVWrapper       _cvWrapper;

	int  _numBigOnes;           // primary elements, not 2nd elements (not duplicated)
	int  _numReplicatedBigOnes; // duplicated
	bool _hasSmallElements;     // flag
	std::vector<VFRegion>       _ornamentRegions; // elements	
	static std::vector<AnElement>  _graphs;          // elements	
	std::vector<AnElement>         _oriGraphs;       // elements
	std::vector<AnElement>         _smallOriGraph1;  // elements

	//std::vector<AVector> _randomPositions;  // for elements

	float _deformationValue;
	float _avgSkinThickness;

	// collission grid
	static CollissionGrid* _cGrid;

	/*PADCalculator      _padCalc; // boundary

	std::vector<std::vector<std::vector<AVector>>> _focals;   // container
	std::vector<std::vector<AVector>> _offsetFocalBoundaries; // container

	// this is old code with VF
	std::vector<VFRegion> _container_vf_region;              // container
	std::vector<std::vector<AVector>> _container_boundaries; // container
	std::vector<std::vector<AVector>> _holes;                // container
	//std::vector<std::vector<AVector>> _tempBoundaries;*/

	//int _clickedMassIndex;
	std::pair<int, int> _clickedIndices;

	//float _avgDist;
	//float _containerArea;

	int _numGrowingElement;
	int _numPoints;
	int _numTriangles;
	int _numTriEdges;
	int _numAuxEdges;

	float _sumVelocity; // _positionDelta

	float _totalRepulsionF;
	float _totalEdgeF;
	float _totalBoundaryF;
	float _totalOverlapF;
	float _avgScaleFactor;
	//float _totalAttractionF;
	//float _totalFoldingF;	
	std::vector<std::vector<float>> _stuffData; // data for csv

	//AVector _peakPos;
	//float _maxDist;	
	//std::vector<float> _max_dist_array;	
	//float _peak_rms;

	std::vector<float> _fill_ratio_array;
	float _fill_diff;
	float _fill_ratio;
	float _fill_rms;
	float _sim_timeout;

	//std::vector<AVector> _peaks;
	//std::vector<AVector> _debugPoints;
	//bool _hasShrinkingInitiated;

	// static stuff
	static std::vector<std::vector<AVector>> _perlinMap;
	FastNoise          myNoise1;
	FastNoise          myNoise2;
	//int _clickedGraphIndex;

// thread
public:
	ThreadPool* _my_threadpool;
	//ThreadPool* _my_threadpool_solve_springs;
	//int _main_iter_01;
	//std::vector<int> _thread_iters_01;
	//std::mutex _mutex_01;

	//void PrepareThreads_01();
	//int _cg_thread_t;
	//int _springs_thread_t;
	//int _c_pt_thread_t;
	//int _solve_thread_t;

	//int _cg_cpu_t;
	//int _c_pt_cpu_t;

	ATimerStat _closest_pt_cpu_time;
	ATimerStat _closest_pt_thread_time;

	ATimerStat _solve_s_cpu_time;
	ATimerStat _solve_s_thread_time;

	ATimerStat _c_grid_cpu_time;
	ATimerStat _c_grid_thread_time;

public:
	//void InitThreads();

	void SolveSprings_Prepare_Threads();
	void SolveSprings_Thread(int startIdx, int endIdx);

	void GetClosestPt_Prepare_Threads();
	void GetClosestPt_Thread(int startIdx, int endIdx);

public:
	// ====================================================
	void CalculateSkeleton();
	int _skeletonIter;

	// ====================================================
	CollissionGrid* _manualGrid;
	std::vector<std::vector<AVector>> _manualElements;   // CreateManualPacking();  // all in 2D array
	std::vector<GraphArt>			  _manualElementsss; // CreateManualPacking2(); // in 3D array
	std::vector<std::vector<AVector>> _manualSkeletons;
	std::vector<std::vector<AVector>> _manualContainer;

	void CalculateMetrics();
	void CalculateMetrics2();
	void CalculateMetrics3();
	void AddToAccumulationBuffer(std::vector<std::vector<AVector>> elem, CVImg& accumulationBuffer, int startVal, int numIter);
	void DrawAccumulationBuffer(CVImg accumulationBuffer, float startColor, float offsetVal, float overlapArea, int numIter);

	void AnalyzeFinishedPacking();
	void CreateManualPacking();
	void CreateManualPacking2();
	void AnalyzeManualPacking();

	// ====================================================
	CollissionGrid* _sqCGrid;
	std::vector<std::vector<AVector>> _squares;
	std::vector<std::vector<AVector>> _squareContainer;
	//void CreateSquares();


};

#endif