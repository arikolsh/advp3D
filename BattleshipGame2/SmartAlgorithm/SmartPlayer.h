#pragma once

#include "IBattleshipGameAlgo.h"
#include <vector>

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

	// Private fields:
	int _playerNum;
	pair<int, int> _attack; //Next point to attack
	pair<int, int> _pos; //Points on player's board
	vector<string> _board; //Holds all player's ships + 'x' mark for every cell that shouldn't be attacked
	int _rows, _cols;

	// Private functions:
	void copyBoard(const char** board);
	void SmartPlayer::markPotentialHits();
	bool potentialHit(int row, int col); //check if the cell is empty and in addition check :down, up, left, right, upper left, upper right, down left, down right
	void SmartPlayer::updatePosition(int i, int j);
	void SmartPlayer::printBoard(bool fullPrint) const;
};