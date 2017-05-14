#include "GameUtils.h"
#include <vector>
#include <filesystem>
#include "GameBoard.h"
#include "GameManager.h"
#include <iostream>
#include <windows.h>
#include "IBattleshipGameAlgo.h"

#define NUM_PLAYERS 2
#define A_NUM 0
#define B_NUM 1
#define ROWS 10
#define COLS 10

using namespace std;

bool PrintMode = true; //for debug purposes

void printBoard(int player, char** board, int rows, int cols);

void getArgs(int argc, char** argv, bool& isQuiet, int& delay, string& searchDir);
int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll);

int main(int argc, char* argv[])
{
	vector<string> inputFiles = { "", "", "" }; //[battle board, dll 1, dll 2]
	vector<string> dllNames = { "", "" };
	vector<string> messages;
	bool isQuiet = false; //default
	int delay = 1000; //default, milliseconds
	string searchDir = ""; //default
	char **playerBoardA, **playerBoardB;

	/* get command line arguments if any */
	getArgs(argc, argv, isQuiet, delay, searchDir);

	/* get DLLs and board file */
	int err = GameUtils::getInputFiles(inputFiles, messages, dllNames, searchDir);
	if (err) { return EXIT_FAILURE; }

	/* init game board data structure */
	GameBoard game_board(ROWS, COLS);
	game_board.init(inputFiles[0].c_str());

	/* get player boards */
	playerBoardA = game_board.getPlayerBoard(A_NUM);
	if (playerBoardA == nullptr)
	{
		return EXIT_FAILURE;
	}
	playerBoardB = game_board.getPlayerBoard(B_NUM);
	if (playerBoardB == nullptr)
	{
		/* free resources */
		GameBoard::destroyBoard(playerBoardA, ROWS);
		return EXIT_FAILURE;
	}

	if (PrintMode)
	{
		cout << "Full board:" << endl;
		game_board.printGameBoard();
		printBoard(A_NUM, playerBoardA, COLS, ROWS);
		printBoard(B_NUM, playerBoardB, COLS, ROWS);
		cout << "dll-1 is: " << inputFiles[1] << endl;
		cout << "dll-2 is: " << inputFiles[2] << endl;
		isQuiet = true;
		delay = 0;
	}

	/* init game manager */
	GameManager manager(&game_board, isQuiet, delay);

	/* declare players */
	IBattleshipGameAlgo* players[NUM_PLAYERS];

	/* load player A from dll */
	HINSTANCE hDllA;
	err = getPlayerFromDll(inputFiles[1], players[A_NUM], hDllA);
	if (err)
	{
		GameBoard::destroyBoard(playerBoardA, ROWS);
		GameBoard::destroyBoard(playerBoardB, ROWS);
		delete players[A_NUM];
		return EXIT_FAILURE;
	}

	/* init player A */
	players[A_NUM]->setBoard(A_NUM, const_cast<const char **>(playerBoardA), ROWS, COLS);
	if (players[A_NUM]->init(searchDir) == false)
	{
		GameBoard::destroyBoard(playerBoardA, ROWS);
		GameBoard::destroyBoard(playerBoardB, ROWS);
		delete players[A_NUM];
		FreeLibrary(hDllA);
		return EXIT_FAILURE;
	}
	GameBoard::destroyBoard(playerBoardA, ROWS); //Not needed once A has set his own board

	/* load player B from dll */
	HINSTANCE hDllB;
	err = getPlayerFromDll(inputFiles[2], players[B_NUM], hDllB);
	if (err)
	{
		GameBoard::destroyBoard(playerBoardB, ROWS);
		delete players[A_NUM];
		delete players[B_NUM];
		FreeLibrary(hDllA);
		return EXIT_FAILURE;
	}

	/* init player B */
	players[B_NUM]->setBoard(B_NUM, const_cast<const char **>(playerBoardB), ROWS, COLS);
	if (players[B_NUM]->init(searchDir) == false)
	{
		GameBoard::destroyBoard(playerBoardB, ROWS);
		delete players[A_NUM];
		delete players[B_NUM];
		FreeLibrary(hDllA);
		FreeLibrary(hDllB);
		return EXIT_FAILURE;
	}
	GameBoard::destroyBoard(playerBoardB, ROWS); //Not needed once A has set his own board

												 /* game execution */
	int winner = manager.runGame(players);

	/* output results */
	manager.gameOver(winner);

	/* freeing resources */
	delete players[A_NUM];
	delete players[B_NUM];

	/* close all dynamic libs we opened */
	FreeLibrary(hDllA);
	FreeLibrary(hDllB);

	return EXIT_SUCCESS;

}

int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll)
{
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

void printBoard(int player, char** board, int rows, int cols)
{
	cout << endl << "Player " << player << " Board:" << endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			cout << board[i][j];
		}
		cout << endl;
	}
	cout << endl;
}


void getArgs(int argc, char** argv, bool& isQuiet, int& delay, string& searchDir)
{
	vector<string> argsVector(argv, argv + argc);
	int i = 1;
	while (i < argc)
	{

		if (argsVector[i] == "-quiet")
		{
			isQuiet = true;
		}
		else if (argsVector[i] == "-delay")
		{
			i++;
			delay = stoi(argsVector[i]);
		}
		else
		{
			searchDir = argsVector[i];
		}
		i++;
	}
	if (isQuiet)
	{
		delay = 0;
	}
}