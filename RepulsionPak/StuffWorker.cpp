
#include "StuffWorker.h"

#include "CGALTriangulation.h"
//#include "PoissonGenerator.h"
#include "Kruskal.h"
#include "ARectangle.h"
#include "AGraph.h"

#include "MySVGRenderer.h"

#include "glew.h"
#include "freeglut.h"
#include "ClipperWrapper.h"

#include "ColorPalette.h"

#include "CollissionGrid.h"

#include "FastNoise.h"

#include "PAD.h"

#include <time.h> // time seed
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <random>

// static stuff
std::vector<std::vector<AVector>> StuffWorker::_perlinMap = std::vector<std::vector<AVector>>();
std::vector<AGraph>  StuffWorker::_graphs = std::vector<AGraph>();
CollissionGrid* StuffWorker::_cGrid = 0;

// constructor
StuffWorker::StuffWorker()
{
	_skeletonIter = 0;

	_man_neg_ratio = -1.0;

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
	_clickedIndices.first  = -1;
	_clickedIndices.second = -1;

	// ----------  ----------
	_avgSkinThickness = 0;

	_numGrowingElement = 0;
	_numPoints         = 0;
	_sumVelocity       = 0.0f;
	_totalRepulsionF   = 0.0f;
	_totalEdgeF        = 0.0f;
	_totalBoundaryF    = 0.0f;
	_totalOverlapF     = 0.0f;
	_avgScaleFactor = 0.0f;

	_aDTransform = 0;
	//_peakPos = AVector();
	//_maxDist = 0;

	_fill_ratio = -5.0;
	_fill_rms = 0;
	//_maxDist = 0;
	//_peak_rms = 0;	
	//_allStopGrow = true;

	//CVImg         _pngImg;
	float imgScale = 4.0f;
	_pngImg.CreateColorImage(SystemParams::_upscaleFactor * imgScale);
	_pngImg.SetColorImageToWhite();
	//OpenCVWrapper _cvWrapper;

	//CreateSquares();       // don't forget to change params.lua
	CreateManualPacking2();  // overlap metrics
	//CreateManualPacking(); // SDF and stuff
	//AnalyzeManualPacking();


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
		{ pMap.push_back(AVector()); }
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
}

//void StuffWorker::InitStatic()
//{
//}

StuffWorker::~StuffWorker()
{
	//if (_cGrid)   { delete _cGrid; }
	if (_containerWorker) { delete _containerWorker; }
	if (_sqCGrid)         { delete _sqCGrid; }
	if (_manualGrid)      { delete _manualGrid; }
	if (_rr)              { delete _rr; }
	if (_aDTransform)     { delete _aDTransform; }
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
	DIR *dp;
	struct dirent *ep;
	/*---------- open directory ----------*/
	dp = opendir(directoryPath.c_str());
	if (dp != NULL)
	{
		while (ep = readdir(dp)) { fileStr.push_back(ep->d_name); }
		(void)closedir(dp);
	}	else { perror("Couldn't open the directory"); }

	return fileStr;
}

// ornaments
void StuffWorker::LoadOrnaments()
{
	std::cout << "ProcessOrnaments - Normal\n";

	// ---------- load ornaments ----------
	PathIO pathIO;

	// ---------- regular ornaments ----------
	std::vector<std::string> ornamentFiles2 = LoadFiles(SystemParams::_ornament_dir); ////

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

		std::string fileName = SystemParams::_ornament_dir + "\\" + ornamentFiles2[a];
		VFRegion reg = pathIO.LoadRegions(fileName)[0]; // note that one file only has one region
		reg._name = ornamentFiles2[a];
		reg.MakeBoundariesClockwise();
		_ornamentRegions.push_back(reg); /////
	}

	// super mportant for big and small !!!!
	_numBigOnes = _ornamentRegions.size();

	// ---------- small ornaments 1 ----------
	std::vector<std::string> ornamentFiles3 = LoadFiles(SystemParams::_ornament_dir_small_1);
	for (unsigned int a = 0; a < ornamentFiles3.size(); a++)
	{
		// is path valid?
		if (ornamentFiles3[a] == "." || ornamentFiles3[a] == "..") { continue; }
		if (!UtilityFunctions::HasEnding(ornamentFiles3[a], ".path")) { continue; }

		std::string fileName = SystemParams::_ornament_dir_small_1 + "\\" + ornamentFiles3[a];
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
		{ numSz *= 2; }
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
				
		AGraph oriGraph; // yohooo	

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
		ARectangle bb    = UtilityFunctions::GetBoundingBox(myOffsetBoundary);
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
			{ arts[a] = UtilityFunctions::MovePoly(arts[a], centerPt, newCenter + centerOffset); }     // moveee
		
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
					{ col = ColorPalette::_palette_01[anOrnament._boundaryFColors[a]]; }
				fRGBColors.push_back(col);
			}
			oriGraph._fColors = fRGBColors;
		}
		else
			{ oriGraph._fColors = anOrnament._boundaryFColorsRGB; }

		// background
		if (anOrnament._boundaryBColorsRGB.size() == 0)
		{
			//std::cout << "duudeB\n";
			std::vector<MyColor> bRGBColors;
			for (unsigned int a = 0; a < anOrnament._boundaryBColors.size(); a++)
			{
				MyColor col;
				if (anOrnament._boundaryBColors[a])
					{ col = ColorPalette::_palette_01[anOrnament._boundaryBColors[a]]; }
				bRGBColors.push_back(col);
			}
			oriGraph._bColors = bRGBColors;
		}
		else
			{ oriGraph._bColors = anOrnament._boundaryBColorsRGB; }

		//std::cout << "art size = " << oriGraph._arts.size() << "\n";
		//std::cout << "f size = " << oriGraph._fColors.size() << "\n";
		//std::cout << "b size = " << oriGraph._bColors.size() << "\n\n";

		if (a >= _numBigOnes)  // small element
			{  _smallOriGraph1.push_back(oriGraph); }
		else // big element
			{ _oriGraphs.push_back(oriGraph); }

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
		{ _oriGraphs[a].CalculatePAD(); }

	////////////////////////////////////////

	///// distance transform
	_aDTransform = new ADistanceTransform(_containerWorker->_container_boundaries, 
		                                  _containerWorker->_holes, 
		                                  _containerWorker->_offsetFocalBoundaries);
	
	////////////////////////////////////////

	///// ori graphs (save to PNG files)
	for (unsigned int a = 0; a < _oriGraphs.size(); a++)
	{
		AGraph g2 = _oriGraphs[a];
		g2.Scale(SystemParams::_element_initial_scale / g2._scale);
		//std::cout << "save ori graph\n";
		_rr->SaveOriGraph(g2, a); // rr is rigid registration
	}

	// initial placement with shape matching here
	// delete random points
	
	// disable shape matching
	std::vector<AGraph> matchGraphs;
	std::vector<bool>   oriGraphFlags;


	for (unsigned int a = 0; a < _oriGraphs.size(); a++)
	{
		oriGraphFlags.push_back(false);
	}

	// shape matching
	if(SystemParams::_do_shape_matching)
	{
		//matchGraphs = _containerWorker->PlacementWithMatching3(_oriGraphs);	
		_containerWorker->PlacementWithMatching3(_oriGraphs, matchGraphs, oriGraphFlags);
		_graphs.insert(_graphs.end(), matchGraphs.begin(), matchGraphs.end());	
	}
	int match_sz = matchGraphs.size();



	std::cout << "initial elements = " << _graphs.size() << "\n";

	///// positions
	for (unsigned int a = 0; a < _containerWorker->_randomPositions.size(); a++)
	{
		int idx = a % _oriGraphs.size();

		if (oriGraphFlags[idx]) { continue; }

		AGraph g = ProcessAnOrnament(_oriGraphs[idx],
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
		{ _aDTransform->AddGraph(_graphs[a]); }
	
	for (AGraph& aGraph : _graphs)  
	{ 
		aGraph.CalculateOriAvgEdgeLength();  ///// ori avg adge length
		aGraph.RecalculateArts();            ///// recalculate arts
	}

	///// assign collision grid
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
			{ _graphs[a]._massList[b]._cGrid = _cGrid; }
	}
	
	_numPoints    = 0;  ///// num points
	_numTriangles = 0;  ///// num triangles
	_numTriEdges  = 0;  ///// num triangle edges
	_numAuxEdges  = 0;  ///// num auxiliary edges
	for (unsigned int a = 0; a < _graphs.size(); a++)
	{ 
		_numPoints    += _graphs[a]._massList.size();       ///// num points
		_numTriangles += _graphs[a]._triangles.size();      ///// num triangles
		_numTriEdges  += _graphs[a]._triEdges.size();       ///// num triangle edges
		_numAuxEdges  += _graphs[a]._auxiliaryEdges.size(); ///// num auxiliary edges
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

	std::vector<AGraph> newGraphs;

	std::cout << "AddNewSmallElements _aDTransform->_peaks.size " << _aDTransform->_peaks.size() << "\n";

	float scaleVal = SystemParams::_element_initial_scale * 0.5;
	for (unsigned int a = 0; a < _aDTransform->_peaks.size(); a++)
	{
		int idx = a % _smallOriGraph1.size();
		AGraph g = ProcessAnOrnament(_smallOriGraph1[idx],
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
	
	for (AGraph& aGraph : newGraphs)
	{ 
		aGraph.CalculateOriAvgEdgeLength();  ///// ori avg adge length
		aGraph.RecalculateArts();            ///// recalculate arts
	}

	///// assign collision grid
	for (unsigned int a = 0; a < newGraphs.size(); a++)
	{
		for (unsigned int b = 0; b < newGraphs[a]._massList.size(); b++)
			{ newGraphs[a]._massList[b]._cGrid = _cGrid; }
	}

	//_numPoints    = 0;    ///// num points
	//_numTriangles = 0; ///// num triangles
	//_numTriEdges  = 0;  ///// num triangle edges
	//_numAuxEdges  = 0;  ///// num auxiliary edges
	for (unsigned int a = 0; a < newGraphs.size(); a++)
	{ 
		_numPoints    += newGraphs[a]._massList.size();       ///// num points
		_numTriangles += newGraphs[a]._triangles.size();      ///// num triangles
		_numTriEdges  += newGraphs[a]._triEdges.size();       ///// num triangle edges
		_numAuxEdges  += newGraphs[a]._auxiliaryEdges.size(); ///// num auxiliary edges
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
		AGraph g1 = _graphs[a];
		g1.Scale(1.0f / g1._scale);

		AGraph g2;
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

AGraph StuffWorker::ProcessAnOrnament(AGraph oriGraph, AVector pos, float scale, int graphID/*, float maxEdgeLengthFactor*/)
{
	AGraph g1 = oriGraph;

	g1._id = graphID; // identification
	g1.Scale(scale);         // make it smaller
	g1.Move(pos);


	g1.RecalculateEdgeLengths(); // NN EDGES
	g1.RecalculateTriangleEdgeLengths();

	if (SystemParams::_should_rotate)
	{
	// rotate
		float randomVal = rand() % 628 - 314;
		randomVal /= 3.14f;
		g1.Rotate(randomVal);
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
	ss << SystemParams::_save_folder << "data.csv";
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
	ss5 << SystemParams::_save_folder << "SVG\\" << "result_" << frameCounter << ".svg";
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
		{ for (AGraph& aGraph : _graphs)  { aGraph.RecalculateArts(); } }
	
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

	for (AGraph& aGraph : _graphs)
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

	_pngImg.SaveImage(SystemParams::_save_folder + "PNG\\" + ss1.str() + ".png");

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
		{ _aDTransform->AddGraph(_graphs[a]); }

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

void StuffWorker::CalculateFillAndRMS(int numIter, bool saveImage)
{
	for (int a = 0; a < _graphs.size(); a++)
	{
		_graphs[a].RecalculateUniUniArts();
	}

	//_aDTransform->UpdateBoundaries(_graphs);
	//_aDTransform->CalculateFill(_cGrid, _fill_ratio, numIter, saveImage);
	_fill_ratio = ClipperWrapper::CalculateFill(_containerWorker->_container_boundaries[0], _graphs);

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

// physics sim
void StuffWorker::Init()
{
	int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
			{ this->_graphs[a]._massList[b].Init(); }
	}
}


// physics sim
void StuffWorker::CalculateThings(float dt)
{
	int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	// ---------- update boundary  ----------
	// OMP
	//#pragma omp for
	for (int a = startIter; a < _graphs.size(); a++)
		{ _graphs[a].UpdateBoundaryAndAvgEdgeLength(); }

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

	// ---------- reset growing ----------
	for (unsigned int a = startIter; a < _graphs.size(); a++)
		{ _graphs[a]._isGrowing = true; }
	if (startIter > 0)
	{
		for (unsigned int a = 0; a < startIter; a++)
			{ _graphs[a]._isGrowing = false; }
	}

	_cGrid->PrecomputeGraphIndices();


	// ---------- get closest point ----------
	// OMP
	//#pragma omp for
	for ( int a = startIter; a < _graphs.size(); a++)
	{
		
		for (int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			
			this->_graphs[a]._massList[b].GetClosestPoints2(a/*, _graphs, _cGrid*/);
		}
	}

	/*if (_fill_ratio > SystemParams::_shrink_fill_ratio && !_hasShrinkingInitiated)
	{
		std::cout << "shrink initiated\n";
		std::cout << _fill_ratio << "\n";
		for (unsigned int a = 0; a < _graphs.size(); a++)
			{ _graphs[a].InitShrinking(); }
		_hasShrinkingInitiated = true;
	}*/

	// calculate scale iter here !!!
	float scale_iter = SystemParams::_growth_scale_iter;
	float fill_diff = _fill_ratio - _man_neg_ratio;

	if (fill_diff > 0) // over
	{
		scale_iter = -SystemParams::_growth_scale_iter_2;
	}
	else if (fill_diff < 0 && fill_diff > -SystemParams::_growth_threshold_a) // under
	{
		scale_iter = SystemParams::_growth_scale_iter_2;
	}
	

	//if(_fill_ratio < _man_neg_ratio)
	//{
		for (unsigned int a = startIter; a < _graphs.size(); a++)
			{ _graphs[a].Grow(scale_iter, _graphs, dt); }
	//}
	/*else
	{
		for (unsigned int a = startIter; a < _graphs.size(); a++)
		{
			_graphs[a]._isGrowing = false;
		}
	}*/

	_numGrowingElement = 0;
	for (unsigned int a = startIter; a < _graphs.size(); a++)
		{ if (_graphs[a]._isGrowing) { _numGrowingElement++; } }

	// ---------- average skin thickness ----------
	_avgSkinThickness = 0;
	for (unsigned int a = 0; a < _graphs.size(); a++)
		{ _avgSkinThickness += _graphs[a]._scale * _graphs[a]._oriSkiOffset * SystemParams::_element_initial_scale; }

	_avgSkinThickness /= ((float)_graphs.size());
	SystemParams::_skin_thickness = _avgSkinThickness;	
}

void StuffWorker::Solve()
{
	int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	//std::vector<int> randomIndices;
	//for (unsigned int a = startIter; a < _graphs.size(); a++) { randomIndices.push_back(a); }
	
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		//int idx = randomIndices[a];
		// ----------  ----------
		_graphs[a].SolveForTriangleSprings();
		_graphs[a].SolveForNegativeSPaceSprings();

		
		//_graphs[a].SolveForNoise();

		// uncomment
		// ----------  ----------
		//_graphs[idx].ComputeFoldingForces();

		// uncomment
		// ---------- ----------
		//_graphs[idx].SelfIntersectionFlagging();

		// ----------  ----------
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
			{ this->_graphs[a]._massList[b].Solve(b, 
				                                  _graphs[a], 
				                                  _containerWorker->_container_boundaries, 
				                                  _containerWorker->_holes, 
				                                  _containerWorker->_offsetFocalBoundaries); }
	}
}

void StuffWorker::Simulate(float dt)
{
	int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }

	//float dampingVal = _positionDelta * SystemParams::_velocity_damping;
	for (unsigned int a = startIter; a < _graphs.size(); a++)
	{
		//float dampingVal = _graphs[a]._sumVelocity * SystemParams::_velocity_damping / (float)_graphs[a]._massList.size();
		for (unsigned int b = 0; b < _graphs[a]._massList.size(); b++)
		{
			this->_graphs[a]._massList[b].Simulate(dt/*, dampingVal, this->_graphs[a], _graphs*/);
		}
	}
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
	Init();          // reset forces to zero
	
	//UpdatePosition(dt); // velocity verlet
	Solve();            // calculate forces
	//UpdateVelocity(dt); // velocity verlet
	Simulate(dt);     // (non-velocity verlet) iterate the masses by the change in time	

	int startIter = 0;
	if (SystemParams::_simulate_2)  { startIter = _numReplicatedBigOnes; }
	
	// ---------- relax ----------
	/*
	for (unsigned int iter = 0; iter < SystemParams::_relax_iter_num; iter++)
	{
		std::vector<int> randomIndices;
		for (unsigned int a = startIter; a < _graphs.size(); a++) { randomIndices.push_back(a); }

		//if (SystemParams::_seed <= 0)
		//{
		//	std::random_shuffle(randomIndices.begin(), randomIndices.end());
		//}
		//else
		{
			int rand_num = rand();
			std::mt19937 g(rand_num);
			std::shuffle(randomIndices.begin(), randomIndices.end(), g);
		}

		for (unsigned int a = 0; a < randomIndices.size(); a++)
		{
			int idx = randomIndices[a];	
			AGraph* gPtr = &this->_graphs[idx];
			for (unsigned int b = 0; b < gPtr->_skinPointNum; b++)
				{ gPtr->_massList[b].UpdateLineSegment(gPtr->_massList); }
			gPtr->SelfIntersectionRelax();
			//delete gPtr;
		}
	}
	*/	
	// ---------- barycentric ----------
	if (SystemParams::_show_elements)
	{
		for (unsigned int a = startIter; a < _graphs.size(); a++)
			{ this->_graphs[a].RecalculateArts(); }
		//for (AGraph& aGraph : _graphs)  { aGraph.RecalculateArts(); } }
	}

	// ---------- position delta ----------
	_sumVelocity = 0;
	_totalRepulsionF   = 0.0f;
	_totalEdgeF        = 0.0f;
	_totalBoundaryF    = 0.0f;
	_totalOverlapF     = 0.0f;
	_avgScaleFactor    = 0.0f;
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
			_totalRepulsionF  += rF;
			_totalEdgeF       += eF;
			_totalBoundaryF   += bF;
			_totalOverlapF    += oF;
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
	std::cout << "AnalyzeManualPacking\n";

	PathIO pathIO;
	VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];

	_manualElements = reg.GetFocalBoundaries();
	_manualContainer = reg.GetBoundaries();

	_man_neg_ratio = ClipperWrapper::CalculateFill(_manualContainer[0], _manualElements);

	std::cout << "_manualElements = " << _manualElements.size() << "\n";
	std::cout << "_man_neg_ratio = " << _man_neg_ratio << "\n";
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

void StuffWorker::CalculateMetrics()
{
	// parameter
	float preOffset = 1.0f; // a bit hack !!!
	//float offVal2 = 10.0f;
	float maxOffVal = 20;
	float offValIter = 0.1f;

	OpenCVWrapper cvWRap;
	float containerArea = cvWRap.GetAreaOriented(_manualContainer[0]);
	std::cout << "containerArea = " << containerArea << "\n";

	// 1 - func-ception
	std::vector< std::vector<AVector>> offsetElements1 = ClipperWrapper::OffsetAll(ClipperWrapper::OffsetAll(_manualElements, preOffset), -preOffset);
	// debug (comment me)
	std::stringstream ss1;
	ss1 << SystemParams::_save_folder << "SVG\\" << "debugA.svg";
	MySVGRenderer::SaveShapesToSVG(ss1.str(), offsetElements1);

	//OpenCVWrapper cvWRap;
	std::vector<float> _offsetVals2;
	std::vector<float> _offsetVals3;

	for (float offVal2 = 0.0f; offVal2 < maxOffVal; offVal2 += offValIter)
	{ // begin for
		std::cout << offVal2 << "\n";
		// 2 - Generate offset elements one by one
		float area2 = 0;
		//float area2b = 0;
		std::vector< std::vector<AVector>> offsetElements2;
		for (unsigned int a = 0; a < offsetElements1.size(); a++)
		{
			// clockwise = element
			// counterclockwise = hole
			float offVal = offVal2;
			if (!UtilityFunctions::IsClockwise(offsetElements1[a])) { offVal = -offVal2; }

			std::vector<std::vector<AVector>> outputPolys1 = ClipperWrapper::RoundOffsettingP(offsetElements1[a], offVal);
			float tempArea;
			std::vector<std::vector<AVector>> outputPolys2 = ClipperWrapper::ClipElementsWithContainer(outputPolys1, _manualContainer[0], tempArea);
			if (!UtilityFunctions::IsClockwise(offsetElements1[a])) { tempArea = -tempArea; }
			area2 += tempArea;
			offsetElements2.insert(offsetElements2.end(), outputPolys2.begin(), outputPolys2.end());

			// AREA2B
			//for (unsigned int b = 0; b < outputPolys2.size(); b++)
			//{
			//	area2b += cvWRap.GetAreaOriented(outputPolys2[b]);
			//}
		}
		//std::cout << "area2 = " << area2 << ", area2b=" << area2b << ", ";
		//std::cout << "b " << std::abs(area2 - area2b) << ", ";
		/*std::stringstream ss2;
		ss2 << SystemParams::_save_folder << "SVG\\" << "debugB_" << offVal2 << ".svg";
		MySVGRenderer::SaveShapesToSVG(ss2.str(), offsetElements2);*/
		_offsetVals2.push_back(area2);

		// 3 - Generate offset of union of elements
		std::vector< std::vector<AVector>> offsetElements3_temp = ClipperWrapper::OffsetAll(offsetElements1, offVal2);
		float area3 = 0;
		std::vector<std::vector<AVector>> offsetElements3 = ClipperWrapper::ClipElementsWithContainer(offsetElements3_temp, _manualContainer[0], area3);

		//float area3b = 0;
		//for (unsigned int b = 0; b < offsetElements3.size(); b++)
		//{
		//	area3b += cvWRap.GetAreaOriented(offsetElements3[b]);
		//}

		//std::cout << "area3 = " << area3 << ", area3b = " << area3b << "\n\n";
		//std::cout << "c " << std::abs(area3 - area3b) << "\n\n";
		/*std::stringstream ss3;
		ss3 << SystemParams::_save_folder << "SVG\\" << "debugC_" << offVal2 << ".svg";
		MySVGRenderer::SaveShapesToSVG(ss3.str(), offsetElements3);*/
		_offsetVals3.push_back(area3);



	} // end for

	//pathIO.SaveSDF2CSV(_distArray, SystemParams::_save_folder + "dist_all.csv");
	PathIO pIO;
	pIO.SaveSDF2CSV(_offsetVals2, SystemParams::_save_folder + "dist_2.csv");
	pIO.SaveSDF2CSV(_offsetVals3, SystemParams::_save_folder + "dist_3.csv");

}

void StuffWorker::CreateManualPacking2()
{
	PathIO pathIO;
	std::vector<VFRegion> regs = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path");
	
	// _manualElementsss && _manualElements
	for (unsigned int a = 0; a < regs.size(); a++)
	{ 
		_manualElementsss.push_back(regs[a].GetFocalBoundaries()); 
		_manualElements.insert(_manualElements.end(), _manualElementsss[a].begin(), _manualElementsss[a].end());
	}

	// find container
	for (unsigned int a = 0; a < regs.size(); a++)
	{
		std::vector<std::vector<AVector>> temp = regs[a].GetBoundaries();
		if (temp.size() > 0)
			{ _manualContainer = temp; }
	}

	// ----
	// HERE
	// ----
	CalculateMetrics();
}

/*
CollissionGrid*                   _manualGrid;
std::vector<std::vector<AVector>> _manualElements;
std::vector<std::vector<AVector>> _manualSkeletons;
std::vector<std::vector<AVector>> _manualContainer;
*/
void StuffWorker::CreateManualPacking()
{
	// ---------- load regions ----------
	PathIO pathIO;
	VFRegion reg = pathIO.LoadRegions(SystemParams::_image_folder + SystemParams::_manual_art_name + ".path")[0];
	   
	// assignments
	_manualElements = reg.GetFocalBoundaries();
	//_manualElements = ClipperWrapper::OffsetAll( ClipperWrapper::OffsetAll( reg.GetFocalBoundaries(), preOffset), -preOffset); // the actual elements	
	_manualSkeletons = reg.GetFields();
	_manualContainer =  reg.GetBoundaries(); // target container

	// ----
	// HERE
	// ----
	//CalculateMetrics();

	/*
	std::stringstream ss5;
	ss5 << SystemParams::_save_folder << "SVG\\" << "result_" << frameCounter << ".svg";
	MySVGRenderer::SaveElementsAndSkins(ss5.str(), 
		                                _graphs, 
		                                _containerWorker->_focals, 
		                                _containerWorker->_offsetFocalBoundaries, 
		                                _containerWorker->_container_boundaries); //B
	*/
	/*std::vector<std::vector<AVector>> offsetShapes = ClipperWrapper::OffsetAll(_manualElements, 5.746f);
	std::stringstream ss5;
	ss5 << SystemParams::_save_folder << "SVG\\" << "offset.svg";
	MySVGRenderer::SaveShapesToSVG(ss5.str(), offsetShapes);*/
		
	// use this for skeletons
	//float offVal = 5.0f;
	//std::vector<std::vector<AVector>> temp1 = ClipperWrapper::RoundOffsettingPP(reg.GetBoundaries(), offVal);
	//_manualContainer = ClipperWrapper::RoundOffsettingPP(temp1, -offVal);
	//_manualContainer = temp1;
	// -------------------


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

	// assign to c grid
	for (unsigned int a = 0; a < _manualElements.size(); a++)
	{
		// boundary
		for (unsigned int b = 0; b < _manualElements[a].size(); b++)
			{ _manualGrid->InsertAPoint(_manualElements[a][b].x, _manualElements[a][b].y, a, b); }
	}

	float m_fill_ratio = 0;
	mDistTransform->_manualSkeletons = _manualSkeletons;
	mDistTransform->CalculateFill(_manualGrid, m_fill_ratio, 0, true);
	mDistTransform->CalculateSDF1(_manualGrid, 0, true);

	delete mDistTransform;
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

	//FastNoise myNoise3(3333); // Create a FastNoise object
	//myNoise3.SetNoiseType(FastNoise::Perlin); // Set the desired noise type
	
	AVector noiseVector;
	noiseVector.x = myNoise1.GetNoise(x, y);
	noiseVector.y = myNoise2.GetNoise(x, y);

	noiseVector = noiseVector.Norm();

	noiseVector.Print();

	return noiseVector;
}

void StuffWorker::ComputePerlinMap(int t)
{
	/*
	//StuffWorker::_perlinMap.clear();

	//FastNoise myNoise1(rand()); // Create a FastNoise object
	//myNoise1.SetNoiseType(FastNoise::Perlin); // Set the desired noise type

	//FastNoise myNoise2(rand()); // Create a FastNoise object
	//myNoise2.SetNoiseType(FastNoise::Perlin); // Set the desired noise type
	
	float scaleFactor = SystemParams::_noise_factor;
	for (unsigned int a = 0; a < SystemParams::_upscaleFactor; a++)
	{
		//std::vector<AVector> pMap;
		for (unsigned int b = 0; b < SystemParams::_upscaleFactor; b++)
		{
			

			//AVector tangentVector = AVector(a, b).DirectionTo(AVector(250.0, 250)).Norm();
			//AVector circleVector = AVector(-tangentVector.y, tangentVector.x);
			
			float val = myNoise1.GetNoise(a * scaleFactor, b * scaleFactor, t * scaleFactor);

			AVector noiseVector = AVector(std::sin(2.0 * 3.14159 * val), std::cos(2.0 * 3.14159 * val));
			//AVector noiseVector;
			//noiseVector.x = myNoise1.GetNoise(a * scaleFactor, b * scaleFactor, t);
			//noiseVector.y = myNoise2.GetNoise(a * scaleFactor, b * scaleFactor, t);

			//noiseVector = (noiseVector * 3.0f + circleVector * 1.0f) / 4.0f;

			StuffWorker::_perlinMap[a][b] = noiseVector;
		}
		//StuffWorker::_perlinMap.push_back(pMap);
	}
	*/
}