#ifndef __An_Object__
#define __An_Object__

class AnObject
{
public:
	float _x;
	float _y;
	int   _info1;  // which grap
	int   _info2;  // which mass

	//float _nx; // normal
	//float _ny; // normal

	/*AnObject(float x, float y, int info1, int info2, float nx, float ny) :
		_x(x),
		_y(y),
		_info1(info1),
		_info2(info2),
		_nx(nx),
		_ny(ny)
	{
	}*/

	AnObject(float x, float y, int info1, int info2) :
		_x(x),
		_y(y),
		_info1(info1),
		_info2(info2)//,
		//_nx(-1000),
		//_ny(-1000)
	{
	}

	AnObject(float x, float y, int info1) :
		_x(x),
		_y(y),
		_info1(info1),
		_info2(-1)//,
		//_nx(-1000),
		//_ny(-1000)
	{
	}

	AnObject(float x, float y) :
		_x(x),
		_y(y),
		_info1(-1),
		_info2(-1)//,
		//_nx(-1000),
		//_ny(-1000)
	{
	}

	//bool HasNormalVector()
	//{
	//	return (this->_nx > -500 && this->_ny > -500);
	//}
};

#endif
