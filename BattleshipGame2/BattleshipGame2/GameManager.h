#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <memory>

class GameManager {
public:
	explicit GameManager(std::string& searchDir, int threads);
	bool init();

private:
	GameManager() = delete;
	GameManager(const GameManager& that) = delete;
	static int GameManager::getPlayerFromDll(std::string dllPath, IBattleshipGameAlgo*& player);
	std::string _searchDir;
	int _threads;
	std::vector<std::string> _boardsPath; //holds the path for each board
	std::vector<std::string> _dllsPath;	//holds the path for each dll
	std::vector<std::string> _messages;
	std::vector<std::vector<std::vector<std::string>>> _boards; //holds vector of 3D boards
	std::vector<std::unique_ptr<IBattleshipGameAlgo>> _players;
};
