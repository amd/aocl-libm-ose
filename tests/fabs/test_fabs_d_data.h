#define NANN 0x7FF07FF7FDEDFFFF
#define NEG_NAN 0xFFF07FF7FDEDFFFF
#define SNAN 0x7FF07FF7FDEDFFFF
#define NEG_SNAN 0xFFF07FF7FDEDFFFF
#define QNAN 0x7FF87FF7FDEDFFFF
#define NEG_QNAN 0xFFF87FF7FDEDFFFF
#define ZERO 0x0000000000000000
#define NEG_ZERO 0x8000000000000000
#define ONE 0x3FF0000000000000
#define NEG_ONE 0xBFF0000000000000
#define INFF 0x7FF0000000000000
#define NEG_INF 0xFFF0000000000000

#include <fenv.h>
#include <stdbool.h>
#include<math.h>

struct __fabs_conformance_test_data {
	uint64_t x, out;
	int32_t exception_flags;
};

/* Test cases to check for exceptions for the fabs() routine. These test cases are not exhaustive */
static struct __fabs_conformance_test_data libm_test_fabs_conformance_data[] = {
	{NANN, NANN, 0},
	{SNAN, SNAN, 0},
	{QNAN, QNAN, 0},
	{NEG_SNAN, SNAN, 0},
	{NEG_QNAN, QNAN, 0},
	{ONE, ONE, 0}, 
	{NEG_ONE, ONE, 0}, 
	{NEG_INF, INFF, 0},
	{INFF, INFF, 0},
	{ZERO, ZERO, 0},
	{NEG_ZERO, ZERO, 0},	
};
