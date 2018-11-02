
#ifndef __Pixel_Tracer__
#define __Pixel_Tracer__


#include "AVector.h"
#include "OpenCVWrapper.h"

#include <vector>

enum CellType
{
	NO_GO     = 0,
	UNVISITED = 1,
	VISITED   = 2,
};

/*struct AnIntVector
{
	int _x;
	int _y;

	AnIntVector()
	{
		_x = -1;
		_y = -1;
	}

	AnIntVector(int x, int y)
	{
		_x = x;
		_y = y;
	}
};*/

class PixelTracer
{
public:
	PixelTracer();
	~PixelTracer();

	void InitImage(CVImg input_img);

public:
	std::vector<std::pair<int, int>> _dirs;

	int _sz; // size of square

	std::vector<CellType> _cell_flags;

	int NumNeighbors(int x, int y);

	//std::vector<std::vector<AVector>> _image; // thinning image
};

#endif
