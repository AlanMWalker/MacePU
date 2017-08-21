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

	pauseForReturnKey();

	return 0;
}
