#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cpudefs\CPUMemory.h"

typedef enum _procInit
{
	Succes = 0,
	MemoryCorruption = -1,
	NoMSMFIlePassed = -2,
	BinaryTooLarge = -3,
	NullPtrException = -4,
	InvalidFileMarkers = -5
} ProcessorInitError;

typedef struct _cpuRegs
{
	uint8 gpr[GPR_COUNT]; // General Purpose Registers
	uint16 programCounter;
	uint16 endOfExecutableMarker;

} Registers;

ProcessorInitError initProcessor(int32 argc, int8* argv[]);

void runProcessor();

void deinitProcessor();

static void processorTick(Registers* reg);

bool isValidExecutableFileMarkers(FILE* pBinaryFile); // Are the headers and footers of the file valid?

#endif // !PROCESSOR_H
