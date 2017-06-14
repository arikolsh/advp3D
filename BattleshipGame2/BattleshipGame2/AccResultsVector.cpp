#include "AccResultsVector.h"

void AccResultsVector::safeAccPush(const PlayerResult& p) {
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

PlayerResult AccResultsVector::safeGet(size_t i) {
	mutex gMutex;
	unique_lock<mutex> gLock(gMutex);
	_cv.wait(gLock, [&i, this]() {return i < _count; });
	return _vec[i];
}

PlayerResult AccResultsVector::unsafeGet(size_t i) {
	return _vec[i];
}

size_t AccResultsVector::unsafeGetSize() const
{
	return _count;
}

void AccResultsVector::releaseCV()
{
	_cv.notify_one();
}
