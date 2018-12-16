
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

	//Display::GetInstance()->DeleteFiles();
	
	SystemParams::LoadParameters();
	
	// --- parameters ---
	SystemParams::_simulate_1 = true;
	std::cout << "Have " << argc << " arguments:" << std::endl;
	//for (int i = 0; i < argc; ++i) 
	//{
	//	std::cout << "param: " << argv[i] << std::endl;
	//}
	if (argc == 11)
	{
		float param1  = std::stof(argv[1]);
		float param2  = std::stof(argv[2]);
		float param3  = std::stof(argv[3]);
		float param4  = std::stof(argv[4]);
		float param5  = std::stof(argv[5]);
		float param6  = std::stof(argv[6]);
		float param7  = std::stof(argv[7]);
		float param8  = std::stof(argv[8]);
		float param9  = std::stof(argv[9]);
		float param10 = std::stof(argv[10]);

		std::cout << "param1: "  << param1  << std::endl;
		std::cout << "param2: "  << param2  << std::endl;
		std::cout << "param3: "  << param3  << std::endl;
		std::cout << "param4: "  << param4  << std::endl;
		std::cout << "param5: "  << param5  << std::endl;
		std::cout << "param6: "  << param6  << std::endl;
		std::cout << "param7: "  << param7  << std::endl;
		std::cout << "param8: "  << param8  << std::endl;
		std::cout << "param9: "  << param9  << std::endl;
		std::cout << "param10: " << param10 << std::endl;

		Display::GetInstance()->_sWorker._man_neg_ratio = param1;
		SystemParams::_k_edge = param2;
		Display::GetInstance()->_sWorker._sim_timeout = param3;

		Display::GetInstance()->DeleteFolders();
	}
	else
	{
		Display::GetInstance()->_sWorker._man_neg_ratio = 0.435;
		//SystemParams::_k_edge = 1000;
		//Display::GetInstance()->_sWorker._sim_timeout = 1000000;
	}
	
	// ------

	if (SystemParams::_seed <= 0)
	{
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
	Display::ShowGL(argc, argv);

	std::cout << "program done\n";

	if (StuffWorker::_cGrid) { delete StuffWorker::_cGrid; }
	
	return 0;
}

