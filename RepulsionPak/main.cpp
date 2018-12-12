
/* ---------- ShapeRadiusMatching V2  ---------- */


#include "stdafx.h"

#include <iostream>

#include <time.h> // time seed
#include <stdlib.h>     /* srand, rand */
#include <time.h> 

//#include "ShapeRadiusMatching.h"

#include "Display.h"
#include "ColorPalette.h"




//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char **argv)
{
	// only for maing the packing bad
	// call this once
	

	SystemParams::LoadParameters();

	if (SystemParams::_seed <= 0)
	{
		//srand(time(NULL));

		SystemParams::_seed = time(NULL) % 1000000;
		SystemParams::_seed %= 1000000;
		SystemParams::_seed %= 1000000;
		SystemParams::_seed %= 1000000;
		SystemParams::_seed %= 10000;
	}

	srand(SystemParams::_seed);

	//if (SystemParams::_seed <= 0)
	//	{ srand(time(NULL)); }
	//else
	//	{ srand(SystemParams::_seed); }

	//ColorPalette::LoadPalette();

	Display::GetInstance()->DoStuff();

	// --- parameters ---
	std::cout << "Have " << argc << " arguments:" << std::endl;
	for (int i = 0; i < argc; ++i) {
		std::cout << argv[i] << std::endl;
	}
	// --- ---

	Display::ShowGL(argc, argv);

	std::cout << "program done\n";

	if (StuffWorker::_cGrid) 
	{
		delete StuffWorker::_cGrid;
	}
	
	return 0;
}

