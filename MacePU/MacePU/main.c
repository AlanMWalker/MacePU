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
		printf("Failed to create processor! Result: ", procInitResult);
		return procInitResult;
	}

	runProcessor();
	
	deinitProcessor();

	system("pause");
	return 0;

	/*
	CPU processor;

	bool isCPURunning = true;

	memset(Memory, 0, 8 * 2048); // clear memory
	// Load instruc into memory

	loadInstructionIntoMemory(OP_LOAD, 2, 0, 0x18); // load into r0 0x18
	loadInstructionIntoMemory(OP_LOAD, 2, 1, 0x18); // load into r1 0x18
	loadInstructionIntoMemory(OP_ADD, 3, 0, 1, 0); // add r0 and r1 and store in r0

	while (isCPURunning)
	{
		processor.instrOpCode.val = *(uint32*)(Memory + processor.regs[7]);
		switch (processor.instrOpCode.val & (OPCODE_MASK >> OPCODE_SHIFT))
		{
		case OP_LOAD:
		{
			const int8 registerIndex = (uint32)*((Memory + processor.regs[7]) + 1);
			const int8 loadValue = (uint32)*((Memory + processor.regs[7]) + 2);

			processor.regs[registerIndex] = loadValue;
			break;
		}
		case OP_STORE:
			printf("I am store\n");
			break;

		case OP_ADD:
		{
			const uint32 dest = (uint32)*((Memory + processor.regs[7]) + 1);
			const uint32 r1 = (uint32)*((Memory + processor.regs[7]) + 2);
			const uint32 r2 = (uint32)*((Memory + processor.regs[7]) + 3);
			processor.regs[dest] = processor.regs[r1] + processor.regs[r2];

			break;
		}
		default: isCPURunning = false; break;
		}
		//Debug purposes
		int32 i = 0;
		for (i; i < REG_TOTAL; ++i)
		{
			printf("Reg Value %d = %d\n", i, processor.regs[i]);
		}
		printf("\n\n");

		processor.regs[7] += 16; //inc instruc ptr
	}

	printf("Hit return to close...\n");
	scanf_s("");
	*/
	return 0;
}

void loadInstructionIntoMemory(uint8 opcode, int32 num, ...)
{
	if (num > MAX_ARGS)
	{
		return;
	}

	if (pinstructionLoaded == NULL)
	{
		pinstructionLoaded = Memory;
	}

	va_list argsList;

	va_start(argsList, num);
	pinstructionLoaded[0] = opcode;
	int32 i;

	for (i = 0; i < num; ++i)
	{
		pinstructionLoaded[i + 1] = va_arg(argsList, int8);
	}

	pinstructionLoaded += 16;
	va_end(argsList);

}
