
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "MySVGRenderer.h"
#include "SystemParams.h"

#include "ClipperWrapper.h"

#include "ColorPalette.h"

#include "simple_svg.h" // external library

void MySVGRenderer::SaveTriangles(std::string filename, std::vector<AGraph> graphs)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);


	svg::Document doc2(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	for (unsigned int a = 0; a < graphs.size(); a++)
	{
		// triangles
		std::vector<AnIdxTriangle> tris = graphs[a]._triangles;
		for (unsigned int b = 0; b < tris.size(); b++)
		{
			AnIdxTriangle aTri = tris[b];
			// triangles
			svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(57, 139, 203)));

			AVector pt1 = graphs[a]._massList[aTri.idx0]._pos;
			AVector pt2 = graphs[a]._massList[aTri.idx1]._pos;
			AVector pt3 = graphs[a]._massList[aTri.idx2]._pos;

			elem << svg::Point(pt1.x, pt1.y);
			elem << svg::Point(pt2.x, pt2.y);
			elem << svg::Point(pt3.x, pt3.y);

			doc2 << elem;
		}

		// masses
		for (unsigned int b = 0; b < graphs[a]._massList.size(); b++)
		{
			AVector pt1 = graphs[a]._massList[b]._pos;
			svg::Circle circ(svg::Point(pt1.x, pt1.y), 0.1, svg::Fill(svg::Color(57, 139, 203)));
			doc2 << circ;
		}
	}

	doc2.save();
}

void MySVGRenderer::SaveElementsAndSkins(std::string filename, 
	                                     std::vector<AGraph> graphs, 
										 const std::vector<std::vector<std::vector<AVector>>>& focals,
										 const std::vector<std::vector<AVector>>& offsetFocalBoundaries,
										 std::vector<std::vector<AVector>> boundaries)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);


	svg::Document doc2(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	for (unsigned int a = 0; a < graphs.size(); a++)
	{
		// elements
		std::vector<std::vector<AVector>> arts = graphs[a]._arts;
		//for (int b = 0; b < arts.size(); b++)
		for (int b = arts.size() - 1; b >= 0; b--)
		{
			//int fIdx = graphs[a]._fColors[b];
			//int bIdx = graphs[a]._bColors[b];

			MyColor fCol = graphs[a]._fColors[b];
			MyColor bCol = graphs[a]._bColors[b];

			// arts
			svg::Polygon artElem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(58, 162, 219)));
			
			//if (bIdx >= 0 && fIdx >= 0)
			if(bCol.IsValid() && fCol.IsValid())
			{
				//MyColor bCol = ColorPalette::_palette_01[bIdx];
				//MyColor fCol = ColorPalette::_palette_01[fIdx];

				artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
			}

			//background
			//else if (bIdx >= 0)
			else if (bCol.IsValid())
			{
			//	MyColor bCol = ColorPalette::_palette_01[bIdx];
				artElem = artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color::Transparent));
			}

			// foreground
			//else if (fIdx >= 0)
			else if (fCol.IsValid())
			{
			//	MyColor fCol = ColorPalette::_palette_01[fIdx];
				artElem = svg::Polygon(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
			}

			for (unsigned int c = 0; c < arts[b].size(); c++)
			{
				AVector pt = arts[b][c];
				artElem << svg::Point(pt.x, pt.y);
			}

			doc2 << artElem;
		}

		// boundary skin
		/*svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(242, 128, 170)));
		for (int c = 0; c < graphs[a]._skin.size(); c++)
		{
			AVector pt = graphs[a]._skin[c];
			elem << svg::Point(pt.x, pt.y);
		}
		doc2 << elem;*/
		/*std::vector<AnIdxTriangle> triangles = graphs[a]._triangles;
		for (unsigned int b = 0; b < triangles.size(); b++)
		{
				//std::cout << ".";
				AnIdxTriangle tri = triangles[b];
				AVector pt0 = graphs[a]._massList[tri.idx0]._pos;
				AVector pt1 = graphs[a]._massList[tri.idx1]._pos;
				AVector pt2 = graphs[a]._massList[tri.idx2]._pos;

				svg::Polygon border(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Black));
				border << svg::Point(pt0.x, pt0.y);
				border << svg::Point(pt1.x, pt1.y);
				border << svg::Point(pt2.x, pt2.y);

				doc2 << border;
		}*/
	}

	// focal arts
	/*for (unsigned int a = 0; a < focals.size(); a++)
	{
		for (unsigned int b = 0; b < focals[a].size(); b++)
		{
			svg::Polygon artElem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(58, 162, 219)));
			for (unsigned int c = 0; c < focals[a][b].size(); c++)
			{
				AVector pt = focals[a][b][c];
				artElem << svg::Point(pt.x, pt.y);
			}
			doc2 << artElem;
		}
	}

	// focal boundaries
	for (unsigned int a = 0; a < offsetFocalBoundaries.size(); a++)
	{
		svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(242, 128, 170)));
		for (unsigned int b = 0; b < offsetFocalBoundaries[a].size(); b++)
		{
			AVector pt = offsetFocalBoundaries[a][b];
			elem << svg::Point(pt.x, pt.y);
		}
		doc2 << elem;
	}*/

	// containers	
	for (unsigned int i = 0; i < boundaries.size(); i++) //B
	{
		svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(157, 156, 158)));
		for (unsigned int c = 0; c < boundaries[i].size(); c++)
		{
			AVector pt = boundaries[i][c]; 
			elem << svg::Point(pt.x, pt.y);
		}
		doc2 << elem;
	}
	

	// clipper
	/*std::vector<std::vector<AVector>> skins;
	for (unsigned int a = 0; a < graphs.size(); a++)
	{
		skins.push_back(graphs[a]._uniArt );
	}*/

	/*std::vector<std::vector<AVector>> intersectPolys;
	std::vector<bool> orientationFlags;
	ClipperWrapper::ClippingContainer(boundaries[0], skins, intersectPolys, orientationFlags);
	*/
	// containers	
	/*for (unsigned int i = 0; i < intersectPolys.size(); i++) //B
	{
		svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(255, 100, 100)));

		if (!orientationFlags[i])
			elem = svg::Polygon(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(100, 255, 100)));


		for (unsigned int c = 0; c < intersectPolys[i].size(); c++)
		{
			AVector pt = intersectPolys[i][c];
			elem << svg::Point(pt.x, pt.y);
		}
		doc2 << elem;
	}*/

	// SAVE
	doc2.save();

}

void MySVGRenderer::SaveGraphElements(std::string filename, std::vector<AGraph> graphs)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);

	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));
	for (unsigned int a = 0; a < graphs.size(); a++)
	{
		std::vector<std::vector<AVector>> arts = graphs[a]._arts;
		for (unsigned int b = 0; b < arts.size(); b++)
		{
			svg::Polygon elem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color::Black));
			for (unsigned int c = 0; c < arts[b].size(); c++)
			{
				AVector pt = arts[b][c];
				elem << svg::Point(pt.x, pt.y);
			}
			doc << elem;
		}
	}

	doc.save();


	
}


void MySVGRenderer::SaveOrnamentsToSVG(std::string filename, 
		                               const std::vector<ArtData>& ornaments, 
								       std::vector<VFRegion> regions,
								       bool saveSimple)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	// ---------- regions ----------
	for (int iter = 0; iter < regions.size(); iter++)
	{
		std::vector<std::vector<AVector>> boundaries = regions[iter].GetBoundaries();

		for (int a = 0; a < boundaries.size(); a++)
		{
			svg::Polygon border(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Blue));
			std::vector<AVector> boundaryPts = boundaries[a];
			for (int b = 0; b < boundaryPts.size(); b++)
			{
				AVector pt = boundaryPts[b];
				border << svg::Point(pt.x, pt.y);
			}
			doc << border;
		}
	}

	// ornaments	
	for (unsigned int iter = 0; iter < ornaments.size(); iter++)
	{
		// non intersecting boundaries are simple boundaries
		// the original _boundaries can have self-intersection
		std::vector<std::vector<AVector>> boundaries;
		std::vector<int> bwFlags = ornaments[iter]._boundaryBWFlags;

		

		if (saveSimple) { boundaries = ornaments[iter]._simpleBoundaries; }
		else { boundaries = ornaments[iter]._boundaries; }

		if (bwFlags.size() == 0)
		{
			std::cout << "now bw flags\n";
			for (unsigned int a = 0; a < boundaries.size(); a++) { bwFlags.push_back(0); }
		}

		// black boundary first
		for (unsigned int a = 0; a < boundaries.size(); a++)
		{
			if (bwFlags[a] != 0) { continue; }

			svg::Polygon border(svg::Fill(svg::Color::Black), svg::Stroke(0.5f, svg::Color::Black));
			std::vector<AVector> boundaryPts = boundaries[a];
			for (unsigned int b = 0; b < boundaryPts.size(); b++)
			{
				AVector pt = boundaryPts[b];
				border << svg::Point(pt.x, pt.y);
			}
			doc << border;
		}

		// white
		for (unsigned int a = 0; a < boundaries.size(); a++)
		{
			if (bwFlags[a] != 1) { continue; }

			svg::Polygon border(svg::Fill(svg::Color::White), svg::Stroke(0.5f, svg::Color::Black));
			std::vector<AVector> boundaryPts = boundaries[a];
			for (unsigned int b = 0; b < boundaryPts.size(); b++)
			{
				AVector pt = boundaryPts[b];
				border << svg::Point(pt.x, pt.y);
			}
			doc << border;
		}
	}
	doc.save();
}

void MySVGRenderer::SaveTrianglesToSVG(std::string filename, std::vector<ArtData> ornaments)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	// boundaries	
	for (unsigned int iter = 0; iter < ornaments.size(); iter++)
	{

		std::vector<std::vector<AVector>>       triPoints = ornaments[iter]._triPoints;
		std::vector<std::vector<AnIdxTriangle>> triangles = ornaments[iter]._triangles;

		for (unsigned int b = 0; b < triangles.size(); b++)
		{
			//std::cout << "num triangle: " << 
			for (unsigned int c = 0; c < triangles[b].size(); c++)
			{
				//std::cout << ".";
				AnIdxTriangle tri = triangles[b][c];
				AVector pt0 = triPoints[b][tri.idx0];
				AVector pt1 = triPoints[b][tri.idx1];
				AVector pt2 = triPoints[b][tri.idx2];

				svg::Polygon border(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Black));
				border << svg::Point(pt0.x, pt0.y);
				border << svg::Point(pt1.x, pt1.y);
				border << svg::Point(pt2.x, pt2.y);

				doc << border;
			}
		}
	}
	doc.save();
}




void MySVGRenderer::SaveShapesToSVG(std::string filename, std::vector<std::vector<AVector>> shapes)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	for (unsigned int a = 0; a < shapes.size(); a++)
	{
		svg::Polygon border(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color::Black));
		std::vector<AVector> boundaryPts = shapes[a];
		for (unsigned int b = 0; b < boundaryPts.size(); b++)
		{
			AVector pt = boundaryPts[b];
			border << svg::Point(pt.x, pt.y);
		}
		doc << border;
	}

	doc.save();
}




void MySVGRenderer::SaveLRFunctions(std::string filename, std::vector<ALine> intersectList)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	for (unsigned int a = 0; a < intersectList.size(); a++)
	{
		AVector pt1 = intersectList[a].GetPointA();
		AVector pt2 = intersectList[a].GetPointB();

		svg::Polyline sline(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Red));
		
		sline << svg::Point(pt1.x, pt1.y);
		sline << svg::Point(pt2.x, pt2.y);

		doc << sline;
	}

	doc.save();
}

void MySVGRenderer::SaveDijkstraDebugImageLOL(std::string filename, 
	                                          std::vector<std::vector<AVector>> myGraph, 
											  std::vector<AVector> blobBoundary, 
											  std::vector<AVector> regionBoundary,
											  std::vector<AVector> cubicCurve,
											  std::vector<AVector> shortestPath,
											  std::vector<AVector> oldStreamline,
											  AVector startPt,
											  AVector endPt)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	// blob boundary
	svg::Polygon border1(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Black));
	for (unsigned int a = 0; a < blobBoundary.size(); a++)
		{ border1 << svg::Point(blobBoundary[a].x, blobBoundary[a].y); }
	doc << border1;
	
	// region boundary
	svg::Polygon border2(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Black));
	for (unsigned int a = 0; a < regionBoundary.size(); a++)
		{ border2 << svg::Point(regionBoundary[a].x, regionBoundary[a].y); }
	doc << border2;

	// graphs
	for (int iter = 0; iter < myGraph.size(); iter++)
	{
		std::vector<AVector> streamline = myGraph[iter];
		/*svg::Polyline line(svg::Fill(svg::Color::Transparent), svg::Stroke(0.1f, svg::Color::Orange));
		for (int a = 0; a < streamline.size(); a++)
			{ line << svg::Point(streamline[a].x, streamline[a].y); }
		doc << line;*/
		for (unsigned int a = 0; a < streamline.size(); a++)
		{
			//line << svg::Point(streamline[a].x, streamline[a].y);
			svg::Circle sCircle1(svg::Point(streamline[a].x, streamline[a].y), 0.5, svg::Fill(svg::Color::Orange), svg::Stroke(0.1f, svg::Color::Transparent));
			doc << sCircle1;
		}
	}

	

	// old streamline
	svg::Polyline oline(svg::Fill(svg::Color::Transparent), svg::Stroke(0.2f, svg::Color::Green));
	for (unsigned int a = 0; a < oldStreamline.size(); a++)
		{ oline << svg::Point(oldStreamline[a].x, oldStreamline[a].y); }
	doc << oline;

	// shrtest path
	svg::Polyline sline(svg::Fill(svg::Color::Transparent), svg::Stroke(0.2f, svg::Color::Yellow));
	for (unsigned int a = 0; a < shortestPath.size(); a++)
		{ sline << svg::Point(shortestPath[a].x, shortestPath[a].y); }
	doc << sline;

	// cubic curve
	svg::Polyline cline(svg::Fill(svg::Color::Transparent), svg::Stroke(0.2f, svg::Color::Red));
	for (unsigned int a = 0; a < cubicCurve.size(); a++)
	{ cline << svg::Point(cubicCurve[a].x, cubicCurve[a].y); }
	doc << cline;


	// start
	svg::Circle sCircle1(svg::Point(startPt.x, startPt.y), 0.5, svg::Fill(svg::Color::Red), svg::Stroke(0.1f, svg::Color::Transparent));
	doc << sCircle1;
	// finish
	svg::Circle sCircle2(svg::Point(endPt.x, endPt.y), 0.5, svg::Fill(svg::Color::Blue), svg::Stroke(0.1f, svg::Color::Transparent));
	doc << sCircle2;

	doc.save();
}

void MySVGRenderer::LineUpArts(std::string filename,
		                       std::vector<GraphArt> oArts_array,
						       std::vector<std::vector<GraphArt>> dArts_array,
						       std::vector<ArtColors> fColors_array,
							   std::vector<ArtColors> bColors_array)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	// ori
	for (unsigned iter = 0; iter < oArts_array.size(); iter++)
	{
		ArtColors fColors = fColors_array[iter];
		ArtColors bColors = bColors_array[iter];
		GraphArt arts = oArts_array[iter];

		// ori
		for (int a = arts.size() - 1; a >= 0; a--)
		{
			//int fIdx = fColors[a];
			//int bIdx = bColors[a];			
			MyColor fCol = fColors[a];
			MyColor bCol = bColors[a];
			svg::Polygon artElem = (svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(58, 162, 219))); // arts
			//if (bIdx >= 0 && fIdx >= 0)
			
			if (fCol.IsValid() && bCol.IsValid())
			{
				//MyColor bCol = ColorPalette::_palette_01[bIdx];
				//MyColor fCol = ColorPalette::_palette_01[fIdx];
				artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
			}
			//background
			//else if (bIdx >= 0)
			else if (bCol.IsValid())
			{
			//	MyColor bCol = ColorPalette::_palette_01[bIdx];
				artElem = artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color::Transparent));
			}
			// foreground
			//else if (fIdx >= 0)
			else if (fCol.IsValid())
			{
			//	MyColor fCol = ColorPalette::_palette_01[fIdx];
				artElem = svg::Polygon(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
			}
			for (unsigned int b = 0; b < arts[a].size(); b++)
			{
				AVector pt = arts[a][b];
				artElem << svg::Point(pt.x, pt.y);
			}
			doc << artElem;
		}
	}

	// deformed
	for (unsigned iter1 = 0; iter1 < dArts_array.size(); iter1++)
	{
		ArtColors fColors = fColors_array[iter1];
		ArtColors bColors = bColors_array[iter1];
		for (unsigned iter2 = 0; iter2 < dArts_array[iter1].size(); iter2++)
		{
			GraphArt arts = dArts_array[iter1][iter2];
			for (int a = arts.size() - 1; a >= 0; a--)
			{
				//int fIdx = fColors[a];
				//int bIdx = bColors[a];
				MyColor fCol = fColors[a];
				MyColor bCol = bColors[a];
				svg::Polygon artElem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(58, 162, 219))); // arts
				//if (bIdx >= 0 && fIdx >= 0)
				if (fCol.IsValid() && bCol.IsValid())
				{
					//MyColor bCol = ColorPalette::_palette_01[bIdx];
					//MyColor fCol = ColorPalette::_palette_01[fIdx];

					artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
				}
				//background
				//else if (bIdx >= 0)
				else if (bCol.IsValid())
				{
				//	MyColor bCol = ColorPalette::_palette_01[bIdx];
					artElem = artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color::Transparent));
				}
				// foreground
				//else if (fIdx >= 0)
				else if (fCol.IsValid())
				{
				//	MyColor fCol = ColorPalette::_palette_01[fIdx];
					artElem = svg::Polygon(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
				}
				for (unsigned int b = 0; b < arts[a].size(); b++)
				{
					AVector pt = arts[a][b];
					artElem << svg::Point(pt.x, pt.y);
				}
				doc << artElem;
			}
		}
	}

	doc.save();
}

void MySVGRenderer::SaveArts(std::string filename,
							 std::vector<std::vector<AVector>> arts,
							 std::vector<int> fColors,
							 std::vector<int> bColors)
{
	svg::Dimensions dimensions(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor);
	svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::TopLeft));

	for (int a = arts.size() - 1; a >= 0; a--)
	{
		int fIdx = fColors[a];
		int bIdx = bColors[a];

		// arts
		svg::Polygon artElem(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(58, 162, 219)));
		//background and foreground
		if (bIdx >= 0 && fIdx >= 0)
		{
			MyColor bCol = ColorPalette::_palette_01[bIdx];
			MyColor fCol = ColorPalette::_palette_01[fIdx];

			artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
		}

		//background
		else if (bIdx >= 0)
		{
			MyColor bCol = ColorPalette::_palette_01[bIdx];
			artElem = artElem = svg::Polygon(svg::Fill(svg::Color(bCol._r, bCol._g, bCol._b)), svg::Stroke(0.5f, svg::Color::Transparent));
		}

		// foreground
		else if (fIdx >= 0)
		{
			MyColor fCol = ColorPalette::_palette_01[fIdx];
			artElem = svg::Polygon(svg::Fill(svg::Color::Transparent), svg::Stroke(0.5f, svg::Color(fCol._r, fCol._g, fCol._b)));
		}

		
		for (unsigned int b = 0; b < arts[a].size(); b++)
		{
			AVector pt = arts[a][b];
			artElem << svg::Point(pt.x, pt.y);
		}
		doc << artElem;
	}

	doc.save();
}