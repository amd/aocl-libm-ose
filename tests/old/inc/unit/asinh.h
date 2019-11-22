//R.obtained=FN_PROTOTYPE(asinh)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = asinh(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_asinh_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
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
    {0x43b3c4eafedcab02LL, 0x40453f2b7feeadccLL, EXC_CHK_NONE}, // normal intermediate
    {0x7FEFFFFFFFFFFFFFLL, 0x408633ce8fb9f87eLL, EXC_CHK_NONE}, // normal max
    {0x8010000000000000LL, 0x8010000000000000LL, EXC_CHK_UNFL}, // -normal min
    {0xc5812e71245acfdbLL, 0xc04f3a2c96b94100LL, EXC_CHK_NONE}, // -normal intermediate
    {0xFFEFFFFFFFFFFFFFLL, 0xc08633ce8fb9f87eLL, EXC_CHK_NONE}, // -normal max															
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
    {0x3FF921FB54442D18LL, 0x3ff3bc04e847ec05LL, EXC_CHK_NONE}, // pi/2
    {0x400921FB54442D18LL, 0x3ffdcbf69f10006dLL, EXC_CHK_NONE}, // pi
    {0x401921FB54442D18LL, 0x40044c62a1e4f804LL, EXC_CHK_NONE}, // 2pi
    {0x3FFB7E151628AED3LL, 0x3ff4f5f9f10fb81fLL, EXC_CHK_NONE}, // e --
    {0x4005BF0A8B145769LL, 0x3ffb9b2abdf5c077LL, EXC_CHK_NONE}, // e
    {0x400DBF0A8B145769LL, 0x40003131303a2543LL, EXC_CHK_NONE}, // e ++															
    {0x0000000000000000LL, 0x0000000000000000LL, EXC_CHK_NONE}, // 0
    {0x3C4536B8B14B676CLL, 0x3c4536b8b14b676cLL, EXC_CHK_UNFL}, // 0.0000000000000000023
    {0x3FDFFFFBCE4217D3LL, 0x3fdecc28ee62c343LL, EXC_CHK_NONE}, // 0.4999989999999999999
    {0x3FE000000000006CLL, 0x3fdecc2caec516cbLL, EXC_CHK_NONE}, // 0.500000000000012
    {0x8000000000000000LL, 0x8000000000000000LL, EXC_CHK_NONE}, // -0
    {0xBBDB2752CE74FF42LL, 0xbbdb2752ce74ff42LL, EXC_CHK_UNFL}, // -0.000000000000000000023
    {0xBFDFFFFBCE4217D3LL, 0xbfdecc28ee62c343LL, EXC_CHK_NONE}, // -0.4999989999999999999
    {0xBFE000000000006CLL, 0xbfdecc2caec516cbLL, EXC_CHK_NONE}, // -0.500000000000012
    {0x3FF0000000000000LL, 0x3fec34366179d427LL, EXC_CHK_NONE}, // 1
    {0x3FEFFFFFC49BD0DCLL, 0x3fec3436377ad8c2LL, EXC_CHK_NONE}, // 0.9999998893750006
    {0x3FF0000000000119LL, 0x3fec34366179d5b4LL, EXC_CHK_NONE}, // 1.0000000000000624998
    {0x3FF7FFFEF39085F4LL, 0x3ff31dbf7424ac8bLL, EXC_CHK_NONE}, // 1.499998999999999967
    {0x3FF8000000000001LL, 0x3ff31dc0090b63d9LL, EXC_CHK_NONE}, // 1.50000000000000012
    {0xBFF0000000000000LL, 0xbfec34366179d427LL, EXC_CHK_NONE}, // -1
    {0xBFEFFFFFC49BD0DCLL, 0xbfec3436377ad8c2LL, EXC_CHK_NONE}, // -0.9999998893750006
    {0xBFF0000000000119LL, 0xbfec34366179d5b4LL, EXC_CHK_NONE}, // -1.0000000000000624998
    {0xBFF7FFFEF39085F4LL, 0xbff31dbf7424ac8bLL, EXC_CHK_NONE}, // -1.499998999999999967
    {0xBFF8000000000001LL, 0xbff31dc0090b63d9LL, EXC_CHK_NONE}, // -1.50000000000000012															
    {0x4000000000000000LL, 0x3ff719218313d087LL, EXC_CHK_NONE}, // 2
    {0xC000000000000000LL, 0xbff719218313d087LL, EXC_CHK_NONE}, // -2
    {0x4024000000000000LL, 0x4007fc5c506d2bdbLL, EXC_CHK_NONE}, // 10
    {0xC024000000000000LL, 0xc007fc5c506d2bdbLL, EXC_CHK_NONE}, // -10
    {0x408F400000000000LL, 0x401e67530a363e15LL, EXC_CHK_NONE}, // 1000
    {0xC08F400000000000LL, 0xc01e67530a363e15LL, EXC_CHK_NONE}, // -1000
    {0x4050D9999999999ALL, 0x40139d8a209a9bcaLL, EXC_CHK_NONE}, // 67.4
    {0xC050D9999999999ALL, 0xc0139d8a209a9bcaLL, EXC_CHK_NONE}, // -67.4
    {0x409EFE6666666666LL, 0x4020925676234ba5LL, EXC_CHK_NONE}, // 1983.6
    {0xC09EFE6666666666LL, 0xc020925676234ba5LL, EXC_CHK_NONE}, // -1983.6
    {0x4055E00000000000LL, 0x4014a8c636b735bfLL, EXC_CHK_NONE}, // 87.5
    {0xC055E00000000000LL, 0xc014a8c636b735bfLL, EXC_CHK_NONE}, // -87.5
    {0x41002BC800000000LL, 0x4028f97cef9a3debLL, EXC_CHK_NONE}, // 132473 
    {0xC1002BC800000000LL, 0xc028f97cef9a3debLL, EXC_CHK_NONE}, // -132473
    {0x4330000000000000LL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52
    {0x4330000000000001LL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52 + 1 
    {0x432FFFFFFFFFFFFFLL, 0x40425e4f7b2737faLL, EXC_CHK_NONE}, // 2^52 -1 + 0.5
    {0xC330000000000000LL, 0xc0425e4f7b2737faLL, EXC_CHK_NONE}, // -2^52
    {0xC330000000000001LL, 0xc0425e4f7b2737faLL, EXC_CHK_NONE}, // -(2^52 + 1) 
    {0xC32FFFFFFFFFFFFFLL, 0xc0425e4f7b2737faLL, EXC_CHK_NONE}, // -(2^52 -1 + 0.5)     

	// special accuracy tests    
	{0x3e30000000000000LL, 0x3e30000000000000LL, EXC_CHK_UNFL}, // 
    {0x3E2FFFFFFFFFFFFFLL, 0x3e2fffffffffffffLL, EXC_CHK_UNFL}, // 
    {0x3e30000000000001LL, 0x3e30000000000001LL, EXC_CHK_UNFL}, // 
    {0xFE37E43C8800759CLL, 0xc0859bbfd8b83e44LL, EXC_CHK_NONE}, // 
    {0xFE37E43C8800758CLL, 0xc0859bbfd8b83e44LL, EXC_CHK_NONE}, // 
    {0xFE37E43C880075ACLL, 0xc0859bbfd8b83e44LL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f87eLL, 0x401d094cf61c34cdLL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f87dLL, 0x401d094cf61c34cdLL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f87fLL, 0x401d094cf61c34ceLL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f8ffLL, 0x401d094cf61c34e5LL, EXC_CHK_NONE}, // 
    {0x408633ce8fb9f800LL, 0x401d094cf61c34b7LL, EXC_CHK_NONE}, // 
    {0x4034000000000000LL, 0x400d841a9abe2ebbLL, EXC_CHK_NONE}, // 
    {0x4035000000000000LL, 0x400de7e84ab59e17LL, EXC_CHK_NONE}, // 
    {0x4033000000000000LL, 0x400d1b3179ec4463LL, EXC_CHK_NONE}, //      
    {0x3e46a09e667f3bcdLL, 0x3e46a09e667f3bcdLL, EXC_CHK_NONE}, //
	{0x3e46a09e667f3bceLL, 0x3e46a09e667f3bceLL, EXC_CHK_NONE},	//
	{0x3e46a09e667f3bccLL, 0x3e46a09e667f3bccLL, EXC_CHK_UNFL},	//
    {0x4196a09e667f3bcdLL, 0x40330fc1931f09caLL, EXC_CHK_NONE},	//
	{0x4196a09e667f3bceLL, 0x40330fc1931f09caLL, EXC_CHK_NONE},	//
	{0x4196a09e667f3bccLL, 0x40330fc1931f09caLL, EXC_CHK_NONE},	//
    {0x3fe62e42e0000000LL, 0x3fe4b4946e555844LL, EXC_CHK_NONE},	//
	{0x3e6efa39ef35793cLL, 0x3e6efa39ef357937LL, EXC_CHK_NONE},	//
    {0x0000000000000000LL, 0x0000000000000000LL, EXC_CHK_NONE},	//
    {0x8000000000000000LL, 0x8000000000000000LL, EXC_CHK_NONE},	//
    {0x3ff0000000000000LL, 0x3fec34366179d427LL, EXC_CHK_NONE},	//
	{0x3ff0000000000001LL, 0x3fec34366179d428LL, EXC_CHK_NONE},	//
	{0x3fefffffffffffffLL, 0x3fec34366179d426LL, EXC_CHK_NONE},	//
    {0x3fd0000000000000LL, 0x3fcfacfb2399e637LL, EXC_CHK_NONE},	//
	{0x3fd0000000000001LL, 0x3fcfacfb2399e639LL, EXC_CHK_NONE}, //
	{0x3fefffffffffffffLL, 0x3fec34366179d426LL, EXC_CHK_NONE},	//
    {0xbfd0000000000000LL, 0xbfcfacfb2399e637LL, EXC_CHK_NONE},	//
	{0xbfd0000000000001LL, 0xbfcfacfb2399e639LL, EXC_CHK_NONE},	//
	{0xbfefffffffffffffLL, 0xbfec34366179d426LL, EXC_CHK_NONE},	//
    {0x3fe0000000000000LL, 0x3fdecc2caec5160aLL, EXC_CHK_NONE},	//
	{0x3fe0000000000001LL, 0x3fdecc2caec5160bLL, EXC_CHK_NONE},	//
	{0x3fdfffffffffffffLL, 0x3fdecc2caec51609LL, EXC_CHK_NONE},	//
    {0xbfe0000000000000LL, 0xbfdecc2caec5160aLL, EXC_CHK_NONE},	//
	{0xbfe0000000000001LL, 0xbfdecc2caec5160bLL, EXC_CHK_NONE},	//
	{0xbfdfffffffffffffLL, 0xbfdecc2caec51609LL, EXC_CHK_NONE},	//
    {0x3fe8000000000000LL, 0x3fe62e42fefa39efLL, EXC_CHK_NONE},	//
	{0x3fe8000000000001LL, 0x3fe62e42fefa39f0LL, EXC_CHK_NONE},	//
	{0x3fe7ffffffffffffLL, 0x3fe62e42fefa39eeLL, EXC_CHK_NONE},	//
    {0xbfe8000000000000LL, 0xbfe62e42fefa39efLL, EXC_CHK_NONE}, //
	{0xbfe8000000000001LL, 0xbfe62e42fefa39f0LL, EXC_CHK_NONE},	//
	{0xbfe7ffffffffffffLL, 0xbfe62e42fefa39eeLL, EXC_CHK_NONE},	//
    {0x4040000000000000LL, 0x4010a2f2393c80a6LL, EXC_CHK_NONE},	//
	{0x4040000000000001LL, 0x4010a2f2393c80a6LL, EXC_CHK_NONE},	//
	{0x403fffffffffffffLL, 0x4010a2f2393c80a6LL, EXC_CHK_NONE},	//
    {0xc040000000000000LL, 0xc010a2f2393c80a6LL, EXC_CHK_NONE},	//
	{0xc040000000000001LL, 0xc010a2f2393c80a6LL, EXC_CHK_NONE},	//
	{0xc03fffffffffffffLL, 0xc010a2f2393c80a6LL, EXC_CHK_NONE},	//
    {0x4020000000000000LL, 0x40063637195fabd3LL, EXC_CHK_NONE},	//
	{0x4020000000000001LL, 0x40063637195fabd4LL, EXC_CHK_NONE},	//
	{0x401fffffffffffffLL, 0x40063637195fabd3LL, EXC_CHK_NONE},	//
    {0xc020000000000000LL, 0xc0063637195fabd3LL, EXC_CHK_NONE},	//
	{0xc020000000000001LL, 0xc0063637195fabd4LL, EXC_CHK_NONE},	//
	{0xc01fffffffffffffLL, 0xc0063637195fabd3LL, EXC_CHK_NONE}, //
    {0x4010000000000000LL, 0x4000c1f8a6e80eebLL, EXC_CHK_NONE},	//
	{0x4010000000000001LL, 0x4000c1f8a6e80eecLL, EXC_CHK_NONE},	//
	{0x400fffffffffffffLL, 0x4000c1f8a6e80eebLL, EXC_CHK_NONE},	//
    {0xc010000000000000LL, 0xc000c1f8a6e80eebLL, EXC_CHK_NONE},	//
	{0xc010000000000001LL, 0xc000c1f8a6e80eecLL, EXC_CHK_NONE},	//
	{0xc00fffffffffffffLL, 0xc000c1f8a6e80eebLL, EXC_CHK_NONE},	//
    {0x4000000000000000LL, 0x3ff719218313d087LL, EXC_CHK_NONE},	//
	{0x4000000000000001LL, 0x3ff719218313d088LL, EXC_CHK_NONE},	//
	{0x3ffffffffffffff2LL, 0x3ff719218313d081LL, EXC_CHK_NONE},	//
    {0x3ff8000000000000LL, 0x3ff31dc0090b63d8LL, EXC_CHK_NONE},	//
    {0x3ff8000000000001LL, 0x3ff31dc0090b63d9LL, EXC_CHK_NONE},	//
	{0x3ff7ffffffffffffLL, 0x3ff31dc0090b63d8LL, EXC_CHK_NONE},	//
    {0xc000000000000000LL, 0xbff719218313d087LL, EXC_CHK_NONE},	//
	{0xc000000000000001LL, 0xbff719218313d088LL, EXC_CHK_NONE}, //
	{0xbffffffffffffff2LL, 0xbff719218313d081LL, EXC_CHK_NONE}, // 
    {0xbff8000000000000LL, 0xbff31dc0090b63d8LL, EXC_CHK_NONE}, //
    {0xbff8000000000001LL, 0xbff31dc0090b63d9LL, EXC_CHK_NONE}, //
	{0xbff7ffffffffffffLL, 0xbff31dc0090b63d8LL, EXC_CHK_NONE}, //
	{0x3FF4F5C28F5C28F6LL, 0x3ff15a3fc3cf331aLL, EXC_CHK_NONE}, //

	//test case reported
    {0xc1fb38337088c7f1, 0xc03767bb64b7dd0e, EXC_CHK_NONE},
    {0xc224247d2d4d9ff3, 0xc0392efcb56cdffb, EXC_CHK_NONE},
    {0xc204f156259e9262, 0xc037d611a1d12f97, EXC_CHK_NONE},
    {0xc1fa4fae1473429e, 0xc0375f0b366c4db8, EXC_CHK_NONE},
    {0xc1cf7c7e5fb74763, 0xc03578ac50d7e318, EXC_CHK_NONE},
    {0xc1c8a62035b79519, 0xc03539ffd87df36f, EXC_CHK_NONE},

};
