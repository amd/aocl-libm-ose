#include "../inc/DataTypes.h"
#include "../inc/Buffer.h"
#include "../inc/Timer.h"
#include "../inc/Constants.h"

#ifdef __INTEL_COMPILER
#define S_SP_POW vsPow
#else
#define S_SP_POW vrsa_powf 
#endif

struct In2Out_32u
{
    U32 in1;
    U32 in2;
    U32 out;
};

const static In2Out_32u ACC_TEST_32_01[] = {

    {0x42ca0000,0x3f800000,0x42ca0000},
    {0x43480000,0x3f800000,0x43480000},

    {0x402df854,0x42b16666,0x7f7a37d0},
// ------------------------------------
    //normal numbers
   {0x3F317218,0x3F317218,0x3F469154},  //1/e,1/e
   {0x3FA00000,0x3FA00000,0x3FA92DED},  //(1.25,1.25
   {0x3F866666,0x40600000,0x3F97D5CF},  //1.05, 3.5
   {0x402DF854,0x402DF854,0x417277D9},  //e
   {0x41000000,0x42000000,0x6F800000},  //8,32
   {0x41078000,0x42000000,0x70C5EB5F},  //8.46875,32
   {0x44000000,0x41078000,0x6594F4F0},  //512,33.875
   {0x3F400000,0x40A00000,0x3E730000},  //.75,5
   {0x41200000,0x42180000,0x7E967699},  //10,38
   {0x3FA00000,0x3F800000,0x3FA00000},  //1.25,1
   {0xc1200000,0x3F800000,0xC1200000},  //-10, 1

   {0x3D9771FC,0xBC0FC60A,0x3F82F582},  //
   {0x44BFDD2A,0xC0EBD680,0x187F7AB2},  //
   {0x43807BF9,0xC08C4085,0x2DF109A8},  //

   {0xc0400000,0x42000000,0x58D2A9FC},  //-3^32
   {0xc0400000,0x40400000,0xC1D80000},  //-3^3
   {0x3F800000,0x3F800000,0x3F800000},  //1
   {0x44000000,0x44000000,0x7F800000},  //error


   {0x41078000,0x3F800000,0x41078000},  // y == 1

   {0x00000000,0x60000000,0x00000000},  // y is +large, x == 0
   {0x3F000000,0x7f800000,0x00000000},  // y ==+inf ,   x < 1
   {0x3F000000,0x60000000,0x00000000},  // y is +large, x < 1
   {0xbf800000,0x7f800000,0x3F800000},  // y ==+inf, x == -1
   {0xbf800000,0x60000000,0x3F800000},  // y is +large, x == -1
   {0x40400000,0x60000000,0x7f800000},  // y is +large, |x| > 1
   {0x40400000,0x7f800000,0x7f800000},  // y ==+inf, |x| > 1
   {0xC0400000,0x60000000,0x7f800000},  // y is +large, |x| > 1
   {0x41078000,0x7f800000,0x7f800000},  // y ==+inf
   {0x41078000,0x60000000,0x7f800000},  // y is large
   {0x00000000,0xE0000000,0x7f800000},  // y is -large, |x| ==0
   {0x80000000,0xFF800000,0x7f800000},  // y ==-inf, |x| == 0
   {0x3F000000,0xE0000000,0x7f800000},  // y is -large, x < 1
   {0x3F000000,0xFF800000,0x7f800000},  // y ==-inf, x < 1
   {0x3F800000,0xE0000000,0x3F800000},  // y is -large, x == 1
   {0x40000000,0xE0000000,0x00000000},  // y is -large, x > 1

   {0x41200000,0xc2200000,0x000116C2},  //denormal
   {0xC1200000,0xc2200000,0x000116C2},  //negative denormal
   {0x41200000,0xC2380000,0x00000000},  //complete underflow
   {0xC1200000,0xC2380000,0x00000000},  //complete underflow
   {0x41000000,0x42240000,0x7D000000},  //overflow
   {0xC5000000,0x43010000,0xFF800000},  //overflow
   {0x41200000,0x10000000,0x3F800000},  //y is very small



// ------------------------------------

   {0x00000000,0xC0400000,0x7F800000},  //+0 ^ -odd integer	      = +inf
   {0x80000000,0xC0400000,0xFF800000},  //-0 ^ -odd integer	      = -inf

   {0x00000000,0xc0800000,0x7F800000},  //+0 ^ -not odd integer   = +inf
   {0x80000000,0xc0080000,0x7F800000},  //-0 ^ -not odd integer	  = +inf

   {0x00000000,0x40400000,0x00000000},  //+0 ^ +odd integer	      = +0
   {0x80000000,0x40400000,0x80000000},  //-0 ^ +odd integer	      = -0

   {0x00000000,0x40800000,0x00000000},  //+0 ^ +not odd integer   = +0
   {0x80000000,0x40080000,0x00000000},  //-0 ^ +not odd integer	  = +0

   {0xbf800000,0x7f800000,0x3F800000},  //-1 ^ +Inf				  = +1
   {0xbf800000,0xFF800000,0x3F800000},  //-1 ^ -Inf				  = +1

   {0x3F800000,0x7FC00000,0x3F800000},  //+1 ^ NaN	 +1^y = +1 for ALL y, even NaN
   {0x3F800000,0xfFC00000,0x3F800000},  //+1 ^ NaN	 +1^y = +1 for ALL y, even NaN
   {0x3F800000,0x7f800000,0x3F800000},  //+1 ^ +Inf				  = +1
   {0x3F800000,0xFF800000,0x3F800000},  //+1 ^ -Inf				  = +1

   {0x40000000,0x00000000,0x3F800000},  //any x ^ +0 			  = +1
   {0x40000000,0x80000000,0x3F800000},  //any x ^ -0 			  = +1
   {0x7f800000,0x00000000,0x3F800000},  //any x ^ +0 			  = +1
   {0xFF800000,0x80000000,0x3F800000},  //any x ^ -0 			  = +1
   {0x7FC00000,0x00000000,0x3F800000},  //any x ^ +0 			  = +1	even NaN
   {0xfFC00000,0x80000000,0x3F800000},  //any x ^ -0 			  = +1	even NaN

   {0xC1200000,0x3FA00000,0x7FC00000},  // finite -x ^ finite non integer = NaN
   {0xC1200000,0xbFA00000,0x7FC00000},  // finite -x ^ finite non integer = NaN

   {0x3F400000,0xFF800000,0x7F800000},  // |x|<1 ^ -inf		  = +inf
   {0xbF400000,0xFF800000,0x7F800000},  // |x|<1 ^ -inf		  = +inf

   {0x41200000,0xFF800000,0x00000000},  // |x|>1 ^ -inf		  = +0
   {0xC1260000,0xFF800000,0x00000000},  // |x|>1 ^ -inf		  = +0

   {0x3F400000,0x7f800000,0x00000000},  // |x|<1 ^ +inf		  = +0
   {0xbF400000,0x7f800000,0x00000000},  // |x|<1 ^ +inf		  = +0

   {0x41200000,0x7f800000,0x7F800000},  // |x|>1 ^ +inf		  = +inf
   {0xC1260000,0x7f800000,0x7F800000},  // |x|>1 ^ +inf		  = +inf

   {0xFF800000,0xC0400000,0x80000000},  // -inf ^ -odd integer = -0

   {0xFF800000,0xc0800000,0x00000000},  // -inf ^ - not odd integer = +0
   {0xFF800000,0xc0080000,0x00000000},  // -inf ^ - not odd integer = +0

   {0xFF800000,0x40400000,0xFF800000},  // -inf ^ +odd integer = -inf

   {0xFF800000,0x40800000,0x7F800000},  // -inf ^ +not odd integer = +inf
   {0xFF800000,0x40F00000,0x7F800000},  // -inf ^ +not odd integer = +inf

   {0x7f800000,0xC0400000,0x00000000},  // +inf ^ y<0 = +0
   {0x7f800000,0xc0800000,0x00000000},  // +inf ^ y<0 = +0
   {0x7f800000,0xc0080000,0x00000000},  // +inf ^ y<0 = +0

   {0x7f800000,0x40400000,0x7F800000},  // +inf ^ y<0 = +inf
   {0x7f800000,0x40800000,0x7F800000},  // +inf ^ y<0 = +inf
   {0x7f800000,0x40080000,0x7F800000},  // +inf ^ y<0 = +inf

   {0x7f800000,0x7FE80000,0x7FE80000},  // +inf ^ NaN = NaN
   {0x40000000,0x7FF80000,0x7FF80000},  // 2 ^ NaN = NaN

   {0xFF800000,0x40400000,0xFF800000},  // -inf ^ +odd integer = -inf
//   {0xc080000000000000,0x4060200000000000,0x00000000},  //overflow
//   {0x41200000,0xc073800000000000,0x00000000},  //denormal
   {0x3F317218,0x3F317218,0x3F469154},  //1/e,1/e
//   {0x3F7357915F4A462D,0x403F96C837AE808A,0x00000000},  //0.0047221830942745626,31.5889925766974
   {0xc0400000,0x42000000,0x58D2A9FC},  //-3^32
   {0x40c09882,0xc24e46dd,0x0000b0b0},  //
   {0x40000000,0x40000000,0x40800000},  //
   {0x40000000,0x40800000,0x41800000},  //
   {0x40800000,0x40000000,0x41800000},  //
   {0x400f0000,0x40800000,0x41C7651E},  //
   {0x40800000,0x400f0000,0x41B123F6},  //
   {0x3DCCCCCD,0x3f800000,0x3DCCCCCD},  //
   {0x402DF854,0xc2Aeac4f,0x0080003C},  //e^ -87.3365 normal
   {0x402DF854,0xc2Aeac50,0x007FFFFC},  //e^ -87.3365 denormal
   {0x402DF854,0x42b17218,0x7F7FFFD7},  //e^ 88.7228 no overflow
   {0x402DF854,0x42b17219,0x7F800000},  //e^ 88.7229 overflow
   {0x402DF854,0xc2cff1b4,0x00000001},  //e^ -87.3365 smallest denormal
   {0x402DF854,0xc2cff1b5,0x00000000},  //e^ -87.3365 underflow

   {0xBE139078,0xCBFBA2CE,0x7F800000},  //-.14, 3.3E7 large even integer  = +inf
   {0xBE139078,0xCB7BA2CF,0xfF800000},  //-.14,-1.6E6 large odd integer  = -inf
   {0x3E139078,0x4BFBA2CE,0x00000000},  //.14, 3.3E7 large number  = 0
   {0x3E139078,0xCBFBA2CE,0x7F800000},  //-.14, 3.3E7 large even integer  = -0
   {0x3E139078,0xCB7BA2CF,0x7F800000},  //-.14, 1.6E6 large odd integer  = -inf
   {0x3E139078,0xCB7BA2CF,0x7F800000},  //.14,-1.6E6 large odd integer  = inf

   {0x3f200000,0x4b000001,0x00000000},  //y could be odd
   {0x3f200000,0x4b800000,0x00000000},  //y must be even, no fractional bits
   {0x3f200000,0x4b800001,0x00000000},  //y must be even, no fractional bits

// ------------------------------------

{0x00000000,0x00000000,0x3f800000},
{0x00000000,0x80000000,0x3f800000},
{0x00000000,0x3f800000,0x00000000},
{0x00000000,0xbf800000,0x7f800000},
{0x00000000,0x7f800000,0x00000000},
{0x00000000,0xff800000,0x7f800000},
{0x00000000,0x7fc00000,0x7fc00000},
{0x00000000,0x7f800001,0x7fc00001},

{0x80000000,0x00000000,0x3f800000},
{0x80000000,0x80000000,0x3f800000},
{0x80000000,0x3f800000,0x80000000},
{0x80000000,0xbf800000,0xff800000},
{0x80000000,0x7f800000,0x00000000},
{0x80000000,0xff800000,0x7f800000},
{0x80000000,0x7fc00000,0x7fc00000},
{0x80000000,0x7f800001,0x7fc00001},

{0x3f800000,0x00000000,0x3f800000},
{0x3f800000,0x80000000,0x3f800000},
{0x3f800000,0x3f800000,0x3f800000},
{0x3f800000,0xbf800000,0x3f800000},
{0x3f800000,0x7f800000,0x3f800000},
{0x3f800000,0xff800000,0x3f800000},
{0x3f800000,0x7fc00000,0x3f800000},
{0x3f800000,0x7f800001,0x3f800000},

{0xbf800000,0x00000000,0x3f800000},
{0xbf800000,0x80000000,0x3f800000},
{0xbf800000,0x3f800000,0xbf800000},
{0xbf800000,0xbf800000,0xbf800000},
{0xbf800000,0x7f800000,0x3f800000},
{0xbf800000,0xff800000,0x3f800000},
{0xbf800000,0x7fc00000,0x7fc00000},
{0xbf800000,0x7f800001,0x7fc00001},

{0x7f800000,0x00000000,0x3f800000},
{0x7f800000,0x80000000,0x3f800000},
{0x7f800000,0x3f800000,0x7f800000},
{0x7f800000,0xbf800000,0x00000000},
{0x7f800000,0x7f800000,0x7f800000},
{0x7f800000,0xff800000,0x00000000},
{0x7f800000,0x7fc00000,0x7fc00000},
{0x7f800000,0x7f800001,0x7fc00001},

{0xff800000,0x00000000,0x3f800000},
{0xff800000,0x80000000,0x3f800000},
{0xff800000,0x3f800000,0xff800000},
{0xff800000,0xbf800000,0x80000000},
{0xff800000,0x7f800000,0x7f800000},
{0xff800000,0xff800000,0x00000000},
{0xff800000,0x7fc00000,0x7fc00000},
{0xff800000,0x7f800001,0x7fc00001},

{0x7fc00000,0x00000000,0x3f800000},
{0x7fc00000,0x80000000,0x3f800000},
{0x7fc00000,0x3f800000,0x7fc00000},
{0x7fc00000,0xbf800000,0x7fc00000},
{0x7fc00000,0x7f800000,0x7fc00000},
{0x7fc00000,0xff800000,0x7fc00000},
{0x7fc00000,0x7fc00000,0x7fc00000},
{0x7fc00000,0x7f800001,0x7fc00000},

{0x7f800001,0x00000000,0x3f800000},
{0x7f800001,0x80000000,0x3f800000},
{0x7f800001,0x3f800000,0x7fc00001},
{0x7f800001,0xbf800000,0x7fc00001},
{0x7f800001,0x7f800000,0x7fc00001},
{0x7f800001,0xff800000,0x7fc00001},
{0x7f800001,0x7fc00000,0x7fc00001},
{0x7f800001,0x7f800001,0x7fc00001},

    };

extern "C" void S_SP_POW(int, void *px, void *py, void *pz);

void acc_vrsapowf()
{
    const In2Out_32u *pTest = ACC_TEST_32_01;

    const size_t LEN = 1024 * 1024;
    const int LEN_INT = (int)LEN;
 
    Buffer<Hold32> buf_x(LEN);
    Buffer<Hold32> buf_y(LEN);
    Buffer<Hold32> buf_z(LEN);
    Buffer<Hold32> buf_z_ref(LEN);
 
    Hold32 *ptr_x = buf_x.Data();
    Hold32 *ptr_y = buf_y.Data();
    Hold32 *ptr_z = buf_z.Data();
    Hold32 *ptr_z_ref = buf_z_ref.Data();
 
    for(size_t i=0; i<LEN; i++)
    { 
        int r = (rand())%(LENGTH_OF(ACC_TEST_32_01));
        ptr_x[i].u32 = pTest[r].in1;
        ptr_y[i].u32 = pTest[r].in2;
        ptr_z_ref[i].u32 = pTest[r].out;
    }

    S_SP_POW(LEN_INT, ptr_x, ptr_y, ptr_z);

    bool fail = false;
    for(size_t i=0; i<LEN; i++)
    {
        U32 ref = ptr_z_ref[i].u32;
        //ref = ref & 0xfffffffe;
        //ptr_y[i].u32 = (ptr_y[i].u32) & 0xfffffffe;
        if(ptr_z[i].u32 != ref)
        {
            //std::cout << "Buffer mismatch at i = " << i << std::endl;
            std::cout << "(" << std::hex << ptr_x[i].u32 << "," << ptr_y[i].u32 << ")" << " , Expected: " <<
                ref << ", Actual: " << ptr_z[i].u32 << std::endl;
            //std::cout << "FAILURE" << std::endl;
            break;
        }
    }
}

int main()
{
    acc_vrsapowf();
    return 0;
}

