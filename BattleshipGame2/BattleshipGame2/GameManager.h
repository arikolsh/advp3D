#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <memory>
#include "GameBoard.h"
#include "PlayerResult.h"

class GameManager {
public:
	explicit GameManager(std::string& searchDir, int threads);
	std::vector<std::pair<int, int>> getNextRoundPair(std::vector<std::vector<int>>& permMatrix, int& carriedPlayer) const;
	bool init();
	void runGame();

private:
	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	int GameManager::getPlayerFromDll(std::string dllPath, IBattleshipGameAlgo*& player);
	std::string _searchDir;
	int _threads;
	std::vector<std::string> _boardsPath; //holds the path for each board
	std::vector<std::string> _dllsPath;	//holds the path for each dll
	std::vector<std::string> _messages;
	std::vector<GameBoard> _boards; //holds vector of 3D boards
	std::vector<std::unique_ptr<IBattleshipGameAlgo>> _players;
	//player results for every player plus additional result container for carry player in case of odd num of players
	vector<PlayerResult> _playerResults;
	void runMatch(std::pair<int, int> playersPair, int boardNum, pair<int, int> resultIndices);
};
