#include <cstdlib>
#include "GameUtils.h"
#include <vector>
#include <filesystem>
#include "GameBoard.h"
#include "GameManager.h"
#include <iostream>

#define NUM_PLAYERS 2
#define A_NUM 0
#define B_NUM 1
#define ROWS 10
#define COLS 10

using namespace std;

bool PrintMode = false;

void printBoard(int player, char** board, int rows, int cols);

void getArgs(int argc, char** argv, bool& isQuiet, int& delay, string& searchDir);

int main(int argc, char* argv[])
{
	bool debug[]{ true,false,false };

	if (debug[0]) { //debug start
		vector<string> inputFiles = { "", "", "" }; //[battle board, dll 1, dll 2]
		vector<string> dllNames = { "", "" };
		vector<string> messages;
		bool isQuiet = false; //default
		int delay = 100; //default, milliseconds
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
		if (playerBoardA == nullptr) { return EXIT_FAILURE; }
		playerBoardB = game_board.getPlayerBoard(B_NUM);
		if (playerBoardB == nullptr)
		{
			/* free resources */
			GameBoard::destroyBoard(playerBoardA, ROWS);
			return EXIT_FAILURE;
		}

		if(PrintMode)
		{
			cout << "Full board:" << endl;
			game_board.printGameBoard();
			printBoard(A_NUM, playerBoardA, COLS, ROWS);
			printBoard(B_NUM, playerBoardB, COLS, ROWS);
		}

		/* init game manager */
		GameManager manager(&game_board, isQuiet, delay);

		/* declare players */
		IBattleshipGameAlgo* players[NUM_PLAYERS];

		//todo: get algorithms

		/*/* init player A #1#
		players[A_NUM]; //= new NaivePlayer();
		//players[A_NUM] = new SmartPlayer();
		players[A_NUM]->setBoard(A_NUM, const_cast<const char **>(playerBoardA), ROWS, COLS);
		if (players[A_NUM]->init(searchDir) == false)
		{
			/* free resources #1#
			GameBoard::destroyBoard(playerBoardA, ROWS);
			GameBoard::destroyBoard(playerBoardB, ROWS);
			delete players[A_NUM];
			return EXIT_FAILURE;
		}
		GameBoard::destroyBoard(playerBoardA, ROWS); //Not needed once A has set his own board
		/* freeing resources #1#
		delete players[A_NUM];*/
	} //debug end

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
	for (int i = 1; i < argc; i++)
	{

		if (argsVector[i] == "-quiet")
		{
			isQuiet = true;
		}
		else if (argsVector[i] == "-delay")
		{
			delay = stoi(argsVector[++i]);
		}
		else
		{
			searchDir = argsVector[i];
			cout << searchDir;
		}
	}
	if (isQuiet)
	{
		delay = 0;
	}
}