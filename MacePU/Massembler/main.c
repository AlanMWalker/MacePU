#include <stdio.h>	
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>

#include <vld.h> // visual leak detector

#include "..\MacePU\include\OperationCodes.h"
#include "..\MacePU\include\DataTypes.h"

//TODO: Convert MASM into a binary format that can be loaded into memory by the CPU then executed
//TODO: Allow user to specify the filename of the assembled binary format
//TODO: Create option to save file to directory of users choice (via command line arg)

#define MASM_FILE_EXT ".masm" // assembly file text format 
#define CPU_BINARY_FORMAT ".msm"  // assembly binary executable format
#define MEMSET_RESET -52

void pauseForReturnKey()
{
	printf("Hit return to close...\n");
	scanf_s("");
}

void handleError(const int8* failedString)
{
	printf(failedString);
	printf("\n");
	pauseForReturnKey();
}

void convertLineToInstruction(const int8* instructionLine, bool isLastInstruction);

int8 convertStringToOpcode(const int8* inBuffer);

bool isArgRegister(const int8* inBuffer);

int main(int argc, int8* argv[])
{
	int8 instrLine[256];

	//uint24 i;
	//i.val = OP_STORE;//
	//
	//i.val = i.val << INSTRUCTION_SHIFT;
	//
	//i.val |= (1 << ARG0_SHIFT);
	//i.val |= (2 << ARG1_SHIFT);
	//
	//int8 opcode = (i.val & INSTRUCTION_MASK) >> INSTRUCTION_SHIFT;

	if (argc < 2)
	{
		handleError("No massembly file passed as argument!");

		return -1;
	}

	int32 argsIndex;

	for (argsIndex = 0; argsIndex < argc; ++argsIndex)
	{
		if (strstr(argv[argsIndex], MASM_FILE_EXT))
		{
			break;
		}
	}

	FILE* pFile = NULL;
	int32 result = fopen_s(&pFile, argv[argsIndex], "r");
	assert(pFile != NULL);

	if (result != 0)
	{
		handleError("Assembly file not found!");
	}

	while (!feof(pFile))
	{
		int8* rtrn = fgets(instrLine, _countof(instrLine), pFile);
		if (rtrn != NULL)
		{
			printf("%s\n", instrLine);
			convertLineToInstruction(instrLine, false);
		}
		else
		{
			convertLineToInstruction(instrLine, true);
		}
		memset(instrLine, MEMSET_RESET, strlen(instrLine));
	}

	if (pFile != NULL)
	{
		fclose(pFile);
	}

	pauseForReturnKey();

	return 0;
}

void convertLineToInstruction(const int8 * instructionLine, bool isLastInstruction)
{
	assert(instructionLine);

	if (instructionLine == NULL)
	{
		return;
	}

	static FILE* pBinaryExecutable = NULL;
	static int8 buffer[100];
	const int32 StringLength = strlen(instructionLine);
	int16 generatedInstruction = 0;
	int8* commaPos = NULL;
	int32 index;

	if (pBinaryExecutable == NULL)
	{
		fopen_s(&pBinaryExecutable, "assembled.msm", "wb+");
	}
	assert(pBinaryExecutable != NULL);

	if (pBinaryExecutable == NULL)
	{
		handleError("Failed to create binary format!");
		pauseForReturnKey();
	}

	commaPos = strchr(instructionLine, ','); // check if there's more than 1 arg

	if (commaPos != NULL) // if there's more than 1 argument
	{
		for (index = 0; index < (signed)strlen(instructionLine); ++index)
		{
			if (instructionLine[index] == ' ')
			{
				buffer[index] = '\0';
				++index;  // ignore space between args
				break;
			}
			buffer[index] = instructionLine[index];
		}

		memset(buffer, MEMSET_RESET, strlen(buffer)); // clear buffer

		int32 opcode = convertStringToOpcode(buffer);

		//TODO insert opcode arg validity check here!

		if (opcode == -1)
		{
			handleError("Failed to find opcode!");
			pauseForReturnKey();
			return;
		}

		//extract arg from instruction line string
		int32 indexBuff = 0;
		for (index; index < (signed)strlen(instructionLine); ++index)
		{
			if (instructionLine[index] == *commaPos)
			{
				buffer[indexBuff] = '\0';
				break;
			}
			buffer[indexBuff] = instructionLine[index];
			++indexBuff;
		}
		int8 argValue1;
		if (isArgRegister(buffer)) // if the instruction is an argument 
		{
			argValue1 = buffer[1];
		}
		else
		{
			handleError("Incorrect load instruction! Can only load into registers!");
			pauseForReturnKey();
		}
		memset(buffer, MEMSET_RESET, strlen(buffer));
	}
	else // if there's not
	{

	}

	if (pBinaryExecutable != NULL && isLastInstruction)
	{
		fclose(pBinaryExecutable);
	}
}

int8 convertStringToOpcode(const int8 * inBuffer)
{
	//TOOD: Convert checking to read from a text file and determine the correct instruction by a table
	assert(inBuffer);

	int8 buffer[250];
	memcpy_s(buffer, _countof(buffer), inBuffer, strlen(inBuffer) + 1);

	for (int32 i = 0; i < (signed)strlen(buffer); ++i)
	{
		buffer[i] = (int8)towlower(i);
	}

	if (strcmp(buffer, "load"))
	{
		return OP_LOAD;
	}
	else if (strcmp(buffer, "store"))
	{
		return OP_STORE;
	}
	else if (strcmp(buffer, "add"))
	{
		return OP_ADD;
	}
	return -1;
}

bool isArgRegister(const int8 * inBuffer)
{
	int8* ptr = strchr(inBuffer, 'r');

	return ptr != NULL;
}
