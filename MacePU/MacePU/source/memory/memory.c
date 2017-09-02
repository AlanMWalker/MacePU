#include <stdio.h>
#include <stdlib.h>
#include <cassert>

#include "memory\memory.h"

static int8* memoryByteArray = NULL;

MemoryInitialiseError initialiseMemory()
{
	memoryByteArray = (int8*)malloc(MAX_CPU_MEMORY_BYTES * sizeof(int8));

	if (memoryByteArray == NULL)
	{
		return FailedToAllocatePool;
	}

	return Success;
}

void deinitMemory()
{
	free(memoryByteArray);
	
}

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, int8 * pMemByte)
{
	assert(bufferSize > 0);
	assert(memoryByteArray != NULL);

	if (bufferSize <= 0 || memoryByteArray == NULL)
	{
		printf("Incorrect buffer to load program");
		return IncorrectBufferSize;
	}

	for (int16 i = 0; i < bufferSize; ++i)
	{
		memoryByteArray[i] = (pMemByte[i]);
	}

	return EXIT_SUCCESS;
}
