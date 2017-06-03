#include "GameBoardTest.h"
#include <iostream>
#include "SmartPlayer.h"
#include "IBattleshipGameAlgo.h"


using namespace std;

int main(int argc, char* argv[])
{
	cout << "Starting SmartPlayer Test" << endl;

	auto board = NewGameBoard();
	board.SetDimentions(10, 10, 10);
	board.initialize();
	board.print_3D_board(false);
	Coordinate coordinate(1, 1, 1);
	board.SetAt(1,1,1) = 'b';
	board.SetAt(3, 3, 0) = 'd';
	board.SetAt(3, 4, 0) = 'd';
	board.SetAt(3, 5, 0) = 'd';
	board.SetAt(3, 6, 0) = 'd';
	board.SetAt(5, 7, 0) = 'p';
	board.SetAt(6, 7, 0) = 'p';
	board.print_3D_board(false);

	IBattleshipGameAlgo* player = new SmartPlayer();

	delete player;
	return 0;
}