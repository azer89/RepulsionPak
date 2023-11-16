
#include "PixelTracer.h"



PixelTracer::PixelTracer()
{
	_dirs.push_back(std::pair<int, int>(-1, -1)); // top left
	_dirs.push_back(std::pair<int, int>(0, -1)); // top
	_dirs.push_back(std::pair<int, int>(1, -1)); // top right
	_dirs.push_back(std::pair<int, int>(-1, 0)); // left

	_dirs.push_back(std::pair<int, int>(1, 0)); // right
	_dirs.push_back(std::pair<int, int>(-1, 1)); // bottom left
	_dirs.push_back(std::pair<int, int>(0, 1)); // bottom
	_dirs.push_back(std::pair<int, int>(1, 1)); // bottom right
}

PixelTracer::~PixelTracer()
{
}


int PixelTracer::NumNeighbors(int x, int y)
{
	int numNeighbor = 0;

	// we don't start at 0
	// but we start at an empty cell
	int startIdx = 0;
	for (unsigned int a = 0; a < _dirs.size(); a++)
	{
		float idx = (x + _dirs[a].first) + (y + _dirs[a].second) * _sz;
		if (idx < 0) { continue; }
		if (_cell_flags[idx] == CellType::NO_GO)
		{
			startIdx = a;
		}
	}

	// cell flags
	for (unsigned int a = 0; a < _dirs.size(); /**/)
	{
		int curIdx = a + startIdx;      // we don't start at 0
		curIdx = curIdx % _dirs.size(); // make sure we check all directions

		float idx = (x + _dirs[curIdx].first) + (y + _dirs[curIdx].second) * _sz;
		if (idx < 0) { continue; }
		if (_cell_flags[idx] != CellType::NO_GO)
		{
			numNeighbor++;
			a++; // skip
		}
		a++;
	}

	return numNeighbor;
}

void PixelTracer::InitImage(CVImg input_img)
{
	this->_sz = input_img.GetRows(); // assume that the image is square
	//int szsz = _sz * _sz;

	//for (unsigned int a = 0; a < szsz; a++)
	//{	
		//_cell_flags.push_back()
	//}

	for (int y = 0; y < _sz; y++)
	{
		for (int x = 0; x < _sz; x++)
		{
			int val = input_img.GetGrayValue(x, y);
			if (val == 0)
			{
				_cell_flags.push_back(CellType::NO_GO);
			}
			else
			{
				_cell_flags.push_back(CellType::UNVISITED);
			}
		}
	}

	OpenCVWrapper cvWrapper;

	// debug
	CVImg testImage;
	testImage.CreateColorImage(_sz, _sz);
	for (unsigned int a = 0; a < _cell_flags.size(); a++)
	{
		int x = a % _sz;
		int y = (a - x) / _sz;

		int nn = 0;

		if (_cell_flags[a] == CellType::NO_GO)
		{
			testImage.SetColorPixel(x, y, MyColor(0, 0, 0));
		}
		else
		{
			testImage.SetColorPixel(x, y, MyColor(255, 255, 255));

			nn = NumNeighbors(x, y);
		}

		if (nn > 3)
		{
			cvWrapper.DrawCircle(testImage._img, AVector(x, y), MyColor(255, 0, 0), 5);
			//testImage.SetColorPixel(x, y, MyColor(0, 255, 0));
		}
	}
cv:imwrite(SystemParams::_output_folder + "SDF\\" + "debug.png", testImage._img);
}
