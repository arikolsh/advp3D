#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

#define INVALID_COORDINATE  { -1 , -1, -1 }
#define EMPTY_CELL ' '
#define MARKED_CELL 'x'
#define A_NUM 0
#define B_NUM 1
#define  NUM_SHIP_TYPES 4

class SmartBoard : public BoardData
{
public:
	explicit SmartBoard() {};
	~SmartBoard() = default;

	void SetDimentions(int rows, int cols, int depth) { _rows = rows, _cols = cols, _depth = depth; }

	void initialize();

	void print_3D_board(bool includePadding, int player) const;

	//returns only selected players' chars
	char charAt(Coordinate c) const override { return internalBoard[c.row][c.col][c.depth]; }

	void copyPlayerShips(const BoardData& board, int player); // Copy all player's ships to _board (which is initialized with EMPTY_CELLs)

	bool shipBelongsToPlayer(char c, int player) const; // return true iff c is a ship that belongs to given player
	const char shipTypesA[NUM_SHIP_TYPES] = { 'B', 'P', 'M', 'D' };
	const char shipTypesB[NUM_SHIP_TYPES] = { 'b', 'p', 'm', 'd' };

	// Overloading operators for safely accessing _board
	char& operator[](Coordinate coor) { return internalBoard[coor.row][coor.col][coor.depth]; }
	const char& operator[](Coordinate coor) const { return internalBoard[coor.row][coor.col][coor.depth]; }
	char& At(int Row, int Col, int Depth) { return internalBoard[Row][Col][Depth]; }
	char& At(Coordinate coor) { return internalBoard[coor.row][coor.col][coor.depth]; }
	const char& At(int Row, int Col, int Depth) const { return internalBoard[Row][Col][Depth]; }

private:
	vector<vector<string>> internalBoard; // Holds all player's ships + 'X' mark for every cell that shouldn't be attacked
};