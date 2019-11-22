//R.obtained=FN_PROTOTYPE(tanpi)(P1.input)
//double,double,ExcFlags
//P1 R E
//unsigned long long int|double, unsigned  long long int|double,ExcFlags|ExcFlags
//R.V3[j] = amd_ref_tanpi(P1.V3[j])
//double double ExcFlags
//ULP amd_ref_tanpi_ULP(P1.V3[j],R.V3[j],&testdata[k].R.ulp[j], &testdata[k].R.relative_error[j])
//testdata[j].R.MaxUlp()

static struct data input[] =
{

	{F64_POS_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_NEG_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
	{F64_POS_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_NEG_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {0x000000000000023DLL	,0x708               ,EXC_CHK_UNFL},
	{0x4f6344a970d9ac13		,0x0                 ,EXC_CHK_NONE},
	{0x723743730464dbf1		,0x0                 ,EXC_CHK_NONE},
    {0x3FF10CA44655D48ALL	,0x3fcac0e68dc2a71f  ,EXC_CHK_NONE},	// 1.0655863521072058
	{0x400f6a7a2955385e		,0xbfcde277ebb1494a  ,EXC_CHK_NONE},
    {0x3FF921FB54442D18		,0xc011afdfd06f6690  ,EXC_CHK_NONE},
    {0x3FF921FB54442D19		,0xc011afdfd06f6680  ,EXC_CHK_NONE},
    {0x3FF921FB54442D17		,0xc011afdfd06f66a0  ,EXC_CHK_NONE},
    {0x3FF921FB54442D1a		,0xc011afdfd06f666f  ,EXC_CHK_NONE},
	{F64_POS_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_NEG_SNAN			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_POS_INF			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_NEG_INF			,0x7ff8000000000001  ,EXC_CHK_INVL },	//
    {F64_POS_QNAN			,0x7ff8000000000001  ,EXC_CHK_NONE },	//
    {F64_NEG_QNAN			,0x7ff8000000000001  ,EXC_CHK_NONE },	//
    {F64_POS_ZERO			,0x0000000000000000  ,EXC_CHK_NONE },	//
    {F64_NEG_ZERO			,0x8000000000000000  ,EXC_CHK_NONE },	//
    {0x000000000000023DLL	,0x708               ,EXC_CHK_UNFL},
    {0x3e38000000000000LL	,0x3e52d97c7f3321d3  ,EXC_CHK_UNFL},	// 5.5879354476928711E-09 = 2^-28*1.5
    {0xbe38000000000000LL	,0xbe52d97c7f3321d3  ,EXC_CHK_UNFL},	// -5.5879354476928711E-09 = -2^-28*1.5
    {0x3fe921fb54442d18LL	,0xbfe99238e68b5506  ,EXC_CHK_NONE},
    {0x3f20000000000001LL	,0x3f3921fb68efea14  ,EXC_CHK_NONE},
    {0x3e40000000000001LL	,0x3e5921fb54442d1b  ,EXC_CHK_NONE},
    {0x7ff8000000000000LL	,0x7ff8000000000001  ,EXC_CHK_NONE},	// +Qnan
    {0x7ffdf00000000000LL	,0x7ff8000000000001  ,EXC_CHK_NONE},	// +Qnan
    {0xfffdf00000000000LL	,0x7ff8000000000001  ,EXC_CHK_NONE},	// -Qnan
    {0xfff8000000000000LL	,0x7ff8000000000001  ,EXC_CHK_NONE},	// -Qnan
    {0x7ff4000000000000LL	,0x7ff8000000000001  ,EXC_CHK_INVL},	// +Snan
    {0xfff4000000000000LL	,0x7ff8000000000001  ,EXC_CHK_INVL},	// -Snan
    {0x3FE9E0C8F112AB1ELL	,0xbfe5f0b7aa7a57e6  ,EXC_CHK_NONE},	// 0.80868956646495227
    {0x40306B51F0157E66LL	,0x400ed7f4c6fed1f8  ,EXC_CHK_NONE},	// 16.41921902203321
    {0x402DDF5ADB92C01ALL	,0xbfc9fce6d480bf5e  ,EXC_CHK_NONE},	// 14.93624006430041
    {0x402DDB778A9EBD8ALL	,0xbfcd2ef6568311a9  ,EXC_CHK_NONE},	// 14.928646404149521
    {0x401c462b9064a63bLL	,0x3fcbfd4fae79322c  ,EXC_CHK_NONE},	// 7.0685255586081963
    {0x3FE921FB54442D19LL	,0xbfe99238e68b5501  ,EXC_CHK_NONE},	// 0.78539816339744839
    {0x3fe921fb54442d20LL	,0xbfe99238e68b54dd  ,EXC_CHK_NONE},	// 0.78539816339744917
    {0x3FF10CA44655D48ALL	,0x3fcac0e68dc2a71f  ,EXC_CHK_NONE},	// 1.0655863521072058
    {0x400921FB54442D18LL	,0x3fde821762e1e1fe  ,EXC_CHK_NONE},	// 3.1415926535897931 = pi
    {0x400923E979F8B36ALL	,0x3fdebdb6534e9153  ,EXC_CHK_NONE},	// 3.1425351647398445
    {0x4002DAE59BB5C33ELL	,0x400093dbc20b4d78  ,EXC_CHK_NONE},	// 2.3568832554668964
    {0x4015FDCA5F9A0E38LL	,0x406270ab5c551529  ,EXC_CHK_NONE},	// 5.4978423059251824
    {0x40b93bda357daddaLL	,0xbfe001364b696afd  ,EXC_CHK_NONE},	// 6459.8523787068789
    {0x40F63525129291FFLL	,0x3ff8b3438bc3e312  ,EXC_CHK_NONE},	// 90962.317034311578
    {0x3FF9207824B27C17LL	,0xc011c869c493eb38  ,EXC_CHK_NONE},	// 1.5704270776257141
    {0x4025fe9b31eb183dLL	,0xbf8183da6f63afbb  ,EXC_CHK_NONE},	// 10.997277793834764
    {0x4046C6CBC45DC8DELL	,0xc017c21eaf821261  ,EXC_CHK_NONE},	// 45.553093477052
    {0x3FF0000000000000LL	,0x8000000000000000  ,EXC_CHK_NONE},	// 1
    {0x4000000000000000LL	,0x0                 ,EXC_CHK_NONE},	// 2
    {0x4008000000000000LL	,0x8000000000000000  ,EXC_CHK_NONE},	// 3
    {0x4024000000000000LL	,0x0                 ,EXC_CHK_NONE},	// 10
    {0xc000000000000000LL	,0x8000000000000000  ,EXC_CHK_NONE},	// - 2
    {0x3ff921fb54442d18LL	,0xc011afdfd06f6690  ,EXC_CHK_NONE},	// 1.5707963267948966 = pi/2
    {0x400921fb54442d18LL	,0x3fde821762e1e1fe  ,EXC_CHK_NONE},	// 3.1415926535897931 = pi
    {0x4012D97C7F3321D2LL	,0xbff44f7aee440fb5  ,EXC_CHK_NONE},	// 4.71238898038469 = 3pi/2
    {0x401921fb54442d18LL	,0x3ff3bd54fb615d98  ,EXC_CHK_NONE},	// 6.2831853071795862 = 2pi
    {0x402921fb54442d18LL	,0xc012e7ab9f5303ab  ,EXC_CHK_NONE},	// 12.566370614359172 = 4pi
    {0x410921fb54442d18LL	,0x400da9809aee65ac  ,EXC_CHK_NONE},	// 205887.41614566068 =  (2^16)pi
    {0x403921fb54442d18LL	,0x3fdc5a238d75baee  ,EXC_CHK_NONE},	// 25.132741228718345 = 8pi
    {0x403921fb54442d19LL	,0x3fdc5a238d75bbde  ,EXC_CHK_NONE},	// 25.132741228718348 close to 8pi
    {0x3ff921fb57442d18LL	,0xc011afdfa002cafd  ,EXC_CHK_NONE},	// 1.5707963379707675 close to pi/2
    {0x400921fb52442d18LL	,0x3fde82172531e326  ,EXC_CHK_NONE},	// 3.1415926386886319 close to pi
    {0x410921fb56442d18LL	,0x400e073af101fe00  ,EXC_CHK_NONE},	// 205887.41712222318 close to (2^16)p
    {0xbff921fb57442d18LL	,0x4011afdfa002cafd  ,EXC_CHK_NONE},	//-1.5707963379707675 close to - pi/2
    {0xc00921fb54442d18LL	,0xbfde821762e1e1fe  ,EXC_CHK_NONE},	//-3.1415926535897931 = - pi
    {0x400921f554442d18LL	,0x3fde815e53b4ee22  ,EXC_CHK_NONE},	// 3.1415812094979962 close to pi
    {0xc00921f554442d18LL	,0xbfde815e53b4ee22  ,EXC_CHK_NONE},	//-3.1415812094979962 close to -pi
    {0xbff921f557442d18LL	,0x4011b0407b317fe5  ,EXC_CHK_NONE},	//-1.570790615924869 close to -pi/2
    {0xbff9217557442d18LL	,0x4011b8566bce4ed0  ,EXC_CHK_NONE},	//-1.570668545612369 close to -pi/2
    {0x400921fb56442d18LL	,0x3fde8217a091e105  ,EXC_CHK_NONE},	// 3.1415926684909543 close to pi
    {0x4012D98C7F3321D2LL	,0xbff44d6e0205bccd  ,EXC_CHK_NONE},	// 4.71245001554094 close to 3pi/2
	{0x412e848abcdef000LL	,0x40024faa91a1cc98  ,EXC_CHK_NONE},	//1000005.3688883781
	{0x439332270327f466LL	,0x0                 ,EXC_CHK_NONE},	//3.4580273251621926E+17
	{0x411fa317083ee0a2LL	,0xbfee6bac1b89a959  ,EXC_CHK_NONE},	//518341.75805235852
	{0x64ca7f352f2afdaeLL	,0x0                 ,EXC_CHK_NONE},	//3.3553869926485123E+177
	{0xd3d196202a791d3dLL	,0x8000000000000000  ,EXC_CHK_NONE},	//-5.8694348256855645E+95
	{0x56fdb2fb3712813bLL	,0x0                 ,EXC_CHK_NONE},	//1.1159953350264303E+111
	{0x54e57b4e03dbe9b3LL	,0x0                 ,EXC_CHK_NONE},	//9.397154915883978E+100
	{0xea96be922b1706c5LL	,0x8000000000000000  ,EXC_CHK_NONE},	//-2.8524095750052955E+205
	{0x655e883346944823LL	,0x0                 ,EXC_CHK_NONE},	//1.9795803721016825E+180

};

