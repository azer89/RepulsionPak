
/* ---------- ShapeRadiusMatching V2  ---------- */

/*================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================*/

#ifndef OPENCV_WRAPPER
#define OPENCV_WRAPPER

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <unordered_map>

#include "ImageThinning.h"
#include "SystemParams.h"
#include "AVector.h"

// duuuh
#include "ArtData.h"
#include "VFRegion.h"
#include "MyColor.h"

#include "AnIndexedLine.h"
//#include "ABlob.h"

#define BGR_255 CV_8UC3

/*================================================================================
================================================================================*/
/*struct MyColor
{
public:
	int _r;
	int _g;
	int _b;

	MyColor(int r, int g, int b)
	{
		this->_r = r;
		this->_g = g;
		this->_b = b;
	}

	MyColor(int val)
	{
		this->_r = val;
		this->_g = val;
		this->_b = val;
	}

	MyColor()
	{
		this->_r = 0;
		this->_g = 0;
		this->_b = 0;
	}

	bool IsNotBlack()
	{
		return (this->_r > 0 && this->_g && this->_b);
	}

	void Print()
	{
		std::cout << "(r= " << _r << ", g=" << _g << ", b=" << _b << ")\n";
	}

public:
	//static MyColor _black;
	//static MyColor _white;

	//static std::vector<MyColor> _palette_01;
};*/

/*================================================================================
wrapper for cv::Mat
================================================================================*/
struct CVImg
{
public:

	cv::Mat		_img;
	bool		_isColor;
	std::string _name;

public:

	// ===== constructors =====
	CVImg(std::string name) { this->_name = name; }
	CVImg() {}

	// get image
	cv::Mat GetCVImage() { return _img; }

	// ===== show =====
	void Show()
	{
		cv::namedWindow(_name, cv::WINDOW_AUTOSIZE);
		cv::imshow(_name, _img);
	}

	// ===== color =====
	/*void LoadColor(std::string filename)
	{
		_img = cv::imread(filename, CV_LOAD_IMAGE_COLOR);
		_isColor = true;
	}*/

	// ===== grayscale ===== 
	/*void LoadGrayscale(std::string filename)
	{
		_img = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
		_isColor = false;
	}*/

	// ===== create =====
	void CreateGrayscaleImage(int height, int width) { _img = cv::Mat::zeros(height, width, CV_8UC1);  _isColor = true; }
	void CreateGrayscaleImage(int squareSize) { _img = cv::Mat::zeros(squareSize, squareSize, CV_8UC1);  _isColor = true; }
	void CreateColorImage(int height, int width) { _img = cv::Mat::zeros(height, width, CV_8UC3);  _isColor = true; }
	void CreateColorImage(int squareSize) { _img = cv::Mat::zeros(squareSize, squareSize, CV_8UC3);  _isColor = true; }
	void CreateFloatImage(int height, int width) { _img = cv::Mat::zeros(height, width, CV_32FC1); _isColor = false; }
	void CreateFloatImage(int squareSize) { _img = cv::Mat::zeros(squareSize, squareSize, CV_32FC1); _isColor = false; }
	void CreateIntegerImage(int height, int width) { _img = cv::Mat::zeros(height, width, CV_32SC1); _isColor = false; }
	void CreateIntegerImage(int squareSize) { _img = cv::Mat::zeros(squareSize, squareSize, CV_32SC1); _isColor = false; }

	// ===== set =====
	void SetFloatPixel(int x, int y, float val) { _img.at<float>(y, x) = val; }

	void SetIntegerPixel(int x, int y, int val) { _img.at<int>(y, x) = val; }

	void SetColorImageToWhite()
	{
		/*for (unsigned int x = 0; x < _img.cols; x++)
		{
			for (unsigned int y = 0; y < _img.rows; y++)
			{
				SetColorPixel(x, y, MyColor(255));
			}
		}*/
		_img.setTo(cv::Scalar(255, 255, 255));
	}

	void SetGrayscaleImageToSomething(int val)
	{
		for (unsigned int x = 0; x < _img.cols; x++)
		{
			for (unsigned int y = 0; y < _img.rows; y++)
			{
				SetGrayValue(x, y, val);
			}
		}
	}

	void SetGrayscaleImageToBlack()
	{
		for (unsigned int x = 0; x < _img.cols; x++)
		{
			for (unsigned int y = 0; y < _img.rows; y++)
			{
				SetGrayValue(x, y, 0);
			}
		}
	}

	void SetColorPixel(int x, int y, MyColor color)
	{
		_img.at<cv::Vec3b>(y, x)[0] = color._b; // blue
		_img.at<cv::Vec3b>(y, x)[1] = color._g; // green
		_img.at<cv::Vec3b>(y, x)[2] = color._r; // red
	}

	void SetGrayValue(int x, int y, int val) { _img.at<uchar>(y, x) = val; }

	// ===== get =====
	float GetFloatValue(int x, int y) { return _img.at<float>(y, x); }

	int   GetIntegerValue(int x, int y) { return _img.at<int>(y, x); }

	int   GetGrayValue(int x, int y) { return (int)_img.at<uchar>(y, x); }

	int   GetColorValue(int x, int y, int channel) { return _img.at<cv::Vec3b>(y, x)[channel]; }

	// row
	int GetRows() { return _img.rows; }

	// column
	int GetCols() { return _img.cols; }

	void SaveImage(std::string filename) { cv::imwrite(filename, _img); }

	void SaveIntegerImage(std::string filename)
	{
		// drawing
		cv::Mat drawing = cv::Mat::zeros(_img.size(), CV_8UC3);

		for (int j = 0; j < drawing.rows; j++) // y
		{
			for (int i = 0; i < drawing.cols; i++) // x
			{
				if (_img.at<int>(j, i) > 0) // white
				{
					drawing.at<cv::Vec3b>(j, i)[0] = 255;
					drawing.at<cv::Vec3b>(j, i)[1] = 255;
					drawing.at<cv::Vec3b>(j, i)[2] = 255;
				}
				else // black
				{
					drawing.at<cv::Vec3b>(j, i)[0] = 0;
					drawing.at<cv::Vec3b>(j, i)[1] = 0;
					drawing.at<cv::Vec3b>(j, i)[2] = 0;
				}
			}
		}
		// save image
		imwrite(filename, drawing);
	}

	void SaveDistanceImage(std::string filename)
	{
		// min max
		double minVal; double maxVal;
		cv::minMaxLoc(_img, &minVal, &maxVal, 0, 0, cv::Mat());
		minVal = abs(minVal); maxVal = abs(maxVal);

		// drawing
		cv::Mat drawing = cv::Mat::zeros(_img.size(), CV_8UC3);

		for (int j = 0; j < drawing.rows; j++) // y
		{
			for (int i = 0; i < drawing.cols; i++) // x
			{
				if (_img.at<float>(j, i) < 0)
				{
					drawing.at<cv::Vec3b>(j, i)[0] = 255 - (int)abs(_img.at<float>(j, i)) * 255 / minVal;
				}
				else if (_img.at<float>(j, i) > 0)
				{
					drawing.at<cv::Vec3b>(j, i)[2] = 255 - (int)_img.at<float>(j, i) * 255 / maxVal;
				}
				else
				{
					drawing.at<cv::Vec3b>(j, i)[0] = 255;
					drawing.at<cv::Vec3b>(j, i)[1] = 255;
					drawing.at<cv::Vec3b>(j, i)[2] = 255;
				}
			}
		}
		// save image
		imwrite(filename, drawing);
	}

	CVImg ThinningFromInteger()
	{
		CVImg thinningImg;
		thinningImg.CreateGrayscaleImage(this->GetCols(), this->GetRows());

		//cv::Mat cloneImg = this->_img.clone();
		//cloneImg.convertTo(cloneImg, CV_8UC1); // you have to convert to CV_8UC1
		CVImg cloneImg;
		cloneImg.CreateGrayscaleImage(SystemParams::_upscaleFactor);
		for (int a = 0; a < SystemParams::_upscaleFactor; a++)
		{
			for (int b = 0; b < SystemParams::_upscaleFactor; b++)
			{
				if (this->GetIntegerValue(a, b) > 0)
				{
					cloneImg.SetGrayValue(a, b, 255);
				}
			}
		}

		ImageThinning::ZhanSuenThinning(cloneImg._img, thinningImg._img);
		return thinningImg;
	}

	CVImg ThinningFromGrayscale()
	{
		CVImg thinningImg;
		thinningImg.CreateGrayscaleImage(this->GetCols(), this->GetRows());
		cv::Mat cloneImg = this->_img.clone();
		cloneImg.convertTo(cloneImg, CV_8UC1); // you have to convert to CV_8UC1
		ImageThinning::ZhanSuenThinning(cloneImg, thinningImg._img);
		return thinningImg;
	}
};

/*================================================================================
OpenCV Rendering
================================================================================*/
class OpenCVWrapper
{
public:
	// ----------  random ----------
	cv::RNG _rng;

public:
	// ----------  hash table ----------
	std::unordered_map <std::string, cv::Mat> _images;

	// ---------- constructor ----------
	OpenCVWrapper();

	// ---------- destructor ----------
	~OpenCVWrapper();

	// ---------- random ----------
	MyColor GetRandomColor();

	// ---------- create a cv::Mat instance ----------
	void CreateImage(std::string imageName, int width, int height, int imgType);

	// ---------- show ----------
	void ShowImage(std::string imageName);

	// ---------- save to an image file ----------
	void SaveImage(std::string imageName, std::string filePath);

	// ---------- set color of a pixel ----------
	void SetPixel(std::string imageName, int x, int y, MyColor col);

	// ---------- weight of an image ----------
	int  GetNumColumns(std::string imageName);

	// ---------- height of an image ----------
	int  GetNumRows(std::string imageName);

	// ---------- OpenCV waitKey ----------
	void WaitKey();

	// ---------- set all pixels white ----------
	void SetWhite(std::string imageName);

	// ---------- the all pixels black ----------
	void SetBlack(std::string imageName);

	// ---------- color to black/white ----------
	CVImg ConvertToBW(std::string imageName);

	// ---------- color of a pixel  ----------
	MyColor GetColor(std::string imageName, int x, int y);


	// get centroid
	// https_//stackoverflow.com/questions/9074202/opencv-2-centroid
	AVector GetPolygonCentroid(const std::vector<AVector>& contour);

	// ---------- Get contour ----------
	std::vector<AVector> GetContour(CVImg img);

	// ---------- Get contours ----------
	std::vector<std::vector<AVector>> GetContours(CVImg img, int blurSize = 0);

	// ----------  point polygon test ----------
	template <typename T>
	float PointPolygonTest(std::vector<T> shape_contours, T pt);

	// ---------- signed distance ----------
	CVImg CalculateSignedDistance(const std::vector<AVector>& contour);

	// ---------- signed distance ----------
	CVImg CalculateSignedDistance(const std::vector<AVector>& contour, CVImg aMask);

	// ---------- signed distance ----------
	float GetSignedDistance(const std::vector<cv::Point>& contour, int x, int y);

	// ---------- signed distance ----------
	float GetSignedDistance(const std::vector<std::vector<cv::Point>>& contours, int x, int y);

	// center
	AVector GetCenter(const std::vector<AVector>& polygon);

	// ---------- conves hull ----------
	std::vector<AVector> GetConvexHull(std::vector<AVector> polygon);

	float GetArea(const std::vector<AVector>& polygon)
	{
		std::vector<cv::Point2f> cvPoly = ConvertList<AVector, cv::Point2f>(polygon);
		return cv::contourArea(cvPoly);

	}

	float GetAreaOriented(const std::vector<AVector>& polygon)
	{
		std::vector<cv::Point2f> cvPoly = ConvertList<AVector, cv::Point2f>(polygon);
		return cv::contourArea(cvPoly, true);

	}

	// ---------- get longest contour ----------
	template <typename T>
	int GetLongestContourIndex(std::vector<std::vector<T>> contours);

	// ---------- text ----------
	void PutText(std::string imageName, std::string text, AVector pos, MyColor col, float scale = 0.5f, float thickness = 1);
	void PutText(cv::Mat img, std::string text, AVector pos, MyColor col, float scale = 0.5f, float thickness = 1);

	// ---------- conversion ----------
	template <typename T, typename U>
	std::vector<U> ConvertList(const std::vector<T>& contour1);

	// ========== drawing ==========

	// ---------- draw ----------
	template <typename T>
	void DrawLine(std::string imageName, T pt1, T pt2, MyColor color, int thickness, float scale = 1.0f);

	// ---------- draw ----------
	void DrawCircle(std::string imageName, AVector pt, MyColor col, float radius)
	{
		cv::Mat drawing = _images[imageName];
		cv::circle(drawing, cv::Point(pt.x, pt.y), radius, cv::Scalar(col._b, col._g, col._r), -1);
	}

	void DrawCircle(cv::Mat img, AVector pt, MyColor col, float radius)
	{
		cv::circle(img, cv::Point(pt.x, pt.y), radius, cv::Scalar(col._b, col._g, col._r), -1);
	}

	// ---------- draw ----------
	template <typename T>
	void DrawPolyOnCVImage(cv::Mat img,
		const std::vector<T>& shape_contours,
		MyColor color,
		bool isClosed,
		float thickness = 1.0f,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawPolysOnCVImage(cv::Mat img,
		const std::vector<std::vector<T>>& shape_contours,
		MyColor color,
		bool isClosed,
		float thickness = 1.0f,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawPoly(std::string imageName,
		const std::vector<T>& shape_contours,
		MyColor color,
		bool isClosed,
		float thickness = 1.0f,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawPolys(std::string imageName,
		const std::vector<std::vector<T>>& shape_contours,
		MyColor color,
		bool isClosed,
		float thickness = 1.0f,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawFilledPoly(std::string imageName,
		const std::vector<T>& shape_contours,
		MyColor color,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawFilledPoly(CVImg img,
		const std::vector<T>& shape_contours,
		MyColor color,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawFilledPolyInt(CVImg& img,
		const std::vector<T>& shape_contours,
		int val,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawPolyInt(CVImg& img,
		const std::vector<T>& shape_contours,
		int val,
		bool isClosed,
		float thickness = 1.0f,
		float scale = 1.0f,
		float xOffset = 0,
		float yOffset = 0);

	// ---------- draw ----------
	template <typename T>
	void DrawLineInt(CVImg& img, T pt1, T pt2, int val, int thickness, float scale = 1.0f);

	// ---------- from ShapeRadiusMatching ----------
	CVImg GetDistanceMapFast(int numRegion,
		const std::vector<ArtData>& artDataArray,
		CVImg artDataMask);

	// ---------- from ShapeRadiusMatching ----------
	CVImg GetDistanceMapSlow(int numRegion,
		VFRegion* aRegion,
		const std::vector<ArtData>& artDataArray,
		const std::vector<AVector>& boundary,
		const std::vector<std::vector<AVector>>& focalBoundaries);

	// ---------- from ShapeRadiusMatching ----------
	std::vector<std::vector<cv::Point>> GetCVContours(const std::vector<std::vector<AVector>>& contours);

	// ---------- from ShapeRadiusMatching ----------
	//void SavePairFunctionsAnBlobContourToImage(LRFunctions pFunc, ABlob aBlob, std::string blobName);

	// ---------- from ShapeRadiusMatching ----------
	//CVImg CreateArtDataMask(int regionNum, VFRegion* aRegion, std::vector<ArtData> ornaments, std::vector<ABlob> blobs);

	// ---------- for Dijkstra ----------
	CVImg CreateDijkstraMask(int blobNumber,
		std::vector<AVector> combinedBoundary,
		std::vector<ArtData> artDataArray);

	// ----------  ----------
	void Triangulate(std::vector<AnIdxTriangle>& myTriangles,
		std::vector<AnIndexedLine>& negSpaceEdges,
		const std::vector<AVector>& randomPoints,
		const std::vector<AVector>& boundary,
		float img_length,
		const std::vector<std::vector<AVector>>& arts);

	void CaptureOpenGL(int w, int h, std::string filename);

};

#endif