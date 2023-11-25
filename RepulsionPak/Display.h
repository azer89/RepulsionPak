
/*
================================================================================
Reza Adhitya Saputra
radhitya@uwaterloo.ca
================================================================================
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <memory>

//include "Simulation.h"
//#include "ShapeRadiusMatching.h"

#include "OpenCVWrapper.h"
#include "StuffWorker.h"

class Display
{
public:

	Display();
	~Display();

	void DoStuff();

	void Draw();
	void Update(int nScreenWidth = 0, int nScreenHeight = 0);
	bool KeyboardEvent(unsigned char nChar, int x, int y);
	bool MouseEvent(int button, int state, int x, int y);

public:

	static std::shared_ptr<Display> GetInstance();

	static void ShowGL(int argc, char** argv);
	static void ResizeCallback(int w, int h);
	static void ShowCallback();



	static void SpecialKeyboardCallback(int key, int x, int y);
	static void KeyboardCallback(unsigned char nChar, int x, int y);
	static void MouseCallback(int button, int state, int x, int y);
	static void MouseWheel(int button, int dir, int x, int y);
	static void MouseDragCallback(int x, int y);
	static void MouseMoveCallback(int x, int y);

	AVector MapScreenToFieldSpace(float nScreenX, float nScreenY);

	//void ThreadTask(std::string msg);

	/*
	_svg_snapshot_capture_time; // 1
	_png_snapshot_capture_time; // 2
	_sdf_capture_time;          // 3
	_rms_capture_time;          // 4
	*/

	void DrawSVGSnapshot(float time_delta);
	void DrawPNGSnapshot(float time_delta);
	void CalculateSDF(float time_delta);
	void CalculateFillRMS(float time_delta);

	void DeleteFolders();
	void DeleteFiles();

public:
	static std::shared_ptr<Display> _static_instance;

	//static char _sceneNameBuf[512];

	float _zoomFactor;
	float _xDragOffset;
	float _yDragOffset;
	float _oriXDragOffset;
	float _oriYDragOffset;

	static float _initScreenWidth;
	static float _initScreenHeight;

	float _screenWidth;
	float _screenHeight;

	std::string _window_title;

	// limiting drawing frame-rate
	//int _frame_rate_time;
	int _time_sum;

	int _noise_time_counter;
	int _noise_time;

	// physics
	//float _slowMotionRatio; // = 10.0f;
	//float _timeElapsed;     // = 0;
	//int   _current_time;              // total time (with or without simulation)
	int _previous_time;
	int _simulation_time;
	//int   _timeprev;          // 0



	int  _frameCounter;
	//MotionUnderGravitation* _motionUnderGravitation; // new MotionUnderGravitation(AVector(0.0f, -9.81f));

	// things
	//ShapeRadiusMatching srMatching;
	StuffWorker _sWorker;

	// click
	AVector _clickPoint;

	// video capture
	//float _captureTime;
	//float _captureTimeCounter;
	//int _captureIntCounter;

	/*
	_svg_snapshot_capture_time; // 1
	_png_snapshot_capture_time; // 2
	_sdf_capture_time;          // 3
	_rms_capture_time;          // 4
	*/
	float _svg_time_counter;
	int   _svg_int_counter;
	float _png_time_counter;
	int   _png_int_counter;
	float _sdf_time_counter;
	int   _sdf_int_counter;
	float _rms_time_counter;
	int   _rms_int_counter;

	int _prev_snapshot_time;
	int _prev_opengl_draw;
	//int _prev_draw_time1;
	//int _prev_draw_time2;
	//int _prev_draw_time3;
	//int _prev_draw_time4;

	//float _addElementTime;
	//float _addElementTimeCounter;
	//int _addElementIntCounter;

	OpenCVWrapper* _cvWrapper;
};

#endif
