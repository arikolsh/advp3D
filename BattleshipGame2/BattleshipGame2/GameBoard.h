#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

class GameBoard : public BoardData
{
public:
	GameBoard() = default;
	explicit GameBoard(int rows, int cols, int depth);
	explicit GameBoard(vector<vector<string>>& board, int rows, int cols, int depth);
	GameBoard(const GameBoard& that);
	char charAt(Coordinate c) const override; //returns only selected players' chars
	void print(bool includePadding) const;
	vector<vector<string>> board() const; //return raw board
	char& setAt(Coordinate c) { return _board[c.row][c.col][c.depth]; }
private:
	vector<vector<string>> _board; // Holds all ships
};