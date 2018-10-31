
#include "PixelTracer.h"



PixelTracer::PixelTracer()
{
}

PixelTracer::~PixelTracer()
{
}

void PixelTracer::InitImage(CVImg input_img)
{
	this->_sz = input_img.GetRows(); // assume that the image is square
	//int szsz = _sz * _sz;

	//for (unsigned int a = 0; a < szsz; a++)
	//{	
		//_cell_flags.push_back()
	//}

	for (int x = 0; x < _sz; x++)
	{
		for (int y = 0; y < _sz; y++)
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

	// debug
	CVImg testImage;
	testImage.CreateGrayscaleImage(_sz, _sz);
	for (unsigned int a = 0; a < _cell_flags.size(); a++)
	{
		int x = a % _sz;
		int y = (a - x) / _sz;

		if (_cell_flags[a] == CellType::NO_GO)
		{
			testImage.SetGrayValue(x, y, 0);
		}
		else
		{
			testImage.SetGrayValue(x, y, 1);
		}
	}
	cv:imwrite(SystemParams::_save_folder + "SDF\\" + "debug.png", testImage._img);
}
