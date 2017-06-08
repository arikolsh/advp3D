#pragma once
#include <ostream>

class PlayerResult
{
public:
	PlayerResult() = delete;
	explicit PlayerResult(const std::string &name);
	void clear(); //clear fields
	int _totalNumPointsFor;
	int _totalNumPointsAgainst;
	int _totalNumLosses;
	int _totalNumWins;
	std::string _name;
	void getReport() const; 
};