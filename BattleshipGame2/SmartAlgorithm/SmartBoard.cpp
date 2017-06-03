#include "SmartBoard.h"

#include <iostream>
#include "IBattleshipGameAlgo.h"

//Create a padded board of size (rows+2 X cols+2 X depth+2) filled with EMPTY_CELLs:
void SmartBoard::initialize()
{
	auto empty_line = std::string(_depth + 2, '-'); //empty_line = "   ... "
	vector<string> cols_vec;
	for (int j = 0; j < _cols + 2; j++)
	{
		cols_vec.push_back(empty_line);
	}
	for (int i = 0; i < _rows + 2; i++)
	{
		internalBoard.push_back(cols_vec);
	}
}

void SmartBoard::print_3D_board(bool includePadding) const
{
	int i, j, k, start, last_row, last_col, last_depth;
	start = includePadding ? 0 : 1;
	last_row = includePadding ? _rows + 2 : _rows + 1;
	last_col = includePadding ? _cols + 2 : _cols + 1;
	last_depth = includePadding ? _depth + 2 : _depth + 1;
	//Print row-col cuts for depth index start...last_depth:
	for (k = start; k < last_depth; k++) {
		cout << "SmartPlayer's board: row-col cut for depth " << k << endl;
		for (i = start; i < last_row; i++) {
			for (j = start; j < last_col; j++) {
				cout << internalBoard[i][j][k];
			}
			cout << endl;
		}
		cout << endl;
	}
	cout << endl;
}

// Copy all player's ships to _board (which is initialized with EMPTY_CELLs)
void SmartBoard::copyPlayerShips(const BoardData& board, int player)
{
	char cell;
	for (int i = 1; i < rows + 1; i++)
	{
		for (int j = 1; j < cols + 1; j++)
		{
			for (int k = 1; k < depth + 1; k++)
			{
				cell = board.charAt(Coordinate(i, j, k));
				if (!shipBelongsToPlayer(cell, player)) { continue; } // Skip non-ship cells
				internalBoard[i][j][k] = cell; // Copy valid ship char
			}
		}
	}
}

bool SmartBoard::shipBelongsToPlayer(char c, int player) const
{
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if ((player == A_NUM && c == shipTypesA[i]) || (player == B_NUM && c == shipTypesA[i]))
		{
			return true;
		}
	}
	return false;
}