#pragma once

#include "IBattleshipGameAlgo.h"
#include <vector>
#include <tuple>

using namespace std;

class SmartPlayer : public IBattleshipGameAlgo
{
public:

	// Class constructor
	explicit SmartPlayer();

	// Class distructor
	~SmartPlayer();

	/* Called once to allow init from files if needed
	* returns whether the init succeeded or failed
	* Empty implementation for smart player (returns true) */
	bool init(const std::string& path) override;

	// called once to notify player on his board
	void setBoard(int player, const char** board, int numRows, int numCols) override;

	// ask player for his move
	std::pair<int, int> attack() override;

	// notify on last move result
	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;


private:

	// Smart player attacks according to a DFA with these 3 states:
	enum AttackingState { Routine, Hunting_x, Hunting_y };
	AttackingState _attacking_state;

	/* 1) When DFA is in Routine state, smart player attacks in predefined order
	*    according to his position in the matrix that represents the board
	*    with all potential hit cells marked with 'X'.
	*    Once succeeded to 'Hit', player moves to Hunting_X state in order to sink the ship,
	*    and if that doesn't work, switches to Hunting_Y state.
	*    After each 'Sink', player returns back to Routine state searching for the next 'Hit' */
	pair<int, int> attackRoutine();

	/* 2) When DFA is in Hunting_X state, smart player attacks in X direction
	*    until 'Sink' (return to Routine state) or 'Miss' (move to Y direction): */
	pair<int, int> attackHuntingX();

	/* 3) When DFA is in Hunting_Y state, smart player attacks in Y direction
	*    until 'Sink' or 'Miss' (return to Routine state): */
	pair<int, int> attackHuntingY();

	// First attack in x direction and if it fails attack y direction and after that return to routine state
	bool _y_directionFailed, _x_directionFailed;

	tuple<int, int, AttackResult, int> _lastAttack; // { row, col, result , player };
	bool _finishedAttacks;
	int _playerNum;
	pair<int, int> _attack; // Next point to attack
	pair<int, int> _pos; // Points on player's board
	vector<string> _board; // Holds all player's ships + 'x' mark for every cell that shouldn't be attacked
	int _rows, _cols; // The given board dimensions

					  // Private functions:
	void copyBoard(const char** board);
	void markPotentialHits(); // Go over the player's board (matrix) and mark all Potential cells with 'X'
	bool potentialHit(int row, int col); // Check cell and it's surroundings to decide if it's a potential hit
	void updatePosition(int i, int j); // update position in the attacking matrix for routine state attacks
	bool isOpponentOwnGoal(int row, int col, int player) const; // return true if opponent just hit his own ship
	void emptySurroundingCells(int row, int col, int y_start, int y_limit, int x_start, int x_limit); // Clean surrounding cells after attacks
	static bool shipBelongsToPlayer(char c, int player); // return true iff c is a ship that belongs to given player
	void printBoard(bool fullPrint) const;
};