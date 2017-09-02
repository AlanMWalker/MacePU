#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <stdbool.h>

#include "memory\memory.h"

static int16 lastOpcode = 0;

int8* createMemoryByteArray()
{
	static int8* memoryByteArray = NULL;

	return memoryByteArray ? memoryByteArray : (memoryByteArray = malloc(MAX_CPU_MEMORY_BYTES));
}

void deinitMemory()
{
	static int8 count = 0;
	++count;
	int8* byteArray = createMemoryByteArray();
	if (byteArray != NULL)
	{
		free(byteArray);
	}
}

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, const int8 * pMemByte)
{
	int8* byteArray = createMemoryByteArray();
	assert(bufferSize > 0);
	assert(pMemByte != NULL);
	assert(byteArray != NULL);

	if (bufferSize <= 0 || pMemByte == NULL)
	{
		printf("Incorrect buffer to load program");
		return IncorrectBufferSize;
	}

	if (byteArray == NULL)
	{
		return FailedToAllocatePool;
	}


	for (int16 i = 0; i < bufferSize; ++i)
	{
		byteArray[i] = (pMemByte[i]);
	}

	return EXIT_SUCCESS;
}

uint16 getEndOfExecutableMarker()
{
	return lastOpcode;
}
