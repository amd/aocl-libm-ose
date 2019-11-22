//R.obtained=FN_PROTOTYPE(atan2f)(P1.input, P2.input)
//float,float,float,ExcFlags
//P1 P2 R E
//unsigned int|float, unsigned int|float, unsigned int|float, ExcFlags|ExcFlags
//R.V3[j] = amd_ref_atan2f(P1.V3[j],P2.V3[j])
//float float float ExcFlags
//ULP amd_ref_atan2f_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{
      {0x4b0aabcd, 0x400aabcd, 0x3fc90fd9, EXC_CHK_NONE },  //
      {0x400aabcd, 0x4b0aabcd, 0x34800000, EXC_CHK_NONE },  //
      {0x400aabcd, 0xbb0aabcd, 0x3fc92fdb, EXC_CHK_NONE },  //
      {0xc00aabcd, 0xbb0aabcd, 0xbfc92fdb, EXC_CHK_NONE },  //
      {0xc00aabcd, 0xbf0aabcd, 0xbfe86b51, EXC_CHK_NONE },  //
      {F32_POS_SNAN, F32_POS_ZERO, F32_POS_SNAN,EXC_CHK_INVL },  //

     {F32_POS_ZERO,F32_NEG_ZERO, F32_POS_PI, EXC_CHK_NONE },  //0	
      {F32_NEG_ZERO,F32_NEG_ZERO, F32_NEG_PI, EXC_CHK_NONE },  //0	

      {F32_POS_ZERO,F32_POS_ZERO, F32_POS_ZERO, EXC_CHK_NONE },  //0	
      {F32_NEG_ZERO,F32_POS_ZERO, F32_NEG_ZERO, EXC_CHK_NONE },  //0	

      {F32_POS_ZERO,0xBF800000, F32_POS_PI, EXC_CHK_NONE },  //0,-1	
      {F32_NEG_ZERO,0xBF800000, F32_NEG_PI, EXC_CHK_NONE },  //0,-1	

      {F32_POS_ZERO,0x3F800000, F32_POS_ZERO, EXC_CHK_NONE },  //0,1	
      {F32_NEG_ZERO,0x3F800000, F32_NEG_ZERO, EXC_CHK_NONE },  //0,1	


      {0xBF800000,F32_POS_ZERO, F32_NEG_PIBY2, EXC_CHK_NONE },  
      {0xBF800000,F32_NEG_ZERO, F32_NEG_PIBY2, EXC_CHK_NONE },  

      {0x3F800000,F32_POS_ZERO, F32_POS_PIBY2, EXC_CHK_NONE },  
      {0x3F800000,F32_NEG_ZERO, F32_POS_PIBY2, EXC_CHK_NONE },  


      {0x3F800000,F32_NEG_INF, F32_POS_PI, EXC_CHK_NONE },  
      {0xBF800000,F32_NEG_INF, F32_NEG_PI, EXC_CHK_NONE },  


      {0x3F800000,F32_POS_INF, F32_POS_ZERO, EXC_CHK_NONE }, 
      {0xBF800000,F32_POS_INF, F32_NEG_ZERO, EXC_CHK_NONE }, 

      {F32_POS_INF,0x3F800000, F32_POS_PIBY2, EXC_CHK_NONE },
      {F32_NEG_INF,0xBF800000, F32_NEG_PIBY2, EXC_CHK_NONE },

      {F32_POS_INF,F32_NEG_INF, 0x4016cbe4, EXC_CHK_NONE },  // 3Pi/4 4016cbe4
      {F32_NEG_INF,F32_NEG_INF, 0xc016cbe4, EXC_CHK_NONE }, // c016cbe4

      {F32_POS_INF,F32_POS_INF, 0x3f490fdb, EXC_CHK_NONE },  //  3f490fdb
      {F32_NEG_INF,F32_POS_INF, 0xbf490fdb, EXC_CHK_NONE }, // bf490fdb


      {F32_POS_SNAN, F32_POS_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
      {F32_NEG_SNAN, F32_POS_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //

      {F32_POS_SNAN, F32_NEG_SNAN, F32_POS_SNAN,EXC_CHK_INVL },  //
      {F32_NEG_SNAN, F32_NEG_SNAN, F32_NEG_SNAN,EXC_CHK_INVL },  //

      {0x96421590,0x7EEBE575,F32_NEG_ZERO, EXC_CHK_UNFL},
      {0x16421590,0x7EEBE575,F32_POS_ZERO, EXC_CHK_UNFL},


};
