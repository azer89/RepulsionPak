
#include "ColorPalette.h"

#include "UtilityFunctions.h"

void ColorPalette::LoadPalette()
{
	_palette_01.push_back(MyColor(190, 221, 244)); // 0 light blue
	_palette_01.push_back(MyColor(58, 162, 219));  // 1 blue
	_palette_01.push_back(MyColor(57, 139, 203));  // 2 dark blue
	//_palette_01.push_back(MyColor(251, 192, 45));  // 3 yellow
	_palette_01.push_back(MyColor(255, 0, 0));  // 3 yellow
	_palette_01.push_back(MyColor(244, 124, 32));  // 4 orange
	_palette_01.push_back(MyColor(242, 128, 170)); // 5 pink
	_palette_01.push_back(MyColor(157, 156, 158)); // 6 gray
	_palette_01.push_back(MyColor(0, 0, 0));       // 7 black
	_palette_01.push_back(MyColor(255, 255, 255)); // 8 white
}

MyColor ColorPalette::AddColorString(std::string str)
{
	std::vector<std::string> colArray = UtilityFunctions::Split(str, ',');
	MyColor col(std::stoi(colArray[0]), std::stoi(colArray[1]), std::stoi(colArray[2]));
	_palette_01.push_back(col);

	return col;
}

std::vector<MyColor> ColorPalette::_palette_01 = std::vector<MyColor>();

MyColor ColorPalette::_back_color = MyColor();