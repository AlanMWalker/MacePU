#include <stdio.h>	
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "assembler\Assembler.h"

static FILE* pFile = NULL;
static FILE* pBinaryExecutable = NULL;

AssemblerReturnCode assembleFile(const char * masmFileLoc)
{
	int8 instrLine[256];
	int32 result = fopen_s(&pFile, masmFileLoc, "r");

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
		pFile = NULL;
	}

	return Success;
}

void convertLineToInstruction(const int8 * instructionLine, bool isLastInstruction)
{
	assert(instructionLine != NULL);

	if (instructionLine == NULL)
	{
		return;
	}

	static int8 buffer[100];
	const int32 StringLength = (signed)strlen(instructionLine);
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
		return;
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

		int32 opcode = convertStringToOpcode(buffer);

		//TODO insert opcode arg validity check here!

		if (opcode == -1)
		{
			handleError("Failed to find opcode!");
			pauseForReturnKey();
			return;
		}
		memset(buffer, MEMSET_RESET, strlen(buffer)); // clear buffer
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

		int8 arg0, arg1;
		if (isArgRegister(buffer)) // if the instruction is an argument 
		{
			arg0 = atoi(&buffer[1]);
		}
		else
		{
			//TODO remove else logic and change to a func call to check correct arg validity
			handleError("Incorrect load instruction! Can only load into registers!");
			pauseForReturnKey();
		}
		memset(buffer, MEMSET_RESET, strlen(buffer));

		index += 1;
		indexBuff = 0;
		for (index; index < (signed)strlen(instructionLine); ++index)
		{
			if (instructionLine[index] == '\n')
			{
				buffer[indexBuff] = '\0';
				break;
			}
			buffer[indexBuff] = instructionLine[index];
			++indexBuff;
		}

		if (!isArgRegister(buffer)) // if the instruction is an argument 
		{
			if (buffer[0] == '0' && buffer[1] == 'x')
			{
				arg1 = atoi(buffer);
			}
			else
			{
				arg1 = (int8)strtol(buffer, NULL, 16);
			}
		}
		int24 instruc = createInstructionInteger(opcode, 2, arg0, arg1);
		fwrite((void*)&instruc, 3, 1, pBinaryExecutable);
	}
	else // if there's not
	{

	}

	if (pBinaryExecutable && isLastInstruction)
	{
		fclose(pBinaryExecutable);
		pBinaryExecutable = NULL;
	}
}

int8 convertStringToOpcode(const int8 * inBuffer)
{
	//TOOD: Convert checking to read from a text file and determine the correct instruction by a table
	assert(inBuffer);

	int8 buffer[250];
	memcpy_s(buffer, _countof(buffer), inBuffer, strlen(inBuffer) + 1);
	const int32 stringlength = (signed)strlen(buffer);

	for (int32 i = 0; i < stringlength; ++i)
	{
		buffer[i] = (int8)towlower(inBuffer[i]);
	}

	if (strcmp(buffer, "load") == true)
	{
		return OP_LOAD;
	}
	else if (strcmp(buffer, "store") == true)
	{
		return OP_STORE;
	}
	else if (strcmp(buffer, "add") == true)
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

int24 createInstructionInteger(uint8 opCode, int32 argCount, ...)
{
	assert(argCount >= 0);
	int24 instructionInt;
	instructionInt.val = 0;
	va_list list;

	va_start(list, argCount);
	instructionInt.val |= opCode;
	instructionInt.val = instructionInt.val << INSTRUCTION_SHIFT;

	switch (argCount)
	{
	case 1:
	{
		const int8 arg = va_arg(list, int8);
		instructionInt.val |= (arg << ARG0_SHIFT);
		break;
	}
	case 2:
	{
		const int8 arg0 = va_arg(list, int8);
		const int8 arg1 = va_arg(list, int8);

		instructionInt.val |= (arg0 << ARG0_SHIFT);
		instructionInt.val |= (arg1 << ARG1_SHIFT);
		break;
	}
	default:
		handleError("Too many args passed for instruction");
		pauseForReturnKey();
		break;
	}

	va_end(list);

	return instructionInt;
}
