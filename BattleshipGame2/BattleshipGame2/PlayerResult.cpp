#include "PlayerResult.h"
#include <iostream>
#include <sstream>

PlayerResult::PlayerResult(const std::string& name) : _totalNumPointsFor(0), _totalNumPointsAgainst(0), _totalNumLosses(0), _totalNumWins(0), _name(name)
{
}

string PlayerResult::getReport() const
{
	ostringstream stream;
	stream << "\nPlayer " << _name << " results so far:" << std::endl;
	stream << "Number of victories: " << _totalNumWins << endl;
	stream << "Total score for player (so far): " << _totalNumPointsFor << endl;
	stream << "Number of losses: " << _totalNumLosses << endl;
	stream << "Total score against (so far): " << _totalNumPointsAgainst << std::endl;
	cout << stream.str();
	return stream.str();
}

void PlayerResult::clear()
{
	_totalNumPointsAgainst = 0;
	_totalNumPointsAgainst = 0;
	_totalNumLosses = 0;
	_totalNumWins = 0;
}



