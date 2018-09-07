
#ifndef MYCOLOR_H
#define MYCOLOR_H


/*================================================================================
================================================================================*/
struct MyColor
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
		this->_r = -1;
		this->_g = -1;
		this->_b = -1;
	}

	bool IsNotBlack()
	{
		return (this->_r > 0 && this->_g && this->_b);
	}

	bool IsValid()
	{
		return (this->_r > -1 && this->_g > -1 && this->_b > -1);
	}

	void Print()
	{
		//std::cout << "(r= " << _r << ", g=" << _g << ", b=" << _b << ")\n";
	}

public:
	//static MyColor _black;
	//static MyColor _white;

	//static std::vector<MyColor> _palette_01;
};

#endif
