#pragma once
#include <vector>

using namespace std;

class GameUtils {
public:
	static void showMessages(vector<string> messages);

	/* getInputFiles function gets the cmd args and a vector of input files with
	* size 3 initialized with empty string in each object and returns success
	* iff it succeeded to fill the given inputFiles vector with the 3 reguired files paths
	* and the returned mesaages vector is with size of 0 (no failures).
	* The inputFiles vector is filled in the following structure:
	* pos0 = battle board
	* pos1 = a attack board
	* pos2 = b attack board	*/
	static int getInputFiles(vector<string>& inputFiles, vector<string>& messages, vector<string>& DLLNames, string searchDir);
	/* print 2d char array for debug purposes */
	static void print2DCharArray(char**arr, int rows, int cols);

private:
	GameUtils() = delete; //disallow creating instance of class

	~GameUtils() = delete; //disallow destructor

						   /* this function returns ERROR macro if inner functions calls failed otherwise the function returns the shell exit code */
	static int execCmd(char const * cmd, string & shellRes);

	/* remove ships with wrong shape or size from board. returns -1 if there are invalid ships and prints proper
	* messages */
	static int fetchInputFiles(vector<string> & inputFiles, vector<string> & messages, vector<string> & DLLNames, const string path);

};