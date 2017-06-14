#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include "GameBoard.h"
#include "PlayerResult.h"
#include "Logger.h"
#include "SafeAccResultsVector.h"

typedef IBattleshipGameAlgo *(*GetAlgoType)();

class GameManager {
public:
	explicit GameManager(std::string& searchDir, int threads);
	void runGameV2();
	bool init();
	void runGame(); //todo: delete
	//player results for every player 
	vector<PlayerResult> _playerResults; //todo: delte

private:
	Logger* _logger;
	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	int getPlayerAlgoFromDll(string dllPath, GetAlgoType& algo) const;
	std::string _searchDir;
	int _maxThreads;
	std::vector<std::string> _messages;
	std::vector<GameBoard> _boards; //holds vector of 3D boards
	vector<GetAlgoType> _playersGet;
	vector<string> _playerNames;
	vector<SafeAccResultsVector> _resultsPerPlayer;
	int _maxNameLength = 0;
	// additional result container for carry player in case of odd num of players
	void runMatch(pair<int, int> playersPair, int boardNum);
	void initPlayersDetails(vector<string>& dllPaths);
	bool initBoards(vector<string> boardPaths);
	void runMatchV2(pair<int, int> playersPair, int boardNum);
	void printResultsForPlayers(); //todo: del
	vector<vector<pair<int, int>>> getAllRoundsSchedule() const;
	vector<pair<int, int>> getAllPossiblePairs() const;
	void resultPrinter(int numTotalMatches);
	void printResultsForPlayers(vector<PlayerResult>& playerResults);
};