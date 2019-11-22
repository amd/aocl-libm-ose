//
//Test cases are from ACML test
//
#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"
#include<math.h>
#ifdef __INTEL_COMPILER
#define S_SP_COS cosf
#else
#define S_SP_COS cosf
//#define S_SP_COS fastcosf
#endif

typedef union
{
	unsigned int u32;
    float f32;
} LF;

typedef struct
{
   unsigned int ux;
   unsigned int ucos;
   unsigned int usin;
} TESTPAIRF;

/* array for cos_sinf */
TESTPAIRF tcos_sinf[] =
{			     //cos	     //sin
    {0x00000000, 0x3f800000, 0x00000000},  //0	
    {0x3F000000, 0x3f60a940, 0x3EF57744},  //0.5
    {0x3F333333, 0x3F43CCB3, 0x3F24EB73},  //.7
    {0x3F4CCCCD, 0x3F325B5F, 0x3F37A4A6},  //.8
    {0x3fc00000, 0x3D90DEAA, 0x3F7F5BD5},  //1.5
	{0x3FC90FDB, 0xb33bbd2e, 0x3f800000},  //1.57079637 = pi/2	
	{0xC07AE148, 0xBF3647C6, 0x3F33BFDD},  //-3.92	

    {0x3f800000, 0x3f0a5140, 0x3f576aa4},  //1
    {0x40000000, 0xbed51133, 0x3F68c7b7},  //2
    {0x40490FDB, 0xbf800000, 0xb3bbbd2e},  //pi
    {0x40C90FDB, 0x3f800000, 0x343bbd2e},  //2pi
    {0x41200000, 0xBF56CD64, 0xBF0B44F8},  //10
    {0x447A0000, 0x3F0FF813, 0x3F53AE61},  //1000
    {0xC8C35000, 0x3f7d62d4, 0x3E11F59F},  //-4e5

	//>=5.0e5 case
	{0x48F42400, 0xBF7BEB6C, 0x3E361962},  //500000	
	{0x48F4CCC8, 0xBF2FD6B2, 0x3F3A0E4A},  //501350.25 > 5.0e5
    {0xC93F6800, 0xBF7CF1B1, 0xBE1DBF77},  //-784000

	//exceptional cases
#ifdef WIN64
	{0x7f800000, 0xffc00000, 0xffc00000},
	{0xff800000, 0xffc00000, 0xffc00000},   // - INF
#else
	{0x7f800000, 0x7fc00000, 0x7fc00000},  // + InF
	{0xff800000, 0x7fc00000, 0x7fc00000},   // - INF
#endif
    {0x7fc00000, 0x7fc00000, 0x7fc00000},  //NaN
    {0x7fc00001, 0x7fc00001, 0x7fc00001},  //NaN
 	{0x7f800001, 0x7fc00001, 0x7fc00001},   // + INF
	{0x7fef8000, 0x7fef8000, 0x7fef8000},   // + QNAN
	{0xffef8000, 0xffef8000, 0xffef8000},   // - QNAN
	{0x80000000, 0x3f800000, 0x80000000},	// - 0

    //make case for expdiff > 15
	{0x43FCE5F1, 0xbf800000, 0xB20FD1DE},  //505.796417, remainder is ~0, very close multiple of piby2
	{0x4831E550, 0xbf800000, 0x379A62A2},  //182165.25, remainder is ~piby4
	{0x42FCE5F1, 0x3F3504F3, 0x3F3504F3},  //126.449104

	//{0x0000DEAD, 0x0000BEEF, 0x00000000}  // end
};

    
extern "C" float S_SP_COS(float);
 
void acc_fastcosf()
{
    const TESTPAIRF *pTest = tcos_sinf;
	LF v_x, v_cos,output;
	int k, flag = 0;

    for(k=0; k<LENGTH_OF(tcos_sinf); k++)
    {
        v_x.u32 = pTest[k].ux;
        output.u32 = pTest[k].ucos;

        v_cos.f32 = S_SP_COS(v_x.f32);

        if(v_cos.u32 != pTest[k].ucos && !(isnan(v_cos.f32) && isnan(output.f32))) 
        {
			flag++;
            std::cout << "Failure: " << std::dec << k << " , expected: " << std::hex << 
                pTest[k].ucos << ", Actual: " << v_cos.u32 << std::endl;
        }
		//else std::cout << "Pass  : " << std::dec << k <<", Test input:" << v_x.f32 << std::endl;
    }
	
	if (flag==0) 	std::cout << "\tTotal " << k << " cases pass." << std::endl;
	else	std::cout << "\t" << std::dec << flag <<" cases fail." << std::endl;

	return;
}

int main()
{
    acc_fastcosf();
    return 0;
}

