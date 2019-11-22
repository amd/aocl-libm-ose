//R.obtained=FN_PROTOTYPE(atan2)(P1.input, P2.input)
//double,double,double,ExcFlags
//P1 P2 R E
//unsigned long long int|double, unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_atan2(P1.V3[j],P2.V3[j])
//double double double ExcFlags
//ULP amd_ref_atan2_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
      {0x405fffffffffffffLL,0x3fe0000000000000LL, 0x3ff911fb59997f3a, EXC_CHK_NONE },
      {0xbfcabcdabcdabcdfLL,0x7fcabcdabc1abcdfLL, 0x80040000001cb936, EXC_CHK_NONE },
      {0xbfcabcdabcdabcdfLL,0xbfcabcdabc1abcdfLL, 0xc002d97c7f16689d, EXC_CHK_NONE },
//};
//#if 0
      {0xbfcabcdabcdabcdfLL,0xbfcabcdabcdabcdfLL, 0xc002d97c7f3321d2, EXC_CHK_NONE },
      {0x3fcabcdabcdabcdfLL,0xbfcabcdabcdabcdfLL, 0x4002d97c7f3321d2, EXC_CHK_NONE },

      {0x032000a000000000LL,0x431abcdabcdabcdfLL, 0x4c9b8ce7d92d8, EXC_CHK_NONE },
      {0x6210000000000000LL,0x41aabcdabcdabcdfLL, 0x3ff921fb54442d18, EXC_CHK_NONE },
      {0x6210000000000000LL,0xc1aabcdabcdabcdfLL, 0x3ff921fb54442d18, EXC_CHK_NONE },
      {F64_POS_SNAN,0xc1aabcdabcdabcdfLL, 0x7ff8000000000001, EXC_CHK_INVL },
      {0x3fcabcdabcdabcdfLL,0x3fcabcdabcdabcdfLL, 0x3fe921fb54442d18, EXC_CHK_NONE },
      {0x3fcabcdabcdabcdfLL,0x434abcdabcdabcdfLL, 0x3c70000000000000, EXC_CHK_NONE },
      {0x3feabcdabcdabcdfLL,0xc37abcdabcdabcdfLL, 0x400921fb54442d18, EXC_CHK_NONE },
      {0xbfeabcdabcdabcdfLL,0xc37abcdabcdabcdfLL, 0xc00921fb54442d18, EXC_CHK_NONE },
      {0x43babcdabcdabcdfLL,0x03cabcdabcdabcdaLL, 0x3ff921fb54442d18, EXC_CHK_NONE },
      {0x43babcdabcdabcdfLL,0x03c0000000000000LL, 0x3ff921fb54442d18, EXC_CHK_NONE },
      {0x41aabcdabcdabcdfLL,0x6210000000000000LL, 0x1f8abcdabcdabcdf, EXC_CHK_NONE },
      {0xc1aabcdabcdabcdfLL,0x6210000000000000LL, 0x9f8abcdabcdabcdf, EXC_CHK_NONE },
      {F64_POS_ZERO,F64_NEG_ZERO, 0x400921fb54442d18LL, EXC_CHK_NONE },  //0	
      {F64_NEG_ZERO,F64_NEG_ZERO, 0xc00921fb54442d18LL, EXC_CHK_NONE },  //0	
      {F64_POS_ZERO,F64_POS_ZERO, F64_POS_ZERO, EXC_CHK_NONE },  //0	
      {F64_NEG_ZERO,F64_POS_ZERO, F64_NEG_ZERO, EXC_CHK_NONE },  //0	
      {F64_POS_ZERO,0xBFF0000000000000LL, 0x400921fb54442d18LL, EXC_CHK_NONE },  //0,-1	
      {F64_NEG_ZERO,0xBFF0000000000000LL, 0xc00921fb54442d18LL, EXC_CHK_NONE },  //0,-1	
      {F64_POS_ZERO,0x3FF0000000000000LL, F64_POS_ZERO, EXC_CHK_NONE },  //0,1	
      {F64_NEG_ZERO,0x3FF0000000000000LL, F64_NEG_ZERO, EXC_CHK_NONE },  //0,1	
      {0xBFF0000000000000LL,F64_POS_ZERO, 0xbff921fb54442d18LL, EXC_CHK_NONE },  
      {0xBFF0000000000000LL,F64_NEG_ZERO, 0xbff921fb54442d18LL, EXC_CHK_NONE },  
      {0x3FF0000000000000LL,F64_POS_ZERO, 0x3ff921fb54442d18LL, EXC_CHK_NONE },  
      {0x3FF0000000000000LL,F64_NEG_ZERO, 0x3ff921fb54442d18LL, EXC_CHK_NONE },  
      {0x3FF0000000000000LL,F64_NEG_INF, 0x400921fb54442d18LL, EXC_CHK_NONE },  
      {0xBFF0000000000000LL,F64_NEG_INF, 0xc00921fb54442d18LL, EXC_CHK_NONE },  
      {0x3FF0000000000000LL,F64_POS_INF, F64_POS_ZERO, EXC_CHK_NONE }, 
      {0xBFF0000000000000LL,F64_POS_INF, F64_NEG_ZERO, EXC_CHK_NONE }, 

      {F64_POS_INF,0x3FF0000000000000LL, 0x3ff921fb54442d18LL, EXC_CHK_NONE },
      {F64_NEG_INF,0xBFF0000000000000LL, 0xbff921fb54442d18LL, EXC_CHK_NONE },

      {F64_POS_INF,F64_NEG_INF, 0x4002d97c7f3321d2LL, EXC_CHK_NONE },  // 3Pi/4 // 4009249249249249/4
      {F64_NEG_INF,F64_NEG_INF, 0xc002d97c7f3321d2LL, EXC_CHK_NONE },

      {F64_POS_INF,F64_POS_INF, 0x3fe921fb54442d18LL, EXC_CHK_NONE },  // 3P
      {F64_NEG_INF,F64_POS_INF, 0xbfe921fb54442d18LL, EXC_CHK_NONE },


      {F64_POS_SNAN, F64_POS_SNAN, F64_POS_SNAN,EXC_CHK_INVL },  //
      {F64_NEG_SNAN, F64_POS_SNAN, F64_NEG_SNAN,EXC_CHK_INVL },  //

      {F64_POS_SNAN, F64_NEG_SNAN, F64_POS_SNAN,EXC_CHK_INVL },  //
      {F64_NEG_SNAN, F64_NEG_SNAN, F64_NEG_SNAN,EXC_CHK_INVL },  //

      {0xB1DB0D2835C40259LL,0x7FE1CCF385EBC8A0LL,F64_NEG_ZERO, EXC_CHK_UNFL},
      {0x31DB0D2835C40259LL,0x7FE1CCF385EBC8A0LL,F64_POS_ZERO, EXC_CHK_UNFL},

};
//#endif