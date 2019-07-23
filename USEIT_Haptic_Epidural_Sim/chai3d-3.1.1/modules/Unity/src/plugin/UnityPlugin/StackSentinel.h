#pragma once
#include <cstdint>


// Credit to Tautvydas Zilys, from this nightmare story: https://blogs.unity3d.com/2016/04/25/debugging-memory-corruption-who-the-hell-writes-2-into-my-stack-2/
struct StackSentinel
{
private:
	static const size_t SENTINEL_BUFFER_SIZE = 16 * 1024;
	uint8_t m_buffer[SENTINEL_BUFFER_SIZE];

public:
	StackSentinel();
	~StackSentinel();
};