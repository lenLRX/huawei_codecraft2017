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

	void set(int t){
		limit = t;
	}

	void start(){
		start_time = high_resolution_clock::now();
	}

	bool timeout(){
		return duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() > limit;
	}

	int get(){
		return duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count();
	}

private:
    Timer(){};
	int limit;
	high_resolution_clock::time_point start_time;
};
#endif//__TIMER_H__