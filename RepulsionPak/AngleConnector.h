
#ifndef ANGLE_CONNECTOR_H
#define ANGLE_CONNECTOR_H

struct AngleConnector
{
public:
	int _index0;
	int _index1;
	float _angle;
	float _dist;
	float _maxDist;

	AngleConnector()
	{
		this->_index0 = -1;
		this->_index1 = -1;
		this->_angle = 0.0f;
		this->_dist = 0;
		this->_maxDist = 0;
	}

	AngleConnector(int index0, int index1, float angle, float dist)
	{
		this->_index0 = index0;
		this->_index1 = index1;
		this->_angle  = angle;
		this->_dist = dist;
		this->_maxDist = 0;
	}

	void MakeLonger(float growth_scale_iter, float dt)
	{
		this->_dist *= (1.0f + (growth_scale_iter * dt)); // grow a bit
	}
};

#endif