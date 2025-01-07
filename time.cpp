#include <chrono>

extern "C"
int _getentropy(uint8_t*, int) {
	return 0;
}

extern "C"
int _gettimeofday(struct timeval *tv, void *tzvp) {
	return 0;
}
