#include <filesystem>
#include "Logger.h"
#include "GameManager.h"
#include "GameUtils.h"

using namespace std;

int main(int argc, char* argv[])
{

	int threads;
	string searchDir, loggerPath;
	GameUtils::getArgs(argc, argv, threads, searchDir, loggerPath);
	auto logger = Logger::getInstance();
	logger->init(loggerPath);
	GameManager gameManager(searchDir, threads);
	bool isStarted = gameManager.init();
	if (!isStarted)
	{
		logger->destroy();
		return EXIT_FAILURE;
	}
	gameManager.runGameV2();
	logger->destroy();
	return EXIT_SUCCESS;

}