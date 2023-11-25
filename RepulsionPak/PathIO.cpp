
/* ---------- ShapeRadiusMatching V2  ---------- */

//#include "stdafx.h"

#include "AnIdxTriangle.h"
#include "PathIO.h"
#include "SystemParams.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include <sstream> // std::stringstream

#include <sys/stat.h>
//#include <unistd.h>

/*
================================================================================
================================================================================
*/
PathIO::PathIO()
{
}

/*
================================================================================
================================================================================
*/
PathIO::~PathIO()
{
}

/*
file format:
	[ori_index]
	[num_point]
	[num_boundary_edge]
	[num_art]
	x0 y0 x1 y1 x2 y2...								% mass pos
	idx_0_0 idx_0_1 idx_1_0 idx_1_1 idx_2_0 idx_2_1...  % boundary_edges
	idx_0_0 idx_0_1 idx_0_2 ...                         % triangles
	x0 y0 x1 y1 x2 y2 ...								% art 1
	x0 y0 x1 y1 x2 y2 ...								% art 2
	...
	idx0 idx1 idx2 ...									% art to tri
	idx0 idx1 idx2 ...									% art to tri
	...
	u0 v0 w0...											% bary of art 1
	u0 v0 w0...											% bary of art 2
	...
*/
// graph
void PathIO::SaveAGraph(AnElement aGraph, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	// num points
	*f << aGraph._massList.size() << "\n";

	// num edges
	//*f << aGraph._edges.size() << "\n";

	// num boundary edges
	*f << aGraph._skinPointNum << "\n";

	// num arts
	*f << aGraph._arts.size() << "\n";

	// mass pos
	for (int a = 0; a < aGraph._massList.size(); a++)
	{
		*f << std::setprecision(20) << aGraph._massList[a]._pos.x << " " << std::setprecision(20) << aGraph._massList[a]._pos.y;
		if (a < aGraph._massList.size() - 1) { *f << " "; }
	}

	*f << "\n";

	// edge indices
	/*for (int a = 0; a < aGraph._edges.size(); a++)
	{
		*f << aGraph._edges[a]._index0 << " " << aGraph._edges[a]._index1;
		if (a < aGraph._edges.size() - 1) { *f << " "; }
	}
	*f << "\n";*/

	// boundary edge indices
	for (int a = 0; a < aGraph._skinIdxEdges.size(); a++)
	{
		*f << aGraph._skinIdxEdges[a]._index0 << " " << aGraph._skinIdxEdges[a]._index1;
		if (a < aGraph._skinIdxEdges.size() - 1) { *f << " "; }
	}

	*f << "\n";

	// triangles
	for (int a = 0; a < aGraph._triangles.size(); a++)
	{
		*f << aGraph._triangles[a].idx0 << " "
			<< aGraph._triangles[a].idx1 << " "
			<< aGraph._triangles[a].idx2;
		if (a < aGraph._triangles.size() - 1) { *f << " "; }
	}

	*f << "\n";

	// arts
	for (int a = 0; a < aGraph._arts.size(); a++)
	{
		for (int b = 0; b < aGraph._arts[a].size(); b++)
		{
			*f << std::setprecision(20)
				<< aGraph._arts[a][b].x << " "
				<< std::setprecision(20)
				<< aGraph._arts[a][b].y;
			if (b < aGraph._arts[a].size() - 1) { *f << " "; }
		}
		*f << "\n";
	}

	// art to triangle indices
	for (int a = 0; a < aGraph._arts2Triangles.size(); a++)
	{
		for (int b = 0; b < aGraph._arts2Triangles[a].size(); b++)
		{
			*f << aGraph._arts2Triangles[a][b];
			if (b < aGraph._arts2Triangles[a].size() - 1) { *f << " "; }
		}
		*f << "\n";
	}

	// barycentric coordinates
	for (int a = 0; a < aGraph._baryCoords.size(); a++)
	{
		for (int b = 0; b < aGraph._baryCoords[a].size(); b++)
		{
			*f << std::setprecision(20)
				<< aGraph._baryCoords[a][b]._u << " "
				<< std::setprecision(20)
				<< aGraph._baryCoords[a][b]._v << " "
				<< std::setprecision(20)
				<< aGraph._baryCoords[a][b]._w;
			if (b < aGraph._baryCoords[a].size() - 1) { *f << " "; }
		}
		if (a < aGraph._baryCoords.size() - 1) { *f << "\n"; }
	}

	f->close();

	delete f;
}

/*
	[ori_index]
	file format:
	[num_point]
	[num_boundary_edge]
	[num_art]
	x0 y0 x1 y1 x2 y2...								% mass pos
	idx_0_0 idx_0_1 idx_1_0 idx_1_1 idx_2_0 idx_2_1...  % boundary_edges
	idx_0_0 idx_0_1 idx_0_2 ...                         % triangles
	x0 y0 x1 y1 x2 y2 ...								% art 1
	x0 y0 x1 y1 x2 y2 ...								% art 2
	...
	idx0 idx1 idx2 ...									% art to tri
	idx0 idx1 idx2 ...									% art to tri
	...
	u0 v0 w0...											% bary of art 1
	u0 v0 w0...											% bary of art 2
	...
*/
AnElement PathIO::LoadAGraph(std::string filename)
{
	//return AGraph();
	AnElement aGraph;
	std::ifstream myfile(filename);

	// num_point
	std::string line1;
	std::getline(myfile, line1);
	int num_mass = std::stoi(line1);

	// num_edge
	//std::string line2;
	//std::getline(myfile, line2);
	//int num_edges = std::stoi(line2);

	// num_boundary_edge
	std::string line22;
	std::getline(myfile, line22);
	int num_boundary_point = std::stoi(line22);

	// num_art
	std::string line23;
	std::getline(myfile, line23);
	int num_arts = std::stoi(line23);

	// mass list
	std::string line3;
	std::getline(myfile, line3);
	std::vector<std::string> arrayTemp3 = UtilityFunctions::Split(line3, ' ');
	size_t halfLength = arrayTemp3.size() / 2;
	for (int a = 0; a < halfLength; a++)
	{
		int idx = a * 2;
		float x = std::stof(arrayTemp3[idx]);
		float y = std::stof(arrayTemp3[idx + 1]);
		AMass aMass;
		aMass._pos = AVector(x, y);
		//aMass._m = 1.0f; // weight 1.0kg
		aMass._idx = a;
		aGraph._massList.push_back(aMass);
	}

	// edges
	/*std::string line4;
	std::getline(myfile, line4);
	std::vector<std::string> arrayTemp4 = UtilityFunctions::Split(line4, ' ');
	halfLength = arrayTemp4.size() / 2;
	for (int a = 0; a < halfLength; a++)
	{
		int idx = a * 2;
		int index0 = std::stoi(arrayTemp4[idx]);
		int index1 = std::stoi(arrayTemp4[idx + 1]);
		float dist   = aGraph._massList[index0]._pos.Distance(aGraph._massList[index1]._pos);
		AnIndexedLine anEdge(index0, index1, dist);
		aGraph._edges.push_back(anEdge);
	}*/

	// boundary edges
	std::string line5;
	std::getline(myfile, line5);
	std::vector<std::string> arrayTemp5 = UtilityFunctions::Split(line5, ' ');
	halfLength = arrayTemp5.size() / 2;
	for (int a = 0; a < halfLength; a++)
	{
		int idx = a * 2;
		int index0 = std::stoi(arrayTemp5[idx]);
		int index1 = std::stoi(arrayTemp5[idx + 1]);
		float dist = aGraph._massList[index0]._pos.Distance(aGraph._massList[index1]._pos);
		AnIndexedLine anEdge(index0, index1, dist);
		aGraph._skinIdxEdges.push_back(anEdge);
	}

	// triangles
	std::string line6;
	std::getline(myfile, line6);
	std::vector<std::string> arrayTemp6 = UtilityFunctions::Split(line6, ' ');
	for (int a = 0; a < arrayTemp6.size(); a += 3)
	{
		int index0 = std::stoi(arrayTemp6[a]);
		int index1 = std::stoi(arrayTemp6[a + 1]);
		int index2 = std::stoi(arrayTemp6[a + 2]);
		AnIdxTriangle tri(index0, index1, index2);
		aGraph._triangles.push_back(tri);
	}

	// arts
	for (int a = 0; a < num_arts; a++)
	{
		std::vector<AVector> anArt;

		std::string line7;
		std::getline(myfile, line7);
		std::vector<std::string> arrayTemp7 = UtilityFunctions::Split(line7, ' ');
		halfLength = arrayTemp7.size() / 2;
		for (int b = 0; b < halfLength; b++)
		{
			int idx = b * 2;
			float x = std::stof(arrayTemp7[idx]);
			float y = std::stof(arrayTemp7[idx + 1]);
			anArt.push_back(AVector(x, y));
		}
		aGraph._arts.push_back(anArt);
	}

	// art to triangle indices
	for (int a = 0; a < num_arts; a++)
	{
		std::vector<int> a2t;

		std::string line8;
		std::getline(myfile, line8);
		std::vector<std::string> arrayTemp8 = UtilityFunctions::Split(line8, ' ');
		for (int b = 0; b < arrayTemp8.size(); b++)
		{
			a2t.push_back(std::stoi(arrayTemp8[b]));
		}
		aGraph._arts2Triangles.push_back(a2t);
	}

	// barycentric coordinates
	for (int a = 0; a < num_arts; a++)
	{
		std::vector<ABary> bCoords;

		std::string line9;
		std::getline(myfile, line9);
		std::vector<std::string> arrayTemp9 = UtilityFunctions::Split(line9, ' ');
		for (int b = 0; b < arrayTemp9.size(); b += 3)
		{
			float u = std::stof(arrayTemp9[b]);
			float v = std::stof(arrayTemp9[b + 1]);
			float w = std::stof(arrayTemp9[b + 2]);
			ABary bary(u, v, w);
			bCoords.push_back(bary);
		}
		aGraph._baryCoords.push_back(bCoords);
	}

	aGraph._skinPointNum = num_boundary_point;
	//aGraph.AssignEdgesToMasses();
	//aGraph.CalculateNNEdges();

	myfile.close();

	return aGraph;
}

/*
================================================================================
================================================================================
*/
/*

file format:

	[num_region] # first line only
	[region number] [is_closed_path] [path_type] x0 y0 x1 y1 x2 y2...
	[region number] [is_closed_path] [path_type] x0 y0 x1 y1 x2 y2...
	[region number] [is_closed_path] [path_type] x0 y0 x1 y1 x2 y2...
	...
	EOF


explanation:

	region number lets us to know a certain path belongs to a certain region.
	a region has it's own vector field


	is_closed_path = 1, if it's closed
	is_closed_path = 0, if it's an open path

	path_type are either SHAPE_PATH or FIELD_PATH (see APath.h)

*/

void PathIO::SaveRegions(std::vector<VFRegion> regions, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	// first line tells us how many regions there are
	*f << regions.size() << "\n";

	// iterate
	for (size_t a = 0; a < regions.size(); a++)
	{
		VFRegion aRegion = regions[a];

		if (aRegion._boundaryFColors.size() > 0)
		{
			*f << "foreground_color " << a << " ";
			for (size_t c = 0; c < aRegion._boundaryFColors.size(); c++)
			{
				*f << aRegion._boundaryFColors[c];
				if (c < aRegion._boundaryFColors.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		if (aRegion._boundaryBColors.size() > 0)
		{
			*f << "background_color " << a << " ";
			for (size_t c = 0; c < aRegion._boundaryBColors.size(); c++)
			{
				*f << aRegion._boundaryBColors[c];
				if (c < aRegion._boundaryBColors.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		// BOUNDARY_PATH = 0,
		for (int b = 0; b < aRegion._boundaryPaths.size(); b++)
		{
			APath boundaryPath = aRegion._boundaryPaths[b];
			SavePath(f, boundaryPath, a);
		}

		// DETAIL_PATH = 1,
		/*for (int b = 0; b < aRegion._detailPaths.size(); b++)
		{
		APath boundaryPath = aRegion._boundaryPaths[b];
		//...not yet implemented
		}*/

		// FOCAL_PATH = 2,
		for (int b = 0; b < aRegion._focalPaths.size(); b++)
		{
			APath focalPath = aRegion._focalPaths[b];
			SavePath(f, focalPath, a);
		}

		// FIELD_PATH = 3,
		for (int b = 0; b < aRegion._fieldPaths.size(); b++)
		{
			APath fieldPath = aRegion._fieldPaths[b];
			SavePath(f, fieldPath, a);
		}

		// SEED_PATH = 4,
		/*for (int b = 0; b < aRegion._seedPaths.size(); b++)
		{
		APath seedPath = aRegion._seedPaths[b];
		SavePath(f, seedPath, a);
		}*/

		// STREAM_LINE = 5
		for (int b = 0; b < aRegion._streamLines.size(); b++)
		{
			APath streamPath;
			streamPath.isClosed = false;
			streamPath.pathType = STREAM_LINE;
			streamPath.points = aRegion._streamLines[b];
			SavePath(f, streamPath, a);
		}

		// SKELETON_LINE = 6
		for (int b = 0; b < aRegion._skeletonLines.size(); b++)
		{
			APath streamPath;
			streamPath.isClosed = false;
			streamPath.pathType = SKELETON_LINE;
			streamPath.points = aRegion._skeletonLines[b];
			SavePath(f, streamPath, a);
		}

		// BOUNDARY MST
		//for (int b = 0; b < aRegion._boundaryMSTPath.size(); b++)
		//{
		//	SavePath(f, aRegion._boundaryMSTPath[b], a);
		//}
	}

	f->close();

	delete f;
}

void PathIO::SaveData(std::vector < std::vector<float> > forceData, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);


	for (int a = 0; a < forceData.size(); a++)
	{

		*f << std::setprecision(20) << forceData[a][0] << ","    // 1
			<< std::setprecision(20) << forceData[a][1] << ","   // 2
			<< std::setprecision(20) << forceData[a][2] << ","   // 3
			<< std::setprecision(20) << forceData[a][3] << ","   // 4
			<< std::setprecision(20) << forceData[a][4] << ","   // 5
			<< std::setprecision(20) << forceData[a][5] << ","   // 6 
			<< std::setprecision(20) << forceData[a][6] << "\n"; // 7
	}

	f->close();

	delete f;
}

/*
================================================================================
================================================================================
*/
void PathIO::SavePath(std::ofstream* f, APath aPath, int no_region)
{
	// no region (metadata)
	*f << no_region << " ";

	// is_closed_path (metadata)
	if (aPath.isClosed) { *f << "1" << " "; }
	else { *f << "0" << " "; }

	// path type (metadata)
	*f << aPath.pathType << " ";

	// points
	for (size_t b = 0; b < aPath.points.size(); b++)
	{
		*f << std::setprecision(20) << aPath.points[b].x << " " << std::setprecision(20) << aPath.points[b].y;
		if (b < aPath.points.size() - 1) { *f << " "; }
	}
	*f << "\n";
}

/*
================================================================================
================================================================================
*/
std::vector<VFRegion> PathIO::LoadRegions(std::string filename)
{
	std::vector<VFRegion> duhRegions;

	//int lineNumber = 0;

	std::ifstream myfile(filename);

	std::string firstLine;
	std::getline(myfile, firstLine);
	int numRegion = (int)std::stoi(firstLine);
	duhRegions = std::vector<VFRegion>(numRegion);

	while (!myfile.eof())
	{
		APath aPath;

		std::string line;
		std::getline(myfile, line);
		std::vector<std::string> arrayTemp = UtilityFunctions::Split(line, ' ');
		if (arrayTemp.size() == 0) { continue; }

		if (arrayTemp[0] == "foreground_color")
		{
			int whatRegionDude = (int)std::stoi(arrayTemp[1]);
			std::vector<int> fColors;
			for (int a = 2; a < arrayTemp.size(); a++)
			{
				fColors.push_back(std::stoi(arrayTemp[a]));
			}
			duhRegions[whatRegionDude]._boundaryFColors = fColors;

			continue;
		}
		else if (arrayTemp[0] == "background_color")
		{
			int whatRegionDude = (int)std::stoi(arrayTemp[1]);
			std::vector<int> bColors;
			for (int a = 2; a < arrayTemp.size(); a++)
			{
				bColors.push_back(std::stoi(arrayTemp[a]));
			}
			duhRegions[whatRegionDude]._boundaryBColors = bColors;

			continue;
		}
		else if (arrayTemp[0] == "foreground_color_rgb")
		{
			int whatRegionDude = (int)std::stoi(arrayTemp[1]);

			std::vector<MyColor> fColors;
			for (int a = 2; a < arrayTemp.size(); a++)
			{
				std::vector<std::string> colTemp = UtilityFunctions::Split(arrayTemp[a], ',');
				MyColor col = MyColor(std::stoi(colTemp[0]), std::stoi(colTemp[1]), std::stoi(colTemp[2]));
				fColors.push_back(col);
			}
			duhRegions[whatRegionDude]._boundaryFColorsRGB = fColors;

			continue;
		}
		else if (arrayTemp[0] == "background_color_rgb")
		{
			int whatRegionDude = (int)std::stoi(arrayTemp[1]);

			std::vector<MyColor> bColors;
			for (int a = 2; a < arrayTemp.size(); a++)
			{
				std::vector<std::string> colTemp = UtilityFunctions::Split(arrayTemp[a], ',');
				MyColor col = MyColor(std::stoi(colTemp[0]), std::stoi(colTemp[1]), std::stoi(colTemp[2]));
				bColors.push_back(col);
			}
			duhRegions[whatRegionDude]._boundaryBColorsRGB = bColors;

			continue;
		}

		std::vector<std::string> metadataArray(arrayTemp.begin(), arrayTemp.begin() + 3); // this is metadata
		std::vector<std::string> pathArray(arrayTemp.begin() + 3, arrayTemp.end()); // this is path

		// read metadata
		int whatRegionDude = (int)std::stoi(metadataArray[0]);
		aPath.isClosed = (int)std::stoi(metadataArray[1]);		// is it closed
		aPath.pathType = (PathType)std::stoi(metadataArray[2]); // path type

		// read path
		if (pathArray.size() < 2) { continue; }
		size_t halfLength = pathArray.size() / 2;
		for (size_t a = 0; a < halfLength; a++)
		{
			int idx = a * 2;
			double x = std::stod(pathArray[idx]);
			double y = std::stod(pathArray[idx + 1]);
			aPath.points.push_back(AVector(x, y));
		}

		/*
			BOUNDARY_PATH = 0
			DETAIL_PATH   = 1
			FOCAL_PATH    = 2
			FIELD_PATH    = 3
			SEED_PATH     = 4
			STREAM_LINE   = 5
			SKELETON_LINE = 6
		*/
		if (aPath.pathType == BOUNDARY_PATH)
		{
			duhRegions[whatRegionDude]._boundaryPaths.push_back(aPath);
		}
		else if (aPath.pathType == DETAIL_PATH)
		{
			//duhRegions[whatRegionDude]._detailPaths.push_back(aPath);
		}
		else if (aPath.pathType == FOCAL_PATH)
		{
			duhRegions[whatRegionDude]._focalPaths.push_back(aPath);
		}
		else if (aPath.pathType == FIELD_PATH)
		{
			duhRegions[whatRegionDude]._fieldPaths.push_back(aPath);
		}
		else if (aPath.pathType == SEED_PATH)
		{
			//duhRegions[whatRegionDude]._seedPaths.push_back(aPath);
		}
		else if (aPath.pathType == SKELETON_LINE)
		{
			duhRegions[whatRegionDude]._skeletonLines.push_back(aPath.points);
		}
		else if (aPath.pathType == HOLE)
		{
			duhRegions[whatRegionDude]._holes.push_back(aPath.points);
		}

	}

	for (unsigned int a = 0; a < duhRegions.size(); a++)
	{
		if (duhRegions[a]._boundaryPaths.size() > 0 &&
			duhRegions[a]._boundaryFColors.size() == 0 &&
			duhRegions[a]._boundaryBColors.size() == 0)
		{
			//std::cout << filename << "\n";
			//std::cout << "region " << a << " doesn't have F and B colors\n\n";
			for (unsigned int b = 0; b < duhRegions[a]._boundaryPaths.size(); b++)
			{
				duhRegions[a]._boundaryFColors.push_back(7);
				duhRegions[a]._boundaryBColors.push_back(8);
			}
		}
	}

	myfile.close();

	return duhRegions;
}
/*
================================================================================
for every ArtData:

[focal_element]
[region_number]
[blob_number]
[ornament_number]
[boundary_number]
x0 y0 x1 y1 x2 y2... # boundary 1
x0 y0 x1 y1 x2 y2... # boundary 2
x0 y0 x1 y1 x2 y2... # boundary 3
...					 # boundary N
x0 y0 x1 y1 x2 y2... # vvsPt 1
x0 y0 x1 y1 x2 y2... # vvsPt 2
x0 y0 x1 y1 x2 y2... # vvsPt 3
...					 # vvsPt N
x0 y0 x1 y1 x2 y2... # vvDir 1
x0 y0 x1 y1 x2 y2... # vvDir 2
x0 y0 x1 y1 x2 y2... # vvDir 3
...					 # vvDir N
i1 i2 i3 ... # vvIdx 1
i1 i2 i3 ... # vvIdx 2
i1 i2 i3 ... # vvIdx 3
...			 # vvIdx N
v1 v2 v3 ... # vvNormDist 1
v1 v2 v3 ... # vvNormDist 2
v1 v2 v3 ... # vvNormDist 3
...			 # vvNormDist N
v1 v2 v3 ... # vvDist 1
v1 v2 v3 ... # vvDist 2
v1 v2 v3 ... # vvDist 3
...			 # vvDist N
v1 v2 v3 ... # vvNewDist 1
v1 v2 v3 ... # vvNewDist 2
v1 v2 v3 ... # vvNewDist 3
...			 # vvNewDist N

================================================================================
*/
void PathIO::SaveArtData(std::vector<ArtData> artDataArray, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	for (int i = 0; i < artDataArray.size(); i++)
	{
		ArtData aData = artDataArray[i];

		// ---------- [focal_element] ----------
		if (aData._isFocalElement) { *f << 1 << "\n"; }
		else { *f << 0 << "\n"; }

		// ---------- [region_number] ----------
		*f << aData._regionNumber << "\n";

		// ---------- [blob_number] ----------
		*f << aData._blobNumber << "\n";

		// ---------- [ornament_number] ----------
		*f << aData._ornamentNumber << "\n";

		// ---------- [boundary_number] ----------
		int boundary_number = aData._boundaries.size();
		*f << boundary_number << "\n";

		/*
		x0 y0 x1 y1 x2 y2... # boundary 1
		x0 y0 x1 y1 x2 y2... # boundary 2
		x0 y0 x1 y1 x2 y2... # boundary 3
		...					 # boundary N
		*/

		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> boundary = aData._boundaries[a];
			for (int b = 0; b < boundary.size(); b++)
			{
				AVector pt = boundary[b];
				*f << std::setprecision(20) << pt.x << " " << std::setprecision(20) << pt.y;
				if (b < boundary.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		// stop if focal ornament
		if (aData._isFocalElement) { continue; }

		/*
		x0 y0 x1 y1 x2 y2... # vvsPt 1
		x0 y0 x1 y1 x2 y2... # vvsPt 2
		x0 y0 x1 y1 x2 y2... # vvsPt 3
		...					 # vvsPt N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> vSpt = aData._vvsPt[a];
			for (int b = 0; b < vSpt.size(); b++)
			{
				AVector pt = vSpt[b];
				*f << std::setprecision(20) << pt.x << " " << std::setprecision(20) << pt.y;
				if (b < vSpt.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		/*
		x0 y0 x1 y1 x2 y2... # vvDir 1
		x0 y0 x1 y1 x2 y2... # vvDir 2
		x0 y0 x1 y1 x2 y2... # vvDir 3
		...					 # vvDir N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> vDir = aData._vvDir[a];
			for (int b = 0; b < vDir.size(); b++)
			{
				AVector pt = vDir[b];
				*f << std::setprecision(20) << pt.x << " " << std::setprecision(20) << pt.y;
				if (b < vDir.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		/*
		i1 i2 i3 ... # vvIdx 1
		i1 i2 i3 ... # vvIdx 2
		i1 i2 i3 ... # vvIdx 3
		...			 # vvIdx N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<int> vIdx = aData._vvIdx[a];
			for (int b = 0; b < vIdx.size(); b++)
			{
				int val = vIdx[b];
				*f << val;
				if (b < vIdx.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		/*
		v1 v2 v3 ... # vvNormDist 1
		v1 v2 v3 ... # vvNormDist 2
		v1 v2 v3 ... # vvNormDist 3
		...			 # vvNormDist N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vNormDist = aData._vvNormDist[a];
			for (int b = 0; b < vNormDist.size(); b++)
			{
				float val = vNormDist[b];
				*f << std::setprecision(20) << val;
				if (b < vNormDist.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

		/*
		v1 v2 v3 ... # vvDist 1
		v1 v2 v3 ... # vvDist 2
		v1 v2 v3 ... # vvDist 3
		...			 # vvDist N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vDist = aData._vvDist[a];
			for (int b = 0; b < vDist.size(); b++)
			{
				float val = vDist[b];
				*f << std::setprecision(20) << val;
				if (b < vDist.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}


		/*
		v1 v2 v3 ... # vvNewDist 1
		v1 v2 v3 ... # vvNewDist 2
		v1 v2 v3 ... # vvNewDist 3
		...			 # vvNewDist N
		*/
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vNewDist = aData._vvNewDist[a];
			for (int b = 0; b < vNewDist.size(); b++)
			{
				float val = vNewDist[b];
				*f << std::setprecision(20) << val;
				if (b < vNewDist.size() - 1) { *f << " "; }
			}
			*f << "\n";
		}

	}

	f->close();
	delete f;
}

/*
================================================================================
for every ArtData:
[focal_ornament]
[region_number]
[blob_number]
[ornament_number]
[boundary_number]
x0 y0 x1 y1 x2 y2... # boundary 1
x0 y0 x1 y1 x2 y2... # boundary 2
x0 y0 x1 y1 x2 y2... # boundary 3
...					 # boundary N
x0 y0 x1 y1 x2 y2... # vvsPt 1
x0 y0 x1 y1 x2 y2... # vvsPt 2
x0 y0 x1 y1 x2 y2... # vvsPt 3
...					 # vvsPt N
x0 y0 x1 y1 x2 y2... # vvDir 1
x0 y0 x1 y1 x2 y2... # vvDir 2
x0 y0 x1 y1 x2 y2... # vvDir 3
...					 # vvDir N
i1 i2 i3 ... # vvIdx 1
i1 i2 i3 ... # vvIdx 2
i1 i2 i3 ... # vvIdx 3
...			 # vvIdx N
v1 v2 v3 ... # vvNormDist 1
v1 v2 v3 ... # vvNormDist 2
v1 v2 v3 ... # vvNormDist 3
...			 # vvNormDist N
v1 v2 v3 ... # vvDist 1
v1 v2 v3 ... # vvDist 2
v1 v2 v3 ... # vvDist 3
...			 # vvDist N
v1 v2 v3 ... # vvNewDist 1
v1 v2 v3 ... # vvNewDist 2
v1 v2 v3 ... # vvNewDist 3
...			 # vvNewDist N

================================================================================
*/
std::vector<ArtData> PathIO::LoadArtData(std::string filename)
{
	std::vector<ArtData> artDataArray;

	std::ifstream myfile(filename);

	while (!myfile.eof())
	{
		// [focal_ornament]
		std::string line0;
		std::getline(myfile, line0);
		if (line0.size() == 0) { continue; } // last line
		bool focalOrnament = std::stoi(line0);

		// [region_number]
		std::string line1;
		std::getline(myfile, line1);
		//if (line1.size() == 0) { continue; } // last line
		int region_number = std::stoi(line1);

		// [blob_number]
		std::string line2;
		std::getline(myfile, line2);
		int blob_number = std::stoi(line2);

		// [ornament_number]
		std::string line3;
		std::getline(myfile, line3);
		int ornament_number = std::stoi(line3);

		// [boundary_number]
		std::string line4;
		std::getline(myfile, line4);
		int boundary_number = std::stoi(line4);

		/*
		x0 y0 x1 y1 x2 y2... # boundary 1
		x0 y0 x1 y1 x2 y2... # boundary 2
		x0 y0 x1 y1 x2 y2... # boundary 3
		...					 # boundary N
		*/
		std::vector<std::vector<AVector>> boundaries;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> boundary;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> boundaryArray = UtilityFunctions::Split(line, ' ');
			if (boundaryArray.size() == 0) { continue; }
			size_t halfLength = boundaryArray.size() / 2;
			for (size_t i = 0; i < halfLength; i++)
			{
				int idx = i * 2;
				float x = std::stof(boundaryArray[idx]);
				float y = std::stof(boundaryArray[idx + 1]);
				boundary.push_back(AVector(x, y));
			}
			boundaries.push_back(boundary);
		}

		// stop if focal ornament
		if (focalOrnament)
		{
			ArtData artData;
			artData._isFocalElement = focalOrnament;
			artData._regionNumber = region_number;
			artData._blobNumber = blob_number;
			artData._ornamentNumber = ornament_number;
			artData._boundaries = boundaries;

			artDataArray.push_back(artData);
			continue;
		}

		/*
		x0 y0 x1 y1 x2 y2... # vvsPt 1
		x0 y0 x1 y1 x2 y2... # vvsPt 2
		x0 y0 x1 y1 x2 y2... # vvsPt 3
		...					 # vvsPt N
		*/
		std::vector<std::vector<AVector>> vvsPt;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> vsPt;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> sPtArray = UtilityFunctions::Split(line, ' ');
			if (sPtArray.size() == 0) { continue; }
			size_t halfLength = sPtArray.size() / 2;
			for (size_t i = 0; i < halfLength; i++)
			{
				int idx = i * 2;
				float x = std::stof(sPtArray[idx]);
				float y = std::stof(sPtArray[idx + 1]);
				vsPt.push_back(AVector(x, y));
			}
			vvsPt.push_back(vsPt);
		}

		/*
		x0 y0 x1 y1 x2 y2... # vvDir 1
		x0 y0 x1 y1 x2 y2... # vvDir 2
		x0 y0 x1 y1 x2 y2... # vvDir 3
		...					 # vvDir N
		*/
		std::vector<std::vector<AVector>> vvDir;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<AVector> vDir;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> dirArray = UtilityFunctions::Split(line, ' ');
			if (dirArray.size() == 0) { continue; }
			size_t halfLength = dirArray.size() / 2;
			for (size_t i = 0; i < halfLength; i++)
			{
				int idx = i * 2;
				float x = std::stof(dirArray[idx]);
				float y = std::stof(dirArray[idx + 1]);
				vDir.push_back(AVector(x, y));
			}
			vvDir.push_back(vDir);
		}

		/*
		i1 i2 i3 ... # vvIdx 1
		i1 i2 i3 ... # vvIdx 2
		i1 i2 i3 ... # vvIdx 3
		...			 # vvIdx N
		*/
		std::vector<std::vector<int>> vvIdx;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<int> vIdx;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> distArray = UtilityFunctions::Split(line, ' ');
			if (distArray.size() == 0) { continue; }
			for (size_t i = 0; i < distArray.size(); i++)
			{
				int val = std::stoi(distArray[i]);
				vIdx.push_back(val);
			}
			vvIdx.push_back(vIdx);
		}

		/*
		v1 v2 v3 ... # vvNormDist 1
		v1 v2 v3 ... # vvNormDist 2
		v1 v2 v3 ... # vvNormDist 3
		...			 # vvNormDist N
		*/
		std::vector<std::vector<float>> vvNormDist;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vNormDist;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> distArray = UtilityFunctions::Split(line, ' ');
			if (distArray.size() == 0) { continue; }
			for (size_t i = 0; i < distArray.size(); i++)
			{
				float val = std::stof(distArray[i]);
				vNormDist.push_back(val);
			}
			vvNormDist.push_back(vNormDist);
		}

		/*
		v1 v2 v3 ... # vvDist 1
		v1 v2 v3 ... # vvDist 2
		v1 v2 v3 ... # vvDist 3
		...			 # vvDist N
		*/
		std::vector<std::vector<float>> vvDist;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vDist;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> distArray = UtilityFunctions::Split(line, ' ');
			if (distArray.size() == 0) { continue; }
			for (size_t i = 0; i < distArray.size(); i++)
			{
				float val = std::stof(distArray[i]);
				vDist.push_back(val);
			}
			vvDist.push_back(vDist);
		}

		/*
		v1 v2 v3 ... # vvNewDist 1
		v1 v2 v3 ... # vvNewDist 2
		v1 v2 v3 ... # vvNewDist 3
		...			 # vvNewDist N
		*/
		std::vector<std::vector<float>> vvNewDist;
		for (int a = 0; a < boundary_number; a++)
		{
			std::vector<float> vNewDist;
			std::string line;
			std::getline(myfile, line);
			std::vector<std::string> distArray = UtilityFunctions::Split(line, ' ');
			if (distArray.size() == 0) { continue; }
			for (size_t i = 0; i < distArray.size(); i++)
			{
				float val = std::stof(distArray[i]);
				vNewDist.push_back(val);
			}
			vvNewDist.push_back(vNewDist);
		}

		ArtData artData;
		artData._isFocalElement = focalOrnament;
		artData._regionNumber = region_number;
		artData._blobNumber = blob_number;
		artData._ornamentNumber = ornament_number;
		artData._boundaries = boundaries;		// boundaries of ornaments
		artData._vvsPt = vvsPt;			// reference point at the streamline
		artData._vvDir = vvDir;			// L/R direction
		artData._vvIdx = vvIdx;			// L/R
		artData._vvNormDist = vvNormDist;		// normalized location at the streamline 0..1
		artData._vvDist = vvDist;			// original distance from ornament point to skeleton
		artData._vvNewDist = vvNewDist;		// actual   distance from ornament point to skeleton

		artDataArray.push_back(artData);
	}

	myfile.close();

	return artDataArray;
}





/*
================================================================================
================================================================================
*/
bool PathIO::DoesFileExist(std::string filename)
{
	if (FILE* file = fopen(filename.c_str(), "r"))
	{
		fclose(file);
		return true;
	}
	else
	{
		return false;
	}
}

/*
================================================================================
================================================================================
*/
void PathIO::SaveSDF2CSV(std::vector<double> distArray, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	for (unsigned int a = 0; a < distArray.size(); a++)
	{
		//if (distArray[a] > 0)
		//{
		*f << std::setprecision(30) << distArray[a] << "\n";
		//}
	}

	f->close();
	delete f;
}


/*
================================================================================
================================================================================
*/
void PathIO::SaveSDF2CSV(std::vector<float> distArray, std::string filename)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	for (unsigned int a = 0; a < distArray.size(); a++)
	{
		//if (distArray[a] > 0)
		//{
		*f << std::setprecision(20) << distArray[a] << "\n";
		//}
	}

	f->close();
	delete f;
}

/*
================================================================================
================================================================================
*/
void PathIO::SaveInfo(std::string filename,
	float simulation_time,   // 1
	float fill_ratio,        // 2
	float fill_rms,          // 3
	float deformation_value, // 4
	float avg_skin_offset,   // 5
	int num_elements,        // 6
	int num_frame,           // 7
	int num_triangle_edge,   // 8
	int num_aux_edge,        // 9
	int num_points,          // 10
	int num_triangles,       // 11
	int seed)
{
	std::ofstream* f = new std::ofstream();
	f->open(filename);

	*f << "simulation_time = " << std::setprecision(20) << simulation_time << "\n";   // 1
	*f << "fill_ratio = " << std::setprecision(20) << fill_ratio << "\n";        // 2
	*f << "fill_rms = " << std::setprecision(20) << fill_rms << "\n";          // 3
	*f << "deformation_value = " << std::setprecision(20) << deformation_value << "\n"; // 4
	*f << "avg_skin_offset = " << std::setprecision(20) << avg_skin_offset << "\n";   // 5

	*f << "num_elements = " << num_elements << "\n";      // 6
	*f << "num_frame = " << num_frame << "\n";         // 7
	*f << "num_triangle_edge = " << num_triangle_edge << "\n"; // 8
	*f << "num_aux_edge = " << num_aux_edge << "\n";      // 9
	*f << "num_points = " << num_points << "\n";        // 10
	*f << "num_triangles = " << num_triangles << "\n";     // 11

	*f << "seed = " << std::setprecision(20) << seed << "\n";     // 12

	f->close();
	delete f;
}

/*
================================================================================
================================================================================
*/
void PathIO::SaveNormSDF2CSV(std::vector<float> distArray, std::string filename)
{
	float maxVal = std::numeric_limits<float>::min();
	float minVal = std::numeric_limits<float>::max();
	for (unsigned int a = 0; a < distArray.size(); a++)
	{
		if (distArray[a] > 0)
		{
			float d = distArray[a];
			if (d > maxVal) { maxVal = d; }
			if (d < minVal) { minVal = d; }
		}
	}
	float diff = maxVal - minVal;
	//std::cout << "min  " << minVal << "\n";
	//std::cout << "max  " << maxVal << "\n";
	//std::cout << "diff " << diff << "\n";

	std::ofstream* f = new std::ofstream();
	f->open(filename);

	for (unsigned int a = 0; a < distArray.size(); a++)
	{
		if (distArray[a] > 0)
		{
			float val = (distArray[a] - minVal) / diff;
			*f << std::setprecision(40) << val << "\n";
		}
	}

	f->close();
	delete f;
}