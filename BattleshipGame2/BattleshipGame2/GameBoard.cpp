#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include "BoardUtils.h"
#define PADDING 2
#define EMPTY_CELL '-'
GameBoard::GameBoard(vector<vector<string>> board)
{
	_rows = int(board.size()) - PADDING;
	_cols = int(board[0].size()) - PADDING;
	_depth = int(board[0][0].size()) - PADDING;
	// get a copy of given board 
	_board = BoardUtils::getBoardCopy(board);
}

char GameBoard::charAt(Coordinate c) const
{
	return _board[c.row][c.col][c.depth];
}

void GameBoard::print(bool includePadding) const
{
	BoardUtils::printBoard(_board, includePadding);
}
