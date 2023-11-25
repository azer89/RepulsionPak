#ifndef __A_Square__
#define __A_Square__

#include "AnObject.h"

#include <iostream>
#include <vector>

#include "glew.h"
#include "freeglut.h"

class ASquare
{
public:
	ASquare(float x, float y, float length) :
		_x(x),
		_y(y),
		_length(length)
	{
		float offVal = 1.0f;
		_x1 = _x + offVal;
		_y1 = _y + offVal;
		_x2 = _x - offVal + _length;
		_y2 = _y - offVal + _length;

		_xCenter = _x + (_length * 0.5);
		_yCenter = _y + (_length * 0.5);

		_containerFlag = 0;

		//_elem_indices.reserve(SystemParams::_max_cg_indices);
		//_elem_indices_actual_sz = 0;
	}

	~ASquare()
	{
	}

public:
	float _x;
	float _y;
	float _length;

	float _xCenter;
	float _yCenter;

	/*
	0 be careful
	1 nope !!!
	*/
	int _containerFlag;

	// drawing
	float _x1;
	float _y1;
	float _x2;
	float _y2;

	std::vector<AnObject*>	_objects;

	std::vector<int> _closestGraphIndices;
	//std::vector<int> _elem_indices;
	//int _elem_indices_actual_sz;

	inline bool Contains(AnObject* obj)
	{
		return !(obj->_x < _x ||
			obj->_y < _y ||
			obj->_x > _x + _length ||
			obj->_y > _y + _length);
	}

	void Clear()
	{
		this->_objects.clear();
	}

	void Draw()
	{
		//if (!_objects.empty())
		{
			if (_containerFlag == 0)
			{
				//std::cout << "gg\n";
				glColor3f(1.0, 0.0, 0.0);
				glPointSize(3.0f);
				glBegin(GL_POINTS);
				glVertex2f(_xCenter, _yCenter);
				glEnd();

			}

			glColor3f(0.5, 0.5, 0.5);
			glLineWidth(0.5f);
			glBegin(GL_LINES);

			// topleft to topright
			glVertex2f(_x1, _y1);
			glVertex2f(_x2, _y1);

			// topright to bottom right
			glVertex2f(_x2, _y1);
			glVertex2f(_x2, _y2);

			// topleft to bottomleft
			glVertex2f(_x1, _y1);
			glVertex2f(_x1, _y2);

			// bottomleft to bottomright
			glVertex2f(_x1, _y2);
			glVertex2f(_x2, _y2);

			glEnd();

			glColor3f(0.223529412, 0.545098039, 0.796078431);
			glPointSize(5.0);
			glBegin(GL_POINTS);
			for (int a = 0; a < _objects.size(); a++)
			{
				glVertex2f(_objects[a]->_x, _objects[a]->_y);
			}
			glEnd();



			/*glColor3f(1, 0, 0);
			glLineWidth(1.0);
			glBegin(GL_LINES);
			for (int a = 0; a < _objects.size(); a++)
			{
				//glVertex2f(_objects[a]->_x, _objects[a]->_y);
				if (_objects[a]->HasNormalVector())
				{
					glVertex2f(_objects[a]->_x, _objects[a]->_y);
					glVertex2f(_objects[a]->_x + _objects[a]->_nx * 5, _objects[a]->_y + _objects[a]->_ny * 5);
				}
			}
			glEnd();*/

		}
	}
};

#endif