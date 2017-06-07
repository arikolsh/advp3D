#pragma once
#include <string>
#include <mutex>

using namespace std;

class Logger {
public:
	static Logger* getInstance();
	static void init(const string path, string mode);
	// 2 layers of severity for now : warning and error
	//in debug mode print everything
	//in release mode print just what we ask for in the exercise(errors only)
	static void log(const string messgae, const string severity);
	static void destroy();
private:
	Logger();
	~Logger();
	Logger(const Logger& that) = delete;
	static string _mode;
	static Logger * _instance;
	static ofstream _log;
};