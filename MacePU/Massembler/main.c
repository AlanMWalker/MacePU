#include <stdio.h>	
#include <stdlib.h>
#include <cassert>
#include <string.h>

#include <vld.h> // visual leak detector

#include "OperationCodes.h"
#include "DataTypes.h"

#include "assembler\Assembler.h"
#include "error\ErrorUtils.h"

//TODO: Convert MASM into a binary format that can be loaded into memory by the CPU then executed
//TODO: Allow user to specify the filename of the assembled binary format
//TODO: Create option to save file to directory of users choice (via command line arg)

int main(int32 argc, int8* argv[])
{
	if (argc < 2)
	{
		handleError("No massembly file passed as argument!");

		return -1;
	}

	int32 argsIndex;
	bool extensionFound = false;
	for (argsIndex = 0; argsIndex < argc && !extensionFound; ++argsIndex)
	{
		if (strstr(argv[argsIndex], MASM_FILE_EXT))
		{
			extensionFound = true;
		}
	}

	if (!extensionFound)
	{
		handleError("Incorrect file extension for Massembler!");
		return -1;
	}

	AssemblerReturnCode code = assembleFile(argv[1]);

	if (code != 0)
	{
		switch (code)
		{
		case UnableToCreateFile:
			handleError("Unable to create the msm binary file!");
			break;
		default:
			break;
		}
		return code;
	}

#if DEBUG_ASSEMBLED
	printf("\n\nPrint assembled file debug\n");

	FILE* file = NULL;
	errno_t result = fopen_s(&file, "assembled.msm", "rb");

	if (result != 0)
	{
		handleError("--DEBUG ERROR -- Failed to open binary assembled file");
		return -1;
	}

	uint8 tempBuff[50];

	// Read Header
	const uint32 readBytes = fread_s(tempBuff, _countof(tempBuff), sizeof(int8), strlen(FILE_HEADER) + 1, file);

	if (strcmp(tempBuff, FILE_HEADER) != 0)
	{
		handleError("--DEBUG ERROR -- Incorrect file header when debug");
		return -1;
	}

	int16 numberOfInstructions = 0;
	fread_s(&numberOfInstructions, sizeof(int16), sizeof(int16), 1, file);

	for (int16 i = 0; i < numberOfInstructions; ++i)
	{
		int24 readLine = { 0 };
		fread_s(tempBuff, 20, 3, 1, file);
		readLine.val |= (tempBuff[2] << OPCODE_SHIFT);
		readLine.val |= (tempBuff[1] << ARG0_SHIFT);
		readLine.val |= tempBuff[0];
		printf("Read in line integer value is : %d\n", readLine.val);
	}
#endif 

	pauseForReturnKey();
	system("pause");
	return 0;
}
