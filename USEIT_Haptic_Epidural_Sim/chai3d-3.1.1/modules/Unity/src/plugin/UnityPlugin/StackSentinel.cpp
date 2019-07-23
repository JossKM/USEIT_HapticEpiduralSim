#include "StackSentinel.h"
#include "DebugSettings.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>

#define MAGIC_DEBUGGING_NUMBER 0xDD

StackSentinel::StackSentinel()
{
	for (size_t i = 0; i < SENTINEL_BUFFER_SIZE; i++)
	{
		m_buffer[i] = MAGIC_DEBUGGING_NUMBER;
	}
}

StackSentinel::~StackSentinel()
{
	for (size_t i = 0; i < SENTINEL_BUFFER_SIZE; i++)
	{
		assert(m_buffer[i] == MAGIC_DEBUGGING_NUMBER);
	}
}

#undef MAGIC_DEBUGGING_NUMBER

