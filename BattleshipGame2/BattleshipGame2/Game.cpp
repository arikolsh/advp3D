#include "GameUtils.h"
#include <vector>
#include <filesystem>
#include "GameBoard.h"
#include "GameManager.h"
#include <iostream>
#include <windows.h>
#include "IBattleshipGameAlgo.h"

using namespace std;

void getArgs(int argc, char** argv, int& threads, string& searchDir);
int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll);

int main(int argc, char* argv[])
{

	string boardPath = "good_board.sboard";
	vector<vector<string>> board3d;
	








}











int getPlayerFromDll(string dllPath, IBattleshipGameAlgo* &player, HINSTANCE& hDll)
{ //todo: guarantee player is converted to ptr
	// define function of the type we expect
	typedef IBattleshipGameAlgo *(*GetAlgoType)();
	GetAlgoType getAlgo;

	hDll = LoadLibraryA(dllPath.c_str());
	if (!hDll)
	{
		cout << "Cannot load dll: " << dllPath << endl;
		return EXIT_FAILURE;
	}

	// Get function pointer

	getAlgo = GetAlgoType(GetProcAddress(hDll, "GetAlgorithm"));
	if (!getAlgo)
	{
		cout << "Algorithm initialization failed for dll: " << dllPath << endl;
		FreeLibrary(hDll);
		return EXIT_FAILURE;
	}

	/* init player A */
	player = getAlgo();
	return EXIT_SUCCESS;
}

void getArgs(int argc, char** argv, int& threads, string& searchDir)
{
	vector<string> argsVector(argv, argv + argc);
	int i = 1;
	threads = 4; //todo: put in properties file
	while (i < argc)
	{
		if (argsVector[i] == "-threads")
		{
			i++;
			threads = stoi(argsVector[i]);
		}
		else
		{
			searchDir = argsVector[i];
		}
		i++;
	}
}