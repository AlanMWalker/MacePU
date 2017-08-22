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

#define MASM_FILE_EXT ".masm" // assembly file text format 
#define CPU_BINARY_FORMAT ".msm"  // assembly binary executable format

int main(int argc, int8* argv[])
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
		pauseForReturnKey();
		return -1;
	}

	AssemblerReturnCode code = assembleFile(argv[1]);
	switch (code)
	{
	case UnableToCreateFile:
		handleError("Unable to create the msm binary file!");
		break;
	default:
		break;
	}

#ifdef _DEBUG

	FILE* file = NULL;
	errno_t result = fopen_s(&file, "assembled.msm", "rb");

	if (result != 0)
	{
		handleError("--DEBUG ERROR -- Failed to open binary assembled file");
		pauseForReturnKey();
		return -1;
	}
	unsigned char tempBuff[4];

	fread_s(tempBuff, 20, 3, 1, file);

	printf("Opcode = %d, Arg0 = %d, Arg1 = %d\n", tempBuff[3], tempBuff[2], tempBuff[1]);
	int32 val = 0;

	val |= (tempBuff[2] << INSTRUCTION_SHIFT);
	val |= (tempBuff[1] << ARG0_SHIFT);
	val |= (tempBuff[0] << ARG1_SHIFT);
	
	printf("%d\n", val);

	if (file != NULL)
	{
		fclose(file);
		file = NULL;
	}
#endif 
	pauseForReturnKey();

	return 0;
}
