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
#include <stdint.h>	
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>	

#include "include\OperationCodes.h"

#define REG_TOTAL 8
#define OP_SHIFT 24
#define OP_MASK 0xFF000000
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

static uint8_t Memory[16384]; // RAM 
static uint8_t* pinstructionLoaded = NULL;

struct cpuinfo //ip = reg7
{
	uint32_t regs[8]; // 8 int regs
	uint32_t currentInstructionOpcode;
} CPU;

void loadInstructionIntoMemory(uint8_t opcode, int32_t num, ...);

int main(void)
{
	bool isCPURunning = true;

	memset(Memory, 0, 8 * 2048); // clear memory 
	// Load instruc into memory

	loadInstructionIntoMemory(OP_LOAD, 2, 0, 0x18); // load into r0 0x18
	loadInstructionIntoMemory(OP_LOAD, 2, 1, 0x18); // load into r1 0x18
	loadInstructionIntoMemory(OP_ADD, 3, 0, 1, 0); // add r0 and r1 and store in r0 

	while (isCPURunning)
	{
		CPU.currentInstructionOpcode = *(uint32_t*)(Memory + CPU.regs[7]);
		switch (CPU.currentInstructionOpcode & (OP_MASK >> OP_SHIFT))
		{
		case OP_LOAD:
		{
			const int8_t registerIndex = (uint32_t)*((Memory + CPU.regs[7]) + 1);
			const int8_t loadValue = (uint32_t)*((Memory + CPU.regs[7]) + 2);

			CPU.regs[registerIndex] = loadValue;
			break;
		}
		case OP_STORE:
			printf("I am store\n");
			break;

		case OP_ADD:
		{
			const uint32_t dest = (uint32_t)*((Memory + CPU.regs[7]) + 1);
			const uint32_t r1 = (uint32_t)*((Memory + CPU.regs[7]) + 2);
			const uint32_t r2 = (uint32_t)*((Memory + CPU.regs[7]) + 3);
			CPU.regs[dest] = CPU.regs[r1] + CPU.regs[r2];

			break;
		}
		default: isCPURunning = false; break;
		}
		//Debug purposes
		int32_t i = 0;
		for (i; i < REG_TOTAL; ++i)
		{
			printf("Reg Value %d = %d\n", i, CPU.regs[i]);
		}
		printf("\n\n");

		CPU.regs[7] += 16; //inc instruc ptr
	}

	system("pause"); // wait for key press before close
	return 0;
}

void loadInstructionIntoMemory(uint8_t opcode, int32_t num, ...)
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
	int32_t i;

	for (i = 0; i < num; ++i)
	{
		pinstructionLoaded[i + 1] = va_arg(argsList, int8_t);
	}
	pinstructionLoaded += 16;
	va_end(argsList);
}
