#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include "BoardUtils.h"
#define PADDING 2
#define EMPTY_CELL '-'
// expect getting a board with padding
GameBoard::GameBoard(vector<vector<string>>& board, int rows, int cols, int depth)
{
	_depth = depth;
	_rows = rows;
	_cols = cols;
	// get a copy of given board 
	_board = board;
}

char GameBoard::charAt(Coordinate c) const
{
	return _board[c.row][c.col][c.depth];
}

void GameBoard::print(bool includePadding) const
{
	BoardUtils::printBoard(_board, includePadding);
}
