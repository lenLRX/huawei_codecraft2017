#ifndef __TIMER_H__
#define __TIMER_H__
#include <chrono>

using namespace std::chrono;

class Timer
{
public:
    static Timer& getInstance(){
		static Timer* instance = new Timer();
		return *instance;
	}

	void start(){
		start_time = high_resolution_clock::now();
	}

	bool timeout(){
		return duration_cast<seconds>(high_resolution_clock::now() - start_time).count() > 88;
	}

private:
    Timer(){};
	high_resolution_clock::time_point start_time;
};
#endif//__TIMER_H__