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
	void SetDimentions(int rows, int cols, int depth) { _rows = rows, _cols = cols, _depth = depth; } //todo: dont need
	void initialize(); //todo: dont need
	void print_3D_board(bool includePadding) const;
	void print(bool includePadding) const;
	//static bool getBoardFromFile(vector<vector<string>> &board3d, string path);
private:
	vector<vector<string>> _board; // Holds all ships
	static vector<vector<string>> getNewEmptyBoard(int rows, int cols, int depth);
	static vector<vector<string>> getBoardCopy(const vector<vector<string>> board);
	//static bool fillBoardWithShipsFromFile(vector<vector<string>>& board3d, ifstream& file, string path, int rows, int cols, int depth);


};