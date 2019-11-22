//R.obtained=FN_PROTOTYPE(atanf)(P1.input)
//float,float,ExcFlags
//P1 R E
//unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_atanf(P1.V3[j])
//float float ExcFlags
//ULP amd_ref_atanf_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

      {0x3e70abcd, 0x3e6c6184, EXC_CHK_NONE },  //0	
      {0xbe70abcd, 0xbe6c6184, EXC_CHK_NONE },  //0	
      {0x3fa0abcd, 0x3f65e9a2, EXC_CHK_NONE },  //0	
      {0xbfa0abcd, 0xbf65e9a2, EXC_CHK_NONE },  //0	
      {0x3f70abcd, 0x3f4129d7, EXC_CHK_NONE },  //0	
      {0xbf70abcd, 0xbf4129d7, EXC_CHK_NONE },  //0	
      {0x3f10abcd, 0x3f03ae51, EXC_CHK_NONE },  //0	
      {0xbf10abcd, 0xbf03ae51, EXC_CHK_NONE },  //0	
      {0xca00abcd, 0xbfc90fd7, EXC_CHK_NONE },  //0	

      {F32_POS_ZERO, F32_POS_ZERO,EXC_CHK_NONE },  //0	
      {F32_NEG_ZERO, F32_NEG_ZERO,EXC_CHK_NONE },  //0
      {F32_POS_INF, F32_POS_PIBY2,EXC_CHK_NONE  },  //1.57079637 = pi/2	 
      {F32_NEG_INF, F32_NEG_PIBY2,EXC_CHK_NONE  },  //1.57079637 = pi/2	

      {F32_POS_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
      {F32_NEG_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //
      {F32_POS_QNAN, F32_POS_QNAN,EXC_CHK_NONE },  //
      {F32_NEG_QNAN, F32_NEG_QNAN,EXC_CHK_NONE },  //

  {F32_POS_LDENORM,F32_POS_LDENORM,EXC_CHK_UNFL},
  {F32_NEG_LDENORM,F32_NEG_LDENORM,EXC_CHK_UNFL},
  {F32_POS_HDENORM,F32_POS_HDENORM,EXC_CHK_UNFL},
  {F32_NEG_HDENORM,F32_NEG_HDENORM,EXC_CHK_UNFL},


};
