#include "PlayerResult.h"

PlayerResult::PlayerResult(const std::string& name) : _totalNumPointsFor(0), _totalNumPointsAgainst(0), _totalNumLosses(0), _totalNumWins(0), _name(name)
{
}

std::string PlayerResult::getReport()
{
	return "";
	//todo
}


