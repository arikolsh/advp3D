#pragma once
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

//#define EMPTY_CELL ' '

class NewGameBoard : public BoardData
{
public:
	//explicit GameBoard();
	//~GameBoard();
	char charAt(Coordinate c) const override; //returns only selected players' chars
	char& SetAt(int Row, int Col, int Depth) { return _board[Row][Col][Depth]; }
	void SetDimentions(int rows, int cols, int depth) { _rows = rows, _cols = cols, _depth = depth; }
	void initialize();
	void print_3D_board(bool includePadding) const;
private:
	vector<vector<string>> _board; // Holds all ships
};