
#include "StuffWorker.h"

#include "CGALTriangulation.h"
#include "Kruskal.h"
#include "ARectangle.h"
#include "AnElement.h"
#include "MySVGRenderer.h"

#include "glew.h"
#include "freeglut.h"

#include "ClipperWrapper.h"
#include "ColorPalette.h"
#include "CollissionGrid.h"
#include "FastNoise.h"

#include <time.h> // time seed
#include <stdlib.h>	/* srand, rand */
#include <time.h> 
#include <random>
#include <thread>

// static stuff
std::vector<std::vector<AVector>> StuffWorker::_perlinMap = std::vector<std::vector<AVector>>();
std::vector<AnElement>  StuffWorker::_graphs = std::vector<AnElement>();
CollissionGrid* StuffWorker::_cGrid = 0;

// constructor
StuffWorker::StuffWorker()
{
	//_closest_pt_cpu_time = 0;
	//_closest_pt_thread_time = 0;

	//_solve_s_cpu_time = 0;
	//_solve_s_thread_time = 0;

	//_c_grid_cpu_time = 0;
	//_c_grid_thread_time = 0;

	_skeletonIter = 0;

	//_man_neg_ratio = -1.0;

	_numBigOnes = 0;
	_numReplicatedBigOnes = 0;

	//_clickedGraphIndex = -1;
	_hasSmallElements = false;

	// ---------- collission ---------- 
	//_cGrid = new CollissionGrid(); // for physics pak
	StuffWorker::_cGrid = new CollissionGrid();
	_sqCGrid = 0;                  // for squares
	_manualGrid = 0;               // for manual packing

	_containerWorker = new ContainerWorker();

	_rr = new RigidRegistration();
	_deformationValue = 0;

	// ---------- mouse click ----------
	_clickedIndices.first = -1;
	_clickedIndices.second = -1;

	// ----------  ----------
	_avgSkinThickness = 0;

	_numGrowingElement = 0;
	_numPoints = 0;
	_sumVelocity = 0.0f;
	_totalRepulsionF = 0.0f;
	_totalEdgeF = 0.0f;
	_totalBoundaryF = 0.0f;
	_totalOverlapF = 0.0f;
	_avgScaleFactor = 0.0f;

	_aDTransform = 0;
	//_peakPos = AVector();
	//_maxDist = 0;

	_fill_diff = -1000;
	_fill_ratio = -5.0;
	_fill_rms = 0;
	_sim_timeout = 100000000;

	//_maxDist = 0;
	//_peak_rms = 0;	
	//_allStopGrow = true;

	//CVImg         _pngImg;
	float imgScale = 4.0f;
	_pngImg.CreateColorImage(SystemParams::_upscaleFactor * imgScale);
	_pngImg.SetColorImageToWhite();
	//OpenCVWrapper _cvWrapper;

	//CreateSquares();       // don't forget to change params.lua
	//CreateManualPacking2();  // overlap metrics
	//CreateManualPacking(); // SDF and stuff
	//AnalyzeManualPacking();

	//glutLeaveMainLoop(); // WOOOOOOOOOOOOOOOOOOO
	//MyColor::_black.Print();
	//MyColor::_white.Print();
	//_hasShrinkingInitiated = false;

	// NOISE FORCE
	myNoise1 = FastNoise(rand()); // Create a FastNoise object
	myNoise1.SetNoiseType(FastNoise::Perlin); // Set the desired noise type

	myNoise2 = FastNoise(rand()); // Create a FastNoise object
	myNoise2.SetNoiseType(FastNoise::Perlin); // Set the desired noise type
	for (unsigned int a = 0; a < SystemParams::_upscaleFactor; a++)
	{
		std::vector<AVector> pMap;
		for (unsigned int b = 0; b < SystemParams::_upscaleFactor; b++)
		{
			pMap.push_back(AVector());
		}
		StuffWorker::_perlinMap.push_back(pMap);
	}
	ComputePerlinMap(0);

	/*
	AVector start_ln;
	AVector end_ln;
	AVector rand_pt;
	AVector pt_on_line;
	*/

	//start_ln = AVector(200, 100);
	//end_ln = AVector(330, 300);
	_my_threadpool = new ThreadPool(SystemParams::_num_threads);
	//_my_threadpool_solve_springs = new ThreadPool(SystemParams::_num_thread_springs);
}

//void StuffWorker::InitStatic()
//{
//}

StuffWorker::~StuffWorker()
{
	//if (_cGrid)   { delete _cGrid; }
	if (_containerWorker) { delete _containerWorker; }
	if (_sqCGrid) { delete _sqCGrid; }
	if (_manualGrid) { delete _manualGrid; }
	if (_rr) { delete _rr; }
	if (_aDTransform) { delete _aDTransform; }
	if (_my_threadpool) { delete _my_threadpool; }
	//if (_my_threadpool_solve_springs) { delete _my_threadpool_solve_springs; }
}

/*
================================================================================
IO
================================================================================
*/
std::vector<std::string> StuffWorker::LoadFiles(std::string directoryPath)
{
	std::vector<std::string> fileStr;
	/*----------  dirent ----------*/
	DIR* dp;
	struct dirent* ep;
	/*---------- open directory ----------*/
	dp = opendir(directoryPath.c_str());
	if (dp != NULL)
	{
		while (ep = readdir(dp)) { fileStr.push_back(ep->d_name); }
		(void)closedir(dp);
	}
	else { perror("Couldn't open the directory"); }

	return fileStr;
}

// ornaments
void StuffWorker::LoadOrnaments()
{
	std::cout << "ProcessOrnaments - Normal\n";

	// ---------- load ornaments ----------
	PathIO pathIO;

	// ---------- regular ornaments ----------
	std::vector<std::string> ornamentFiles2 = LoadFiles(SystemParams::_primary_elem_dir); ////

	//if (SystemParams::_seed <= 0)
	//{
	//	std::random_shuffle(ornamentFiles2.begin(), ornamentFiles2.end());
	//}
	//else
	{
		std::mt19937 g(SystemParams::_seed);
		std::shuffle(ornamentFiles2.begin(), ornamentFiles2.end(), g);
	}

	for (unsigned int a = 0; a < ornamentFiles2.size(); a++)
	{
		// enough ???
		if (_ornamentRegions.size() >= _containerWorker->_randomPositions.size()) { break; }

		// is path valid?
		if (ornamentFiles2[a] == "." || ornamentFiles2[a] == "..") { continue; }
		if (!UtilityFunctions::HasEnding(ornamentFiles2[a], ".path")) { continue; }

		std::string fileName = SystemParams::_primary_elem_dir + "\\" + ornamentFiles2[a];
		VFRegion reg = pathIO.LoadRegions(fileName)[0]; // note that one file only has one region
		reg._name = ornamentFiles2[a];
		reg.MakeBoundariesClockwise();
		_ornamentRegions.push_back(reg); /////
	}

	// super mportant for big and small !!!!
	_numBigOnes = _ornamentRegions.size();

	// ---------- small ornaments 1 ----------
	std::vector<std::string> ornamentFiles3 = LoadFiles(SystemParams::_secondary_elem_dir);
	for (unsigned int a = 0; a < ornamentFiles3.size(); a++)
	{
		// is path valid?
		if (ornamentFiles3[a] == "." || ornamentFiles3[a] == "..") { continue; }
		if (!UtilityFunctions::HasEnding(ornamentFiles3[a], ".path")) { continue; }

		std::string fileName = SystemParams::_secondary_elem_dir + "\\" + ornamentFiles3[a];
		VFRegion reg = pathIO.LoadRegions(fileName)[0]; // note that one file only has one region
		reg._name = ornamentFiles3[a];
		reg.MakeBoundariesClockwise();
		_ornamentRegions.push_back(reg);
		//_smallOrnamentRegions1.push_back(reg);
	}
	//std::cout << "_ornamentRegions size = " << _ornamentRegions.size() << "\n";
	//std::cout << "_smallOrnamentRegions1 size = " << _smallOrnamentRegions1.size() << "\n";
}

//void StuffWorker::CalculatePAD()
//{
//}

void StuffWorker::ProcessFocalOrnaments()
{
	//	std::vector<std::vector<AVector>> focals = _region[0].GetFocalBoundaries();
	//	_boundaries.insert(_boundaries.end(), focals.begin(), focals.end());

	//for (unsigned int a = 0; a < _regions.size(); a++)
	//{
	//	std::vector<std::vector<AVector>> fBoundaries = _regions[a].GetFocalBoundaries();
	//	_focals.insert(_focals.end(), fBoundaries.begin(), fBoundaries.end());
	//}
}

// is called from ProcessOrnaments()
void StuffWorker::SkinAndTriangulateOrnaments()
{
	OpenCVWrapper cvWrapper; // for triangulation

	_oriGraphs.clear(); ////// _oriGraph
	int numSz = _ornamentRegions.size();
	if (SystemParams::_create_bad_packing > 0)
	{
		numSz *= 2;
	}
	//for (unsigned int a = 0; a < _ornamentRegions.size(); a++) ////// _ornamentRegions
	for (unsigned int iter = 0; iter < numSz; iter++)
	{
		int a = iter % _ornamentRegions.size(); // index
		VFRegion anOrnament = _ornamentRegions[a]; ////// _ornamentRegions		
		std::vector<std::vector<AVector>> arts = anOrnament.GetBoundaries(); // the actual art

		std::vector<std::vector<AVector>> arts_for_offset = arts;
		std::vector<std::vector<AVector>> focals = anOrnament.GetFocalBoundaries();
		if (focals.size() > 0)
		{
			std::cout << "has braces\n";
			arts_for_offset.insert(arts_for_offset.end(), focals.begin(), focals.end());
		}

		AnElement oriGraph; // yohooo	

		// BAD PACKING? GOOD PACKING?????????? BACKPACKING?
		float addOffset = 0;
		int isBadOffset = 0;
		if (SystemParams::_create_bad_packing > 0)
		{
			int randVal = rand() % 10;
			if (randVal > 8) { addOffset = 50; isBadOffset = 2; }
			else if (randVal > 6) { addOffset = 25; isBadOffset = 1; }
		}
		float skinOffset = SystemParams::_skin_offset + addOffset;

		// skinning
		std::vector<AVector> myOffsetBoundary;
		std::vector<AVector> unionBoundary;
		float img_length = 0;
		oriGraph._oriSkiOffset = skinOffset;
		myOffsetBoundary = ClipperWrapper::RoundOffsettingPP(arts_for_offset, skinOffset)[0];
		//unionBoundary = ClipperWrapper::RoundOffsettingPP(arts, 1)[0]; // hack
		//unionBoundary = ClipperWrapper::RoundOffsettingPP(arts, 0)[0]; // hack
		unionBoundary = ClipperWrapper::RoundOffsettingP(myOffsetBoundary, -skinOffset)[0]; // hack
		ARectangle bb = UtilityFunctions::GetBoundingBox(myOffsetBoundary);
		img_length = bb.witdh;
		if (bb.height > bb.witdh) { img_length = bb.height; }
		AVector centerPt = bb.GetCenter();

		// BAD PACKING? GOOD PACKING?????????? BACKPACKING?
		AVector centerOffset(0, 0);
		if (isBadOffset == 1) { centerOffset += AVector(12, 12); }
		else if (isBadOffset == 2) { centerOffset += AVector(25, 25); }

		// moving to new center
		img_length += 5.0f; // triangulation error without this ?
		AVector newCenter = AVector((img_length / 2.0f), (img_length / 2.0f));
		myOffsetBoundary = UtilityFunctions::MovePoly(myOffsetBoundary, centerPt, newCenter);          // moveee
		unionBoundary = UtilityFunctions::MovePoly(unionBoundary, centerPt, newCenter + centerOffset); // moveee
		for (int a = 0; a < arts.size(); a++)                                                          // moveee
		{
			arts[a] = UtilityFunctions::MovePoly(arts[a], centerPt, newCenter + centerOffset);
		}     // moveee

// random points
		int boundaryPointNum = 0;
		std::vector<AVector> randomPoints;
		_containerWorker->CreatePoints(myOffsetBoundary, img_length, randomPoints, boundaryPointNum);
		//}

		// ---------- not kruskal ----------
		for (int a = 0; a < randomPoints.size(); a++)
		{
			AMass aM(randomPoints[a]);
			aM._idx = a;
			oriGraph._massList.push_back(aM);
		}
		for (int a = 0; a < randomPoints.size(); a++)
		{
			oriGraph._massList[a].CalculateIndicesOfLineSegment(boundaryPointNum);
		}

		oriGraph._skinPointNum = boundaryPointNum;
		oriGraph.CalculateBoundaryEdges();

		//oriGraph.CalculateNNEdges();

		// ---------- triangulation ----------
		//oriGraph._triangles = cvWrapper.Triangulate(randomPoints, myOffsetBoundary, img_length, arts);
		cvWrapper.Triangulate(oriGraph._triangles,
			oriGraph._negSpaceEdges,
			randomPoints,
			myOffsetBoundary,
			img_length,
			arts);



		// ---------- triangle edge  ----------
		oriGraph.CalculateTriangleEdges();

		oriGraph.RemoveShortNegSpaceEdges();

		//std::cout << "oriGraph._negSpaceEdges " << oriGraph._negSpaceEdges.size() << "\n";

		// ---------- arts ----------
		oriGraph._uniArt = unionBoundary;
		oriGraph._arts = arts;
		oriGraph._uniuniArts = ClipperWrapper::GetUniPolys(arts);
		oriGraph.TesselateArts();
		oriGraph.CalculateCentroid();
		oriGraph.CalculateVecToCentroidArray();
		oriGraph.ComputeBarycentric();


		// rigid or not rigid
		if (anOrnament._skeletonLines.size() > 0)
		{
			std::cout << anOrnament._name << " is rigid!\n";
			oriGraph._isRigid = true;
		}

		// ---------- save to a text file ----------
		////size_t lastindex = anOrnament._name.find_last_of(".");       // without extension  
		////std::string rawname = anOrnament._name.substr(0, lastindex); // name
		////std::string graphFilename = SystemParams::_ornament_dir + "\\" + rawname + ".graph"; ////// SystemParams::_ornament_dir
		////PathIO pathIO;
		////pathIO.SaveAGraph(oriGraph, graphFilename); // SAVE !!!
		//}

		// yadda yadda
		oriGraph.UpdateBoundaryAndAvgEdgeLength();

		// ---------- foreground and backgound colors ----------
		//oriGraph._fColors = anOrnament._boundaryFColors; // foreground colors
		//oriGraph._bColors = anOrnament._boundaryBColors; // background colors
		// foreground
		if (anOrnament._boundaryFColorsRGB.size() == 0)
		{
			//std::cout << "duudeF\n";
			std::vector<MyColor> fRGBColors;
			for (unsigned int a = 0; a < anOrnament._boundaryFColors.size(); a++)
			{
				MyColor col;
				if (anOrnament._boundaryFColors[a] >= 0)
				{
					col = ColorPalette::_palette_01[anOrnament._boundaryFColors[a]];
				}
				fRGBColors.push_back(col);
			}
			oriGraph._fColors = fRGBColors;
		}
		else
		{
			oriGraph._fColors = anOrnament._boundaryFColorsRGB;
		}

		// background
		if (anOrnament._boundaryBColorsRGB.size() == 0)
		{
			//std::cout << "duudeB\n";
			std::vector<MyColor> bRGBColors;
			for (unsigned int a = 0; a < anOrnament._boundaryBColors.size(); a++)
			{
				MyColor col;
				if (anOrnament._boundaryBColors[a])
				{
					col = ColorPalette::_palette_01[anOrnament._boundaryBColors[a]];
				}
				bRGBColors.push_back(col);
			}
			oriGraph._bColors = bRGBColors;
		}
		else
		{
			oriGraph._bColors = anOrnament._boundaryBColorsRGB;
		}

		//std::cout << "art size = " << oriGraph._arts.size() << "\n";
		//std::cout << "f size = " << oriGraph._fColors.size() << "\n";
		//std::cout << "b size = " << oriGraph._bColors.size() << "\n\n";

		if (a >= _numBigOnes)  // small element
		{
			_smallOriGraph1.push_back(oriGraph);
		}
		else // big element
		{
			_oriGraphs.push_back(oriGraph);
		}

	} // for (unsigned int iter = 0; iter < numSz; iter++)

	/*
	std::vector<int> randomIndices;
	for (unsigned int a = startIter; a < _graphs.size(); a++) { randomIndices.push_back(a); }
	std::random_shuffle(randomIndices.begin(), randomIndices.end());
	*/

	//if (SystemParams::_seed <= 0)
	//{
	//	std::random_shuffle(_oriGraphs.begin(), _oriGraphs.end());
	//	std::random_shuffle(_smallOriGraph1.begin(), _smallOriGraph1.end());
	//}
	//else
	{
		std::mt19937 g(SystemParams::_seed);
		std::shuffle(_oriGraphs.begin(), _oriGraphs.end(), g);
		std::shuffle(_smallOriGraph1.begin(), _smallOriGraph1.end(), g);
	}
}

// is called from ProcessOrnaments()
void StuffWorker::ProcessSmallOrnaments1()
{
}

//
void StuffWorker::ProcessOrnaments()
{
	////////////////////////////////////////

	SkinAndTriangulateOrnaments();

	////////////////////////////////////////
	for (unsigned int a = 0; a < _oriGraphs.size(); a++)
	{
		_oriGraphs[a].CalculatePAD();
	}

	////////////////////////////////////////

	///// distance transform
	_aDTransform = new ADistanceTransform(_containerWorker->_container_boundaries,
		_containerWorker->_holes,
		_containerWorker->_offsetFocalBoundaries);

	////////////////////////////////////////

	///// ori graphs (save to PNG files)
	for (unsigned int a = 0; a < _oriGraphs.size(); a++)
	{
		AnElement g2 = _oriGraphs[a];
		g2.Scale(SystemParams::_element_initial_scale / g2._scale);
		//std::cout << "save ori graph\n";
		_rr->SaveOriGraph(g2, a); // rr is rigid registration
	}

	// initial placement with shape matching here
	// delete random points

	// disable shape matching
	std::vector<AnElement> matchGraphs;
	std::vector<bool>   oriGraphFlags;


	for (unsigned int a = 0; a < _oriGraphs.size(); a++)
	{
		oriGraphFlags.push_back(false);
	}

	// shape matching
	if (SystemParams::_do_shape_matching)
	{
		//matchGraphs = _containerWorker->PlacementWithMatching3(_oriGraphs);	

		int time1 = glutGet(GLUT_ELAPSED_TIME);

		_containerWorker->PlacementWithMatching3(_oriGraphs, matchGraphs, oriGraphFlags);
		_graphs.insert(_graphs.end(), matchGraphs.begin(), matchGraphs.end());

		int time2 = glutGet(GLUT_ELAPSED_TIME);

		std::cout << "shape matching time = " << time2 - time1 << "\n";


	}
	int match_sz = matchGraphs.size();



	std::cout << "initial elements = " << _graphs.size() << "\n";

	///// positions
	for (unsigned int a = 0; a < _containerWorker->_randomPositions.size(); a++)
	{
		int idx = a % _oriGraphs.size();

		// uncomment this if you want repetition
		if (oriGraphFlags[idx]) { continue; }

		AnElement g = ProcessAnOrnament(_oriGraphs[idx],
			_containerWorker->_randomPositions[a],
			SystemParams::_element_initial_scale,
			a + match_sz/*,
			SystemParams::_max_growth*/);
		g.UpdateBoundaryAndAvgEdgeLength();
		g._ori_id = idx; // keep track the original graph
		g._bigOne = true;
		_graphs.push_back(g);
	}

	////////////////////////////////////////

	///// collision grid
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._skinPointNum; b++) // boundary
		{
			AVector p1 = _graphs[a]._massList[b]._pos;
			_cGrid->InsertAPoint(p1.x, p1.y, a, b);
		}
	}

	///// distance transform
	for (int a = 0; a < _graphs.size(); a++)
	{
		_aDTransform->AddGraph(_graphs[a]);
	}

	for (AnElement& aGraph : _graphs)
	{
		aGraph.CalculateOriAvgEdgeLength();  ///// ori avg adge length
		aGraph.RecalculateArts();            ///// recalculate arts
	}

	///// assign collision grid
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			_graphs[a]._massList[b]._cGrid = _cGrid;
		}
	}

	_numPoints = 0;  ///// num points
	_numTriangles = 0;  ///// num triangles
	_numTriEdges = 0;  ///// num triangle edges
	_numAuxEdges = 0;  ///// num auxiliary edges
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		_numPoints += _graphs[a]._massList.size();       ///// num points
		_numTriangles += _graphs[a]._triangles.size();      ///// num triangles
		_numTriEdges += _graphs[a]._triEdges.size();       ///// num triangle edges
		_numAuxEdges += _graphs[a]._auxiliaryEdges.size(); ///// num auxiliary edges
	}

	// GROWTH DISABLED
	/*
	///// Area
	float sumGraphArea = 0;
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		_graphs[a].UpdateBoundaryAndAvgEdgeLength();
		_graphs[a]._oriArea = _cvWrapper.GetArea(_graphs[a]._skin);
		sumGraphArea += _graphs[a]._oriArea;
	}

	///// container area
	_containerArea = _cvWrapper.GetArea(_boundaries[0]); //B
	for (unsigned int a = 0; a < _offsetFocalBoundaries.size(); a++) //B
	{
		float ar = _cvWrapper.GetArea(_offsetFocalBoundaries[a]); //B
		_containerArea -= ar; //B
	}

	/////  max growth
	float maxGrowth = _containerArea / sumGraphArea;
	SystemParams::_max_growth = maxGrowth;
	*/
}

// not used
void StuffWorker::AddNewSmallElements()
{
	_hasSmallElements = true; // a flag
	_numReplicatedBigOnes = _graphs.size();

	std::vector<AnElement> newGraphs;

	std::cout << "AddNewSmallElements _aDTransform->_peaks.size " << _aDTransform->_peaks.size() << "\n";

	float scaleVal = SystemParams::_element_initial_scale * 0.5;
	for (unsigned int a = 0; a < _aDTransform->_peaks.size(); a++)
	{
		int idx = a % _smallOriGraph1.size();
		AnElement g = ProcessAnOrnament(_smallOriGraph1[idx],
			_aDTransform->_peaks[a],
			scaleVal,
			_graphs.size() + a/*,
			SystemParams::_max_growth*/);
		g._ori_id = idx; // keep track the original graph
		g._bigOne = false;
		newGraphs.push_back(g);
	}

	///// collision grid
	for (unsigned int a = 0; a < newGraphs.size(); a++)
	{
		for (unsigned int b = 0; b < newGraphs[a]._skinPointNum; b++) // boundary
		{
			AVector p1 = newGraphs[a]._massList[b]._pos;
			_cGrid->InsertAPoint(p1.x, p1.y, _graphs.size() + a, b);
		}
	}

	///// distance transform
	// DISABLE THIS BECAUSE FILLING ELEMENTS
	// ARE NOT CONSIDERED FOR EVALUATION
	//for (int a = 0; a < newGraphs.size(); a++)
	//	{ _aDTransform->AddGraph(newGraphs[a]); }

	for (AnElement& aGraph : newGraphs)
	{
		aGraph.CalculateOriAvgEdgeLength();  ///// ori avg adge length
		aGraph.RecalculateArts();            ///// recalculate arts
	}

	///// assign collision grid
	for (unsigned int a = 0; a < newGraphs.size(); a++)
	{
		for (unsigned int b = 0; b < newGraphs[a]._massList.size(); b++)
		{
			newGraphs[a]._massList[b]._cGrid = _cGrid;
		}
	}

	//_numPoints    = 0;    ///// num points
	//_numTriangles = 0; ///// num triangles
	//_numTriEdges  = 0;  ///// num triangle edges
	//_numAuxEdges  = 0;  ///// num auxiliary edges
	for (unsigned int a = 0; a < newGraphs.size(); a++)
	{
		_numPoints += newGraphs[a]._massList.size();       ///// num points
		_numTriangles += newGraphs[a]._triangles.size();      ///// num triangles
		_numTriEdges += newGraphs[a]._triEdges.size();       ///// num triangle edges
		_numAuxEdges += newGraphs[a]._auxiliaryEdges.size(); ///// num auxiliary edges
	}

	std::cout << "newGraphs size = " << newGraphs.size() << "\n";
	std::cout << "SystemParams::_max_growth_small = " << SystemParams::_max_growth_small << "\n";

	// INSERT !!!!
	_graphs.insert(_graphs.end(), newGraphs.begin(), newGraphs.end());

}

void StuffWorker::SaveGraphs()
{
	float dist = 0;
	_rr->Clear();

	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		AnElement g1 = _graphs[a];
		g1.Scale(1.0f / g1._scale);

		AnElement g2;
		if (g1._bigOne) { g2 = _oriGraphs[g1._ori_id]; }
		else { g2 = _smallOriGraph1[g1._ori_id]; }
		g2.Scale(SystemParams::_element_initial_scale / g2._scale);

		dist += _rr->CalculateDistance(g1, g2, a, g1._ori_id);
	}

	_rr->LineUpArts();

	_deformationValue = dist;
	std::cout << "deformation dist = " << dist << "\n";

	//for (unsigned int a = 0; a < _graphs.size(); a++)
	//	{ SaveGraphToPNG(_graphs[a], SystemParams::_save_folder + "DEFORMED_ELEMENTS\\", a); }
}

/*void StuffWorker::SaveGraphToPNG(AGraph oriGraph, std::string saveFolder, int numGraph, float initScale)
{
	AGraph g1 = oriGraph;
	g1.Scale(initScale * 1.0f / g1._scale);

	if (g1._ori_id != -1)
	{
		AGraph g2 = _oriGraphs[g1._ori_id];
		g2.Scale(SystemParams::_element_initial_scale * 1.0f / g2._scale);

		_rr->CalculateDistance(g1, g2, numGraph, g1._ori_id);
	}
}*/

AnElement StuffWorker::ProcessAnOrnament(AnElement oriGraph, AVector pos, float scale, int graphID/*, float maxEdgeLengthFactor*/)
{
	AnElement g1 = oriGraph;

	g1._id = graphID; // identification
	g1.Scale(scale);         // make it smaller
	g1.Move(pos);


	g1.RecalculateEdgeLengths(); // NN EDGES
	g1.RecalculateTriangleEdgeLengths();
	g1.RecalculateArts();

	if (SystemParams::_should_rotate)
	{
		// rotate
		/*float pi = 3.14159265359;
		float xPosNorm = -g1._centroid.x / SystemParams::_upscaleFactor;
		g1.Rotate(xPosNorm * pi);*/
	}

	return g1;
}



void StuffWorker::DragClickedMass(float x, float y)
{
	// clicking is disabled
	/*if (_clickedIndices.first != -1 && _clickedIndices.second != -1)
	{
		_graphs[_clickedIndices.first]._massList[_clickedIndices.second]._pos = AVector(x, y);
		// _graphs[_clickedIndices.first]._massList[_clickedIndices.second]._prevPos = AVector(x, y); // delete
	}*/
}

void StuffWorker::DragClickedGraph(float x, float y)
{
	if (_clickedIndices.first != -1 && _clickedIndices.second != -1)
	{
		//std::cout << "drag\n";

		_graphs[_clickedIndices.first].Move(AVector(x, y));
		_graphs[_clickedIndices.first].UpdateBoundaryAndAvgEdgeLength();
		_graphs[_clickedIndices.first].RecalculateArts();
		//_graphs[_clickedIndices.first]._massList[_clickedIndices.second]._pos = AVector(x, y);
		// _graphs[_clickedIndices.first]._massList[_clickedIndices.second]._prevPos = AVector(x, y); // delete

		/*for (unsigned a = 0; a < _graphs.size(); a++)
		{
			_graphs[a].UpdateBoundaryAndAvgEdgeLength();
			_graphs[a].RecalculateArts();
		}
		_aDTransform->UpdateBoundaries(_graphs);*/
	}
}

void StuffWorker::FindClickedMass(float x, float y)
{
	// clicking is disabled
	AVector pt(x, y);
	float minDist = std::numeric_limits<float>::max();

	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			float d = this->_graphs[a]._massList[b]._pos.Distance(pt);
			if (d < minDist)
			{
				minDist = d;
				_clickedIndices.first = a;
				_clickedIndices.second = b;
			}
		}
	}

	if (minDist > 10.0f)
	{
		_clickedIndices.first = -1;
		_clickedIndices.second = -1;
	}
}

void StuffWorker::DrawClickedMass()
{
	/*if (_clickedIndices.first < 0 || _clickedIndices.first >= _graphs.size())
		{ return; }

	if (_clickedIndices.second < 0 || _clickedIndices.second >= _graphs[_clickedIndices.first]._massList.size())
		{ return; }

	_graphs[_clickedIndices.first]._massList[_clickedIndices.second].Draw(_graphs[_clickedIndices.first]._massList);*/
}

void StuffWorker::Draw()
{
	// NOISE DEACTIVATED
	/*if (SystemParams::_show_noise)
	{
		glColor3f(0.7, 0.7, 0.7);
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (unsigned int a = 0; a < SystemParams::_upscaleFactor; a += 4)
		{
			for (unsigned int b = 0; b < SystemParams::_upscaleFactor; b += 4)
			{
				AVector noiseVector = StuffWorker::_perlinMap[a][b];
				glVertex2f(a, b);
				glVertex2f(a + noiseVector.x * 12.0f, b + noiseVector.y * 12.0f);
			}
		}
		glEnd();
	}*/

	// debug, delete me
	/*glColor3f(1, 0, 1);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (unsigned int a = 0; a < _aDTransform->_peaks.size(); a++)
	{
		AVector pt = _aDTransform->_peaks[a];
		glVertex2f(pt.x, pt.y);
	}
	glEnd();*/

	// Draw Peak
	/*if (!_peakPos.IsInvalid())
	{
		glColor3f(1, 0, 0);
		glPointSize(10.0);
		glBegin(GL_POINTS);
		glVertex2f(_peakPos.x, _peakPos.y);
		glEnd();
	}*/

	/*glColor3f(0.22745098, 0.635294118, 0.858823529);
	glLineWidth(1.0f);
	glBegin(GL_LINES);
	for (int a = 0; a < _squares.size(); a++)
	{
		//_squareGraphs[a].Draw();
		for (int b = 0; b < _squares[a].size(); b++)
		{
			int c = b + 1;
			if (c >= 4) { c = 0; }
			glVertex2f(_squares[a][b].x, _squares[a][b].y);
			glVertex2f(_squares[a][c].x, _squares[a][c].y);
		}
	}
	glEnd();*/

	if (SystemParams::_show_element_boundary)
	{
		//_offsetBoundaries
		glColor3f(0.949019608, 0.501960784, 0.666666667);
		glLineWidth(1.0f);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < _containerWorker->_offsetFocalBoundaries.size(); i++)
		{
			int ofb_sz = _containerWorker->_offsetFocalBoundaries[i].size();
			int b = ofb_sz - 1;
			for (unsigned int a = 0; a < _containerWorker->_offsetFocalBoundaries[i].size(); a++)
			{
				//int b = a + 1;
				//if (b >= _containerWorker->_offsetFocalBoundaries[i].size()) { b = 0; }
				glVertex2f(_containerWorker->_offsetFocalBoundaries[i][b].x, _containerWorker->_offsetFocalBoundaries[i][b].y);
				glVertex2f(_containerWorker->_offsetFocalBoundaries[i][a].x, _containerWorker->_offsetFocalBoundaries[i][a].y);

				b = a;
			}
		}
		glEnd();
	}


	if (SystemParams::_show_elements)
	{
		glColor3f(0.22745098, 0.635294118, 0.858823529);
		glLineWidth(1.0f);
		glBegin(GL_LINES);

		for (unsigned int a = 0; a < _containerWorker->_focals.size(); a++)
		{
			for (unsigned int b = 0; b < _containerWorker->_focals[a].size(); b++)
			{
				for (unsigned int c = 0; c < _containerWorker->_focals[a][b].size(); c++)
				{
					int d = c + 1;
					if (d >= _containerWorker->_focals[a][b].size()) { d = 0; }
					glVertex2f(_containerWorker->_focals[a][b][c].x, _containerWorker->_focals[a][b][c].y);
					glVertex2f(_containerWorker->_focals[a][b][d].x, _containerWorker->_focals[a][b][d].y);
				}

				//int c = b + 1;
				//if (c >= _arts[a].size()) { c = 0; }
				//glVertex2f(_arts[a][b].x, _arts[a][b].y);
				//glVertex2f(_arts[a][c].x, _arts[a][c].y);
			}
		}

		glEnd();
	}


	// ---------- draw container ----------
	_containerWorker->Draw();

	//glColor3f(1, 0, 1);
	//glPointSize(5.0);
	//glBegin(GL_POINTS);
	//for (unsigned int a = 0; a < _aDTransform->_peaks.size(); a++)
	//	{ glVertex2f(_aDTransform->_peaks[a].x, _aDTransform->_peaks[a].y); }
	//glEnd();

	// ---------- draw graphs ----------
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		_graphs[a].Draw();
	}

	if (SystemParams::_show_collission_grid)
	{
		_cGrid->Draw();
	}


	//glLineWidth(3.0f);	
	//glBegin(GL_LINES);	
	/*
	AVector start_ln;
	AVector end_ln;
	AVector rand_pt;
	AVector pt_on_line;
	*/
	//glColor3f(1, 0.1, 0.1);
	//glVertex2f(start_ln.x, start_ln.y);
	//glVertex2f(end_ln.x, end_ln.y);

	//glColor3f(0.1, 0.1, 1);
	//glVertex2f(rand_pt.x, rand_pt.y);
	//glColor3f(0.1, 1, 0.1);
	//glVertex2f(pt_on_line.x, pt_on_line.y);

	//glEnd();
}

void StuffWorker::SaveDataToCSV()
{
	/*std::vector<float> fData = { _totalRepulsionF,  // 1
								 _totalEdgeF,       // 2
								 _totalBoundaryF,   // 3
								 _totalFoldingF,    // 4
								 _totalAttractionF, // 5
								 _maxDist,          // 6
								 _peak_rms,         // 7
								 _fill_ratio,       // 8
								 _fill_rms,         // 9
								 _sumVelocity };   // 10*/
	std::vector<float> fData = { _totalRepulsionF,  // 1
								 _totalEdgeF,       // 2
								 _totalBoundaryF,   // 3
								 _totalOverlapF,    // 4
								 _sumVelocity,      // 5
								 _fill_ratio,       // 6
								 _fill_rms          // 7
	};
	_stuffData.push_back(fData);

	std::stringstream ss;
	ss << SystemParams::_output_folder << "data.csv";
	PathIO pIO;
	pIO.SaveData(_stuffData, ss.str());
}

void StuffWorker::SaveSVG(int frameCounter)
{
	//std::stringstream ss6;
	//ss6 << SystemParams::_save_folder << "SVG\\" << "tri_" << frameCounter << ".svg";
	//MySVGRenderer::SaveTriangles(ss6.str(), _graphs); //B

	// Elements, Skins, and Container
	std::stringstream ss5;
	ss5 << SystemParams::_output_folder << "SVG\\" << "result_" << frameCounter << ".svg";
	MySVGRenderer::SaveElementsAndSkins(ss5.str(),
		_graphs,
		_containerWorker->_focals,
		_containerWorker->_offsetFocalBoundaries,
		_containerWorker->_container_boundaries); //B
}

void StuffWorker::SavePNG(int frameCounter)
{
	//std::cout << "png\n";

	// make sure
	if (!SystemParams::_show_elements)
	{
		for (AnElement& aGraph : _graphs) { aGraph.RecalculateArts(); }
	}

	float imgScale = 4.0f;
	float lineThickness = 2.0f;

	//OpenCVWrapper* cvcv = new OpenCVWrapper;

	// --------- element only ---------
	std::stringstream ss1;
	ss1 << "es_" << frameCounter;

	_pngImg.SetColorImageToWhite();
	// container boundary
	for (unsigned int i = 0; i < _containerWorker->_container_boundaries.size(); i++)
	{
		_cvWrapper.DrawPolyOnCVImage(_pngImg._img, _containerWorker->_container_boundaries[i], MyColor(237, 28, 36), true, 1, imgScale);
	}

	for (unsigned int i = 0; i < _containerWorker->_holes.size(); i++)
	{
		_cvWrapper.DrawPolyOnCVImage(_pngImg._img, _containerWorker->_holes[i], MyColor(237, 28, 36), true, 1, imgScale);
	}

	for (AnElement& aGraph : _graphs)
	{
		// element
		//for (int a = 0; a < aGraph._arts.size(); a++)
		for (int a = aGraph._arts.size() - 1; a >= 0; a--) // backward
		{
			//int fIdx = aGraph._fColors[a];
			//int bIdx = aGraph._bColors[a];

			//background
			//if (bIdx >= 0)
			if (aGraph._bColors[a].IsValid())
			{
				//_cvWrapper.DrawFilledPoly(_pngImg, aGraph._arts[a], ColorPalette::_palette_01[bIdx], imgScale); 
				_cvWrapper.DrawFilledPoly(_pngImg, aGraph._arts[a], aGraph._bColors[a], imgScale);
			}

			// foreground
			//if (fIdx >= 0)
			if (aGraph._fColors[a].IsValid())
			{
				//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, aGraph._arts[a], ColorPalette::_palette_01[fIdx], true, lineThickness, imgScale);
				_cvWrapper.DrawPolyOnCVImage(_pngImg._img, aGraph._arts[a], aGraph._fColors[a], true, lineThickness, imgScale);
			}


			//MyColor fColor = MyColor(57, 139, 203);
			//if (fIdx >= 0) { fColor = ColorPalette::_palette_01[fIdx]; }
			//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, aGraph._arts[a], fColor, true, lineThickness, imgScale);
			//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, aGraph._arts[a], MyColor(57, 139, 203), true, lineThickness, imgScale);
		}

		// skin 242, 128, 170
		//_cvWrapper.DrawPolyOnCVImage(_pngImg._img, aGraph._skin, MyColor(242, 128, 170), true, lineThickness, imgScale);
	}

	//for (unsigned int a = 0; a < _offsetFocalBoundaries.size(); a++)
	//{
	//	// skin 242, 128, 170
	//	_cvWrapper.DrawPolyOnCVImage(_pngImg._img, _offsetFocalBoundaries[a], MyColor(242, 128, 170), true, lineThickness, imgScale);
	//}
	//for (unsigned int a = 0; a < _focals.size(); a++) // focal elements 
	//{
	//	_cvWrapper.DrawPolysOnCVImage(_pngImg._img, _focals[a], MyColor(57, 139, 203), true, lineThickness, imgScale);
	//}

	_pngImg.SaveImage(SystemParams::_output_folder + "PNG\\" + ss1.str() + ".png");

	//delete cvcv;
}

void StuffWorker::RecreateDistanceTransform(float scale)
{
	if (_aDTransform) { delete _aDTransform; }
	//_aDTransform = new ADistanceTransform(_graphs, _boundaries, _offsetFocalBoundaries, scale);
	_aDTransform = new ADistanceTransform(_containerWorker->_container_boundaries,
		_containerWorker->_holes,
		_containerWorker->_offsetFocalBoundaries,
		scale);
	for (int a = 0; a < _graphs.size(); a++)
	{
		_aDTransform->AddGraph(_graphs[a]);
	}

}

void StuffWorker::CalculateSDF(int numIter, bool saveImage)
{
	for (int a = 0; a < _graphs.size(); a++)
	{
		//_graphs[a].RecalculateArts();
		//_graphs[a]._uniuniArts = ClipperWrapper::GetUniPolys(_graphs[a]._arts);
	}

	_aDTransform->UpdateBoundaries(_graphs);

	// ---------- collission grid ----------
	int qtIter = 0;
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		// boundary
		for (unsigned int b = 0; b < _graphs[a]._skinPointNum; b++)
		{
			_cGrid->_objects[qtIter]->_x = _graphs[a]._massList[b]._pos.x;
			_cGrid->_objects[qtIter]->_y = _graphs[a]._massList[b]._pos.y;
			qtIter++;
		}
	}
	// ---------- move points in collission grid ----------
	_cGrid->MovePoints();
	std::cout << "begin to calculate SDF\n";
	_aDTransform->CalculateSDF2(_graphs, _cGrid, numIter, saveImage);
}

void StuffWorker::CalculateFillAndRMS()
{
	for (int a = 0; a < _graphs.size(); a++)
	{
		_graphs[a].RecalculateUniUniArts();
	}

	//_aDTransform->UpdateBoundaries(_graphs);
	//_aDTransform->CalculateFill(_cGrid, _fill_ratio, numIter, saveImage);
	_fill_ratio = ClipperWrapper::CalculateFill(_containerWorker->_container_boundaries[0], _graphs); //  uniuniart

	_fill_ratio_array.push_back(_fill_ratio);
	int sz = SystemParams::_rms_window + 1;
	if (_fill_ratio_array.size() >= sz)
	{
		_fill_rms = 0;
		for (int a = _fill_ratio_array.size() - sz + 1; a < _fill_ratio_array.size(); a++)
		{
			float val = _fill_ratio_array[a] - _fill_ratio_array[a - 1];
			val *= val;
			_fill_rms += val;
		}
		_fill_rms /= (float)SystemParams::_rms_window;
		_fill_rms = std::sqrt(_fill_rms);
	}
}

void StuffWorker::UpdateCollisionGrid_PrepareThreadPool()
{
	// prepare vector
	int len = _cGrid->_squares.size();
	int num_threads = SystemParams::_num_threads;
	int thread_stride = (len + num_threads - 1) / num_threads;


	std::vector<std::thread> t_list;
	for (int a = 0; a < num_threads; a++)
	{
		int startIdx = a * thread_stride;
		int endIdx = startIdx + thread_stride;
		_my_threadpool->submit(&CollissionGrid::PrecomputeGraphIndices_ThreadTask, _cGrid, startIdx, endIdx);
	}
	_my_threadpool->waitFinished(); // sync
}

void StuffWorker::Final_PrepareThreadPool(float dt)
{
	int len = _graphs.size();
	int num_threads = SystemParams::_num_threads;
	int thread_stride = (len + num_threads - 1) / num_threads;

	for (int a = 0; a < num_threads; a++)
	{
		int startIdx = a * thread_stride;
		int endIdx = startIdx + thread_stride;
		//_c_pt_threadpool->submit(work_proc_2, startIdx, endIdx);
		_my_threadpool->submit(&StuffWorker::Final_ThreadTask, this, dt, startIdx, endIdx);
	}

	_my_threadpool->waitFinished(); // sync
}

// a task for a thread
void StuffWorker::Final_ThreadTask(float dt, int startIdx, int endIdx)
{
	for (unsigned int iter = startIdx; iter < endIdx; iter++)
	{
		// make sure...
		if (iter >= _graphs.size()) { break; }

		// ---------- reset forces to zero ----------
		for (unsigned int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			this->_graphs[iter]._massList[b].Init();
		}

		// ---------- get closest point ----------
		for (int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			_graphs[iter]._massList[b].GetClosestPoints2(iter);
		}

		// ---------- solve spring forces ----------
		_graphs[iter].SolveForTriangleSprings();
		_graphs[iter].SolveForNegativeSPaceSprings();

		// ---------- solve other forces ----------
		for (unsigned int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			this->_graphs[iter]._massList[b].Solve(b,
				_graphs[iter],
				_containerWorker->_container_boundaries,
				_containerWorker->_holes,
				_containerWorker->_offsetFocalBoundaries);
		}

		// ---------- integration ----------
		for (unsigned int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			this->_graphs[iter]._massList[b].Simulate(dt);
		}


		//this->_graphs[iter].RecalculateArts();
		// visualization
		//if (SystemParams::_show_elements)
		//{
		//	this->_graphs[iter].RecalculateArts();
		//}
	}
}

void StuffWorker::Finall_ThreadPass(float dt)
{
	int startIter = 0;
	// I'M DISABLING 2ND ELEMENTS
	if (SystemParams::_simulate_2) { startIter = _numReplicatedBigOnes; }

	// ---------- get closest point ----------
	auto start2 = std::chrono::steady_clock::now(); // timing
	Final_PrepareThreadPool(dt);
	auto elapsed2 = std::chrono::steady_clock::now() - start2; // timing
	_closest_pt_thread_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count()); // timing


	// SINGLE THREAD VERSION (COMMENTED)
	/*auto start3 = std::chrono::steady_clock::now(); // timing
	for (int a = startIter; a < _graphs.size(); a++)
	{
		// reset forces
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Init();
		}

		// ---------- get closest point ----------
		for (int b = 0; b < _graphs[a]._massList.size(); b++)
		{

			this->_graphs[a]._massList[b].GetClosestPoints2(a);
		}

		// ---------- solve spring forces ----------
		_graphs[a].SolveForTriangleSprings();
		_graphs[a].SolveForNegativeSPaceSprings();

		// ---------- solve other forces ----------
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Solve(b,
				_graphs[a],
				_containerWorker->_container_boundaries,
				_containerWorker->_holes,
				_containerWorker->_offsetFocalBoundaries);
		}

		// integration
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Simulate(dt);
		}
	}
	auto elapsed3 = std::chrono::steady_clock::now() - start3; // timing
	_closest_pt_cpu_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed3).count()); // timing*/
}

// physics sim
void StuffWorker::Init()
{
	// not used anymore
	/*int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
			{ this->_graphs[a]._massList[b].Init(); }
	}*/
}

/*void StuffWorker::PrepareThreads_01()
{
	_main_iter_01 = -1;
	for (int a = 0; a < SystemParams::_num_thread_c_pt; a++)
	{
		_thread_iters_01.push_back(-1);
	}
}*/



void StuffWorker::SolveSprings_Prepare_Threads()
{
	/*
	int len = _graphs.size();
	int num_threads = SystemParams::_num_thread_springs;
	int thread_stride = (len + num_threads - 1) / num_threads;

	//std::vector<std::thread> t_list;
	for (int a = 0; a < num_threads; a++)
	{
		int startIdx = a * thread_stride;
		int endIdx = startIdx + thread_stride;
		//t_list.push_back(std::thread(&StuffWorker::SolveSprings_Thread, this, startIdx, endIdx));
		_my_threadpool_solve_springs->submit(&StuffWorker::SolveSprings_Thread, this, startIdx, endIdx);
	}
	_my_threadpool_solve_springs->waitFinished();
	*/
}

void StuffWorker::SolveSprings_Thread(int startIdx, int endIdx)
{

	/*
	for (unsigned int iter = startIdx; iter < endIdx; iter++)
	{
		// make sure...
		if (iter >= _graphs.size()) { break; }

		_graphs[iter].SolveForTriangleSprings();
		_graphs[iter].SolveForNegativeSPaceSprings();

		for (unsigned int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			this->_graphs[iter]._massList[b].Solve(b,
											_graphs[iter],
											_containerWorker->_container_boundaries,
											_containerWorker->_holes,
											_containerWorker->_offsetFocalBoundaries);
		}
	}
	*/
}

/*void work_proc_2(int startIdx, int endIdx)
{
	for (unsigned int iter = startIdx; iter < endIdx; iter++)
	{
		// make sure...
		if (iter >= StuffWorker::_graphs.size()) { break; }

		for (int b = 0; b < StuffWorker::_graphs[iter]._massList.size(); b++)
		{
			StuffWorker::_graphs[iter]._massList[b].GetClosestPoints2(iter);
		}

	}
}*/

void StuffWorker::GetClosestPt_Prepare_Threads()
{
	/*
	int len = _graphs.size();
	int num_threads = SystemParams::_num_thread_c_pt;
	int thread_stride = (len + num_threads - 1) / num_threads;

	for (int a = 0; a < num_threads; a++)
	{
		int startIdx = a * thread_stride;
		int endIdx = startIdx + thread_stride;
		//_c_pt_threadpool->submit(work_proc_2, startIdx, endIdx);
		_my_threadpool->submit(&StuffWorker::GetClosestPt_Thread, this, startIdx, endIdx);
	}

	_my_threadpool->waitFinished();
	*/
	/*std::vector<std::thread> t_list;
	for (int a = 0; a < num_threads; a++)
	{
		int startIdx = a * thread_stride;
		int endIdx = startIdx + thread_stride;
		t_list.push_back(std::thread(&StuffWorker::GetClosestPt_Thread, this, startIdx, endIdx));
	}

	for (int a = 0; a < num_threads; a++)
	{
		t_list[a].join();
	}*/


}

void StuffWorker::GetClosestPt_Thread(int startIdx, int endIdx)
{
	/*for (unsigned int iter = startIdx; iter < endIdx; iter++)
	{
		// make sure...
		if (iter >= _graphs.size()) { break; }

		for (int b = 0; b < _graphs[iter]._massList.size(); b++)
		{
			_graphs[iter]._massList[b].GetClosestPoints2(iter);
		}

	}*/
}


// physics sim
void StuffWorker::CalculateThings(float dt)
{
	int startIter = 0;
	// I'M DISABLING 2ND ELEMENTS
	if (SystemParams::_simulate_2) { startIter = _numReplicatedBigOnes; }


	// ---------- collission grid ----------
	// Currently not multithreads
	int qtIter = 0;
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		// boundary
		for (unsigned int b = 0; b < _graphs[a]._skinPointNum; b++)
		{
			_cGrid->_objects[qtIter]->_x = _graphs[a]._massList[b]._pos.x;
			_cGrid->_objects[qtIter]->_y = _graphs[a]._massList[b]._pos.y;
			qtIter++;
		}
	}
	// ---------- move points in collission grid ----------
	// Currently not multithreads
	_cGrid->MovePoints();

	// ---------- collission grid ----------
	auto start1 = std::chrono::steady_clock::now(); // timing
	//_cGrid->PrecomputeData_Prepare_Threads();
	UpdateCollisionGrid_PrepareThreadPool();
	auto elapsed1 = std::chrono::steady_clock::now() - start1; // timing
	_c_grid_thread_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed1).count()); // timing

	// ---------- collission grid SINGLE THREAD ----------
	//auto start0 = std::chrono::steady_clock::now(); // timing
	//_cGrid->PrecomputeGraphIndices();
	//auto elapsed0 = std::chrono::steady_clock::now() - start0; // timing
	//_c_grid_cpu_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed0).count()); // timing

	// ---------- reset growing ----------
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		_graphs[a]._isGrowing = true;
	}

	// for primary elem in 2nd simulation
	if (startIter > 0)
	{
		for (unsigned int a = 0; a < startIter; a++)
		{
			_graphs[a]._isGrowing = false;
		}
	}

	// growing // no multithreading
	float scale_iter = SystemParams::_growth_scale_iter;
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		_graphs[a].Grow(scale_iter, _graphs, dt);
	}

	// bunch of stuff // no multithreading
	_numGrowingElement = 0;
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		if (_graphs[a]._isGrowing) { _numGrowingElement++; }
	}

	// average skin thickness // no multithreading
	_avgSkinThickness = 0;
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		_avgSkinThickness += _graphs[a]._scale * _graphs[a]._oriSkiOffset * SystemParams::_element_initial_scale;
	}
	_avgSkinThickness /= ((float)_graphs.size());
	SystemParams::_skin_thickness = _avgSkinThickness;


	// ---------- update boundary  ----------
	// calculation below needs to finish before closest point queries
	// and it's worthless to be send to threadpool	
	for (int a = startIter; a < _graphs.size(); a++)
	{
		_graphs[a].UpdateBoundaryAndAvgEdgeLength();
	}

	// CODE BELOW IS MOVED TO AlmostAllYourShit()
	// ---------- get closest point ----------
	/*auto start2 = std::chrono::steady_clock::now(); // timing
	GetClosestPt_Prepare_Threads();
	auto elapsed2 = std::chrono::steady_clock::now() - start2; // timing
	_closest_pt_thread_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count()); // timing


	// ---------- get closest point ----------
	auto start3 = std::chrono::steady_clock::now(); // timing
	for (int a = startIter; a < _graphs.size(); a++)
	{
		for (int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].GetClosestPoints2(a);
		}
	}
	auto elapsed3 = std::chrono::steady_clock::now() - start3; // timing
	_closest_pt_cpu_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed3).count()); // timing*/
}

void StuffWorker::Solve()
{
	// not used anymore, see see AlmostAllYourShit()

	/*int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	//std::vector<int> randomIndices;
	//for (unsigned int a = startIter; a < _graphs.size(); a++) { randomIndices.push_back(a); }

	auto start2 = std::chrono::system_clock::now();
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		//int idx = randomIndices[a];
		// ----------  ----------
		_graphs[a].SolveForTriangleSprings();
		_graphs[a].SolveForNegativeSPaceSprings();

		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Solve(b,
				_graphs[a],
				_containerWorker->_container_boundaries,
				_containerWorker->_holes,
				_containerWorker->_offsetFocalBoundaries);
		}
	}
	auto elapsed2 = std::chrono::system_clock::now() - start2; // timing
	_solve_s_cpu_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count()); // timing

	auto start1 = std::chrono::system_clock::now();
	SolveSprings_Prepare_Threads();
	auto elapsed1 = std::chrono::system_clock::now() - start1; // timing
	_solve_s_thread_time.AddTime(std::chrono::duration_cast<std::chrono::microseconds>(elapsed1).count()); // timing*/

}

void StuffWorker::Simulate(float dt)
{
	// not used anymore, see AlmostAllYourShit()


	//int startIter = 0;
	//if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	/*
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		//float dampingVal = _graphs[a]._sumVelocity * SystemParams::_velocity_damping / (float)_graphs[a]._massList.size();
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Simulate(dt);
		}
	}
	*/
}

// velocity verlet - update position
void StuffWorker::UpdatePosition(float dt)
{
	/*float dtdt_half = dt * dt * 0.5f;
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].UpdatePos(dt, dtdt_half);
		}
	}*/
}

// velocity verlet - update velocity
void StuffWorker::UpdateVelocity(float dt)
{
	/*float dt_half = dt * 0.5;
	//float dampingVal = _sumVelocity / ((float)_numPoints) * SystemParams::_velocity_damping;
	float dampingVal = 1.0; // NEED TO SET THIS
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
			{ this->_graphs[a]._massList[b].UpdateVelocity(dt_half, dampingVal); }
	}*/
}


void StuffWorker::Operate(float dt)
{
	// ---------- initialization ----------
	CalculateThings(dt);

	// ---------- main simulation ----------
	Finall_ThreadPass(dt);
	//Init();          // reset forces to zero	
	//UpdatePosition(dt); // velocity verlet
	//Solve();            // calculate forces
	//UpdateVelocity(dt); // velocity verlet
	//Simulate(dt);     // (non-velocity verlet) iterate the masses by the change in time	

	int startIter = 0;
	if (SystemParams::_simulate_2) { startIter = _numReplicatedBigOnes; }

	// ---------- barycentric ----------
	if (SystemParams::_show_elements)
	{
		for (unsigned int a = startIter; a < _graphs.size(); a++)
		{
			this->_graphs[a].RecalculateArts();
		}
	}

	// ---------- position delta ----------
	_sumVelocity = 0;
	_totalRepulsionF = 0.0f;
	_totalEdgeF = 0.0f;
	_totalBoundaryF = 0.0f;
	_totalOverlapF = 0.0f;
	_avgScaleFactor = 0.0f;
	//_totalFoldingF     = 0.0f;
	//_totalAttractionF  = 0.0f;

	// ---------- force data ----------
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		_avgScaleFactor += _graphs[a]._scale;
		_graphs[a].CalculateSumVelocity();
		_sumVelocity += _graphs[a]._sumVelocity;
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			//_positionDelta += _graphs[a]._massList[b]._prevPos.Distance(_graphs[a]._massList[b]._pos); 
			//_positionDelta += _graphs[a]._massList[b]._velocity.Length();			

			float rF = _graphs[a]._massList[b]._repulsionForce.Length();
			float eF = _graphs[a]._massList[b]._edgeForce.Length();
			float bF = _graphs[a]._massList[b]._boundaryForce.Length();
			float oF = _graphs[a]._massList[b]._overlapForce.Length();

			//float aF = _graphs[a]._massList[b]._attractionForce.Length();
			_totalRepulsionF += rF;
			_totalEdgeF += eF;
			_totalBoundaryF += bF;
			_totalOverlapF += oF;
			//_totalFoldingF    += fF;
			//_totalAttractionF += aF;
		}
	}
	//std::cout << "_positionDelta :" << _positionDelta << "\n";
	//std::cout << "_numPoints :" << _numPoints << "\n";
	//std::cout << "\n\n";

	//_avgVelocity /= (float)_numPoints;
}

/*void StuffWorker::InitVerlet()
{
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		this->_graphs[a].InitVerlet();
	}
}*/

void StuffWorker::AnalyzeManualPacking()
{
	// analyze manual packing is disabled


	/*std::cout << "AnalyzeManualPacking\n";

	PathIO pathIO;
	VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];

	_manualElements = reg.GetFocalBoundaries();
	_manualContainer = reg.GetBoundaries();

	_man_neg_ratio = ClipperWrapper::CalculateFill(_manualContainer[0], _manualElements);

	std::cout << "_manualElements = " << _manualElements.size() << "\n";
	std::cout << "_man_neg_ratio = " << _man_neg_ratio << "\n";*/
}

/*void StuffWorker::CreateManualPacking2()
{
	// ---------- load regions ----------
	PathIO pathIO;
	VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];

	// assignments
	_manualElements = reg.GetFocalBoundaries();
	_manualSkeletons = reg.GetFields();
	_manualContainer = reg.GetBoundaries();

	std::cout << "_manualSkeletons.size() " << _manualSkeletons.size() << "\n";

	// prepare things
	std::vector<std::vector<AVector>> focals;
	ADistanceTransform* mDistTransform = new ADistanceTransform(_manualContainer, _containerWorker->_holes, focals, 2.0);
	for (int a = 0; a < _manualElements.size(); a++)
	{
		mDistTransform->AddGraph(_manualElements[a]);
	}

	_manualGrid = new CollissionGrid();
	std::vector<std::vector<AVector>> offsetFocalBoundaries;
	_manualGrid->AnalyzeContainer(_manualContainer, _containerWorker->_holes, offsetFocalBoundaries);
}*/


void StuffWorker::CalculateSkeleton()
{
	_aDTransform->VoronoiSkeleton(_cGrid, _skeletonIter++);
}

void StuffWorker::DrawAccumulationBuffer(CVImg accumulationBuffer, float startColor, float offsetVal, float overlapArea, int numIter)
{
	int img_sz = accumulationBuffer.GetRows();

	CVImg colorBuffer;
	colorBuffer.CreateColorImage(img_sz);
	//CVImg substractImg;
	//substractImg.CreateGrayscaleImage(img_sz);
	//substractImg.SetGrayscaleImageToSomething(startColor);
	for (unsigned int x = 0; x < img_sz; x++)
	{
		for (unsigned int y = 0; y < img_sz; y++)
		{
			int val = accumulationBuffer.GetGrayValue(x, y);

			if (val > startColor + 1)
			{
				//std::cout << val << "\n";
				//accumulationBuffer.SetGrayValue(x, y, 255);

				colorBuffer.SetColorPixel(x, y, MyColor(9, 116, 178));
			}

			else if (val > startColor)
			{
				//std::cout << val << "\n";
				//accumulationBuffer.SetGrayValue(x, y, 20);
				colorBuffer.SetColorPixel(x, y, MyColor(95, 178, 230));
			}
			else
			{
				//std::cout << val << "\n";
				//accumulationBuffer.SetGrayValue(x, y, 0);
				colorBuffer.SetColorPixel(x, y, MyColor(255, 255, 255));
			}
		}
	}

	std::stringstream ss2;
	ss2 << "offset = " << offsetVal;
	_cvWrapper.PutText(colorBuffer._img, ss2.str(), AVector(10, 40), MyColor(0, 0, 0), 1, 2);

	std::stringstream ss1;
	ss1 << "overlap area = " << overlapArea;
	_cvWrapper.PutText(colorBuffer._img, ss1.str(), AVector(10, 70), MyColor(0, 0, 0), 1, 2);

	std::stringstream ss;
	ss << SystemParams::_output_folder << "OVERLAP\\" << "overlap_" << numIter << ".png";
	colorBuffer.SaveImage(ss.str());
	/*accumulationBuffer._img -= startColor;
	for (unsigned int x = 0; x < img_sz; x++)
	{
		for (unsigned int y = 0; y < img_sz; y++)
		{
			int val = accumulationBuffer.GetGrayValue(x, y);

			if (val > startColor)
			{
			}
		}
	}*/
}

void StuffWorker::AddToAccumulationBuffer(std::vector<std::vector<AVector>> elem, CVImg& accumulationBuffer, int startColor, int numIter)
{
	int img_sz = accumulationBuffer.GetRows();
	float scale = img_sz / SystemParams::_upscaleFactor;
	CVImg elementImage;
	elementImage.CreateGrayscaleImage(img_sz);
	elementImage.SetGrayscaleImageToSomething(startColor);

	CVImg shapeImage;
	shapeImage.CreateGrayscaleImage(img_sz);

	for (unsigned int a = 0; a < elem.size(); a++)
	{
		shapeImage.SetGrayscaleImageToBlack();

		_cvWrapper.DrawFilledPolyInt(shapeImage, elem[a], 1, scale);

		// hole (counter clockwise)
		if (!UtilityFunctions::IsClockwise(elem[a]))
		{
			elementImage._img -= shapeImage._img;
			/*for (unsigned int x = 0; x < img_sz; x++)
			{
				for (unsigned int y = 0; y < img_sz; y++)
				{
					int val = accumulationBuffer.GetGrayValue(x, y) - elementImg.GetGrayValue(x, y) ;
					accumulationBuffer.SetGrayValue(x, y, val);
				}
			}*/
		}
		else
		{
			// not hole
			elementImage._img += shapeImage._img;
			/*for (unsigned int x = 0; x < img_sz; x++)
			{
				for (unsigned int y = 0; y < img_sz; y++)
				{
					int val = accumulationBuffer.GetGrayValue(x, y) + elementImg.GetGrayValue(x, y);
					accumulationBuffer.SetGrayValue(x, y, val);
				}
			}*/
		}
	}

	for (unsigned int x = 0; x < img_sz; x++)
	{
		for (unsigned int y = 0; y < img_sz; y++)
		{
			int val = elementImage.GetGrayValue(x, y);
			if (val < startColor)
			{
				accumulationBuffer.SetGrayValue(x, y, accumulationBuffer.GetGrayValue(x, y) - 1);
			}
			else if (val > startColor)
			{
				accumulationBuffer.SetGrayValue(x, y, accumulationBuffer.GetGrayValue(x, y) + 1);
			}
		}
	}

	//std::stringstream ss;
	//ss << SystemParams::_save_folder << "FILL\\" << "element_" << numIter << ".png";
	//accumulationBuffer.SaveImage(ss.str());
}

void StuffWorker::CalculateMetrics()
{
	// parameter
	double maxOffVal = 15.05;
	double offValIter = 0.05;

	bool saveSVGA = false; // elements without offset
	bool saveSVGB = false;  // overlap
	bool saveSVGC = false; // positive space only
	bool saveSVGD = false; // positive space clipped by offset container

	// displaying overlap
	int saveIter = 0;
	int saveIter2 = 0; // delete me debug
	float startColor = 100;
	int img_sz = SystemParams::_upscaleFactor * 2.0f;
	CVImg accumulationBuffer;
	accumulationBuffer.CreateGrayscaleImage(img_sz);

	//accumulationBuffer.SetGrayscaleImageToBlack();

	OpenCVWrapper cvWRap;
	double containerArea = cvWRap.GetAreaOriented(_manualContainer[0]);
	std::cout << "containerArea = " << containerArea << "\n";

	// 1 - func-ception
	//std::vector< std::vector<AVector>> offsetElements1 = ClipperWrapper::OffsetAll(ClipperWrapper::OffsetAll(_manualElements, preOffset), -preOffset);
	std::vector< std::vector<AVector>> offsetElements1 = _manualElements;
	// debug (comment me)
	if (saveSVGA)
	{
		std::stringstream ss1;
		ss1 << SystemParams::_output_folder << "SVG\\" << "debugA.svg";
		MySVGRenderer::SaveShapesToSVG(ss1.str(), offsetElements1);
	}
	std::vector<double> _offsetVals2;
	std::vector<double> _offsetVals3;
	std::vector<double> _negVals;

	for (double offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)
	{ // begin for

		// accumulation
		accumulationBuffer.SetGrayscaleImageToSomething(startColor);

		// 2 - Generate offset elements one by one
		double area2 = 0;
		//float area2b = 0;
		std::vector< std::vector<AVector>> offsetElements2;
		for (unsigned int a = 0; a < _manualElementsss.size(); a++)
		{
			/* clockwise = element
			   counterclockwise = hole */

			   // float offVal = offVal2;
			   //if (!UtilityFunctions::IsClockwise(_manualElementsss[a][b])) { offVal = -offVal2; }

			std::vector<std::vector<AVector>> outputPolys1 = ClipperWrapper::OffsetAll(_manualElementsss[a], offVal);

			float tempArea;
			std::vector<std::vector<AVector>> outputPolys2 = ClipperWrapper::ClipElementsWithContainer(outputPolys1, _manualContainer[0], tempArea);
			//if (!UtilityFunctions::IsClockwise(_manualElementsss[a][b])) { tempArea = -tempArea; }
			area2 += tempArea;
			offsetElements2.insert(offsetElements2.end(), outputPolys2.begin(), outputPolys2.end());

			// accumulation buffer
			AddToAccumulationBuffer(outputPolys2, accumulationBuffer, startColor, saveIter2++);

			// AREA2B
			//for (unsigned int b = 0; b < outputPolys2.size(); b++)
			//{ area2b += cvWRap.GetAreaOriented(outputPolys2[b]); }

		}
		//std::cout << "area2 = " << area2 << ", area2b=" << area2b << ", ";
		//std::cout << "b " << std::abs(area2 - area2b) << ", ";

		// draw
		if (saveSVGB)
		{
			std::stringstream ss2;
			ss2 << SystemParams::_output_folder << "SVG\\" << "debugB_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss2.str(), offsetElements2);
		}
		// area
		_offsetVals2.push_back(area2);

		// 3 - Generate offset of union of elements
		std::vector< std::vector<AVector>> offsetElements3_temp = ClipperWrapper::OffsetAll(offsetElements1, offVal);
		float area3 = 0; // area of the entire elements
		std::vector<std::vector<AVector>> offsetElements3 = ClipperWrapper::ClipElementsWithContainer(offsetElements3_temp, _manualContainer[0], area3);

		//float area3b = 0;
		//for (unsigned int b = 0; b < offsetElements3.size(); b++)
		//{ area3b += cvWRap.GetAreaOriented(offsetElements3[b]); }

		// draw
		if (saveSVGC)
		{
			std::stringstream ss3;
			ss3 << SystemParams::_output_folder << "SVG\\" << "debugC_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss3.str(), offsetElements3);
		}
		// area
		_offsetVals3.push_back(area3);

		// 4 - offset of union of elements minus offset of container
		// this for SCP
		std::vector<AVector> offset_container = ClipperWrapper::RoundOffsettingP(_manualContainer[0], -offVal)[0];
		//std::vector<AVector> offset_container = ClipperWrapper::RoundOffsettingP(_manualContainer[0], SystemParams::_container_offset)[0];
		double offContainerArea = cvWRap.GetAreaOriented(offset_container); // area of the offset container
		float area4 = 0; // area of the entire elements
		std::vector<std::vector<AVector>> offsetElements4 = ClipperWrapper::ClipElementsWithContainer(offsetElements3_temp, offset_container, area4);

		_negVals.push_back((offContainerArea - area4) / containerArea); // IS THIS CORRECT ? ratio of neg space using offset container. YES


		// draw
		if (saveSVGD)
		{
			std::stringstream ss4;
			ss4 << SystemParams::_output_folder << "SVG\\" << "debugD_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss4.str(), offsetElements4);
		}

		// draw
		DrawAccumulationBuffer(accumulationBuffer, startColor, offVal, area2 - area3, saveIter++);

		std::cout << offVal << "\n";
		//std::cout << offContainerArea << "\n";
		//std::cout << offVal << " --> " << area2 - area3 << "\n";

	} // end for (float offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)

	PathIO pIO;
	pIO.SaveSDF2CSV(_offsetVals2, SystemParams::_output_folder + "dist_2.csv"); // for overlap metric
	pIO.SaveSDF2CSV(_offsetVals3, SystemParams::_output_folder + "dist_3.csv"); // for overlap metric
	pIO.SaveSDF2CSV(_negVals, SystemParams::_output_folder + "dist_4.csv"); // for scp

}

void StuffWorker::CalculateMetrics3()
{
	// parameter
	double maxOffVal = 16;
	double offValIter = 0.1;

	bool saveOriSVG = false; // original elements without offset
	bool saveOverlapSVG = false;  // overlap
	bool saveEmptySVG = false; // positive space only
	bool saveSCPSVG = false; // positive space clipped by offset container

	std::vector<double> overlap_area_array;
	std::vector<double> empty_area_array;
	std::vector<double> scp_array;

	OpenCVWrapper cvWRap;
	double containerArea = cvWRap.GetAreaOriented(_manualContainer[0]);
	std::cout << "containerArea = " << containerArea << "\n";

	// 1 -
	std::vector< std::vector<AVector>> ori_elements = _manualElements;

	for (double offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)
	{

		std::vector< std::vector<AVector>> allElements_uni_not_clipped = ClipperWrapper::OffsetAll(ori_elements, offVal);
		float area = 0;
		std::vector<std::vector<AVector>> offsetElements4 = ClipperWrapper::ClipElementsWithContainer(allElements_uni_not_clipped, _manualContainer[0], area);

		std::stringstream ss5;
		ss5 << SystemParams::_output_folder << "SVG\\" << "offset_" << offVal << ".svg";
		MySVGRenderer::SaveShapesToSVG(ss5.str(), offsetElements4);

		std::cout << offVal << "\n";
		//std::cout << offContainerArea << "\n";
		//std::cout << offVal << " --> " << area2 - area3 << "\n";

	} // end for (float offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)

}

void StuffWorker::CalculateMetrics2()
{
	// parameter
	double maxOffVal = 16;
	double offValIter = 0.1;

	bool saveOriSVG = false; // original elements without offset
	bool saveOverlapSVG = false;  // overlap
	bool saveEmptySVG = false; // positive space only
	bool saveSCPSVG = false; // positive space clipped by offset container

	std::vector<double> overlap_area_array;
	std::vector<double> empty_area_array;
	std::vector<double> scp_array;

	OpenCVWrapper cvWRap;
	double containerArea = cvWRap.GetAreaOriented(_manualContainer[0]);
	std::cout << "containerArea = " << containerArea << "\n";

	// 1 -
	std::vector< std::vector<AVector>> ori_elements = _manualElements;
	// debug (comment me)
	if (saveOriSVG)
	{
		std::stringstream ss1;
		ss1 << SystemParams::_output_folder << "SVG\\" << "debugA.svg";
		MySVGRenderer::SaveShapesToSVG(ss1.str(), ori_elements);
	}

	std::vector<std::vector<AVector>> overlapPolys_notUni;
	std::vector<std::vector<AVector>> overlapPolys_Uni;
	for (double offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)
	{ // begin for
		// precompute offset elements
		std::vector<GraphArt> offset_elements;
		for (unsigned int a = 0; a < _manualElementsss.size(); a++)
		{
			std::vector<std::vector<AVector>> outputPolysA = ClipperWrapper::OffsetAll(_manualElementsss[a], offVal);
			float tempArea2; // this does nothing
			std::vector<std::vector<AVector>> outputPolysB = ClipperWrapper::ClipElementsWithContainer(outputPolysA, _manualContainer[0], tempArea2);
			offset_elements.push_back(outputPolysB);
		}


		// 2 - overlap
		for (unsigned int a = 0; a < offset_elements.size() - 1; a++)
		{
			/* clockwise = element
			counterclockwise = hole */

			for (unsigned int b = a + 1; b < offset_elements.size(); b++)
			{
				float tempArea; // this does nothing
				std::vector<std::vector<AVector>> outputPolys1 = ClipperWrapper::ClipElementsWithElements(offset_elements[a], offset_elements[b], tempArea);
				overlapPolys_notUni.insert(overlapPolys_notUni.end(), outputPolys1.begin(), outputPolys1.end());
			}
		}
		float overlapArea;
		overlapPolys_Uni = ClipperWrapper::GetUniPolys(overlapPolys_notUni, overlapArea);
		overlap_area_array.push_back(overlapArea);

		// draw
		if (saveOverlapSVG)
		{
			std::stringstream ss2;
			ss2 << SystemParams::_output_folder << "SVG\\" << "overlap_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss2.str(), overlapPolys_Uni);
		}
		// area
		//_offsetVals2.push_back(area2);

		// 3 - empty poly
		float all_elem_area;
		std::vector< std::vector<AVector>> allElements_uni_not_clipped = ClipperWrapper::OffsetAll(ori_elements, offVal);
		//float area_temp = 0; // does nothing
		//std::vector<std::vector<AVector>> allElements_uni_clipped = ClipperWrapper::ClipElementsWithContainer(allElements_uni_not_clipped, _manualContainer[0], area_temp);
		float empty_area;
		std::vector<std::vector<AVector>> empty_polys = ClipperWrapper::DiffContainerWithElements(allElements_uni_not_clipped, _manualContainer[0], empty_area);
		empty_area_array.push_back(empty_area);

		// draw
		if (saveEmptySVG)
		{
			std::stringstream ss3;
			ss3 << SystemParams::_output_folder << "SVG\\" << "empty_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss3.str(), empty_polys);
		}
		// area
		//_offsetVals3.push_back(area3);

		// 4 - offset of union of elements minus offset of container
		// this for SCP
		std::vector<AVector> offset_container = ClipperWrapper::RoundOffsettingP(_manualContainer[0], -offVal)[0];
		double offContainerArea = cvWRap.GetAreaOriented(offset_container); // area of the offset container
		float area4 = 0; // area of the entire elements
		std::vector<std::vector<AVector>> offsetElements4 = ClipperWrapper::ClipElementsWithContainer(allElements_uni_not_clipped, offset_container, area4);

		scp_array.push_back((offContainerArea - area4) / containerArea); // IS THIS CORRECT ? ratio of neg space using offset container. YES


		// draw
		if (saveSCPSVG)
		{
			std::stringstream ss4;
			ss4 << SystemParams::_output_folder << "SVG\\" << "scp_" << offVal << ".svg";
			MySVGRenderer::SaveShapesToSVG(ss4.str(), offsetElements4);
		}

		std::stringstream ss5;
		ss5 << SystemParams::_output_folder << "SVG\\" << "offset_" << offVal << ".svg";
		MySVGRenderer::SaveShapesToSVG(ss5.str(), allElements_uni_not_clipped);

		std::cout << offVal << "\n";
		//std::cout << offContainerArea << "\n";
		//std::cout << offVal << " --> " << area2 - area3 << "\n";

	} // end for (float offVal = 0.0f; offVal < maxOffVal; offVal += offValIter)

	PathIO pIO;
	pIO.SaveSDF2CSV(overlap_area_array, SystemParams::_output_folder + "overlap.csv"); // for overlap metric
	pIO.SaveSDF2CSV(empty_area_array, SystemParams::_output_folder + "empty.csv"); // for overlap metric
	pIO.SaveSDF2CSV(scp_array, SystemParams::_output_folder + "scp.csv"); // for scp

}



void StuffWorker::AnalyzeFinishedPacking()
{
	// _manualElementsss && _manualElements
	for (unsigned int a = 0; a < _numBigOnes; a++)
	{
		_manualElementsss.push_back(_graphs[a]._uniuniArts);
		_manualElements.insert(_manualElements.end(), _manualElementsss[a].begin(), _manualElementsss[a].end());
	}

	// find container
	//for (unsigned int a = 0; a < regs.size(); a++)
	//{
	//	std::vector<std::vector<AVector>> temp = regs[a].GetBoundaries();
	//	if (temp.size() > 0)
		//{
			//_manualContainer = temp;
		//}
	//}
	_manualContainer = _containerWorker->_container_boundaries;

	// ----
	// HERE
	// ----
	CalculateMetrics();
}

void StuffWorker::CreateManualPacking2()
{
	//PathIO pathIO;
	//std::vector<VFRegion> regs = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path");
	//
	//// _manualElementsss && _manualElements
	//for (unsigned int a = 0; a < regs.size(); a++)
	//{ 
	//	_manualElementsss.push_back(regs[a].GetFocalBoundaries()); 
	//	_manualElements.insert(_manualElements.end(), _manualElementsss[a].begin(), _manualElementsss[a].end());
	//}

	//// find container
	//for (unsigned int a = 0; a < regs.size(); a++)
	//{
	//	std::vector<std::vector<AVector>> temp = regs[a].GetBoundaries();
	//	if (temp.size() > 0)
	//		{ _manualContainer = temp; }
	//}

	///*VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];

	//// assignments
	//_manualElements = reg.GetFocalBoundaries();
	//std::cout << "_manualElements.size() = " << _manualElements.size() << "\n";
	//for (unsigned int a = 0; a < _manualElements.size(); a++)
	//{
	//	GraphArt g;
	//	g.push_back(_manualElements[a]);
	//	_manualElementsss.push_back(g);
	//}

	////_manualSkeletons = reg.GetFields();
	//_manualContainer = reg.GetBoundaries(); // target container
	//*/

	//// ----
	//// HERE
	//// ----
	//CalculateMetrics();
}

/*
CollissionGrid*                   _manualGrid;
std::vector<std::vector<AVector>> _manualElements;
std::vector<std::vector<AVector>> _manualSkeletons;
std::vector<std::vector<AVector>> _manualContainer;
*/
void StuffWorker::CreateManualPacking()
{
	//	// ---------- load regions ----------
	//	PathIO pathIO;
	//	VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];
	//	   
	//	// assignments
	//	_manualElements = reg.GetFocalBoundaries();
	//	//_manualElements = ClipperWrapper::OffsetAll( ClipperWrapper::OffsetAll( reg.GetFocalBoundaries(), preOffset), -preOffset); // the actual elements	
	//	_manualSkeletons = reg.GetFields();
	//	_manualContainer =  reg.GetBoundaries(); // target container
	//
	//	// ----
	//	// HERE
	//	// ----
	//	//CalculateMetrics();
	//
	//	/*
	//	std::stringstream ss5;
	//	ss5 << SystemParams::_save_folder << "SVG\\" << "result_" << frameCounter << ".svg";
	//	MySVGRenderer::SaveElementsAndSkins(ss5.str(), 
	//		                                _graphs, 
	//		                                _containerWorker->_focals, 
	//		                                _containerWorker->_offsetFocalBoundaries, 
	//		                                _containerWorker->_container_boundaries); //B
	//	*/
	//	/*std::vector<std::vector<AVector>> offsetShapes = ClipperWrapper::OffsetAll(_manualElements, 5.746f);
	//	std::stringstream ss5;
	//	ss5 << SystemParams::_save_folder << "SVG\\" << "offset.svg";
	//	MySVGRenderer::SaveShapesToSVG(ss5.str(), offsetShapes);*/
	//		
	//	// use this for skeletons
	//	//float offVal = 5.0f;
	//	//std::vector<std::vector<AVector>> temp1 = ClipperWrapper::RoundOffsettingPP(reg.GetBoundaries(), offVal);
	//	//_manualContainer = ClipperWrapper::RoundOffsettingPP(temp1, -offVal);
	//	//_manualContainer = temp1;
	//	// -------------------
	//
	//
	//	std::cout << "_manualSkeletons.size() " << _manualSkeletons.size() << "\n";
	//
	//	// prepare things
	//	std::vector<std::vector<AVector>> focals;
	//	ADistanceTransform* mDistTransform = new ADistanceTransform(_manualContainer, _containerWorker->_holes, focals, 2.0);
	//	for (int a = 0; a < _manualElements.size(); a++)
	//	{
	//		mDistTransform->AddGraph(_manualElements[a]);
	//	}
	//
	//	_manualGrid = new CollissionGrid();
	//	std::vector<std::vector<AVector>> offsetFocalBoundaries;
	//	_manualGrid->AnalyzeContainer(_manualContainer, _containerWorker->_holes, offsetFocalBoundaries);
	//
	//	// assign to c grid
	//	for (unsigned int a = 0; a < _manualElements.size(); a++)
	//	{
	//		// boundary
	//		for (unsigned int b = 0; b < _manualElements[a].size(); b++)
	//			{ _manualGrid->InsertAPoint(_manualElements[a][b].x, _manualElements[a][b].y, a, b); }
	//	}
	//
	//	float m_fill_ratio = 0;
	//	mDistTransform->_manualSkeletons = _manualSkeletons;
	//	mDistTransform->CalculateFill(_manualGrid, m_fill_ratio, 0, true);
	//	mDistTransform->CalculateSDF1(_manualGrid, 0, true);
	//
	//	delete mDistTransform;
}

/*void StuffWorker::CreateSquares()
{
	//float sqSize = 45.5f; // 80
	//float spaceGap = 5.0; // 25
	float sqSize = 80.0f; // 80
	float spaceGap = 25.0f; // 25
	float sz = SystemParams::_upscaleFactor;

	for (float xPos = 0.0f; xPos <= sz; xPos += sqSize + spaceGap)
	{
		for (float yPos = 0.0f; yPos <= sz; yPos += sqSize + spaceGap)
		{
			std::vector<AVector> art;
			art.push_back(AVector(xPos, yPos));
			art.push_back(AVector(xPos + sqSize, yPos));
			art.push_back(AVector(xPos + sqSize, yPos + sqSize));
			art.push_back(AVector(xPos, yPos + sqSize));

			_squares.push_back(art);
			//AGraph g;
			//g._uniArt = art;
			//_squareGraphs.push_back(g);
		}
	}

	std::vector<AVector> container;
	container.push_back(AVector(0, 0));
	container.push_back(AVector(0 + sz, 0));
	container.push_back(AVector(0 + sz, 0 + sz));
	container.push_back(AVector(0, 0 + sz));
	_squareContainer.push_back(container);

	std::vector<std::vector<AVector>> focals;

	ADistanceTransform* sqDistTransform = new ADistanceTransform(_squares, _squareContainer, focals, 2);
	_sqCGrid = new CollissionGrid();

	std::vector<std::vector<AVector>> offsetFocalBoundaries;
	_sqCGrid->AnalyzeContainer(_squareContainer, offsetFocalBoundaries);

	for (unsigned int a = 0; a < _squares.size(); a++)
	{
		// boundary
		for (unsigned int b = 0; b < _squares[a].size(); b++)
			{ _sqCGrid->InsertAPoint(_squares[a][b].x, _squares[a][b].y, a, b); }
	}

	float sq_fill_ratio = 0;
	sqDistTransform->CalculateFill(_sqCGrid, sq_fill_ratio, 0, true);
	sqDistTransform->CalculateSDF(_sqCGrid, 0, true);

	//delete cGrid;
	delete sqDistTransform;

}*/

AVector StuffWorker::GetPerlinVector(int x, int y)
{
	FastNoise myNoise1(1111); // Create a FastNoise object
	myNoise1.SetNoiseType(FastNoise::Perlin); // Set the desired noise type

	FastNoise myNoise2(2222); // Create a FastNoise object
	myNoise2.SetNoiseType(FastNoise::Perlin); // Set the desired noise type

	AVector noiseVector;
	noiseVector.x = myNoise1.GetNoise(x, y);
	noiseVector.y = myNoise2.GetNoise(x, y);

	noiseVector = noiseVector.Norm();

	noiseVector.Print();

	return noiseVector;
}

void StuffWorker::ComputePerlinMap(int t)
{
}