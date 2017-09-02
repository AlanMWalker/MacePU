#ifndef MEMORY_H
#define MEMORY_H

#include "cpudefs\DataTypes.h"
#include "cpudefs\CPUMemory.h"

typedef enum _memInit
{
	Success = 0,
	FailedToAllocatePool = -1,
	IncorrectBufferSize = -2
} MemoryInitialiseError;

static int8* createMemoryByteArray();

void deinitMemory();

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, const int8* memoryByteArray);

uint16 getEndOfExecutableMarker();
#endif // !MEMORY_H
