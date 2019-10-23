
#include "Display.h"

#include "glew.h"
#include "freeglut.h"

#include "imgui.h"
#include "imgui_impl_glut.h"

#include "SystemParams.h"

#include "ColorPalette.h"

#include <iostream>
#include <sstream>

#include <thread>

#include <cstdlib>



//#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

std::shared_ptr<Display> Display::_static_instance = nullptr;

float Display::_initScreenWidth  = 1800;
float Display::_initScreenHeight = 1000;

//char sceneNameBuf[512] = "";

//LPWSTR ConvertToLPWSTR(const std::string& s)
//{
//	LPWSTR ws = new wchar_t[s.size() + 1]; // +1 for zero at the end
//	copy(s.begin(), s.end(), ws);
//	ws[s.size()] = 0; // zero at the end
//	return ws;
//}

Display::Display() :
    _xDragOffset(0),
	_yDragOffset(0),
	_zoomFactor(1.0f),
	_screenWidth(0.0f),
	_screenHeight(0.0f),
	_window_title(SystemParams::_window_title)
{
	_clickPoint = AVector(-1000, -1000);

	//_prev_draw_time1 = 0;
	//_prev_draw_time2 = 0;
	//_prev_draw_time3 = 0;

	_svg_time_counter = 0.0f;
	_svg_int_counter  = 0;
	_png_time_counter = 0.0f;
	_png_int_counter  = 0;
	_sdf_time_counter = 0.0f;
	_sdf_int_counter  = 0;
	_rms_time_counter = 0.0f;
	_rms_int_counter  = 0;

	_prev_snapshot_time = 0;
	_prev_opengl_draw = 0;

	
	//_time            = 0;
	_previous_time   = 0;
	_simulation_time = 0;
	//_timeprev        = 0;

	_noise_time_counter = 0;
	_noise_time = 0;

	_frameCounter = 0;

	//_frame_rate_time = 33; // 30 fps
	_time_sum = 0;

	// video capture
	//_captureTime = 5000; // every 10 seconds
	//_captureTimeCounter    = 0;
	//_captureIntCounter     = 0;
	//_addElementTime        = 50000; // every 50 seconds
	//_addElementTimeCounter = 0;
	//_addElementIntCounter = 0;
	_cvWrapper = new OpenCVWrapper;
}

void Display::DoStuff()
{
	// perform something here
	_sWorker._containerWorker->LoadContainer(_sWorker._cGrid); // ugly code!!!
	_sWorker.LoadOrnaments();
	_sWorker.ProcessOrnaments();
	//_sWorker.InitVerlet();
	//_sWorker.DrawFrame(-1); // draw a frame
}

Display::~Display()
{
	if (_cvWrapper) { delete _cvWrapper; }
}

// static
std::shared_ptr<Display> Display::GetInstance()
{
	

	if (_static_instance == nullptr)
	{ 
		_initScreenWidth = SystemParams::_screen_width;
		_initScreenHeight = SystemParams::_screen_height;

		_static_instance = std::shared_ptr<Display>(new Display()); 
		_static_instance->_screenWidth = _initScreenWidth;
		_static_instance->_screenHeight = _initScreenHeight;
	}
	return _static_instance;
}

// static
void Display::ShowCallback()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//ImGui_ImplGLUT_Init();
	Display::GetInstance()->Draw();
	glutSwapBuffers();
}

void Display::DrawSVGSnapshot(float time_delta)
{
	_svg_time_counter += time_delta;

	if (_svg_time_counter > SystemParams::_svg_snapshot_capture_time)
	{
		int time1 = glutGet(GLUT_ELAPSED_TIME);

		_sWorker.SaveSVG(_svg_int_counter);

		int time2 = glutGet(GLUT_ELAPSED_TIME);
		_prev_snapshot_time += (time2 - time1);
		_svg_time_counter = 0;
		_svg_int_counter++;
	}
}

void Display::DrawPNGSnapshot(float time_delta)
{
	_png_time_counter += time_delta;

	//std::cout << SystemParams::_png_snapshot_capture_time << "\n";

	if (_png_time_counter > SystemParams::_png_snapshot_capture_time)
	{
		int time1 = glutGet(GLUT_ELAPSED_TIME);

		_sWorker.SavePNG(_png_int_counter);

		int time2 = glutGet(GLUT_ELAPSED_TIME);
		_prev_snapshot_time += (time2 - time1);
		_png_time_counter = 0;
		_png_int_counter++;


		/*
		AVector start_ln;
		AVector end_ln;
		AVector rand_pt;
		AVector pt_on_line;
		*/
		//_sWorker.rand_pt = AVector(rand() % 500, rand() % 500);
		//_sWorker.pt_on_line = UtilityFunctions::ClosestPtAtFiniteLine2(_sWorker.start_ln, _sWorker.end_ln, _sWorker.rand_pt);

	}

	
	
}

void Display::CalculateSDF(float time_delta)
{
	_sdf_time_counter += time_delta;
	if (_sdf_time_counter > SystemParams::_sdf_capture_time)
	{
		int time1 = glutGet(GLUT_ELAPSED_TIME);

		_sWorker.CalculateSDF(_sdf_int_counter, true);

		int time2 = glutGet(GLUT_ELAPSED_TIME);
		_prev_snapshot_time += (time2 - time1);
		_sdf_time_counter = 0;
		_sdf_int_counter++;
	}
}

void Display::CalculateFillRMS(float time_delta)
{
	
	_rms_time_counter += time_delta;

	// time period
	if (_rms_time_counter > SystemParams::_rms_capture_time)
	{
		int time1 = glutGet(GLUT_ELAPSED_TIME);

		_sWorker.CalculateFillAndRMS();
		//_sWorker.CalculateFillAndRMS(_rms_int_counter);
		//_sWorker.SaveDataToCSV();

		int time2 = glutGet(GLUT_ELAPSED_TIME);
		_prev_snapshot_time += (time2 - time1);
		_rms_time_counter = 0;
		_rms_int_counter++;
	}
	// F it, we need to update as many times as possible
	else if (std::abs(_sWorker._fill_diff) < SystemParams::_growth_threshold_a)
	{
		_sWorker.CalculateFillAndRMS();
	}
}

void Display::Draw()
{
	// ---------- calculate time ----------
	int current_time = glutGet(GLUT_ELAPSED_TIME); // current time
	int time_delta = current_time - _previous_time - _prev_snapshot_time - _prev_opengl_draw;
	_previous_time = current_time; 

	_prev_snapshot_time = 0; // reset 

	bool shouldSimulate = SystemParams::_simulate_1 || SystemParams::_simulate_2;

	if (!shouldSimulate && SystemParams::_sleep_time > 0)
	{ // SLEEEEP
		//std::cout << "sleep\n";
		int sleep_time = SystemParams::_sleep_time;
		std::chrono::milliseconds timespan(sleep_time);
		std::this_thread::sleep_for(timespan);
	}

	if (shouldSimulate && _simulation_time < 0.0001)
	{		
		// ---------- update boundary  ----------
		for (unsigned int a = 0; a < _sWorker._graphs.size(); a++)
		{
			_sWorker._graphs[a].UpdateBoundaryAndAvgEdgeLength();    // update things
			_sWorker._graphs[a]._currentArea = _sWorker._graphs[a].GetArea();  // update area
		}
		if (SystemParams::_output_files)
		{
			_sWorker.SaveSVG(_svg_int_counter++);
			_sWorker.SavePNG(_png_int_counter++);
		}
	}

	if (shouldSimulate)
	{
		_noise_time_counter += time_delta;
		if (_noise_time_counter >= 1000)
		{
			_sWorker.ComputePerlinMap(_simulation_time / 1000.0f);
			_noise_time_counter = 0;
		}

		DrawSVGSnapshot(time_delta);
		DrawPNGSnapshot(time_delta);
		CalculateFillRMS(time_delta);
		//CalculateSDF(time_delta);

		/*if (_sWorker._fill_ratio < _sWorker._man_neg_ratio * 0.75)
		{
			SystemParams::_k_edge_dynamic = 2.0f;
		}
		else if (_sWorker._fill_ratio > _sWorker._man_neg_ratio * 0.9)
		{
			SystemParams::_k_edge_dynamic = 0.5f;
		}
		else
		{
			SystemParams::_k_edge_dynamic = 1.0f;
		}*/

		// ---------- STOP ----------
		//if (_rms_int_counter   > SystemParams::_rms_window &&    // bigger than window
		//	_sWorker._fill_rms < SystemParams::_rms_threshold && // fill ratio does not improve
		//	_sWorker._numGrowingElement == 0)                    // no element is growing

		// _simulation_time
		// ---------- STOP ----------
		if(_sWorker._fill_ratio >= SystemParams::_target_fill_ratio)
		{			
			SystemParams::_simulate_1 = false; // flags
			SystemParams::_simulate_2 = false; // flags			

			std::cout << "is simulation done?\n";
			// Fill and SDF
			//_sWorker.RecreateDistanceTransform(4.0f);
			//for (AGraph& aGraph : _sWorker._graphs)  { aGraph.RecalculateArts(); }
			//_sWorker.CalculateFillAndRMS(_rms_int_counter++, false); // can be REMOVED
			//_sWorker.CalculateSDF(_sdf_int_counter++, false);
			/*_sWorker.SaveSVG(_svg_int_counter++); // can be REMOVED
			_sWorker.SavePNG(_png_int_counter++); // can be REMOVED
			_sWorker.SaveGraphs(); // can be REMOVED */
			
			//std::cout << "stop\n";

			// add new small elements
			if (!_sWorker._hasSmallElements && SystemParams::_peak_dist_stop < 100)
			{
				std::cout << "add small elements\n";

				_sWorker.SaveSVG(_svg_int_counter++);
				_sWorker.SavePNG(_png_int_counter++);
				
				//_sWorker.RecreateDistanceTransform(1.0f);
				int time_a = glutGet(GLUT_ELAPSED_TIME);
				std::cout << "1\n";
				for (AnElement& aGraph : _sWorker._graphs)  { aGraph.RecalculateArts(); }
				std::cout << "2\n";
				_sWorker.CalculateSDF(_sdf_int_counter++, false);
				std::cout << "3\n";

				if (SystemParams::_output_files)
				{
					//_sWorker.SaveGraphs();					
					std::cout << "4\n";

					std::cout << "save info\n";
					std::string infoFile = "info_main_1.txt";
					if (_sWorker._hasSmallElements) { infoFile = "info_main_2.txt"; }
					PathIO pIO;
					pIO.SaveInfo(SystemParams::_output_folder + infoFile,
						_simulation_time,           // 1
						_sWorker._fill_ratio,       // 2
						_sWorker._fill_rms,         // 3
						_sWorker._deformationValue, // 4
						_sWorker._avgSkinThickness, // 5
						_sWorker._graphs.size(),    // 6
						_frameCounter,              // 7
						_sWorker._numTriEdges,      // 8
						_sWorker._numAuxEdges,      // 9
						_sWorker._numPoints,        // 10
						_sWorker._numTriangles,    // 11
						SystemParams::_seed);
				}
				
				_sWorker._fill_ratio_array.clear(); // clearing the fill record to buy some time

				int time_b = glutGet(GLUT_ELAPSED_TIME);
				std::cout << "sdf time: " << time_b - time_a << "\n";

				int time_1 = glutGet(GLUT_ELAPSED_TIME);

				_sWorker.AddNewSmallElements();     // smalll elements
				SystemParams::_simulate_2 = true;   // rerun

				int time_2 = glutGet(GLUT_ELAPSED_TIME);

				std::cout << "small element time: " << time_2 - time_1 << "\n";
			}
			else
			{
				for (AnElement& aGraph : _sWorker._graphs)  { aGraph.RecalculateArts(); } // can be REMOVED				
				_sWorker.SaveSVG(_svg_int_counter++);

				if (SystemParams::_output_files)
				{
					//_sWorker.CalculateSDF(_sdf_int_counter++, false); // can be REMOVED
					//_sWorker.AnalyzeFinishedPacking();
					//_sWorker.SaveGraphs();

					// === can be REMOVED ===
					std::cout << "save info\n";
					std::string infoFile = "1st_simulation_statistics.txt";
					if (_sWorker._hasSmallElements) { infoFile = "2nd_simulation_statistics.txt"; }
					PathIO pIO;
					pIO.SaveInfo(SystemParams::_output_folder + infoFile,
						_simulation_time,           // 1
						_sWorker._fill_ratio,       // 2
						_sWorker._fill_rms,         // 3
						_sWorker._deformationValue, // 4
						_sWorker._avgSkinThickness, // 5
						_sWorker._graphs.size(),    // 6
						_frameCounter,              // 7
						_sWorker._numTriEdges,      // 8
						_sWorker._numAuxEdges,      // 9
						_sWorker._numPoints,        // 10
						_sWorker._numTriangles,    // 11
						SystemParams::_seed);
				}


				std::cout << "simulation DONE!!!\n";

				//glutLeaveMainLoop();
			}

		}
	} // if(simulate)

	// ---------- physics simulation ----------
	if (shouldSimulate)
	{
		_simulation_time += time_delta;
		_sWorker.Operate(SystemParams::_dt);      // calculation
	}

	// ---------- OpenGL Draw ----------
	_prev_opengl_draw = 0; // reset
	int timeA = glutGet(GLUT_ELAPSED_TIME); 

	_sWorker.Draw();

	//
	//ImGui_ImplGLUT_Init();

	// Draw imgui
	ImGui_ImplGLUT_NewFrame(this->_screenWidth, this->_screenHeight, 1.0f / 30.0f);
	ImGui::SetNextWindowPos(ImVec2(5, 15), ImGuiSetCond_FirstUseEver);  // set position

	bool show_another_window = false;	
	ImGui::Begin("PhysicsPak", &show_another_window, ImVec2(240, 600));

	//if (_rms_time_counter > 0)
	//{
		if (SystemParams::_simulate_1)
		{
			ImGui::Text("SIMULATION #1");
			ImGui::Text("Processing regular elements");
		}
		else if (SystemParams::_simulate_2)
		{
			ImGui::Text("SIMULATION #2");
			ImGui::Text("Proccessing filling elements");
		}
		else
		{
			ImGui::Text("SIMULATION HAS DONE");
			ImGui::Text("");
		}
	//}
	//else
	//{
	//	ImGui::Text("SIMULATION HASN'T STARTED");
	//	ImGui::Text("");
	//}
	ImGui::Separator();

	if (time_delta > 0) { ImGui::Text(("FPS: " + std::to_string(1000 / time_delta)).c_str()); }
	else { ImGui::Text("FPS : -"); }
	ImGui::Text(("Runtime: " + std::to_string((int)(_simulation_time / 1000.0f)) + " s").c_str());

	ImGui::Text(("Target fill ratio: " + std::to_string(SystemParams::_target_fill_ratio)).c_str());
	ImGui::Text(("Fill ratio: "   + std::to_string(_sWorker._fill_ratio)).c_str());

	ImGui::Text(("Num vertices: "     + std::to_string(_sWorker._numPoints)).c_str());

	//ImGui::Text(("Deformation: " + std::to_string(_sWorker._deformationValue)).c_str());

	ImGui::Text(("Average skin offset: " + std::to_string(_sWorker._avgSkinThickness)).c_str());
	ImGui::Text(("# Growing elements: " + std::to_string(_sWorker._numGrowingElement) + " / " + std::to_string(_sWorker._graphs.size())).c_str());

	
	//ImGui::Text(("_springs_thread_t = " + std::to_string(_sWorker._springs_thread_t)).c_str());
	ImGui::Text(("_c_pt_thread_t    = " + std::to_string(_sWorker._c_pt_thread_t)).c_str());
	ImGui::Text(("_c_pt_cpu_t       = " + std::to_string(_sWorker._c_pt_cpu_t)).c_str());

	//ImGui::Text(("_cg_thread_t      = " + std::to_string(_sWorker._cg_thread_t)).c_str());
	//ImGui::Text(("_cg_cpu_t         = " + std::to_string(_sWorker._cg_cpu_t)).c_str());
	
	
		
	//ImGui::Text(("# Points: "    + std::to_string(_sWorker._numPoints)).c_str());
	//ImGui::Text(("# Triangles: " + std::to_string(_sWorker._numTriangles)).c_str());

	ImGui::Text(("Seed: " + std::to_string(SystemParams::_seed)).c_str());

	//ImGui::Text(("Average scale: " + std::to_string(_sWorker._avgScaleFactor)).c_str());

	ImGui::Separator();

	if (ImGui::Button("Simulate")) { SystemParams::_simulate_1 = true; }
	ImGui::Separator();
	ImGui::Checkbox("Show container",  &SystemParams::_show_boundary);	
	ImGui::Checkbox("Render elements",   &SystemParams::_show_elements);
	ImGui::Checkbox("Show skins", &SystemParams::_show_element_boundary);


	ImGui::Checkbox("Show triangles", &SystemParams::_show_triangles);
	ImGui::Checkbox("Show bending edges", &SystemParams::_show_bending_springs);
	ImGui::Checkbox("Show shape matching", &SystemParams::_show_shape_matching);

	ImGui::Checkbox("Show uni art", &SystemParams::_show_uni_art);


	ImGui::Separator();

	if (ImGui::Button("Save to SVG"))       
	{ 
		//_sWorker.SaveGraphs(); 
		_sWorker.SaveSVG(_svg_int_counter++);
		_sWorker.CalculateSDF(_sdf_int_counter++, true);
	}


	if (ImGui::Button("Reload parameters")) { SystemParams::LoadParameters();  }


	if (ImGui::Button("Delete files"))
	{
		DeleteFiles();
	}

	
	ImGui::End();
	ImGui::Render();

	//ImGui::GetIO().RenderDrawListsFn = ImGui::GetDrawData();
	
	int timeB = glutGet(GLUT_ELAPSED_TIME);
	_prev_opengl_draw = timeB - timeA;

	if (shouldSimulate) { _frameCounter++; }
}

void Display::DeleteFolders()
{
	/*
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "FILL\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "SDF\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "PNG\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "SVG\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "ORI_ELEMENTS\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "DEFORMED_ELEMENTS\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "DEBUG\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "OVERLAP\\*.*";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del /Q " << SystemParams::_save_folder << "SKELETON\\*.*";
	std::system(ss.str().c_str()); }
	*/
}

void Display::DeleteFiles()
{
	DeleteFolders();
	
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "data.csv";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "dist.csv";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "dist_mask.csv";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "dist_all.csv";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "info_main_1.txt";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "info_main_2.txt";
	std::system(ss.str().c_str()); }
	{ std::stringstream ss;
	ss << "del " << SystemParams::_output_folder << "params.lua";
	std::system(ss.str().c_str()); }
}

void Display::Update(int nScreenWidth, int nScreenHeight)
{
	/*_screenWidth = nScreenWidth;
	_screenHeight = nScreenHeight;

	float xOffset  = 0;
	float yOffset  = 0;
	float scaleVal = 1.0;

	if (_screenWidth > _screenHeight) // wide
	{
		xOffset = (_screenWidth - _screenHeight)* 0.5f;
		scaleVal = _screenHeight / SystemParams::_upscaleFactor;
	}
	else // tall
	{
		yOffset = (_screenHeight - _screenWidth) * 0.5f;
		scaleVal = _screenWidth / SystemParams::_upscaleFactor;
	}

	float xZOffset = (1.0f - _zoomFactor) * _screenWidth * 0.5f;
	float yZOffset = (1.0f - _zoomFactor) * _screenHeight * 0.5f;

	glViewport(0, 0, _screenWidth, _screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D((0 - xOffset - xZOffset - _xDragOffset),     // left
		(_screenWidth - xOffset + xZOffset - _xDragOffset), // right
		(_screenHeight - yOffset + yZOffset - _yDragOffset), //bottom
		(0 - yOffset - yZOffset - _yDragOffset));	// flip the y axis // top
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(scaleVal, scaleVal, scaleVal);*/

	_screenWidth = nScreenWidth;
	_screenHeight = nScreenHeight;


	float sz = 0.0f;

	if (_screenWidth > _screenHeight) { sz = _screenHeight * _zoomFactor;	 } // wide
	else  { sz = _screenWidth * _zoomFactor; } // tall

	float xOffset = (_screenWidth - sz + _xDragOffset)* 0.5f;
	float yOffset = (_screenHeight - sz + _yDragOffset)* 0.5f;
	float scaleVal = sz / SystemParams::_upscaleFactor;

	//float xZOffset = (1.0f - _zoomFactor) * _screenWidth * 0.5f;
	//float yZOffset = (1.0f - _zoomFactor) * _screenHeight * 0.5f;

	glViewport(0, 0, _screenWidth, _screenHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D((0 - xOffset),     // left
		(_screenWidth - xOffset), // right
		(_screenHeight - yOffset), //bottom
		(0 - yOffset));	// flip the y axis // top

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(1.0, 1.0, 1.0);
	glScalef(scaleVal, scaleVal, scaleVal);
}

bool Display::KeyboardEvent(unsigned char nChar, int nX, int nY)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(nChar);

	/*if (nChar == '1')
	{
		SystemParams::_simulate_1 = true;
	}
	else if (nChar == '2')
	{
		SystemParams::_show_boundary = !SystemParams::_show_boundary;
	}
	else if (nChar == '3')
	{
		SystemParams::_show_elements = !SystemParams::_show_elements;
	}
	else if (nChar == '4')
	{
		SystemParams::_show_element_boundary = !SystemParams::_show_element_boundary;
	}
	else if (nChar == '5')
	{
		SystemParams::_show_triangles = !SystemParams::_show_triangles;
	}
	else if (nChar == '6')
	{
		SystemParams::_show_bending_springs = !SystemParams::_show_bending_springs;
	}
	else if (nChar == '7')
	{
		//_sWorker.SaveGraphs();
		_sWorker.SaveSVG(_svg_int_counter++);
	}
	else if (nChar == '8')
	{
		SystemParams::LoadParameters();
	}
	else if (nChar == '9')
	{
		DeleteFiles();
	}*/

	/*
	io.KeyMap[ImGuiKey_LeftArrow]  = GLUT_KEY_LEFT
	io.KeyMap[ImGuiKey_RightArrow] = GLUT_KEY_RIGHT
	io.KeyMap[ImGuiKey_UpArrow]    = GLUT_KEY_UP
	io.KeyMap[ImGuiKey_DownArrow]  = GLUT_KEY_DOWN
	*/

	

	return true;
}

// static
void Display::ShowGL(int argc, char **argv)
{
	

	glutInit(&argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

	glutInitWindowSize(_initScreenWidth, _initScreenHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow( Display::GetInstance()->_window_title.c_str());

	// callback
	glutDisplayFunc(      ShowCallback);
	glutIdleFunc(         ShowCallback);  // so we can animate !
	glutReshapeFunc(      ResizeCallback);	
	glutKeyboardFunc(     KeyboardCallback);
	glutSpecialFunc(      SpecialKeyboardCallback);
	glutMouseFunc(        MouseCallback);
	glutMouseWheelFunc(   MouseWheel);
	glutMotionFunc(       MouseDragCallback);
	glutPassiveMotionFunc(MouseMoveCallback);
	
	glEnable(GL_MULTISAMPLE);
	MyColor back_color = ColorPalette::_back_color;
	glClearColor((float)back_color._r / 255.0f, (float)back_color._g / 255.0f, (float)back_color._b / 255.0f, 1.0);
	//glClearColor(0.89, 0.95, 0.99, 1.0);
	//glClearColor(0.7, 1.0, 0.7, 1.0);
	//glClearColor(0.5, 0.5, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	//
	ImGui_ImplGLUT_Init();

	//ImGui_ImplGLUT_NewFrame(Display::_static_instance->_screenWidth, Display::_static_instance->_screenHeight, 1.0f / 30.0f);
	//ImGui::SetNextWindowPos(ImVec2(5, 15), ImGuiSetCond_FirstUseEver);  // set position

	glutMainLoop();
}

// static
void Display::MouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		// Zoom in
		Display::GetInstance()->_zoomFactor += 0.05;
		//std::cout << "zoom " << Display::GetInstance()->_zoomFactor << "\n";
		Display::GetInstance()->Update(Display::GetInstance()->_screenWidth, Display::GetInstance()->_screenHeight);
		glutPostRedisplay();
	}
	else
	{
		// Zoom out
		Display::GetInstance()->_zoomFactor -= 0.05;
		if (Display::GetInstance()->_zoomFactor < 0.1)
		{
			Display::GetInstance()->_zoomFactor = 0.1;
		}
		//std::cout << "zoom " << Display::GetInstance()->_zoomFactor << "\n";
		Display::GetInstance()->Update(Display::GetInstance()->_screenWidth, Display::GetInstance()->_screenHeight);
		glutPostRedisplay();
	}
	
}

// static
void Display::ResizeCallback(int w, int h)
{
	glViewport(0, 0, w, h);
	Display::GetInstance()->Update(w, h);
}

// static
void Display::SpecialKeyboardCallback(int key, int x, int y)
{
	//std::cout << "key! \n";

	if (key == GLUT_KEY_LEFT)
	{
		//std::cout << " left\n";
		Display::GetInstance()->_xDragOffset += 50.0f;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		//std::cout << " right\n";
		Display::GetInstance()->_xDragOffset -= 50.0f;
	}
	else if (key == GLUT_KEY_UP)
	{
		//std::cout << " up\n";
		Display::GetInstance()->_yDragOffset -= 50.0f;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		//std::cout << " down\n";
		Display::GetInstance()->_yDragOffset += 50.0f;
	}

	Display::GetInstance()->Update(Display::GetInstance()->_screenWidth, Display::GetInstance()->_screenHeight);
	glutPostRedisplay();
}

// static
void Display::KeyboardCallback(unsigned char nChar, int x, int y)
{
	if (Display::GetInstance()->KeyboardEvent(nChar, x, y)) { glutPostRedisplay(); }

	//std::cout << "key callback\n";
}

// static
void Display::MouseCallback(int button, int state, int x, int y)
{
	if (Display::GetInstance()->MouseEvent(button, state, x, y)) { glutPostRedisplay(); }
}

bool Display::MouseEvent(int button, int state, int x, int y)
{
	//x += SystemParams::_mouse_offset_x;
	//y += SystemParams::_mouse_offset_y;

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x + SystemParams::_mouse_offset_x, (float)y + SystemParams::_mouse_offset_y);

	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		io.MouseDown[0] = true;

		_clickPoint = MapScreenToFieldSpace((float)x, (float)y);
		//std::cout << "mouse down, x: " << _clickPoint.x << ", y: " << _clickPoint.y << "\n";
		// physics simulation
		_sWorker.FindClickedMass(_clickPoint.x, _clickPoint.y);
		_sWorker._containerWorker->_padCalc.FindClickedPoint(_clickPoint.x, _clickPoint.y);

		//_sWorker.GetPerlinVector(_clickPoint.x, _clickPoint.y);
	}
	else
	{
		io.MouseDown[0] = false;
	}

	return true;
}

// static
void Display::MouseDragCallback(int x, int y)
{
	//x += SystemParams::_mouse_offset_x;
	//y += SystemParams::_mouse_offset_y;

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x + SystemParams::_mouse_offset_x, (float)y + SystemParams::_mouse_offset_y);

	if (io.MouseDown[0])
	{
		AVector pos = Display::GetInstance()->MapScreenToFieldSpace((float)x, (float)y);
		std::pair<int, int> indices = Display::GetInstance()->_sWorker._clickedIndices;
		if (!SystemParams::_simulate_1)
		{
			std::cout << "drag disabled bruv\n";
			//Display::GetInstance()->_sWorker.DragClickedGraph(pos.x, pos.y);
		}
	}

	glutPostRedisplay();
}

// static
void Display::MouseMoveCallback(int x, int y)
{
	//x += SystemParams::_mouse_offset_x;
	//y += SystemParams::_mouse_offset_y;

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2((float)x + SystemParams::_mouse_offset_x, (float)y + SystemParams::_mouse_offset_y);

	

	glutPostRedisplay();
}

// this is because the display space isn't the same with the field space
AVector Display::MapScreenToFieldSpace(float xScreen, float yScreen)
{	
	float fX, fY;
	float sz;

	if (_screenWidth > _screenHeight) // wide
	{
		/*float offVal = (_screenWidth - _screenHeight) * 0.5f;
		fX = (xScreen - offVal) / (_screenHeight);
		//fX = ( (xScreen - (_screenWidth * 0.5f) ) / _screenHeight) + 0.5; // this is also correct
		fY = (yScreen) / (_screenHeight);
		*/
		sz = _screenHeight * _zoomFactor;
		
	}
	else // tall
	{
		sz = _screenWidth * _zoomFactor;

		//fX = xScreen / (_screenWidth);
		//fY = ( (yScreen - (_screenHeight * 0.5f) ) / _screenWidth) + 0.5f;
	}

	
	float offValX = (_screenWidth - sz + _xDragOffset) * 0.5f;
	float offValY = (_screenHeight - sz + _yDragOffset) * 0.5f;
	fX = (xScreen - offValX) / sz;
	fY = (yScreen - offValY) / sz;

	//std::cout << "uv space     = (" << fX << ", " << fY << ")\n";

	fX *= SystemParams::_upscaleFactor;
	fY *= SystemParams::_upscaleFactor;

	//std::cout << "screen space = (" << xScreen << ", " << yScreen << ")\n";
	//std::cout << "image space  = (" << fX      << ", " << fY      << ")\n\n";

	return AVector(fX, fY); // flipped
}