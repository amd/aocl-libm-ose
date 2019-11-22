#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "./DataTypes.h"

// for exp
static const U64 TestInput_F64_01[] = {
    0x40218f5c28f5c28f, // +8.78
    0xc0218f5c28f5c28f, // -8.78
    0x3ff0000000000000, // +1.0
    0xbff0000000000000, // -1.0
    };

// for log
static const U64 TestInput_F64_02[] = {
    0x4028666666666666, // 12.2
    0x4068d66666666666, // 198.7
    0x3FF0F5C28F5C28F6, // 1.01
    0x3FEE147AE147AE14, // 0.99
    };

// for pow
static const U64 TestInput_F64_03[] = {
    0x400fc28f5c28f5c3, // 3.97
    0x4059b5c28f5c28f6, // 102.84
    0xc00fc28f5c28f5c3, // -3.97
    0xc059b5c28f5c28f6, // -102.84
};

static const U64 TestInput_F64_04[] = {
    0x402f570a3d70a3d7, // 15.67
    0x4012147ae147ae14, // 4.52
    0x4030000000000000, // 16
    0x4022000000000000, // 9
};

//test input for cos, sin and tan function
static const U64 TestInput_F64_05[] = {
	0x401acccccccccccd, // 6.7, typical case
	0x4020000000000000, // 8, typical case
	0x41232cbe00000000, // 628319, typical case
	0x41232cc100000000, // 628320.5, using remainder cases
	0xbfe6666666666666, //-0.7 < pi/4
	0x3e3e1094d643f784, // 0.000000007 < 2^(-27)
	0x3f0f75104d551d69, // 0.00006 < 2^(-13)
};

static const U32 TestInput_F32_01[] = {
    0x410c7ae1, // +8.78
    0xc10c7ae1, // -8.78
    0x3f800000, // +1.0
    0xbf800000, // -1.0
    };
 
static const U32 TestInput_F32_02[] = {
    0x41433333, // 12.2
    0x4346b333, // 198.7
    0x3f8147ae, // 1.01
    0x3f7d70a4, // 0.99
    };

static const U32 TestInput_F32_03[] = {
    0x407e147b, // 3.97
    0x42cdae14, // 102.84
    0xc07e147b, // -3.97
    0xc2cdae14, // -102.84
};

static const U32 TestInput_F32_04[] = {
    0x417ab852, // 15.67
    0x4090a3d7, // 4.52
    0x41800000, // 16
    0x41100000, // 9
};

//for function cosf, sinf and tanf
static const U32 TestInput_F32_05[] = {

    //0x38d1b717, // 0.0001
	//0x4090a3d7,// 4.52 <  3pi/2
	//0x40a051ec,// 5.01 > 3pi/2
	0xbf333333, //-0.7
	0x40d66666, // 6.7
	0x454475c3, // 3143.36
    0x491965f0, // 628319
};

static const double MAX_TIME = 1000.0;
static const double MILLION = 1000000.0;

//The following for complete set of LIBM functions
//for function acosf and asinf
static const U32 TestInput_F32_06[] = {
	0x3e800000, // 0.25 < 0.5, typical
	0x3f400000, // 0.75 > 0.5, typical
	0xbe800000, // abs(-0.25) < 0.5, typical
    0xbf400000, // abs(-0.75) > 0.5, typical
	0x32000000, // 2^(-27) < 2 ^(-26)
};

//test input for acos and asin function
static const U64 TestInput_F64_06[] = {
	0x3fbf9add3739635f, // 0.123456789 < 0.5, typical
	0x3fef9add3c0e56b8, // 0.987654321 > 0.5, typical
	0xbfbf9add3739635f, // abs(-0.123456789) < 0.5, typical
    0xbfef9add3c0e56b8, // abs(-0.987654321) > 0.5, typical
	0x3c60000000000000, // 2^(-57) < 2^(-56)
};

//for function atanf 
static const U32 TestInput_F32_07[] = {
	0x42c80000, // 100 > 39/16, typical
	0xc2c80000, // -100< -39/16, typical
	0x400c0000, // 19/16 < 2.1875 < 39/16
	0xbf880000, // 11/16 < abs(-1.0625) < 19/16
	0x3f100000, // 7/16 < 0.5625 < 11/16
	0xbea00000, // abs(-0.3125) < 7/16
	0xb5800000, // abs(-2^(-20)) < 2 ^(-19)
	0x4d000000, // 134217728 = 2^27 > 2^26
};

//test input for atan function
static const U64 TestInput_F64_07[] = {
	0x4059000000000000, // 100 > 39/16, typical
	0xc059000000000000, // -100< -39/16, typical
	0x4001800000000000, // 19/16 < 2.1875 < 39/16
	0xbff1000000000000, // 11/16 < abs(-1.0625) < 19/16
	0x3fe2000000000000, // 7/16 < 0.5625 < 11/16
	0xbfd4000000000000, // abs(-0.3125) < 7/16
	0xbe40000000000000, // abs(-2^(-27)) < 2 ^(-26)
	0x4370000000002345, // 7.20575940380724E+16 > 2^(56)
};

//for function atan2f 
static const U32 TestInput_F32_08[] = {
	0x40400000, // 3 > 0.0625 typical case
	0xc0a00000, // abs(- 5) typical case 
	0x3fb79e06, // 1.43451 for < 0.0625 case
	0x3b03126f, // 0.002 for < 1e-4 case
};

static const U32 TestInput_F32_09[] = {
	0x40a00000, // 5 > 0.0625 typical case 
	0x40400000, // 3 > 0.0625 typical case
	0x41b8cccd, //23.1 for < 0.0625 case
	0x43480000, // 200 for < 1e-4 case
};

//test input for atan2 function
static const U64 TestInput_F64_08[] = {
	0x4008000000000000, // 3 > 0.0625 typical case
	0xc014000000000000, // abs(- 5) typical case 
	0x3ff6f3c0c1fc8f32, // 1.43451 for < 0.0625 case
	0x3e8ad7f29abcaf48, // 0.0000002 for < 1e-8 case
};

static const U64 TestInput_F64_09[] = {
	0xc014000000000000, // 5 > 0.0625 typical case 
	0x4008000000000000, // 3 > 0.0625 typical case
	0x403719999999999a, //23.1 for < 0.0625 case
	0x4069000000000000, // 200 for < 1e-8 case
};

//test input for acoshf
static const U32 TestInput_F32_10[] = {
	0x40b00000, //5.5 > 2 (<1/sqrt(epsilon))typical case
	0x3fc00000, //1.5 < 2 typical case
	0x46000005, //8192.005 > 1/sqrt(eps)
};

//test input for acosh
static const U64 TestInput_F64_10[] = {
	0x4060400000000000, //130 > 128 (< 1/sqrt(eps) typical
	0x4041800000000000, //35 > 32 (<128) typical
	0x4031000000000000, //17 > 8 (<32) typical
	0x401c000000000000, //7 > 4 (<8) typical
	0x4004000000000000, //2.5 > 2 (<4) typical
	0x3ffe147ae147ae14, //1.88 > 1.75 (<2) typical
	0x3ffb16872b020c4a, //1.693 >1.5 (<1.75) typical
	0x3ff3c0c1fc8f3238, //1.23456 > 1.13 (< 1.5) typical
	0x3ff1432ca57a786c, //1.0789 > 1.06 (<1.13) typical
	0x3ff0de69ad42c3ca, //1.0543 > 1 (< 1.06) typical 
	0x4196a09e667f3bdd, //94906265.6242518 > 1/sqrt(eps)
};

//test input for asinhf
static const U32 TestInput_F32_11[] = {
	0x40b00000, //5.5 > 4 (<1/sqrt(epsilon))typical case
	0x40200000, //2.5 > 2 (<4) typical
	0x3fc00000, //1.5 < 2 typical case
	0x46000005, //8192.005 > 1/sqrt(eps)
};

//test input for asinh
static const U64 TestInput_F64_11[] = {
	0xbfbf9a6b50b0f27c, // 0 < abs(-0.12345) < 0.25, typical
	0x3fd61f969e3c9689, // 0.25 < 0.345678 < 0.5, typical
	0xbfe60c5eb313be23, // 0.5 < abs(-0.68901) < 0.75, typical
	0x3fe93f91e646f156, // 0.75 < 0.78901 < 1, typical
	0x3ff3c083126e978d, // 1 < 1.2345 < 1.5, typical
	0x3ffadcc63f141206, // 1.5 < 1.6789 < 2, typical
	0x4001147ae147ae14, // 2 < 2.135 < 4, typical
	0xc01561e4f765fd8b, // 4 < abs(-5.3456) < 8, typical
	0x403591628cbd1245, // 8 < 21.56791 < 32, typical
	0x40408fef9db22d0e, // 32 < 33.1245 < 1/sqrt(eps), typical
	0x4196a09e667f3bdd, //94906265.6242518 > 1/sqrt(eps)
};

//test input for atanhf
static const U32 TestInput_F32_12[] = {
	0x3e800000, // 0.25 < 0.5, typical
	0x3f400000, // 0.75 > 0.5, typical
    0xbf400000, // abs(-0.75) > 0.5, typical
	0x32000000, // 2^(-27) < 2 ^(-13)
};

//test input for atanh
static const U64 TestInput_F64_12[] = {
	0x3fbf9add3739635f, // 0.123456789 < 0.5, typical
	0x3fef9add3c0e56b8, // 0.987654321 > 0.5, typical
    0xbfef9add3c0e56b8, // abs(-0.987654321) > 0.5, typical
	0x3c60000000000000, // 2^(-57) < 2^(-28)
};

//test input for coshf and sinhf
static const U32 TestInput_F32_13[] = {
	0x42120000, //  36.5 > small_threshold = 36.1236
	0x4128f5c3, // 10.56 < small_threshold = 36.1236
	0x38000000, //2^(-15) < 2^(-14)
};

//test input for cosh and sinh
static const U64 TestInput_F64_13[] = {
	0x4045ba5e353f7cee, //43.456 > small_threshold (20 for cosh, sinh 36.1236)
	0x40251eb851eb851f, // 10.56 < small_threshold (20 for cosh, sinh 36.1236)
	0x3c60000000000000, // 2^(-57) < 2^(-28)
};

//test input for tanhf
static const U32 TestInput_F32_14[] = {
	0x3f400000, // 0.75 < 0.9
	0x3f733333, // 0.9 < 0.95 < 1
	0x40600000, // 1 < 3.5 < 10.0
	0x4121999a, //10.1 > large_threshold=10.0
	0x38000000, //2^(-15) < 2^(-13)
};

//test input for tanh
static const U64 TestInput_F64_14[] = {
	0x3fe8000000000000, // 0.75 < 0.9
	0x3feccccccccccccd, // 0.9 < 0.95 < 1
	0x400c000000000000, // 1 < 3.5 < 20.0
	0x4035333333333333, // 21.2 > large_threshold=20.0
	0x3c60000000000000, // 2^(-57) < 2^(-28)
};

//test input for modff
static const U32 TestInput_F32_15[] = {
	0x3f400000, // 0.75 < 1
	0xc2f6e979, //abs(-123.456) >1
};

//test input for modf
static const U64 TestInput_F64_15[] = {
	0x3fe8000000000000, // 0.75 < 1
	0xc05edd2f1a9fbe77, //abs(-123.456) >1 
};

//test input for ceilf and floorf
static const U32 TestInput_F32_16[] = {
	0xbf400000, // abs(-0.75) < 1
	0xc2f6e979, //abs(-123.456) >1
	0x42f6e9fc, //123.457
	0x80000000, // negative zero
};

//test input for ceil and floor
static const U64 TestInput_F64_16[] = {
	0xbfe8000000000000, // abs(-0.75) < 1
	0xc05edd2f1a9fbe77, //abs(-123.456) >1 
	0x405edd3f7ced9168, //123.457
	0x8000000000000000, // negative zero
};

//test input for hypotf
static const U32 TestInput_F32_17[] = {
	0x3f400000, // 0.75
	0x42f6e9fc, //123.457
	0x3a1d4952, //0.0006
};
static const U32 TestInput_F32_18[] = {
	0x3f400000, // 0.75 
	0xc2f6e979, //abs(-123.456)
	0x449a5000, //1234.5
};

//test input for hypot
static const U64 TestInput_F64_17[] = {
	0x3fe8000000000000, // 0.75
	0x405edd3f7ced9168, //123.457 same level
	0x3f0f75104d551d69, //0.00006 different exp
};
static const U64 TestInput_F64_18[] = {
	0x3fe8000000000000, // 0.75
	0xc05edd2f1a9fbe77, //abs(-123.456)
	0x40c81cd99999999a, //12345.7
};

//test input for copysignf
static const U32 TestInput_F32_19[] = {
	0x3f400000, // 0.75
	0x42f6e9fc, //123.457
	0xba1d4952, //-0.0006
	0xc4454666, //-789.1
};
static const U32 TestInput_F32_20[] = {
	0x3f400000, // 0.75 
	0xc2f6e979, //-123.456
	0x449a5000, //1234.5
	0xc4454666, //-789.1
};

//test input for copysign
static const U64 TestInput_F64_19[] = {
	0x3fe8000000000000, // 0.75 
	0x405edd3f7ced9168, //123.457 
	0xbf0f75104d551d69, //-0.00006 
	0xc0c81cd99999999a, //-12345.7
};
static const U64 TestInput_F64_20[] = {
	0x3fe8000000000000, // 0.75
	0xc05edd2f1a9fbe77, //-123.456
	0x40c81cd99999999a, //12345.7
	0xc05edd2f1a9fbe77, //-123.456
};

//test input for _finitef
static const U32 TestInput_F32_21[] = {
	0x7f800000, // inf
	0xff800000, //-inf
	0x7fc00001, // NaN
	0xffef8000, //-Qnan
	0xc2f6e979, // -123.456, finite
};

//no _finite function
static const U64 TestInput_F64_21[] = {
	0x7ff0000000000000, // infinite
	0xfff0000000000000, //- inf
	0x7ffc000000000000, // qnan
	0x7ff4000000000000, // snan
	0xc05edd2f1a9fbe77, //-123.456, finite
};

//test input for fmodf/remainderf
static const U32 TestInput_F32_22[] = {
	0x3f400000, // 0.75
	0x42f6e9fc, //123.457
	0xba1d4952, //-0.0006
	0xc4454666, //-789.1
};
static const U32 TestInput_F32_23[] = {
	0xc2f6e979, //-123.456
	0x3f400000, // 0.75 
	0x449a5000, //1234.5
	0xc4454666, //-789.1
};

//test input for fmod/remainder
static const U64 TestInput_F64_22[] = {
	0x3fe8000000000000, // 0.75 
	0x405edd3f7ced9168, //123.457 
	0xbf0f75104d551d69, //-0.00006 
	0xc0c81cd99999999a, //-12345.7
};
static const U64 TestInput_F64_23[] = {
	0xc05edd2f1a9fbe77, //-123.456
	0x3fe8000000000000, // 0.75
	0x40c81cd99999999a, //12345.7
	0xc05edd2f1a9fbe77, //-123.456
};

#endif // __CONSTANTS_H__
