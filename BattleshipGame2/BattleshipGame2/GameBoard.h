#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

//#define EMPTY_CELL ' '

class GameBoard : public BoardData
{
public:
	GameBoard() = delete;
	explicit GameBoard(vector<vector<string>> board, int rows, int cols, int depth);
	GameBoard(const GameBoard& that) = delete;
	char charAt(Coordinate c) const override; //returns only selected players' chars
	void print(bool includePadding) const;
	char& setAt(Coordinate c) { return _board[c.row][c.col][c.depth]; }

	//Copy constructor
	//GameBoard(const GameBoard &board);

	//Instead of copy constructor...................replace
	static GameBoard getGameBoardCopy(const GameBoard board);

private:
	vector<vector<string>> _board; // Holds all ships
	static vector<vector<string>> getNewEmptyBoard(int rows, int cols, int depth);
	static vector<vector<string>> getBoardCopy(const vector<vector<string>> board);
};