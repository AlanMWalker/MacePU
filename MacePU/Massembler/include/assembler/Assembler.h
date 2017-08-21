#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>

#include "DataTypes.h"
#include "OperationCodes.h"
#include "error\ErrorUtils.h"

#define MEMSET_RESET -52

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