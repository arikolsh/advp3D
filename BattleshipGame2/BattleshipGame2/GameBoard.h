#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

class GameBoard : public BoardData
{
public:
	GameBoard() = default;
	explicit GameBoard(int rows, int cols, int depth);
	explicit GameBoard(vector<vector<string>>& board, int rows, int cols, int depth, int shipsPerPlayer[]);
	GameBoard(const GameBoard& that);
	char charAt(Coordinate c) const override; //returns only selected players' chars
	void print(bool includePadding) const;
	vector<vector<string>> board() const; //return raw board
	char& setAt(Coordinate c) { return _board[c.row][c.col][c.depth]; }
	pair<int, int> getShipsPerPlayer() const { return _shipsPerPlayer; }
private:
	vector<vector<string>> _board; // Holds all ships
	pair<int,int> _shipsPerPlayer;
};