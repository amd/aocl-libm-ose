//R.obtained=FN_PROTOTYPE(logb)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_logb(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_logb_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
   { F64_POS_SNAN ,      0x7ff8000000000001LL   ,EXC_CHK_INVL},
    { F64_NEG_SNAN ,     0xfff8000000000001LL  ,EXC_CHK_INVL},
    { F64_POS_QNAN ,     0x7ff8000000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_QNAN ,     0xfff8000000000000LL  ,EXC_CHK_NONE},
    { F64_POS_INF  ,     0x7ff0000000000000LL  ,EXC_CHK_NONE},
#ifdef WINDOWS
    { F64_NEG_INF  ,     F64_NEG_INF  ,EXC_CHK_NONE},
#else
    { F64_NEG_INF  ,     0x7ff0000000000000LL  ,EXC_CHK_NONE},
#endif

    { F64_POS_ONE  ,     0x0000000000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_ONE  ,     0x0000000000000000LL  ,EXC_CHK_NONE},
    { F64_POS_ZERO ,     0xfff0000000000000LL  ,EXC_CHK_ZERO},
    { F64_NEG_ZERO ,     0xfff0000000000000LL  ,EXC_CHK_ZERO},
    { F64_POS_HDENORM,   0xc08ff80000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_HDENORM,   0xc08ff80000000000LL  ,EXC_CHK_NONE},
    { F64_POS_LDENORM,   0xc090c80000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_LDENORM,   0xc090c80000000000LL  ,EXC_CHK_NONE},
    { F64_POS_HNORMAL,   0x408ff80000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_HNORMAL,   0x408ff80000000000LL  ,EXC_CHK_NONE},
    { F64_POS_LNORMAL,   0xc08ff00000000000LL  ,EXC_CHK_NONE},
    { F64_NEG_LNORMAL,   0xc08ff00000000000LL  ,EXC_CHK_NONE},    
    { F64_POS_PIBY2,     0x0000000000000000LL  ,EXC_CHK_NONE},  //pi/2
   {0x0000000000000000LL,0xfff0000000000000LL,EXC_CHK_ZERO},  //0.0
   {0x3EE0624DD2F1A9FCLL,0xc031000000000000LL,EXC_CHK_NONE},  //0.0000078125
   {0x3EF0624DC31C6CA1LL,0xc030000000000000LL,EXC_CHK_NONE},  //0.0000156249991
   {0x3FE02467BE553AC5LL,0xbff0000000000000LL,EXC_CHK_NONE},  //0.504444
   {0x3FF0000000000000LL,0x0000000000000000LL,EXC_CHK_NONE},  //1
   {0x4000000000000000LL,0x3ff0000000000000LL,EXC_CHK_NONE},  //2
   {0x3D3C25C268497682LL,0xc046000000000000LL,EXC_CHK_NONE},  //1.0000000e-13
   {F64_POS_PI,          0x3ff0000000000000LL,EXC_CHK_NONE},  //pi
   {0x401921FB54442D18LL,0x4000000000000000LL,EXC_CHK_NONE},  //2pi
   {0x4024000000000000LL,0x4008000000000000LL,EXC_CHK_NONE},  //10
   {0x408F400000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //1000
   {0x4060000000000000LL,0x401c000000000000LL,EXC_CHK_NONE},  //128
   {0x4086240000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //708.5
   {0x4086280000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //709
   {0x408C000000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //896
   {0xC086240000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //-708.5
   {0xC089600000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //-812
   {0xC070000000000000LL,0x4020000000000000LL,EXC_CHK_NONE},  //-256
   {0xc086232bdd7abcd2LL,0x4022000000000000LL,EXC_CHK_NONE},  // -708.3964185322641 smallest normal result
   {0xc086232bdd7abcd3LL,0x4022000000000000LL,EXC_CHK_NONE},  // -708.3964185322642 largest denormal result
   {0xC087480000000000LL,0x4022000000000000LL,EXC_CHK_NONE},  //-745
   {0x40862e42fefa39eeLL,0x4022000000000000LL,EXC_CHK_NONE},  //7.09782712893383973096e+02   largest value  --
   {0x40862e42fefa39efLL,0x4022000000000000LL,EXC_CHK_NONE},  //7.09782712893383973096e+02   largest value
   {0x40862e42fefa39ffLL,0x4022000000000000LL,EXC_CHK_NONE},  //7.09782712893383973096e+02   overflow
   {0x4200000000000000LL,0x4040800000000000LL,EXC_CHK_NONE},  //large   overflow
   {0xC05021EB851EB852LL,0x4018000000000000LL,EXC_CHK_NONE}, // -64.53
   {0xC0FF5D35B020C49CLL,0x4030000000000000LL,EXC_CHK_NONE}, // -128467.3555
   {0xBFD3333333333333LL,0xc000000000000000LL,EXC_CHK_NONE}, // -0.3
   {0xBFE48E410B630A91LL,0xbff0000000000000LL,EXC_CHK_NONE}, // -0.642365
   {0xBFE0000000000000LL,0xbff0000000000000LL,EXC_CHK_NONE}, // -0.5
   {0x3FD3333333333333LL,0xc000000000000000LL,EXC_CHK_NONE}, // 0.3
   {0x3FE48E410B630A91LL,0xbff0000000000000LL,EXC_CHK_NONE}, // 0.642365
   {0x3FE0000000000000LL,0xbff0000000000000LL,EXC_CHK_NONE}, // 0.5
   {0x405021EB851EB852LL,0x4018000000000000LL,EXC_CHK_NONE}, // 64.53
   {0x40FF5D35B020C49CLL,0x4030000000000000LL,EXC_CHK_NONE}, // 128467.3555
   {0xC08F400000000000LL,0x4022000000000000LL,EXC_CHK_NONE}, // -1000
   {0x408F400000000000LL,0x4022000000000000LL,EXC_CHK_NONE}, // 1000
   {0x0000000000000000LL,0xfff0000000000000LL,EXC_CHK_ZERO},

};

