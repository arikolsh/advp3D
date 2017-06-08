#include "PlayerResult.h"
#include <iostream>

using namespace std;

PlayerResult::PlayerResult(const std::string& name) : _totalNumPointsFor(0), _totalNumPointsAgainst(0), _totalNumLosses(0), _totalNumWins(0), _name(name)
{
}

float PlayerResult::getWinPercentage() const
{
	float totalGames = _totalNumLosses + _totalNumWins;
	if (totalGames == 0)
	{
		return 0;
	}
	return (_totalNumWins / totalGames) * 100;
}



