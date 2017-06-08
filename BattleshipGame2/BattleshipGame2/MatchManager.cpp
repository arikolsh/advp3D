#include "MatchManager.h"
#include "IBattleshipGameAlgo.h"
#include "GameBoard.h"
#include "Ship.h"
#include <iostream>
#include <mutex>
#include "GameManager.h"
#include <sstream>
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
#define EMPTY_CELL ' '
#define NUM_SHIPS 5
#define SUCCESS 0
#define FAILURE -1
#define EMPTY_CELL ' '
#define VISITED 'x'

bool debugMode = false;

MatchManager::MatchManager(GameBoard &gameBoard)
{
	_playersNumActiveShips = { NUM_SHIPS, NUM_SHIPS };
	_playerScores = { 0, 0 };
	_currentPlayer = A_NUM; //player A starts the game
	_gameBoard = GameBoard(gameBoard); // Copy the given board
	fillMapWithShips();
	_logger = Logger::getInstance();
}

MatchManager::~MatchManager() {}

void MatchManager::fillMapWithShips()
{
	char cell;
	for (int i = 1; i < _gameBoard.rows() + 1; i++)
	{
		for (int j = 1; j < _gameBoard.cols() + 1; j++)
		{
			for (int k = 1; k < _gameBoard.depth() + 1; k++)
			{
				Coordinate coordinate = Coordinate(i, j, k);
				cell = _gameBoard.charAt(coordinate);
				if (!Ship::isShip(cell)) { continue; } // Skip non-ship cells
				_gameBoard.setAt(coordinate) = VISITED; // Mark coordinate as visited
				insertShipToMap(coordinate, cell);
			}
		}
	}
}

MatchManager::ShipDirection MatchManager::findShipDirection(struct Coordinate c, char ship) const
{
	if (_gameBoard.charAt(Coordinate(c.row + 1, c.col, c.depth)) == ship)
	{
		return VERTICAL;
	}
	if (_gameBoard.charAt(Coordinate(c.row, c.col + 1, c.depth)) == ship)
	{
		return HORIZONTAL;
	}
	return DEPTH; // Default also for single-cell ships ('b' / 'B')
}

/* create the ship object and then find the ship orientation (VERTICAL / HORIZONTAL / Depth)
* and add all cells which belong to the current ship to _shipsMap */
void MatchManager::insertShipToMap(Coordinate c, char ship_char)
{
	int shipLen, direction;
	vector<int> insPoint = { -1, -1, -1 };
	Coordinate coor(INVALID_COORDINATE);
	pair<shared_ptr<Ship>, bool> shipAndHit;
	//create the ship object:
	shared_ptr<Ship> ship = make_shared<Ship>(ship_char);
	shipLen = ship->getLife();
	shipAndHit = { ship , false };
	//Find the ship orientation (VERTICAL / HORIZONTAL / Depth):
	direction = findShipDirection(c, ship_char);
	//Find all cells which belong to the current ship and add them to _shipsMap:
	for (int m = 0; m < shipLen; m++)
	{
		switch (direction)
		{
		case VERTICAL:
			coor = Coordinate(c.row + m, c.col, c.depth);
			break;
		case HORIZONTAL:
			coor = Coordinate(c.row, c.col + m, c.depth);
			break;
		case DEPTH:
			coor = Coordinate(c.row, c.col, c.depth + m);
			break;
		default: break;
		}
		_gameBoard.setAt(coor) = VISITED;
		_shipsMap[{coor.row, coor.col, coor.depth}] = shipAndHit;
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

void MatchManager::logShipsMap()
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
	auto found = _shipsMap.find({ attack.row, attack.col, attack.depth });
	if (found == _shipsMap.end()) //attack point not in map --> Miss
	{
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
			return AttackResult::Miss;
		}
		return AttackResult::Hit; //you don't get another turn if cell was already hit
	}

	ship->hit(); //Hit the ship (Take one off the ship life)
	found->second.second = true; //Mark cell as a 'Hit'
	int shipType = ship->getType();
	if (isOwnGoal(attackedPlayerNum, shipType))
	{
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
		return AttackResult::Sink;
	}
	return AttackResult::Hit; //Hit
}

bool MatchManager::isOwnGoal(int attackedPlayerNum, char shipType)
{
	return attackedPlayerNum == A_NUM && shipType != toupper(shipType)
		|| attackedPlayerNum == B_NUM && shipType != tolower(shipType);
}

void MatchManager::gameOver(int winner)
{
	if (winner != -1) //We have a winner
	{
		cout << "Player " << (winner == A_NUM ? "A" : "B") << " won" << endl;
	}
	cout << "Points:" << endl << "Player A: " << _playerScores.first << endl << "Player B: " << _playerScores.second << endl;
	//logShipsMap();
}

void MatchManager::gameOver(int winner, pair<int, int> playersPair, PlayerResult& resA, PlayerResult& resB) const
{
	// Updates results for players:
	resA._totalNumPointsFor += _playerScores.first;
	resA._totalNumPointsAgainst += _playerScores.second;
	resB._totalNumPointsFor += _playerScores.second;
	resB._totalNumPointsAgainst += _playerScores.first;
	if (winner == A_NUM)
	{
		resA._totalNumWins += 1;
		resB._totalNumLosses += 1;
	}
	else if (winner == B_NUM)
	{
		resA._totalNumLosses += 1;
		resB._totalNumWins += 1;
	}
	//else: Winner == -1 which means no winner..

	// Debug: log all info:
	ostringstream stream;
	stream << "\n#### Current Match ####\n";
	std::thread::id this_id = std::this_thread::get_id();
	stream << "Running in thread id: " << this_id << endl;
	stream << "Running match: " << "player " << playersPair.first << " against player " << playersPair.second << endl;
	if (winner != -1) //We have a winner
	{
		stream << "Player " << (winner == A_NUM ? playersPair.first : playersPair.second) << " won" << endl;
	}
	stream << "Points:" << endl << "Player " << playersPair.first << ": " << _playerScores.first << endl;
	stream << "Player " << playersPair.second << ": " << _playerScores.second << endl;
	// Player A total results:
	stream << "\nPlayer " << playersPair.first << " results so far:" << endl;
	stream << "Number of victories: " << resA._totalNumWins << endl;
	stream << "Total score for player (so far): " << resA._totalNumPointsFor << endl;
	stream << "Number of losses: " << resA._totalNumLosses << endl;
	stream << "Total score against (so far): " << resA._totalNumPointsAgainst << endl;
	// Player B total results:
	stream << "\nPlayer " << playersPair.second << " results so far:" << endl;
	stream << "Number of victories: " << resB._totalNumWins << endl;
	stream << "Total score for player (so far): " << resB._totalNumPointsFor << endl;
	stream << "Number of losses: " << resB._totalNumLosses << endl;
	stream << "Total score against (so far): " << resB._totalNumPointsAgainst << endl;
	_logger->log(stream.str(), "Debug");
}

int MatchManager::runGame(IBattleshipGameAlgo* players[NUM_PLAYERS])
{
	int winner = -1;
	//finishedAttacks[i] is true iff players[i] finished all his attacks
	bool finishedAttacks[NUM_PLAYERS] = { false,false };
	Coordinate attackPoint(INVALID_COORDINATE);
	AttackResult attackResult;
	int attacker;
	//logShipsMap();
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
	return winner;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void MatchManager::buildPlayerBoards(const GameBoard board, GameBoard& targetBoard1, GameBoard& targetBoard2) const
{
	int rows = board.rows();
	int cols = board.cols();
	int depth = board.depth();
	for (int i = 1; i <= rows; i++)
	{
		for (int j = 1; j <= cols; j++)
		{
			for (int k = 1; k <= depth; k++)
			{
				char cell = board.charAt(Coordinate(i, j, k));

				if (Ship::isShip(cell))
				{
					if (cell == toupper(cell))
					{
						targetBoard1.setAt(Coordinate(i, j, k)) = cell;
					}
					else
					{
						targetBoard2.setAt(Coordinate(i, j, k)) = cell;
					}
				}
			}
		}
	}
}
