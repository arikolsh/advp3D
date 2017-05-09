#include "SmartPlayer.h"
#include "IBattleshipGameAlgo.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <utility>

#define EMPTY_CELL '-'
#define MARKED_CELL 'x'
#define INVALID_ATTACK  { -1 , -1} 
#define START_POINT { 1 , 1 }
#define A_NUM 0
#define B_NUM 1

#define RUBBER_BOAT 'b'
#define ROCKET_SHIP 'p'
#define SUBMARINE 'm'
#define DESTROYER 'd'
#define NUM_SHIP_TYPES 5

bool print_mode = false; //debug purposes

IBattleshipGameAlgo* GetAlgorithm()
{
	return new SmartPlayer();			// Return last instance
}

//Constructor
SmartPlayer::SmartPlayer() : _playerNum(-1), _rows(0), _cols(0), _pos(START_POINT),
_finishedAttacks(false), _attack({ -1,-1 }), _attacking_state(Routine), _x_directionFailed(false), _y_directionFailed(false)
{
}

//Destructor
SmartPlayer::~SmartPlayer()
{
	//Empty for now
}

bool SmartPlayer::init(const std::string& path)
{
	return true;
}

void SmartPlayer::setBoard(int player, const char** board, int numRows, int numCols)
{
	_playerNum = player;
	_rows = numRows, _cols = numCols;
	copyBoard(board); // Copy all player's ships to _board
	markPotentialHits(); // Mark all cells that are a "potential hit" (might hold an opponent ship)
	if (print_mode)
	{
		cout << endl << "smart player: " << _playerNum << endl;
		printBoard(false);
	}
}

// Copy all player's ships to _board (which is initialized with EMPTY_CELLs)
void SmartPlayer::copyBoard(const char** board)
{
	char cell;
	//First, Create a board of size numRows+2 x numCols+2 filled with EMPTY_CELLs:
	auto empty_line = string(_cols + 2, EMPTY_CELL); //empty_line = "_____..._"
	for (int i = 0; i < _rows + 2; i++)
	{
		_board.push_back(empty_line);
	}

	// Copy all player's ships to _board
	for (int i = 0; i < _rows; i++)
	{
		for (int j = 0; j < _cols; j++)
		{
			cell = board[i][j];
			_board[i + 1][j + 1] = cell == ' ' ? EMPTY_CELL : cell;
		}
	}
}

/* _board is (_rows+2)x(_cols+2) so Loop only over non-padding cells.
* If a cell is isolated, it might hold an opponent ship
* so mark this cell as a potential hit 'x' */
void SmartPlayer::markPotentialHits()
{
	for (int i = 1; i < _rows + 1; i++)
	{
		for (int j = 1; j < _cols + 1; j++)
		{
			if (potentialHit(i, j))
			{
				_board[i][j] = MARKED_CELL;
			}
		}
	}
}

//check the cell itself and in addition check :down, up, left, right, upper left, upper right, down left, down right
bool SmartPlayer::potentialHit(int row, int col)
{
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (abs(i) == abs(j) && i != 0) { continue; } //Skip the diagonals
			if (_board[row + i][col + j] != EMPTY_CELL
				&& _board[row + i][col + j] != MARKED_CELL)
			{
				return false; //Found one of the own player's ship in the surroundings
			}
		}
	}
	return true;
}


/* Compute and return the next smart attack move according to the _attacking_state.
* If Finished attacking all potential points return {-1,-1}. */
pair<int, int> SmartPlayer::attack()
{
	if (_finishedAttacks) { return INVALID_ATTACK; }

	switch (_attacking_state)
	{
	case Routine:
		return attackRoutine();

	case Hunting_x:
		return attackHuntingX();

	case Hunting_y:
		return attackHuntingY();
	}

	return INVALID_ATTACK;
}

/* Find next 'x' marked cell to attack.
* If Finished attacking all potential points return {-1,-1}. */
pair<int, int> SmartPlayer::attackRoutine()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in Routine State" << endl;
	}
	int i = _pos.first, j = _pos.second;
	while (i <= _rows)
	{
		while (j <= _cols)
		{
			if (_board[i][j] == MARKED_CELL) // Got next attack point
			{
				_attack = { i, j };
				_board[i][j] = EMPTY_CELL; // Mark this point as attacked
				updatePosition(i, j);
				return _attack;
			}
			if (++j > _cols)
			{
				j = 1; // Skip to next line
				break;
			}
		}
		i++;
	}
	_finishedAttacks = true;
	return INVALID_ATTACK;
}

pair<int, int> SmartPlayer::attackHuntingX()
{
	//return attackRoutine();
	if (print_mode) {
		cout << "Player " << _playerNum << " is in Hunting_X State" << endl;
	}
	int i = get<0>(_lastAttack); // 'get' returns a refrence
	int j = get<1>(_lastAttack);
	if (_x_directionFailed || j >= _cols) // Failed attacking in X direction
	{
		if (!_y_directionFailed && i <= _rows)
		{
			return attackHuntingY(); // Switch to Y direction attack
		}
		return attackRoutine(); // Switch to Routin state attack
	}
	return{ i, j + 1 }; // Attack in X direction (next cell to the right)
}

pair<int, int> SmartPlayer::attackHuntingY()
{
	//return attackRoutine();
	if (print_mode) {
		cout << "Player " << _playerNum << " is in Hunting_Y State" << endl;
	}
	int i = get<0>(_lastAttack);
	int j = get<1>(_lastAttack);
	if (i >= _rows || _y_directionFailed)
	{
		return attackRoutine();
	}
	return{ i + 1, j };
}


bool SmartPlayer::shipBelongsToPlayer(char c, int player)
{
	char shipTypes[NUM_SHIP_TYPES] = { RUBBER_BOAT, ROCKET_SHIP, SUBMARINE, DESTROYER };

	if (player == A_NUM) // Convert types to upper case
	{
		for (int i = 0; i < NUM_SHIP_TYPES; i++)
		{
			shipTypes[i] = toupper(shipTypes[i]);
		}
	}

	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (c == shipTypes[i]) {
			return true;
		}
	}
	return false;
}


// Next position in matrix (Will update _finishedAttacks = true if needed)
void SmartPlayer::updatePosition(int i, int j)
{
	if (j < _cols)
	{
		_pos = { i, j + 1 }; // Next column
	}
	else if (i < _rows) // j == _cols
	{
		_pos = { i + 1, 1 }; // Next row
	}
	else // i == _rows && j == _cols --> Reaching here means player finished attacks
	{
		_finishedAttacks = true;
	}
}


void SmartPlayer::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{
	// If attacked point is not a ship that belongs to this player, mark cell as empty to avoid attacking it again
	if (shipBelongsToPlayer(_board[row][col], _playerNum) == false)
	{
		_board[row][col] = EMPTY_CELL; // Avoid attacking this point again
	}
	if (result == AttackResult::Sink)
	{
		emptySurroundingCells(row, col); // Avoid attacking surrounding cells
	}

	if (player == _playerNum) // This player attacked
	{
		switch (result)
		{
		case AttackResult::Miss:
			switch (_attacking_state)
			{
			case Routine: // Keep attacking in Routine state until getting a 'Hit'
				_x_directionFailed = false;
				_y_directionFailed = false;
				_attacking_state = Routine;
				return;
			case Hunting_x: // Failed to hit on X direction --> Try to sink on Y direction
				_x_directionFailed = true;
				_y_directionFailed = false;
				_lastAttack = { row, col - 1 , result , player }; //Take 1 off since previous attack in X direction failed
				_attacking_state = Hunting_y;
				return;
			case Hunting_y: // Failed to sink on both X and Y directions --> Back to Routine state
				_y_directionFailed = true;
				_attacking_state = Routine;
				return;
			}
			break;

		case AttackResult::Hit:
			switch (_attacking_state)
			{
			case Routine:
				_x_directionFailed = false;
				_y_directionFailed = false;
				_lastAttack = { row, col , result , player };
				_attacking_state = Hunting_x; // Try to attack first on X direction in order to sink this ship
				break;
			case Hunting_x: // Found ship direction to be X --> attack until sink
				_x_directionFailed = false;
				_y_directionFailed = true; // Ship isn't in Y direction
				_lastAttack = { row, col , result , player };
				//_attacking_state = Hunting_x;
				break;
			case Hunting_y: // Found ship direction to be Y --> attack until sink
				_x_directionFailed = true; // Ship isn't in X direction
				_y_directionFailed = false;
				_lastAttack = { row, col , result , player };
				//_attacking_state = Hunting_y;
				break;
			}
			break;

		case AttackResult::Sink:
			switch (_attacking_state)
			{
			case Routine: // Keep attacking in Routine state until next 'Hit'
				break;
			case Hunting_x: // Succeeded to sink ship on X direction --> return to Routine state
				_attacking_state = Routine;
				break;
			case Hunting_y: // Succeeded to sink ship on Y direction --> return to Routine state
				_attacking_state = Routine;
				break;
			}
			break;
		}
	}

	else { // Opponent attacked --> Only the 'Own Goal' case matters 
		   // (If opponent miss or hit one of this player's ship, got nothing to do)

		   // If opponent hit his own ship, it revealed a target that smart player will try to sink:
		if (result == AttackResult::Hit && isOpponentOwnGoal(row, col, player))
		{
			if (_attacking_state == Routine)
			{
				_x_directionFailed = false;
				_y_directionFailed = false;
				// Save this attack until player gets his turn to attack the adjacent cells
				_lastAttack = { row, col , result , player };
				_attacking_state = Hunting_x; // Try to sink ship on X direction (and then in Y direction)
				if (print_mode)
				{
					cout << "Opponent performed an 'Own Goal', smart player will try to sink this ship" << endl;
				}
				/* Remark:
				* If opponent hit his ship (own goal) we want to sink it
				* but we don’t know where it begins!!!
				* In this solution we start only from that point right or down (and not left or up)
				* We can think of a new function: attackAfterOwnGoal() that will first attack x and then y,
				* but will identify the direction and attack until sinking - this is risky and hard */
				return;
			}
			else  //_attacking_state = Hunting_x or Hunting_y
			{
				//Player is already busy hunting another ship
				//Haven't decided yet what to do in this case...
			}
		}
	}
}


bool SmartPlayer::isOpponentOwnGoal(int row, int col, int player) const
{
	return player != _playerNum &&
		(_board[row][col] == EMPTY_CELL ||
			_board[row][col] == MARKED_CELL);
}


void SmartPlayer::emptySurroundingCells(int row, int col)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			//Skip the center and diagonals, as well as ships that belong to this player
			if (abs(i) == abs(j) || shipBelongsToPlayer(_board[row][col], _playerNum))
			{
				continue;
			}
			_board[row + i][col + j] = EMPTY_CELL;
		}
	}
}


void SmartPlayer::printBoard(bool fullPrint) const
{
	int start = fullPrint ? 0 : 1;
	int last_row = fullPrint ? _rows + 2 : _rows + 1;
	int last_col = fullPrint ? _cols + 2 : _cols + 1;
	for (int i = start; i < last_row; i++) {
		for (int j = start; j < last_col; j++) {
			cout << _board[i][j];
		}
		cout << endl;
	}
	cout << endl;
}
