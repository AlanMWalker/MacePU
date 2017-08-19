#include <stdio.h>	
#include <string.h>
#include <stdlib.h>

#include "..\MacePU\include\OperationCodes.h"
#include "..\MacePU\include\DataTypes.h"

#define MASM_FILE_EXT ".masm"

void pauseForReturnKey()
{
	printf("Hit return to close...\n");
	scanf_s("");
}

void handleError(const char* failedString)
{
	printf(failedString);
	printf("\n");
	pauseForReturnKey();
}

void convertLineToInstruction(char* instructionLine);

int main(int argc, char* argv[])
{
	char instrLine[256];

	uint24 i;
	i.val = OP_STORE;

	i.val = i.val << INSTRUCTION_SHIFT;

	i.val |= (1 << ARG0_SHIFT);
	i.val |= (2 << ARG1_SHIFT);

	//OP_MASK >> OP_SHIFT

	int8 opcode = (i.val & INSTRUCTION_MASK) >> INSTRUCTION_SHIFT;

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

	if (result != 0)
	{
		handleError("Assembly file not found!");
	}

	while (!feof(pFile))
	{
		char* rtrn = fgets(instrLine, _countof(instrLine), pFile);
		if (rtrn != NULL)
		{
			printf("%s\n", instrLine);
			convertLineToInstruction(instrLine);
		}
	}

	fclose(pFile);
	pauseForReturnKey();

	return 0;
}

void convertLineToInstruction(char * instructionLine)
{
	if (instructionLine == NULL)
	{
		return;
	}

	const int32 StringLength = strlen(instructionLine);
	int16 generatedInstruction = 0;


}
