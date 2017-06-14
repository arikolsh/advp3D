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


GameManager::GameManager(string& searchDir, int threads) : _searchDir(searchDir), _maxThreads(threads)
{
	_logger = Logger::getInstance();
}

void GameManager::runMatch(pair<int, int> playersPair, int boardNum)
{
	ostringstream stream;
	stream << "Running match: " << "player " << playersPair.first << " against player " << playersPair.second;
	_logger->log(stream.str());

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

void GameManager::runMatchV2(pair<int, int> playersPair, int boardNum)
{
	ostringstream stream;
	stream << "Running match: " << "player " << playersPair.first << " against player " << playersPair.second;
	_logger->log(stream.str());

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
	PlayerResult res1(playersPair.first);
	PlayerResult res2(playersPair.second);
	// Update PlayerResult for each player:
	matchManager.gameOver(winner, playersPair, res1, res2);
	_resultsPerPlayer[playersPair.first].safeAccPush(res1);
	_resultsPerPlayer[playersPair.second].safeAccPush(res2);
}

void GameManager::runGameV2()
{
	//schedule[i] holds the pair of the match and for each player the slot in which we need to update his result
	vector<pair<int, int>> allPossibleMatchPairs = getAllPossiblePairs();
	thread resultPrinterThread(&GameManager::resultPrinter, this, allPossibleMatchPairs.size()*_boards.size()); //run printer thread
	vector<thread> activeThreads;
	int boardNum = -1, matchCount = 0;
	while (true)
	{
		if (matchCount == 0) { boardNum++; }
		if (boardNum >= _boards.size()) { break; }
		pair<int, int> match = allPossibleMatchPairs[matchCount];
		activeThreads.push_back(thread(&GameManager::runMatchV2, this, match, boardNum));
		matchCount = (matchCount + 1) % allPossibleMatchPairs.size();
		if (activeThreads.size() >= _maxThreads) { //reached max thread count, join
			for (auto i = 0; i < activeThreads.size(); i++)
			{ //wait for matches to finish
				if (activeThreads[i].joinable())
					activeThreads.at(i).join();
			}
			activeThreads.clear(); // clear threads buffer
		}
	}
	if (activeThreads.size() > 0) { //still active threads
		for (auto i = 0; i < activeThreads.size(); i++)
		{ //wait for matches to finish
			if (activeThreads[i].joinable())
				activeThreads.at(i).join();
		}
		activeThreads.clear(); // clear threads buffer
	}
	if (resultPrinterThread.joinable())
		resultPrinterThread.join();
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
		int curRound = 0;
		while (curRound < schedule.size())
		{	//------- one board round -------//

			vector<pair<int, int>> pairs = schedule[curRound]; //get next pairs for round and update carriedPlayer
			vector<thread> activeThreads;
			int curPairIndex = 0;
			while (curPairIndex < pairs.size()) //still pending tasks
			{ //------- round -------//

				while (activeThreads.size() < _maxThreads && curPairIndex < pairs.size())
				{
					cout << "LOOP" << endl;
					pair<int, int> currentPair = pairs[curPairIndex++];
					if (currentPair.first == -1 || currentPair.second == -1)
					{
						continue;
					} //skip the player that didnt have a pair
					activeThreads.push_back(thread(&GameManager::runMatch, this, currentPair, boardNum));
				}
				for (auto i = 0; i < activeThreads.size(); i++)
				{ //wait for matches to finish
					if (activeThreads[i].joinable())
						activeThreads.at(i).join();
				}
				activeThreads.clear(); // clear threads buffer
			}
			printResultsForPlayers(); // Print current match results
			curRound++;
		}
	}
	printResultsForPlayers(); // Print Final results
}

void GameManager::resultPrinter(int numTotalMatches) //this the thread that prints results
{
	size_t currRound = 0;
	vector<PlayerResult> results;
	while (currRound < numTotalMatches)
	{
		if (currRound == 46)
		{
			cout << endl;
		}

		for (size_t i = 0; i < _playersGet.size(); i++)
		{//get results for player i
			results.push_back(_resultsPerPlayer[i].safeGet(currRound));
		}
		cout << "ROUND " << currRound << endl;
		printResultsForPlayers(results);
		results.clear();
		currRound++;
	}
}

void GameManager::printResultsForPlayers(vector<PlayerResult>& playerResults)
{
	ostringstream stream;
	stream << left << setfill(' ') << setw(5) << "#" << setw(_maxNameLength + 5) << "Player Name"
		<< setw(20) << "Total Wins" << setw(20) << "Total Losses" << setw(10) << "%" << setw(15)
		<< "Pts For" << setw(15) << "Pts Against" << endl;
	stream << setfill('-') << setw(115) << "-" << endl << setfill(' ');
	vector<PlayerResult> sortedResults(playerResults.begin(), playerResults.end());
	sort(sortedResults.begin(), sortedResults.end(), PlayerResult::cmd);
	for (int i = 0; i < sortedResults.size(); i++)
	{
		stream << setw(5) << to_string(i + 1).append(".")
			<< setw(_maxNameLength + 5) << _playerNames[sortedResults[i]._playerNum]
			<< setw(20) << sortedResults[i]._totalNumWins
			<< setw(20) << sortedResults[i]._totalNumLosses
			<< setw(10) << setprecision(2) << fixed << sortedResults[i].getWinPercentage()
			<< setw(15) << sortedResults[i]._totalNumPointsFor
			<< setw(15) << sortedResults[i]._totalNumPointsAgainst << endl;
	}
	cout << stream.str() << endl;
	//_logger->log(stream.str());
}

void GameManager::printResultsForPlayers()
{
	ostringstream stream;
	stream << left << setfill(' ') << setw(5) << "#" << setw(_maxNameLength + 5) << "Player Name"
		<< setw(20) << "Total Wins" << setw(20) << "Total Losses" << setw(10) << "%" << setw(15)
		<< "Pts For" << setw(15) << "Pts Against" << endl;
	stream << setfill('-') << setw(115) << "-" << endl << setfill(' ');
	vector<PlayerResult> sortedResults(_playerResults.begin(), _playerResults.end());
	sort(sortedResults.begin(), sortedResults.end(), PlayerResult::cmd);
	for (int i = 0; i < sortedResults.size(); i++)
	{
		stream << setw(5) << to_string(i + 1).append(".")
			<< setw(_maxNameLength + 5) << _playerNames[sortedResults[i]._playerNum]
			<< setw(20) << sortedResults[i]._totalNumWins
			<< setw(20) << sortedResults[i]._totalNumLosses
			<< setw(10) << setprecision(2) << fixed << sortedResults[i].getWinPercentage()
			<< setw(15) << sortedResults[i]._totalNumPointsFor
			<< setw(15) << sortedResults[i]._totalNumPointsAgainst << endl;
	}
	cout << stream.str() << endl;
	//_logger->log(stream.str());
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
		auto mid = players.size() / 2;
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

vector<pair<int, int>> GameManager::getAllPossiblePairs() const
{
	vector<vector<pair<int, int>>> roundsSchedule = getAllRoundsSchedule();
	vector<pair<int, int>> allPairs;
	for (auto v = 0; v < roundsSchedule.size(); v++)
	{
		for (auto p = 0; p < roundsSchedule[v].size(); p++)
		{
			pair<int, int> currentPair = roundsSchedule[v][p];
			if (currentPair.first == -1 || currentPair.second == -1) { continue; } //skip the player that didnt have a pair
			allPairs.push_back(currentPair);
		}
	}
	return allPairs;
}

void GameManager::initPlayersDetails(vector<string> &dllPaths)
{
	ostringstream stream;
	for (size_t i = 0; i < dllPaths.size(); i++)
	{
		GetAlgoType tmpGetAlgo;
		int err = getPlayerAlgoFromDll(dllPaths[i], tmpGetAlgo);
		if (err) {
			stream.str(string()); //clear stream
			stream << "Warning: invalid dll in file" << dllPaths[i] << ", skip to next dll";
			_logger->log(stream.str());
			continue;
		}
		_playersGet.push_back(tmpGetAlgo); //dll is good
		stream.str(string()); //clear stream
		stream << "valid dll in file" << dllPaths[i] << ", added to manager";
		_logger->log(stream.str());
		// init player result for specific player
		// get player name: remove .dll suffix and last '/'
		auto x = dllPaths[i].find_last_of("/\\");
		string name = dllPaths[i].substr(x + 1, dllPaths[i].size() - 4 - x - 1);
		if (_maxNameLength <= name.size()) {
			_maxNameLength = name.size();
		}
		_playerNames.push_back(name);
		_resultsPerPlayer = vector<SafeAccResultsVector>(_playersGet.size());
		auto currentPlayerNum = _playersGet.size() - 1;
		_playerResults.push_back(PlayerResult(currentPlayerNum));
		stream.str(string()); //clear stream
		stream << "added result slot for player" << name;
		_logger->log(stream.str());
	}
}

bool GameManager::initBoards(vector<string> boardPaths)
{
	ostringstream stream;
	vector<vector<string>> tmpBoard;
	int numShips[] = { 0,0 };
	int boardDepth = 0, boardRows = 0, BoardCols = 0;
	for (size_t i = 0; i < boardPaths.size(); i++)
	{
		if (!BoardUtils::getBoardFromFile(tmpBoard, boardPaths[i], boardDepth, boardRows, BoardCols))
		{
			stream.str(string()); //clear stream
			stream << "Error: failed to read board from file " << boardPaths[i];
			_logger->log(stream.str());
			return false;
		}
		if (BoardUtils::isValidBoard(tmpBoard, boardDepth, boardRows, BoardCols, numShips)) {
			stream.str(string()); //clear stream
			if (numShips[0] == numShips[1])
			{
				stream << "valid board in file" << boardPaths[i] << ", added to manager";
				_logger->log(stream.str());
				_boards.push_back(GameBoard(tmpBoard, boardRows, BoardCols, boardDepth, numShips));
			}
			else
			{
				stream << "Warning: invalid board in file" << boardPaths[i] << ", skip to next board";
				_logger->log(stream.str());
			}
		}
	}
	return true;
}

bool GameManager::init() {
	ostringstream stream;
	vector<string> boardPaths;
	vector<string> dllPaths;
	int err = GameUtils::getInputFiles(boardPaths, dllPaths, _messages, _searchDir);
	for (int m = 0; m < _messages.size(); m++)
	{
		_logger->log(_messages[m]);
	}

	if (err) {
		return false;
	}

	if (!initBoards(boardPaths)) {
		return false;
	}

	initPlayersDetails(dllPaths);

	if ((_playersGet.size() <= 1) || (_boards.size() == 0)) {
		_logger->log("Error: invalid number of players or number of boards, exiting game");
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