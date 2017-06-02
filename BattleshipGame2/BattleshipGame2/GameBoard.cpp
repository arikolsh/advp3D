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

//////////////////// ofek funcs below ////////////////////

//Create a padded board of size (rows+2 X cols+2 X depth+2) filled with EMPTY_CELLs:
void GameBoard::initialize() //todo: dont need this
{
	auto empty_line = std::string(_depth + 2, '='); //empty_line = "   ... "
	vector<string> cols_vec;
	for (int j = 0; j < _cols + 2; j++)
	{
		cols_vec.push_back(empty_line);
	}
	for (int i = 0; i < _rows + 2; i++)
	{
		_board.push_back(cols_vec);
	}
}

void GameBoard::print_3D_board(bool includePadding) const
{
	int i, j, k, start, last_row, last_col, last_depth;
	start = includePadding ? 0 : 1;
	last_row = includePadding ? _rows + 2 : _rows + 1;
	last_col = includePadding ? _cols + 2 : _cols + 1;
	last_depth = includePadding ? _depth + 2 : _depth + 1;

	cout << "SmartPlayer's board: X-Y dimentions" << endl;
	for (i = start; i < last_row; i++) {
		for (j = start; j < last_col; j++) {
			cout << _board[i][j][0];
		}
		cout << endl;
	}

	cout << "SmartPlayer's board: X-Z dimentions" << endl;
	for (j = start; j < last_col; j++) {
		for (k = start; k < last_depth; k++) {
			cout << _board[0][j][k];
		}
		cout << endl;
	}

	cout << "SmartPlayer's board: Y-Z dimentions" << endl;
	for (i = start; i < last_row; i++) {
		for (k = start; k < last_depth; k++) {
			cout << _board[i][0][k];
		}
		cout << endl;
	}
	cout << endl;
}

/////// end of ofek funs ///////

