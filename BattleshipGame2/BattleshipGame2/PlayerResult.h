#pragma once
#include <ostream>

using namespace std;

class PlayerResult
{
public:
	PlayerResult() = delete;
	explicit PlayerResult(size_t playerNum);
	float getWinPercentage() const;
	static bool cmd(const PlayerResult& p1, const PlayerResult& p2);
	int _totalNumPointsFor;
	int _totalNumPointsAgainst;
	int _totalNumLosses;
	int _totalNumWins;
	size_t _playerNum;
};