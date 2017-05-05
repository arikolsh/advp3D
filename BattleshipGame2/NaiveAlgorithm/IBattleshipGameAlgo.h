#pragma once

#include <utility> // for std::pair
#include <string>

enum class AttackResult {
	Miss, Hit, Sink
};

// IBattleshipGameAlgo for Ex2 - do not change this class - you should use it as is
class IBattleshipGameAlgo {

public:

	virtual ~IBattleshipGameAlgo() = default;

	// called once to notify player on his board
	virtual void setBoard(int player, const char** board, int numRows, int numCols) = 0;

	// called once to allow init from files if needed 
	// returns whether the init succeeded or failed
	virtual bool init(const std::string& path) = 0;

	// ask player for his move
	virtual std::pair<int, int> attack() = 0;

	// notify on last move result
	virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) = 0;
};

#ifdef ALGO_EXPORTS // A flag defined in this project's Preprocessor's Definitions
#define ALGO_API extern "C" __declspec(dllexport) // If we build - export
#else
// If someone else includes this .h - import
#define ALGO_API extern "C" __declspec(dllimport)
#endif

// This method must be implemented in each player(algorithm) .cpp file
ALGO_API IBattleshipGameAlgo* GetAlgorithm();