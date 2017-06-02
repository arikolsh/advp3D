#include <vector>
#include <filesystem>
#include "GameManager.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include "GameBoard.h"
#define MIN(a, b) ((a < b) ? (a) : (b))
#define EMPTY_CELL '-'
#define PADDING 2
#define DIM_DELIMITER 'X'

using namespace std;

void getArgs(int argc, char** argv, int& threads, string& searchDir);
int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll);

void split(string line, vector<string>& splitLine, char delimiter)
{
	stringstream lineStream(line);
	string part;

	while (getline(lineStream, part, delimiter))
	{
		splitLine.push_back(part);
	}
}

bool fill3DBoardWithShipsFromFile(vector<vector<string>>& board3d, ifstream& file, string path, int rows, int cols, int depth)
{
	//// fill 3d board with ships ////
	string line;
	int boardCount = 1;
	while (boardCount <= depth && getline(file, line)) { //skip empty line
		int row = 1;
		while (row <= rows && getline(file, line)) {
			int m = MIN(cols, (int)line.length());
			for (int col = 1; col <= m; col++) { //1,2,3,...,cols-1,cols
				if (Ship::isShip(line[col - 1])) { //check if valid ship char
					board3d[boardCount][row][col] = line[col - 1];
				}
			}
			row++;
		}
		boardCount++;
	}
	if (boardCount < depth)
	{
		cout << "Error: wrong number of boards in file " << path << endl;
		return false;
	}
	return true;
}

vector<vector<string>> getNew3DBoard(int depth, int rows, int cols)
{
	vector<vector<string>> board3d(depth + PADDING);
	//construct string of empty cells
	ostringstream stringStream;
	for (int col = 0; col < cols + PADDING; col++)
	{
		stringStream << EMPTY_CELL;
	}
	string  emptyCellStr = stringStream.str();
	//fill 3d board with empty cells
	for (int d = 0; d < depth + PADDING; d++)
	{
		board3d[d] = vector<string>();
		for (int row = 0; row < rows + PADDING; row++)
		{
			board3d[d].push_back(emptyCellStr);
		}
	}
	return board3d;
}

bool get3DBoardFromFile(vector<vector<string>> &board3d, string path)
{
	string line;
	ifstream file(path);
	int rows, cols, depth;
	if (!file.is_open()) {
		cout << "Error: failed to open file " << path << endl;
		return false;
	}
	// get board dimensions 
	if (!getline(file, line))
	{
		cout << "Error: invalid file " << path << endl;
		return false;
	}
	// split line to vector with 'x' delimeter
	vector<string> dims;
	split(line, dims, DIM_DELIMITER);
	if (dims.size() != 3)
	{
		cout << "Error: invalid line in file " << path << endl;
		return false;
	}
	try {
		cols = stoi(dims[0]);
		rows = stoi(dims[1]);
		depth = stoi(dims[2]);
	}
	catch (exception const & e)
	{
		cout << "Error: invalid board dimensions in file " << path << ", " << e.what() << endl;
		return false;
	}
	// init 3d board with padding
	board3d = getNew3DBoard(depth, rows, cols);
	// fill board with ships from file
	return fill3DBoardWithShipsFromFile(board3d, file, path, rows, cols, depth);
}

void print3DBoard(vector<vector<string>> board3d, bool printPadding)
{
	//// print 3d board ////
	int start = printPadding ? 0 : 1;
	int depthEnd = printPadding ? board3d.size() - 1 : board3d.size() - PADDING;
	int rowsEnd = printPadding ? board3d[0].size() - 1 : board3d[0].size() - PADDING;
	int colsEnd = printPadding ? board3d[0][0].size() - 1 : board3d[0][0].size() - PADDING;
	for (int depth = start; depth <= depthEnd; depth++)
	{
		cout << endl;
		for (int row = start; row <= rowsEnd; row++)
		{
			cout << board3d[depth][row].substr(start, colsEnd) << endl;
		}
	}
}

int main(int argc, char* argv[])
{
	string path = "good_board.sboard";
	vector<vector<string>> board;
	get3DBoardFromFile(board, path);
	///////// validate 3d board /////////









	///////// init gameboard /////////
	GameBoard gameBoard(board);
	gameBoard.print(false);
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