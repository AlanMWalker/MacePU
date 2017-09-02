#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>

#include "cpudefs\DataTypes.h"
#include "processor\processor.h"
#include "memory\memory.h"

#define FREE_FILE(file) if(file != NULL){ fclose(file); }

ProcessorInitError initProcessor(int32 argc, int8* argv[])
{
	const int32 FilePathIndex = 1;

	static int8* filePath = NULL;

	if (argc <= 1)
	{
		printf("ERROR! No MSM file was passed to processor!");
		return NoMSMFIlePassed;
	}

	filePath = argv[FilePathIndex];

	FILE* pBinaryFile = NULL;

	errno_t result = fopen_s(&pBinaryFile, filePath, "rb");

	if (result != 0)
	{
		printf("ERROR! No MSM file was found at: \"%s\"\n", filePath);
		return NoMSMFIlePassed;
	}

	fseek(pBinaryFile, 0, SEEK_END);
	const int64 binarySize = ftell(pBinaryFile);
	fseek(pBinaryFile, 0, SEEK_SET);

	printf("Binary size (bytes): %lld\n", binarySize);


	if (binarySize > MAX_PROGRAM_MEMORY_BYTES)
	{
		perror("Binary file too large!");
		return BinaryTooLarge;
	}

	int8* byteArray = (int8*)malloc((size_t)binarySize * sizeof(int8));

	if (byteArray == NULL)
	{
		return NullPtrException;
	}

	memset(byteArray, 0, (size_t)(sizeof(int8) * binarySize));
	
	fread_s(byteArray, binarySize, binarySize, 1, pBinaryFile);
	
	MemoryInitialiseError memInit = initialiseMemory();
	if (memInit != EXIT_SUCCESS)
	{

		printf("\nMemory initialise error inside: %s", __FILE__);
		FREE_FILE(pBinaryFile);
		return MemoryCorruption;
	}

	memInit = loadProgramIntoMemory((int16)binarySize * sizeof(int8), byteArray);

	if (memInit != EXIT_SUCCESS)
	{
		printf("\nMemory initialise error inside: %s \n at: %ld\n", __FILE__, __LINE__);
		FREE_FILE(pBinaryFile);
		return memInit;
	}

	FREE_FILE(pBinaryFile);
	free((void*)byteArray);
	byteArray = NULL;
	return Succes;
}

void runProcessor()
{
	bool running = true;
	int32 tickCount = 5;
	int32 i = 0;
	while (running == true)
	{
		LARGE_INTEGER tStart, tEnd;
		LARGE_INTEGER tFreq;
		double tSecsElapsed;
		QueryPerformanceFrequency(&tFreq);
		QueryPerformanceCounter(&tStart);

		processorTick();

		QueryPerformanceCounter(&tEnd);
		tSecsElapsed = (tEnd.QuadPart - tStart.QuadPart) / (double)tFreq.QuadPart;
		double tMsElapsed = tSecsElapsed * 1000;

		double tMsPerIteration = tMsElapsed;
		printf("%lf ms\n", tMsElapsed);
		//Sleep(1000);
		if (i >= tickCount)
		{
			running = false;
		}
		++i;

	}
}

void deinitProcessor()
{
	deinitMemory();
}

void processorTick()
{
}
