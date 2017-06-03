#include "GameBoardOld.h"
#include <cstdio>
#include <locale>
#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <thread>
#define ALLOC_ERR printf("Error: Allocation error\n")
#define ADJ_SHIPS_ERR printf("Adjacent Ships on Board\n")
#define TOO_FEW_SHIPS_ERR(j) printf("Too few ships for player %c\n",j)
#define TOO_MANY_SHIPS_ERR(j) printf("Too many ships for player %c\n",j)
#define ILLEGAL_SHIP_ERR(s, j) printf("Wrong size or shape for ship %c for player %c\n",s,j)
#define WRONG_PATH_ERR(p) printf("Wrong path: %s\n",p)
#define MIN(a, b) ((a < b) ? (a) : (b))
#define SUCCESS 0
#define FAILURE -1
#define EMPTY_CELL '-'
#define HORIZONTAL 1
#define VERTICAL 0
#define SHIPS_FOR_PLAYER 5
#define RUB_LEN 1; //number of blocks for each ship
#define ROC_LEN 2;
#define SUB_LEN 3;
#define DES_LEN 4;
#define RUBBER_BOAT 'b'
#define ROCKET_SHIP 'p'
#define SUBMARINE 'm'
#define DESTROYER 'd'
#define NUM_SHIP_TYPES 4
#define A_NUM 0
#define B_NUM 1
#define BOARD_OFFSET 0 //offset of board gui
#define WHITE 15

/* class methods implementation */
GameBoardOld::GameBoardOld(int rows, int cols)
{
	//add padding
	_rows = rows + 2;
	_cols = cols + 2;
}

GameBoardOld::~GameBoardOld()
{
	//Remove all map elements while freeing the shared_ptr<Ship> ships:
	_shipsMap.clear();
}

int GameBoardOld::init(string path)
{
	int err;
	err = fillBoardFromFile(path);
	if (err) {
		return FAILURE;
	}
	return SUCCESS;
}

char** GameBoardOld::initBoard(int rows, int cols, char c)
{
	char** board = new char*[rows];
	for (int i = 0; i < rows; i++)
	{
		board[i] = new char[cols];
	}
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				board[i][j] = c;
			}
		}
	
	return board;
}

vector<string> GameBoardOld::getFullBoard() const
{
	return _fullBoard;
}

char ** GameBoardOld::getPlayerBoard(int player) const
{
	char c;
	bool condition;
	// cases: 
	// player is A => decider is false => will only insert upper case chars
	// player is B => decider is true => will only insert lower case chars 
	bool decider = player == A_NUM ? false : true;
	char** playerBoard = initBoard(_rows - 2, _cols - 2, ' ');
	if (playerBoard == nullptr)
	{
		return nullptr;
	}
	for (int i = 0; i < _rows - 2; i++)
	{
		for (int j = 0; j < _cols - 2; j++)
		{
			c = _fullBoard[i + 1][j + 1];
			if (c == EMPTY_CELL)
			{
				continue;
			}
			condition = tolower(c) == c; //condition true iff c is lower case
			if (condition == decider) { playerBoard[i][j] = c; }
		}
	}
	return playerBoard;
}


void GameBoardOld::printGameBoard()
{
	for (int i = 1; i < _rows - 1; i++) {
		for (int j = 1; j < _rows - 1; j++) {
			cout << _fullBoard[i][j];
		}
		cout << endl;
	}
}


int GameBoardOld::getRows(bool padding) const
{
	return padding ? _rows : _rows - 2;
}

int GameBoardOld::getCols(bool padding) const
{
	return padding ? _cols : _cols - 2;
}


void GameBoardOld::destroyBoard(char ** board, int rows)
{
	if (board == nullptr)
	{
		return;
	}
	for (int i = 0; i < rows; i++)
	{
		delete[] board[i];
	}
	delete[] board;
}

int GameBoardOld::fillBoardFromFile(string path)
{
	string line;
	int row = 1, err;
	int m;
	ifstream file(path);
	char **tmpBoard = initBoard(_rows, _cols, EMPTY_CELL);
	if (tmpBoard == nullptr) {
		return FAILURE;
	}
	if (!file.is_open()) {
		cout << "Error: failed to open file " << path << endl;
		destroyBoard(tmpBoard, _rows);
		return FAILURE;
	}
	while (getline(file, line) && row <= _rows) {
		m = MIN(_cols, (int)line.length());
		for (int i = 1; i <= m; i++) { //1,2,3,4,5,6,7,8,9,10
			if (Ship::isShip(line[i - 1])) { //check if valid ship char
				tmpBoard[row][i] = line[i - 1];
			}
		}
		row++;
	}
	// convert char** to vector<string>
	for (int i = 0; i < _rows; i++) { _fullBoard.push_back(tmpBoard[i]); }
	destroyBoard(tmpBoard, _rows);
	err = validateBoard();
	if (err) {
		return FAILURE;
	}
	return SUCCESS;
}

int GameBoardOld::validateBoard() {
	bool err;
	char visited = 'x';
	char illgeal = 'i';
	bool isAdjShips = false;
	char ship;
	int direction, k, shipLen;
	int shipCountA = 0, shipCountB = 0;
	vector<string> boardCpy(_fullBoard);
	for (int i = 0; i < _rows; i++) { //copy board content
		for (int j = 0; j < _cols; j++) {
			boardCpy[i][j] = _fullBoard[i][j];
		}
	}

	/*
	* mark invalid ships from board to avoid counting them in other errors
	* and if encountered illegal ships, print proper errors
	*/
	err = markInvalidShips(boardCpy);

	for (int i = 1; i < _rows - 1; i++) {
		for (int j = 1; j < _cols - 1; j++) {
			shipLen = 0;
			if (boardCpy[i][j] == EMPTY_CELL || boardCpy[i][j] == visited || boardCpy[i][j] == illgeal) { continue; }
			ship = boardCpy[i][j];
			boardCpy[i][j] = visited;
			shipLen++;
			direction = (boardCpy[i][j + 1] == ship) ? HORIZONTAL : VERTICAL;
			k = 1;

			if (direction == HORIZONTAL) {
				while (_fullBoard[i][j + k] == ship) {
					boardCpy[i][j + k] = visited;
					shipLen++;
					k++;
				}
			}
			else if (direction == VERTICAL) {
				while (_fullBoard[i + k][j] == ship) {
					boardCpy[i + k][j] = visited;
					shipLen++;
					k++;
				}
			}

			//check surroundings: checking no adjacent ships
			if (direction == HORIZONTAL) {
				for (int r = -1; r <= 1; r += 2) { //{-1,1}
					for (int c = 0; c < shipLen; c++) {
						if (boardCpy[i + r][j + c] != EMPTY_CELL) { //stumbled another ship
							isAdjShips = true;
						}
					}
				}
			}
			else if (direction == VERTICAL) {
				for (int r = 0; r < shipLen; r++) {
					for (int c = -1; c <= 1; c += 2) { //{-1,1}
						if (boardCpy[i + r][j + c] != EMPTY_CELL) { //stumbled another ship
							isAdjShips = true;
						}
					}
				}
			}

			//check which player
			if (ship == tolower(ship)) { //lowerCase = player B
				shipCountB++;
			}
			else { //upperCase = player A
				shipCountA++;
			}
		}
	}

	if (shipCountA > SHIPS_FOR_PLAYER) {
		TOO_MANY_SHIPS_ERR('A');
		err = true;
	}
	if (shipCountA < SHIPS_FOR_PLAYER) {
		TOO_FEW_SHIPS_ERR('A');
		err = true;
	}
	if (shipCountB > SHIPS_FOR_PLAYER) {
		TOO_MANY_SHIPS_ERR('B');
		err = true;
	}
	if (shipCountB < SHIPS_FOR_PLAYER) {
		TOO_FEW_SHIPS_ERR('B');
		err = true;
	}
	if (isAdjShips) {
		ADJ_SHIPS_ERR;
		err = true;
	}
	return err ? FAILURE : SUCCESS;
}

bool GameBoardOld::markInvalidShips(vector<string> boardCpy) {

	bool err = false;
	bool isInvalid = false; //stores if current ship is illegal in iteration
	char ship;
	int direction, k, shipLen;
	char shipTypes[NUM_SHIP_TYPES] = { RUBBER_BOAT, ROCKET_SHIP, SUBMARINE, DESTROYER };
	bool illegalShipsA[NUM_SHIP_TYPES] = { false }; //{RUBBER,ROCKET,SUBMARINE,DESTROYER}
	bool illegalShipsB[NUM_SHIP_TYPES] = { false };
	char visited = 'x'; //visited legal
	char illegal = 'i'; //illegal symbol

	for (int i = 1; i < _rows - 1; i++) {
		for (int j = 1; j < _cols - 1; j++) {
			shipLen = 0;
			if (boardCpy[i][j] == EMPTY_CELL || boardCpy[i][j] == visited || boardCpy[i][j] == illegal) { continue; }
			ship = boardCpy[i][j];
			boardCpy[i][j] = visited;
			shipLen++;
			direction = (boardCpy[i][j + 1] == ship) ? HORIZONTAL : VERTICAL;
			k = 1;
			if (direction == HORIZONTAL) {
				while (boardCpy[i][j + k] == ship) {
					boardCpy[i][j + k] = visited;
					shipLen++;
					k++;
				}

			}
			else if (direction == VERTICAL) {
				while (boardCpy[i + k][j] == ship) {
					boardCpy[i + k][j] = visited;
					shipLen++;
					k++;
				}
			}

			//check if ship size is valid (until now)
			int index = 0;
			switch (tolower(ship)) { //{RUBBER,ROCKET,SUBMARINE,DESTROYER}
			case RUBBER_BOAT:
				index = 0;
				isInvalid = shipLen != RUB_LEN;
				break;
			case ROCKET_SHIP:
				index = 1;
				isInvalid = shipLen != ROC_LEN;
				break;
			case SUBMARINE:
				index = 2;
				isInvalid = shipLen != SUB_LEN;
				break;
			case DESTROYER:
				index = 3;
				isInvalid = shipLen != DES_LEN;
				break;
			default:;
			}

			//check surroundings: checking valid shape
			if (direction == HORIZONTAL) {
				for (int r = -1; r <= 1; r += 2) { //{-1,1}
					for (int c = 0; c < shipLen; c++) {
						if (boardCpy[i + r][j + c] == ship) { //bad shape indeed
							boardCpy[i + r][j + c] = illegal; //mark illegal
							isInvalid = true;
						}
					}
				}
				if (isInvalid) { //current ship is illegal, mark entire shape
					for (int l = 0; l < shipLen; l++) {
						boardCpy[i][j + l] = illegal;
					}
				}
			}
			else if (direction == VERTICAL) {
				for (int r = -1; r < shipLen; r++) {
					for (int c = 0; c <= 1; c += 2) { //{-1,1}
						if (boardCpy[i + r][j + c] == ship) {
							boardCpy[i + r][j + c] = illegal;
							isInvalid = true;
						}
					}
				}

				if (isInvalid) { //mark entire shape
					for (int l = 0; l < shipLen; l++) {
						boardCpy[i + l][j] = illegal;
					}
				}
			}

			//check which player
			if (ship == tolower(ship)) { //lowerCase = player B
				illegalShipsB[index] |= isInvalid;
			}
			else { //upperCase = player A
				illegalShipsA[index] |= isInvalid;
			}
		}
	}

	//print possible errors:
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (illegalShipsA[i]) {
			ILLEGAL_SHIP_ERR(toupper(shipTypes[i]), 'A');
			err = true;
		}
	}

	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (illegalShipsB[i]) {
			ILLEGAL_SHIP_ERR(shipTypes[i], 'B');
			err = true;
		}
	}

	for (int i = 0; i < _rows; i++) { //remove visited symbol from legal ships, leave illegal symbols
		for (int j = 0; j < _cols; j++) {
			if (boardCpy[i][j] == visited)
			{
				boardCpy[i][j] = _fullBoard[i][j];
			}
		}
	}

	return err;
}