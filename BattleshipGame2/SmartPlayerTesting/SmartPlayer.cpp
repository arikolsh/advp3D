#include "SmartPlayer.h"
#include "IBattleshipGameAlgo.h"
#include <iostream>
#include <algorithm>
#include <random>

bool print_mode = false;

// Return a smart player instance
IBattleshipGameAlgo* GetAlgorithm() { return new SmartPlayer(); }

SmartPlayer::SmartPlayer() : _playerNum(-1), _attackPoint(INVALID_COORDINATE),
_lastAttack(INVALID_COORDINATE), _firstHit(INVALID_COORDINATE), _cleanedFirstHit(false), _attackingState(Routine) {}

SmartPlayer::~SmartPlayer() {}

void SmartPlayer::setPlayer(int player) { _playerNum = player; }

void SmartPlayer::setBoard(const BoardData& board)
{
	_board.SetDimentions(board.rows(), board.cols(), board.depth());
	_board.initialize(); // Initialize an empty board with paddings
	copyPlayerShips(board); // Copy all player's ships to the private _board
	getAllPotentialHits(); // Fills the _potentialAttacks vector with all points marked ('X') as potential hits
	updateDirections(true, true, true); // Initialize all directions as valid for attack
}

//Create a padded board of size (rows+2 X cols+2 X depth+2) filled with EMPTY_CELLs:
void SmartPlayer::SmartBoard::initialize()
{
	auto empty_line = string(depth + 2, EMPTY_CELL); //empty_line = "----...----"
	for (int i = 0; i < rows + 2; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			internalBoard[i].push_back(empty_line);
		}
	}
}

// Copy all player's ships to _board (which is initialized with EMPTY_CELLs)
void SmartPlayer::copyPlayerShips(const BoardData& board)
{
	char cell;
	for (int i = 1; i < _board.rows + 1; i++)
	{
		for (int j = 1; j < _board.cols + 1; j++)
		{
			for (int k = 1; k < _board.depth + 1; k++)
			{
				cell = board.charAt(Coordinate(i, j, k));
				if (!shipBelongsToPlayer(cell)) { continue; } // Skip non-ship cells
				_board.At(i, j, k) = cell; // Copy valid ship char
			}
		}
	}
}

/* Compute and return the next smart attack according to the _attacking_state (of our DFA).
* If Finished attacking all potential points, the Coordinate(-1,-1,-1) is returned. */
Coordinate SmartPlayer::attack()
{
	switch (_attackingState)
	{
	case Routine:
		return attackRoutine();

	case Hunting_x_forwards:
		return attack_x_forwards();

	case Hunting_x_backwards:
		return attack_x_backwards();

	case Hunting_y_forwards:
		return attack_y_forwards();

	case Hunting_y_backwards:
		return attack_y_backwards();

	case Hunting_z_forwards:
		return attack_z_forwards();

	case Hunting_z_backwards:
		return attack_z_backwards();
	}
	return attackRoutine();
}

void SmartPlayer::notifyOnAttackResult(int player, Coordinate move, AttackResult result)
{
	// Unmark the attacked cell (to avoid attacking it again):
	if (_board[move] == MARKED_CELL) { _board[move] = EMPTY_CELL; }

	if (player == _playerNum) // This player attacked
	{
		// Remember this move (used when hunting a ship):
		if (result == AttackResult::Hit) { _lastAttack = move; }
		if (result == AttackResult::Sink)
		{
			clearSurroundingsAfterSink(move); // On 'Sink' unmark all Surroundings cells
			_attackingState = Routine; // Good job, back to Routine until next 'Hit'
		}

		switch (_attackingState)
		{
		case Routine:
			switch (result)
			{
			case AttackResult::Hit:
				setRandomHuntingState(); // Randomly choose the hunting direction for next attack
				_firstHit = move; // Remember this move (for cleaning and in case we switch hunting direction)
				_cleanedFirstHit = false; // Remember to clear _firstHit surrounding cells (once we find the direction)
			case AttackResult::Miss: break; // Keep attacking in Routine state until next 'Hit'
			case AttackResult::Sink: break; // Sank a 'b' ship! Keep attacking in Routine state until next 'Hit'
			}
			break;

		case Hunting_x_forwards:
			switch (result)
			{
			case AttackResult::Miss:
				_attackingState = Hunting_x_backwards; // Try attacking in the opposite direction
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack opposite direction
				break;
			case AttackResult::Hit:
				updateDirections(true, false, false); // Found the direction to be X
													  //clear surrounding cells in y and z directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;

		case Hunting_x_backwards:
			switch (result)
			{
			case AttackResult::Miss:
				_validDirections[0] = false; // Mark X direction as a failure
				setRandomHuntingState(); // Randomly switch to a different hunting direction (Y or Z)
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack in next direction
				break;
			case AttackResult::Hit:
				updateDirections(true, false, false); // Found the direction to be X
													  //clear surrounding cells in y and z directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;

		case Hunting_y_forwards:
			switch (result)
			{
			case AttackResult::Miss:
				_attackingState = Hunting_y_backwards; // Try attacking in the opposite direction
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack opposite direction
				break;
			case AttackResult::Hit:
				updateDirections(false, true, false); // Found the direction to be Y
													  //clear surrounding cells in x and z directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;

		case Hunting_y_backwards:
			switch (result)
			{
			case AttackResult::Miss:
				_validDirections[1] = false; // Mark Y direction as a failure
				setRandomHuntingState(); // Randomly switch to a different hunting direction (X or Z)
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack in next direction
				break;
			case AttackResult::Hit:
				updateDirections(false, true, false); // Found the direction to be Y
													  //clear surrounding cells in x and z directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;

		case Hunting_z_forwards:
			switch (result)
			{
			case AttackResult::Miss:
				_attackingState = Hunting_z_backwards; // Try attacking in the opposite direction
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack opposite direction
				break;
			case AttackResult::Hit:
				updateDirections(false, false, true); // Found the direction to be Z
													  //clear surrounding cells in x and y directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;

		case Hunting_z_backwards:
			switch (result)
			{
			case AttackResult::Miss:
				_validDirections[2] = false; // Mark Z direction as a failure
				setRandomHuntingState(); // Randomly switch to a different hunting direction (X or Z)
				_lastAttack = _firstHit; // "Go back" to first 'Hit' position and attack in next direction
				break;
			case AttackResult::Hit:
				updateDirections(false, false, true); // Found the direction to be Z
													  //clear surrounding cells in x and y directions (including arround the _startPosition)
				break;
			case AttackResult::Sink: break; // Return to Routine_state until next 'Hit'
			}
			break;
		}
	}
	/* If opponent was the one to attack:
	* We care only about self hits (own goals),
	* But since we know the opponent is a smart player (won't hit himself),
	* it's not relevant for this exercise.. */
}

// Get a valid random attacking direction X/Y/Z
void SmartPlayer::setRandomHuntingState()
{
	_attackingState = Routine; // Default
	int rand_direction;
	random_device rd; // initialise the random seed
	mt19937 rng(rd()); // Mersenne-Twister random engine
	uniform_int_distribution<int> uni(0, 2);
	// Just in case player already failed attacking in all directions:
	if (!_validDirections[0] && !_validDirections[1] && !_validDirections[2]) { return; }
	while (true) // Iterate until getting a random attacking direction X/Y/Z that is valid (hasn't fail yet)
	{
		rand_direction = uni(rng); // rand_direction = 0, 1 or 2 (x, y or z) with "equal probability"
		if (_validDirections[rand_direction])
		{
			_attackingState = _attackingDirections[rand_direction];
			break; // Found a valid random attacking direction
		}
	}
}

Coordinate SmartPlayer::attackRoutine()
{
	auto foundAttack = false;
	while (!_potentialAttacks.empty()) // Still got more potential attacks
	{
		_attackPoint = _potentialAttacks.back();
		_potentialAttacks.pop_back(); // Delete attack so we won't attack same spot twice 
		if (_board[_attackPoint] == MARKED_CELL) // Make sure this cell is still marked for attack
		{
			foundAttack = true;
			break;
		}
	}
	updateDirections(true, true, true); // Mark all directions as valid for hunting in case of 'Hit'
	return foundAttack ? _attackPoint : Coordinate(INVALID_COORDINATE);
}

Coordinate SmartPlayer::attack_x_forwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_x_forwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (col >= _board.cols) // Can't attack forwards
	{
		return attack_x_backwards(); // Try attacking backwards
	}
	return Coordinate(row, col + 1, depth); // Attack in forwards X direction (next cell to the right)
}

Coordinate SmartPlayer::attack_x_backwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_x_backwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (col <= 1) // Can't attack backwards
	{
		_validDirections[0] = false; // Mark direction as invalid for attack
		setRandomHuntingState(); // randomly switch to a different direction
		return attack(); // Start over (to get next attack move)
	}
	return Coordinate(row, col - 1, depth); // Attack in backwards X direction (next cell to the left)
}

Coordinate SmartPlayer::attack_y_forwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_y_forwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (row >= _board.rows) // Can't attack forwards
	{
		return attack_y_backwards(); // Try attacking backwards
	}
	return Coordinate(row + 1, col, depth); // Attack in forwards Y direction
}

Coordinate SmartPlayer::attack_y_backwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_y_backwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (row <= 1) // Can't attack backwards
	{
		_validDirections[1] = false; // Mark direction as invalid for attack
		setRandomHuntingState(); // randomly switch to a different direction
		return attack(); // Start over (to get next attack move)	
	}
	return Coordinate(row - 1, col, depth); // Attack in backwards Y direction
}

Coordinate SmartPlayer::attack_z_forwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_z_forwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (depth >= _board.depth) // Can't attack forwards
	{
		return attack_z_backwards(); // Try attacking backwards
	}
	return Coordinate(row, col, depth + 1); // Attack in forwards Z direction
}

Coordinate SmartPlayer::attack_z_backwards()
{
	if (print_mode) {
		cout << "Player " << _playerNum << " is in attack_z_backwards" << endl;
	}
	auto row = _lastAttack.row, col = _lastAttack.col, depth = _lastAttack.depth;
	if (depth <= 1) // Can't attack backwards
	{
		_validDirections[2] = false; // Mark direction as invalid for attack
		setRandomHuntingState(); // randomly switch to a different direction
		return attack(); // Start over (to get next attack move)
	}
	return Coordinate(row, col, depth - 1); // Attack in backwards X direction (next cell to the left)
}

void SmartPlayer::getAllPotentialHits()
{
	auto engine = default_random_engine{};
	Coordinate coordinate(INVALID_COORDINATE);
	for (int i = 1; i < _board.rows + 1; i++)
	{
		for (int j = 1; j < _board.cols + 1; j++)
		{
			for (int k = 1; k < _board.depth + 1; k++)
			{
				coordinate.row = i, coordinate.col = j, coordinate.depth = k;
				if (isPotentialHit(coordinate))
				{
					_board[coordinate] = MARKED_CELL;
					_potentialAttacks.push_back(coordinate);
				}
			}
		}
	}
	// Shuffle _potentialAttacks vector so we generate random attacks in Routine_state:
	shuffle(begin(_potentialAttacks), end(_potentialAttacks), engine);
}

// Return true if the given coordinate and it's neighbors (down, up, left, right) are EMPTY_CELLs
// Meaning none of the player's ships were found in the coordinate's surroundings
bool SmartPlayer::isPotentialHit(Coordinate coordinate)
{
	auto row = coordinate.row, col = coordinate.col, depth = coordinate.depth;
	return  (_board.At(row, col, depth) == EMPTY_CELL || _board.At(row, col, depth) == MARKED_CELL) &&
		(_board.At(row + 1, col, depth) == EMPTY_CELL || _board.At(row + 1, col, depth) == MARKED_CELL) &&
		(_board.At(row - 1, col, depth) == EMPTY_CELL || _board.At(row - 1, col, depth) == MARKED_CELL) &&
		(_board.At(row, col + 1, depth) == EMPTY_CELL || _board.At(row, col + 1, depth) == MARKED_CELL) &&
		(_board.At(row, col - 1, depth) == EMPTY_CELL || _board.At(row, col - 1, depth) == MARKED_CELL) &&
		(_board.At(row, col, depth + 1) == EMPTY_CELL || _board.At(row, col, depth + 1) == MARKED_CELL) &&
		(_board.At(row, col, depth - 1) == EMPTY_CELL || _board.At(row, col, depth - 1) == MARKED_CELL);
}

void SmartPlayer::updateDirections(bool x, bool y, bool z)
{
	_validDirections[0] = x;
	_validDirections[1] = y;
	_validDirections[2] = z;
}

bool SmartPlayer::shipBelongsToPlayer(char c) const
{
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if ((_playerNum == A_NUM && c == shipTypesA[i]) || (_playerNum == B_NUM && c == shipTypesA[i]))
		{
			return true;
		}
	}
	return false;
}

void SmartPlayer::clearSurroundingsAfterSink(Coordinate sink)
{
	auto row = sink.row, col = sink.col, depth = sink.depth;
	if (_board.At(row + 1, col, depth) == MARKED_CELL)
		_board.At(row + 1, col, depth) = EMPTY_CELL;
	if (_board.At(row - 1, col, depth) == MARKED_CELL)
		_board.At(row - 1, col, depth) = EMPTY_CELL;
	if (_board.At(row, col + 1, depth) == MARKED_CELL)
		_board.At(row, col + 1, depth) = EMPTY_CELL;
	if (_board.At(row, col - 1, depth) == MARKED_CELL)
		_board.At(row, col - 1, depth) = EMPTY_CELL;
	if (_board.At(row, col, depth + 1) == MARKED_CELL)
		_board.At(row, col, depth + 1) = EMPTY_CELL;
	if (_board.At(row, col, depth - 1) == MARKED_CELL)
		_board.At(row, col, depth - 1) = EMPTY_CELL;
}

/* Mark the Surrounding cells arround the 'Hit' Coordinate as EMPTY_CELLs
* according to the _attackingState.
* First 'Hit' was in Routine_state when the direction was unknown - now it's time to clean arround it: */
void SmartPlayer::clearSurroundingsAfterHit(Coordinate hit)
{
	// 1) Hunting X direction: clear Surrounding cells in y and z directions:
	if (_attackingState == Hunting_x_forwards || _attackingState == Hunting_x_backwards)
	{
		if (!_cleanedFirstHit)
		{
			clearSurroundingsAfterHit_X(_firstHit);
			_cleanedFirstHit = true; // Mark cell as cleaned so we won't clean more than once..
		}
		clearSurroundingsAfterHit_X(hit);
	}
	// 2) Hunting Y direction: clear Surrounding cells in y and z directions:
	else if (_attackingState == Hunting_y_forwards || _attackingState == Hunting_y_backwards)
	{
		if (!_cleanedFirstHit)
		{
			clearSurroundingsAfterHit_Y(_firstHit);
			_cleanedFirstHit = true; // Mark cell as cleaned so we won't clean more than once..
		}
		clearSurroundingsAfterHit_Y(hit);
	}
	// 3) Hunting Z direction: clear Surrounding cells in y and z directions
	else if (_attackingState == Hunting_z_forwards || _attackingState == Hunting_z_backwards)
	{
		if (!_cleanedFirstHit)
		{
			clearSurroundingsAfterHit_Z(_firstHit);
			_cleanedFirstHit = true; // Mark cell as cleaned so we won't clean more than once..
		}
		clearSurroundingsAfterHit_Z(hit);
	}
}

void SmartPlayer::clearSurroundingsAfterHit_X(Coordinate hit)
{
	if (print_mode) { cout << "Hunting X direction: clear Surrounding cells in y and z directions" << endl; }
	_board.At(hit.row + 1, hit.col, hit.depth) = EMPTY_CELL;
	_board.At(hit.row - 1, hit.col, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col + 1, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col - 1, hit.depth) = EMPTY_CELL;
}

void SmartPlayer::clearSurroundingsAfterHit_Y(Coordinate hit)
{
	if (print_mode) { cout << "Hunting Y direction: clear Surrounding cells in x and z directions" << endl; }
	_board.At(hit.row, hit.col + 1, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col - 1, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col, hit.depth + 1) = EMPTY_CELL;
	_board.At(hit.row, hit.col, hit.depth - 1) = EMPTY_CELL;
}

void SmartPlayer::clearSurroundingsAfterHit_Z(Coordinate hit)
{
	if (print_mode) { cout << "Hunting Z direction: clear Surrounding cells in x and y directions" << endl; }
	_board.At(hit.row + 1, hit.col, hit.depth) = EMPTY_CELL;
	_board.At(hit.row - 1, hit.col, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col + 1, hit.depth) = EMPTY_CELL;
	_board.At(hit.row, hit.col - 1, hit.depth) = EMPTY_CELL;
}

void SmartPlayer::print_3D_board(bool includePadding) const
{
	int i, j, k, start, last_row, last_col, last_depth;
	start = includePadding ? 0 : 1;
	last_row = includePadding ? _board.rows + 2 : _board.rows + 1;
	last_col = includePadding ? _board.cols + 2 : _board.cols + 1;
	last_depth = includePadding ? _board.depth + 2 : _board.depth + 1;

	cout << "SmartPlayer's board: X-Y dimentions" << endl;
	for (i = start; i < last_row; i++) {
		for (j = start; j < last_col; j++) {
			cout << _board.At(i, j, 0);
		}
		cout << endl;
	}

	cout << "SmartPlayer's board: X-Z dimentions" << endl;
	for (j = start; j < last_col; j++) {
		for (k = start; k < last_depth; k++) {
			cout << _board.At(0, j, k);
		}
		cout << endl;
	}

	cout << "SmartPlayer's board: Y-Z dimentions" << endl;
	for (i = start; i < last_row; i++) {
		for (k = start; k < last_depth; k++) {
			cout << _board.At(i, 0, k);
		}
		cout << endl;
	}
	cout << endl;
}