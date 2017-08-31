#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>

#include "DataTypes.h"
#include "OperationCodes.h"
#include "error\ErrorUtils.h"
#include "FiletypeInformation.h"

#define MEMSET_RESET -52

#define DEBUG_ASSEMBLED 1 // Set 1 if need to debug assembled file
#define	MAX_PROGRAM_MEMORY_SIZE 0xFF << 16
/*
File structure -
5 bytes header - "S-MSM" (inc null terminator)
Instruction Codes until EOF marker
5 bytes footer - "B-MSM" (inc null terminator)
*/

typedef enum _AssemblerReturnCode
{
	Success = 0,
	UnableToCreateFile = -1,
	UnableToLoadFile = -2,
	ParsingError = -3
} AssemblerReturnCode;

//Non-parsed instruction line data structure
//TODO if functions associated with manipulating this structure grow beyond 2-3 then create separate translation unit.
typedef struct _instrLine
{
	char instructionLineString[100];
	int32 instructionLineNumber;
	bool isLastInstruction;
}  InstructionLine;

//Parsed instruction line data structure
typedef struct _pinstrLine
{
	int8 opcode;
	int8 arg0;
	int8 arg1;
	int8 argCount;
} ParsedLine;


typedef struct _instrMemory
{
	int8 instructionMemory[MAX_PROGRAM_MEMORY_SIZE];
	int16 numberOfInstructions;
} InstructionMemory;


AssemblerReturnCode assembleFile(const char* masmFileLoc);

bool isArgRegister(const int8* inBuffer);

bool isValidInstructionLine(const InstructionLine* instructionLine, int8 argCount); // Is the instruction line syntactically correct

int24 createInstructionInteger(ParsedLine* pParsedLIne);

#endif //!ASSEMBLER_H
