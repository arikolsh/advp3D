#include "SafeAccResultsVector.h"

void SafeAccResultsVector::safeAccPush(const PlayerResult& p) {
	unique_lock<mutex> pLock(_pLock);
	PlayerResult resultTillNow = p;
	// construct accumulated result until now
	if (_count > 0) {
		resultTillNow._totalNumLosses += _vec.back()._totalNumLosses;
		resultTillNow._totalNumPointsAgainst += _vec.back()._totalNumPointsAgainst;
		resultTillNow._totalNumPointsFor += _vec.back()._totalNumPointsFor;
		resultTillNow._totalNumWins += _vec.back()._totalNumWins;
	}
	_vec.push_back(resultTillNow);
	++_count;
	_cv.notify_one();
}

PlayerResult SafeAccResultsVector::safeGet(int i) {
	mutex gMutex;
	unique_lock<mutex> gLock(gMutex);
	_cv.wait(gLock, [&i, this]() {return i < _count; });
	return _vec[i];
}