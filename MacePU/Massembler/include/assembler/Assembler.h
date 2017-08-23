#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>

#include "DataTypes.h"
#include "OperationCodes.h"
#include "error\ErrorUtils.h"

#define MEMSET_RESET -52

#define FILE_HEADER "S-MSM"
#define FILE_FOOTER "E-MSM"

#ifdef _DEBUG
#define DEBUG_ASSEMBLED 1 // Set 1 if need to debug assembled file
#endif

/*
File structure -
5 bytes header - "S-MSM" (inc null terminator)
Instruction Codes until EOF marker
5 bytes footer - "B-MSM" (inc null terminator)
*/

typedef enum _AssemblerReturnCode
{
	Success = 0,
	UnableToCreateFile = -1
} AssemblerReturnCode;

AssemblerReturnCode assembleFile(const char* masmFileLoc);

void convertLineToInstruction(const int8* instructionLine, bool isLastInstruction);

int8 convertStringToOpcode(const int8* inBuffer);

bool isArgRegister(const int8* inBuffer);

int24 createInstructionInteger(uint8 opCode, int32 argCount, ...);

#endif //!ASSEMBLER_H
