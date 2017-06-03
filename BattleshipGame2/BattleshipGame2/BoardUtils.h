#pragma once
#include <vector>
/*
 * common utility functions for vector<vecotr<string>>
 */
class BoardUtils {
	BoardUtils() = delete; //disallow creating instance of class
	~BoardUtils() = delete; //disallow destructor
	/*
	* split string into vector of substrings according to delimiter.
	* example: "hi:how:are:you" => [hi,how,are,you] with delimiter ':' .
	*/
	static void split(std::string line, std::vector<std::basic_string<char>>& splitLine, char delimiter);
	/*
	* fill board3d with ships according to ships positioned in file.
	*/
	static bool fillBoardWithShipsFromFile(std::vector<std::vector<std::string>>& board3d, std::ifstream& file, std::string path, int rows, int cols, int depth);
	/*
	* check for a given ship ,in a certain direction, on the board its surroundings, and return true
	* iff they are empty.
	*/
	static bool isEmptySurroundings(std::vector<std::vector<std::string>>& board, int direction, int depth, int row, int col, int shipLen);
	/*
	* for a specifc ship on the board in a certain direction, mark its cells and return its length.
	*/
	static int markCellsAndGetLen(std::vector<std::vector<std::string>>& board, char ship, int direction, int depth, int row, int col);
	/*
	* for a specifc cell which contains a ship type char, return its direction. can be RIGHT,DOWN,DEEP.
	*/
	static int findShipDirection(std::vector<std::vector<std::string>> board, int depth, int row, int col, char ship);
public:
	/*
	* fill board3d with the board in given path, and put in depth, rows and cols its dimensions.
	* return true if successful and false otherwise.
	*/
	static bool getBoardFromFile(std::vector<std::vector<std::string>> &board3d, std::string path, int& depth, int& rows, int& cols);
	/*
	* check if valid board and put number of ships for each player in numShips[]
	* return true iff no adjacent ships, no invalid shapes, number of ships is MAX_NUM_SHIPS.
	*/
	static bool isValidBoard(std::vector<std::vector<std::string>> board, int depth, int rows, int cols, int numShips[]);
	/*
	* return new padded vector of vector of string with the given dimensions.
	* newBoard.size() will be the depth +PADDING (number of matrices)
	* newBoard[0].size() will be the number of rows +PADDING (in each matrix)
	* newBoard[0][0].size() will be the number of colums +PADDING (in each matrix).
	*/
	static std::vector<std::vector<std::string>> getNewEmptyBoard(int depth, int rows, int cols);
	/*
	* print vector<vector<string>> to console.
	*/
	static void printBoard(std::vector<std::vector<std::string>> board3d, bool printPadding);
	/*
	* get a copy of board.
	*/
	static std::vector<std::vector<std::basic_string<char>>> getBoardCopy(const std::vector<std::vector<std::string>> board);
};
