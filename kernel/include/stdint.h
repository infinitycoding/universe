#ifndef	_stdint_h_
#define	_stdint_h_

#define TRUE		1
#define true		1
#define FALSE		0
#define false		0
#define NULL 0

typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned int      uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef unsigned int uintptr_t;
typedef long int intptr_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

typedef int bool;
typedef int boolean;


#endif