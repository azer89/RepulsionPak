
#include "ADistanceTransform.h"
#include "UtilityFunctions.h"
#include "PathIO.h"
#include "CollissionGrid.h"

ADistanceTransform::ADistanceTransform(//const std::vector<AGraph>& graphs, 
	                                   const std::vector<std::vector<AVector>>& containers,
									   const std::vector<std::vector<AVector>>& holes,
									   const std::vector<std::vector<AVector>>& focals,
									   float scale) //B
{
	//_avgSkinThickness = 0;
	_maxDist = 0;

	// copy
	_containers = containers;

	_scale = scale;
	_sz = SystemParams::_upscaleFactor * _scale;

	int szsz = _sz * _sz;
	_containerDistArray = std::vector<float>(szsz);
	_distArray = std::vector<float>(szsz);
	for (unsigned int a = 0; a < szsz; a++)
	{
		_distArray[a] = 0;
	}

	// container 
	// currently only support one container
	std::vector<std::vector<cv::Point2f>> cvContainers;
	for (unsigned int a = 0; a < containers.size(); a++)
	{
		cvContainers.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(containers[a]));
	}
	//std::vector<cv::Point2f> cvContainer = _cvWrapper.ConvertList<AVector, cv::Point2f>(containers[0]);//B
	std::vector<std::vector<cv::Point2f>> cvHoles;
	for (int a = 0; a < focals.size(); a++)
		{ cvHoles.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(focals[a])); }
	for (int a = 0; a < holes.size(); a++)
	{
		cvHoles.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(holes[a]));
	}
	_container_size = 0;
	//for (int y = 0; y < _containerDistImage.rows; y++)
	for (int xIter = 0; xIter < _sz; xIter++)
	{
		//for (int x = 0; x < _containerDistImage.cols; x++)
		for (int yIter = 0; yIter < _sz; yIter++)
		{
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

			//float d = cv::pointPolygonTest(cvContainer, cv::Point2f(xActual, yActual), true);
			float d = -1;
			for (unsigned int a = 0; a < containers.size(); a++)
			{
				float ds = cv::pointPolygonTest(cvContainers[a], cv::Point2f(xActual, yActual), true);
				if (ds >= 0)
				{
					d = ds;
					break;
				}
			}

			// ===== MULTIPLE CONTAINERS =====
			// 2 UNCOMMENT THIS FOR MULTIPLE BOUNDARIES
			/*for (int a = 0; a < cvHoles.size(); a++)
			{
				float d2 = cv::pointPolygonTest(cvHoles[a], cv::Point2f(xActual, yActual), true);
				if (d2 >= 0 && d < 0)
				{
					d = d2;
					break;
				}
			}*/
			//if (d >= 0)  { _container_size++; }
			//_containerDistArray[xIter + yIter * _sz] = d;


			// ===== MULTIPLE CONTAINERS =====
			// 1 UNCOMMENT THIS FOR HOLES
			
			bool isInsideHole = false;
			for (int a = 0; a < cvHoles.size(); a++)
			{
				float d2 = cv::pointPolygonTest(cvHoles[a], cv::Point2f(xActual, yActual), true);
				if (d2 > 0){ isInsideHole = true; break; }
			}

			if (d >= 0 && !isInsideHole)  
			{ 
				_container_size++; 
			}

			_containerDistArray[xIter + yIter * _sz] = d; // _containerDistArray
			 
			if (isInsideHole) { _containerDistArray[xIter + yIter * _sz] = -1; } // _containerDistArray
		}
	}

	//PathIO pathIO;
	///=== pathIO.SaveSDF2CSV(thinningDistArray, SystemParams::_save_folder + "dist.csv");
	

	//_fill_img_template
	_fill_img_template.CreateIntegerImage(_sz);
	for (unsigned int x = 0; x < _sz; x++)
	{
		for (unsigned int y = 0; y < _sz; y++)
		{
			//float x = ((float)xIter) / ((float)SystemParams::_upscaleFactor);
			//float y = ((float)yIter) / ((float)SystemParams::_upscaleFactor);

			//if (_containerDistImage.at<float>(y, x) >= 0)  
			if (_containerDistArray[x + y * _sz] >= 0)
				{ _fill_img_template.SetIntegerPixel(x, y, 1); } // inside boundary
			else { _fill_img_template.SetIntegerPixel(x, y, 0); }

		}
	}
}

//ADistanceTransform::ADistanceTransform(//const std::vector<std::vector<AVector>> graphs,
//									   const std::vector<std::vector<AVector>>& containers,
//									   const std::vector<std::vector<AVector>>& focals,
//									   float scale)
//{
//	// copy
//	_containers = containers;
//
//	_scale = scale;
//	_sz = SystemParams::_upscaleFactor * _scale;
//
//	_containerDistArray = std::vector<float>(_sz * _sz);
//	_distArray = std::vector<float>(_sz * _sz);
//
//	//_distImage = cv::Mat::zeros(sz, sz, CV_32FC1);
//	//_containerDistImage = cv::Mat::zeros(sz, sz, CV_32FC1);
//
//	// graph boundaries
//	/*for (int a = 0; a < graphs.size(); a++)
//	{
//		_graphBoundaries.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(graphs[a]));
//		_voronoiColors.push_back(MyColor(rand() % 254 + 1, rand() % 254 + 1, rand() % 254 + 1));// voronoi
//		
//		CVImg img; // voronoi
//		img.CreateGrayscaleImage(_sz);  // voronoi
//		img.SetGrayscaleImageToBlack(); // voronoi
//		_voronoiBWImages.push_back(img);  // voronoi
//	}*/
//	//{ _graphBoundaries.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(graphs[a]._boundary)); }
//
//	// container 
//	// currently only support one container
//	std::vector<cv::Point2f> cvContainer = _cvWrapper.ConvertList<AVector, cv::Point2f>(containers[0]);//B
//	std::vector<std::vector<cv::Point2f>> cvHoles;
//	for (int a = 0; a < focals.size(); a++)
//	{
//		cvHoles.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(focals[a]));
//	}
//	_container_size = 0;
//	//for (int y = 0; y < _containerDistImage.rows; y++)
//	for (int xIter = 0; xIter < _sz; xIter++)
//	{
//		//for (int x = 0; x < _containerDistImage.cols; x++)
//		for (int yIter = 0; yIter < _sz; yIter++)
//		{
//			float xActual = ((float)xIter) / _scale;
//			float yActual = ((float)yIter) / _scale;
//
//			float d = cv::pointPolygonTest(cvContainer, cv::Point2f(xActual, yActual), true);
//
//			bool isInsideHole = false;
//			for (int a = 0; a < cvHoles.size(); a++)
//			{
//				float d2 = cv::pointPolygonTest(cvHoles[a], cv::Point2f(xActual, yActual), true);
//				if (d2 > 0){ isInsideHole = true; break; }
//			}
//
//			if (d >= 0 && !isInsideHole)  { _container_size++; }
//
//			_containerDistArray[xIter + yIter * _sz] = d;
//			if (isInsideHole) { _containerDistArray[xIter + yIter * _sz] = -1; }
//		}
//	}
//
//	//_fill_img_template
//	_fill_img_template.CreateIntegerImage(_sz);
//	for (unsigned int x = 0; x < _sz; x++)
//	{
//		for (unsigned int y = 0; y < _sz; y++)
//		{
//			//float x = ((float)xIter) / ((float)SystemParams::_upscaleFactor);
//			//float y = ((float)yIter) / ((float)SystemParams::_upscaleFactor);
//
//			//if (_containerDistImage.at<float>(y, x) >= 0)  
//			if (_containerDistArray[x + y * _sz] >= 0)
//			{
//				_fill_img_template.SetIntegerPixel(x, y, 1);
//			} // inside boundary
//			else { _fill_img_template.SetIntegerPixel(x, y, 0); }
//
//		}
//	}
//}


ADistanceTransform::~ADistanceTransform()
{	
}


void ADistanceTransform::AddGraph(const std::vector<AVector>& graphs)
{
	_graphBoundaries.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(graphs));
	_voronoiColors.push_back(MyColor(rand() % 254 + 1, rand() % 254 + 1, rand() % 254 + 1));// voronoi

	CVImg img; // voronoi
	img.CreateGrayscaleImage(_sz);  // voronoi
	img.SetGrayscaleImageToBlack(); // voronoi
	_voronoiBWImages.push_back(img);  // voronoi
}


void ADistanceTransform::AddGraph(const AGraph& aGraph)
{
	_graphBoundaries.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(aGraph._uniArt));
	_voronoiColors.push_back(MyColor(rand() % 254 + 1, rand() % 254 + 1, rand() % 254 + 1));// voronoi
		
	CVImg img; // voronoi
	img.CreateGrayscaleImage(_sz);  // voronoi
	img.SetGrayscaleImageToBlack(); // voronoi
	_voronoiBWImages.push_back(img);  // voronoi
}

void ADistanceTransform::UpdateBoundaries(const std::vector<AGraph>& graphs)
{
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
	{
		for (unsigned int b = 0; b < _graphBoundaries[a].size(); b++)
		{
			_graphBoundaries[a][b].x = graphs[a]._uniArt[b].x;
			_graphBoundaries[a][b].y = graphs[a]._uniArt[b].y;
		}
	}
}

// use this
void ADistanceTransform::CalculateFill(CollissionGrid* cGrid, 
	                                   float& fill_ratio, 
									   int numIter, 
									   bool saveImage)
{
	std::cout << "don't use CalculateFill\n";
	CVImg fillImg;
	fillImg._img = _fill_img_template._img.clone(); // need to be cloned
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
		{ _cvWrapper.DrawFilledPolyInt(fillImg, _graphBoundaries[a], 0, _scale); } // scaled

	int fill_counter = _container_size;
	for (unsigned int x = 0; x < _sz; x++)
	{
		for (unsigned int y = 0; y < _sz; y++)
		{
			fill_counter -= fillImg.GetIntegerValue(x, y);
		}
	}
	fill_ratio = ((float)fill_counter) / ((float)_container_size);

	// SAVE ?
	if (saveImage)
	{
		std::stringstream ss;
		ss << SystemParams::_save_folder << "FILL\\" << "fill_" << numIter << ".png";
		fillImg.SaveIntegerImage(ss.str());
	}

}

void ADistanceTransform::CalculateSDF2(const std::vector<AGraph>& graphs, CollissionGrid* cGrid, int numIter, bool saveImage)
{
	int szsz = _sz * _sz;
	cGrid->PrecomputeGraphIndices();

	for (unsigned int xIter = 0; xIter < _sz; xIter++)
	{
		for (unsigned int yIter = 0; yIter < _sz; yIter++)
		{
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

			int idxxxx = xIter + yIter * _sz;

			float containerDistVal = _containerDistArray[idxxxx];

			std::vector<int> graphIndices;
			cGrid->GetGraphIndices1B(xActual, yActual, graphIndices);

			// if no closest graph and outside
			if (graphIndices.size() == 0 && containerDistVal < 0)
			{ continue; } // (Overlap Mask) outside container

			float minDist = containerDistVal;
			bool isInside = false;
			for (unsigned int a = 0; a < graphIndices.size(); a++)
			{
				int gIdx = graphIndices[a];
				if (gIdx >= _graphBoundaries.size()) { continue; } // exclude filling elements
				
				//float d = -cv::pointPolygonTest(_graphBoundaries[gIdx], cv::Point2f(xActual, yActual), true);
				float d = graphs[gIdx].DistToArts(AVector(xActual, yActual));

				// // hack
				if (graphs[gIdx].InsideArts(AVector(xActual, yActual))) /*{ d = -d; }*/ { d = -1; }
				
				if (d <= 0) // inside
				{
					minDist = 0; // SDF
					isInside = true;
				}

				if (d < minDist) // outside
				{
					minDist = d;
				} // SDF

				if (isInside) { break; } // no point to continue
			}
			
			if (!isInside && containerDistVal < 0)
			{
				//overlapMask[xIter + yIter * _sz] = -1;  // (Overlap Mask) outside element outside container
				minDist = 0; // (SDF)
			}


			// no closest element AND inside container
			else if (graphIndices.size() == 0 && containerDistVal > 0)
				//{ minDist = 0; }
			{
				minDist = containerDistVal;
			}
			else if (graphIndices.size() == 0 && containerDistVal < 0) // ?? BUG ???
			{
				minDist = -1;
			}

			_distArray[idxxxx] = minDist;
		}
	}

	std::stringstream ss6;
	ss6 << "dist_" << numIter;
	DebugDistanceImage(ss6.str());

	if (SystemParams::_output_files)
	{
		PathIO pathIO;
		pathIO.SaveSDF2CSV(_distArray, SystemParams::_save_folder + "dist_all.csv");
		pathIO.SaveSDF2CSV(_containerDistArray, SystemParams::_save_folder + "dist_mask.csv");
	}



	// create new elements !!!!
	CalculatePeaks();

	// ---------- global max distance _maxDist ----------
	_maxDist = std::numeric_limits<float>::min();
	for (unsigned int a = 0; a < _distArray.size(); a++)
	{
		if (_maxDist < _distArray[a]) { _maxDist = _distArray[a]; }
	}


}

// use this
// 
void ADistanceTransform::CalculateSDF1(CollissionGrid* cGrid, int numIter, bool saveImage)
{
	int szsz = _sz * _sz;
	//std::vector<int> overlapMask(szsz);

	cGrid->PrecomputeGraphIndices();

	for (unsigned int xIter = 0; xIter < _sz; xIter++)
	{
		for (unsigned int yIter = 0; yIter < _sz; yIter++)
		{			
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

			int idxxxx = xIter + yIter * _sz;

			float containerDistVal = _containerDistArray[idxxxx];

			std::vector<int> graphIndices;
			//cGrid->GetGraphIndices1(xActual, yActual, graphIndices);
			cGrid->GetGraphIndices1B(xActual, yActual, graphIndices);

			// if no closest graph and outside
			if (graphIndices.size() == 0 && containerDistVal < 0)
				{ /*overlapMask[xIter + yIter * _sz] = -1;*/  continue;} // (Overlap Mask) outside container

			//overlapMask[xIter + yIter * _sz] = 0; // (Overlap Mask) 

			////// exclude container
			//float minDist = std::numeric_limits<float>::max(); 
			////// include container
			float minDist = containerDistVal; 
			bool isInside = false;
			for (unsigned int a = 0; a < graphIndices.size(); a++)
			{
				int gIdx = graphIndices[a];
				if (gIdx >= _graphBoundaries.size()) { continue; } // exclude filling elements
				float d = -cv::pointPolygonTest(_graphBoundaries[gIdx], cv::Point2f(xActual, yActual), true);

				// hack
				if (d < 0) { d = -1; }

				if (d <= 0) // inside
				{
					minDist = 0; // SDF
					//overlapMask[xIter + yIter * _sz]++; // (Overlap Mask) 
					isInside = true;					
				}

				if (d < minDist) // outside
					{ minDist = d; } // SDF

				if (isInside) { break; } // no point to continue
			}

			
			//if (isInside && containerDistVal < 0)
			//	{ /*overlapMask[xIter + yIter * _sz] = 2;*/  } // (Overlap Mask) inside an element and outside container
			
			/*else*/ if (!isInside && containerDistVal < 0)
			{ 
				//overlapMask[xIter + yIter * _sz] = -1;  // (Overlap Mask) outside element outside container
				minDist = 0; // (SDF)
			}
			//else if (!isInside && containerDistVal > 0)
			//{
			//	minDist = containerDistVal;
			//}
			
			// no closest element AND inside container
			else if (graphIndices.size() == 0 && containerDistVal > 0)
				//{ minDist = 0; }
				{ minDist = containerDistVal; }
			else if (graphIndices.size() == 0 && containerDistVal < 0) // ?? BUG ???
			    { minDist = -1; }

			_distArray[idxxxx] = minDist;
		}
	}

	std::stringstream ss6;
	ss6 << "dist_" << numIter;
	DebugDistanceImage(ss6.str());

	if (SystemParams::_output_files)
	{
		PathIO pathIO;
		pathIO.SaveSDF2CSV(_distArray, SystemParams::_save_folder + "dist_all.csv");
		pathIO.SaveSDF2CSV(_containerDistArray, SystemParams::_save_folder + "dist_mask.csv");
	}



	// create new elements !!!!
	CalculatePeaks();

	// ---------- global max distance _maxDist ----------
	_maxDist = std::numeric_limits<float>::min();
	for (unsigned int a = 0; a < _distArray.size(); a++)
	{
		if (_maxDist < _distArray[a]) { _maxDist = _distArray[a]; }
	}


}

CVImg ADistanceTransform::SkeletonDraw(std::string imageName)
{
	CVImg thinningImage;
	thinningImage.CreateGrayscaleImage(_sz, _sz);

	OpenCVWrapper cvWrapper;
	for (unsigned int a = 0; a < _manualSkeletons.size(); a++)
	{
		for (unsigned int b = 0; b < _manualSkeletons[a].size() - 1; b++)
		{
			AVector pt1 = _manualSkeletons[a][b] * _scale;
			AVector pt2 = _manualSkeletons[a][b + 1] * _scale;
			cv::line(thinningImage._img, cv::Point2f(pt1.x, pt1.y), cv::Point2f(pt2.x, pt2.y), 255, 1);
		}
		
	}
	thinningImage.SaveImage(SystemParams::_save_folder + "SDF\\" + imageName + ".png");
	return thinningImage;
}

CVImg ADistanceTransform::VoronoiSkeleton(CollissionGrid* cGrid, int numIter)
{
	// reset
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
		{ _voronoiBWImages[a].SetGrayscaleImageToBlack(); }

	CVImg voronoiImage;
	voronoiImage.CreateColorImage(_sz, _sz);
	for (int xIter = 0; xIter < _sz; xIter++)
	{
		for (int yIter = 0; yIter < _sz; yIter++)
		{
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

			float containerDistVal = _containerDistArray[xIter + yIter * _sz];

			std::vector<int> graphIndices;
			cGrid->GetGraphIndices1(xActual, yActual, graphIndices);

			// if outside
			if (containerDistVal < 0)
			{ 
				voronoiImage.SetColorPixel(xIter, yIter, MyColor(0)); 
				continue;
			}

			// no closest graph
			if (graphIndices.size() == 0)
			{
				voronoiImage.SetColorPixel(xIter, yIter, MyColor(0));
				continue;
			}

			float minDist = std::numeric_limits<float>::max();
			float idx = -1;
			for (unsigned int a = 0; a < graphIndices.size(); a++)
			{
				int gIdx = graphIndices[a];
				if (gIdx >= _graphBoundaries.size()) { continue; } // exclude filling elements

				float d = -cv::pointPolygonTest(_graphBoundaries[gIdx], cv::Point2f(xActual, yActual), true);
				
				if (d <= 0) // inside
				{ 
					idx = gIdx;
					voronoiImage.SetColorPixel(xIter, yIter, _voronoiColors[gIdx]);
					break; 
				}
				
				if (d < minDist) // outside 
				{ 
					idx = gIdx;
					voronoiImage.SetColorPixel(xIter, yIter, _voronoiColors[gIdx]);
					minDist = d; 
				}
			}
			_voronoiBWImages[idx].SetGrayValue(xIter, yIter, 255);

		} // yIter
	} // xIter

	// find contour
	OpenCVWrapper cvWrapper;
	std::vector<std::vector<AVector>> vooronoiContours;
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
	{
		std::vector<AVector> contour = cvWrapper.GetContour(_voronoiBWImages[a]);
		vooronoiContours.push_back(contour);
		//std::stringstream ss2;
		//ss2 << "voronoi_color" << numIter << "_" << a;
		//ss2 << "voronoi_color_" << a;
		//_voronoiBWImages[a].SaveImage(SystemParams::_save_folder + "SDF\\Debug\\" + ss2.str() + ".png");
	}

	CVImg voronoiEdgeImage;
	voronoiEdgeImage.CreateGrayscaleImage(_sz, _sz);
	voronoiEdgeImage.SetGrayscaleImageToBlack();
	float lineWidth = 3;
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
	{
		std::vector<cv::Point2f> cvContours = cvWrapper.ConvertList<AVector, cv::Point2f>(vooronoiContours[a]);
		for (size_t b = 0; b < cvContours.size() - 1; b++)
			{ cv::line(voronoiEdgeImage._img, cvContours[b], cvContours[b + 1], 255, lineWidth); }
		{ cv::line(voronoiEdgeImage._img, cvContours[cvContours.size() - 1], cvContours[0], 255, lineWidth); }
		//cvWrapper.DrawPolyOnCVImage(voronoiEdgeImage._img, vooronoiContours[a], _voronoiColors[a], true, 1);
	}
	CVImg thinningImage = voronoiEdgeImage.ThinningFromGrayscale();
	
	// cleanup
	for (int xIter = 0; xIter < _sz; xIter++)
	{
		for (int yIter = 0; yIter < _sz; yIter++)
		{
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

			float d = UtilityFunctions::DistanceToClosedCurves(_containers, AVector(xActual, yActual));
			if (thinningImage.GetGrayValue(xIter, yIter) > 0 && d < 1.0f)
				{ thinningImage.SetGrayValue(xIter, yIter, 0); }
		}
	}
	
	std::stringstream ss1;
	ss1 << "voronoi_skeleton_" << numIter;
	thinningImage.SaveImage( SystemParams::_save_folder + "SDF\\" + ss1.str() + ".png" );

	std::stringstream ss2;
	ss2 << "voronoi_color" << numIter;
	voronoiImage.SaveImage( SystemParams::_save_folder + "SDF\\" + ss2.str() + ".png" );

	//CVImg thinningImg;
	return thinningImage;
}

CVImg ADistanceTransform::SkeletonDistance(std::vector<int> overlapMask, std::string imageName)
{
	CVImg emptyImage;
	emptyImage.CreateGrayscaleImage(_sz, _sz);
	for (int x = 0; x < _sz; x++)
	{
		for (int y = 0; y < _sz; y++)
		{
			emptyImage.SetGrayValue(x, y, 0);
			int val = overlapMask[x + y * _sz];
			if (val == 0) // empty space
			{
				emptyImage.SetGrayValue(x, y, 255);
			}
		}
	}
	///////////
	// BLURRING
	///////////
	if (SystemParams::_skin_thickness > 0)
	{
		int blurSize = SystemParams::_skin_thickness * _scale;
		if (blurSize < 3) { blurSize = 3; }
		if (blurSize % 2 == 0) { blurSize++; }
		std::cout << "blur " << blurSize << "\n";
		cv::blur(emptyImage._img, emptyImage._img, cv::Size(blurSize, blurSize));
		emptyImage.SaveImage(SystemParams::_save_folder + "SDF\\" + imageName + "_blur.png");
		for (int x = 0; x < _sz; x++)
		{
			for (int y = 0; y < _sz; y++)
			{
				if (emptyImage.GetGrayValue(x, y) < 127) { emptyImage.SetGrayValue(x, y, 0); }
				else { emptyImage.SetGrayValue(x, y, 255); }
			}
		}
		std::cout << "blur";
	}
	///////////
	// THINNING
	///////////
	CVImg thinningImage = emptyImage.ThinningFromGrayscale();
	thinningImage.SaveImage( SystemParams::_save_folder + "SDF\\" + imageName + ".png" );
	emptyImage.SaveImage( SystemParams::_save_folder + "SDF\\" + imageName + "_mask.png" );
	return thinningImage;
}

void ADistanceTransform::DebugOverlapMask(std::vector<int> overlapMask, CVImg thinningImage, std::string imageName)
{

	cv::Mat drawing = cv::Mat::zeros(cv::Size(_sz, _sz), CV_8UC3);

	for (int x = 0; x < _sz; x++)
	{
		for (int y = 0; y < _sz; y++)
		{
			int val = overlapMask[x + y * _sz];
			if (val == -1) // outside container
			{
				drawing.at<cv::Vec3b>(y, x)[0] = 255;
				drawing.at<cv::Vec3b>(y, x)[1] = 255;
				drawing.at<cv::Vec3b>(y, x)[2] = 255; 
			}
			else if (val == 0) // empty space
			{
				//179,229,252
				drawing.at<cv::Vec3b>(y, x)[0] = 252; 
				drawing.at<cv::Vec3b>(y, x)[1] = 229; 
				drawing.at<cv::Vec3b>(y, x)[2] = 179; 
			}
			else if (val == 1) // inside skin
			{
				// 2,119,189
				drawing.at<cv::Vec3b>(y, x)[0] = 189;
				drawing.at<cv::Vec3b>(y, x)[1] = 119;
				drawing.at<cv::Vec3b>(y, x)[2] = 2;
			}
			else if (val >= 2) // skin overlap or protrude outside container
			{
				// 255,82,82
				drawing.at<cv::Vec3b>(y, x)[0] = 82; 
				drawing.at<cv::Vec3b>(y, x)[1] = 82; 
				drawing.at<cv::Vec3b>(y, x)[2] = 255;
			}
		}
	}

	for (int x = 0; x < _sz; x++)
	{
		for (int y = 0; y < _sz; y++)
		{
			if (thinningImage.GetGrayValue(x, y) > 0)
			{
				cv::circle(drawing, cv::Point(x, y), _scale, cv::Scalar(32, 124, 244), -1, 8);
				//drawing.at<cv::Vec3b>(y, x)[0] = 0;
				//drawing.at<cv::Vec3b>(y, x)[1] = 255;
				//drawing.at<cv::Vec3b>(y, x)[2] = 0;
			}
		}
	}

	cv::imwrite( SystemParams::_save_folder + "SDF\\" + imageName + ".png" , drawing);
}

void ADistanceTransform::DebugDistanceImage(std::string imageName)
{
	/*double minVal = std::numeric_limits<float>::max();
	double maxVal = std::numeric_limits<float>::min();
	for (unsigned a = 0; a < _distArray.size(); a++)
	{
		float d = _distArray[a];
		if (d < minVal) { minVal = d; }
		if (d > maxVal) { maxVal = d; }
	}
	//cv::minMaxLoc(distImage, &minVal, &maxVal, 0, 0, cv::Mat());


	minVal = abs(minVal); maxVal = abs(maxVal);*/
	double minVal = 20;
	double maxVal = 20;

	//std::cout << minVal << " " << maxVal << "\n";
	//int intMaxVal = maxVal;

	/// Depicting the  distances graphically
	//int sz = SystemParams::_upscaleFactor;
	cv::Mat drawing = cv::Mat::zeros(_sz, _sz, CV_8UC3);

	//for (int j = 0; j < distImage.rows; j++)
	for (int j = 0; j < _sz; j++)
	{
		//for (int i = 0; i < distImage.cols; i++)
		for (int i = 0; i < _sz; i++)
		{
			float d = _distArray[i + j * _sz];
			if (d < 0)
			{
				if (d < -20) d = -20;

				drawing.at<cv::Vec3b>(j, i)[0] = 255 - (int)abs(d) * 255 / minVal;
			}
			else if (d > 0)
			{
				if (d > 20) d = 20;

				drawing.at<cv::Vec3b>(j, i)[2] = 255 - (int)d * 255 / maxVal;
			}
			else
			{
				drawing.at<cv::Vec3b>(j, i)[0] = 255;
				drawing.at<cv::Vec3b>(j, i)[1] = 255;
				drawing.at<cv::Vec3b>(j, i)[2] = 255;
			}
		}
	}

	// debug
	//std::cout << "debug\n";
	cv::imwrite( SystemParams::_save_folder + "SDF\\" + imageName + ".png" , drawing);
}


void  ADistanceTransform::DebugDistanceImage(CVImg thinningImage, std::string imageName)
{
	/*double minVal = std::numeric_limits<float>::max(); 
	double maxVal = std::numeric_limits<float>::min();
	for (unsigned a = 0; a < _distArray.size(); a++)
	{
		float d = _distArray[a];
		if (d < minVal) { minVal = d; }
		if (d > maxVal) { maxVal = d; }
	}
	//cv::minMaxLoc(distImage, &minVal, &maxVal, 0, 0, cv::Mat());
	
	
	minVal = abs(minVal); maxVal = abs(maxVal);*/
	double minVal = 20;
	double maxVal = 20;

	//std::cout << minVal << " " << maxVal << "\n";
	//int intMaxVal = maxVal;

	/// Depicting the  distances graphically
	//int sz = SystemParams::_upscaleFactor;
	cv::Mat drawing = cv::Mat::zeros(_sz, _sz, CV_8UC3);

	//for (int j = 0; j < distImage.rows; j++)
	for (int j = 0; j < _sz; j++)
	{
		//for (int i = 0; i < distImage.cols; i++)
		for (int i = 0; i < _sz; i++)
		{
			/*if (distImage.at<float>(j, i) > maxVal * 0.8)
			{
				drawing.at<cv::Vec3b>(j, i)[0] = 0;
				drawing.at<cv::Vec3b>(j, i)[1] = 255;
				drawing.at<cv::Vec3b>(j, i)[2] = 0;
			}
			else */
			float d = _distArray[i + j * _sz];
			if (d < 0)
			{
				if (d < -20) d = -20;

				drawing.at<cv::Vec3b>(j, i)[1] = 255 - (int)abs(d) * 255 / minVal;
			}
			else if (d > 0)
			{
				if (d > 20) d = 20;

				drawing.at<cv::Vec3b>(j, i)[2] = 255 - (int)d * 255 / maxVal;
			}
			else
			{
				drawing.at<cv::Vec3b>(j, i)[0] = 255; 
				drawing.at<cv::Vec3b>(j, i)[1] = 255; 
				drawing.at<cv::Vec3b>(j, i)[2] = 255;
			}
		}
	}

	/*for (int x = 0; x < _sz; x++)
	{
		for (int y = 0; y < _sz; y++)
		{
			if (thinningImage.GetGrayValue(x, y) > 0)
			{
				//cv::circle(drawing, cv::Point(x, y), 3 * _scale, cv::Scalar(32, 124, 244), -1, 8);
				drawing.at<cv::Vec3b>(y, x)[0] = 0;
				drawing.at<cv::Vec3b>(y, x)[1] = 255;
				drawing.at<cv::Vec3b>(y, x)[2] = 0;
			}
		}
	}*/

	// debug
	//std::cout << "debug\n";
	cv::imwrite( SystemParams::_save_folder  + "SDF\\" + imageName + ".png", drawing);
}

/*cv::Mat ADistanceTransform::CalculateDistanceTransform(const std::vector<AVector> boundary, cv::Mat& dImage)
{
	//cv::Mat dImage = cv::Mat::zeros(SystemParams::_upscaleFactor, SystemParams::_upscaleFactor, CV_32FC1); // should be zeros	

	// ---------- calculate grid ----------
	for (int y = 0; y < dImage.rows; y++)
	{
		for (int x = 0; x < dImage.cols; x++)
		{
			float d = cv::pointPolygonTest(cvContour, cv::Point2f(x, y), true);
			dImage.at<float>(y, x) = -d; // because I want positive (outside) and negative (inside)
		}
	}

}*/

void ADistanceTransform::CalculatePeaks()
{
	if (SystemParams::_peak_dist_stop > 100)
	{
		std::cout << "not calculating peak\n";
		return;
	}

	_peaks.clear();

	std::vector<std::pair<float, AVector>> distPeaks;
	for (unsigned int j = 0; j < _sz; j++)
	{
		for (unsigned int i = 0; i < _sz; i++)
		{
			//float d = _distImage.at<float>(j, i);
			float d = _distArray[i + j * _sz];
			if (d > SystemParams::_peak_dist_stop) { distPeaks.push_back(std::pair<float, AVector>(d, AVector(i, j) / _scale)); }
		}
	}

	if (distPeaks.size() == 0)
	{
		std::cout << "distPeak is 0\n";
		return;
	}


	std::stable_sort(distPeaks.begin(), distPeaks.end(), [](const std::pair<float, AVector> &x,
			                                            const std::pair<float, AVector> &y)
	{ return x.first > y.first; });

	AVector ptFirst = distPeaks[0].second;
	{ _peaks.push_back(ptFirst); }

	CollissionGrid cg(SystemParams::_peak_gap);
	cg.InsertAPoint(ptFirst.x, ptFirst.y, -1, -1);

	//int xPos = -1;
	//int yPos = -1;
	std::vector<AVector> ptss;

	for (unsigned int a = 0; a < distPeaks.size(); a++)
	{
		//if (_peaks.size() > 50) { break; }

		AVector pt = distPeaks[a].second;

		//int curXPos = -1;
		//int curYPos = -1;
		//cg.GetCellPosition(curXPos, curYPos, pt.x, pt.y);
		//if (curXPos != xPos || curYPos != yPos)
		//{
		//	xPos = curXPos;
		//	yPos = curYPos;
		//	ptss.clear();
		//	cg.GetClosestPoints(pt.x, pt.y, ptss);
		//}
		
		ptss.clear();
		cg.GetClosestPoints(pt.x, pt.y, ptss);

		//if (_peaks.size() == 0) { _peaks.push_back(pt); }
		//if (UtilityFunctions::DistanceToBunchOfPoints(_peaks, pt) > SystemParams::_peak_gap) { _peaks.push_back(pt); }
		if (UtilityFunctions::DistanceToBunchOfPoints(ptss, pt) > SystemParams::_peak_gap) 
		{ 
			_peaks.push_back(pt); 
			cg.InsertAPoint(pt.x, pt.y, -1, -1);
		}
	}
	//}

	//std::cout << "===== _peaks size " << _peaks.size() << " =====\n\n";
}

// soon to be deprecated
/*void ADistanceTransform::CalculateDistanceTransform2(CollissionGrid* cGrid, 
													 AVector& peakPos,
													 float& maxDist,
													 float& fill_percentage,
													 std::vector<AVector>& peaks,
													 int numIter)
{
	peakPos = AVector();
	maxDist = std::numeric_limits<float>::min();
	for (int y = 0; y < _distImage.rows; y++)
	{
		for (int x = 0; x < _distImage.cols; x++)
		{
			if (_containerDistImage.at<float>(y, x) < 0) { continue; }

			std::vector<int> graphIndices;

			// ---------- neighbouring graph ----------
			std::vector<AnObject*> qtObjects = cGrid->GetObjects(x, y);
			for (unsigned int a = 0; a < qtObjects.size(); a++)
			{
				if (UtilityFunctions::GetIndexFromIntList(graphIndices, qtObjects[a]->_info1) == -1)
					{ graphIndices.push_back(qtObjects[a]->_info1); }
			}

			// ---------- distance ----------
			float minDist = std::numeric_limits<float>::max();
			for (unsigned int a = 0; a < graphIndices.size(); a++)
			{
				float d = -cv::pointPolygonTest(_graphBoundaries[graphIndices[a]], cv::Point2f(x, y), true);
				if (d <= 0) // inside
					{ minDist = 0; break; }
				
				if (d < minDist) // outside
					{ minDist = d; }
			} 

			if (graphIndices.size() == 0) { minDist = _containerDistImage.at<float>(y, x); }

			_distImage.at<float>(y, x) = minDist;

			// ---------- peak ----------
			if (minDist > maxDist)
			{
				maxDist = minDist;
				peakPos = AVector(x, y);
			}
		}
	}

	// ---------- save to image ----------
	std::stringstream ss;
	ss << "dist_" << numIter;
	DebugDistanceImage(_distImage, ss.str());

	// ---------- fill ratio ----------
	fill_percentage = 0;
	for (int y = 0; y < _distImage.rows; y++)
	{
		for (int x = 0; x < _distImage.cols; x++)
		{
			if (_containerDistImage.at<float>(y, x) < 0) { continue; }

			if (_distImage.at<float>(y, x) <= 0) { fill_percentage += 1.0f; }
		}
	}
	fill_percentage /= (float)_container_size;

	

	////////////////////
	if (!SystemParams::_activate_attraction_force) { return; }

	peaks.clear();

	std::vector<std::pair<float, AVector>> distPeaks;
	for (int j = 0; j < _distImage.rows; j++)
	{
		for (int i = 0; i < _distImage.cols; i++)
		{
			float d = _distImage.at<float>(j, i);
			if (d > SystemParams::_peak_dist_stop * 1.0) { distPeaks.push_back(std::pair<float, AVector>(d, AVector(i, j))); }
		}
	}

	if (!peakPos.IsInvalid()) { distPeaks.push_back(std::pair<float, AVector>(_maxDist, peakPos)); }

	std::cout << "distPeaks size " << distPeaks.size() << "\n";

	if (distPeaks.size() < SystemParams::_num_peak_threshold)
	{
		std::random_shuffle(distPeaks.begin(), distPeaks.end());

		std::sort(distPeaks.begin(), distPeaks.end(), [](const std::pair<float, AVector> &x,
			const std::pair<float, AVector> &y)
		{ return x.first > y.first; });

		for (int a = 0; a < distPeaks.size(); a++)
		{
			AVector pt = distPeaks[a].second;
			if (peaks.size() == 0) { peaks.push_back(pt); }
			else  if (UtilityFunctions::DistanceToBunchOfPoints(peaks, pt) > SystemParams::_peak_gap) { peaks.push_back(pt); }
		}
	}

	
}*/