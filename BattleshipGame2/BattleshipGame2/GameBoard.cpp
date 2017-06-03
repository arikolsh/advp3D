#include <iostream>
#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include <sstream>
#define PADDING 2
#define EMPTY_CELL '-'


////// global functions //////
// expect getting a board with padding
GameBoard::GameBoard(vector<vector<string>> board, int rows, int cols, int depth)
{
	_depth = depth;
	_rows = rows;
	_cols = cols;
	// get a copy of given board 
	_board = getBoardCopy(board);
}

char GameBoard::charAt(Coordinate c) const
{
	return _board[c.depth][c.row][c.col];
}

void GameBoard::print(bool includePadding) const
{
	size_t start = includePadding ? 0 : 1;
	size_t depthEnd = includePadding ? _board.size() - 1 : _board.size() - PADDING;
	size_t rowsEnd = includePadding ? _board[0].size() - 1 : _board[0].size() - PADDING;
	size_t colsEnd = includePadding ? _board[0][0].size() - 1 : _board[0][0].size() - PADDING;
	for (size_t depth = start; depth <= depthEnd; depth++)
	{
		cout << endl;
		for (size_t row = start; row <= rowsEnd; row++)
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
