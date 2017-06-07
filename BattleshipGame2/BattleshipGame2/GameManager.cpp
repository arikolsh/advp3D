#include "GameManager.h"
#include "GameUtils.h"
#include "BoardUtils.h"
#include "IBattleshipGameAlgo.h"
#include <windows.h>
#include <vector>
#include <memory>

#define SUCCESS 0
#define FAILURE -1

using namespace std;

GameManager::GameManager(string& searchDir, int threads) : _searchDir(searchDir), _threads(0)
{
}

bool GameManager::init() {
	int numShips[] = { 0,0 };
	int boardDepth = 0, boardRows = 0, BoardCols = 0;
	vector<vector<vector<string>>> tmpBoards;
	IBattleshipGameAlgo* tmp;
	int err = GameUtils::getInputFiles(_boardsPath, _dllsPath, _messages, _searchDir);
	if (err) {
		//write to log
		return false;
	}
	tmpBoards.resize(_boardsPath.size());	//get the new size  - after we got result from getInputFiles(resize also add objects)
	for (size_t i = 0; i < _boardsPath.size(); i++)
	{
		if (!BoardUtils::getBoardFromFile(tmpBoards[i], _boardsPath[i], boardDepth, boardRows, BoardCols))
		{
			//write to log : cout << "Error: failed to read board from file " << path << endl;
			return false;
		}
		if (BoardUtils::isValidBoard(tmpBoards[i], boardDepth, boardRows, BoardCols, numShips)) {
			if (numShips[0] == numShips[1])
			{
				_boards.push_back(tmpBoards[i]);
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
		_players.push_back(unique_ptr<IBattleshipGameAlgo>(tmp));
	}
	if ((_players.size() <= 1) || (_boards.size() == 0)) {
		_players.clear();
		// write to log
		return false;
	}
	BoardUtils::printBoard(_boards[0], false);

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
