#include "PlayerResult.h"
#include <iostream>

using namespace std;

PlayerResult::PlayerResult(size_t playerNum) : _totalNumPointsFor(0), _totalNumPointsAgainst(0), _totalNumLosses(0), _totalNumWins(0), _playerNum(playerNum)
{
}

float PlayerResult::getWinPercentage() const
{
	float totalGames = float(_totalNumLosses) + float(_totalNumWins);
	if (totalGames == 0)
	{
		return 0;
	}
	return (_totalNumWins / totalGames) * 100;
}

bool PlayerResult::cmd(const PlayerResult & p1, const PlayerResult & p2)
{
	return p1._totalNumWins > p2._totalNumWins;
}


