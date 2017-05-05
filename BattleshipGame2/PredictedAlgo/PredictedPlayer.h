#pragma once

#include "IBattleshipGameAlgo.h"
#include <vector>


using namespace std;

class PredictedPlayer : public IBattleshipGameAlgo
{
public:

	// Class constructor
	explicit PredictedPlayer();

	// Class distructor
	~PredictedPlayer();

	/* Called once to allow init from files if needed
	* returns whether the init succeeded or failed */
	bool init(const std::string& path) override;

	// called once to notify player on his board
	virtual void setBoard(int player, const char** board, int numRows, int numCols) override;

	// ask player for his move
	std::pair<int, int> attack() override;

	void notifyOnAttackResult(int player, int row, int col, AttackResult result) override;	// notify on last move result

private:
	
	// Private fields:
	int _playerNum;
	string _attackFilePath; //The input filePath with all attacks to be executed
	vector<pair <int, int>> _playerAttacks; //A vector with all player attacks
	int _attackPosition; //Next attack index in _playerAttacks vector
	
	// Private functions:
	void getAttacksFromFile();
	static bool isValidAttack(pair<int, int> attack);
	int PredictedPlayer::fetchInputFiles(vector<string> & attackFiles, vector<string> & messages, const string path) const;
	static int PredictedPlayer::execCmd(char const * cmd, string & shellRes);
	static void PredictedPlayer::showMessages(vector<string> messages);
	int PredictedPlayer::getAttackFiles(vector<string> & attackFiles, string searchDir) const;
	void SetAttackFilePath(int playerNum, const string& path);
	pair<int, int> getAttackPair(string& line) const;
};