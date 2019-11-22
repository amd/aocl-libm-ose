//R.obtained=FN_PROTOTYPE(sinh)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_sinh(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_sinh_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
    {0x0000000000000001LL, 0x0000000000000001LL, EXC_CHK_UNFL}, // denormal min
    {0x0005fde623545abcLL, 0x0005fde623545abcLL, EXC_CHK_UNFL}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0x000fffffffffffffLL, EXC_CHK_UNFL}, // denormal max
    {0x8000000000000001LL, 0x8000000000000001LL, EXC_CHK_UNFL}, // -denormal min
    {0x8002344ade5def12LL, 0x8002344ade5def12LL, EXC_CHK_UNFL}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0x800fffffffffffffLL, EXC_CHK_UNFL}, // -denormal max
    {0x0010000000000000LL, 0x0010000000000000LL, EXC_CHK_UNFL}, // normal min
    {0x43b3c4eafedcab02LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // normal max
    {0x8010000000000000LL, 0x8010000000000000LL, EXC_CHK_UNFL}, // -normal min
    {0xc5812e71245acfdbLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -normal max															
    {0x7FF0000000000000LL, 0x7ff0000000000000LL, EXC_CHK_NONE}, // inf
    {0xFFF0000000000000LL, 0xfff0000000000000LL, EXC_CHK_NONE}, // -inf
    {0x7FF8000000000000LL, 0x7ff8000000000000LL, EXC_CHK_NONE}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL, EXC_CHK_NONE}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL, EXC_CHK_NONE}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL, EXC_CHK_NONE}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000001LL, EXC_CHK_NONE}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff9123425dcba31LL, EXC_CHK_NONE}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xffffffffffffffffLL, EXC_CHK_NONE}, // -qnan max															
    {0x7FF0000000000001LL, 0x7ff8000000000001LL, EXC_CHK_INVL}, // snan min
    {0x7FF5344752a0bd90LL, 0x7ffd344752a0bd90LL, EXC_CHK_INVL}, // snan intermediate
    {0x7FF7FFFFFFFFFFFFLL, 0x7fffffffffffffffLL, EXC_CHK_INVL}, // snan max
    {0xFFF0000000000001LL, 0xfff8000000000001LL, EXC_CHK_INVL}, // -snan min
    {0xfFF432438995fffaLL, 0xfffc32438995fffaLL, EXC_CHK_INVL}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFFLL, 0xffffffffffffffffLL, EXC_CHK_INVL}, // -snan max															
    {0x3FF921FB54442D18LL, 0x4002690f661dd820LL, EXC_CHK_NONE}, // pi/2
    {0x400921FB54442D18LL, 0x402718f45d72e671LL, EXC_CHK_NONE}, // pi
    {0x401921FB54442D18LL, 0x4070bbeb1603926aLL, EXC_CHK_NONE}, // 2pi
    {0x3FFB7E151628AED3LL, 0x4005950fa1f030c5LL, EXC_CHK_NONE}, // e --
    {0x4005BF0A8B145769LL, 0x401e2d3246d48e6fLL, EXC_CHK_NONE}, // e
    {0x400DBF0A8B145769LL, 0x403495aaf7d6b9d9LL, EXC_CHK_NONE}, // e ++															
    {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, EXC_CHK_UNFL}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3fe0accdb2828938LL, EXC_CHK_NONE}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3fe0acd00fe63c11LL, EXC_CHK_NONE}, // 0.500000000000012
    {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, EXC_CHK_UNFL}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xbfe0accdb2828938LL, EXC_CHK_NONE}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xbfe0acd00fe63c11LL, EXC_CHK_NONE}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3ff2cd9fc44eb982LL, EXC_CHK_NONE}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3ff2cd9f967c1586LL, EXC_CHK_NONE}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3ff2cd9fc44ebb34LL, EXC_CHK_NONE}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x400108c26f0145f0LL, EXC_CHK_NONE}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x400108c3aabd6a62LL, EXC_CHK_NONE}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xbff2cd9fc44eb982LL, EXC_CHK_NONE}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xbff2cd9f967c1586LL, EXC_CHK_NONE}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xbff2cd9fc44ebb34LL, EXC_CHK_NONE}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xc00108c26f0145f0LL, EXC_CHK_NONE}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xc00108c3aabd6a62LL, EXC_CHK_NONE}, // -1.50000000000000012															
    {0x4000000000000000LL, 0x400d03cf63b6e19fLL, EXC_CHK_NONE}, // 2
    {0xC000000000000000LL, 0xc00d03cf63b6e19fLL, EXC_CHK_NONE}, // -2
    {0x4024000000000000LL, 0x40c5829dced69992LL, EXC_CHK_NONE}, // 10
    {0xC024000000000000LL, 0xc0c5829dced69992LL, EXC_CHK_NONE}, // -10
    {0x408F400000000000LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 1000
    {0xC08F400000000000LL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -1000
    {0x4050D9999999999ALL, 0x45f2dd7567cd83eeLL, EXC_CHK_NONE}, // 67.4
    {0xC050D9999999999ALL, 0xc5f2dd7567cd83eeLL, EXC_CHK_NONE}, // -67.4
    {0x409EFE6666666666LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 1983.6
    {0xC09EFE6666666666LL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -1983.6
    {0x4055E00000000000LL, 0x47c2d7566d26536bLL, EXC_CHK_NONE}, // 87.5
    {0xC055E00000000000LL, 0xc7c2d7566d26536bLL, EXC_CHK_NONE}, // -87.5
    {0x41002BC800000000LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 132473 
    {0xC1002BC800000000LL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -132473
    {0x4330000000000000LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 2^52
    {0x4330000000000001LL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 2^52 + 1 
    {0x432FFFFFFFFFFFFFLL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -2^52
    {0xC330000000000001LL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -(2^52 + 1) 
    {0xC32FFFFFFFFFFFFFLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, // -(2^52 -1 + 0.5)
     
    // special accuracy tests    
    {0x3e30000000000000LL, 0x3e30000000000000LL, EXC_CHK_NONE},  //min, small enough that cosh(x) = 1 //  
    {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, EXC_CHK_UNFL}, //min - 1 bit
    {0x3e30000000000001LL, 0x3e30000000000001LL, EXC_CHK_NONE}, //min + 1 bit
    {0xFE37E43C8800759CLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, //lambda + x = 1, x = -1.0000000000000000e+300
    {0xFE37E43C8800758CLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, //lambda + x < 1
    {0xFE37E43C880075ACLL, 0xfff0000000000000LL, EXC_CHK_OVFL}, //lambda + x > 1
    {0x408633ce8fb9f87eLL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, //max arg, x = 89.41598629223294,max coshf arg
    {0x408633ce8fb9f87dLL, 0x7feffffffffffd3bLL, EXC_CHK_NONE}, //max arg - 1 bit
    {0x408633ce8fb9f87fLL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, //max arg + 1 bit
    {0x408633ce8fb9f8ffLL, 0x7ff0000000000000LL, EXC_CHK_OVFL}, // > max 
    {0x408633ce8fb9f800LL, 0x7feffffffffe093bLL, EXC_CHK_NONE}, // < max
    {0x4034000000000000LL, 0x41aceb088b68e804LL, EXC_CHK_NONE}, //small_threshold = 20
    {0x4035000000000000LL, 0x41c3a6e1fd9eecfdLL, EXC_CHK_NONE}, //small_threshold+1 = 21
    {0x4033000000000000LL, 0x419546d8f9ed26e1LL, EXC_CHK_NONE}, //small_threshold - 1 = 19                          

  //special exception tests
  {F64_POS_ZERO, F64_POS_ZERO,EXC_CHK_NONE },  //0	
  {F64_NEG_ZERO, F64_NEG_ZERO,EXC_CHK_NONE },  //0
  {F64_POS_INF, F64_POS_INF,EXC_CHK_NONE },  
  {F64_NEG_INF, F64_NEG_INF,EXC_CHK_NONE },  
  {F64_POS_SNAN, F64_POS_SNAN,EXC_CHK_INVL },  //
  {F64_NEG_SNAN, F64_NEG_SNAN,EXC_CHK_INVL },  //
  {F64_POS_QNAN, F64_POS_QNAN,EXC_CHK_NONE },  //
  {F64_NEG_QNAN, F64_NEG_QNAN,EXC_CHK_NONE },  //
  {0x43E0000020000000LL,F64_POS_INF,EXC_CHK_OVFL},  //
  {0x408633CED916872BLL,F64_POS_INF,EXC_CHK_OVFL}, //7.104760e+002
  {0xC0FF5D35B020C49CLL,F64_NEG_INF,EXC_CHK_OVFL},  // -708.3964185322642 largest denormal result
  {0xC08F400000000000LL,F64_NEG_INF,EXC_CHK_OVFL}, // -1000
  {F64_POS_LDENORM,F64_POS_LDENORM,EXC_CHK_UNFL},
  {F64_NEG_LDENORM,F64_NEG_LDENORM,EXC_CHK_UNFL},

};

