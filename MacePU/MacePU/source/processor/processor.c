#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <cassert>

#include "cpudefs\DataTypes.h"
#include "processor\processor.h"
#include "memory\memory.h"
#include "cpudefs\OperationCodes.h"

#include "FiletypeInformation.h"

#define FREE_FILE(file) if(file != NULL){ fclose(file); }

const double CLOCK_TIME_PER_TICK = 1.0 / 3.1258;

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

	if (!isValidExecutableFileMarkers(pBinaryFile))
	{
		FREE_FILE(pBinaryFile);
		return InvalidFileMarkers;
	}

	int8* byteArray = (int8*)malloc((size_t)binarySize * sizeof(int8));

	if (byteArray == NULL)
	{
		return NullPtrException;
	}

	memset(byteArray, 0, (size_t)(sizeof(int8) * binarySize));

	fread_s((void*)byteArray, (size_t)binarySize, (size_t)binarySize, 1, pBinaryFile);

	MemoryInitialiseError memInit = loadProgramIntoMemory((int16)binarySize * sizeof(int8), byteArray);

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
	Registers cpuRegisters;
	cpuRegisters.programCounter = 0;
	cpuRegisters.endOfExecutableMarker = getEndOfExecutableNumber();
	printf("\nnumber of instructions: %d\n", cpuRegisters.endOfExecutableMarker);

	for (int8 i = 0; i < _countof(cpuRegisters.gpr); ++i)
	{
		cpuRegisters.gpr[i] = 0;
	}

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

		processorTick(&cpuRegisters);

		QueryPerformanceCounter(&tEnd);
		tSecsElapsed = (tEnd.QuadPart - tStart.QuadPart) / (double)tFreq.QuadPart;
		double tMsElapsed = tSecsElapsed * 1000;

		//double tMsPerIteration = tMsElapsed;

		printf("%lf ms\n", tMsElapsed);

		for (int8 i = 0; i < _countof(cpuRegisters.gpr); ++i)
		{
			printf("R%d value is: %d\n", i, cpuRegisters.gpr[i]);
		}

		const double SleepTime = (CLOCK_TIME_PER_TICK - tSecsElapsed) * 1000;
		if (SleepTime >= 0.0)
		{
			Sleep((DWORD) SleepTime);
		}

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

void processorTick(Registers * regs)
{
	int24 instrLine = getInstructionLine(regs->programCounter);
	handleInstructionLine(instrLine, regs);
	++regs->programCounter;
}

void handleInstructionLine(int24 instrLine, Registers* regs)
{
	const int8 opcode = getOpcode(instrLine);
	const bool isArg1Reg = isArg1Register(instrLine);

	int8 arg0 = 0, arg1 = 0;

	switch (opcode)
	{

	case OP_LOAD:
		arg0 = getArg0(instrLine);
		arg1 = getArg1(instrLine);
		if (!isArg1Reg)
		{

			regs->gpr[arg0] = arg1;
		}
		else
		{
			regs->gpr[arg0] = regs->gpr[arg1];
		}

		break;

	case OP_STORE:
		break;

	case OP_ADD:
		arg0 = getArg0(instrLine);
		arg1 = getArg1(instrLine);

		if (!isArg1Reg)
		{
			regs->gpr[arg0] += arg1;
		}
		else
		{
			regs->gpr[arg0] += regs->gpr[arg1];
		}

		break;

	default: break;
	}
}

bool isValidExecutableFileMarkers(FILE * pBinaryFile)
{
	assert(pBinaryFile != NULL);

	if (pBinaryFile == NULL)
	{
		return false;
	}

	int8 markerBuffer[20];
	fread_s(markerBuffer, _countof(markerBuffer), sizeof(int8), strlen(FILE_HEADER) + 1, pBinaryFile);

	if (strcmp(markerBuffer, FILE_HEADER) != 0)
	{
		printf("Incorrect file header!!\n");
		return false;
	}

	fseek(pBinaryFile, -((signed)strlen(FILE_FOOTER) + 1), SEEK_END);

	fread_s(markerBuffer, _countof(markerBuffer), sizeof(int8), strlen(FILE_FOOTER) + 1, pBinaryFile);

	if (strcmp(markerBuffer, FILE_FOOTER))
	{
		printf("Incorrect file footer!!\n");
		return false;
	}

	fseek(pBinaryFile, 0, SEEK_SET);

	return true;
}

int8 getOpcode(int24 intsrLine)
{
	return (intsrLine.val & OPCODE_MASK) >> OPCODE_SHIFT;
}

int8 getArg0(int24 intsrLine)
{
	return (intsrLine.val & ARG0_MASK) >> ARG0_SHIFT;
}

int8 getArg1(int24 intsrLine)
{
	return (intsrLine.val & ARG1_MASK);
}

bool isArg1Register(int24 instrLine)
{
	int8 val = (instrLine.val & ARG1_INFO_MASK) >> IS_ARG1_REG_SHIFT;

	return val != 0;
}
