#ifndef OPERATION_CODES_H
#define OPERATION_CODES_H

#define OP_NOP 0x00 // no op
#define OP_LOAD 0x01 // Register, Value
#define OP_STORE 0x02 // Dest, Register
#define OP_ADD 0x03 // A1, A2

#define INSTRUCTION_LENGTH_BITS 24
#define INSTRUCTION_LENGTH_BYTES 3

#define INSTRUCTION_SHIFT 16
#define ARG0_SHIFT 8
#define ARG1_SHIFT 4

#define INSTRUCTION_MASK 0xFF0000
#define ARG0_MASK 0x00FF00
#define ARG1_MASK 0x0000FF

#define MAX_ARG_COUNT 2

typedef struct _uint24
{
	unsigned int val : 24;
} uint24;

typedef struct _int24
{
	signed int val : 24;
} int24;
#endif // !OPERATION_CODES_H
