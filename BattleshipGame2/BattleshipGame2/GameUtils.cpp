#include <iostream>
#include "GameUtils.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Logger.h"

#define MAX_PATH 1024
#define SEARCH_DEFAULT_CMD "dir /b /a-d 2> nul"
#define WRONG_PATH "Wrong path: "
#define MISSING_BOARD_FILE "Missing board file (*.sboard) looking in path: "
#define MIN_DLL_REQ "(needs at least two)"
#define FILES_FILE_NAME "files.txt"
#define BOARD_EXTENSION "sboard"
#define ATTACK_A_EXTENSION "attack-a"
#define ATTACK_B_EXTENSION "attack-b"
#define WORKING_DIR "Working Directory"
#define EMPTY_CELL ' '
#define SUCCESS 0
#define FAILURE -1
#define FULL_BOARD_LEN 12
#define DLLS_AMOUNT 2
#define FIRST 0
#define SECOND 1
#define THIRD 2
#define SHELL_RES 4096
#define MODE "r"
#define MISSING_DLL "Missing an algorithm (dll) file looking in path: "
#define DLL_EXTENSION "dll"


void GameUtils::getArgs(int argc, char** argv, int& threads, string& searchDir)
{
	vector<string> argsVector(argv, argv + argc);
	int i = 1;
	threads = 4;
	searchDir = "";
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

int GameUtils::getInputFiles(vector<string> & boards, vector<string> & dlls, vector<string> & messages, string searchDir)
{
	int op_res = fetchInputFiles(boards, dlls, messages, searchDir);
	if (op_res == FAILURE)
	{
		Logger* logger = Logger::getInstance();
		logger->log("Error: failed to fetch input files from super-file container");
		return FAILURE;
	}
	if (messages.size() != 0)
	{
		showMessages(messages);
		return FAILURE;
	}
	return SUCCESS;
}

void GameUtils::print2DCharArray(char** arr, int rows, int cols)
{
	if (arr == nullptr)
	{
		return;
	}
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			cout << arr[i][j];
		}
		cout << endl;
	}
}

void GameUtils::showMessages(vector<string> messages)
{
	for (vector<string>::iterator it = messages.begin(); it != messages.end(); ++it)
	{
		cout << *it;
	}
}

int GameUtils::execCmd(char const * cmd, string & shellRes)
{
	FILE * pPipe;
	int execRes = FAILURE, exitCode = 0;
	char shellTmpRes[SHELL_RES] = { 0 };
	pPipe = _popen(cmd, MODE);
	if (pPipe == NULL)	//if an error occured
	{
		return FAILURE;
	}
	while (fgets(shellTmpRes, sizeof(shellTmpRes), pPipe))
	{
		shellRes += string(shellTmpRes);
	}
	/* Close pipe and get shelexit code */
	exitCode = _pclose(pPipe); // returns -1 in case of a _popen failure otherwise returns the shell exit code
	return exitCode;
}

// python zip equivalent
vector<pair<int, int>> GameUtils::zip(vector<int> arr1, vector<int> arr2)
{
	auto len = arr1.size() <= arr2.size() ? arr1.size() : arr2.size();
	vector<pair<int, int>> ret;
	for (auto i = 0; i < len; i++)
	{
		ret.push_back(make_pair(arr1[i], arr2[i]));
	}
	return ret;
}

int GameUtils::fetchInputFiles(vector<string> & boards, vector<string> & dlls, vector<string> & messages, const string path)
{
	int hasBoardFile = 0, hasDLL = 0, opRes;
	size_t delimIndex;
	const char * searchCmd;
	string s_search_cmd, pathToFiles, line, fileExtension, shellRes;
	ostringstream tmpSearchCmd, tmpPathToFiles, wrongPath, missingBoardFile, missingDLL, missingBAttckFile, fullPath;
	vector<string> tmpBoards, tmpDLLS;

	if (path.empty() == true)
	{
		//prepare messsages
		wrongPath << WRONG_PATH << WORKING_DIR << endl;
		missingBoardFile << MISSING_BOARD_FILE << WORKING_DIR << endl;
		missingDLL << MISSING_DLL << WORKING_DIR << MIN_DLL_REQ << endl;
		opRes = execCmd(SEARCH_DEFAULT_CMD, shellRes);
		if (FAILURE == opRes)
		{
			return FAILURE;
		}
		if (1 == opRes)
		{
			messages.push_back(wrongPath.str());
			return SUCCESS;
		}
	}
	else
	{
		fullPath << path << "\\";
		//prepare messsages
		wrongPath << WRONG_PATH << path << endl;
		missingBoardFile << MISSING_BOARD_FILE << path << endl;
		missingDLL << MISSING_DLL << path << MIN_DLL_REQ << endl;
		/* build the dir command: with arguments /b /a-d. if this command fail(op res == 1) there are 2 options:
		1. wrong or non-exist path - in this case present wrong path messasge and return.
		2. the dir exist but it is empty  - in this case we need to collect the "missing files warnings" and return,
		therfore we need to continue and open the output file.
		*/
		tmpSearchCmd << "dir \"" << path << "\" /b /a-d 2> nul";
		s_search_cmd = tmpSearchCmd.str();
		searchCmd = s_search_cmd.c_str();
		opRes = execCmd(searchCmd, shellRes);
		if (FAILURE == opRes)
		{
			return FAILURE;
		}
		if (1 == opRes)
		{
			// build the new dir command: with arguments /b /a. if also this command fail(op res == 1) than it's a wrong path
			shellRes.clear(); // clear the shellcmd content
			tmpSearchCmd.str("");
			tmpSearchCmd << "dir \"" << path << "\" /b /a 2> nul";
			s_search_cmd = tmpSearchCmd.str();
			searchCmd = s_search_cmd.c_str();
			opRes = execCmd(searchCmd, shellRes);
			if (FAILURE == opRes)
			{
				return FAILURE;
			}
			if (1 == opRes)
			{
				messages.push_back(wrongPath.str());
				return SUCCESS;
			}
		}
	}
	//in this point we act in the same logic for the two cases
	istringstream  inputFilesContainer(shellRes); // declaration must to be dynamic 
	while (getline(inputFilesContainer, line))
	{
		delimIndex = line.find_last_of('.');
		if (delimIndex == string::npos) {	//string::npos returns when '.' was not found
			continue;
		}
		if (line.substr(delimIndex + 1).compare(BOARD_EXTENSION) == 0)
		{
			// if we read from the working directory
			if (path.empty() == true)
			{
				tmpBoards.push_back(line);
				if (hasBoardFile != 1) { //if this is the first board that we see - we need it fot the messages collector
					hasBoardFile = 1;
				}
				continue;
			}
			// if we read from a given path
			if (path.empty() == false)
			{
				fullPath << line;
				tmpBoards.push_back(fullPath.str());
				if (hasBoardFile != 1) {
					hasBoardFile = 1;
				}
				fullPath.str("");
				fullPath << path << "\\";
			}
			continue;
		}
		if (line.substr(delimIndex + 1).compare(DLL_EXTENSION) == 0)
		{
			// if we read from the working directory
			if (path.empty() == true)
			{
				tmpDLLS.push_back(line);
				hasDLL++;
				continue;
			}
			// if we read from a given path
			fullPath << line;
			tmpDLLS.push_back(fullPath.str());
			hasDLL++;
			fullPath.str("");
			fullPath << path << "\\";
			continue;
		}
		fullPath.str("");
		fullPath << path << "\\";
	}
	//check flags
	if (hasBoardFile == 0)
	{
		messages.push_back(missingBoardFile.str());
	}
	if (hasDLL < DLLS_AMOUNT) // if we have less than 2 DLL's
	{
		messages.push_back(missingDLL.str());
	}
	if (messages.size() == 0)
	{
		for (size_t i = 0; i < tmpBoards.size(); i++)
		{
			boards.push_back(tmpBoards[i]);
		}
		for (size_t i = 0; i < tmpDLLS.size(); i++)
		{
			dlls.push_back(tmpDLLS[i]);
		}
	}
	return SUCCESS;
}