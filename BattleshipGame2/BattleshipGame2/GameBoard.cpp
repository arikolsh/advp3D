#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include "BoardUtils.h"
#include <iostream>
#define PADDING 2
#define EMPTY_CELL '-'

vector<vector<string>> GameBoard::board() const
{
	return _board;
}

GameBoard::GameBoard(int rows, int cols, int depth)
{
	_rows = rows;
	_cols = cols;
	_depth = depth;
	_board = BoardUtils::getNewEmptyBoard(depth + PADDING, rows + PADDING, cols + PADDING);

}

// expect getting a board with padding
GameBoard::GameBoard(vector<vector<string>>& board, int rows, int cols, int depth)
{
	_depth = depth;
	_rows = rows;
	_cols = cols;
	// get a copy of given board 
	_board = board;
}

GameBoard::GameBoard(const GameBoard& that)
{
	_depth = that.depth();
	_rows = that.rows();
	_cols = that.cols();
	_board = that.board();
}

char GameBoard::charAt(Coordinate c) const
{
	return _board[c.row][c.col][c.depth];
}

void GameBoard::print(bool includePadding) const
{
	BoardUtils::printBoard(_board, includePadding);
}
