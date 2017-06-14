#pragma once
#include "IBattleshipGameAlgo.h"
#include "SmartBoard.h"
#include "SmartLogger.h"
#include <vector>

#define INVALID_COORDINATE  { -1 , -1, -1 }
#define EMPTY_CELL '-'
#define MARKED_CELL 'x'
#define SURROUNDING_SHIP_CELL '0'

using namespace std;

class SmartPlayer : public IBattleshipGameAlgo
{
public:
	// Class constructor
	explicit SmartPlayer();

	// Class distructor
	~SmartPlayer();

	// called every time the player changes his order
	void setPlayer(int player) override;

	// called once at the beginning of each new game
	void setBoard(const BoardData& board) override;

	// ask player for his move
	Coordinate attack() override;

	// notify on last move result
	void notifyOnAttackResult(int player, Coordinate move, AttackResult result) override;

private:
	int _playerNum;
	SmartBoard _board;
	vector<Coordinate> _potentialAttacks; // Container for all points marked ('X') as potential hits
	Coordinate _attackPoint; // Next point to attack
	Coordinate _lastAttack; // Remember last attack (row, col, depth)
	Coordinate _firstHit; // Remember the position when starting to hunt a direction - 
	bool _cleanedFirstHit; //(helps when switching forwards to backwords + helps cleaning surrounding cells)

	SmartLogger _logger;
	void initLogger(bool shouldLog);
	void notifyOnAttackToLogger(int player, Coordinate move, AttackResult result, bool bothPlayers);

	enum AttackingState // Smart player attacks according to a DFA with these 3 states:
	{
		Routine,
		Hunting_x_forwards, Hunting_x_backwards,
		Hunting_y_forwards, Hunting_y_backwards,
		Hunting_z_forwards, Hunting_z_backwards
	};
	AttackingState _attackingState;
	const vector<AttackingState> _attackingDirections = { Hunting_x_forwards, Hunting_y_forwards, Hunting_z_forwards };

	/* ChooseHuntingState: randomly sets the next _attackingState from _attackingDirections - using
	* _validDirections vector to skip directions that were set to "false" (meaning we already failed while trying them) */
	void setRandomHuntingState();

	/* This vector serves ChooseHuntingState() to randomly pick the next hunting direction -
	* while skipping directions that were set to "false" (meaning we already failed while trying them) */
	vector<bool> _validDirections = { true, true, true }; // mark x_direction, y_direction, z_direction as valid
	void updateDirections(bool x, bool y, bool z);

	/* 1) When DFA is in Routine state, smart player attacks potential cells in random order
	*    Once succeeded to 'Hit', player randomly chooses the hunting direction.
	*    If needed (upon "Miss") player switches his hunting direction until succeeding to "Sink".
	*    After each 'Sink', player returns back to Routine state searching for the next 'Hit' */
	Coordinate attackRoutine();

	/* 2) When DFA is in Hunting_x_forwards state, smart player attacks in forward X direction
	*    until 'Sink' (return to Routine state) or 'Miss' (move to Hunting_x_backwards) */
	Coordinate attack_x_forwards();

	/* 3) When DFA is in Hunting_x_backwards state, smart player attacks in backwords X direction
	*    until 'Sink' or 'Miss' (go to ChooseHuntingState to randomly pick next hunting directions): */
	Coordinate attack_x_backwards();

	/* 4) When DFA is in Hunting_y_forwards state, smart player attacks in forward Y direction
	*    until 'Sink' (return to Routine state) or 'Miss' (move to Hunting_y_backwards) */
	Coordinate attack_y_forwards();

	/* 5) When DFA is in Hunting_y_backwards state, smart player attacks in backwords Y direction
	*    until 'Sink' or 'Miss' (go to ChooseHuntingState to randomly pick next hunting directions): */
	Coordinate attack_y_backwards();

	/* 6) When DFA is in Hunting_z_forwards state, smart player attacks in forward Z direction
	*    until 'Sink' (return to Routine state) or 'Miss' (move to Hunting_z_backwards) */
	Coordinate attack_z_forwards();

	/* 7) When DFA is in Hunting_z_backwards state, smart player attacks in backwords Z direction
	*    until 'Sink' or 'Miss' (go to ChooseHuntingState to randomly pick next hunting directions): */
	Coordinate attack_z_backwards();

	// Some more Private functions:
	bool isPotentialHit(Coordinate coordinate); // Check cell and it's surroundings to decide if it's a potential hit
	void getAllPotentialHits(); // Go over the player's board to find all Potential cells and mark them with 'X'
													  
	void clearSurroundings(Coordinate hit, AttackResult result); // Clean surrounding cells after Hit/Sink according to hunting direction
	void clearCellsInBoard(Coordinate hit, int r, int c, int d); // Do the cleaning in board
	void logAfterCleaning(Coordinate hit, int r, int c, int d);
};