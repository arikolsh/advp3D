#include <iostream>
#include "GameBoardTest.h"
#include "IBattleshipGameAlgo.h"


//NewGameBoard::NewGameBoard() {}

char NewGameBoard::charAt(Coordinate c) const
{
	return _board[c.row][c.col][c.depth];
}

//Create a padded board of size (rows+2 X cols+2 X depth+2) filled with EMPTY_CELLs:
void NewGameBoard::initialize()
{
	auto empty_line = std::string(_depth + 2, '-'); //empty_line = "   ... "
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

void NewGameBoard::print_3D_board(bool includePadding) const
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