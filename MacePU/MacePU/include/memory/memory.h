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

static int8* getByteMemoryArray();

void deinitMemory();

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, const int8* memoryByteArray);

uint16 getEndOfExecutableNumber();

int24 getInstructionLine(uint16 instructionPtrValue);

#endif // !MEMORY_H
