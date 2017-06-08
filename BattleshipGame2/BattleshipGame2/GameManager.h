#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <memory>
#include "GameBoard.h"
#include "PlayerResult.h"
typedef IBattleshipGameAlgo *(*GetAlgoType)();

class GameManager {
public:
	explicit GameManager(std::string& searchDir, int threads);
	std::vector<std::pair<int, int>> getNextRoundPair(std::vector<std::vector<int>>& permMatrix, int& carriedPlayer) const;
	bool init();
	void runGame();

	//player results for every player 
	vector<PlayerResult> _playerResults;

private:

	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	int GameManager::getPlayerFromDll(std::string dllPath, IBattleshipGameAlgo*& player) const;
	int getPlayerAlgoFromDll(string dllPath, GetAlgoType& algo) const;
	std::string _searchDir;
	int _threads;
	std::vector<std::string> _messages;
	std::vector<GameBoard> _boards; //holds vector of 3D boards
	vector<GetAlgoType> _playersGet;

	// additional result container for carry player in case of odd num of players
	PlayerResult _carryResult;
	void runMatch(pair<int, int> playersPair, int boardNum, PlayerResult& result1, PlayerResult& result2);
	void printResultsForPlayers();
};
