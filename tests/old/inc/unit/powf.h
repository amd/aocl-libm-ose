//R.obtained=FN_PROTOTYPE(powf)(P1.input, P2.input)
//float,float,float,ExcFlags
//P1 P2 R E
//unsigned int|float, unsigned int|float, unsigned int|float,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_powf(P1.V3[j],P2.V3[j])
//float float float ExcFlags
//ULP amd_ref_powf_ULP(P1.V3[j],P2.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()
#include <unistd.h>
static struct data input[] =
{
	//{0xbf800000,0x7fc00001,0x0,EXC_CHK_NONE},


   {0x5F800000,0x3E000000,0x43800000,EXC_CHK_NONE},
   {F32_POS_QNAN,F32_POS_INF,F32_POS_QNAN,EXC_CHK_NONE},
   {F32_NEG_QNAN,F32_POS_INF,F32_POS_QNAN,EXC_CHK_NONE},
   {F32_POS_QNAN,F32_NEG_INF,F32_POS_QNAN,EXC_CHK_NONE},
   {F32_NEG_QNAN,F32_NEG_INF,F32_POS_QNAN,EXC_CHK_NONE},

   {F32_POS_HDENORM,F32_POS_HDENORM,0x3F800000,EXC_CHK_NONE},

{F32_POS_HDENORM,F32_NEG_HDENORM,0x3f800000,EXC_CHK_NONE},
{F32_NEG_HDENORM,F32_POS_LDENORM,0x7fC00000,EXC_CHK_INVL},
{F32_NEG_HDENORM,F32_NEG_LDENORM,0x7fC00000,EXC_CHK_INVL},
{F32_POS_LDENORM,F32_POS_HDENORM,0x3f800000,EXC_CHK_NONE},
{F32_POS_LDENORM,F32_NEG_HDENORM,0x3f800000,EXC_CHK_NONE},
{F32_NEG_LDENORM,F32_POS_LDENORM,0x7fC00000,EXC_CHK_INVL},
{F32_NEG_LDENORM,F32_NEG_LDENORM,0x7fC00000,EXC_CHK_INVL},
{F32_POS_HDENORM,F32_POS_LNORMAL,0x3f800000,EXC_CHK_NONE},
{F32_POS_LDENORM,F32_NEG_HNORMAL,0x7f800000,EXC_CHK_OVFL},//10
{F32_POS_HNORMAL,F32_POS_HNORMAL,0x7F800000,EXC_CHK_OVFL},
{F32_POS_HNORMAL,F32_NEG_HNORMAL,0x00000000,EXC_CHK_UNFL},
{F32_NEG_HNORMAL,F32_POS_LNORMAL,0x7fC00000,EXC_CHK_INVL},
{F32_POS_LNORMAL,F32_POS_HNORMAL,0x00000000,EXC_CHK_UNFL},
{F32_POS_LNORMAL,F32_NEG_HNORMAL,0x7f800000,EXC_CHK_OVFL},
{F32_NEG_LNORMAL,F32_POS_LNORMAL,0x7fC00000,EXC_CHK_INVL},//16
{0x40000000,0X42FE0000,0x7f000000,EXC_CHK_NONE},
{0x40000000,0X43000000,F32_POS_INF,EXC_CHK_OVFL},
{F32_POS_ZERO,0xC0400000,F32_POS_INF,EXC_CHK_ZERO},
{F32_NEG_ZERO,0xC0400000,F32_NEG_INF,EXC_CHK_ZERO},//20
{F32_POS_ZERO,0xC0000000,F32_POS_INF,EXC_CHK_ZERO},
{F32_NEG_ZERO,0xC0000000,F32_POS_INF,EXC_CHK_ZERO},
{F32_POS_ZERO,0x40400000,F32_POS_ZERO,EXC_CHK_NONE},
{F32_NEG_ZERO,0x40400000,F32_NEG_ZERO,EXC_CHK_NONE},
{F32_POS_ZERO,0x40000000,F32_POS_ZERO,EXC_CHK_NONE},
{F32_NEG_ZERO,0x40000000,F32_POS_ZERO,EXC_CHK_NONE},
{0xBF800000,F32_POS_INF,0x3F800000,EXC_CHK_NONE},
#ifdef WINDOWS
{0xBF800000,F32_NEG_INF,0xbf800000,EXC_CHK_NONE},
#else
{0xBF800000,F32_NEG_INF,0x3F800000,EXC_CHK_NONE},
#endif
{0x3F800000,0x3F800000,0x3F800000,EXC_CHK_NONE},
{0x3F800000,F32_POS_QNAN,0x3F800000,EXC_CHK_NONE},//30
{0x3F800000,F32_POS_ZERO,0x3F800000,EXC_CHK_NONE},
{0x3F800000,F32_NEG_ZERO,0x3F800000,EXC_CHK_NONE},
#ifdef WINDOWS
{F32_POS_QNAN,F32_POS_ZERO,0x7fc00000,EXC_CHK_NONE},
{F32_NEG_QNAN,F32_POS_ZERO,0xffc00000,EXC_CHK_NONE},
{F32_POS_QNAN,F32_NEG_ZERO,0x7fc00000,EXC_CHK_NONE},
{F32_NEG_QNAN,F32_NEG_ZERO,0xffc00000,EXC_CHK_NONE},
#else
{F32_POS_QNAN,F32_POS_ZERO,0x3F800000,EXC_CHK_NONE},
{F32_NEG_QNAN,F32_POS_ZERO,0x3F800000,EXC_CHK_NONE},
{F32_POS_QNAN,F32_NEG_ZERO,0x3F800000,EXC_CHK_NONE},
{F32_NEG_QNAN,F32_NEG_ZERO,0x3F800000,EXC_CHK_NONE},
#endif
{0xBF800000,0x3FC00000,F32_POS_QNAN,EXC_CHK_INVL},
{0x3F000000,F32_NEG_INF,F32_POS_INF,EXC_CHK_NONE},
{0x3FC00000,F32_NEG_INF,F32_POS_ZERO,EXC_CHK_NONE},
{0x3F000000,F32_POS_INF,F32_POS_ZERO,EXC_CHK_NONE},//40
{0x3FC00000,F32_POS_INF,F32_POS_INF,EXC_CHK_NONE},
{F32_NEG_INF,0xC0400000,F32_NEG_ZERO,EXC_CHK_NONE},
{F32_NEG_INF,0xC0000000,F32_POS_ZERO,EXC_CHK_NONE},
{F32_NEG_INF,0x40400000,F32_NEG_INF,EXC_CHK_NONE},
{F32_NEG_INF,0x40000000,F32_POS_INF,EXC_CHK_NONE},
{F32_POS_INF,0xC0400000,F32_POS_ZERO,EXC_CHK_NONE},
{F32_POS_INF,0x40000000,F32_POS_INF,EXC_CHK_NONE},
{0x7F61B1E5,0x7F167699,F32_POS_INF,EXC_CHK_OVFL},
{0x7F61B1E5,0xFF167699,F32_POS_ZERO,EXC_CHK_UNFL},

/*Additional Test cases for Accuracy Testing*/
    {0x402DF854,0xc2ce8ed0,0x00000001,EXC_CHK_UNFL},//50 //e^ -103.278931
    {0x402DF854,0xc2ce8ed1,0x00000000,EXC_CHK_UNFL}, //e^ -103.278938
    {0x402df854,0x42b16666,0x7f7a37d0,EXC_CHK_NONE},
// ------------------------------------
    //normal numbers
   {0x3F317218,0x3F317218,0x3F469154,EXC_CHK_NONE},  //1/e,1/e
   {0x3FA00000,0x3FA00000,0x3FA92DED,EXC_CHK_NONE},  //(1.25,1.25
   {0x3F866666,0x40600000,0x3F97D5CF,EXC_CHK_NONE},  //1.05, 3.5
   {0x402DF854,0x402DF854,0x417277D9,EXC_CHK_NONE},  //e
   {0x41000000,0x42000000,0x6F800000,EXC_CHK_NONE},  //8,32
   {0x41078000,0x42000000,0x70C5EB5F,EXC_CHK_NONE},  //8.46875,32
   {0x44000000,0x41078000,0x6594F4F0,EXC_CHK_NONE},  //512,33.875
   {0x3F400000,0x40A00000,0x3E730000,EXC_CHK_NONE},//60  //.75,5
   {0x41200000,0x42180000,0x7E967699,EXC_CHK_NONE},  //10,38
   {0x3FA00000,0x3F800000,0x3FA00000,EXC_CHK_NONE},  //1.25,1
   {0xc1200000,0x3F800000,0xC1200000,EXC_CHK_NONE},  //-10, 1
   {0x3D9771FC,0xBC0FC60A,0x3F82F582,EXC_CHK_NONE},  //
   {0x44BFDD2A,0xC0EBD680,0x187F7AB2,EXC_CHK_NONE},  //
   {0x43807BF9,0xC08C4085,0x2DF109A8,EXC_CHK_NONE},  //
   {0xc0400000,0x42000000,0x58D2A9FC,EXC_CHK_NONE},  //-3^32
   {0xc0400000,0x40400000,0xC1D80000,EXC_CHK_NONE},  //-3^3
   {0x3F800000,0x3F800000,0x3F800000,EXC_CHK_NONE},  //1
   {0x44000000,0x44000000,0x7F800000,EXC_CHK_OVFL},//70  //error
   {0x41078000,0x3F800000,0x41078000,EXC_CHK_NONE},  // y == 1
   {0x00000000,0x60000000,0x00000000,EXC_CHK_NONE},  // y is +large, x == 0
   {0x3F000000,0x7f800000,0x00000000,EXC_CHK_NONE},  // y ==+inf ,   x < 1
   {0x3F000000,0x60000000,0x00000000,EXC_CHK_UNFL},  // y is +large, x < 1
   {0xbf800000,0x7f800000,0x3F800000,EXC_CHK_NONE},  // y ==+inf, x == -1
   {0xbf800000,0x60000000,0x3F800000,EXC_CHK_NONE},  // y is +large, x == -1
   {0x40400000,0x60000000,0x7f800000,EXC_CHK_OVFL},  // y is +large, |x| > 1
   {0x40400000,0x7f800000,0x7f800000,EXC_CHK_NONE},  // y ==+inf, |x| > 1
   {0xC0400000,0x60000000,0x7f800000,EXC_CHK_OVFL},  // y is +large, |x| > 1
   {0x41078000,0x7f800000,0x7f800000,EXC_CHK_NONE},//80  // y ==+inf
   {0x41078000,0x60000000,0x7f800000,EXC_CHK_OVFL},  // y is large
   {0x00000000,0xE0000000,0x7f800000,EXC_CHK_ZERO},  // y is -large, |x| ==0
   {0x80000000,0xFF800000,0x7f800000,EXC_CHK_ZERO},  // y ==-inf, |x| == 0
   {0x3F000000,0xE0000000,0x7f800000,EXC_CHK_OVFL},  // y is -large, x < 1
   {0x3F000000,0xFF800000,0x7f800000,EXC_CHK_NONE},  // y ==-inf, x < 1
   {0x3F800000,0xE0000000,0x3F800000,EXC_CHK_NONE},  // y is -large, x == 1
   {0x40000000,0xE0000000,0x00000000,EXC_CHK_UNFL},  // y is -large, x > 1
   {0x41200000,0xc2200000,0x000116C2,EXC_CHK_UNFL},  //denormal
   {0xC1200000,0xc2200000,0x000116C2,EXC_CHK_UNFL},  //negative denormal
   {0x41200000,0xC2380000,0x00000000,EXC_CHK_UNFL},//90  //complete underflow
   {0xC1200000,0xC2380000,0x00000000,EXC_CHK_UNFL},  //complete underflow
   {0x41000000,0x42240000,0x7D000000,EXC_CHK_NONE},  //
   {0xC5000000,0x43010000,0xFF800000,EXC_CHK_OVFL},  //overflow
   {0x41200000,0x10000000,0x3F800000,EXC_CHK_NONE},  //y is very small
// ------------------------------------
   {0x00000000,0xC0400000,0x7F800000,EXC_CHK_ZERO},  //+0 ^ -odd integer	      = +inf
   {0x80000000,0xC0400000,0xFF800000,EXC_CHK_ZERO},  //-0 ^ -odd integer	      = -inf
   {0x00000000,0xc0800000,0x7F800000,EXC_CHK_ZERO},  //+0 ^ -not odd integer   = +inf
   {0x80000000,0xc0080000,0x7F800000,EXC_CHK_ZERO},  //-0 ^ -not odd integer	  = +inf
   {0x00000000,0x40400000,0x00000000,EXC_CHK_NONE},//99  //+0 ^ +odd integer	      = +0
   {0x80000000,0x40400000,0x80000000,EXC_CHK_NONE},  //-0 ^ +odd integer	      = -0
   {0x00000000,0x40800000,0x00000000,EXC_CHK_NONE},  //+0 ^ +not odd integer   = +0
   {0x80000000,0x40080000,0x00000000,EXC_CHK_NONE},  //-0 ^ +not odd integer	  = +0
   {0xbf800000,0x7f800000,0x3F800000,EXC_CHK_NONE},  //-1 ^ +Inf				  = +1


   {0x3F800000,0x7FC00000,0x3F800000,EXC_CHK_NONE},  //+1 ^ NaN	 +1^y = +1 for ALL y, even NaN
   {0x3F800000,0xfFC00000,0x3F800000,EXC_CHK_NONE},  //+1 ^ NaN	 +1^y = +1 for ALL y, even NaN
   {0x3F800000,0x7f800000,0x3F800000,EXC_CHK_NONE},  //+1 ^ +Inf				  = +1
   {0x3F800000,0xFF800000,0x3F800000,EXC_CHK_NONE},  //+1 ^ -Inf				  = +1
   {0x40000000,0x00000000,0x3F800000,EXC_CHK_NONE},  //any x ^ +0 			  = +1
   {0x40000000,0x80000000,0x3F800000,EXC_CHK_NONE},  //any x ^ -0 			  = +1
   {0x7f800000,0x00000000,0x3F800000,EXC_CHK_NONE},  //any x ^ +0 			  = +1
   {0xFF800000,0x80000000,0x3F800000,EXC_CHK_NONE},  //any x ^ -0 			  = +1
#ifdef WINDOWS
   {0x7FC00000,0x00000000,0x7Fc00000,EXC_CHK_NONE},  //any x ^ +0 			  = +1	even NaN
   {0xfFC00000,0x80000000,0xfFc00000,EXC_CHK_NONE},  //any x ^ -0 			  = +1	even NaN
#else
   {0x7FC00000,0x00000000,0x3F800000,EXC_CHK_NONE},  //any x ^ +0 			  = +1	even NaN
   {0xfFC00000,0x80000000,0x3F800000,EXC_CHK_NONE},  //any x ^ -0 			  = +1	even NaN
#endif

   {0xC1200000,0x3FA00000,0x7FC00000,EXC_CHK_INVL},//115  // finite -x ^ finite non integer = NaN
   {0xC1200000,0xbFA00000,0x7FC00000,EXC_CHK_INVL},  // finite -x ^ finite non integer = NaN
   {0x3F400000,0xFF800000,0x7F800000,EXC_CHK_NONE},  // |x|<1 ^ -inf		  = +inf
   {0xbF400000,0xFF800000,0x7F800000,EXC_CHK_NONE},  // |x|<1 ^ -inf		  = +inf
   {0x41200000,0xFF800000,0x00000000,EXC_CHK_NONE},  // |x|>1 ^ -inf		  = +0
   {0xC1260000,0xFF800000,0x00000000,EXC_CHK_NONE},//120  // |x|>1 ^ -inf		  = +0
   {0x3F400000,0x7f800000,0x00000000,EXC_CHK_NONE},  // |x|<1 ^ +inf		  = +0
   {0xbF400000,0x7f800000,0x00000000,EXC_CHK_NONE},  // |x|<1 ^ +inf		  = +0
   {0x41200000,0x7f800000,0x7F800000,EXC_CHK_NONE},  // |x|>1 ^ +inf		  = +inf
   {0xC1260000,0x7f800000,0x7F800000,EXC_CHK_NONE},  // |x|>1 ^ +inf		  = +inf
   {0xFF800000,0xC0400000,0x80000000,EXC_CHK_NONE},  // -inf ^ -odd integer = -0
   {0xFF800000,0xc0800000,0x00000000,EXC_CHK_NONE},  // -inf ^ - not odd integer = +0
   {0xFF800000,0xc0080000,0x00000000,EXC_CHK_NONE},  // -inf ^ - not odd integer = +0
   {0xFF800000,0x40400000,0xFF800000,EXC_CHK_NONE},  // -inf ^ +odd integer = -inf
   {0xFF800000,0x40800000,0x7F800000,EXC_CHK_NONE},  // -inf ^ +not odd integer = +inf
   {0xFF800000,0x40F00000,0x7F800000,EXC_CHK_NONE},//130  // -inf ^ +not odd integer = +inf
   {0x7f800000,0xC0400000,0x00000000,EXC_CHK_NONE},  // +inf ^ y<0 = +0
   {0x7f800000,0xc0800000,0x00000000,EXC_CHK_NONE},  // +inf ^ y<0 = +0
   {0x7f800000,0xc0080000,0x00000000,EXC_CHK_NONE},  // +inf ^ y<0 = +0
   {0x7f800000,0x40400000,0x7F800000,EXC_CHK_NONE},  // +inf ^ y<0 = +inf
   {0x7f800000,0x40800000,0x7F800000,EXC_CHK_NONE},  // +inf ^ y<0 = +inf
   {0x7f800000,0x40080000,0x7F800000,EXC_CHK_NONE},  // +inf ^ y<0 = +inf
   {0x7f800000,0x7FE80000,0x7FE80000,EXC_CHK_NONE},  // +inf ^ NaN = NaN
   {0x40000000,0x7FF80000,0x7FF80000,EXC_CHK_NONE},  // 2 ^ NaN = NaN
   {0xFF800000,0x40400000,0xFF800000,EXC_CHK_NONE},  // -inf ^ +odd integer = -inf
   {0x3F317218,0x3F317218,0x3F469154,EXC_CHK_NONE},//140  //1/e,1/e
   {0xc0400000,0x42000000,0x58D2A9FC,EXC_CHK_NONE},  //-3^32
   {0x40c09882,0xc24e46dd,0x0000b0b0,EXC_CHK_UNFL},  //
   {0x40000000,0x40000000,0x40800000,EXC_CHK_NONE},  //
   {0x40000000,0x40800000,0x41800000,EXC_CHK_NONE},  //
   {0x40800000,0x40000000,0x41800000,EXC_CHK_NONE},  //
   {0x400f0000,0x40800000,0x41C7651E,EXC_CHK_NONE},  //
   {0x40800000,0x400f0000,0x41B123F6,EXC_CHK_NONE},  //
   {0x3DCCCCCD,0x3f800000,0x3DCCCCCD,EXC_CHK_NONE},  //
   {0x402DF854,0xc2Aeac4f,0x0080003C,EXC_CHK_NONE},  //e^ -87.3365 normal
   {0x402DF854,0xc2Aeac50,0x007FFFFC,EXC_CHK_UNFL},//150  //e^ -87.3365 denormal
   {0x402DF854,0x42b17218,0x7F7FFFD7,EXC_CHK_NONE},  //e^ 88.7228 no overflow
   {0x402DF854,0x42b17219,0x7F800000,EXC_CHK_OVFL},  //e^ 88.7229 overflow
   {0x402DF854,0xc2cff1b4,0x00000001,EXC_CHK_UNFL},  //e^ -87.3365 smallest denormal
   {0x402DF854,0xc2cff1b5,0x00000000,EXC_CHK_UNFL},  //e^ -87.3365 underflow
   {0xBE139078,0xCBFBA2CE,0x7F800000,EXC_CHK_OVFL},  //-.14, 3.3E7 large even integer  = +inf
   {0xBE139078,0xCB7BA2CF,0xfF800000,EXC_CHK_OVFL},  //-.14,-1.6E6 large odd integer  = -inf
   {0x3E139078,0x4BFBA2CE,0x00000000,EXC_CHK_UNFL},  //.14, 3.3E7 large number  = 0
   {0x3E139078,0xCBFBA2CE,0x7F800000,EXC_CHK_OVFL},  //-.14, 3.3E7 large even integer  = -0
   {0x3E139078,0xCB7BA2CF,0x7F800000,EXC_CHK_OVFL},  //-.14, 1.6E6 large odd integer  = -inf
   {0x3E139078,0xCB7BA2CF,0x7F800000,EXC_CHK_OVFL},//160  //.14,-1.6E6 large odd integer  = inf
   {0x3f200000,0x4b000001,0x00000000,EXC_CHK_UNFL},  //y could be odd
   {0x3f200000,0x4b800000,0x00000000,EXC_CHK_UNFL},  //y must be even, no fractional bits
    {F32_POS_ZERO,0xc3740000,F32_POS_INF,EXC_CHK_ZERO},
    {F32_NEG_ZERO,0xc8a8c980,F32_POS_INF,EXC_CHK_ZERO},
};

