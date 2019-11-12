#ifndef __A_TIMER_STAT__
#define __A_TIMER_STAT__

class ATimerStat
{
private:
	float _total_time;
	float _counter;
	float _average;

public:
	ATimerStat()
	{
		_total_time = 0;
		_counter = 0;
		_average = 0;
	}

	void AddTime(float t)
	{
		_total_time += t;
		_counter += 1.0f;
		_average = _total_time / _counter;
	}

	float Avg()
	{
		return _average;
	}

};

#endif