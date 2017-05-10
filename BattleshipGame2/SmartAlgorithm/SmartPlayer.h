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
	* returns whether the init succeeded or failed */
	bool init(const std::string& path) override;

	// called once to notify player on his board
	void setBoard(int player, const char** board, int numRows, int numCols) override;

	// ask player for his move
	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;	// notify on last move result

	static void TEST_PLAYER();


private:

	enum AttackingState { Routine, Hunting_x, Hunting_y };
	AttackingState _attacking_state;
	tuple<int, int, AttackResult, int> _lastAttack; // { row, col, result , player };
	bool _finishedAttacks;
	// Private fields:
	int _playerNum;
	pair<int, int> _attack; //Next point to attack
	pair<int, int> _pos; //Points on player's board
	vector<string> _board; //Holds all player's ships + 'x' mark for every cell that shouldn't be attacked
	int _rows, _cols;
	bool _y_directionFailed, _x_directionFailed;

	// Private functions:
	void copyBoard(const char** board);
	void markPotentialHits();
	bool potentialHit(int row, int col); //check if the cell is empty and in addition check :down, up, left, right, upper left, upper right, down left, down right
	void updatePosition(int i, int j);
	bool isOpponentOwnGoal(int row, int col, int player) const;
	void emptySurroundingCells(int row, int col, int y_limit, int x_limit);
	pair<int, int> attackRoutine();
	pair<int, int> attackHuntingX();
	pair<int, int> attackHuntingY();
	static bool shipBelongsToPlayer(char c, int player);
	void printBoard(bool fullPrint) const;
};