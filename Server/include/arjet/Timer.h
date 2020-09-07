#pragma once
#include <iostream>
#include <chrono>
class Timer {
public:
	void Start() { //Resets start time
		startTime = std::chrono::system_clock::now();
	}
	void Stop() {
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count();
	}
	auto Results(){
		return duration;
	}
private:
	std::chrono::system_clock::time_point startTime;
	long long duration;
};