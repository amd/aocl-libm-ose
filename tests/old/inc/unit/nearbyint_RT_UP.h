//R.obtained=FN_PROTOTYPE(nearbyint)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double,unsigned long long int|double, ExcFlags|ExcFlags
//R.V3[j] = nearbyint(P1.V3[j])
//double, double,ExcFlags
//ULP amd_ref_nearbyint_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

{F64_POS_SNAN,F64_POS_QNAN,EXC_CHK_INVL},
{F64_NEG_SNAN,F64_NEG_QNAN,EXC_CHK_INVL},
{F64_POS_QNAN,F64_POS_QNAN,EXC_CHK_NONE},
{F64_NEG_QNAN,F64_NEG_QNAN,EXC_CHK_NONE},
{F64_POS_INF,F64_POS_INF,EXC_CHK_NONE},
{F64_NEG_INF,F64_NEG_INF,EXC_CHK_NONE},
{F64_POS_ONE,F64_POS_ONE,EXC_CHK_NONE},
{F64_NEG_ONE,F64_NEG_ONE,EXC_CHK_NONE},
{F64_POS_ZERO,F64_POS_ZERO,EXC_CHK_NONE},
{F64_NEG_ZERO,F64_NEG_ZERO,EXC_CHK_NONE},
{F64_POS_HDENORM ,0x3ff0000000000000LL,EXC_CHK_NONE},
{F64_NEG_HDENORM ,0x8000000000000000LL,EXC_CHK_NONE},
{F64_POS_LDENORM ,0x3ff0000000000000LL,EXC_CHK_NONE},
{F64_NEG_LDENORM ,0x8000000000000000LL,EXC_CHK_NONE},
{F64_POS_HNORMAL ,0x7fefffffffffffffLL,EXC_CHK_NONE},
{F64_NEG_HNORMAL ,0xffefffffffffffffLL,EXC_CHK_NONE},
{F64_POS_LNORMAL ,0x3ff0000000000000LL,EXC_CHK_NONE},
{F64_NEG_LNORMAL ,0x8000000000000000LL,EXC_CHK_NONE},
{F64_POS_PI      ,0x4010000000000000LL,EXC_CHK_NONE},
{F64_NEG_PI      ,0xc008000000000000LL,EXC_CHK_NONE},
{F64_POS_PIBY2   ,0x4000000000000000LL,EXC_CHK_NONE},
{F64_NEG_PIBY2   ,0xbff0000000000000LL,EXC_CHK_NONE},
{0x0000000000000001LL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // denormal min
{0x0005fde623545abcLL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // denormal intermediate
{0x000FFFFFFFFFFFFFLL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // denormal max
{0x8000000000000001LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -denormal min
{0x8002344ade5def12LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -denormal intermediate
{0x800FFFFFFFFFFFFFLL, 0x8000000000000000LL,EXC_CHK_NONE}, // -denormal max
{0x0010000000000000LL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // normal min
{0x43b3c4eafedcab02LL, 0x43b3c4eafedcab02LL,EXC_CHK_NONE}, // normal intermediate
{0x7FEFFFFFFFFFFFFFLL, 0x7fefffffffffffffLL,EXC_CHK_NONE}, // normal max
{0x8010000000000000LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -normal min
{0xc5812e71245acfdbLL, 0xc5812e71245acfdbLL,EXC_CHK_NONE}, // -normal intermediate
{0xFFEFFFFFFFFFFFFFLL, 0xffefffffffffffffLL,EXC_CHK_NONE}, // -normal max
{0x7FF0000000000000LL, 0x7ff0000000000000LL,EXC_CHK_NONE}, // inf
{0xFFF0000000000000LL, 0xfff0000000000000LL,EXC_CHK_NONE}, // -inf
{0x7FF8000000000000LL, 0x7ff8000000000000LL,EXC_CHK_NONE}, // qnan min
{0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL,EXC_CHK_NONE}, // qnan intermediate
{0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL,EXC_CHK_NONE}, // qnan max
{0xFFF8000000000000LL, 0xfff8000000000000LL,EXC_CHK_NONE}, // indeterninate
{0xFFF8000000000001LL, 0xfff8000000000001LL,EXC_CHK_NONE}, // -qnan min
{0xFFF9123425dcba31LL, 0xfff9123425dcba31LL,EXC_CHK_NONE}, // -qnan intermediate
{0xFFFFFFFFFFFFFFFFLL, 0xffffffffffffffffLL,EXC_CHK_NONE}, // -qnan max
{0x3FF921FB54442D18LL, 0x4000000000000000LL,EXC_CHK_NONE}, // pi/2
{0x400921FB54442D18LL, 0x4010000000000000LL,EXC_CHK_NONE}, // pi
{0x401921FB54442D18LL, 0x401c000000000000LL,EXC_CHK_NONE}, // 2pi
{0x3FFB7E151628AED3LL, 0x4000000000000000LL,EXC_CHK_NONE}, // e --
{0x4005BF0A8B145769LL, 0x4008000000000000LL,EXC_CHK_NONE}, // e
{0x400DBF0A8B145769LL, 0x4010000000000000LL,EXC_CHK_NONE}, // e ++
{0x0000000000000000LL, 0x0000000000000000LL,EXC_CHK_NONE}, // 0
{0x3C4536B8B14B676CLL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // 0.0000000000000000023
{0x3FDFFFFBCE4217D3LL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // 0.4999989999999999999
{0x3FE000000000006CLL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // 0.500000000000012
{0x8000000000000000LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -0
{0xBBDB2752CE74FF42LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -0.000000000000000000023
{0xBFDFFFFBCE4217D3LL, 0x8000000000000000LL,EXC_CHK_NONE}, // -0.4999989999999999999
{0xBFE000000000006CLL, 0x8000000000000000LL,EXC_CHK_NONE}, // -0.500000000000012
{0x3FF0000000000000LL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // 1
{0x3FEFFFFFC49BD0DCLL, 0x3ff0000000000000LL,EXC_CHK_NONE}, // 0.9999998893750006
{0x3FF0000000000119LL, 0x4000000000000000LL,EXC_CHK_NONE}, // 1.0000000000000624998
{0x3FF7FFFEF39085F4LL, 0x4000000000000000LL,EXC_CHK_NONE}, // 1.499998999999999967
{0x3FF8000000000001LL, 0x4000000000000000LL,EXC_CHK_NONE}, // 1.50000000000000012
{0xBFF0000000000000LL, 0xbff0000000000000LL,EXC_CHK_NONE}, // -1
{0xBFEFFFFFC49BD0DCLL, 0x8000000000000000LL,EXC_CHK_NONE}, // -0.9999998893750006
{0xBFF0000000000119LL, 0xbff0000000000000LL,EXC_CHK_NONE}, // -1.0000000000000624998
{0xBFF7FFFEF39085F4LL, 0xbff0000000000000LL,EXC_CHK_NONE}, // -1.499998999999999967
{0xBFF8000000000001LL, 0xbff0000000000000LL,EXC_CHK_NONE}, // -1.50000000000000012
{0x4000000000000000LL, 0x4000000000000000LL,EXC_CHK_NONE}, // 2
{0xC000000000000000LL, 0xc000000000000000LL,EXC_CHK_NONE}, // -2
{0x4024000000000000LL, 0x4024000000000000LL,EXC_CHK_NONE}, // 10
{0xC024000000000000LL, 0xc024000000000000LL,EXC_CHK_NONE}, // -10
{0x408F400000000000LL, 0x408f400000000000LL,EXC_CHK_NONE}, // 1000
{0xC08F400000000000LL, 0xc08f400000000000LL,EXC_CHK_NONE}, // -1000
{0x4050D9999999999ALL, 0x4051000000000000LL,EXC_CHK_NONE}, // 67.4
{0xC050D9999999999ALL, 0xc050c00000000000LL,EXC_CHK_NONE}, // -67.4
{0x409EFE6666666666LL, 0x409f000000000000LL,EXC_CHK_NONE}, // 1983.6
{0xC09EFE6666666666LL, 0xc09efc0000000000LL,EXC_CHK_NONE}, // -1983.6
{0x4055E00000000000LL, 0x4056000000000000LL,EXC_CHK_NONE}, // 87.5
{0xC055E00000000000LL, 0xc055c00000000000LL,EXC_CHK_NONE}, // -87.5
{0x41002BC800000000LL, 0x41002bc800000000LL,EXC_CHK_NONE}, // 132473
{0xC1002BC800000000LL, 0xc1002bc800000000LL,EXC_CHK_NONE}, // -132473
{0x4330000000000000LL, 0x4330000000000000LL,EXC_CHK_NONE}, // 2^52
{0x4330000000000001LL, 0x4330000000000001LL,EXC_CHK_NONE}, // 2^52 + 1
{0x432FFFFFFFFFFFFFLL, 0x4330000000000000LL,EXC_CHK_NONE}, // 2^52 -1 + 0.5
{0xC330000000000000LL, 0xc330000000000000LL,EXC_CHK_NONE}, // -2^52
{0xC330000000000001LL, 0xc330000000000001LL,EXC_CHK_NONE}, // -(2^52 + 1)
{0xC32FFFFFFFFFFFFFLL, 0xc32ffffffffffffeLL,EXC_CHK_NONE}  // -(2^52 -1 + 0.5)

};
