
#include "ADistanceTransform.h"
#include "UtilityFunctions.h"
#include "PathIO.h"
#include "CollissionGrid.h"

ADistanceTransform::ADistanceTransform(
	const std::vector<std::vector<AVector>>& containers,
	const std::vector<std::vector<AVector>>& holes,
	const std::vector<std::vector<AVector>>& focals,
	float scale)
{
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
	{
		cvHoles.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(focals[a]));
	}
	for (int a = 0; a < holes.size(); a++)
	{
		cvHoles.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(holes[a]));
	}
	_container_size = 0;
	for (int xIter = 0; xIter < _sz; xIter++)
	{
		for (int yIter = 0; yIter < _sz; yIter++)
		{
			float xActual = ((float)xIter) / _scale;
			float yActual = ((float)yIter) / _scale;

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
				if (d2 > 0) { isInsideHole = true; break; }
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
			if (_containerDistArray[x + y * _sz] >= 0)
			{
				_fill_img_template.SetIntegerPixel(x, y, 1);
			} // inside boundary
			else { _fill_img_template.SetIntegerPixel(x, y, 0); }

		}
	}
}

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


void ADistanceTransform::AddGraph(const AnElement& aGraph)
{
	_graphBoundaries.push_back(_cvWrapper.ConvertList<AVector, cv::Point2f>(aGraph._uniArt));
	_voronoiColors.push_back(MyColor(rand() % 254 + 1, rand() % 254 + 1, rand() % 254 + 1));// voronoi

	CVImg img; // voronoi
	img.CreateGrayscaleImage(_sz);  // voronoi
	img.SetGrayscaleImageToBlack(); // voronoi
	_voronoiBWImages.push_back(img);  // voronoi
}

void ADistanceTransform::UpdateBoundaries(const std::vector<AnElement>& graphs)
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
	{
		_cvWrapper.DrawFilledPolyInt(fillImg, _graphBoundaries[a], 0, _scale);
	} // scaled

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
		ss << SystemParams::_output_folder << "FILL\\" << "fill_" << numIter << ".png";
		fillImg.SaveIntegerImage(ss.str());
	}

}

// USE THIS for your simulation
void ADistanceTransform::CalculateSDF2(const std::vector<AnElement>& graphs, CollissionGrid* cGrid, int numIter, bool saveImage)
{
	int szsz = _sz * _sz;
	std::vector<int> overlapMask(szsz); // (Overlap Mask) initialization

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
			{
				overlapMask[xIter + yIter * _sz] = -1; // (Overlap Mask) outside container
				continue;
			}

			overlapMask[xIter + yIter * _sz] = 0; // (Overlap Mask) empty space


			////// exclude container
			//float minDist = std::numeric_limits<float>::max();
			////// include container
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
					overlapMask[xIter + yIter * _sz]++; // (Overlap Mask)
					minDist = 0; // SDF
					isInside = true;
				}

				if (d < minDist) // outside
				{
					minDist = d;
				} // SDF

				if (isInside) { break; } // no point to continue
			}

			if (isInside && containerDistVal < 0)
			{
				overlapMask[xIter + yIter * _sz] = 2; // (Overlap Mask) inside an element and outside container
			}

			if (!isInside && containerDistVal < 0)
			{
				overlapMask[xIter + yIter * _sz] = -1;  // (Overlap Mask) outside element outside container
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

	// THINNING
	/*CVImg thinningImage;
	std::stringstream ss3;
	ss3 << "thin_" << numIter;
	thinningImage = SkeletonDistance(overlapMask, ss3.str());*/

	std::stringstream ss6;
	ss6 << "dist_" << numIter;
	DebugDistanceImage(ss6.str());

	if (SystemParams::_output_files)
	{
		PathIO pathIO;
		pathIO.SaveSDF2CSV(_distArray, SystemParams::_output_folder + "dist_all.csv");
		pathIO.SaveSDF2CSV(_containerDistArray, SystemParams::_output_folder + "dist_mask.csv");
	}

	// create new elements !!!!
	CalculatePeaks();

	// ---------- global max distance _maxDist ----------
	_maxDist = std::numeric_limits<float>::min();
	for (unsigned int a = 0; a < _distArray.size(); a++)
	{
		if (_maxDist < _distArray[a]) { _maxDist = _distArray[a]; }
	}

	if (SystemParams::_output_files)
	{
		std::vector<float> dummyARray;
		dummyARray.push_back(0);
		std::stringstream ss666;
		ss666 << "dist_max_is_" << _maxDist << ".csv";
		PathIO pathIO;
		pathIO.SaveSDF2CSV(dummyARray, SystemParams::_output_folder + ss666.str());
	}
}

// THIS is called by CreateManualPacking()
void ADistanceTransform::CalculateSDF1(CollissionGrid* cGrid, int numIter, bool saveImage)
{
	int szsz = _sz * _sz;
	std::vector<int> overlapMask(szsz);

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
			{
				overlapMask[xIter + yIter * _sz] = -1;  continue;
			} // (Overlap Mask) outside container

			overlapMask[xIter + yIter * _sz] = 0; // (Overlap Mask) 

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
					overlapMask[xIter + yIter * _sz]++; // (Overlap Mask) 
					isInside = true;
				}

				if (d < minDist) // outside
				{
					minDist = d;
				} // SDF

				if (isInside) { break; } // no point to continue
			}


			if (isInside && containerDistVal < 0)
			{
				overlapMask[xIter + yIter * _sz] = 2;
			} // (Overlap Mask) inside an element and outside container

			/*else*/ if (!isInside && containerDistVal < 0)
			{
				overlapMask[xIter + yIter * _sz] = -1;  // (Overlap Mask) outside element outside container
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
		pathIO.SaveSDF2CSV(_distArray, SystemParams::_output_folder + "dist_all.csv");
		pathIO.SaveSDF2CSV(_containerDistArray, SystemParams::_output_folder + "dist_mask.csv");
	}

	/*CVImg thinningImage;
	std::stringstream ss3;
	ss3 << "thin_" << numIter;
	thinningImage = SkeletonDistance(overlapMask, ss3.str());*/

	// create new elements !!!!
	CalculatePeaks();

	// ---------- global max distance _maxDist ----------
	_maxDist = std::numeric_limits<float>::min();
	for (unsigned int a = 0; a < _distArray.size(); a++)
	{
		if (_maxDist < _distArray[a]) { _maxDist = _distArray[a]; }
	}

	if (SystemParams::_output_files)
	{
		std::vector<float> dummyARray;
		dummyARray.push_back(0);
		std::stringstream ss666;
		ss666 << "dist_max_is_" << _maxDist << ".csv";
		PathIO pathIO;
		pathIO.SaveSDF2CSV(dummyARray, SystemParams::_output_folder + ss666.str());
	}

	std::cout << "_maxDist = " << _maxDist << "\n";
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
	thinningImage.SaveImage(SystemParams::_output_folder + "SDF\\" + imageName + ".png");
	return thinningImage;
}

CVImg ADistanceTransform::VoronoiSkeleton(CollissionGrid* cGrid, int numIter)
{
	// reset
	for (unsigned int a = 0; a < _graphBoundaries.size(); a++)
	{
		_voronoiBWImages[a].SetGrayscaleImageToBlack();
	}

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
		{
			cv::line(voronoiEdgeImage._img, cvContours[b], cvContours[b + 1], 255, lineWidth);
		}
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
			{
				thinningImage.SetGrayValue(xIter, yIter, 0);
			}
		}
	}

	std::stringstream ss1;
	ss1 << "voronoi_skeleton_" << numIter;
	thinningImage.SaveImage(SystemParams::_output_folder + "SDF\\" + ss1.str() + ".png");

	std::stringstream ss2;
	ss2 << "voronoi_color" << numIter;
	voronoiImage.SaveImage(SystemParams::_output_folder + "SDF\\" + ss2.str() + ".png");

	//CVImg thinningImg;
	return thinningImage;
}

CVImg ADistanceTransform::SkeletonDistance(std::vector<int> overlapMask, std::string imageName)
{
	CVImg emptyImage;

	int scale_factor = 2;
	int thin_sz = _sz * scale_factor;

	emptyImage.CreateGrayscaleImage(thin_sz, thin_sz);
	for (int x = 0; x < thin_sz; x++)
	{
		for (int y = 0; y < thin_sz; y++)
		{
			emptyImage.SetGrayValue(x, y, 0);
			int val = overlapMask[(x / scale_factor) + (y / scale_factor) * _sz];
			if (val == 0) // empty space
			{
				emptyImage.SetGrayValue(x, y, 255);
			}
		}
	}
	///////////
	// BLURRING
	///////////
	/*if (SystemParams::_skin_thickness > 0)
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
	}*/
	///////////
	// THINNING
	///////////
	CVImg thinningImage = emptyImage.ThinningFromGrayscale();
	//thinningImage.SaveImage( SystemParams::_save_folder + "SDF\\" + imageName + ".png" );
	emptyImage.SaveImage(SystemParams::_output_folder + "SDF\\" + imageName + "_mask.png");

	/*cv::Mat cloneImg = thinningImage._img.clone();
	cloneImg.convertTo(cloneImg, CV_32FC1);
	cv:imwrite(SystemParams::_save_folder + "SDF\\" + "CV_32FC1.png", cloneImg);
	CmCurveEx cm(cloneImg);
	cm.Demo(cloneImg, true);*/

	//PixelTracer pT;
	//pT.InitImage(thinningImage);

	CVImg testImage;
	testImage.CreateGrayscaleImage(thin_sz, thin_sz);
	for (int y = 0; y < thin_sz; y++)
	{
		for (int x = 0; x < thin_sz; x++)
		{
			int val = thinningImage.GetGrayValue(x, y);
			if (val == 0)
			{
				testImage.SetGrayValue(x, y, 255); // CellType::NO_GO
			}
			else
			{
				testImage.SetGrayValue(x, y, 0); // CellType::UNVISITED
			}
		}
	}

cv:imwrite(SystemParams::_output_folder + "SDF\\" + imageName + ".png", testImage._img);

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
			}
		}
	}

	cv::imwrite(SystemParams::_output_folder + "SDF\\" + imageName + ".png", drawing);
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
	double minVal = 15;
	double maxVal = 15;

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
			/*if (d < 0)
			{
				if (d < -20) d = -20;
				drawing.at<cv::Vec3b>(j, i)[0] = 255 - (int)abs(d) * 255 / minVal;
			}
			else */if (d > 0)
			{
				// negative space
				if (d > 20) d = 20;
				drawing.at<cv::Vec3b>(j, i)[2] = 255 - (int)d * 255 / maxVal;
				drawing.at<cv::Vec3b>(j, i)[1] = 255 - (int)d * 255 / maxVal;
				drawing.at<cv::Vec3b>(j, i)[0] = 255 - (int)d * 255 / maxVal;
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
	cv::imwrite(SystemParams::_output_folder + "SDF\\" + imageName + ".png", drawing);
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
	cv::imwrite(SystemParams::_output_folder + "SDF\\" + imageName + ".png", drawing);
}

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


	std::stable_sort(distPeaks.begin(), distPeaks.end(), [](const std::pair<float, AVector>& x,
		const std::pair<float, AVector>& y)
		{
			return x.first > y.first;
		});

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