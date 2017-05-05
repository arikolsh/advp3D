#pragma once
#include "GameBoard.h"
#define NUM_PLAYERS 2

class GameManager{
public:
	explicit GameManager(GameBoard* gameBoard, bool isQuiet, int delay);
	int getPlayerScore(int player) const;
	bool isPlayerDefeated(int player) const;
	int getCurrentPlayer() const;
	static void printShipsMap(map<pair<int, int>, pair<shared_ptr<Ship>, bool>>& shipsMap);
	/* Search for the attack point in shipsMap:
	* If attack point is not in map --> it's a Miss!
	* Else, hit the ship in map by taking one off the ship life counter
	* If ship life is 0 (already sank) consider a Miss.
	* If ship life has now become 0 --> it's a Sink!
	* Else, return Hit. */
	AttackResult executeAttack(int attackedPlayerNum, pair<int, int> attack);
	void gameOver(int winner) const;
	/*
	* Each player declares his next attack.
	* Then, his enemy executes the attack and returns the AttackResult.
	* If the player hits an enemy's ship, he gets another turn
	* If the player make an own goal, he doesn't get another turn
	* Game is over once a player loses all his ships, or all attackes were taken.
	* run game will run the flow of the game and eventually returns -1 if no winners or
	* the number of the winner player.
	*/
	int runGame(IBattleshipGameAlgo * players[NUM_PLAYERS]);
private: 
	map<pair<int, int>, pair<shared_ptr<Ship>, bool>> _shipsMap;
	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	GameBoard* _gameBoard;
	/*player data: first is A, second is B*/
	pair<int, int> _playerScores;
	pair<int, int> _playersNumActiveShips;
	/*who is the next player to attack*/
	int _currentPlayer;
	/*true iff game is in quiet mode i.e. no gui*/
	bool _isQuiet;
	/*delay in milliseconds with each gui operation*/
	int _delay;
	/* Each player board is prepared in advance and hidden from the opponent!
	* fillPlayerBoard gets the full board with both players ships,
	* and fills the given player's board with his ships only. */
	void fillMapWithShips();
	/*return true if own goal*/
	static bool isOwnGoal(int attackedPlayerNum, char shipType);
};
