#include <iostream>
#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include <sstream>
#define PADDING 2
#define EMPTY_CELL '-'


////// global functions //////
//split string to vector using a delimiter
void split(string line, vector<string>& splitLine, char delimiter);
//////

GameBoard::GameBoard()
{
}

GameBoard::GameBoard(int rows, int cols, int depth)
{
	// dimensions without padding 
	_rows = rows;
	_cols = cols;
	_depth = depth;
	// get a 3d board with padding 
	_board = getNewEmptyBoard(_rows + PADDING, _cols + PADDING, _depth + PADDING);
}

GameBoard::GameBoard(vector<vector<string>> board)
{
	_depth = board.size() - PADDING;
	_rows = board[0].size() - PADDING;
	_cols = board[0][0].size() - PADDING;
	// get a copy of given board 
	_board = getBoardCopy(board);
}

char GameBoard::charAt(Coordinate c) const
{
	return _board[c.row][c.col][c.depth];
}

void GameBoard::print(bool includePadding) const
{
	int start = includePadding ? 0 : 1;
	int depthEnd = includePadding ? _board.size() - 1 : _board.size() - PADDING;
	int rowsEnd = includePadding ? _board[0].size() - 1 : _board[0].size() - PADDING;
	int colsEnd = includePadding ? _board[0][0].size() - 1 : _board[0][0].size() - PADDING;
	for (int depth = start; depth <= depthEnd; depth++)
	{
		cout << endl;
		for (int row = start; row <= rowsEnd; row++)
		{
			cout << _board[depth][row].substr(start, colsEnd) << endl;
		}
	}
}

vector<vector<string>> GameBoard::getNewEmptyBoard(int rows, int cols, int depth)
{
	vector<vector<string>> board3d(depth);
	//construct string of empty cells
	auto emptyCellStr = string(cols, EMPTY_CELL); //empty_line = "   ... "
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

vector<vector<string>> GameBoard::getBoardCopy(const vector<vector<string>> board)
{
	vector<vector<string>> copy(board.size());
	for (int d = 0; d < board.size(); d++)
	{
		copy[d] = vector<string>();
		for (int row = 0; row < board[0].size(); row++)
		{
			copy[d].push_back(board[d][row]);
		}
	}
	return copy;
}
