#pragma once

#include <fstream>
#include "SmartBoard.h"

using namespace  std;

class SmartLogger
{
public:
	SmartLogger();

	SmartLogger(string logPath, int player, bool shouldLog);

	SmartLogger(const SmartLogger &logger);  // copy constructor

	~SmartLogger(); // Destructor

	SmartLogger& operator=(const SmartLogger& logger);

	bool createLog();

	bool log(const string);

	void SmartLogger::log_3D_board(SmartBoard board, bool includePadding, int player);

	void log_potential_attacks(vector<Coordinate> potentialAttacks);


private:
	int _player;
	string _logPath;
	ofstream _logFile;
	bool _shouldLog;

};