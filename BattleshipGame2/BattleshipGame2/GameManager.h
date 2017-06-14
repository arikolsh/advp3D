#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include "GameBoard.h"
#include "PlayerResult.h"
#include "Logger.h"

typedef IBattleshipGameAlgo *(*GetAlgoType)();

class GameManager {
public:
	explicit GameManager(std::string& searchDir, int threads);
	vector<vector<pair<int, int>>> getAllRoundsSchedule() const;
	vector<pair<int, int>> getAllPossiblePairs() const;
	void runGameV2();
	vector<pair<pair<int, int>, pair<int, int>>> getSchedule() const;
	void initPlayersDetails(vector<string>& dllPaths);
	bool initBoards(vector<string> boardPaths);
	bool init();
	void runGame();
	//player results for every player 
	vector<PlayerResult> _playerResults;

private:
	Logger* _logger;
	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	int getPlayerAlgoFromDll(string dllPath, GetAlgoType& algo) const;
	std::string _searchDir;
	int _threads;
	std::vector<std::string> _messages;
	std::vector<GameBoard> _boards; //holds vector of 3D boards
	vector<GetAlgoType> _playersGet;
	vector<string> _playerNames;
	int _maxNameLength = 0;
	// additional result container for carry player in case of odd num of players
	void runMatch(pair<int, int> playersPair, int boardNum);
	void printResultsForPlayers();
};