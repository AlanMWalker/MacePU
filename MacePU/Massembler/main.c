#include <stdio.h>	
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "..\MacePU\include\OperationCodes.h"

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

	if (argc < 2)
	{
		handleError("No massembly file passed as argument!");

		return -1;
	}

	int32_t argsIndex;

	for (argsIndex = 0; argsIndex < argc; ++argsIndex)
	{
		if (strstr(argv[argsIndex], MASM_FILE_EXT))
		{
			break;
		}
	}

	FILE* pFile = NULL;
	int32_t result = fopen_s(&pFile, argv[argsIndex], "r");
	
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
	const int32_t StringLength = strlen(instructionLine);
}
