#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <stdbool.h>

#include "memory\memory.h"
#include "FiletypeInformation.h"
#include "cpudefs\OperationCodes.h"

static int16 lastOpcode = 0;
static int16 firstOpcode = 0;
static int16 numberOfOperations = 0;

int8* getByteMemoryArray()
{
	static int8* memoryByteArray = NULL;

	return memoryByteArray ? memoryByteArray : (memoryByteArray = malloc(MAX_CPU_MEMORY_BYTES));
}

void deinitMemory()
{
	static int8 count = 0;
	++count;
	int8* byteArray = getByteMemoryArray();
	if (byteArray != NULL)
	{
		free(byteArray);
	}
}

MemoryInitialiseError loadProgramIntoMemory(int16 bufferSize, const int8 * pMemByte)
{
	int8* byteArray = getByteMemoryArray();
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

	numberOfOperations = *(pMemByte + strlen(FILE_HEADER) + 1);

	firstOpcode = ((int16)strlen(FILE_HEADER) + 1) + sizeof(int16); //number of instructions is 
	lastOpcode = (bufferSize - firstOpcode) - ADDRESS_BUS_LENGTH_BYTES;

	return EXIT_SUCCESS;
}

uint16 getEndOfExecutableNumber()
{
	return ((lastOpcode - firstOpcode) + 1) / 3;
}

int24 getInstructionLine(uint16 programCounter)
{
	int24 instrLine;
	instrLine.val = 0;
	int8* byteArr = getByteMemoryArray();
	assert(byteArr);

	if (byteArr == NULL)
	{
		printf("Failed to get an instruction line with programCounter value of: %d\n", programCounter);
		return instrLine;
	}

	int8* startPos = &byteArr[firstOpcode + (programCounter * 3)];

	if (startPos == NULL)
	{
		return instrLine;
	}

	instrLine.val = (instrLine.val | (*startPos));
	instrLine.val = (instrLine.val | *(startPos + 1) << ARG0_SHIFT);
	instrLine.val = (instrLine.val | *(startPos + 2) << OPCODE_SHIFT);

	return instrLine;
}
