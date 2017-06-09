#include "SmartLogger.h"
#include "SmartBoard.h"
#include <iostream>
#include <sstream>

SmartLogger::SmartLogger() : _player(-1), _shouldLog(false) {}

SmartLogger::SmartLogger(string logPath, int player, bool shouldLog)
{
	_player = player, _logPath = logPath, _shouldLog = shouldLog;
}

SmartLogger::SmartLogger(const SmartLogger &logger)
{
	_player = logger._player, _logPath = logger._logPath, _shouldLog = logger._shouldLog;
}

SmartLogger::~SmartLogger()
{
	if (!_shouldLog) { return; }
	_logFile.close();
}

SmartLogger& SmartLogger::operator=(const SmartLogger& logger)
{
	_player = logger._player, _logPath = logger._logPath, _shouldLog = logger._shouldLog;
	return *this;
}

bool SmartLogger::createLog()
{
	if (!_shouldLog) { return false; }
	_logFile.open(_logPath);
	return true;
}

bool SmartLogger::log(const string msg)
{
	if (!_shouldLog) { return false; }
	_logFile << msg.c_str() << endl;
	return true;
}

void SmartLogger::log_3D_board(SmartBoard board, bool includePadding, int player)
{
	if (!_shouldLog) { return; }
	int i, j, k, start, last_row, last_col, last_depth;
	start = includePadding ? 0 : 1;
	last_row = includePadding ? board.rows() + 2 : board.rows() + 1;
	last_col = includePadding ? board.cols() + 2 : board.cols() + 1;
	last_depth = includePadding ? board.depth() + 2 : board.depth() + 1;
	//Print row-col cuts for depth index start...last_depth:
	for (k = start; k < last_depth; k++) {
		_logFile << "SmartPlayer number " << player << " board: row-col cut for depth " << k << endl;
		for (i = start; i < last_row; i++) {
			for (j = start; j < last_col; j++) {
				_logFile << board.At(i, j, k);
			}
			_logFile << endl;
		}
		_logFile << endl;
	}
	_logFile << endl;
}

void SmartLogger::log_potential_attacks(vector<Coordinate> potentialAttacks)
{
	if (!_shouldLog) { return; }
	ostringstream stream;
	stream << "Printing potential attacks for player " << _player << endl;
	for (auto c : potentialAttacks)
	{
		stream << c.row << "," << c.col << "," << c.depth << endl;
	}
	log(stream.str());
}