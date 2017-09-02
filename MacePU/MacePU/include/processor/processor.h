#ifndef PROCESSOR_H
#define PROCESSOR_H

typedef enum _procInit
{
	Succes = 0,
	MemoryCorruption = -1,
	NoMSMFIlePassed = -2,
	BinaryTooLarge = -3,
	NullPtrException = -4
} ProcessorInitError;


ProcessorInitError initProcessor(int32 argc, int8* argv[]);

void runProcessor();

void deinitProcessor();

static void processorTick();

#endif // !PROCESSOR_H
