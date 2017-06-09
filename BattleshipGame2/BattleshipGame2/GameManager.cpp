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
#include <iomanip>
#include <algorithm>

#define SUCCESS 0
#define FAILURE -1

using namespace std;


GameManager::GameManager(string& searchDir, int threads) : _searchDir(searchDir), _threads(threads)
{
	_logger = Logger::getInstance();
}

void GameManager::runMatch(pair<int, int> playersPair, int boardNum)
{
	ostringstream stream;
	stream << "Running match: " << "player " << playersPair.first << " against player " << playersPair.second;
	Logger* logger = Logger::getInstance();
	logger->log(stream.str());

	// Initialize the Match Manager (with the full board):
	MatchManager matchManager(_boards[boardNum]);

	// Set boards for both players:
	GameBoard board1(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	GameBoard board2(_boards[boardNum].rows(), _boards[boardNum].cols(), _boards[boardNum].depth());
	matchManager.buildPlayerBoards(_boards[boardNum], board1, board2);

	// Set both players:
	auto player1 = unique_ptr<IBattleshipGameAlgo>(_playersGet[playersPair.first]());
	player1->setPlayer(playersPair.first);
	player1->setBoard(board1);
	auto player2 = unique_ptr<IBattleshipGameAlgo>(_playersGet[playersPair.second]());
	player2->setPlayer(playersPair.second);
	player2->setBoard(board2);

	// Run this match:
	IBattleshipGameAlgo* players[2] = { player1.get(), player2.get() };
	int winner = matchManager.runGame(players, { playersPair.first, playersPair.second });

	// Update PlayerResult for each player:
	matchManager.gameOver(winner, playersPair, _playerResults[playersPair.first], _playerResults[playersPair.second]);
}

void GameManager::runGame()
{
	vector<vector<pair<int, int>>> schedule = getAllRoundsSchedule();
	for (auto boardNum = 0; boardNum < _boards.size(); boardNum++)
	{		//------- board rounds -------//
		/*
		* holds all the possible pairs of players for a match.
		* permMatrix[i][j]=1 i and j already played
		* against each other. for every i permMatrix[i][i] = 1 in advance because
		* player_i cannot play againset himself.
		*/
		while (schedule.size() > 0)
		{	//------- one board round -------//
			int numActiveThreads = 0;
			vector<pair<int, int>> pairs = schedule.back(); //get next pairs for round and update carriedPlayer
			schedule.pop_back();
			vector<thread> matchThreads;
			int lastThreadOffset = 0;
			while (pairs.size() > 0) //still pending tasks
			{ //------- round -------//
				while (numActiveThreads < _threads && pairs.size() > 0)
				{
					pair<int, int> currentPair = pairs.back();
					if (currentPair.first == -1 || currentPair.second == -1) { continue; } //skip the player that didnt have a pair
					matchThreads.push_back(thread(&GameManager::runMatch, this, currentPair, boardNum));
					pairs.pop_back();
					numActiveThreads++;
				}
				for (auto i = lastThreadOffset; i < matchThreads.size(); i++)
				{ //wait for matches to finish
					if (matchThreads[i].joinable())
						matchThreads.at(i).join();
				}
				lastThreadOffset += numActiveThreads;
				numActiveThreads = 0;
			}
			printResultsForPlayers(); // Print current match results
		}
	}
	printResultsForPlayers(); // Print Final results
}


void GameManager::printResultsForPlayers()
{
	ostringstream stream;
	stream << left << setfill(' ') << setw(5) << "#" << setw(_maxNameLength + 5) << "Player Name"
		<< setw(20) << "Total Wins"	<< setw(20) << "Total Losses" << setw(10) << "%" << setw(15) 
		<< "Pts For" << setw(15) << "Pts Against" << endl;
	stream << setfill('-') << setw(115) << "-" << endl << setfill(' ');
	vector<PlayerResult> sortedResults(_playerResults.begin(), _playerResults.end());
	sort(sortedResults.begin(), sortedResults.end(), PlayerResult::cmd);
	for (int i = 0; i < sortedResults.size(); i++)
	{
		stream << setw(5) << to_string(i + 1).append(".")
			<< setw(_maxNameLength + 5) << sortedResults[i]._name
			<< setw(20) << sortedResults[i]._totalNumWins
			<< setw(20) << sortedResults[i]._totalNumLosses
			<< setw(10) << setprecision(2) << fixed << sortedResults[i].getWinPercentage()
			<< setw(15) << sortedResults[i]._totalNumPointsFor
			<< setw(15) << sortedResults[i]._totalNumPointsAgainst << endl;
	}
	cout << stream.str() << endl;
	_logger->log(stream.str());
}

vector<vector<pair<int, int>>> GameManager::getAllRoundsSchedule() const
{
	vector<int> players;
	for (int i = 0; i < _playersGet.size(); i++)
	{
		players.push_back(i);
	}
	vector < vector<pair<int, int>>> schedule;
	if (players.size() % 2 == 1)
	{ //num of players is odd
		players.push_back(-1); //add dummy player
	}
	for (auto i = 0; i < players.size() - 1; i++)
	{
		int mid = players.size() / 2;
		vector<int> left(players.begin(), players.begin() + mid);
		vector<int> right(players.begin() + mid, players.end());
		reverse(right.begin(), right.end());
		// switch sides after each round
		if (i % 2 == 1)
		{
			schedule.push_back(GameUtils::zip(left, right));
		}
		else
		{
			schedule.push_back(GameUtils::zip(right, left));
		}
		int last = players.back();
		players.pop_back();
		players.insert(players.begin() + 1, last);
	}
	// generate flipped pairs
	vector <vector<pair<int, int>>> scheduleFinal;
	for (int i = 0; i < schedule.size(); i++)
	{
		scheduleFinal.push_back(schedule[i]);
	}

	for (int pv = 0; pv < schedule.size(); pv++)
	{
		vector<pair<int, int>> pairVec = schedule[pv];
		vector<pair<int, int>> flipPairVec;
		for (int i = 0; i < pairVec.size(); i++)
		{
			flipPairVec.push_back(make_pair(pairVec[i].second, pairVec[i].first));
		}
		scheduleFinal.push_back(flipPairVec);
	}
	return scheduleFinal;
}

bool GameManager::init() {
	ostringstream stream;
	Logger* logger = Logger::getInstance();
	int numShips[] = { 0,0 };
	int boardDepth = 0, boardRows = 0, BoardCols = 0;
	vector<vector<string>> tmpBoard;
	vector<string> boardPaths;
	vector<string> dllPaths;
	int err = GameUtils::getInputFiles(boardPaths, dllPaths, _messages, _searchDir);
	for (int m = 0; m < _messages.size(); m++)
	{
		logger->log(_messages[m]);
	}
	if (err) {
		return false;
	}
	for (size_t i = 0; i < boardPaths.size(); i++)
	{
		if (!BoardUtils::getBoardFromFile(tmpBoard, boardPaths[i], boardDepth, boardRows, BoardCols))
		{
			stream.str(string()); //clear stream
			stream << "Error: failed to read board from file " << boardPaths[i];
			logger->log(stream.str());
			return false;
		}
		if (BoardUtils::isValidBoard(tmpBoard, boardDepth, boardRows, BoardCols, numShips)) {
			stream.str(string()); //clear stream
			if (numShips[0] == numShips[1])
			{
				stream << "valid board in file" << boardPaths[i] << ", added to manager";
				logger->log(stream.str());
				_boards.push_back(GameBoard(tmpBoard, boardRows, BoardCols, boardDepth));
			}
			else
			{
				stream << "Warning: invalid board in file" << boardPaths[i] << ", skip to next board";
				logger->log(stream.str());
			}
		}
	}
	for (size_t i = 0; i < dllPaths.size(); i++)
	{
		GetAlgoType tmpGetAlgo;
		err = getPlayerAlgoFromDll(dllPaths[i], tmpGetAlgo);
		if (err) {
			stream.str(string()); //clear stream
			stream << "Warning: invalid dll in file" << dllPaths[i] << ", skip to next dll";
			logger->log(stream.str());
			continue;
		}
		_playersGet.push_back(tmpGetAlgo); //dll is good
		stream.str(string()); //clear stream
		stream << "valid dll in file" << dllPaths[i] << ", added to manager";
		logger->log(stream.str());
		// init player result for specific player
		// get player name: remove .dll suffix and last '/'
		int x = dllPaths[i].find_last_of("/\\");
		string name = dllPaths[i].substr(x + 1, dllPaths[i].size() - 4 - x - 1);
		if (_maxNameLength <= name.size()) {
			_maxNameLength = name.size();
		}
		_playerResults.push_back(PlayerResult(name));
		stream.str(string()); //clear stream
		stream << "added result slot for player" << name;
		logger->log(stream.str());
	}
	if ((_playersGet.size() <= 1) || (_boards.size() == 0)) {
		logger->log("Error: invalid number of players or number of boards, exiting game");
		return false;
	}

	cout << "Number of legal players: " << _playersGet.size() << endl;
	cout << "Number of legal boards: " << _boards.size() << endl;

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int GameManager::getPlayerAlgoFromDll(string dllPath, GetAlgoType& algo) const
{
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoType)();
	GetAlgoType getAlgo;

	HINSTANCE hDll = LoadLibraryA(dllPath.c_str());
	if (!hDll)
	{
		return FAILURE;
	}

	// Get function pointer
	getAlgo = GetAlgoType(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getAlgo)
	{
		FreeLibrary(hDll);
		return FAILURE;
	}
	algo = getAlgo;
	return SUCCESS;
}
