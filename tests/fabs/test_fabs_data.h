#define NANN 0x7fbfffff
#define SNAN 0x7fa00000
#define NEG_SNAN 0xFFA00000
#define QNAN 0x7ffe0000
#define ZERO 0x00000000
#define NEG_ZERO 0x80000000
#define ONE 0x3F800000
#define NEG_ONE 0xBF800000
#define INFF 0x7F800000
#define NEG_INF 0xFF800000
#define HALF 0x3f000000

#include <fenv.h>
#include <stdbool.h>
#include<math.h>

struct __fabsf_conformance_test_data {
	uint32_t x, out;
	int32_t exception_flags;
};

/* Test cases to check for exceptions for the fabsf() routine. These test cases are not exhaustive */
static struct __fabsf_conformance_test_data libm_test_fabsf_conformance_data[] = {
	{NANN, NANN, 0},
	{SNAN, SNAN, 0},
	{NEG_SNAN, SNAN, 0},
	{QNAN, QNAN, 0},
	{ONE, ONE, 0}, 
	{NEG_ONE, ONE, 0}, 
	{NEG_INF, INFF, 0},
	{INFF, INFF, 0},
	{ZERO, NEG_ZERO, 0},
	{NEG_ZERO, ZERO, 0},	
};

