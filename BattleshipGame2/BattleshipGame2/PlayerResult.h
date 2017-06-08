#pragma once
#include <ostream>

using namespace std;

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
	string _name;
	string getReport() const;
};