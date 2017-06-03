#include "BoardUtils.h"
#include <vector>
#include <filesystem>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Ship.h"
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
/*
* split string into vector of substrings according to delimiter.
* example: "hi:how:are:you" => [hi,how,are,you] with delimiter ':' .
*/
void BoardUtils::split(string line, vector<string>& splitLine, char delimiter)
{
	stringstream lineStream(line);
	string part;

	while (getline(lineStream, part, delimiter))
	{
		splitLine.push_back(part);
	}
}
/*
* fill board3d with ships according to ships positioned in file.
*/
bool BoardUtils::fillBoardWithShipsFromFile(vector<vector<string>>& board3d, ifstream& file, string path, int rows, int cols, int depth)
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
/*
* return new padded vector of vector of string with the given dimensions.
* newBoard.size() will be the depth +PADDING (number of matrices)
* newBoard[0].size() will be the number of rows +PADDING (in each matrix)
* newBoard[0][0].size() will be the number of colums +PADDING (in each matrix).
*/
vector<vector<string>> BoardUtils::getNewEmptyBoard(int depth, int rows, int cols)
{
	vector<vector<string>> board3d(depth);
	//construct string of empty cells
	string  emptyCellStr(cols, EMPTY_CELL);
	//fill 3d board with empty cells
	for (int d = 0; d < depth; d++)
	{
		board3d[d] = vector<string>();
		for (int row = 0; row < rows; row++)
		{
			board3d[d].push_back(emptyCellStr);
		}
	}
	return board3d;
}
/*
* fill board3d with the board in given path (not necessarily a valid board), and put in depth, rows and cols its dimensions.
* return true if successful and false otherwise.
*/
bool BoardUtils::getBoardFromFile(vector<vector<string>> &board3d, string path, int& depth, int& rows, int& cols)
{
	string line;
	ifstream file(path);
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
	board3d = getNewEmptyBoard(depth + PADDING, rows + PADDING, cols + PADDING);
	// fill board with ships from file
	return fillBoardWithShipsFromFile(board3d, file, path, rows, cols, depth);
}
/*
* print vector<vector<string>> to console.
*/
void BoardUtils::printBoard(vector<vector<string>> board3d, bool printPadding)
{
	//// print 3d board ////
	size_t start = printPadding ? 0 : 1;
	size_t depthEnd = printPadding ? board3d.size() - 1 : board3d.size() - PADDING;
	size_t rowsEnd = printPadding ? board3d[0].size() - 1 : board3d[0].size() - PADDING;
	size_t colsEnd = printPadding ? board3d[0][0].size() - 1 : board3d[0][0].size() - PADDING;
	for (size_t depth = start; depth <= depthEnd; depth++)
	{
		cout << endl;
		for (size_t row = start; row <= rowsEnd; row++)
		{
			cout << board3d[depth][row].substr(start, colsEnd) << endl;
		}
	}
}
/*
* check for a given ship ,in a certain direction, on the board its surroundings, and return true
* iff they are empty.
*/
bool BoardUtils::isEmptySurroundings(vector<vector<string>>& board, int direction, int depth, int row, int col, int shipLen)
{
	bool valid = true;
	if (direction == RIGHT)
	{
		for (int i = 0; i < shipLen; i++) //general direction to the right
		{	//check one cell deep, one cell out
			valid &= board[depth + 1][row][col + i] == EMPTY_CELL;
			valid &= board[depth - 1][row][col + i] == EMPTY_CELL;
			//check one cell up, one cell down
			valid &= board[depth][row + 1][col + i] == EMPTY_CELL;
			valid &= board[depth][row - 1][col + i] == EMPTY_CELL;

			if (!valid)
			{
				return false;
			}
		}
	}
	else if (direction == DOWN)
	{
		for (int i = 0; i < shipLen; i++) //general direction down
		{	//check one cell deep, one cell out
			valid &= board[depth + 1][row + i][col] == EMPTY_CELL;
			valid &= board[depth - 1][row + i][col] == EMPTY_CELL;
			//check one cell right, one cell left
			valid &= board[depth][row + i][col + 1] == EMPTY_CELL;
			valid &= board[depth][row + i][col - 1] == EMPTY_CELL;
			if (!valid)
			{
				return false;
			}
		}
	}
	else if (direction == DEEP)
	{
		for (int i = 0; i < shipLen; i++) //general direction in depth
		{	//check one cell right, one cell left
			valid &= board[depth + i][row][col + 1] == EMPTY_CELL;
			valid &= board[depth + i][row][col - 1] == EMPTY_CELL;
			//check one cell up, one cell down
			valid &= board[depth + i][row + 1][col] == EMPTY_CELL;
			valid &= board[depth + i][row - 1][col] == EMPTY_CELL;
			if (!valid)
			{
				return false;
			}
		}
	}
	return true;
}
/*
* for a specifc ship on the board in a certain direction, mark its cells and return its length.
*/
int BoardUtils::markCellsAndGetLen(vector<vector<string>>& board, char ship, int direction, int depth, int row, int col)
{
	int k = 1;
	int shipLen = 1;
	if (direction == RIGHT) {
		while (board[depth][row][col + k] == ship) {
			board[depth][row][col + k] = VISITED_CELL;
			shipLen++;
			k++;
		}
	}
	else if (direction == DOWN) {
		while (board[depth][row + k][col] == ship) {
			board[depth][row + k][col] = VISITED_CELL;
			shipLen++;
			k++;
		}
	}
	else if (direction == DEEP) {
		while (board[depth + k][row][col] == ship) {
			board[depth + k][row][col] = VISITED_CELL;
			shipLen++;
			k++;
		}
	}
	return shipLen;
}
/*
* for a specifc cell which contains a ship type char, return its direction. can be RIGHT,DOWN,DEEP.
*/
int BoardUtils::findShipDirection(vector<vector<string>> board, int depth, int row, int col, char ship)
{
	if (ship == board[depth][row][col + 1])
	{
		return RIGHT;
	}
	if (ship == board[depth][row + 1][col])
	{
		return DOWN;
	}
	// else if (ship == board[d + 1][row][col])
	return DEEP;

}
/*
* check if valid board and put number of ships for each player in numShips[]
* return true iff no adjacent ships, no invalid shapes, number of ships is MAX_NUM_SHIPS.
*/
bool BoardUtils::isValidBoard(vector<vector<string>> board, int depth, int rows, int cols, int numShips[])
{
	auto playerNum = [](char c) {return tolower(c) == c ? 1 : 0; };
	for (int d = 1; d <= depth; d++)
	{
		//cout << "in depth: " << d << endl;
		for (int row = 1; row <= rows; row++)
		{
			//cout << "in row: " << row << endl;
			for (int col = 1; col <= cols; col++)
			{
				//cout << "in col: " << col << endl;
				//cout << "cell is: " << board[d][row][col] << endl;
				if (!Ship::isShip(board[d][row][col]))
				{ //not a ship skip to next iteration
					continue;
				}
				// encountered first cell of certain ship
				char ship = board[d][row][col];
				// mark first cell
				board[d][row][col] = VISITED_CELL;
				// find the general direction of the ship
				int direction = findShipDirection(board, d, row, col, ship);
				// mark ship's cells and get the length of ship
				int shipLen = markCellsAndGetLen(board, ship, direction, d, row, col);
				// check if size of ship is valid
				if (shipLen != Ship::getShipLenByType(ship))
				{
					return false;
				}
				// traverse surroundings to check invalid shape or adjacent ships
				if (!isEmptySurroundings(board, direction, d, row, col, shipLen)) {
					return false;
				}
				numShips[playerNum(ship)]++;
			}
		}
	}
	return true;
}