#include "Logger.h"
#include <fstream> 
#include <ctime>
#include <string>
#include <future>


#define START "Program execution started..."
#define TAB '\t'
#define SIZE 26

static mutex _mutex;

//static variables
Logger* Logger::_instance = nullptr;
ofstream Logger::_log;

Logger* Logger::getInstance() {
	if (!_instance) {
		_instance = new Logger();
	}
	return _instance;
}

Logger::Logger() {
	// Just for the linker
}

void Logger::init(const string path)
{
	_log.open(path);
	_log << START << endl;
}

void Logger::log(const string messgae)
{
	//unique_lock<mutex> lock(_mutex);
	std::lock_guard<std::mutex> lk(_mutex);
	char currTime[SIZE];
	time_t now = time(0);
	ctime_s(currTime, sizeof(currTime), &now);

	string sTime(currTime);
	sTime = sTime.substr(0, sTime.length() - 1);
	_log << sTime << TAB << messgae << endl;

}

void Logger::destroy() {
	delete _instance;
	_instance = nullptr;
}

Logger::~Logger()
{
	// do nothing - just for the delete call from destroy
}