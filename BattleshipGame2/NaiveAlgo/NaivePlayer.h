#pragma once

#include "IBattleshipGameAlgo.h"
#include <vector>

using namespace std;

class NaivePlayer : public IBattleshipGameAlgo
{
public:

	// Class constructor
	explicit NaivePlayer();

	// Class distructor
	~NaivePlayer();

	void NaivePlayer::setBoard(int player, const char** board, int numRows, int numCols) override;

	/* Called once to allow init from files if needed
	* returns whether the init succeeded or failed */
	bool init(const std::string& path) override;

	// ask player for his move
	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;	// notify on last move result

private:
	//fields
	int _playerNum;
	vector<string> _board;
	vector<pair <int, int>> _playerAttacks; //A vector with all player attacks
	int _attackPosition; //Next attack index in _playerAttacks vector
	int _rows;
	int _cols;
	//methods
	bool NaivePlayer::isIsolated(int row, int col); 
};