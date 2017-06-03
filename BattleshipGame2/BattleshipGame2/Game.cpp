#include <vector>
#include <filesystem>
#include <iostream>
#include <windows.h>
//#include "IBattleshipGameAlgo.h"
#include <sstream>
#include "GameBoard.h"
#include "BoardUtils.h"
#define MIN(a, b) ((a < b) ? (a) : (b))
#define EMPTY_CELL '-' //todo: eventually should be ' ' (space)
#define VISITED_CELL 'v'
#define PADDING 2
#define DIM_DELIMITER 'X' //according to this token we split the first line in every board file
#define RIGHT 0
#define DOWN 1
#define DEEP 2
#define MAX_NUM_SHIPS 5 //max number of ships per player


using namespace std;

void getArgs(int argc, char** argv, int& threads, string& searchDir);
int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll);

int main(int argc, char* argv[])
{

	////// to put in manager //////
	string path = "good_board.sboard";
	vector<vector<string>> board;
	int depth, rows, cols;
	// get3d board from file
	if (!BoardUtils::getBoardFromFile(board, path, depth, rows, cols))
	{
		cout << "Error: failed to read board from file " << path << endl;
		return false;
	}
	// validate board
	int numShips[] = { 0,0 };
	if (!BoardUtils::isValidBoard(board, depth, rows, cols, numShips)) {
		cout << "Warning: invalid board in file " << path << endl;
		return false; //todo: skip to next board
	}
	if (numShips[0] != numShips[1])
	{
		cout << "Warning: board not balanced in file " << path << endl;
	}
	BoardUtils::printBoard(board, false);

	///////// init gameboard /////////
	//GameBoard gameBoard(board,rows,cols,depth);
	//gameBoard.print(false);
}







int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll)
{ //todo: guarantee player is converted to ptr
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoType)();
	GetAlgoType getAlgo;

	hDll = LoadLibraryA(dllPath.c_str());
	if (!hDll)
	{
		cout << "Cannot load dll: " << dllPath << endl;
		return EXIT_FAILURE;
	}

	// Get function pointer

	getAlgo = GetAlgoType(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getAlgo)
	{
		cout << "Algorithm initialization failed for dll: " << dllPath << endl;
		FreeLibrary(hDll);
		return EXIT_FAILURE;
	}

	/* init player A */
	player = getAlgo();
	return EXIT_SUCCESS;
}

void getArgs(int argc, char** argv, int& threads, string& searchDir)
{
	vector<string> argsVector(argv, argv + argc);
	int i = 1;
	threads = 4; //todo: put in properties file
	while (i < argc)
	{
		if (argsVector[i] == "-threads")
		{
			i++;
			threads = stoi(argsVector[i]);
		}
		else
		{
			searchDir = argsVector[i];
		}
		i++;
	}
}