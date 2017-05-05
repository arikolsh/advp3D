#include <fstream>
#include <iostream>
#include "GameUtils.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#define MAX_PATH 1024
#define SEARCH_DEFAULT_CMD "dir /b /a-d 2> nul"
#define WRONG_PATH "Wrong path: "
#define MISSING_BOARD_FILE "Missing board file (*.sboard) looking in path: "
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



int GameUtils::getInputFiles(vector<string> & inputFiles, vector<string> & messages, vector<string> & DLLNames, string searchDir)
{
	int op_res = fetchInputFiles(inputFiles, messages, DLLNames, searchDir);
	if (op_res == FAILURE)
	{
		cout << "Error: failed to fetch input files from super-file container" << endl;
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
	if(arr==nullptr)
	{
		return;
	}
	for(int i=0;i<rows;i++)
	{
		for(int j=0;j<cols;j++)
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
		shellRes += "\n"; // add EOL
	}
	/* Close pipe and get shelexit code */
	exitCode = _pclose(pPipe); // returns -1 in case of a _popen failure otherwise returns the shell exit code
	return exitCode;
}

int GameUtils::fetchInputFiles(vector<string> & inputFiles, vector<string> & messages, vector<string> & DLLNames, const string path)
{
	int hasBoardFile = 0, hasDLL = 0, opRes;
	size_t delimIndex;
	const char * searchCmd;
	string s_search_cmd, pathToFiles, line, fileExtension, shellRes;
	ostringstream tmpSearchCmd, tmpPathToFiles, wrongPath, missingBoardFile, missingDLL, missingBAttckFile, fullPath;
	vector<string> tmpInputFiles, tmpDLLFiles, tmpDLLNames;

	if (path.empty() == true)
	{
		//prepare messsages
		wrongPath << WRONG_PATH << WORKING_DIR << endl;
		missingBoardFile << MISSING_BOARD_FILE << WORKING_DIR << endl;
		missingDLL << MISSING_DLL << WORKING_DIR << endl;
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
		missingDLL << MISSING_DLL << path << endl;
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
			tmpSearchCmd << "dir " << path << " /b /a 2> nul";
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
		delimIndex = line.find('.');
		if (delimIndex == string::npos) {	//string::npos returns when '.' was not found
			continue;
		}
		if (line.substr(delimIndex + 1).compare(BOARD_EXTENSION) == 0)
		{
			// if we read from the working directory
			if ((hasBoardFile != 1) && (path.empty() == true))
			{
				tmpInputFiles.push_back(line);
				hasBoardFile = 1;
				continue;
			}
			// if we read from a given path
			if ((hasBoardFile != 1) && (path.empty() == false))
			{
				fullPath << line;
				tmpInputFiles.push_back(fullPath.str());
				hasBoardFile = 1;
				fullPath.str("");
				fullPath << path << "\\";
			}
			continue;
		}
		if (line.substr(delimIndex + 1).compare(DLL_EXTENSION) == 0)
		{
			//push dll name only to tmpDLLNames array
			tmpDLLNames.push_back(line.substr(0, delimIndex));
			// if we read from the working directory
			if (path.empty() == true)
			{
				tmpDLLFiles.push_back(line);
				hasDLL++;
				continue;
			}
			// if we read from a given path
			fullPath << line;
			tmpDLLFiles.push_back(fullPath.str());
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
		inputFiles[FIRST] = tmpInputFiles[FIRST];
		//lexicographic order of the dll's full path
		sort(tmpDLLFiles.begin(), tmpDLLFiles.end());
		//lexicographic order of the dll's names only
		sort(tmpDLLNames.begin(), tmpDLLNames.end());
		//fill the rest of the vector with the DLLS full path
		inputFiles[SECOND] = tmpDLLFiles[FIRST];
		inputFiles[THIRD] = tmpDLLFiles[SECOND];
		//fill the DLLNames with names only
		DLLNames[FIRST] = tmpDLLNames[FIRST];
		DLLNames[SECOND] = tmpDLLNames[SECOND];
	}
	return SUCCESS;
}