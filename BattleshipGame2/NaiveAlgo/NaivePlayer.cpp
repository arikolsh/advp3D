#include "NaivePlayer.h"

#define EMPTY_CELL '-'

using namespace std;

NaivePlayer::NaivePlayer(): _playerNum(-1),_attackPosition(0),_rows(0),_cols(0)
{
}

void NaivePlayer::setBoard(int player, const char ** board, int numRows, int numCols)
{
	_rows = numRows;
	_cols = numCols;
	string emptyLine = string(_cols + 2, EMPTY_CELL);
	for (int i = 0; i < numRows + 2; i++) { // padding with 2 lines and colums for simplicity
		_board.push_back(emptyLine);
	}
	for (int i = 0; i < numRows; i++) {
		for (int j = 0; j < numCols; j++)
		{
			_board[i + 1][j + 1] = board[i][j];
		}
	}
}

bool NaivePlayer::isIsolated(int row, int col)
{
	//check the cell itself and in addition check :down, up, left, right, upper left, upper right, down left, down right
	return (_board[row][col] == EMPTY_CELL &&
		_board[row + 1][col] == EMPTY_CELL && // down
		_board[row - 1][col] == EMPTY_CELL && // up
		_board[row][col - 1] == EMPTY_CELL && // left
		_board[row][col + 1] == EMPTY_CELL);// right
}

bool NaivePlayer::init(const std::string & path)
{
	int i = 1, j = 1;
	while (i < _rows + 1) // go from (1,1) until (11, 11) inclusive for both
	{
		j = 1;
		while (j < _cols + 1)
		{
			if (isIsolated(i, j) == true)
			{
				_playerAttacks.push_back(pair<int, int>(i, j));
			}
			j++;
		}
		i++;
	}
	return true;
}

pair<int, int> NaivePlayer::attack()
{
	if (_attackPosition == _playerAttacks.size() - 1)
	{
		return pair<int, int>(-1, -1);
	}
	return _playerAttacks[_attackPosition++];
}

void NaivePlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	//Do nothing for now
}

NaivePlayer::~NaivePlayer()
{
	//Do nothing for now
}