#include "MatchManager.h"
#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include "Ship.h"
#include <iostream>
#define NUM_SHIP_TYPES 4
#define NUM_PLAYERS 2
#define A_NUM 0
#define B_NUM 1
#define BOMB_COLOR_A 6 //brown
#define BOMB_COLOR_B 3 //cyan
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

bool printAttacks = false; //debug purposes
bool printMaps = false; //debug purposes

MatchManager::MatchManager(GameBoard gameBoard)
{
	_playersNumActiveShips = { NUM_SHIPS, NUM_SHIPS };
	_playerScores = { 0, 0 };
	_currentPlayer = A_NUM; //player A starts the game
	_gameBoard = GameBoard::getGameBoardCopy(gameBoard); // Copt the given board
	fillMapWithShips();
}

void MatchManager::fillMapWithShips()
{
	int i, j, k, m, shipLen, direction;
	int rows = _gameBoard.rows(), cols = _gameBoard.cols(), depth = _gameBoard.depth();
	Coordinate coordinate(INVALID_COORDINATE);
	Coordinate tmpCoordinate(INVALID_COORDINATE);
	pair<shared_ptr<Ship>, bool> shipAndHit;
	char visited = 'x';
	char cell;
	for (i = 1; i < rows + 1; i++)
	{
		for (j = 1; j < cols + 1; j++)
		{
			for (k = 1; k < depth + 1; k++)
			{
				coordinate = Coordinate(i, j, k);
				cell = _gameBoard.charAt(coordinate);
				if (!Ship::isShip(cell)) { continue; } // Skip non-ship cells
				_gameBoard.setAt(coordinate) = visited; // Mark coordinate as visited 'x'
				shared_ptr<Ship> ship = make_shared<Ship>(cell); //create the ship object
				//Find all cells which belong to the current ship and add them to _shipsMap:
				//shipLen = ship.getLife();
				//direction = (boardCpy[i][j + 1] == cell) ? HORIZONTAL : VERTICAL;
				m = 0;
				while (k < shipLen)
				{
					if (direction == HORIZONTAL)
					{
						//xy = { i, j + m };
						//boardCpy[i][j + m] = visited;
					}
					else if (direction == VERTICAL)
					{
						//xy = { i + m, j };
						//boardCpy[i + m][j] = visited;

					}
					shipAndHit = { ship , false };
					//_shipsMap.insert(make_pair(xy, shipAndHit));
					m++;
				}
			}
		}
	}
}

int MatchManager::getPlayerScore(int player) const
{
	return player == A_NUM ? _playerScores.first : _playerScores.second;
}

bool MatchManager::isPlayerDefeated(int player) const
{
	return player == A_NUM ? _playersNumActiveShips.first <= 0
		: _playersNumActiveShips.second <= 0;
}

void MatchManager::printShipsMap()
{
	int count = 1;
	for (auto iter = _shipsMap.begin(); iter != _shipsMap.end(); ++iter)
	{
		auto coordinate = iter->first;
		auto ship = iter->second.first;
		cout << "Map entry " << count << " is:";
		cout << "(" << coordinate[0] << "," << coordinate[1] << "," << coordinate[2] << ")";
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
AttackResult MatchManager::executeAttack(int attackedPlayerNum, Coordinate attack)
{
	if (printAttacks)
	{
		cout << "player " << 1 - attackedPlayerNum << " is attacking "
			<< attack.row << "," << attack.col << "," << attack.depth << endl;
		cout << "Result: ";
	}
	auto found = _shipsMap.find({ attack.row, attack.col, attack.depth });
	if (found == _shipsMap.end()) //attack point not in map --> Miss
	{
		if (printAttacks)
		{
			cout << "Miss" << endl;
		}
		_currentPlayer = attackedPlayerNum;
		return AttackResult::Miss;
	}
	auto ship = found->second.first; //attack point is in map --> get the ship
	auto shipWasHit = found->second.second;

	if (shipWasHit == true) //Not the first hit on this specific cell (i,j)
	{
		//pass turn to opponent
		_currentPlayer = attackedPlayerNum;
		if (ship->getLife() == 0) //ship already sank.. Miss
		{
			if (printAttacks)
			{
				cout << "Miss (hit a sunken ship)" << endl;
			}
			return AttackResult::Miss;
		}
		if (printAttacks)
		{
			cout << "Hit (ship was already hit before but still has'nt sunk..)" << endl;
		}
		return AttackResult::Hit; //you don't get another turn if cell was already hit
	}

	ship->hit(); //Hit the ship (Take one off the ship life)
	found->second.second = true; //Mark cell as a 'Hit'
	if (printAttacks)
	{
		cout << "Hit ship " << ship->getType() << endl;
	}
	int shipType = ship->getType();
	if (isOwnGoal(attackedPlayerNum, shipType))
	{
		if (printAttacks)
		{
			cout << "own goal! player " << 1 - attackedPlayerNum << " hit his own ship" << endl;
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
		if (printAttacks)
		{
			cout << "Sink! Score: " << ship->getSinkPoints() << endl;
		}
		return AttackResult::Sink;
	}
	return AttackResult::Hit; //Hit
}

bool MatchManager::isOwnGoal(int attackedPlayerNum, char shipType)
{
	return attackedPlayerNum == A_NUM && shipType != toupper(shipType)
		|| attackedPlayerNum == B_NUM && shipType != tolower(shipType);
}

void MatchManager::gameOver(int winner) const
{
	if (winner != -1) //We have a winner
	{
		cout << "Player " << (winner == A_NUM ? "A" : "B") << " won" << endl;
	}
	cout << "Points:" << endl;
	cout << "Player A: " << _playerScores.first << endl;
	cout << "Player B: " << _playerScores.second << endl;
}

int MatchManager::runGame(IBattleshipGameAlgo* players[NUM_PLAYERS])
{
	if (printMaps)
	{
		cout << "Ships Map at start point: " << endl;
		printShipsMap();
	}

	int winner = -1;
	//finishedAttacks[i] is true iff players[i] finished all his attacks
	bool finishedAttacks[NUM_PLAYERS] = { false,false };
	Coordinate attackPoint(INVALID_COORDINATE);
	AttackResult attackResult;
	int attacker;
	while (true)
	{
		attacker = _currentPlayer;
		//Player declares his next attack:
		attackPoint = players[_currentPlayer]->attack();
		if (attackPoint.row == -1)
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
		/*if (printAttacks)
		{
		cout << "notifyOnAttackResult: Player " << attacker << " attacked "
		<< attackPoint.first << "," << attackPoint.second << " Result is "
		<< static_cast<int>(attackResult) <<endl << endl;
		}*/
		players[A_NUM]->notifyOnAttackResult(attacker, attackPoint, attackResult);
		players[B_NUM]->notifyOnAttackResult(attacker, attackPoint, attackResult);
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
	if (printMaps)
	{
		cout << "Ships Map at Finish point: " << endl;
		printShipsMap();
	}
	return winner;
}


