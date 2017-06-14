#pragma once
#include <vector>
#include "PlayerResult.h"
#include <mutex>

class SafeVector
{
	vector<PlayerResult> buffer;
	int _count = 0;
	mutex _pLock; //lock for pushing to vector
	condition_variable _cv;

public:
	void safePush(const PlayerResult& p);
	PlayerResult safeGet(int i);
};
