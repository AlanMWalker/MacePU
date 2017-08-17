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

#define REG_TOTAL 8

#define OP_LOAD 0x01 
#define OP_STORE 0x02 
#define OP_ADD 0x03

#define OP_SHIFT 24
#define OP_MASK 0xFF000000

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
static uint8_t Memory[8 * 2048]; // RAM 

struct cpuinfo //ip = reg7
{
	uint32_t regs[8]; // 8 int regs
	uint32_t currentInstructionOpcode;
} CPU;

int main(void)
{
	bool isCPURunning = true;

	memset(Memory, 0, 8 * 2048);
	// Load instruc into memory

	Memory[0] = OP_ADD;

	while (1)
	{
		CPU.currentInstructionOpcode = *(uint32_t*)(Memory + CPU.regs[7]);
		switch (CPU.currentInstructionOpcode & (OP_MASK >> OP_SHIFT))
		{
		case OP_LOAD:
			printf("I am load");
			break;

		case OP_STORE:
			printf("I am sub\n");
			break;

		case OP_ADD:
			printf("I am add\n");
			break;
		default:
		{
			break; break;
		}
		}
		//Debug purposes
		int32_t i = 0;
		for (i; i < REG_TOTAL; ++i)
		{
			printf("Reg Value %d = %d\n", i, CPU.regs[i]);
		}
		printf("\n\n");

		++CPU.regs[7]; //inc instruc ptr
	}

	return 0;
}