//R.obtained=FN_PROTOTYPE(acosh)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_acosh(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_acosh_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

    {0x0000000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // denormal min
    {0x0005fde623545abcLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // denormal intermediate
    {0x000FFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // denormal max
    {0x8000000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -denormal min
    {0x8002344ade5def12LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -denormal intermediate
    {0x800FFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -denormal max
    {0x0010000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // normal min
    {0x43b3c4eafedcab02LL, 0x40453f2b7feeadccLL, EXC_CHK_NONE}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x408633ce8fb9f87eLL, EXC_CHK_NONE}, // normal max
    {0x8010000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -normal min
    {0xc5812e71245acfdbLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -normal max															
    {0x7FF0000000000000LL, 0x7ff0000000000000LL, EXC_CHK_NONE}, // inf
    {0xFFF0000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -inf
    {0x7FF8000000000000LL, 0x7ff8000000000000LL, EXC_CHK_NONE}, // qnan min
    {0x7FFe1a5701234dc3LL, 0x7ffe1a5701234dc3LL, EXC_CHK_NONE}, // qnan intermediate
    {0x7FFFFFFFFFFFFFFFLL, 0x7fffffffffffffffLL, EXC_CHK_NONE}, // qnan max
    {0xFFF8000000000000LL, 0xfff8000000000000LL, EXC_CHK_NONE}, // indeterninate
    {0xFFF8000000000001LL, 0xfff8000000000000LL, EXC_CHK_NONE}, // -qnan min
    {0xFFF9123425dcba31LL, 0xfff8000000000000LL, EXC_CHK_NONE}, // -qnan intermediate
    {0xFFFFFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_NONE}, // -qnan max															
    {0x7FF0000000000001LL, 0x7ff8000000000001LL, EXC_CHK_INVL}, // snan min
    {0x7FF5344752a0bd90LL, 0x7ffd344752a0bd90LL, EXC_CHK_INVL}, // snan intermediate
    {0x7FF7FFFFFFFFFFFFLL, 0x7fffffffffffffffLL, EXC_CHK_INVL}, // snan max
    {0xFFF0000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -snan min
    {0xfFF432438995fffaLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -snan intermediate
    {0xFFF7FFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -snan max															
    {0x3FF921FB54442D18LL, 0x3ff05f23c6cbaf31LL, EXC_CHK_NONE}, // pi/2
    {0x400921FB54442D18LL, 0x3ffcfc02f90106c1LL, EXC_CHK_NONE}, // pi
    {0x401921FB54442D18LL, 0x400432718f372526LL, EXC_CHK_NONE}, // 2pi
    {0x3FFB7E151628AED3LL, 0x3ff22ec7e51925b9LL, EXC_CHK_NONE}, // e --
    {0x4005BF0A8B145769LL, 0x3ffa84eef63326e7LL, EXC_CHK_NONE}, // e
    {0x400DBF0A8B145769LL, 0x3fffce1775f12536LL, EXC_CHK_NONE}, // e ++															
    {0x0000000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 0
    {0x3C4536B8B14B676CLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 0.500000000000012
    {0x8000000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -0
    {0xBBDB2752CE74FF42LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x0000000000000000LL, EXC_CHK_NONE}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3e97b4dfc079256bLL, EXC_CHK_NONE}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x3feecc2ace93e00eLL, EXC_CHK_NONE}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x3feecc2caec5160bLL, EXC_CHK_NONE}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1.50000000000000012															
    {0x4000000000000000LL, 0x3ff5124271980435LL, EXC_CHK_NONE}, // 2
    {0xC000000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -2
    {0x4024000000000000LL, 0x4007f21ed1ce05d4LL, EXC_CHK_NONE}, // 10
    {0xC024000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -10
    {0x408F400000000000LL, 0x401e6752e8a84ed4LL, EXC_CHK_NONE}, // 1000
    {0xC08F400000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1000
    {0x4050D9999999999ALL, 0x40139d6d46400cc2LL, EXC_CHK_NONE}, // 67.4
    {0xC050D9999999999ALL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -67.4
    {0x409EFE6666666666LL, 0x4020925671dfb9aeLL, EXC_CHK_NONE}, // 1983.6
    {0xC09EFE6666666666LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -1983.6
    {0x4055E00000000000LL, 0x4014a8b5181890bdLL, EXC_CHK_NONE}, // 87.5
    {0xC055E00000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -87.5
    {0x41002BC800000000LL, 0x4028f97cef99ff43LL, EXC_CHK_NONE}, // 132473 
    {0xC1002BC800000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -132473
    {0x4330000000000000LL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52
    {0x4330000000000001LL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52 + 1 
    {0x432FFFFFFFFFFFFFLL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -2^52
    {0xC330000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -(2^52 + 1) 
    {0xC32FFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // -(2^52 -1 + 0.5)
    
	// special accuracy tests
	{0x3e30000000000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0x3E2FFFFFFFFFFFFFLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0x3e30000000000001LL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0xFE37E43C8800759CLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0xFE37E43C8800758CLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0xFE37E43C880075ACLL, 0xfff8000000000000LL, EXC_CHK_INVL}, // 
    {0x408633ce8fb9f87eLL, 0x401d094cb3a2e27fLL, EXC_CHK_NONE}, // 
    {0x7feffffffffffd3bLL, 0x408633ce8fb9f87dLL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f87fLL, 0x401d094cb3a2e27fLL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f8ffLL, 0x401d094cb3a2e296LL, EXC_CHK_NONE}, // 
    {0x7feffffffffe093bLL, 0x408633ce8fb9f800LL, EXC_CHK_NONE}, // 
    {0x41aceb088b68e804LL, 0x4034000000000000LL, EXC_CHK_NONE}, // 
    {0x41c3a6e1fd9eecfdLL, 0x4035000000000000LL, EXC_CHK_NONE}, // 
    {0x419546d8f9ed26e2LL, 0x4033000000000000LL, EXC_CHK_NONE}, // 
    {0x4196a09e667f3bcdLL, 0x40330fc1931f09caLL, EXC_CHK_NONE}, //
	{0x4196a09e667f3bccLL, 0x40330fc1931f09caLL, EXC_CHK_NONE}, //
	{0x4196a09e667f3bceLL, 0x40330fc1931f09caLL, EXC_CHK_NONE}, //
	{0x3fe62e42e0000000LL, 0xfff8000000000000LL, EXC_CHK_INVL}, //
	{0x3e6efa39ef35793cLL, 0xfff8000000000000LL, EXC_CHK_INVL}, //
	{0x3FF0000000000000LL, 0x0000000000000000LL, EXC_CHK_NONE}, //
	{0x3FF0000000000001LL, 0x3e56a09e667f3bccLL, EXC_CHK_NONE}, //
	{0x3FEFFFFFFFFFFFF7LL, 0xfff8000000000000LL, EXC_CHK_INVL}, //
	{0x4060000000000000LL, 0x40162e3efef439e2LL, EXC_CHK_NONE}, //
	{0x4060000000000001LL, 0x40162e3efef439e2LL, EXC_CHK_NONE}, //
	{0x405fffffffffffffLL, 0x40162e3efef439e2LL, EXC_CHK_NONE}, //
	{0x3ff8000000000000LL, 0x3feecc2caec5160aLL, EXC_CHK_NONE}, //
	{0x3ff8000000000001LL, 0x3feecc2caec5160bLL, EXC_CHK_NONE}, //
	{0x3ff7ffffffffffffLL, 0x3feecc2caec51608LL, EXC_CHK_NONE}, //
	{0x4040000000000000LL, 0x4010a272393ad5fbLL, EXC_CHK_NONE}, //
	{0x4040000000000001LL, 0x4010a272393ad5fbLL, EXC_CHK_NONE}, //
	{0x403fffffffffffffLL, 0x4010a272393ad5fbLL, EXC_CHK_NONE}, //
	{0x4020000000000000LL, 0x40062636e408c33bLL, EXC_CHK_NONE}, //
	{0x4020000000000001LL, 0x40062636e408c33cLL, EXC_CHK_NONE}, //
	{0x401fffffffffffffLL, 0x40062636e408c33bLL, EXC_CHK_NONE}, //
	{0x4010000000000000LL, 0x400081eb4b421591LL, EXC_CHK_NONE}, //
	{0x4010000000000001LL, 0x400081eb4b421592LL, EXC_CHK_NONE}, //
	{0x400fffffffffffffLL, 0x400081eb4b421591LL, EXC_CHK_NONE}, //
	{0x4000000000000000LL, 0x3ff5124271980435LL, EXC_CHK_NONE}, //
	{0x4000000000000001LL, 0x3ff5124271980436LL, EXC_CHK_NONE}, //
	{0x3FFFFFFFFFFFFFFBLL, 0x3ff5124271980432LL, EXC_CHK_NONE},	//
    {0x3ffc000000000000LL, 0x3ff28a7cbb850063LL, EXC_CHK_NONE}, //
	{0x3ffc000000000001LL, 0x3ff28a7cbb850063LL, EXC_CHK_NONE}, //
	{0x3ffbffffffffffffLL, 0x3ff28a7cbb850062LL, EXC_CHK_NONE}, //
	{0x3FF8000000000000LL, 0x3feecc2caec5160aLL, EXC_CHK_NONE},	//
	{0x3FFC000000000000LL, 0x3ff28a7cbb850063LL, EXC_CHK_NONE},	//
	{0x3FFAAA88C50B0C79LL, 0x3ff193d10e5c0d18LL, EXC_CHK_NONE},	//
	{0x3FF1FA2163FDD65ALL, 0x3fdf7f49962df105LL, EXC_CHK_NONE}, //
	{0x3FF2147AE147AE14LL, 0x3fe02523a6a33560LL, EXC_CHK_NONE},	//
	{0x3FF0F5C28F5C28F6LL, 0x3fd60f9522de1c73LL, EXC_CHK_NONE},	//
	{0x3FF0DAD538AC18F8LL, 0x3fd4d4139c60af76LL, EXC_CHK_NONE},	//
	{0x3FF17961804D9839LL, 0x3fdb442906f1bc39LL, EXC_CHK_NONE},	//
	{0x3FF3C47C30D306A3LL, 0x3fe58ceae7b87440LL, EXC_CHK_NONE},	//
	{0xc196a09e667f3bcdLL, 0xfff8000000000000LL, EXC_CHK_INVL}, //
	{0x3ff0000000000000LL, 0x0000000000000000LL, EXC_CHK_NONE}, //
	{0x400b0902de00d1b7LL, 0x3ffe360fd1e19969LL, EXC_CHK_NONE}, //
	{0x401b0902de00d1b7LL, 0x4004c9acee4a3151LL, EXC_CHK_NONE}, //
	{0x402646c226809d4aLL, 0x4008cfe2a958feebLL, EXC_CHK_NONE}, //
    {0x403edcb09e98dcdbLL, 0x40107d59b557e190LL, EXC_CHK_NONE},	//
	{0x40507b8bd66277c4LL, 0x401386d999aa5105LL, EXC_CHK_NONE}, //
	{0x405d15f75104d552LL, 0x4015cc772a14b8b0LL, EXC_CHK_NONE}, //
	{0x4070b36657b84dbaLL, 0x40191fefb51bd5acLL, EXC_CHK_NONE}, //
	{0x4080344e3ffef391LL, 0x401bc6d1deb359c0LL, EXC_CHK_NONE},	//
	{0x408b3290268900c6LL, 0x401dd918b117fc4fLL, EXC_CHK_NONE},	//
	{0x4094aa3c6fbd273eLL, 0x401f859a250cdb87LL, EXC_CHK_NONE},	//
	{0x41092092ba93c86bLL, 0x4029db248521d916LL, EXC_CHK_NONE}, //
	{0x42237a59f4429268LL, 0x40392665e1572e41LL, EXC_CHK_NONE},	//
	{0x4457b62df67fc4fdLL, 0x4048cdac82cb51b7LL, EXC_CHK_NONE},	//
	{0x48c191ef3d6a018aLL, 0x4058a14ffccd501eLL, EXC_CHK_NONE},	//
	{0x51934b4f70b3a04cLL, 0x40688b21b9ce55e4LL, EXC_CHK_NONE},	//
	{0x6337443f25f59cbbLL, 0x4078800a984ed8c7LL, EXC_CHK_NONE},	//


};

