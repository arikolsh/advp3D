#pragma once
#include <vector>

using namespace std;

class GameUtils {
public:
	static void showMessages(vector<string> messages);
	static void getArgs(int argc, char** argv, int& threads, string& searchDir);
	/* getInputFiles function gets the cmd args and a vector of input files (without pre-defined size)
	* the function returns successs iff it succeeded to fill the given inputFiles vector with the required files
	* and the returned mesaages vector is with size of 0 (no failures).
	* The inputFiles vector is filled in the following structure:
	* pos0 = battle board
	* all the next positions = unordered DLLs */
	static int getInputFiles(vector<string> & boards, vector<string> & dlls, vector<string>& messages, string searchDir);
	/* print 2d char array for debug purposes */
	static void print2DCharArray(char**arr, int rows, int cols);
	static vector<pair<int, int>> zip(vector<int> arr1, vector<int> arr2);

private:
	GameUtils() = delete; //disallow creating instance of class

	~GameUtils() = delete; //disallow destructor
						   /* this function returns ERROR macro if inner functions calls failed otherwise the function returns the shell exit code */
	static int execCmd(char const * cmd, string & shellRes);

	/* remove ships with wrong shape or size from board. returns -1 if there are invalid ships and prints proper
	* messages */
	static int fetchInputFiles(vector<string> & boards, vector<string> & dlls, vector<string> & messages, const string path);

};