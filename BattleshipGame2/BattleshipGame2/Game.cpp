#include <filesystem>
#include "Logger.h"
#include "GameManager.h"
#include "GameUtils.h"

using namespace std;

int main(int argc, char* argv[])
{
	int threads;
	string searchDir;
	GameUtils::getArgs(argc, argv, threads, searchDir);
	///check logger//
	Logger* logger = Logger::getInstance();
	logger->init("game.log", "DEBUG");
	string path = "";
	GameManager gameManager(path, threads);
	bool isStarted =gameManager.init();
	if(!isStarted)
	{
		logger->destroy();
		return EXIT_FAILURE;
	}
	gameManager.runGame();
	logger->destroy();
	return EXIT_SUCCESS;

}