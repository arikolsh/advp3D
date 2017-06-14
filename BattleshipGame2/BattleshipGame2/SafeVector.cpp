#include "SafeVector.h"

void SafeVector::safePush(const PlayerResult& p) {
	unique_lock<mutex> pLock(_pLock);
	buffer.push_back(p);
	++_count;
	_cv.notify_one();
}

PlayerResult SafeVector::safeGet(int i) {
	mutex gMutex;
	unique_lock<mutex> gLock(gMutex);
	_cv.wait(gLock, [&i, this]() {return i < _count; });
	return buffer[i];
}