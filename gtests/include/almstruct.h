#ifndef __ALMSTRUCT_H__
#define __ALMSTRUCT_H__

#include "defs.h"

using namespace ALM;
#define MAX_INPUT_RANGES 2

#define _ALIGN_FACTOR 256
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define THOUSAND (1000)
#define MILLION  (THOUSAND * THOUSAND)

#define NITER    (100 * THOUSAND)
#define NELEM     5000 

typedef struct {
	uint32_t in;
	uint32_t out;
	uint32_t exptdexpt;
	uint32_t in2;
	uint32_t in3;
}libm_test_special_data_f32;

typedef struct {
	uint64_t in;
	uint64_t out;
	uint64_t exptdexpt;
	uint64_t in2;
	uint64_t in3;
}libm_test_special_data_f64;

typedef struct {
  double min;
  double max;
  enum RangeType type;
} InputRange;

typedef struct {
  double max_ulp_err; /* ULP error */
  double ulp_threshold;
} InputData;

typedef struct {
	 uint32_t tstcnt; 
  char print[12][100];
} PrintTstRes;  

/*
 * The structure is filled with the command line arguments
 * Based on the Testtype,Floattype and FloatWidth,
 *                        testcases are decided
 */
typedef struct {
  FloatType ftype;
  FloatWidth fwidth;
  FloatQuantity fqty;

  TestType ttype;
  int verboseflag;
  uint32_t niter;  
  uint32_t count;
  InputRange range[MAX_INPUT_RANGES];
} InputParams;


/*
 * The structure is passed to the AccuTestFixtureFloat
 * or AccuTestFixtureDouble to SetUp function, to populate
 * the input values for the test fixture
 */
typedef struct {
  InputRange range[MAX_INPUT_RANGES];
  uint32_t count;
  int verboseflag;
  InputData *inpData;
  PrintTstRes *prttstres;
  uint32_t nargs;
} AccuParams;

/*
 * The structure is passed to the SpecTestFixtureFloat
 * or SpecTestFixtureDouble to SetUp function, to read the values
 * from the table and feed the values to the test fixture
 */
typedef struct {
  libm_test_special_data_f32 *data32;
  libm_test_special_data_f64 *data64;
  uint32_t countf;
  uint32_t countd;
  int verboseflag;
  PrintTstRes *prttstres;  
  uint32_t nargs;
} SpecParams;


int gtest_main(int argc, char **argv, InputParams *params);
int gbench_main(int argc, char **argv, InputParams *params);
#endif
