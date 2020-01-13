#ifndef __TEST_LOG_DATA_H__
#define __TEST_LOG_DATA_H__

#ifndef __TEST_LOG_INTERNAL__
#warning "This file is not expected to be included generously"
#endif
#include <fenv.h>
#include <libm_tests.h>

/* Test cases to check for exceptions for the log() routine. These test cases are not exhaustive */
static struct {
	uint64_t in, out, except;
} test_log_conformance_data[] = {
	{0x0000000000000000, 0xfff0000000000000, 4},	//0 -inf
	{0x8000000000000000, 0xfff0000000000000, 4}, 	// -0  -inf
	{0x3ff0000000000000, 0x0000000000000000, 0},	//1, 0
	{0xbff0000000000000, 0x7ff8000000000000, FE_INVALID},   //-1 nan
	{0x7FF0000000000000, 0x7ff0000000000000, 0}, 	//inf, inf
	{0xfff0000000000000, 0x7ff8000000000000, FE_INVALID},	//-inf, nan
	{0x7ff87ff7fdedffff, 0x7ff87ff7fdedffff, 0},		//nan, nan
	{0x7ff07ff7fdedffff, 0x7ff87ff7fdedffff, FE_INVALID},	//snan, nan
	{0xfff07ff7fdedffff, 0xfff87ff7fdedffff, FE_INVALID},	//-nan, -nan
	{0x7ff87ff7fdedffff, 0x7ff87ff7fdedffff, 0},		//qnan, nan
};

/* Test cases to check for exceptions for the logf() routine. These test cases are not exhaustive */
static struct {
	uint32_t in, out, except;
} test_logf_conformance_data[] = {
	{0x00000000, 0xFF800000, 4},	//log(0) is -inf
	{0x3f800000, 0x00000000, 0},	//1, 0
	{0x80000000, 0xff800000, 4},	//-0, -inf
	{0xbf800000, 0xffc00000, FE_INVALID}, 	//-1, -nan
	{0x7F800000, 0x7f800000, 0},	//inf, inf
	{0xff800000, 0xffc00000, FE_INVALID}, 	//-inf, -nan
	{0x7fbfffff, 0x7fffffff, FE_INVALID}, 	//nan nan
	{0x7fa00000, 0x7fe00000, FE_INVALID}, 	//snan, nan
	{0xffa00000, 0xffe00000, FE_INVALID}, 	//-nan, -nan
	{0x7ffe0000, 0x7ffe0000, 0}, 		//qnan, nan
};

/* Scalar log in AMD LibM handles values between e^(-1/16) and e^(1/16) as special case, while vector versions don't.
   Hence, some values in this range is added as special cases
*/

static struct {
	uint32_t in, out;
} test_logf_special_data[] = {
    {0x3c000000,0xc09b43d5  },  //0.0078125
    {0x3f012345,0xbf2f2e21  },  //0.504444
    {0x3F87AE14,0x3d6eab52  },  //1.06
    {0x00000000,0xff800000},  //0.0
    {0x3c7fffff,0xc0851592  },  //0.0156249991
    {0x3f800000,0x00000000  },  //1
    {0x40000000,0x3f317218  },  //2
    {0x33d6bf95,0xc180f1dc  },  //1.0000000e-7
    {0x4048f5c3,0x3f928683  },  //pi  */
    {0x40c90fdb,0x3feb3f8e  },  //2pi
    {0x41200000,0x40135d8e  },  //10
    {0x447a0000,0x40dd0c55  },  //1000
    {0x42800000,0x40851592  },  //64
    {0x42af0000,0x408f17aa  },  //87.5
    {0x42b00000,0x408f4658  },  //88
    {0x42c00000,0x40920f24  },  //96
    {0xc2af0000,0x7fc00000 },  //-87.5
    {0xc2e00000,0x7fc00000 },  //-112
    {0xc3000000,0x7fc00000 },  //-128
    {0xc2aeac4f,0x7fc00000 },  //-87.3365  smallest normal result
    {0xc2aeac50,0x7fc00000 },  //-87.3365  largest denormal result
    {0xc2ce0000,0x7fc00000 },  //-103
    {0x42b17216,0x408f895b  },  //88.7228   largest value  --
    {0x42b17217,0x408f895b  },  //88.7228   largest value
    {0x42b17218,0x408f895c  },  //88.7228   overflow
    {0x50000000,0x41b6fda9  },  //large   overflow
    {0xc20a1eb8,0x7fc00000 }, // -34.53
    {0xc6de66b6,0x7fc00000 }, // -28467.3555
    {0xbe99999a,0x7fc00000 }, // -0.3
    {0xbf247208,0x7fc00000 }, // -0.642365
    {0xbf000000,0x7fc00000 }, // -0.5
    {0x3e99999a,0xbf9a1bc8  }, // 0.3
    {0x3f247208,0xbee29c4a  }, // 0.642365
    {0x3f000000,0xbf317218  }, // 0.5
    {0x420a1eb8,0x4062ad51  }, // 34.53
    {0x46de66b6,0x41241aae  }, // 28467.3555
    {0xc2c80000,0x7fc00000 }, // -100
    {0x42c80000,0x40935d8e  }, // 100
};

static struct {
	uint64_t in, out;
} test_log_special_data[] = {
    {0x3fee1177d9f91c22,0xbfafe1687bdb1469}, // 0.93963234494083658354668
    {0x3ff0008078a074ad,0x3f200ed397156a4a}, // 1.00012251968227450227289
    {0x3fee1b973ae0e299,0xbfaf3527a491b3b3}, // 0.94086801051806212559114
    {0x3fee91075ba4fb7c,0xbfa7771db911f95b}, // 0.95520370387372066645071
    {0x3fef6da08661d8f4,0xbf9276491033af3b}, // 0.98213220831419656420280
    {0x3fefdaa8ad5104b9,0xbf72b6968fc30537}, // 0.99544175959467573466810
    {0x3fefff2f8d7f5d39,0xbf1a0ea4f2eb8f62}, // 0.99990060459359775268950
    {0x3ff00281d1da414b,0x3f440cfcb4e691a9}, // 1.00061208699851778014533
    {0xc090000000000000,0xfff8000000000000}, // -1024.0
    {0xbff0000000000000,0xfff8000000000000}, // -1.0
    {0x8000000000000000,0xfff8000000000000}, // -0.0
    {0x0000000000000000,0x7ff8000000000000}, // +0.0
    {0xc090000000000000,0x7ff8000000000000}, // +1024
    {0x3ff0000000000000,0x0000000000000000}, // +1.0
    {0x7ff0000000000000,0x7ff0000000000000},
    {0xfff0000000000000,0x7FF8000000000000},

    {0x3ff0097d13350953,0x3f62f4881b43f2d5},
    {0x0003ec71c47096d5,0xc0862e6a7c4872d2},
    {0x000fffffffffffff,0xc086232bdd7abcd2},
    {0x3ff1000000000000,0x3faf0a30c01162a6}, // 1.0625
    {0x3ff0ffffffffffff,0x3faf0a30c0116288}, // 1.0624999999999998
    {0x3fee000000000000,0xbfb08598b59e3a07}, // 0.9375
    {0x3fee000000000001,0xbfb08598b59e39fe}, // 0.93750000000000011
    {0x3ff199999999999a,0x3fb8663f793c46cc},
    {0x3ff1082b577d34ec,0x3fafffffffffffd3},
    {0x3ff2216045b6f5cc,0x3fbffffffffffff2},
    {0x3c9b4626891a283b,0xc04272c3d7f0cfea},  // 9.4625784658987686E-17
    {0x3ea2345689904135,0xc02cdaa189f4f9f9},  // 5.4253475234754763E-07
    {0x410ca5f86198d046,0x4028bb656d0a5205},  // 234687.04765475
    {0x40f422117627d4d2,0x4026a3e81ffaa69e},  // 82465.091346579866
    {0x3FF028F5C28F5C29,0x3F8460D6CCCA367C},  //1.01
    {0x3ff0000000000000,0x0000000000000000},  //1.0
    {0x3ff4000000000000,0x3FCC8FF7C79A9A22},  //1.25
    {0x4005BF0A8B145769,0x3ff0000000000000},  //e
    {0x3ff8000000000000,0x3FD9F323ECBF984C},  //1.5
    {0xbff0000000000000,0x7FF8000000000000},  //-1.0
    {0xfff0000000000000,0x7FF8000000000000},  //-inf
    {0xfff8000000000000,0xfff8000000000000},  //Qnan
    {0xfff4000000000000,0xfffc000000000000},  //Snan
    {0x7ff0000000000000,0x7ff0000000000000},  //+inf
    {0x7ff8000000000000,0x7ff8000000000000},  //Qnan
    {0x7ff4000000000000,0x7ffc000000000000},  //Snan
    {0x0000000000000000,0xfff0000000000000},  //+0.0
    {0x8000000000000000,0xfff0000000000000},  //-0.0
    {0x3FF0F5C28F5C28F6,0x3FADD56C1D883EE6},  //1.06
    {0x3FEE147AE147AE14,0xBFAFAE2206CABE40},  //.94
    {0x3FF028F5C28F5C29,0x3F8460D6CCCA367C},  //1.01
    {0x3FEFAE147AE147AE,0xBF8495453E6FD4BC},  //.99
    {0x3fe0000000000000,0xBFE62E42FEFA39EF},  //.5
    {0x4310005D38118000,0x4041542740EB8CF2},  //1.126e+015
    {0x4024000000000000,0x40026BB1BBB55516},  //10
    {0x40157AE147AE147B,0x3FFAE4ABCE14D015},  //5.37
    {0x3FF02015E8B29697,0x3F7FF5E7947206C3},  // 1.0078333940267947 max ulp error
    {0x3ff0000000000001,0x3cafffffffffffff},
    {0x7ff0000000000000,0x7ff0000000000000},
    {0x7ff0000000000000,0x7ff0000000000000},
};

#endif  /* __TEST_LOG_DATA_H */
