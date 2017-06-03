#include "GameBoardTest.h"
#include <iostream>
#include "SmartPlayer.h"
#include "IBattleshipGameAlgo.h"


using namespace std;

int main(int argc, char* argv[])
{
	cout << "Starting SmartPlayer Test" << endl;

	auto board = NewGameBoard();
	board.SetDimentions(10, 10, 4);
	board.initialize();
	//board.print_3D_board(false);

	board.SetAt(1, 1, 1) = 'b';
	board.SetAt(3, 3, 1) = 'd';
	board.SetAt(3, 4, 1) = 'd';
	board.SetAt(3, 5, 1) = 'd';
	board.SetAt(3, 6, 1) = 'd';
	board.SetAt(5, 7, 1) = 'p';
	board.SetAt(6, 7, 1) = 'p';
	board.SetAt(10, 8, 1) = 'm';
	board.SetAt(10, 9, 1) = 'm';
	board.SetAt(10, 10, 1) = 'm';
	board.SetAt(10, 10, 2) = 'm';
	board.SetAt(10, 10, 3) = 'm';
	board.SetAt(10, 10, 4) = 'm';

	board.print_3D_board(false);

	IBattleshipGameAlgo* player = new SmartPlayer();

	delete player;
	return 0;
}