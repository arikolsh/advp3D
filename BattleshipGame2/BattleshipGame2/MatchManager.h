#pragma once
#include "GameBoard.h"
#include "IBattleshipGameAlgo.h"
#include <map>
#include <memory>
#include "Ship.h"
#include <fstream>
#include "PlayerResult.h"

#define NUM_PLAYERS 2
#define INVALID_COORDINATE  { -1 , -1, -1 }

class MatchManager {
public:
	explicit MatchManager(GameBoard &gameBoard);
	~MatchManager();
	int getPlayerScore(int player) const;
	bool isPlayerDefeated(int player) const;
	void logShipsMap();
	/* Search for the attack point in shipsMap:
	* If attack point is not in map --> it's a Miss!
	* Else, hit the ship in map by taking one off the ship life counter
	* If ship life is 0 (already sank) consider a Miss.
	* If ship life has now become 0 --> it's a Sink!
	* Else, return Hit. */
	AttackResult executeAttack(int attackedPlayerNum, Coordinate attack);

	void buildPlayerBoards(const GameBoard board, GameBoard& board1, GameBoard& board2) const;

	/*Each player declares his next attack.
	* Then, his enemy executes the attack and returns the AttackResult.
	* If the player hits an enemy's ship, he gets another turn
	* If the player make an own goal, he doesn't get another turn
	* Game is over once a player loses all his ships, or all attackes were taken.
	* run game will run the flow of the game and eventually returns -1 if no winners or
	* the number of the winner player. */
	int runGame(IBattleshipGameAlgo * players[NUM_PLAYERS]);
	void gameOver(int winner);
	void gameOver(int winner, pair<int, int> playersPair, PlayerResult &resA, PlayerResult &resB) const;

private:
	MatchManager() = delete;
	MatchManager(const MatchManager& that) = delete;
	GameBoard _gameBoard;
	/*player data: first is A, second is B*/
	pair<int, int> _playerScores;
	pair<int, int> _playersNumActiveShips;
	/*who is the next player to attack*/
	int _currentPlayer;
	map<vector<int>, pair<shared_ptr<Ship>, bool>> _shipsMap;

	/* Each player board is prepared in advance and hidden from the opponent!
	* fillPlayerBoard gets the full board with both players ships,
	* and fills the given player's board with his ships only. */
	void fillMapWithShips();

	enum ShipDirection { VERTICAL = 0, HORIZONTAL = 1, DEPTH = 2 };

	ShipDirection findShipDirection(Coordinate c, char ship) const;

	void insertShipToMap(Coordinate c, char ship_char);

	/*return true if own goal*/
	static bool isOwnGoal(int attackedPlayerNum, char shipType);

	ofstream _logFile;
};
