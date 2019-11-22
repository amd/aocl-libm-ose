//R.obtained=FN_PROTOTYPE(sqrt)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_sqrt(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_sqrt_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
{F64_POS_SNAN,F64_POS_QNAN,EXC_CHK_INVL},
{F64_NEG_SNAN,F64_NEG_QNAN,EXC_CHK_INVL},
{F64_POS_QNAN,F64_POS_QNAN,EXC_CHK_NONE},
{F64_NEG_QNAN,F64_NEG_QNAN,EXC_CHK_NONE},
{F64_POS_INF,F64_POS_INF,EXC_CHK_NONE},
{F64_NEG_INF,F64_NEG_QNAN,EXC_CHK_INVL},
{F64_POS_ONE,F64_POS_ONE,EXC_CHK_NONE},
{F64_NEG_ONE,F64_NEG_QNAN,EXC_CHK_INVL},
{F64_POS_ZERO,F64_POS_ZERO,EXC_CHK_NONE},
{F64_NEG_ZERO,F64_NEG_ZERO,EXC_CHK_NONE},
{F64_POS_HDENORM    ,0x1fffffffffffffffLL,EXC_CHK_NONE},
{F64_NEG_HDENORM    ,0xfff8000000000000LL,EXC_CHK_INVL},
{F64_POS_LDENORM    ,0x1e60000000000000LL,EXC_CHK_NONE},
{F64_NEG_LDENORM    ,0xfff8000000000000LL,EXC_CHK_INVL},
{F64_POS_HNORMAL    ,0x5fefffffffffffffLL,EXC_CHK_NONE},
{F64_NEG_HNORMAL    ,0xfff8000000000000LL,EXC_CHK_INVL},
{F64_POS_LNORMAL    ,0x2000000000000000LL,EXC_CHK_NONE},
{F64_NEG_LNORMAL    ,0xfff8000000000000LL,EXC_CHK_INVL},
{F64_POS_PI         ,0x3ffc5bf89853a94dLL,EXC_CHK_NONE},
{F64_NEG_PI         ,0xfff8000000000000LL,EXC_CHK_INVL},
{F64_POS_PIBY2      ,0x3ff40d9324a48138LL,EXC_CHK_NONE},
{F64_NEG_PIBY2      ,0xfff8000000000000LL,EXC_CHK_INVL},
{0xffefffffffffffffLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xc0862c4379671324LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x3e45798ee2308c3aLL,0x3f1a36e2eb1c432dLL, EXC_CHK_NONE},
{0x3fb999999999999aLL,0x3fd43d136248490fLL, EXC_CHK_NONE},
{0x3fe0000000000000LL,0x3fe6a09e667f3bcdLL, EXC_CHK_NONE},
{0x3feccccccccccccdLL,0x3fee5b9d136c6d96LL, EXC_CHK_NONE},
{0x3ff199999999999aLL,0x3ff0c7ebc96a56f6LL, EXC_CHK_NONE},
{0x3ff8000000000000LL,0x3ff3988e1409212eLL, EXC_CHK_NONE},
{0x3ffe666666666666LL,0x3ff60df2453ab723LL, EXC_CHK_NONE},
{0xbe45798ee2308c3aLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfb999999999999aLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfe0000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfeccccccccccccdLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff199999999999aLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff8000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbffe666666666666LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff0000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4024000000fff000LL,0x40094c583b7c39cfLL, EXC_CHK_NONE},
{0x408f4fff00000000LL,0x403fa7852d50e88eLL, EXC_CHK_NONE},
{0x408f4c0000000000LL,0x403fa5800714049cLL, EXC_CHK_NONE},
{0x409003999999999aLL,0x404001ccb2e431c4LL, EXC_CHK_NONE},
{0x42cf000000000000LL,0x415f7efbeb8d4f12LL, EXC_CHK_NONE},
{0x42c80000000fffffLL,0x415bb67ae861897eLL, EXC_CHK_NONE},
{0x42d10237f00fffffLL,0x41607f22db360b55LL, EXC_CHK_NONE},
{0x42e1067adfc342b0LL,0x416757569bc41671LL, EXC_CHK_NONE},
{0x42e1067adfc34208LL,0x416757569bc415fdLL, EXC_CHK_NONE},
{0x42e800000091fabfLL,0x416bb67ae8ac94adLL, EXC_CHK_NONE},
{0x42f8000001234fcfLL,0x4173988e14800ea1LL, EXC_CHK_NONE},
{0x42f80000000cccf8LL,0x4173988e140e5b00LL, EXC_CHK_NONE},
{0x43010237f0ccdfffLL,0x4177546a9ba1d67dLL, EXC_CHK_NONE},
{0x430800056891fa02LL,0x417bb67e07b9e9b7LL, EXC_CHK_NONE},
{0x43110237f00000ffLL,0x41807f22db2e4979LL, EXC_CHK_NONE},
{0x43110237f00000feLL,0x41807f22db2e4978LL, EXC_CHK_NONE},
{0x4321ffffffffffffLL,0x4187ffffffffffffLL, EXC_CHK_NONE},
{0x43210237f00000ffLL,0x4187546a9b15554fLL, EXC_CHK_NONE},
{0x43310237f00000ffLL,0x41907f22db2e4979LL, EXC_CHK_NONE},
{0x433800056891fa00LL,0x4193989049489a2aLL, EXC_CHK_NONE},
{0x4378000000000000LL,0x41b3988e1409212eLL, EXC_CHK_NONE},
{0x1LL,0x1e60000000000000LL, EXC_CHK_NONE},
{0x5fde623545abcLL,0x1ff3951f73e71d7bLL, EXC_CHK_NONE},
{0xfffffffffffffLL,0x1fffffffffffffffLL, EXC_CHK_NONE},
{0x8000000000000001LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x8002344ade5def12LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x800fffffffffffffLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x10000000000000LL,0x2000000000000000LL, EXC_CHK_NONE},
{0x8010000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xc5812e71245acfdbLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xffefffffffffffffLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x7ff0000000000000LL,0x7ff0000000000000LL, EXC_CHK_NONE},
{0xfff0000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x7ff8000000000000LL,0x7ff8000000000000LL, EXC_CHK_NONE},
{0x7ffe1a5701234dc3LL,0x7ffe1a5701234dc3LL, EXC_CHK_NONE},
{0x7fffffffffffffffLL,0x7fffffffffffffffLL, EXC_CHK_NONE},
{0xfff8000000000000LL,0xfff8000000000000LL, EXC_CHK_NONE},
{0xfff8000000000001LL,0xfff8000000000001LL, EXC_CHK_NONE},
{0xfff9123425dcba31LL,0xfff9123425dcba31LL, EXC_CHK_NONE},
{0xffffffffffffffffLL,0xffffffffffffffffLL, EXC_CHK_NONE},
{0x7ff0000000000001LL,0x7ff8000000000001LL, EXC_CHK_INVL},
{0x7ff5344752a0bd90LL,0x7ffd344752a0bd90LL, EXC_CHK_INVL},
{0x7ff7ffffffffffffLL,0x7fffffffffffffffLL, EXC_CHK_INVL},
{0xfff0000000000001LL,0xfff8000000000001LL, EXC_CHK_INVL},
{0xfff432438995fffaLL,0xfffc32438995fffaLL, EXC_CHK_INVL},
{0xfff7ffffffffffffLL,0xffffffffffffffffLL, EXC_CHK_INVL},
{0x3ff921fb54442d18LL,0x3ff40d931ff62705LL, EXC_CHK_NONE},
{0x400921fb54442d18LL,0x3ffc5bf891b4ef6aLL, EXC_CHK_NONE},
{0x401921fb54442d18LL,0x40040d931ff62705LL, EXC_CHK_NONE},
{0x3ffb7e151628aed3LL,0x3ff4f92b7e61eb29LL, EXC_CHK_NONE},
{0x4005bf0a8b145769LL,0x3ffa61298e1e069cLL, EXC_CHK_NONE},
{0x400dbf0a8b145769LL,0x3ffeda410adbd55cLL, EXC_CHK_NONE},
{0x0LL,0x0LL, EXC_CHK_NONE},
{0x3c4536b8b14b676cLL,0x3e1a0df7c04f6375LL, EXC_CHK_NONE},
{0x3fdffffbce4217d3LL,0x3fe6a09ceadf2b44LL, EXC_CHK_NONE},
{0x3fe000000000006cLL,0x3fe6a09e667f3c19LL, EXC_CHK_NONE},
{0xbbdb2752ce74ff42LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfdffffbce4217d3LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfe000000000006cLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x3ff0000000000000LL,0x3ff0000000000000LL, EXC_CHK_NONE},
{0x3fefffffc49bd0dcLL,0x3fefffffe24de860LL, EXC_CHK_NONE},
{0x3ff0000000000119LL,0x3ff000000000008cLL, EXC_CHK_NONE},
{0x3ff7fffef39085f4LL,0x3ff3988da672841bLL, EXC_CHK_NONE},
{0x3ff8000000000001LL,0x3ff3988e1409212fLL, EXC_CHK_NONE},
{0xbff0000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbfefffffc49bd0dcLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff0000000000119LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff7fffef39085f4LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xbff8000000000001LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4000000000000000LL,0x3ff6a09e667f3bcdLL, EXC_CHK_NONE},
{0xc000000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4024000000000000LL,0x40094c583ada5b53LL, EXC_CHK_NONE},
{0xc024000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x408f400000000000LL,0x403f9f6e4990f227LL, EXC_CHK_NONE},
{0xc08f400000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4050d9999999999aLL,0x40206b646433bf41LL, EXC_CHK_NONE},
{0xc050d9999999999aLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x409efe6666666666LL,0x404644d0e18c772fLL, EXC_CHK_NONE},
{0xc09efe6666666666LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4055e00000000000LL,0x4022b5524ae1278eLL, EXC_CHK_NONE},
{0xc055e00000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x41002bc800000000LL,0x4076bf7e96796acaLL, EXC_CHK_NONE},
{0xc1002bc800000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x4330000000000000LL,0x4190000000000000LL, EXC_CHK_NONE},
{0x4330000000000001LL,0x4190000000000000LL, EXC_CHK_NONE},
{0x432fffffffffffffLL,0x418fffffffffffffLL, EXC_CHK_NONE},
{0xc330000000000000LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xc330000000000001LL,0xfff8000000000000LL, EXC_CHK_INVL},
{0xc32fffffffffffffLL,0xfff8000000000000LL, EXC_CHK_INVL},
{0x43480677adfcfa00LL,0x419bba36903197e0LL, EXC_CHK_NONE},
{0x435801239871fa00LL,0x41a399051db93653LL, EXC_CHK_NONE},
{0x4368111144444444LL,0x41abc053c163c6a0LL, EXC_CHK_NONE},
{0x4388ffffffffffffLL,0x41bc48c6001f0abfLL, EXC_CHK_NONE},
{0x4398ffffffffffffLL,0x41c4000000000000LL, EXC_CHK_NONE},
{0x43a8ffffffffffffLL,0x41cc48c6001f0abfLL, EXC_CHK_NONE},
{0x43b8aaaa68ffffffLL,0x41d3ddc070c11638LL, EXC_CHK_NONE},
{0x43c8aaaa69ffffffLL,0x41dc1856cd12d27eLL, EXC_CHK_NONE},
{0x43d8009678abcd00LL,0x41e398cb81a85e3fLL, EXC_CHK_NONE},
{0x7fefffffffffffffLL,0x5fefffffffffffffLL, EXC_CHK_NONE},
{0x43e8098734209870LL,0x41ebbbfaa30bcbb2LL, EXC_CHK_NONE},
{0x43f800876abcda00LL,0x41f398c55c5e11b5LL, EXC_CHK_NONE},
{0x44080091781cdba0LL,0x41fbb6cee46fd9f8LL, EXC_CHK_NONE},
{0x4408012786abcde0LL,0x41fbb7258589c1f3LL, EXC_CHK_NONE},
{0x43b3c4eafedcab02LL,0x41d1c8f7df8b4aa2LL, EXC_CHK_NONE},

};
