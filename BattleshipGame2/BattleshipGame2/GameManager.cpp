#include "GameManager.h"
#include <iostream>
#define NUM_SHIP_TYPES 4
#define NUM_PLAYERS 2
#define A_NUM 0
#define B_NUM 1
#define BOMB_COLOR 6 //brown
#define BOMB_SYMBOL '@'
#define HIT_COLOR 5 //magneta
#define HIT_SYMBOL '*'
#define EMPTY_CELL_COLOR 8 //gray
#define EMPTY_CELL '-'
#define NUM_SHIPS 5
#define SUCCESS 0
#define FAILURE -1
#define EMPTY_CELL '-'
#define HORIZONTAL 1
#define VERTICAL 0

bool printMode = false;

GameManager::GameManager(GameBoard* gameBoard, bool isQuiet, int delay)
{
	_playersNumActiveShips = { NUM_SHIPS, NUM_SHIPS };
	_playerScores = { 0, 0 };
	_currentPlayer = A_NUM; //player A starts the game
	_gameBoard = gameBoard;
	_isQuiet = isQuiet;
	_delay = delay;
	fillMapWithShips();
}

void GameManager::fillMapWithShips()
{
	int i, j, k, shipLen, direction;
	pair<int, int> xy;
	pair<shared_ptr<Ship>, bool> shipAndHit;
	char visited = 'x', cell;
	vector<string> boardCpy(_gameBoard->getFullBoard());
	int rows = _gameBoard->getRows(/* with padding */true);
	int cols = _gameBoard->getCols(/* with padding */true);

	//Iterate over the board and create the shipsMap:
	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++) {
			cell = boardCpy[i][j];
			if (cell == EMPTY_CELL || cell == visited) { continue; }
			boardCpy[i][j] = visited;
			//cell is a ship character!
			shared_ptr<Ship> ship(new Ship(cell)); //create the ship object
												   //Add to shipsMap all cells which belong to the current ship:
			shipLen = ship->getLife();
			direction = (boardCpy[i][j + 1] == cell) ? HORIZONTAL : VERTICAL;
			k = 0;
			while (k < shipLen)
			{
				if (direction == HORIZONTAL)
				{
					xy = { i, j + k };
					boardCpy[i][j + k] = visited;
				}
				else if (direction == VERTICAL)
				{
					xy = { i + k, j };
					boardCpy[i + k][j] = visited;

				}
				shipAndHit = { ship , false };
				_shipsMap.insert(make_pair(xy, shipAndHit));
				k++;
			}
		}
	}
}

int GameManager::getPlayerScore(int player) const
{
	return player == A_NUM ? _playerScores.first : _playerScores.second;
}

bool GameManager::isPlayerDefeated(int player) const
{
	return player == A_NUM ? _playersNumActiveShips.first <= 0 
	: _playersNumActiveShips.second <= 0;
}

int GameManager::getCurrentPlayer() const
{
	return _currentPlayer;
}

void GameManager::printShipsMap(map<pair<int, int>, pair<shared_ptr<Ship>, bool>>& shipsMap)
{
	pair<int, int> cell;
	int count = 1;
	for (auto iter = shipsMap.begin(); iter != shipsMap.end(); ++iter)
	{
		cell = iter->first;
		auto ship = iter->second.first;
		cout << "Map entry " << count << " is:";
		cout << "(" << cell.first << "," << cell.second << ")";
		cout << " shipType: " << ship->getType();
		cout << " shipLife: " << ship->getLife() << endl;

		count++;
	}
}

/* Search for the attack point in shipsMap:
* If attack point is not in map --> it's a Miss!
* Else, hit the ship in map by taking one off the ship life counter
* If ship life is 0 (already sank) consider a Miss.
* If ship life has now become 0 --> it's a Sink!
* Else, return Hit. */
AttackResult GameManager::executeAttack(int attackedPlayerNum, pair<int, int> attack)
{
	if (printMode)
	{
		cout << "player " << attackedPlayerNum << " is attacked on "
			<< attack.first << "," << attack.second << endl;
		cout << "Result: ";
	}

	if (!_isQuiet)
	{ //bomp the point
		_gameBoard->mark(attack.first, attack.second, BOMB_SYMBOL, BOMB_COLOR, _delay);
	}

	auto found = _shipsMap.find(attack);
	if (found == _shipsMap.end()) //attack point not in map --> Miss
	{
		if (printMode)
		{
			cout << "Miss" << endl;
		}
		_currentPlayer = attackedPlayerNum;
		if (!_isQuiet)
		{ //print the empty cell symbol
			_gameBoard->mark(attack.first, attack.second, EMPTY_CELL, EMPTY_CELL_COLOR, _delay);
		}
		return AttackResult::Miss;
	}
	auto ship = found->second.first; //attack point is in map --> get the ship
	auto shipWasHit = found->second.second;

	if (!_isQuiet)
	{ //from here in all cases print hit symbol
		_gameBoard->mark(attack.first, attack.second, HIT_SYMBOL, HIT_COLOR, _delay);
	}

	if (shipWasHit == true) //Not the first hit on this specific cell (i,j)
	{
		//pass turn to opponent
		_currentPlayer = attackedPlayerNum;
		if (ship->getLife() == 0) //ship already sank.. Miss
		{
			if (printMode)
			{
				cout << "Miss (hit a sunken ship)" << endl;
			}
			return AttackResult::Miss;
		}
		if (printMode)
		{
			cout << "Hit (ship was already hit before but still has'nt sunk..)" << endl;
		}
		return AttackResult::Hit; //you don't get another turn if cell was already hit
	}

	ship->hit(); //Hit the ship (Take one off the ship life)
	found->second.second = true; //Mark cell as a 'Hit'
	if (printMode)
	{
		cout << "Hit ship " << ship->getType() << endl;
	}
	int shipType = ship->getType();
	if (isOwnGoal(attackedPlayerNum, shipType))
	{
		if (printMode)
		{
			cout << "own goal! player " << _currentPlayer << " hit his own ship" << endl;
		}
		//in case of own goal pass turn to opponent
		_currentPlayer = attackedPlayerNum;
	}
	if (ship->getLife() == 0) //It's a Sink
	{
		if (shipType == toupper(shipType))
		{
			_playersNumActiveShips.first--; //Player A lost a ship
		}
		else
		{
			_playersNumActiveShips.second--; //Player B lost a ship
		}
		//update player points
		//playerB's ship was the one that got hit
		//add points to playerA
		if (shipType == tolower(shipType))
		{
			_playerScores.first += ship->getSinkPoints();
		}
		//playerA's ship was the one that got hit
		//add points to playerB
		else
		{
			_playerScores.second += ship->getSinkPoints();
		}
		if (printMode)
		{
			cout << "Sink! Score: " << ship->getSinkPoints() << endl;
		}
		return AttackResult::Sink;
	}
	return AttackResult::Hit; //Hit
}

bool GameManager::isOwnGoal(int attackedPlayerNum, char shipType)
{
	return attackedPlayerNum == A_NUM && shipType != toupper(shipType)
		|| attackedPlayerNum == B_NUM && shipType != tolower(shipType);
}

void GameManager::gameOver(int winner) const
{
	if (winner != -1) //We have a winner
	{
		cout << "Player " << (winner == A_NUM ? "A" : "B") << " won" << endl;
	}
	cout << "Points:" << endl;
	cout << "Player A: " << _playerScores.first << endl;
	cout << "Player B: " << _playerScores.second << endl;
}

int GameManager::runGame(IBattleshipGameAlgo* players[NUM_PLAYERS])
{
	if (!_isQuiet)
	{
		int drawDelay = 15;
		_gameBoard->draw(drawDelay);
	}
	if(printMode)
	{
		cout << "Ships Map at start point: " << endl;
		printShipsMap(_shipsMap);
	}

	int winner = -1;
	//finishedAttacks[i] is true iff players[i] finished all his attacks
	bool finishedAttacks[NUM_PLAYERS] = { false,false };
	pair<int, int> attackPoint;
	AttackResult attackResult;
	while (true)
	{
		//Player declares his next attack:
		attackPoint = players[_currentPlayer]->attack();
		if (attackPoint.first == -1)
		{
			finishedAttacks[_currentPlayer] = true;
			if (finishedAttacks[0] && finishedAttacks[1])
			{ //both players finished all their attacks
				break;
			}
			//switch to opponent
			_currentPlayer = 1 - _currentPlayer;
			continue;
		}
		

		attackResult = executeAttack(1 - _currentPlayer, attackPoint);
		players[A_NUM]->notifyOnAttackResult(_currentPlayer, attackPoint.first, attackPoint.second, attackResult);
		players[B_NUM]->notifyOnAttackResult(_currentPlayer, attackPoint.first, attackPoint.second, attackResult);
		//check for defeated players
		if (isPlayerDefeated(1 - _currentPlayer))
		{
			//current player sunk all opponent's ships
			winner = _currentPlayer; //winner is the current player
			break;
		}
		if (isPlayerDefeated(_currentPlayer))
		{
			//current player sunk his own last ship
			winner = 1 - _currentPlayer; //winner is the opponent
			break;
		}
	}
	if (printMode)
	{
		cout << "Ships Map at Finish point: " << endl;
		printShipsMap(_shipsMap);
	}

	return winner;
}


