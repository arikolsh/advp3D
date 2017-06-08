#include "GameManager.h"
#include "GameUtils.h"
#include "BoardUtils.h"
#include "IBattleshipGameAlgo.h"
#include <windows.h>
#include <vector>
#include <memory>
#include "PlayerResult.h"
#include <thread>
#include "MatchManager.h"
#include <iostream>
#include <sstream>
#include "Logger.h"

#define SUCCESS 0
#define FAILURE -1

using namespace std;

GameManager::GameManager(string& searchDir, int threads) : _searchDir(searchDir), _threads(threads), _carryResult("carried")
{
}

void GameManager::runMatch(pair<int, int> playersPair, int boardNum, PlayerResult& result1, PlayerResult& result2)
{
	ostringstream  s;
	s << "I AM IN THREAD !!!" << "players: " << playersPair.first << "," << playersPair.second << endl;;
	std::cout << s.str();
	Logger* logger = Logger::getInstance();
	logger->log("hi how are you", "ERROR");
	MatchManager matchManager(_boards[boardNum]);
	GameBoard board1(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	GameBoard board2(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	matchManager.buildPlayerBoards(_boards[boardNum], board1, board2);
	if (true) return;

	_players[playersPair.first]->setBoard(board1);
	_players[playersPair.second]->setBoard(board2);

	///from here up everything is good
	IBattleshipGameAlgo* players[2] = { _players[playersPair.first].get(), _players[playersPair.second].get() };
	//matchManager.runGame(players, result1, result2); //todo: uncomment
	//todo: run game need to get result by reference and update them
}

void GameManager::runGame()
{
	int numPlayers = _players.size();
	for (auto boardNum = 0; boardNum < _boards.size(); boardNum++)
	{		//------- board rounds -------//
		/*
		* holds all the possible pairs of players for a match.
		* permMatrix[i][j]=1 i and j already played
		* against each other. for every i permMatrix[i][i] = 1 in advance because
		* player_i cannot play againset himself.
		*/
		vector<vector<int>> permMatrix(numPlayers, vector<int>(numPlayers));
		int counter = 0;
		while (counter < numPlayers*numPlayers - numPlayers)
		{	//------- one board round -------//
			int numActiveThreads = 0;
			int carriedPlayer = -1; //this will hold the player index that played twice
			vector<pair<int, int>> pairs = move(getNextRoundPair(permMatrix, carriedPlayer)); //get next pairs for round and update carriedPlayer
			if (carriedPlayer >= 0) { _carryResult.clear(); }
			counter += pairs.size();
			vector<thread> matchThreads;
			int lastThreadOffset = 0;
			bool usedCarryResultSlot = false;
			while (pairs.size() > 0) //still pending tasks
			{ //------- round -------//
				while (numActiveThreads < _threads && pairs.size() > 0)
				{
					pair<int, int> currentPair = pairs.back();
					PlayerResult firstResultSlot = _playerResults[currentPair.first];
					PlayerResult secondResultSlot = _playerResults[currentPair.second];
					if (!usedCarryResultSlot && carriedPlayer >= 0)
					{
						if (currentPair.first == carriedPlayer)
						{
							firstResultSlot = _carryResult;
							usedCarryResultSlot = true;
						}
						else if (currentPair.second == carriedPlayer)
						{
							secondResultSlot = _carryResult;
							usedCarryResultSlot = true;
						}

					}
					matchThreads.push_back(thread(&GameManager::runMatch, this, currentPair, boardNum, firstResultSlot, secondResultSlot));
					pairs.pop_back();
					numActiveThreads++;
				}
				for (auto i = lastThreadOffset; i < matchThreads.size(); i++)
				{ //wait for matches to finish
					if (matchThreads[i].joinable())
						matchThreads.at(i).join();
				}
				lastThreadOffset = numActiveThreads;
				numActiveThreads = 0;
			}

			for (int res = 0; res < _playerResults.size(); res++)
			{ //print results exculding the carry
				//todo: IOMANIP?
				std::cout << res << "." << _playerResults[res].getReport() << endl;
			}

			if (carriedPlayer >= 0)
			{ //spill carried player result to its appropriate player result bucket and will be printed next round with next round results 
				_playerResults[carriedPlayer]._totalNumLosses += _carryResult._totalNumLosses;
				_playerResults[carriedPlayer]._totalNumPointsAgainst += _carryResult._totalNumPointsAgainst;
				_playerResults[carriedPlayer]._totalNumPointsFor += _carryResult._totalNumPointsFor;
				_playerResults[carriedPlayer]._totalNumWins += _carryResult._totalNumWins;
			}
		}
		std::cout << "##### end #####" << endl;
		//last print
		for (int res = 0; res < _playerResults.size(); res++)
		{ //print results exculding the carry
		  //todo: IOMANIP?
			std::cout << res << "." << _playerResults[res].getReport() << endl;
		}


	}

}

vector<pair<int, int>> GameManager::getNextRoundPair(vector<vector<int>>& permMatrix, int& carriedPlayer) const
{
	int numPlayers = _players.size();
	/*
	* isActive[i] stores true iff player_i was selected for the round.
	* cleared after every round.
	*/
	vector<bool> isActive(numPlayers, false);
	for (int i = 0; i < numPlayers; i++) { permMatrix[i][i] = 1; }
	///get next round pairs ///
	vector<pair<int, int>> pairs;
	for (size_t player = 0; player < numPlayers; player++)
	{

		if (isActive[player]) { continue; }
		size_t other = 0;
		for (; other < numPlayers; other++)
		{
			if (isActive[other] || permMatrix[player][other] == 1) { continue; }
			pairs.push_back(make_pair(player, other));
			isActive[player] = true;
			isActive[other] = true;
			permMatrix[player][other] = 1;
			break;
		}

		if (other == numPlayers)
		{	// player left with no pair
			// assign someone to him
			int k = 1;
			while (k < numPlayers)
			{
				carriedPlayer = (player + k) % numPlayers;
				if (permMatrix[player][carriedPlayer] != 1)
				{
					pairs.push_back(make_pair(player, carriedPlayer));
					isActive[player] = true;
					permMatrix[player][carriedPlayer] = 1;
					break;
				}
				k++;
			}
		}
	}
	return pairs;
}

bool GameManager::init() {
	int numShips[] = { 0,0 };
	int boardDepth = 0, boardRows = 0, BoardCols = 0;
	vector<vector<string>> tmpBoard;
	IBattleshipGameAlgo* tmp;
	vector<string> dllPaths;
	vector<string> boardPaths;
	int err = GameUtils::getInputFiles(boardPaths, dllPaths, _messages, _searchDir);
	if (err) {
		//write to log
		return false;
	}
	for (size_t i = 0; i < boardPaths.size(); i++)
	{
		if (!BoardUtils::getBoardFromFile(tmpBoard, boardPaths[i], boardDepth, boardRows, BoardCols))
		{
			//write to log : std::cout << "Error: failed to read board from file " << path << endl;
			return false;
		}
		if (BoardUtils::isValidBoard(tmpBoard, boardDepth, boardRows, BoardCols, numShips)) {
			if (numShips[0] == numShips[1])
			{
				_boards.push_back(GameBoard(tmpBoard, boardRows, BoardCols, boardDepth));
				//_boards.erase(_boards.begin() + i); //renove invalid boards
				//write to log : std::cout << "Warning: board not balanced in file " << path << endl;
			}
			//_boards.erase(_boards.begin() + i); //renove invalid boards
			//write to log : std::cout << "Warning: invalid board in file " << path << endl;
		}
	}
	for (size_t i = 0; i < dllPaths.size(); i++)
	{
		cout << dllPaths[i] << endl;
		err = getPlayerFromDll(dllPaths[i], tmp);
		if (err) {
			//write to log:
			return false;
		}
		// init player result for specific player
		_players.push_back(unique_ptr<IBattleshipGameAlgo>(tmp));
		string name = dllPaths[i].substr(0, dllPaths[i].size() - 4); //remove .dll suffix
		_playerResults.push_back(PlayerResult(name));
	}
	if ((_players.size() <= 1) || (_boards.size() == 0)) {
		_players.clear();
		// write to log
		return false;
	}
	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int GameManager::getPlayerFromDll(string dllPath, IBattleshipGameAlgo *& player) const
{
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoType)();
	GetAlgoType getAlgo;

	HINSTANCE hDll = LoadLibraryA(dllPath.c_str());
	if (!hDll)
	{
		//write to log : std::cout << "Cannot load dll: " << dllPath << endl;
		return FAILURE;
	}

	// Get function pointer
	getAlgo = GetAlgoType(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getAlgo)
	{
		//write to log : std::cout << "Algorithm initialization failed for dll: " << dllPath << endl;
		FreeLibrary(hDll);
		return FAILURE;
	}

	/* init player A */
	player = getAlgo();
	return SUCCESS;
}
