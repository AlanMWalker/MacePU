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

MemoryInitialiseError initialiseMemory();

void deinitMemory();

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, int8* memoryByteArray);

#endif // !MEMORY_H
