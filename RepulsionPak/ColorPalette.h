
#ifndef __COLOR_PALLETTE_H__
#define __COLOR_PALLETTE_H__

#include "OpenCVWrapper.h"

class ColorPalette
{
public:
	ColorPalette();
	~ColorPalette();

public:
	static std::vector<MyColor> _palette_01;

	static MyColor _back_color;

	static void LoadPalette();

	static MyColor AddColorString(std::string str);
};

//std::vector<MyColor> ColorPalette::_palette_01 = std::vector<MyColor>();

#endif