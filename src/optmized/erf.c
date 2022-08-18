/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * Signature:
 * double amd_erf(double x);
 * Computes the error function of x
 *
 * Maximum ULP is 1
 * This implementation is not IEEE 754 compliant and does not add NaN
 *
 *
 *
 * Implementation Notes
 * ----------------------
 *  if x > 5.921875
 *     return 1.0
 *
 *  if 1.0 < x <= 5.921875
 *      return 1 - exp(x * (-1.128379187296725880250046355483349639021658562123785624241077772244440380334 + x * (-0.6366195457073876372509361142098611365613842613451845660187996937557503018823 + x * (-0.1027738038253763891484262453655884204155629869121723722850012821433164644791 + x * (1.913243693101246229615418743844370388075005777844094932469302445019561018102e-2 + x * (1.998707644618184596542106699767206637202017055190115416498988142320935906728e-4 + x * (-1.679950617157649513178423186602994068311261153819246763122540490733252240355e-3 + x * (5.682647571399760822003020648693277727496012581516293169049116719378006087893e-4 + x * (-2.818273911088010165447137284827909484886276724408557355141697719950587206877e-6 + x * (-8.36042022762110512242996750328325353972110028846909056553927683453789559227e-5 + x * (4.191455298928218929847177350759122028849343840188933573265092552576170371577e-5 + x * (-8.94904245690245597674874042823736926000974508890388440707889473372545259696e-6 + x * (-1.601882217162214515081888614919409470063027935497972455767918716242089281421e-6 + x * (2.31574630784712754973742190163107433655818809186437678061563310059888470168e-6 + x * (-1.172680992715071340493059820700392703705830766464313487241539324358635528004e-6 + x * (4.13210810338385978173496941399410474499527188352425674123806234656925006419e-7 + x * (-1.137660952467942894308567339231771985024943217727472804427969925659315798727e-7 + x * (2.546714067025517336549407706069957092654240232998318059326873262951273469763e-8 + x * (-4.70715336447586025464737282902371449697148633126885701207509741955964750364e-9 + x * (7.212269733032393575867814592370512099873599127284319320707115757119896010476e-10 + x * (-9.13106690276395716383330753633769163836507729250416766899133334975562674476e-11 + x * (9.46445824648881368455203983456955354916849432612181966942472168002823557179e-12 + x * (-7.906121725383095942008118812747448909876652238288826528280910120198289047195e-13 + x * (5.194722012253767308053138754911521581978716730817867231978915010711208408926e-14 + x * (-2.585421224354306958949626150820942151713180335583947339915528022483172928221e-15 + x * (9.16212068599430587351650963049444851622453306999463288171920071757598026267e-17 + x * (-2.059692622984663526501933425995537571997781281645587526345284118549918482945e-18 + x * 2.207460647025675590632388342660853493815957554686199336180586042426504769581e-20))))))))))))))))))))))))))
 *
 *  if x <= 1.0
 *       return x * (1.1283791670955125738937314010593949281312966570053 + x^2 * (-0.37612638903183752369652155981913698166008526585726 + x^2 * (0.112837916709551197416729610919604884249719086253066 + x^2 * (-2.6866170645129733514902076201178133386366245407882e-2 + x^2 * (5.223977625422085948080051857141886948079415494572e-3 + x^2 * (-8.5483270218532048674999332786810493359332696351988e-4 + x^2 * (1.2055332899358121316702316574124762033182544384491e-4 + x^2 * (-1.492564746512234380442190850670176249503305853731e-5 + x^2 * (1.64620434078680492544788115992208726721677151630746e-6 + x^2 * (-1.6364614422311898040823024094770326267130574040383e-7 + x^2 * (1.47921457592634145554905303811520256460334047342844e-8 + x^2 * (-1.2163041953184373789598610527806213730744601584638e-9 + x^2 * (8.7037899668340024850681208476361919205890015225006e-11 + x^2 * (-4.2228963895845588173221408182760780548474992824147e-12))))))))))))))
 *
 * if x is +/- zero, return zero
 * if x is +/- infinity, 1 is returned
 * if x is NaN, NaN is returned
 *
 */

#include <libm_util_amd.h>
#include <libm/alm_special.h>
#include <libm/amd_funcs_internal.h>
#include <libm_macros.h>
#include <libm/types.h>
#include <libm/typehelper.h>
#include <libm/compiler.h>
#include <libm/poly.h>

#define BOUND1 1.0
#define BOUND2 5.921875

double ALM_PROTO_OPT(erf)(double x) {

    /* Set the polynomial coefficients */
  
    uint64_t AH[]={0x3ff20dd750429b6d, 0xbfd812746b0379e7, 0x3fbce2f21a042bdd, 0xbf9b82ce3128899b,
		   0x3f7565bcd0e64ab7, 0xbf4c02db3fed8f63, 0x3f1f9a326bfb70f8, 0xbeef4d255e146073,
		   0x3ebb9e64d07be3ef, 0xbe85f6d6b51269e9, 0x3e4fc411710911f4, 0xbe14e55cc7bb4618,
		   0x3dd7ecbf68f948f5, 0xbd92928f03cb38af};

    uint64_t AL[]={0x3c71ad832697ef5c, 0x3c7084b5e614b124, 0x3c58e5678d53ab5d, 0xbc30d43e1f0f8adf,
		   0xbbc2ee1cf17e6b80, 0xbbe860b74d61f9ae, 0x3bbbc25d2f652555, 0x3b7b598a032c016c,
		   0xbb47ddf936cb55b4, 0x3b229d9c224176a9, 0x3ae3c9c9fab36ed8, 0x3ab5033a60ab2c39,
		   0xba66b25e0f27c1b6, 0xba31317573c855ae};

    uint64_t BH[]={0xbff20dd755aed2ee, 0xbfe45f2ff419d7eb, 0xbfba4f624e50c559, 0x3f9397741ba55b4f,
		   0x3f2a328ccb28100f, 0xbf5b86393d69e01a, 0x3f429ef34610bb0e, 0xbec7a43277be6e7f,
		   0xbf15ea954222701b, 0x3f05f9ad1313f752, 0xbee2c47b07a47511, 0xbebae0081fc4d9cc,
		   0x3ec36d06ff08d6e3, 0xbeb3aca0631d0e93, 0x3e9bbae85cddc95f, 0xbe7e89f250292e10,
		   0x3e5b585ab4ee56db, 0xbe343791e2362fe3, 0x3e08c7fac39ca5e7, 0xbdd9196ac7b021d3,
		   0x3da4d0042e2379da, 0xbd6bd1338d1165b7, 0x3d2d3e62265eae4b, 0xbce749935116736a,
		   0x3c9a6873aa639266, 0xbc42ff4fcbd3a152, 0x3bda0fa3f3374aaf};
 
    uint64_t BL[]={0x3c8b2313a4818d9a, 0x3c772cb40cbce9be, 0xbbbac96f6b512800, 0x3c3c4b0af95c687b,
		   0xbbc5d813d4ff2075, 0x3bf0f9b1617d1293, 0xbbe31f4143550988, 0x3b4ccb54bd312634,
		   0xbbb09d2ee89a8bfb, 0xbbab40809add97f7, 0xbb8cb3fdc0627eae, 0xbb47ce138a9429fa,
		   0x3b5a789f6b66a77a, 0xbb5f2c7c40532c9a, 0x3b3323b3ddffd597, 0xbb05a57bfea11630,
		   0x3aaab3a8be36c5e0, 0x3ad64a8258b1c635, 0x3aa48818cdd9c592, 0xba7ce184840ba4e4,
		   0xba45b338c3a68ce0, 0x3a087c4ee4d44b45, 0xb9c1e937f5ef5205, 0x39608358059dabd0,
		   0xb939485118994192, 0x38ddf56b2d1eead0, 0xb84a37a497cb8358};

    uint64_t ux = asuint64(x);
    uint64_t sign =  ux & SIGNBIT_DP64;

    ux = ux & ~SIGNBIT_DP64;
    x = asdouble(ux);

    double result;

    if((ux <= asuint64(BOUND1))) {

        double x2 = x * x;
	double sH = asdouble(AH[13]);
	double sL = 0.0;
	double rH, rL, tH, tL, cH, cL, z;
       	double sHH, sHT, x2H, x2T;
       	uint64_t usH, ux2;

	for (int i=12; i >= 4; i--) {
	  sH = sH*x2 + asdouble(AH[i]);
	}
	/* Greater accuracy is needed for the remaining iterations. */
	/* split x2 into x2H and x2T */
	ux2 = asuint64(x2);
	ux2 &= 0xfffffffff8000000;
	x2H = asdouble(ux2);
	x2T = x2 - x2H;
	for (int i=3; i >= 0; i--) {
	  /* [rH, rL] = TwoProduct(sH,x2) */
	  rH = sH * x2;

	  /* split sH into sHH and sHT */
	  usH = asuint64(sH);
	  usH &= 0xfffffffff8000000;
	  sHH = asdouble(usH);
	  sHT = sH - sHH;

	  rL = sHT*x2T-(((rH-sHH*x2H)-sHT*x2H)-sHH*x2T);

	  /* [tH, tL] = FastTwoSum(rH,(sL*x2)) */
	  z = sL * x2;
	  tH = rH + z;
	  tL = z - (tH-rH);

	  /* [cH, cL] = FastTwoSum(tH,(tL+rL)) */
	  z = tL + rL;
	  cH = tH + z;
	  cL = z - (cH-tH);

	  /* [rH, rL] = TwoSum(cH,AH[i]) */
	  rH = cH + asdouble(AH[i]);
	  z = rH - cH;
	  rL = asdouble(AL[i]) + (cH-(rH-z))+(asdouble(AH[i])-z);

	  /* [sH, sL] = FastTwoSum(rH,(rL+cL)) */
	  z = rL + cL;
	  sH = rH + z;
	  sL = z - (sH-rH);
	}
	result = sH * x + sL * x;
        result = asdouble(sign | asuint64(result));
    }
    else if((ux <= asuint64(BOUND2))) {

        double sH = asdouble(BH[26]);
	double sL = 0.0;
	double rH, rL, tH, tL, cH, cL, z;
       	double sHH, sHT, xH, xT;
       	uint64_t usH, uxH;

	for (int i=25; i >= 2; i--) {
	  sH = sH*x + asdouble(BH[i]);
	}
	/* Greater accuracy is needed for the remaining iterations. */
	/* split x into xH and xT */
	uxH = ux & 0xfffffffff8000000;
	xH = asdouble(uxH);
	xT = x - xH;
	for (int i=1; i >= 0; i--) {
	  /* [rH, rL] = TwoProduct(sH,x) */
	  rH = sH * x;
	  /* split sH into sHH and sHT */
	  usH = asuint64(sH);
	  usH &= 0xfffffffff8000000;
	  sHH = asdouble(usH);
	  sHT = sH - sHH;

	  rL = sHT*xT-(((rH-sHH*xH)-sHT*xH)-sHH*xT);

	  /* [tH, tL] = FastTwoSum(rH,(sL*x)) */
	  z = sL * x;
	  tH = rH + z;
	  tL = z - (tH-rH);

	  /* [cH, cL] = FastTwoSum(tH,(tL+rL)) */
	  z = tL + rL;
	  cH = tH + z;
	  cL = z - (cH-tH);

	  /* [rH, rL] = TwoSum(cH,BH[i]) */
	  rH = cH + asdouble(BH[i]);
	  z = rH - cH;
	  rL = asdouble(BL[i]) + (cH-(rH-z))+(asdouble(BH[i])-z);

	  /* [sH, sL] = FastTwoSum(rH,(rL+cL)) */
	  z = rL + cL;
	  sH = rH + z;
	  sL = z - (sH-rH);
	}
	double result1 = sH * x + sL * x;
	result = 1.0 - exp(result1);
        result = asdouble(sign | asuint64(result));
    }
    else {
        /* x is NaN */
        if(ux > PINFBITPATT_DP64) {

            if(!(ux & QNAN_MASK_64)) /* x is snan */
                return __alm_handle_errorf(ux, AMD_F_INVALID);

            return x;
        }
        result = 1.0;
        result = asdouble(sign | asuint64(result));
    }
    return result;
}
