#include <filesystem>
#include "Logger.h"
#include "GameManager.h"
#include "GameUtils.h"

using namespace std;
//checked
int main(int argc, char* argv[])
{
	int threads;
	string searchDir;
	GameUtils::getArgs(argc, argv, threads, searchDir);
	///check logger//
	Logger* logger = Logger::getInstance();
	logger->init("game.log", "DEBUG");
	logger->destroy();
	GameManager gameManager(searchDir, threads);
	bool isStarted = gameManager.init();
	if (!isStarted)
	{
		return EXIT_FAILURE;
	}
	gameManager.runGame();
	return EXIT_SUCCESS;

}
