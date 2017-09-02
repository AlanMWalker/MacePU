/*
Project: Macpu & Massembly Assembler
Lead: Alan M. Walker
Language: C
IDE: Visual Studio 2015
OS tested on: Windows 10

Disclaimer:

I am no expert in CPU architecture nor in the standard practices and coding standards of the C programming language. I am, however, a C++ programmer who specialises in creating games.
This project is to assist me in understanding more of the low level operations that occur in a CPU and to better understand how to create efficient code using low level understandings.
I am perfectly happy for anyone, who so wishes, to take the project further. If this project assists you as a learning aide then it did for you what I'm hoping it does for me.

Thanks
P.S Check the Git readme for links I used for research.

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>	
#include <cassert>
#include <stdlib.h>

#include <vld.h> // visual leak detector

#include "include\cpudefs\OperationCodes.h"
#include "include\cpudefs\DataTypes.h"
#include "processor\processor.h"

#define REG_TOTAL 8
#define MAX_ARGS 3
#define IP 7


// Basic CPU simulation featuring assembler for the project
// 8 * 8bit CPU registers  
// R7 is Program counter 
// All operations performed in registers
// 16 bit instruction addressing (upper byte is for opcode, lower bytes are args) 

//TODO: 
// - Perform checks to see if the program has reached the end of the memory pool 
// - Create a way of seperating program data from program instructions 
// - Create an assembler project
// - Create a stack 
// - Define more instructions for inst set 

static uint8 Memory[16384]; // RAM 
static uint8* pinstructionLoaded = NULL;

typedef struct _cpuinfo //ip = reg7
{
	uint8 regs[8]; // 8 int regs
	uint24 instrOpCode;
} CPU;

//void loadInstructionIntoMemory(uint8 opcode, int32 num, ...);

int main(int32 argc, int8* argv[])
{
	const ProcessorInitError procInitResult = initProcessor(argc, argv);
	if (procInitResult != Succes)
	{
		printf("Failed to create processor! Result: %d", procInitResult);
		system("pause");
		return procInitResult;
	}

	runProcessor();

	deinitProcessor();

	system("pause");
	return 0;
}
