#ifndef OPERATION_CODES_H
#define OPERATION_CODES_H

#define OP_NOP 0x00 // no op
#define OP_LOAD 0x01 // Register, Value
#define OP_STORE 0x02 // Dest, Register
#define OP_ADD 0x03 // A1, A2

#define ADDRESS_BUS_LENGTH_BITS 24
#define ADDRESS_BUS_LENGTH_BYTES 3

#define ARG_SIZE_BYTES 1
#define ARG_SIZE_BITS 8

#define OPCODE_LEGTH_BYTES 1
#define OPCODE_LENGTH_BITS 8

#define OPCODE_SHIFT 16
#define ARG0_SHIFT 8

#define OPCODE_MASK 0xFF0000
#define ARG0_MASK 0x00FF00
#define ARG1_MASK 0x0000FF


#define MAX_ARG_COUNT 2

#endif // !OPERATION_CODES_H
