#include "Logger.h"
#include <fstream> 
#include <ctime>
#include <sstream>
#include <string>

mutex _mutex;

#define START "Program execution started..."
#define TAB '\t'
#define SIZE 26

//static variables
Logger* Logger::_instance = nullptr;
string Logger::_mode = "DEBUG";
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

void Logger::init(const string path, const string mode)
{
	_log.open(path);
	_mode = mode;
	_log << START << endl;
}
void Logger::log(const string messgae, const string severity)
{
	unique_lock<mutex> lock(_mutex);
	char currTime[SIZE];
	time_t now = time(0);
	ctime_s(currTime, sizeof(currTime), &now);

	//std::string thetime = ctime(&now);
	string sTime(currTime);
	sTime = sTime.substr(0, sTime.length() - 1);
	if (_mode == "DEBUG") {
		_log << sTime << TAB << messgae << TAB << severity << endl;
	}
}

void Logger::destroy() {
	delete _instance;
	_instance = nullptr;
}

Logger::~Logger()
{
	// do nothing - just for the delete call from destroy
}