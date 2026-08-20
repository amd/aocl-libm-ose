/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
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
 */

#include <stdint.h>
#include <immintrin.h>
#include "libm/compiler.h"
#include "libm_macros.h"
#include "libm/amd_funcs_internal.h"
#include "libm/types.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"

static const uint64_t L203[4] ALIGN32_USED = {
    0x3f7428a2f98d728bULL, 0x3f7428a2f98d728bULL,
    0x3f7428a2f98d728bULL, 0x3f7428a2f98d728bULL,
};

static const uint64_t L2Pow23[2] ALIGN32_USED = {
    0x4b0000004b000000ULL, 0x4b0000004b000000ULL,
};

static const uint64_t DoubleCubeRootTable256[256] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff00558e6547c36ULL,
    0x3ff00ab8f9d2f374ULL, 0x3ff010204b673fc7ULL,
    0x3ff0158eec36749bULL, 0x3ff01b04ed9fdb53ULL,
    0x3ff02082613df53cULL, 0x3ff0260758e78308ULL,
    0x3ff02b93e6b091f0ULL, 0x3ff031281ceb8ea2ULL,
    0x3ff036c40e2a5e2aULL, 0x3ff03c67cd3f7ceaULL,
    0x3ff03f3c9fee224cULL, 0x3ff044ec379f7f79ULL,
    0x3ff04aa3cd578d67ULL, 0x3ff0506374d40a3dULL,
    0x3ff0562b4218a6e3ULL, 0x3ff059123d3a9848ULL,
    0x3ff05ee6694e7166ULL, 0x3ff064c2ee6e07c6ULL,
    0x3ff06aa7e19c01c5ULL, 0x3ff06d9d8b1deccaULL,
    0x3ff0738f4b6cc8e2ULL, 0x3ff07989af9f9f59ULL,
    0x3ff07c8a2611201cULL, 0x3ff08291a9958f03ULL,
    0x3ff088a208c3fe28ULL, 0x3ff08bad91dd7d8bULL,
    0x3ff091cb6588465eULL, 0x3ff097f24eab04a1ULL,
    0x3ff09b0932aee3f2ULL, 0x3ff0a13de8970de4ULL,
    0x3ff0a45bc08a5ac7ULL, 0x3ff0aa9e79bfa986ULL,
    0x3ff0b0eaa961ca5bULL, 0x3ff0b4145573271cULL,
    0x3ff0ba6ee5f9aad4ULL, 0x3ff0bd9fd0dbe02dULL,
    0x3ff0c408fc1cfd4bULL, 0x3ff0c741430e2059ULL,
    0x3ff0cdb9442ea813ULL, 0x3ff0d0f905168e6cULL,
    0x3ff0d7801893d261ULL, 0x3ff0dac772091bdeULL,
    0x3ff0e15dd5c330abULL, 0x3ff0e4ace71080a4ULL,
    0x3ff0e7fe920f3037ULL, 0x3ff0eea9c37e497eULL,
    0x3ff0f203512f4314ULL, 0x3ff0f8be68db7f32ULL,
    0x3ff0fc1ffa42d902ULL, 0x3ff102eb3af9ed89ULL,
    0x3ff10654f1e29cfbULL, 0x3ff109c1679c189fULL,
    0x3ff110a29f080b3dULL, 0x3ff114176891738aULL,
    0x3ff1178f0099b429ULL, 0x3ff11e86ac2cd7abULL,
    0x3ff12206c7cf4046ULL, 0x3ff12589c21fb842ULL,
    0x3ff12c986355d0d2ULL, 0x3ff13024129645cfULL,
    0x3ff133b2b13aa0ebULL, 0x3ff13ad8cdc48ba3ULL,
    0x3ff13e70544b1d4fULL, 0x3ff1420adb77c99aULL,
    0x3ff145a867b1bfeaULL, 0x3ff14ceca1189d6dULL,
    0x3ff15093574284e9ULL, 0x3ff1543d2473ea9bULL,
    0x3ff157ea0d433a46ULL, 0x3ff15f4d44462724ULL,
    0x3ff163039bd7cde6ULL, 0x3ff166bd21c3a8e2ULL,
    0x3ff16a79dad1fb59ULL, 0x3ff171fcf9aaac3dULL,
    0x3ff175c3693980c3ULL, 0x3ff1798d1f73f3efULL,
    0x3ff17d5a2156e97fULL, 0x3ff1812a73ea2593ULL,
    0x3ff184fe1c406b8fULL, 0x3ff18caf82b8dba4ULL,
    0x3ff1908d4b38a510ULL, 0x3ff1946e7e36f7e5ULL,
    0x3ff1985320ff72a2ULL, 0x3ff19c3b38e975a8ULL,
    0x3ff1a026cb58453dULL, 0x3ff1a415ddbb2c10ULL,
    0x3ff1a808758d9e32ULL, 0x3ff1aff84bac98eaULL,
    0x3ff1b3f5952e1a50ULL, 0x3ff1b7f67a896220ULL,
    0x3ff1bbfb0178d186ULL, 0x3ff1c0032fc3cf91ULL,
    0x3ff1c40f0b3eefc4ULL, 0x3ff1c81e99cc193fULL,
    0x3ff1cc31e15aae72ULL, 0x3ff1d048e7e7b565ULL,
    0x3ff1d463b37e0090ULL, 0x3ff1d8824a365852ULL,
    0x3ff1dca4b237a4f7ULL, 0x3ff1e0caf1b71965ULL,
    0x3ff1e4f50ef85e61ULL, 0x3ff1e923104dbe76ULL,
    0x3ff1ed54fc185286ULL, 0x3ff1f18ad8c82efcULL,
    0x3ff1f5c4acdc91aaULL, 0x3ff1fa027ee4105bULL,
    0x3ff1fe44557cc808ULL, 0x3ff2028a37548ccfULL,
    0x3ff206d42b291a95ULL, 0x3ff20b2237c8466aULL,
    0x3ff20f74641030a6ULL, 0x3ff213cab6ef77c7ULL,
    0x3ff2182537656c13ULL, 0x3ff21c83ec824406ULL,
    0x3ff220e6dd675180ULL, 0x3ff2254e114737d2ULL,
    0x3ff229b98f66228cULL, 0x3ff22e295f19fd31ULL,
    0x3ff2329d87caabb6ULL, 0x3ff2371610f243f2ULL,
    0x3ff23b93021d47daULL, 0x3ff2401462eae0b8ULL,
    0x3ff2449a3b0d1b3fULL, 0x3ff2449a3b0d1b3fULL,
    0x3ff2492492492492ULL, 0x3ff24db370778844ULL,
    0x3ff25246dd846f45ULL, 0x3ff256dee16fdfd4ULL,
    0x3ff25b7b844dfe71ULL, 0x3ff2601cce474fd2ULL,
    0x3ff264c2c798fbe5ULL, 0x3ff2696d789511e2ULL,
    0x3ff2696d789511e2ULL, 0x3ff26e1ce9a2cd73ULL,
    0x3ff272d1233edcf3ULL, 0x3ff2778a2dfba8d0ULL,
    0x3ff27c4812819c13ULL, 0x3ff2810ad98f6e10ULL,
    0x3ff285d28bfa6d45ULL, 0x3ff285d28bfa6d45ULL,
    0x3ff28a9f32aecb79ULL, 0x3ff28f70d6afeb08ULL,
    0x3ff294478118ad83ULL, 0x3ff299233b1bc38aULL,
    0x3ff299233b1bc38aULL, 0x3ff29e040e03fdfbULL,
    0x3ff2a2ea0334a07bULL, 0x3ff2a7d52429b556ULL,
    0x3ff2acc57a7862c2ULL, 0x3ff2acc57a7862c2ULL,
    0x3ff2b1bb0fcf4190ULL, 0x3ff2b6b5edf6b54aULL,
    0x3ff2bbb61ed145cfULL, 0x3ff2c0bbac5bfa6eULL,
    0x3ff2c0bbac5bfa6eULL, 0x3ff2c5c6a0aeb681ULL,
    0x3ff2cad705fc97a6ULL, 0x3ff2cfece6945583ULL,
    0x3ff2cfece6945583ULL, 0x3ff2d5084ce0a331ULL,
    0x3ff2da294368924fULL, 0x3ff2df4fd4cff7c3ULL,
    0x3ff2df4fd4cff7c3ULL, 0x3ff2e47c0bd7d237ULL,
    0x3ff2e9adf35eb25aULL, 0x3ff2eee5966124e8ULL,
    0x3ff2eee5966124e8ULL, 0x3ff2f422fffa1e92ULL,
    0x3ff2f9663b6369b6ULL, 0x3ff2feaf53f61612ULL,
    0x3ff2feaf53f61612ULL, 0x3ff303fe552aea57ULL,
    0x3ff309534a9ad7ceULL, 0x3ff309534a9ad7ceULL,
    0x3ff30eae3fff6ff3ULL, 0x3ff3140f41335c2fULL,
    0x3ff3140f41335c2fULL, 0x3ff319765a32d7aeULL,
    0x3ff31ee3971c2b5bULL, 0x3ff3245704302c13ULL,
    0x3ff3245704302c13ULL, 0x3ff329d0add2bb20ULL,
    0x3ff32f50a08b48f9ULL, 0x3ff32f50a08b48f9ULL,
    0x3ff334d6e9055a5fULL, 0x3ff33a6394110fe6ULL,
    0x3ff33a6394110fe6ULL, 0x3ff33ff6aea3afedULL,
    0x3ff3459045d8331bULL, 0x3ff3459045d8331bULL,
    0x3ff34b3066efd36bULL, 0x3ff350d71f529dd8ULL,
    0x3ff350d71f529dd8ULL, 0x3ff356847c9006b4ULL,
    0x3ff35c388c5f80bfULL, 0x3ff35c388c5f80bfULL,
    0x3ff361f35ca116ffULL, 0x3ff361f35ca116ffULL,
    0x3ff367b4fb5e0985ULL, 0x3ff36d7d76c96d0aULL,
    0x3ff36d7d76c96d0aULL, 0x3ff3734cdd40cd95ULL,
    0x3ff379233d4cd42aULL, 0x3ff379233d4cd42aULL,
    0x3ff37f00a5a1ef96ULL, 0x3ff37f00a5a1ef96ULL,
    0x3ff384e52521006cULL, 0x3ff38ad0cad80848ULL,
    0x3ff38ad0cad80848ULL, 0x3ff390c3a602dc60ULL,
    0x3ff390c3a602dc60ULL, 0x3ff396bdc60bdb88ULL,
    0x3ff39cbf3a8ca7a9ULL, 0x3ff39cbf3a8ca7a9ULL,
    0x3ff3a2c8134ee2d1ULL, 0x3ff3a2c8134ee2d1ULL,
    0x3ff3a8d8604cefe3ULL, 0x3ff3aef031b2b706ULL,
    0x3ff3aef031b2b706ULL, 0x3ff3b50f97de6de5ULL,
    0x3ff3b50f97de6de5ULL, 0x3ff3bb36a36163d8ULL,
    0x3ff3bb36a36163d8ULL, 0x3ff3c1656500d20aULL,
    0x3ff3c79bedb6afb8ULL, 0x3ff3c79bedb6afb8ULL,
    0x3ff3cdda4eb28aa2ULL, 0x3ff3cdda4eb28aa2ULL,
    0x3ff3d420995a63c0ULL, 0x3ff3d420995a63c0ULL,
    0x3ff3da6edf4b9061ULL, 0x3ff3da6edf4b9061ULL,
    0x3ff3e0c5325b9fc2ULL, 0x3ff3e723a499453fULL,
    0x3ff3e723a499453fULL, 0x3ff3ed8a484d473aULL,
    0x3ff3ed8a484d473aULL, 0x3ff3f3f92ffb72d8ULL,
    0x3ff3f3f92ffb72d8ULL, 0x3ff3fa706e6394a4ULL,
    0x3ff3fa706e6394a4ULL, 0x3ff400f01682764aULL,
    0x3ff400f01682764aULL, 0x3ff407783b92e17aULL,
    0x3ff407783b92e17aULL, 0x3ff40e08f10ea81aULL,
    0x3ff40e08f10ea81aULL, 0x3ff414a24aafb1e6ULL,
    0x3ff414a24aafb1e6ULL, 0x3ff41b445c710fa7ULL,
    0x3ff41b445c710fa7ULL, 0x3ff421ef3a901411ULL,
    0x3ff421ef3a901411ULL, 0x3ff428a2f98d728bULL,
};

static const uint64_t DoubleReciprocalTable256[256] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3fefe00000000000ULL,
    0x3fefc00000000000ULL, 0x3fefa00000000000ULL,
    0x3fef800000000000ULL, 0x3fef600000000000ULL,
    0x3fef400000000000ULL, 0x3fef200000000000ULL,
    0x3fef000000000000ULL, 0x3feee00000000000ULL,
    0x3feec00000000000ULL, 0x3feea00000000000ULL,
    0x3fee900000000000ULL, 0x3fee700000000000ULL,
    0x3fee500000000000ULL, 0x3fee300000000000ULL,
    0x3fee100000000000ULL, 0x3fee000000000000ULL,
    0x3fede00000000000ULL, 0x3fedc00000000000ULL,
    0x3feda00000000000ULL, 0x3fed900000000000ULL,
    0x3fed700000000000ULL, 0x3fed500000000000ULL,
    0x3fed400000000000ULL, 0x3fed200000000000ULL,
    0x3fed000000000000ULL, 0x3fecf00000000000ULL,
    0x3fecd00000000000ULL, 0x3fecb00000000000ULL,
    0x3feca00000000000ULL, 0x3fec800000000000ULL,
    0x3fec700000000000ULL, 0x3fec500000000000ULL,
    0x3fec300000000000ULL, 0x3fec200000000000ULL,
    0x3fec000000000000ULL, 0x3febf00000000000ULL,
    0x3febd00000000000ULL, 0x3febc00000000000ULL,
    0x3feba00000000000ULL, 0x3feb900000000000ULL,
    0x3feb700000000000ULL, 0x3feb600000000000ULL,
    0x3feb400000000000ULL, 0x3feb300000000000ULL,
    0x3feb200000000000ULL, 0x3feb000000000000ULL,
    0x3feaf00000000000ULL, 0x3fead00000000000ULL,
    0x3feac00000000000ULL, 0x3feaa00000000000ULL,
    0x3fea900000000000ULL, 0x3fea800000000000ULL,
    0x3fea600000000000ULL, 0x3fea500000000000ULL,
    0x3fea400000000000ULL, 0x3fea200000000000ULL,
    0x3fea100000000000ULL, 0x3fea000000000000ULL,
    0x3fe9e00000000000ULL, 0x3fe9d00000000000ULL,
    0x3fe9c00000000000ULL, 0x3fe9a00000000000ULL,
    0x3fe9900000000000ULL, 0x3fe9800000000000ULL,
    0x3fe9700000000000ULL, 0x3fe9500000000000ULL,
    0x3fe9400000000000ULL, 0x3fe9300000000000ULL,
    0x3fe9200000000000ULL, 0x3fe9000000000000ULL,
    0x3fe8f00000000000ULL, 0x3fe8e00000000000ULL,
    0x3fe8d00000000000ULL, 0x3fe8b00000000000ULL,
    0x3fe8a00000000000ULL, 0x3fe8900000000000ULL,
    0x3fe8800000000000ULL, 0x3fe8700000000000ULL,
    0x3fe8600000000000ULL, 0x3fe8400000000000ULL,
    0x3fe8300000000000ULL, 0x3fe8200000000000ULL,
    0x3fe8100000000000ULL, 0x3fe8000000000000ULL,
    0x3fe7f00000000000ULL, 0x3fe7e00000000000ULL,
    0x3fe7d00000000000ULL, 0x3fe7b00000000000ULL,
    0x3fe7a00000000000ULL, 0x3fe7900000000000ULL,
    0x3fe7800000000000ULL, 0x3fe7700000000000ULL,
    0x3fe7600000000000ULL, 0x3fe7500000000000ULL,
    0x3fe7400000000000ULL, 0x3fe7300000000000ULL,
    0x3fe7200000000000ULL, 0x3fe7100000000000ULL,
    0x3fe7000000000000ULL, 0x3fe6f00000000000ULL,
    0x3fe6e00000000000ULL, 0x3fe6d00000000000ULL,
    0x3fe6c00000000000ULL, 0x3fe6b00000000000ULL,
    0x3fe6a00000000000ULL, 0x3fe6900000000000ULL,
    0x3fe6800000000000ULL, 0x3fe6700000000000ULL,
    0x3fe6600000000000ULL, 0x3fe6500000000000ULL,
    0x3fe6400000000000ULL, 0x3fe6300000000000ULL,
    0x3fe6200000000000ULL, 0x3fe6100000000000ULL,
    0x3fe6000000000000ULL, 0x3fe5f00000000000ULL,
    0x3fe5e00000000000ULL, 0x3fe5d00000000000ULL,
    0x3fe5c00000000000ULL, 0x3fe5b00000000000ULL,
    0x3fe5a00000000000ULL, 0x3fe5900000000000ULL,
    0x3fe5800000000000ULL, 0x3fe5800000000000ULL,
    0x3fe5700000000000ULL, 0x3fe5600000000000ULL,
    0x3fe5500000000000ULL, 0x3fe5400000000000ULL,
    0x3fe5300000000000ULL, 0x3fe5200000000000ULL,
    0x3fe5100000000000ULL, 0x3fe5000000000000ULL,
    0x3fe5000000000000ULL, 0x3fe4f00000000000ULL,
    0x3fe4e00000000000ULL, 0x3fe4d00000000000ULL,
    0x3fe4c00000000000ULL, 0x3fe4b00000000000ULL,
    0x3fe4a00000000000ULL, 0x3fe4a00000000000ULL,
    0x3fe4900000000000ULL, 0x3fe4800000000000ULL,
    0x3fe4700000000000ULL, 0x3fe4600000000000ULL,
    0x3fe4600000000000ULL, 0x3fe4500000000000ULL,
    0x3fe4400000000000ULL, 0x3fe4300000000000ULL,
    0x3fe4200000000000ULL, 0x3fe4200000000000ULL,
    0x3fe4100000000000ULL, 0x3fe4000000000000ULL,
    0x3fe3f00000000000ULL, 0x3fe3e00000000000ULL,
    0x3fe3e00000000000ULL, 0x3fe3d00000000000ULL,
    0x3fe3c00000000000ULL, 0x3fe3b00000000000ULL,
    0x3fe3b00000000000ULL, 0x3fe3a00000000000ULL,
    0x3fe3900000000000ULL, 0x3fe3800000000000ULL,
    0x3fe3800000000000ULL, 0x3fe3700000000000ULL,
    0x3fe3600000000000ULL, 0x3fe3500000000000ULL,
    0x3fe3500000000000ULL, 0x3fe3400000000000ULL,
    0x3fe3300000000000ULL, 0x3fe3200000000000ULL,
    0x3fe3200000000000ULL, 0x3fe3100000000000ULL,
    0x3fe3000000000000ULL, 0x3fe3000000000000ULL,
    0x3fe2f00000000000ULL, 0x3fe2e00000000000ULL,
    0x3fe2e00000000000ULL, 0x3fe2d00000000000ULL,
    0x3fe2c00000000000ULL, 0x3fe2b00000000000ULL,
    0x3fe2b00000000000ULL, 0x3fe2a00000000000ULL,
    0x3fe2900000000000ULL, 0x3fe2900000000000ULL,
    0x3fe2800000000000ULL, 0x3fe2700000000000ULL,
    0x3fe2700000000000ULL, 0x3fe2600000000000ULL,
    0x3fe2500000000000ULL, 0x3fe2500000000000ULL,
    0x3fe2400000000000ULL, 0x3fe2300000000000ULL,
    0x3fe2300000000000ULL, 0x3fe2200000000000ULL,
    0x3fe2100000000000ULL, 0x3fe2100000000000ULL,
    0x3fe2000000000000ULL, 0x3fe2000000000000ULL,
    0x3fe1f00000000000ULL, 0x3fe1e00000000000ULL,
    0x3fe1e00000000000ULL, 0x3fe1d00000000000ULL,
    0x3fe1c00000000000ULL, 0x3fe1c00000000000ULL,
    0x3fe1b00000000000ULL, 0x3fe1b00000000000ULL,
    0x3fe1a00000000000ULL, 0x3fe1900000000000ULL,
    0x3fe1900000000000ULL, 0x3fe1800000000000ULL,
    0x3fe1800000000000ULL, 0x3fe1700000000000ULL,
    0x3fe1600000000000ULL, 0x3fe1600000000000ULL,
    0x3fe1500000000000ULL, 0x3fe1500000000000ULL,
    0x3fe1400000000000ULL, 0x3fe1300000000000ULL,
    0x3fe1300000000000ULL, 0x3fe1200000000000ULL,
    0x3fe1200000000000ULL, 0x3fe1100000000000ULL,
    0x3fe1100000000000ULL, 0x3fe1000000000000ULL,
    0x3fe0f00000000000ULL, 0x3fe0f00000000000ULL,
    0x3fe0e00000000000ULL, 0x3fe0e00000000000ULL,
    0x3fe0d00000000000ULL, 0x3fe0d00000000000ULL,
    0x3fe0c00000000000ULL, 0x3fe0c00000000000ULL,
    0x3fe0b00000000000ULL, 0x3fe0a00000000000ULL,
    0x3fe0a00000000000ULL, 0x3fe0900000000000ULL,
    0x3fe0900000000000ULL, 0x3fe0800000000000ULL,
    0x3fe0800000000000ULL, 0x3fe0700000000000ULL,
    0x3fe0700000000000ULL, 0x3fe0600000000000ULL,
    0x3fe0600000000000ULL, 0x3fe0500000000000ULL,
    0x3fe0500000000000ULL, 0x3fe0400000000000ULL,
    0x3fe0400000000000ULL, 0x3fe0300000000000ULL,
    0x3fe0300000000000ULL, 0x3fe0200000000000ULL,
    0x3fe0200000000000ULL, 0x3fe0100000000000ULL,
    0x3fe0100000000000ULL, 0x3fe0000000000000ULL,
};

static const uint64_t Coefficient1[4] ALIGN32_USED = {
    0x3fd5555555555555ULL, 0x3fd5555555555555ULL,
    0x3fd5555555555555ULL, 0x3fd5555555555555ULL,
};

static const uint64_t Coefficient2[4] ALIGN32_USED = {
    0xbfbc71c71c71c71cULL, 0xbfbc71c71c71c71cULL,
    0xbfbc71c71c71c71cULL, 0xbfbc71c71c71c71cULL,
};

static const uint64_t DefinedCubeRootTable[5] ALIGN32_USED = {
    0x3fe428a2f98d728bULL, 0x3fe965fea53d6e3dULL, /* indices -2, -1 */
    0x3ff0000000000000ULL, 0x3ff428a2f98d728bULL,
    0x3ff965fea53d6e3dULL,
};

static const uint64_t DoubleExp1Mask[2] ALIGN32_USED = {
    0x3fffffff3fffffffULL, 0x3fffffff3fffffffULL,
};

static const uint64_t DoubleExp2Mask[2] ALIGN32_USED = {
    0x3f8000003f800000ULL, 0x3f8000003f800000ULL,
};

static const uint64_t DoubleExp3Mask[2] ALIGN32_USED = {
    0x7f8000007f800000ULL, 0x7f8000007f800000ULL,
};

static const uint64_t DoubleExp4Mask[2] ALIGN32_USED = {
    0x007f8000007f8000ULL, 0x007f8000007f8000ULL,
};

static const uint64_t DoubleExpBias[2] ALIGN32_USED = {
    0x000003ff000003ffULL, 0x000003ff000003ffULL,
};

static const uint64_t DoubleNRealOne[4] ALIGN32_USED = {
    0xbff0000000000000ULL, 0xbff0000000000000ULL,
    0xbff0000000000000ULL, 0xbff0000000000000ULL,
};

static const uint64_t DoubleRealOne[4] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff0000000000000ULL,
    0x3ff0000000000000ULL, 0x3ff0000000000000ULL,
};

static const uint64_t FloatExpBias[2] ALIGN32_USED = {
    0x0000007f0000007fULL, 0x0000007f0000007fULL,
};

static const uint64_t FloatOneby3[2] ALIGN32_USED = {
    0x3eaaaaab3eaaaaabULL, 0x3eaaaaab3eaaaaabULL,
};

static const uint64_t InfMask[2] ALIGN32_USED = {
    0x7f8000007f800000ULL, 0x7f8000007f800000ULL,
};

static const uint64_t QnanMask[2] ALIGN32_USED = {
    0x0040000000400000ULL, 0x0040000000400000ULL,
};

static const uint64_t RealSign[2] ALIGN32_USED = {
    0x8000000080000000ULL, 0x8000000080000000ULL,
};

static const uint64_t SignMask[2] ALIGN32_USED = {
    0x7fffffff7fffffffULL, 0x7fffffff7fffffffULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
v_f32x4_t ALM_PROTO_FMA3(vrs4_cbrtf)(v_f32x4_t x)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(120), rsp)
        movdqa(xmm6, mem(rsp, 0))
        movdqa(xmm7, mem(rsp, 16))
        movdqa(xmm8, mem(rsp, 32))
        movdqa(xmm9, mem(rsp, 48))
        movdqa(xmm10, mem(rsp, 64))
        movdqa(xmm11, mem(rsp, 80))
        movdqa(xmm12, mem(rsp, 96))
#endif
        vmovaps(xmm0, xmm9)
        vcmpneqps(xmm0, xmm0, xmm11)
        vpxor(xmm6, xmm6, xmm6)
        vandps(mem_rip(SignMask), xmm0, xmm2)
        vcmpeqps(xmm6, xmm2, xmm8)
        vpcmpeqd(mem_rip(InfMask), xmm2, xmm4)
        vandps(mem_rip(QnanMask), xmm11, xmm3)
        vorps(xmm8, xmm4, xmm4)
        vorps(xmm11, xmm4, xmm11)
        vandpd(xmm11, xmm0, xmm12)
        vandnpd(xmm11, xmm11, xmm1)
        vorpd(xmm1, xmm12, xmm12)
        vandnpd(xmm12, xmm3, xmm1)
        vandpd(xmm3, xmm3, xmm12)
        vorpd(xmm1, xmm12, xmm12)
        vandnps(xmm0, xmm11, xmm0)
        vandps(mem_rip(DoubleExp3Mask), xmm0, xmm1)
        vpsrld(imm(23), xmm1, xmm1)
        vmulps(mem_rip(L2Pow23), xmm0, xmm4)
        vandps(mem_rip(DoubleExp3Mask), xmm4, xmm2)
        vpsrld(imm(23), xmm2, xmm2)
        vpcmpeqd(xmm6, xmm1, xmm6)
        vandnpd(xmm1, xmm6, xmm10)
        vandpd(xmm6, xmm2, xmm1)
        vorpd(xmm10, xmm1, xmm1)
        vandnpd(xmm0, xmm6, xmm10)
        vandpd(xmm6, xmm4, xmm0)
        vorpd(xmm10, xmm0, xmm0)
        vpsubd(mem_rip(FloatExpBias), xmm1, xmm1)
        vcvtdq2ps(xmm1, xmm3)
        vmulps(mem_rip(FloatOneby3), xmm3, xmm2)
        vcvttps2dq(xmm2, xmm2)
        vpaddd(xmm2, xmm2, xmm4)
        vpaddd(xmm2, xmm4, xmm4)
        vpsubd(xmm4, xmm1, xmm10)
        vpaddd(mem_rip(DoubleExpBias), xmm2, xmm2)
        vpunpckhqdq(xmm2, xmm2, xmm3)
        vpmovsxdq(xmm2, xmm2)
        vpmovsxdq(xmm3, xmm3)
        vpsllq(imm(52), xmm2, xmm2)
        vpsllq(imm(52), xmm3, xmm3)
        vinsertf128(imm(1), xmm3, ymm2, ymm2)
        vandps(mem_rip(DoubleExp1Mask), xmm0, xmm0)
        vorps(mem_rip(DoubleExp2Mask), xmm0, xmm0)
        vandps(mem_rip(DoubleExp4Mask), xmm0, xmm3)
        vpsrld(imm(15), xmm3, xmm3)
        lea(mem_rip(DoubleReciprocalTable256), r11)
        vmovd(xmm3, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(225), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(30), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(225), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(30), xmm3, xmm3, xmm3)
        vinsertf128(imm(1), xmm8, ymm5, ymm5)
        vcvtps2pd(xmm0, ymm0)
        vmovapd(mem_rip(DoubleNRealOne), ymm4)
        vfmadd231pd(ymm5, ymm0, ymm4)
        vmulpd(ymm4, ymm4, ymm5)
        vmovapd(mem_rip(DoubleRealOne), ymm7)
        vfmadd231pd(mem_rip(Coefficient1), ymm4, ymm7)
        vfmadd231pd(mem_rip(Coefficient2), ymm5, ymm7)
        vandnps(xmm10, xmm11, xmm10)
        lea(mem_rip(DefinedCubeRootTable+16), r11)
        vmovd(xmm10, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(225), xmm10, xmm10, xmm10)
        vmovd(xmm10, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(30), xmm10, xmm10, xmm10)
        vmovd(xmm10, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(225), xmm10, xmm10, xmm10)
        vmovd(xmm10, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(30), xmm10, xmm10, xmm10)
        vinsertf128(imm(1), xmm8, ymm5, ymm5)
        vmulpd(ymm5, ymm2, ymm2)
        lea(mem_rip(DoubleCubeRootTable256), r11)
        vmovd(xmm3, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(225), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm5, xmm5)
        vshufps(imm(30), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovlpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(225), xmm3, xmm3, xmm3)
        vmovd(xmm3, eax)
        cdqe
        vmovhpd(mem(r11, rax, 8), xmm8, xmm8)
        vshufps(imm(30), xmm3, xmm3, xmm3)
        vinsertf128(imm(1), xmm8, ymm5, ymm5)
        vmulpd(ymm5, ymm2, ymm2)
        vmulpd(ymm7, ymm2, ymm2)
        vmulpd(mem_rip(L203), ymm2, ymm3)
        vcvtpd2ps(ymm2, xmm2)
        vcvtpd2ps(ymm3, xmm3)
        vandps(mem_rip(RealSign), xmm9, xmm9)
        vandpd(xmm6, xmm3, xmm0)
        vandnpd(xmm2, xmm6, xmm1)
        vorpd(xmm1, xmm0, xmm0)
        vxorps(xmm9, xmm0, xmm0)
        vandnpd(xmm0, xmm11, xmm0)
        vandpd(xmm11, xmm12, xmm1)
        vorpd(xmm1, xmm0, xmm0)
#ifdef _WIN64
        movdqa(mem(rsp, 0), xmm6)
        movdqa(mem(rsp, 16), xmm7)
        movdqa(mem(rsp, 32), xmm8)
        movdqa(mem(rsp, 48), xmm9)
        movdqa(mem(rsp, 64), xmm10)
        movdqa(mem(rsp, 80), xmm11)
        movdqa(mem(rsp, 96), xmm12)
        add(imm(120), rsp)
#endif
        ret
    end_asm( : : :)
}
