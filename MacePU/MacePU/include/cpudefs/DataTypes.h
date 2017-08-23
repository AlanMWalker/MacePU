#ifndef DATA_TYPES_H
#define DATA_TYPES_H

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;

typedef struct _uint24
{
	unsigned int val : 24;
} uint24;

typedef struct _int24
{
	signed int val : 24;
} int24;

#endif // !DATA_TYPES_H
