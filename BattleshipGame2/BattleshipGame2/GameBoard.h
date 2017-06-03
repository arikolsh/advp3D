#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

//#define EMPTY_CELL ' '

class GameBoard : public BoardData
{
public:
	GameBoard() = delete;
	explicit GameBoard(vector<vector<string>>& board, int rows, int cols, int depth);
	GameBoard(const GameBoard& that);
	char charAt(Coordinate c) const override; //returns only selected players' chars
	void print(bool includePadding) const;
	vector<vector<string>> board() const; //return raw board
private:
	vector<vector<string>> _board; // Holds all ships
};