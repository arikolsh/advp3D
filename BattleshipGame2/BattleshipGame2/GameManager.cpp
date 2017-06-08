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

GameManager::GameManager(string& searchDir, int threads) : _searchDir(searchDir), _threads(threads) {}

void GameManager::runMatch(pair<int, int> playersPair, int boardNum, pair<int, int> resultIndices)
{
	cout << "Running match: " << "player " << playersPair.first << " against player " << playersPair.second << endl;//"   resultSlots: " << resultIndices.first << ", " << resultIndices.second << endl;
	//Logger* logger = Logger::getInstance();
	//logger->log("hi how are you", "ERROR");
	
	MatchManager matchManager(_boards[boardNum]);
	
	GameBoard board1(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	GameBoard board2(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	matchManager.buildPlayerBoards(_boards[boardNum], board1, board2);
	
	_players[playersPair.first]->setPlayer(playersPair.first);
	_players[playersPair.first]->setBoard(board1);
	
	_players[playersPair.second]->setPlayer(playersPair.second);
	_players[playersPair.second]->setBoard(board2);
	
	IBattleshipGameAlgo* players[2] = { _players[playersPair.first].get(), _players[playersPair.second].get() };
	int winner = matchManager.runGame(players);
	matchManager.gameOver(winner, _playerResults[resultIndices.first], _playerResults[resultIndices.second], _mutex);
	// Should delete players or is it smart ptrs ???????????????????????????????
	//delete players[0];
	//delete players[1];
}

void GameManager::runGame()
{
	int CARRIED_RESULT_SLOT = _playerResults.size() - 1; //this is relevant only if odd number of players
	//cout << "carried slot: " << CARRIED_RESULT_SLOT << endl;
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

			cout << "****************" << endl;
			for (int i = 0; i < pairs.size(); i++)
			{
				cout << pairs[i].first << "," << pairs[i].second << endl;
			}
			cout << "****************" << endl;
			counter += pairs.size();
			vector<thread> matchThreads;
			int lastThreadOffset = 0;
			bool usedCarryResult = false;
			while (pairs.size() > 0) //still pending tasks
			{ //------- round -------//
				while (numActiveThreads < _threads && pairs.size() > 0)
				{
					pair<int, int> currentPair = pairs.back();
					pair<int, int> resultIndices = make_pair(currentPair.first, currentPair.second);
					if ((currentPair.first == carriedPlayer || currentPair.second == carriedPlayer) && !usedCarryResult)
					{
						int firstResultIndex, secondResultIndex;
						firstResultIndex = (currentPair.first == carriedPlayer) ? CARRIED_RESULT_SLOT : currentPair.first;
						secondResultIndex = (currentPair.second == carriedPlayer) ? CARRIED_RESULT_SLOT : currentPair.second;
						resultIndices = make_pair(firstResultIndex, secondResultIndex);
						usedCarryResult = true;
					}
					matchThreads.push_back(thread(&GameManager::runMatch, this, currentPair, boardNum, resultIndices));
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

			for (int res = 0; res < _playerResults.size() - 1; res++)
			{ //print results exculding the carry
				//todo: IOMANIP?
				cout << res << "." << _playerResults[res].getReport() << endl;
			}

			if (carriedPlayer >= 0)
			{ //spill carried player result to its appropriate player result bucket and will be printed next round with next round results 
				_playerResults[carriedPlayer]._totalNumLosses += _playerResults[CARRIED_RESULT_SLOT]._totalNumLosses;
				_playerResults[carriedPlayer]._totalNumPointsAgainst += _playerResults[CARRIED_RESULT_SLOT]._totalNumPointsAgainst;
				_playerResults[carriedPlayer]._totalNumPointsFor += _playerResults[CARRIED_RESULT_SLOT]._totalNumPointsFor;
				_playerResults[carriedPlayer]._totalNumWins += _playerResults[CARRIED_RESULT_SLOT]._totalNumWins;
			}
		}
		//last print
		for (int res = 0; res < _playerResults.size() - 1; res++)
		{ //print results exculding the carry
		  //todo: IOMANIP?
			cout << res << "." << _playerResults[res].getReport() << endl;
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
	int err = GameUtils::getInputFiles(_boardsPath, _dllsPath, _messages, _searchDir);
	if (err) {
		//write to log
		return false;
	}
	for (size_t i = 0; i < _boardsPath.size(); i++)
	{
		if (!BoardUtils::getBoardFromFile(tmpBoard, _boardsPath[i], boardDepth, boardRows, BoardCols))
		{
			//write to log : cout << "Error: failed to read board from file " << path << endl;
			return false;
		}
		if (BoardUtils::isValidBoard(tmpBoard, boardDepth, boardRows, BoardCols, numShips)) {
			if (numShips[0] == numShips[1])
			{
				_boards.push_back(GameBoard(tmpBoard, boardRows, BoardCols, boardDepth));
				//_boards.erase(_boards.begin() + i); //renove invalid boards
				//write to log : cout << "Warning: board not balanced in file " << path << endl;
			}
			//_boards.erase(_boards.begin() + i); //renove invalid boards
			//write to log : cout << "Warning: invalid board in file " << path << endl;
		}
	}
	for (size_t i = 0; i < _dllsPath.size(); i++)
	{
		err = getPlayerFromDll(_dllsPath[i], tmp);
		if (err) {
			//write to log:
			return false;
		}
		// init player result for specific player
		_players.push_back(unique_ptr<IBattleshipGameAlgo>(tmp));
		string name = _dllsPath[i].substr(0, _dllsPath[i].size() - 4); //remove .dll suffix
		_playerResults.push_back(PlayerResult(name));
	}
	if ((_players.size() <= 1) || (_boards.size() == 0)) {
		_players.clear();
		// write to log
		return false;
	}
	// if num of players is odd then there will be a player result we will carry to next round
	if (_players.size() % 2 != 0)
	{
		PlayerResult carriedResult("carried");
		_playerResults.push_back(carriedResult);
	}
	return true;
}

int GameManager::getPlayerFromDll(string dllPath, IBattleshipGameAlgo *& player)
{
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoType)();
	GetAlgoType getAlgo;

	HINSTANCE hDll = LoadLibraryA(dllPath.c_str());
	if (!hDll)
	{
		//write to log : cout << "Cannot load dll: " << dllPath << endl;
		return FAILURE;
	}

	// Get function pointer
	getAlgo = GetAlgoType(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getAlgo)
	{
		//write to log : cout << "Algorithm initialization failed for dll: " << dllPath << endl;
		FreeLibrary(hDll);
		return FAILURE;
	}

	/* init player A */
	player = getAlgo();
	return SUCCESS;
}
