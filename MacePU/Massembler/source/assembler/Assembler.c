#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cassert>
#include <ctype.h>
#include <string.h>

#include "assembler\Assembler.h"

#define STRING_MATCH 0
#define INVALID_INSTRUCTION_LINE 0xFF << OPCODE_SHIFT

static const char REGISTER_PREFIX = 'r';

static FILE* pFile = NULL;
static FILE* pBinaryExecutable = NULL;

static InstructionMemory programMemory;

typedef enum _masmparseError
{
	NoParseError = 0,
	OpcodeNotFound = -1,
	InvalidInstructionLine = -2,
	ProgramMemoryOverflow = -3

} MASMParseError;

static MASMParseError convertStringToOpcode(const InstructionLine* pinstLine, ParsedLine* pParsedLine);
MASMParseError convertLineToInstruction(const InstructionLine* pInstructionLine);

AssemblerReturnCode assembleFile(const char * masmFileLoc)
{
	const int32 result = fopen_s(&pFile, masmFileLoc, "r");

	programMemory.numberOfInstructions = 0;

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
	MASMParseError lineParseResult = NoParseError;

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

		lineParseResult = convertLineToInstruction(&instruction);
		if (lineParseResult != NoParseError)
		{
			break;
		}
		memset(instruction.instructionLineString, MEMSET_RESET, strlen(instruction.instructionLineString) + 1);
		++lineNumber;
	}

	if (lineParseResult != NoParseError)
	{
		return ParsingError;
	}

	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}

	//write the number of instructions to a file
	fwrite(&programMemory.numberOfInstructions, sizeof(int16), 1, pBinaryExecutable);

	const int8 increment = ADDRESS_BUS_LENGTH_BYTES;

	int8* pMemoryBlock = programMemory.instructionMemory;

	for (; pMemoryBlock != NULL; pMemoryBlock += increment)
	{
		const ptrdiff_t index = (pMemoryBlock - programMemory.instructionMemory);
		if (index > programMemory.numberOfInstructions * ADDRESS_BUS_LENGTH_BYTES || index + 1 == MAX_PROGRAM_MEMORY_SIZE)
		{
			break;
		}
		fwrite(pMemoryBlock, ADDRESS_BUS_LENGTH_BYTES, 1, pBinaryExecutable);
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
MASMParseError convertLineToInstruction(const InstructionLine* pInstructionLine)
{
	// Sanity checks on instruction line validity
	assert(pInstructionLine != NULL);
	assert(pInstructionLine->instructionLineString != NULL && pInstructionLine->instructionLineString >= 0);

	if (pInstructionLine == NULL)
	{
		return InvalidInstructionLine;
	}

	if (pInstructionLine->instructionLineString == NULL || pInstructionLine->instructionLineNumber < 0)
	{
		return InvalidInstructionLine;
	}

	static int8 buffer[100];
	const int32 StringLength = (signed)strlen(pInstructionLine->instructionLineString);
	ParsedLine parsedInstructionLine = { 0 , 0 , 0, 0 };
	int16 generatedInstruction = 0;
	int8* commaPos = NULL;
	int32 index;

	commaPos = strchr(pInstructionLine->instructionLineString, ','); // check if there's more than 1 arg
	if (programMemory.numberOfInstructions == MAX_PROGRAM_MEMORY_SIZE)
	{
		return ProgramMemoryOverflow;
	}

	if (commaPos != NULL) // if there's more than 1 argument
	{
		parsedInstructionLine.argCount = 2;
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

		MASMParseError opcode = convertStringToOpcode(pInstructionLine, &parsedInstructionLine);

		if (opcode != NoParseError)
		{
			handleError("Error when finding opcode!");
			pauseForReturnKey();
			return opcode;
		}

		//TODO insert opcode arg validity check here!

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

		if (isArgRegister(buffer)) // if the instruction is an argument 
		{
			parsedInstructionLine.arg0 = atoi(&buffer[1]);
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
				parsedInstructionLine.arg1 = atoi(buffer);
			}
			else
			{
				parsedInstructionLine.arg1 = (int8)strtol(buffer, NULL, 16);
			}
		}
	}
	else // if there's not
	{

	}

	//TODO re-write to make the instruction be written into array by createInstructionInteger
	int24 instruc = createInstructionInteger(&parsedInstructionLine);

	//Temp conversion from in24 to instruc
	//write in little endian form
	int16 indexInArray = programMemory.numberOfInstructions * ADDRESS_BUS_LENGTH_BYTES;
	programMemory.instructionMemory[indexInArray] = (instruc.val & ARG1_MASK);
	programMemory.instructionMemory[indexInArray + 1] = (instruc.val & ARG0_MASK) >> ARG0_SHIFT;
	programMemory.instructionMemory[indexInArray + 2] = (instruc.val & OPCODE_MASK) >> OPCODE_SHIFT;

	//fwrite((void*)&instruc, 3, 1, pBinaryExecutable);

	++programMemory.numberOfInstructions;

	return NoParseError;
}

static MASMParseError convertStringToOpcode(const InstructionLine* pinstLine, ParsedLine* pParsedLine)
{
	//TOOD: Convert checking to read from a text file and determine the correct instruction by a table
	assert(pinstLine);
	assert(pParsedLine);
	assert(pinstLine->instructionLineString);

	if (pinstLine == NULL || pParsedLine == NULL || pinstLine->instructionLineString == NULL)
	{
		printf("Nullptr found when parsing opcode\n");
	}

	// Extract the operation name 
	int8 buffer[100];
	memcpy_s(buffer, _countof(buffer), pinstLine->instructionLineString, strlen(pinstLine->instructionLineString) + 1);

	const int32 stringlength = (signed)strlen(buffer);
	const int8* EndOfOpcode = strchr(buffer, ' '); // find the first space in the instruction line as it will indicate the end position of the opcode stubstring

	//if we couldn't find the space character the line is invalid, so return opcode not found 
	//TODO further syntactic analysis here is possible in future
	if (EndOfOpcode == NULL)
	{
		return OpcodeNotFound;
	}

	int8* nullTerm = buffer;

	for (; nullTerm != NULL; ++nullTerm)
	{
		if (nullTerm == EndOfOpcode)
		{// if we reach the end of the string, end
			*nullTerm = '\0';
			break;
		}
		*nullTerm = (int8)tolower(*nullTerm);
	}

	//TODO convert if-block to a lookup table selection
	if (strcmp(buffer, "load") == STRING_MATCH)
	{
		pParsedLine->opcode = OP_LOAD;
	}
	else if (strcmp(buffer, "store") == STRING_MATCH)
	{
		pParsedLine->opcode = OP_STORE;
	}
	else if (strcmp(buffer, "add") == STRING_MATCH)
	{
		pParsedLine->opcode = OP_ADD;
	}
	else
	{
		return OpcodeNotFound;
	}

	return NoParseError;
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

int24 createInstructionInteger(ParsedLine* pParsedLIne)
{
	int24 instructionInt;
	instructionInt.val = 0;
	assert(pParsedLIne != NULL);
	if (pParsedLIne == NULL)
	{
		instructionInt.val = INVALID_INSTRUCTION_LINE;
		return instructionInt;
	}

	instructionInt.val |= pParsedLIne->opcode;
	instructionInt.val = instructionInt.val << OPCODE_SHIFT;

	switch (pParsedLIne->argCount)
	{
	case 1:
	{
		//const int8 arg = va_arg(list, int8);
		instructionInt.val |= (pParsedLIne->arg0 << ARG0_SHIFT);
		break;
	}
	case 2:
	{
		instructionInt.val |= (pParsedLIne->arg0 << ARG0_SHIFT);
		instructionInt.val |= pParsedLIne->arg1;
		break;
	}
	default:
		handleError("Too many or too few args passed for instruction");
		pauseForReturnKey();
		break;
	}


#if DEBUG_ASSEMBLED
	printf("value of instruc line %d\n", instructionInt.val);
#endif

	return instructionInt;
}
