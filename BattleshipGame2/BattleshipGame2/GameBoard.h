#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

//#define EMPTY_CELL ' '

class GameBoard : public BoardData
{
public:
	GameBoard();
	GameBoard(int rows, int cols, int depth);
	explicit GameBoard(vector<vector<string>> board);
	GameBoard(const GameBoard& that) = delete;
	char charAt(Coordinate c) const override; //returns only selected players' chars
	void print(bool includePadding) const;
private:
	vector<vector<string>> _board; // Holds all ships
};