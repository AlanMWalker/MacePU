#include <stdio.h>	
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "assembler\Assembler.h"

static const char REGISTER_PREFIX = 'r';

static FILE* pFile = NULL;
static FILE* pBinaryExecutable = NULL;

enum MASMPArseError
{
	NoParseError = 0
};

AssemblerReturnCode assembleFile(const char * masmFileLoc)
{
	const int32 result = fopen_s(&pFile, masmFileLoc, "r");

	//Sanity checks on assembly file passed 
	assert(pFile != NULL);

	if (result != 0)
	{
		handleError("Assembly file not found!");
	}

	if (pBinaryExecutable == NULL)
	{
		fopen_s(&pBinaryExecutable, "assembled.msm", "wb+");
	}

	assert(pBinaryExecutable != NULL);

	if (pBinaryExecutable == NULL)
	{
		handleError("Failed to create binary format!");
		pauseForReturnKey();
		return UnableToCreateFile;
	}

	// Write binary header to the assembled file
	fwrite(FILE_HEADER, sizeof(int8), strlen(FILE_HEADER) + 1, pBinaryExecutable);

	InstructionLine instruction;
	int32 lineNumber = 0;

	while (!feof(pFile))
	{
		const int8* rtrn = fgets(instruction.instructionLineString, _countof(instruction.instructionLineString), pFile);
		instruction.instructionLineNumber = lineNumber;
		if (rtrn != NULL)
		{
			printf("%s\n", instruction.instructionLineString);
			instruction.isLastInstruction = false;
		}
		else
		{
			instruction.isLastInstruction = true;
		}
		convertLineToInstruction(&instruction);

		memset(instruction.instructionLineString, MEMSET_RESET, strlen(instruction.instructionLineString) + 1);
		++lineNumber;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	if (pBinaryExecutable != NULL)
	{
		fwrite(FILE_FOOTER, sizeof(int8), strlen(FILE_FOOTER) + 1, pBinaryExecutable);
		fclose(pBinaryExecutable);
		pBinaryExecutable = NULL;
	}

	return Success;
}

//TODO Change return MSMPArseError
void convertLineToInstruction(const InstructionLine* pInstructionLine)
{
	// Sanity checks on instruction line validity
	assert(pInstructionLine != NULL);
	assert(pInstructionLine->instructionLineString != NULL && pInstructionLine->instructionLineString >= 0);

	if (pInstructionLine == NULL)
	{
		return;
	}

	if (pInstructionLine->instructionLineString == NULL || pInstructionLine->instructionLineNumber < 0)
	{
		return;
	}

	static int8 buffer[100];
	const int32 StringLength = (signed)strlen(pInstructionLine->instructionLineString);
	int16 generatedInstruction = 0;
	int8* commaPos = NULL;
	int32 index;

	commaPos = strchr(pInstructionLine->instructionLineString, ','); // check if there's more than 1 arg

	if (commaPos != NULL) // if there's more than 1 argument
	{
		for (index = 0; index < (signed)strlen(pInstructionLine->instructionLineString); ++index)
		{
			if (pInstructionLine->instructionLineString[index] == ' ')
			{
				buffer[index] = '\0';
				++index;  // ignore space between args
				break;
			}
			buffer[index] = pInstructionLine->instructionLineString[index];
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
		for (index; index < (signed)strlen(pInstructionLine->instructionLineString); ++index)
		{
			if (pInstructionLine->instructionLineString[index] == *commaPos)
			{
				buffer[indexBuff] = '\0';
				break;
			}
			buffer[indexBuff] = pInstructionLine->instructionLineString[index];
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
		for (index; index < (signed)strlen(pInstructionLine->instructionLineString); ++index)
		{
			if (pInstructionLine->instructionLineString[index] == '\n')
			{
				buffer[indexBuff] = '\0';
				break;
			}
			buffer[indexBuff] = pInstructionLine->instructionLineString[index];
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

	if (strcmp(buffer, "load") == 0)
	{
		return OP_LOAD;
	}
	else if (strcmp(buffer, "store") == 0)
	{
		return OP_STORE;
	}
	else if (strcmp(buffer, "add") == 0)
	{
		return OP_ADD;
	}

	return -1;
}

bool isArgRegister(const int8 * inBuffer)
{
	const int8* ptr = strchr(inBuffer, REGISTER_PREFIX);
	return ptr != NULL;
}

//TODO Change return to MSMParseError
bool isValidInstructionLine(const InstructionLine* instructionLine, int8 argCount)
{
	assert(argCount >= 0 && argCount <= MAX_ARG_COUNT);

	if (argCount < 0 || argCount > MAX_ARG_COUNT)
	{
		return false;
	}


	const int32 StringLength = strlen(instructionLine->instructionLineString) + 1;
	int8* commaPos = NULL;
	
	//below is comma placement & argument validity check
	switch (argCount)
	{
	case 0:
	case 1:
	{
		commaPos = strchr(instructionLine->instructionLineString, ',');
		if (commaPos != NULL)
		{
			printf("Invalid instruction line! Comma found on line featuring %d argument operation (line number %d: )\n", argCount, instructionLine->instructionLineNumber);
			return false;
		}
		break;
	}
	case 2:
	{
		int32 commaCount = 0;
		for (int32 i = 0; i < StringLength; ++i)
		{
			if (instructionLine->instructionLineString[i] == ',')
			{
				++commaCount;
			}

			//if more than one comma wsa found whilst parsing the line
			if (commaCount > 1)
			{
				printf("Comma found at invalid position in instruction line (position: %d of line number %d)\n", (i + 1), instructionLine->instructionLineNumber);
			}
		}
		break;
	}
	default:
		return false;
	}

	return true;
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
		instructionInt.val |= arg1;
		break;
	}
	default:
		handleError("Too many args passed for instruction");
		pauseForReturnKey();
		break;
	}

	va_end(list);

#if DEBUG_ASSEMBLED
	printf("value of instruc line %d\n", instructionInt.val);
#endif

	return instructionInt;
}
