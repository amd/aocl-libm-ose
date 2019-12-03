//
//Test cases are from ACML test
//
#include "../inc/accu/DataTypes.h"
#include "../inc/accu/Buffer.h"
#include "../inc/accu/Timer.h"
#include "../inc/accu/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_SINCOS sincos
#else
#define S_SP_SINCOS sincos
//#define S_SP_SINCOS fastsincos
#endif

typedef struct
{
   U64 ux;
   U64 ucos;
   U64 usin;
} TESTPAIR;

/* array for sincos */
TESTPAIR tcos_sin[] =
{			                //cos	           //sin
	{0x3e38000000000000, 0x3ff0000000000000, 0x3E38000000000000},  // 5.5879354476928711E-09 = 2^-28*1.5
    {0xbe38000000000000, 0x3ff0000000000000, 0xBE38000000000000},  // -5.5879354476928711E-09 = -2^-28*1.5 
    {0x0000000000000000, 0x3ff0000000000000, 0x0000000000000000},  // 0
    {0x8000000000000000, 0x3ff0000000000000, 0x8000000000000000},  // -0

    {0x3f18000000000000, 0x3feffffffdc00000, 0x3F17FFFFFF700000},  //  9.1552734375E-05	 =  2^-14 *1.5
    {0xbf18000000000000, 0x3feffffffdc00000, 0xbF17FFFFFF700000},  // -9.1552734375E-05	 = -2^-14 *1.5

	{0x3fe0000000000000, 0x3fec1528065b7d50, 0x3fdeaee8744b05f0},  // .5
    {0xbfe0000000000000, 0x3fec1528065b7d50, 0xBFDEAEE8744B05F0},  //-.5
    {0x3FEFFEE4E9DABC6B, 0x3fe14b164296c5e0, 0x3FEAECBB972204E8},  // 0.999865013834904
    {0x3fe921fb54442d18, 0x3fe6a09e667f3bcd, 0x3FE6A09E667F3BCC},  // 0.78539816339744828 = pi/4
    {0xbfe921fb54442d18, 0x3fe6a09e667f3bcd, 0xBFE6A09E667F3BCC},  //-0.78539816339744828 =-pi/4

	//5e5 checks
    {0x412E848abcdef000, 0x3fd2d92261ee0d59, 0xBFEE94B095AB2499},  // 1000005.3688883781 > 5e5
	{0x439332270327F466, 0x3f8720ef3d9337a0, 0xBFEFFF7A431A18DC},  // 3.4580273251621926E+17 

	//NAN/INF checks
#ifdef WIN64 // windows does not support sinsincos function
    {0x7ff0000000000000, 0xfff8000000000000, 0xfff8000000000000},  // +inf  
	{0xfff0000000000000, 0xfff8000000000000, 0xfff8000000000000},  // -inf
#else
    {0x7ff0000000000000, 0x7ff8000000000000, 0x7ff8000000000000},  // +inf
	{0xfff0000000000000, 0x7ff8000000000000, 0x7ff8000000000000},  // -inf
#endif

    {0x7ff8000000000000, 0x7ff8000000000000, 0x7ff8000000000000},  // +Qnan
    {0x7ffdf00000000000, 0x7ffdf00000000000, 0x7ffdf00000000000},  // +Qnan
    {0xfffdf00000000000, 0xfffdf00000000000, 0xfffdf00000000000},  // -Qnan
    {0xfff8000000000000, 0xfff8000000000000, 0xfff8000000000000},  // -Qnan
    {0x7ff4000000000000, 0x7ffc000000000000, 0x7ffc000000000000},  // +Snan
    {0xfff4000000000000, 0xfffc000000000000, 0xfffc000000000000},  // -Snan

	//Scalar Precision sincos - 0.5*x*xx term
    {0x3FE9E0C8F112AB1E, 0x3fe618243eefd4fd, 0x3FE725F420C5EF11},  // 0.80868956646495227

	//Scalar precision sincos +xx term, Spotted by vector check
    {0x40306B51F0157E66, 0xbfe83dca43349085, 0xBFE4E39E91D623F6},  // 16.41921902203321
    {0x402DDF5ADB92C01A, 0xbfe6ef49dae5cece, 0x3FE650DDA3993550},  // 14.93624006430041
    {0x402DDB778A9EBD8A, 0xbfe6c3bcf257d639, 0x3FE67D4903528613},  // 14.928646404149521

	//near piby4
    {0x401c462b9064a63b, 0x3fe6a0f446b1f80a, 0x3FE6A0488506901A},  // 7.0685255586081963

	//very close to piby4
    {0x3FE921FB54442D19, 0x3fe6a09e667f3bcc, 0x3FE6A09E667F3BCD},  // 0.78539816339744839
    {0x3fe921fb54442d20, 0x3fe6a09e667f3bc7, 0x3FE6A09E667F3BD2},  // 0.78539816339744917

	//sincos expdiff is < 15
    {0x3FF10CA44655D48A, 0x3fdef9b63399ad36, 0x3FEC0098D09C9F69},  // 1.0655863521072058
    {0x400921FB54442D18, 0xbff0000000000000, 0x3CA1A62633145C07},  // 3.1415926535897931 = pi
    {0x400923E979F8B36A, 0xbfefffff118a9e97, 0xBF4EE25AFBAE2B42},  // 3.1425351647398445
    {0x4002DAE59BB5C33E, 0xbfe6a49b6cfd78bd, 0x3FE69CA0ABE92212},  // 2.3568832554668964
    {0x4015FDCA5F9A0E38, 0x3fe6a0f032d9f6da, 0xBFE6A04C98FCC90E},  // 5.4978423059251824
    {0x40b93bda357dadda, 0x3fe7ad37100a511a, 0x3FE586F2678A5A63},  // 6459.8523787068789
    {0x40F63525129291FF, 0x3fe99a6170de0d7d, 0x3FE33228B38E66C1},  // 90962.317034311578
    {0x3FF9207824B27C17, 0x3f3832f911d6420b, 0x3FEFFFFFDB667252},  // 1.5704270776257141
    {0x4025fe9b31eb183d, 0x3f5be9050b9f7107, 0xBFEFFFFCF504B054},  // 10.997277793834764
    {0x439332270327F466, 0x3f8720ef3d9337a0, 0xBFEFFF7A431A18DC},  // 3.4580273251621926E+17

	//sincos expdiff is > 15
    {0x4046C6CBC45DC8DE, 0xbc26d61b58c99c43, 0x3FF0000000000000},  // 45.553093477052
    {0x411FA317083EE0A2, 0xbfe2512769b8cee7, 0xBFEA3D31BD2D673C},  // 518341.75805235852

	//integer
	//MS answer for the following case 0x3fe14a280fb5068c
    {0x3FF0000000000000, 0x3fe14a280fb5068b, 0x3feaed548f090cee},  // 1
    {0x4000000000000000, 0xbfdaa22657537205, 0x3fed18f6ead1b446},  // 2 
	{0x4008000000000000, 0xbfefae04be85e5d2, 0x3FC210386DB6D55B},  // 3
    {0x4024000000000000, 0xbfead9ac890c6b1f, 0xBFE1689EF5F34F52},  // 10
    {0xc000000000000000, 0xbfdaa22657537205, 0xbfed18f6ead1b446},  // - 2

	//Pi related
    {0x3ff921fb54442d18, 0x3c91a62633145c07, 0x3ff0000000000000},  // 1.5707963267948966 = pi/2
    {0x400921fb54442d18, 0xbff0000000000000, 0x3CA1A62633145C07},  // 3.1415926535897931 = pi
    {0x4012D97C7F3321D2, 0xbcaa79394c9e8a0a, 0xbff0000000000000},  // 4.71238898038469 = 3pi/2
    {0x401921fb54442d18, 0x3ff0000000000000, 0xBCB1A62633145C07},  // 6.2831853071795862 = 2pi
    {0x402921fb54442d18, 0x3ff0000000000000, 0xBCC1A62633145C07},  // 12.566370614359172 = 4pi
    {0x410921fb54442d18, 0x3ff0000000000000, 0xBDA1A62633145C07},  // 205887.41614566068 =  (2^16)pi
    {0x403921fb54442d18, 0x3ff0000000000000, 0xBCD1A62633145C07},  // 25.132741228718345 = 8pi
    {0x403921fb54442d19, 0x3ff0000000000000, 0x3CE72CECE675D1FD},  // 25.132741228718348 close to 8pi

	//MS answer for the following case 0xbe47fffffdcb3b39
    {0x3ff921fb57442d18, 0xbe47fffffdcb3b3a, 0x3FEFFFFFFFFFFFFF},  // 1.5707963379707675 close to pi/2
    {0x400921fb52442d18, 0xbfefffffffffffff, 0x3E5000000234C4C6},  // 3.1415926386886319 close to pi
    {0x410921fb56442d18, 0x3fefffff0000019c, 0x3F4FFFFFA6412185},  // 205887.41712222318 close to (2^16)pi
    {0xbff921fb57442d18, 0xbe47fffffdcb3b3a, 0xBFEFFFFFFFFFFFFF},  //-1.5707963379707675 close to - pi/2
    {0xc00921fb54442d18, 0xbff0000000000000, 0xbCA1A62633145C07},  //-3.1415926535897931 = - pi
    {0x400921f554442d18, 0xbfeffffffff70000, 0x3EE7FFFFFFFEDA62},  // 3.1415812094979962 close to pi
    {0xc00921f554442d18, 0xbfeffffffff70000, 0xbEE7FFFFFFFEDA62},  //-3.1415812094979962 close to -pi
    {0xbff921f557442d18, 0x3ed7f40000008b3a, 0xBFEFFFFFFFFDC23F},  //-1.570790615924869 close to -pi/2
    {0xbff9217557442d18, 0x3f20bf9fff3c46cf, 0xBFEFFFFFFB9DF240},  //-1.570668545612369 close to -pi/2
    {0x400921fb56442d18, 0xbfefffffffffffff, 0xBE4FFFFFFB967673},  // 3.1415926684909543 close to pi
    {0x4012D98C7F3321D2, 0x3f0fffffffaa40c6, 0xBFEFFFFFFF000000},  // 4.71245001554094 close to 3pi/2

    //{0x000000000000DEAD, 0x000000000000BEEF, 0x0000000000000000},  // end
};

extern "C" double S_SP_SINCOS(double x, double* sinr, double* cosr);

//#include "math.h"

void acc_fastsincos()
{
    const TESTPAIR *pTest = tcos_sin;
	Hold64 v_x, v_sin, v_cos;
	int k, flag=0;

	//v_x.f64 = 5.01;
	//std::cout << "input: U64= " << std::hex << v_x.u64<< ", F64= " << std::dec << v_x.f64<< std::endl;

    for(k=0; k<LENGTH_OF(tcos_sin); k++)
    {
        v_x.u64 = pTest[k].ux;

        S_SP_SINCOS(v_x.f64, &(v_sin.f64), &(v_cos.f64));

		if ((v_sin.u64 != pTest[k].usin) || (v_cos.u64 != pTest[k].ucos))
		{	
			flag++;

            std::cout << "Failure: " << std::dec << k << " , expected: (" << std::hex << 
                pTest[k].usin << ", " << pTest[k].ucos << "), Actual: (" << v_sin.u64 << ", " << v_cos.u64 
				<< ")" << std::endl;
			//v_sincos.f64 = sincos(v_x.f64);
			//std::cout << "\t The real answer is 0x" << std::hex << v_sincos.u64 << std::endl;
        }
    }
	
	if (flag==0) 	std::cout << "\t" << std::dec << k << " test cases passed!!!" << std::endl;
	else	std::cout << "\t" << std::dec << flag <<" cases fail" << std::endl;

	return;
}

int main()
{
    acc_fastsincos();
    return 0;
}

