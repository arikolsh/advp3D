#pragma once
#include <vector>
#include "PlayerResult.h"
#include <mutex>

class SafeAccResultsVector
{
	vector<PlayerResult> _vec;
	size_t _count = 0;
	mutex _pLock; //lock for pushing to vector
	condition_variable _cv;

public:
	void safeAccPush(const PlayerResult& p);
	PlayerResult safeGet(size_t i);
};
