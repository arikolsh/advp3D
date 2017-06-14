#include "BoardUtils.h"
#include <vector>
#include <filesystem>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include "Ship.h"
#include "Logger.h"
#define MIN(a, b) ((a < b) ? (a) : (b))
#define EMPTY_CELL ' '
#define VISITED_CELL 'v'
#define PADDING 2
#define DIM_DELIMITER 'x' //according to this token we split the first line in every board file
#define RIGHT 0
#define DOWN 1
#define DEEP 2

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
					board3d[row][col][boardCount] = line[col - 1];
				}
			}
			row++;
		}
		boardCount++;
	}
	if (boardCount < depth)
	{
		Logger* logger = Logger::getInstance();
		ostringstream stream;
		stream << "Error: wrong number of boards in file " << path;
		logger->log(stream.str());
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
	vector<vector<string>> board3d(rows);
	//construct string of empty cells
	string  emptyCellStr(depth, EMPTY_CELL);
	//fill 3d board with empty cells
	for (int row = 0; row < rows; row++)
	{
		board3d[row] = vector<string>();
		for (int col = 0; col < cols; col++)
		{
			board3d[row].push_back(emptyCellStr);
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
	ostringstream stream;
	string line;
	ifstream file(path);
	Logger* logger = Logger::getInstance();
	if (!file.is_open()) {
		stream.str(string());
		stream << "Error: failed to open file " << path;
		logger->log(stream.str());
		return false;
	}
	// get board dimensions 
	if (!getline(file, line))
	{
		stream.str(string());
		stream << "Error: invalid file " << path;
		logger->log(stream.str());
		return false;
	}
	// split line to vector with 'x' delimeter
	vector<string> dims;
	split(line, dims, DIM_DELIMITER);
	if (dims.size() != 3)
	{
		stream.str(string());
		stream << "Error: invalid line in file " << path;
		logger->log(stream.str());
		return false;
	}
	try {
		cols = stoi(dims[0]);
		rows = stoi(dims[1]);
		depth = stoi(dims[2]);
	}
	catch (exception const & e)
	{
		stream.str(string());
		stream << "Error: invalid board dimensions in file " << path << ", " << e.what();
		logger->log(stream.str());
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
	size_t rowsEnd = printPadding ? board3d.size() - 1 : board3d.size() - PADDING;
	size_t colsEnd = printPadding ? board3d[0].size() - 1 : board3d[0].size() - PADDING;
	size_t depthEnd = printPadding ? board3d[0][0].size() - 1 : board3d[0][0].size() - PADDING;

	for (size_t depth = start; depth <= depthEnd; depth++)
	{
		cout << endl;
		for (size_t row = start; row <= rowsEnd; row++)
		{
			for (size_t col = start; col <= colsEnd; col++) {
				cout << board3d[row][col][depth];
			}
			cout << endl;
		}
		cout << endl;
	}
	cout << endl;
}
/*
* check for a given ship ,in a certain direction, on the board its surroundings, and return true
* iff they are empty.
*/
bool BoardUtils::isEmptySurroundings(vector<vector<string>>& board, int direction, int depth, int row, int col, int shipLen)
{
	bool valid = true;
	if (direction == DEEP)
	{
		for (int i = 0; i < shipLen; i++) //general direction in depth
		{	//check one cell deep, one cell out
			valid &= board[row + 1][col][depth + i] == EMPTY_CELL;
			valid &= board[row - 1][col][depth + i] == EMPTY_CELL;
			//check one cell up, one cell down
			valid &= board[row][col + 1][depth + i] == EMPTY_CELL;
			valid &= board[row][col - 1][depth + i] == EMPTY_CELL;

			if (!valid)
			{
				return false;
			}
		}
	}
	else if (direction == RIGHT)
	{
		for (int i = 0; i < shipLen; i++) //general direction to the right
		{	//check one cell deep, one cell out
			valid &= board[row + 1][col + i][depth] == EMPTY_CELL;
			valid &= board[row - 1][col + i][depth] == EMPTY_CELL;
			//check one cell right, one cell left
			valid &= board[row][col + i][depth + 1] == EMPTY_CELL;
			valid &= board[row][col + i][depth - 1] == EMPTY_CELL;
			if (!valid)
			{
				return false;
			}
		}
	}
	else if (direction == DOWN)
	{
		for (int i = 0; i < shipLen; i++) //general direction down
		{	//check one cell right, one cell left
			valid &= board[row + i][col][depth + 1] == EMPTY_CELL;
			valid &= board[row + i][col][depth - 1] == EMPTY_CELL;
			//check one cell up, one cell down
			valid &= board[row + i][col + 1][depth] == EMPTY_CELL;
			valid &= board[row + i][col - 1][depth] == EMPTY_CELL;
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
	if (direction == DEEP) {
		while (board[row][col][depth + k] == ship) {
			board[row][col][depth + k] = VISITED_CELL;
			shipLen++;
			k++;
		}
	}
	else if (direction == RIGHT) {
		while (board[row][col + k][depth] == ship) {
			board[row][col + k][depth] = VISITED_CELL;
			shipLen++;
			k++;
		}
	}
	else if (direction == DOWN) {
		while (board[row + k][col][depth] == ship) {
			board[row + k][col][depth] = VISITED_CELL;
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
	if (ship == board[row][col][depth + 1])
	{
		return DEEP;
	}
	if (ship == board[row][col + 1][depth])
	{
		return RIGHT;
	}
	return DOWN;

}
/*
* check if valid board and put number of ships for each player in numShips[]
* return true iff no adjacent ships, no invalid shapes.
*/
bool BoardUtils::isValidBoard(vector<vector<string>> board, int depth, int rows, int cols, int numShips[])
{
	auto playerNum = [](char c) {return tolower(c) == c ? 1 : 0; };
	for (int d = 1; d <= depth; d++)
	{
		for (int row = 1; row <= rows; row++)
		{
			for (int col = 1; col <= cols; col++)
			{
				if (!Ship::isShip(board[row][col][d]))
				{ //not a ship skip to next iteration
					continue;
				}
				// encountered first cell of certain ship
				char ship = board[row][col][d];
				// mark first cell
				board[row][col][d] = VISITED_CELL;
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

vector<vector<string>> BoardUtils::getBoardCopy(const vector<vector<string>> board)
{
	vector<vector<string>> copy(board.size());
	for (int row = 0; row < board.size(); row++)
	{
		copy[row] = vector<string>();
		for (int col = 0; col < board[0].size(); col++)
		{
			copy[row].push_back(board[row][col]);
		}
	}
	return copy;
}
