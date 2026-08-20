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
#include "libm/compiler.h"
#include "libm_macros.h"
#include "libm/amd_funcs_internal.h"

#define LIBM_ASM_SYNTAX_ATT
#include "libm/libm_asm_macros.h"

#define fname_special alm_log_special

static const uint32_t FlagXNan[1] ALIGN32_USED = {
    0x00000003U,
};

static const uint32_t FlagXNeg[1] ALIGN32_USED = {
    0x00000002U,
};

static const uint32_t FlagXZero[1] ALIGN32_USED = {
    0x00000001U,
};

static const uint64_t Log256Lead[258] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3f6ff00aa0000000ULL,
    0x3f7fe02a60000000ULL, 0x3f87dc4750000000ULL,
    0x3f8fc0a8b0000000ULL, 0x3f93cea440000000ULL,
    0x3f97b91b00000000ULL, 0x3f9b9fc020000000ULL,
    0x3f9f829b00000000ULL, 0x3fa1b0d980000000ULL,
    0x3fa39e87b0000000ULL, 0x3fa58a5ba0000000ULL,
    0x3fa77458f0000000ULL, 0x3fa95c8300000000ULL,
    0x3fab42dd70000000ULL, 0x3fad276b80000000ULL,
    0x3faf0a30c0000000ULL, 0x3fb0759830000000ULL,
    0x3fb16536e0000000ULL, 0x3fb253f620000000ULL,
    0x3fb341d790000000ULL, 0x3fb42edcb0000000ULL,
    0x3fb51b0730000000ULL, 0x3fb60658a0000000ULL,
    0x3fb6f0d280000000ULL, 0x3fb7da7660000000ULL,
    0x3fb8c345d0000000ULL, 0x3fb9ab4240000000ULL,
    0x3fba926d30000000ULL, 0x3fbb78c820000000ULL,
    0x3fbc5e5480000000ULL, 0x3fbd4313d0000000ULL,
    0x3fbe270760000000ULL, 0x3fbf0a30c0000000ULL,
    0x3fbfec9130000000ULL, 0x3fc0671510000000ULL,
    0x3fc0d77e70000000ULL, 0x3fc1478580000000ULL,
    0x3fc1b72ad0000000ULL, 0x3fc2266f10000000ULL,
    0x3fc29552f0000000ULL, 0x3fc303d710000000ULL,
    0x3fc371fc20000000ULL, 0x3fc3dfc2b0000000ULL,
    0x3fc44d2b60000000ULL, 0x3fc4ba36f0000000ULL,
    0x3fc526e5e0000000ULL, 0x3fc59338d0000000ULL,
    0x3fc5ff3070000000ULL, 0x3fc66acd40000000ULL,
    0x3fc6d60fe0000000ULL, 0x3fc740f8f0000000ULL,
    0x3fc7ab8900000000ULL, 0x3fc815c0a0000000ULL,
    0x3fc87fa060000000ULL, 0x3fc8e928d0000000ULL,
    0x3fc9525a90000000ULL, 0x3fc9bb3620000000ULL,
    0x3fca23bc10000000ULL, 0x3fca8becf0000000ULL,
    0x3fcaf3c940000000ULL, 0x3fcb5b5190000000ULL,
    0x3fcbc28670000000ULL, 0x3fcc296850000000ULL,
    0x3fcc8ff7c0000000ULL, 0x3fccf63540000000ULL,
    0x3fcd5c2160000000ULL, 0x3fcdc1bca0000000ULL,
    0x3fce270760000000ULL, 0x3fce8c0250000000ULL,
    0x3fcef0adc0000000ULL, 0x3fcf550a50000000ULL,
    0x3fcfb91860000000ULL, 0x3fd00e6c40000000ULL,
    0x3fd0402590000000ULL, 0x3fd071b850000000ULL,
    0x3fd0a324e0000000ULL, 0x3fd0d46b50000000ULL,
    0x3fd1058bf0000000ULL, 0x3fd1368700000000ULL,
    0x3fd1675ca0000000ULL, 0x3fd1980d20000000ULL,
    0x3fd1c898c0000000ULL, 0x3fd1f8ff90000000ULL,
    0x3fd22941f0000000ULL, 0x3fd2596010000000ULL,
    0x3fd2895a10000000ULL, 0x3fd2b93030000000ULL,
    0x3fd2e8e2b0000000ULL, 0x3fd31871c0000000ULL,
    0x3fd347dd90000000ULL, 0x3fd3772660000000ULL,
    0x3fd3a64c50000000ULL, 0x3fd3d54fa0000000ULL,
    0x3fd4043080000000ULL, 0x3fd432ef20000000ULL,
    0x3fd4618bc0000000ULL, 0x3fd4900680000000ULL,
    0x3fd4be5f90000000ULL, 0x3fd4ec9730000000ULL,
    0x3fd51aad80000000ULL, 0x3fd548a2c0000000ULL,
    0x3fd5767710000000ULL, 0x3fd5a42ab0000000ULL,
    0x3fd5d1bdb0000000ULL, 0x3fd5ff3070000000ULL,
    0x3fd62c82f0000000ULL, 0x3fd659b570000000ULL,
    0x3fd686c810000000ULL, 0x3fd6b3bb20000000ULL,
    0x3fd6e08ea0000000ULL, 0x3fd70d42e0000000ULL,
    0x3fd739d7f0000000ULL, 0x3fd7664e10000000ULL,
    0x3fd792a550000000ULL, 0x3fd7bede00000000ULL,
    0x3fd7eaf830000000ULL, 0x3fd816f410000000ULL,
    0x3fd842d1d0000000ULL, 0x3fd86e9190000000ULL,
    0x3fd89a3380000000ULL, 0x3fd8c5b7c0000000ULL,
    0x3fd8f11e80000000ULL, 0x3fd91c67e0000000ULL,
    0x3fd9479410000000ULL, 0x3fd972a340000000ULL,
    0x3fd99d9580000000ULL, 0x3fd9c86b00000000ULL,
    0x3fd9f323e0000000ULL, 0x3fda1dc060000000ULL,
    0x3fda484090000000ULL, 0x3fda72a490000000ULL,
    0x3fda9cec90000000ULL, 0x3fdac718c0000000ULL,
    0x3fdaf12930000000ULL, 0x3fdb1b1e00000000ULL,
    0x3fdb44f770000000ULL, 0x3fdb6eb590000000ULL,
    0x3fdb985890000000ULL, 0x3fdbc1e080000000ULL,
    0x3fdbeb4d90000000ULL, 0x3fdc149ff0000000ULL,
    0x3fdc3dd7a0000000ULL, 0x3fdc66f4e0000000ULL,
    0x3fdc8ff7c0000000ULL, 0x3fdcb8e070000000ULL,
    0x3fdce1af00000000ULL, 0x3fdd0a63a0000000ULL,
    0x3fdd32fe70000000ULL, 0x3fdd5b7f90000000ULL,
    0x3fdd83e720000000ULL, 0x3fddac3530000000ULL,
    0x3fddd46a00000000ULL, 0x3fddfc8590000000ULL,
    0x3fde248810000000ULL, 0x3fde4c71a0000000ULL,
    0x3fde744260000000ULL, 0x3fde9bfa60000000ULL,
    0x3fdec399d0000000ULL, 0x3fdeeb20c0000000ULL,
    0x3fdf128f50000000ULL, 0x3fdf39e5b0000000ULL,
    0x3fdf6123f0000000ULL, 0x3fdf884a30000000ULL,
    0x3fdfaf5880000000ULL, 0x3fdfd64f20000000ULL,
    0x3fdffd2e00000000ULL, 0x3fe011fab0000000ULL,
    0x3fe02552a0000000ULL, 0x3fe0389ee0000000ULL,
    0x3fe04bdf90000000ULL, 0x3fe05f14b0000000ULL,
    0x3fe0723e50000000ULL, 0x3fe0855c80000000ULL,
    0x3fe0986f40000000ULL, 0x3fe0ab76b0000000ULL,
    0x3fe0be72e0000000ULL, 0x3fe0d163c0000000ULL,
    0x3fe0e44980000000ULL, 0x3fe0f72410000000ULL,
    0x3fe109f390000000ULL, 0x3fe11cb810000000ULL,
    0x3fe12f7190000000ULL, 0x3fe1422020000000ULL,
    0x3fe154c3d0000000ULL, 0x3fe1675ca0000000ULL,
    0x3fe179eab0000000ULL, 0x3fe18c6e00000000ULL,
    0x3fe19ee6b0000000ULL, 0x3fe1b154b0000000ULL,
    0x3fe1c3b810000000ULL, 0x3fe1d610f0000000ULL,
    0x3fe1e85f50000000ULL, 0x3fe1faa340000000ULL,
    0x3fe20cdcd0000000ULL, 0x3fe21f0bf0000000ULL,
    0x3fe23130d0000000ULL, 0x3fe2434b60000000ULL,
    0x3fe2555bc0000000ULL, 0x3fe2676200000000ULL,
    0x3fe2795e10000000ULL, 0x3fe28b5000000000ULL,
    0x3fe29d37f0000000ULL, 0x3fe2af15f0000000ULL,
    0x3fe2c0e9e0000000ULL, 0x3fe2d2b400000000ULL,
    0x3fe2e47430000000ULL, 0x3fe2f62a90000000ULL,
    0x3fe307d730000000ULL, 0x3fe3197a00000000ULL,
    0x3fe32b1330000000ULL, 0x3fe33ca2b0000000ULL,
    0x3fe34e2890000000ULL, 0x3fe35fa4e0000000ULL,
    0x3fe37117b0000000ULL, 0x3fe38280f0000000ULL,
    0x3fe393e0d0000000ULL, 0x3fe3a53730000000ULL,
    0x3fe3b68440000000ULL, 0x3fe3c7c7f0000000ULL,
    0x3fe3d90260000000ULL, 0x3fe3ea3390000000ULL,
    0x3fe3fb5b80000000ULL, 0x3fe40c7a40000000ULL,
    0x3fe41d8fe0000000ULL, 0x3fe42e9c60000000ULL,
    0x3fe43f9fe0000000ULL, 0x3fe4509a50000000ULL,
    0x3fe4618bc0000000ULL, 0x3fe4727430000000ULL,
    0x3fe48353d0000000ULL, 0x3fe4942a80000000ULL,
    0x3fe4a4f850000000ULL, 0x3fe4b5bd60000000ULL,
    0x3fe4c679a0000000ULL, 0x3fe4d72d30000000ULL,
    0x3fe4e7d810000000ULL, 0x3fe4f87a30000000ULL,
    0x3fe50913c0000000ULL, 0x3fe519a4c0000000ULL,
    0x3fe52a2d20000000ULL, 0x3fe53aad00000000ULL,
    0x3fe54b2460000000ULL, 0x3fe55b9350000000ULL,
    0x3fe56bf9d0000000ULL, 0x3fe57c57f0000000ULL,
    0x3fe58cadb0000000ULL, 0x3fe59cfb20000000ULL,
    0x3fe5ad4040000000ULL, 0x3fe5bd7d30000000ULL,
    0x3fe5cdb1d0000000ULL, 0x3fe5ddde50000000ULL,
    0x3fe5ee02a0000000ULL, 0x3fe5fe1ed0000000ULL,
    0x3fe60e32f0000000ULL, 0x3fe61e3ef0000000ULL,
    0x3fe62e42e0000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Log256Tail[258] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3db5885e0250435aULL,
    0x3de620cf11f86ed2ULL, 0x3dff0214edba4a25ULL,
    0x3dbf807c79f3db4eULL, 0x3dea352ba779a52bULL,
    0x3dff56c46aa49fd5ULL, 0x3dfebe465fef5196ULL,
    0x3e0cf0660099f1f8ULL, 0x3e1247b2ff85945dULL,
    0x3e13fd7abf5202b6ULL, 0x3e1f91c9a918d51eULL,
    0x3e08cb73f118d3caULL, 0x3e1d91c7d6fad074ULL,
    0x3de1971bec28d14cULL, 0x3e15b616a423c78aULL,
    0x3da162a6617cc971ULL, 0x3e166391c4c06d29ULL,
    0x3e2d46f5c1d0c4b8ULL, 0x3e2e14282df1f6d3ULL,
    0x3e186f47424a660dULL, 0x3e2d4c8de077753eULL,
    0x3e2e0c307ed24f1cULL, 0x3e226ea18763bdd3ULL,
    0x3e25cad69737c933ULL, 0x3e2af62599088901ULL,
    0x3e18c66c83d6b2d0ULL, 0x3e1880ceb36fb30fULL,
    0x3e2495aac6ca17a4ULL, 0x3e2761db4210878cULL,
    0x3e2eb78e862bac2fULL, 0x3e19b2cd75790dd9ULL,
    0x3e2c55e5cbd3d50fULL, 0x3db162a6617cc971ULL,
    0x3dfdbeabaaa2e519ULL, 0x3e1652cb7150c647ULL,
    0x3e39a11cb2cd2ee2ULL, 0x3e219d0ab1a28813ULL,
    0x3e24bd9e80a41811ULL, 0x3e3214b596faa3dfULL,
    0x3e303fea46980bb8ULL, 0x3e31c8ffa5fd28c7ULL,
    0x3dce8f743bcd96c5ULL, 0x3dfd98c5395315c6ULL,
    0x3e3996fa3ccfa7b2ULL, 0x3e1cd2af2ad13037ULL,
    0x3e1d0da1bd17200eULL, 0x3e3330410ba68b75ULL,
    0x3df4f27a790e7c41ULL, 0x3e13956a86f6ff1bULL,
    0x3e2c6748723551d9ULL, 0x3e2500de9326cdfcULL,
    0x3e1086c848df1b59ULL, 0x3e04357ead6836ffULL,
    0x3e24832442408024ULL, 0x3e3d10da8154b13dULL,
    0x3e39e8ad68ec8260ULL, 0x3e3cfbf706abaf18ULL,
    0x3e3fc56ac6326e23ULL, 0x3e39105e3185cf21ULL,
    0x3e3d017fe5b19cc0ULL, 0x3e3d1f6b48dd13feULL,
    0x3e20b63358a7e73aULL, 0x3e263063028c211cULL,
    0x3e2e6a6886b09760ULL, 0x3e3c138bb891cd03ULL,
    0x3e369f7722b7221aULL, 0x3df57d8fac1a628cULL,
    0x3e3c55e5cbd3d50fULL, 0x3e1552d2ff48fe2eULL,
    0x3e37b8b26ca431bcULL, 0x3e292decdc1c5f6dULL,
    0x3e3abc7c551aaa8cULL, 0x3e36b540731a354bULL,
    0x3e32d341036b89efULL, 0x3e4f9ab21a3a2e0fULL,
    0x3e239c871afb9fbdULL, 0x3e3e6add2c81f640ULL,
    0x3e435c95aa313f41ULL, 0x3e249d4582f6cc53ULL,
    0x3e47574c1c07398fULL, 0x3e4ba846dece9e8dULL,
    0x3e16999fafbc68e7ULL, 0x3e4c9145e51b0103ULL,
    0x3e479ef2cb44850aULL, 0x3e0beec73de11275ULL,
    0x3e2ef4351af5a498ULL, 0x3e45713a493b4a50ULL,
    0x3e45c23a61385992ULL, 0x3e42a88309f57299ULL,
    0x3e4530faa9ac8aceULL, 0x3e25fec2d792a758ULL,
    0x3e35a517a71cbcd7ULL, 0x3e3707dc3e1cd9a3ULL,
    0x3e3a1a9f8ef43049ULL, 0x3e4409d0276b3674ULL,
    0x3e20e2f613e85bd9ULL, 0x3df0027433001e5fULL,
    0x3e35dde2836d3265ULL, 0x3e2300134d7aaf04ULL,
    0x3e3cb7e0b42724f5ULL, 0x3e2d6e93167e6308ULL,
    0x3e3d1569b1526adbULL, 0x3e0e99fc338a1a41ULL,
    0x3e4eb01394a11b1cULL, 0x3e04f27a790e7c41ULL,
    0x3e25ce3ca97b7af9ULL, 0x3e281f0f940ed857ULL,
    0x3e4d36295d88857cULL, 0x3e21aca1ec4af526ULL,
    0x3e445743c7182726ULL, 0x3e23c491aead337eULL,
    0x3e3aef401a738931ULL, 0x3e21cede76092a29ULL,
    0x3e4fba8f44f82bb4ULL, 0x3e446f5f7f3c3e1aULL,
    0x3e47055f86c9674bULL, 0x3e4b41a92b6b6e1aULL,
    0x3e443d162e927628ULL, 0x3e4466174013f9b1ULL,
    0x3e3b05096ad69c62ULL, 0x3e40b169150faa58ULL,
    0x3e3cd98b1df85da7ULL, 0x3e468b507b0f8fa8ULL,
    0x3e48422df57499baULL, 0x3e11351586970274ULL,
    0x3e117e08acba92eeULL, 0x3e26e04314dd0229ULL,
    0x3e497f3097e56d1aULL, 0x3e3356e655901286ULL,
    0x3e0cb761457f94d6ULL, 0x3e39af67a85a9dacULL,
    0x3e453410931a909fULL, 0x3e22c587206058f5ULL,
    0x3e223bc358899c22ULL, 0x3e4d7bf8b6d223cbULL,
    0x3e47991ec5197ddbULL, 0x3e4a79e6bb3a9219ULL,
    0x3e3a4c43ed663ec5ULL, 0x3e461b5a1484f438ULL,
    0x3e4b4e36f7ef0c3aULL, 0x3e115f026acd0d1bULL,
    0x3e3f36b535cecf05ULL, 0x3e2ffb7fbf3eb5c6ULL,
    0x3e3e6a6886b09760ULL, 0x3e3135eb27f5bbc3ULL,
    0x3e470be7d6f6fa57ULL, 0x3e4ce43cc84ab338ULL,
    0x3e4c01d7aac3bd91ULL, 0x3e45c58d07961060ULL,
    0x3e3628bcf941456eULL, 0x3e4c58b2a8461cd2ULL,
    0x3e33071282fb989aULL, 0x3e420dab6a80f09cULL,
    0x3e44f8d84c397b1eULL, 0x3e40d0ee08599e48ULL,
    0x3e1d68787e37da36ULL, 0x3e366187d591bafcULL,
    0x3e22346600bae772ULL, 0x3e390377d0d61b8eULL,
    0x3e4f5e0dd966b907ULL, 0x3e49023cb79a00e2ULL,
    0x3e44e05158c28ad8ULL, 0x3e3bfa7b08b18ae4ULL,
    0x3e4ef1e63db35f67ULL, 0x3e0ec2ae39493d4fULL,
    0x3e40afe930ab2fa0ULL, 0x3e225ff8a1810dd4ULL,
    0x3e469743fb1a71a5ULL, 0x3e5f9cc676785571ULL,
    0x3e5b524da4cbf982ULL, 0x3e5a4c8b381535b8ULL,
    0x3e5839be809caf2cULL, 0x3e50968a1cb82c13ULL,
    0x3e5eae6a41723fb5ULL, 0x3e5d9c29a380a4dbULL,
    0x3e4094aa0ada625eULL, 0x3e5973ad6fc108caULL,
    0x3e4747322fdbab97ULL, 0x3e593692fa9d4221ULL,
    0x3e5c5a992dfbc7d9ULL, 0x3e4e1f33e102387aULL,
    0x3e464fbef14c048cULL, 0x3e4490f513ca5e3bULL,
    0x3e37a6af4d4c799dULL, 0x3e57574c1c07398fULL,
    0x3e57b133417f8c1cULL, 0x3e5feb9e0c176514ULL,
    0x3e419f25bb3172f7ULL, 0x3e45f68a7bbfb852ULL,
    0x3e5ee278497929f1ULL, 0x3e5ccee006109d58ULL,
    0x3e5ce081a07bd8b3ULL, 0x3e570e12981817b8ULL,
    0x3e292ab6d93503d0ULL, 0x3e58cb7dd7c3b61eULL,
    0x3e4efafd0a0b78daULL, 0x3e5e907267c4288eULL,
    0x3e5d31ef96780875ULL, 0x3e23430dfcd2ad50ULL,
    0x3e344d88d75bc1f9ULL, 0x3e5bec0f055e04fcULL,
    0x3e5d85611590b9adULL, 0x3df320568e583229ULL,
    0x3e5a891d1772f538ULL, 0x3e22edc9dabba74dULL,
    0x3e4b9009a1015086ULL, 0x3e52a12a8c5b1a19ULL,
    0x3e3a7885f0fdac85ULL, 0x3e5f4ffcd43ac691ULL,
    0x3e52243ae2640aadULL, 0x3e546513299035d3ULL,
    0x3e5b39c3a62dd725ULL, 0x3e5ba6dd40049f51ULL,
    0x3e451d1ed7177409ULL, 0x3e5cb0f2fd7f5216ULL,
    0x3e3ab150cd4e2213ULL, 0x3e5cfd7bf3193844ULL,
    0x3e53fff8455f1dbdULL, 0x3e5fee640b905fc9ULL,
    0x3e54e2adf548084cULL, 0x3e3b597adc1ecdd2ULL,
    0x3e4345bd096d3a75ULL, 0x3e5101b9d2453c8bULL,
    0x3e508ce55cc8c979ULL, 0x3e5bbf017e595f71ULL,
    0x3e37ce733bd393dcULL, 0x3e233bb0a503f8a1ULL,
    0x3e30e2f613e85bd9ULL, 0x3e5e67555a635b3cULL,
    0x3e2ea88df73d5e8bULL, 0x3e3d17e03bda18a8ULL,
    0x3e5b607d76044f7eULL, 0x3e52adc4e71bc2fcULL,
    0x3e5f99dc7362d1d9ULL, 0x3e5473fa008e6a6aULL,
    0x3e2b75bb09cb0985ULL, 0x3e5ea04dd10b9abaULL,
    0x3e5802d0d6979674ULL, 0x3e174688ccd99094ULL,
    0x3e496f16abb9df22ULL, 0x3e46e66df2aa374fULL,
    0x3e4e66525ea4550aULL, 0x3e42d02f34f20cbdULL,
    0x3e46cfce65047188ULL, 0x3e39b78c842d58b8ULL,
    0x3e4735e624c24bc9ULL, 0x3e47eba1f7dd1adfULL,
    0x3e586b3e59f65355ULL, 0x3e1ce38e637f1b4dULL,
    0x3e58d82ec919edc7ULL, 0x3e4c52648ddcfa37ULL,
    0x3e52482ceae1ac12ULL, 0x3e55a312311aba4fULL,
    0x3e411e236329f225ULL, 0x3e5b48c8cd2f246cULL,
    0x3e6efa39ef35793cULL, 0x0000000000000000ULL,
};

static const uint64_t LogFInv[258] ALIGN32_USED = {
    0x4000000000000000ULL, 0x3fffe01fe01fe020ULL,
    0x3fffc07f01fc07f0ULL, 0x3fffa11caa01fa12ULL,
    0x3fff81f81f81f820ULL, 0x3fff6310aca0dbb5ULL,
    0x3fff44659e4a4271ULL, 0x3fff25f644230ab5ULL,
    0x3fff07c1f07c1f08ULL, 0x3ffee9c7f8458e02ULL,
    0x3ffecc07b301ecc0ULL, 0x3ffeae807aba01ebULL,
    0x3ffe9131abf0b767ULL, 0x3ffe741aa59750e4ULL,
    0x3ffe573ac901e574ULL, 0x3ffe3a9179dc1a73ULL,
    0x3ffe1e1e1e1e1e1eULL, 0x3ffe01e01e01e01eULL,
    0x3ffde5d6e3f8868aULL, 0x3ffdca01dca01dcaULL,
    0x3ffdae6076b981dbULL, 0x3ffd92f2231e7f8aULL,
    0x3ffd77b654b82c34ULL, 0x3ffd5cac807572b2ULL,
    0x3ffd41d41d41d41dULL, 0x3ffd272ca3fc5b1aULL,
    0x3ffd0cb58f6ec074ULL, 0x3ffcf26e5c44bfc6ULL,
    0x3ffcd85689039b0bULL, 0x3ffcbe6d9601cbe7ULL,
    0x3ffca4b3055ee191ULL, 0x3ffc8b265afb8a42ULL,
    0x3ffc71c71c71c71cULL, 0x3ffc5894d10d4986ULL,
    0x3ffc3f8f01c3f8f0ULL, 0x3ffc26b5392ea01cULL,
    0x3ffc0e070381c0e0ULL, 0x3ffbf583ee868d8bULL,
    0x3ffbdd2b899406f7ULL, 0x3ffbc4fd65883e7bULL,
    0x3ffbacf914c1bad0ULL, 0x3ffb951e2b18ff23ULL,
    0x3ffb7d6c3dda338bULL, 0x3ffb65e2e3beee05ULL,
    0x3ffb4e81b4e81b4fULL, 0x3ffb37484ad806ceULL,
    0x3ffb2036406c80d9ULL, 0x3ffb094b31d922a4ULL,
    0x3ffaf286bca1af28ULL, 0x3ffadbe87f94905eULL,
    0x3ffac5701ac5701bULL, 0x3ffaaf1d2f87ebfdULL,
    0x3ffa98ef606a63beULL, 0x3ffa82e65130e159ULL,
    0x3ffa6d01a6d01a6dULL, 0x3ffa574107688a4aULL,
    0x3ffa41a41a41a41aULL, 0x3ffa2c2a87c51ca0ULL,
    0x3ffa16d3f97a4b02ULL, 0x3ffa01a01a01a01aULL,
    0x3ff9ec8e951033d9ULL, 0x3ff9d79f176b682dULL,
    0x3ff9c2d14ee4a102ULL, 0x3ff9ae24ea5510daULL,
    0x3ff999999999999aULL, 0x3ff9852f0d8ec0ffULL,
    0x3ff970e4f80cb872ULL, 0x3ff95cbb0be377aeULL,
    0x3ff948b0fcd6e9e0ULL, 0x3ff934c67f9b2ce6ULL,
    0x3ff920fb49d0e229ULL, 0x3ff90d4f120190d5ULL,
    0x3ff8f9c18f9c18faULL, 0x3ff8e6527af1373fULL,
    0x3ff8d3018d3018d3ULL, 0x3ff8bfce8062ff3aULL,
    0x3ff8acb90f6bf3aaULL, 0x3ff899c0f601899cULL,
    0x3ff886e5f0abb04aULL, 0x3ff87427bcc092b9ULL,
    0x3ff8618618618618ULL, 0x3ff84f00c2780614ULL,
    0x3ff83c977ab2beddULL, 0x3ff82a4a0182a4a0ULL,
    0x3ff8181818181818ULL, 0x3ff8060180601806ULL,
    0x3ff7f405fd017f40ULL, 0x3ff7e225515a4f1dULL,
    0x3ff7d05f417d05f4ULL, 0x3ff7beb3922e017cULL,
    0x3ff7ad2208e0ecc3ULL, 0x3ff79baa6bb6398bULL,
    0x3ff78a4c8178a4c8ULL, 0x3ff77908119ac60dULL,
    0x3ff767dce434a9b1ULL, 0x3ff756cac201756dULL,
    0x3ff745d1745d1746ULL, 0x3ff734f0c541fe8dULL,
    0x3ff724287f46debcULL, 0x3ff713786d9c7c09ULL,
    0x3ff702e05c0b8170ULL, 0x3ff6f26016f26017ULL,
    0x3ff6e1f76b4337c7ULL, 0x3ff6d1a62681c861ULL,
    0x3ff6c16c16c16c17ULL, 0x3ff6b1490aa31a3dULL,
    0x3ff6a13cd1537290ULL, 0x3ff691473a88d0c0ULL,
    0x3ff6816816816817ULL, 0x3ff6719f3601671aULL,
    0x3ff661ec6a5122f9ULL, 0x3ff6524f853b4aa3ULL,
    0x3ff642c8590b2164ULL, 0x3ff63356b88ac0deULL,
    0x3ff623fa77016240ULL, 0x3ff614b36831ae94ULL,
    0x3ff6058160581606ULL, 0x3ff5f66434292dfcULL,
    0x3ff5e75bb8d015e7ULL, 0x3ff5d867c3ece2a5ULL,
    0x3ff5c9882b931057ULL, 0x3ff5babcc647fa91ULL,
    0x3ff5ac056b015ac0ULL, 0x3ff59d61f123ccaaULL,
    0x3ff58ed2308158edULL, 0x3ff5805601580560ULL,
    0x3ff571ed3c506b3aULL, 0x3ff56397ba7c52e2ULL,
    0x3ff5555555555555ULL, 0x3ff54725e6bb82feULL,
    0x3ff5390948f40febULL, 0x3ff52aff56a8054bULL,
    0x3ff51d07eae2f815ULL, 0x3ff50f22e111c4c5ULL,
    0x3ff5015015015015ULL, 0x3ff4f38f62dd4c9bULL,
    0x3ff4e5e0a72f0539ULL, 0x3ff4d843bedc2c4cULL,
    0x3ff4cab88725af6eULL, 0x3ff4bd3edda68fe1ULL,
    0x3ff4afd6a052bf5bULL, 0x3ff4a27fad76014aULL,
    0x3ff49539e3b2d067ULL, 0x3ff4880522014880ULL,
    0x3ff47ae147ae147bULL, 0x3ff46dce34596066ULL,
    0x3ff460cbc7f5cf9aULL, 0x3ff453d9e2c776caULL,
    0x3ff446f86562d9fbULL, 0x3ff43a2730abee4dULL,
    0x3ff42d6625d51f87ULL, 0x3ff420b5265e5951ULL,
    0x3ff4141414141414ULL, 0x3ff40782d10e6566ULL,
    0x3ff3fb013fb013fbULL, 0x3ff3ee8f42a5af07ULL,
    0x3ff3e22cbce4a902ULL, 0x3ff3d5d991aa75c6ULL,
    0x3ff3c995a47babe7ULL, 0x3ff3bd60d9232955ULL,
    0x3ff3b13b13b13b14ULL, 0x3ff3a524387ac822ULL,
    0x3ff3991c2c187f63ULL, 0x3ff38d22d366088eULL,
    0x3ff3813813813814ULL, 0x3ff3755bd1c945eeULL,
    0x3ff3698df3de0748ULL, 0x3ff35dce5f9f2af8ULL,
    0x3ff3521cfb2b78c1ULL, 0x3ff34679ace01346ULL,
    0x3ff33ae45b57bcb2ULL, 0x3ff32f5ced6a1dfaULL,
    0x3ff323e34a2b10bfULL, 0x3ff3187758e9ebb6ULL,
    0x3ff30d190130d190ULL, 0x3ff301c82ac40260ULL,
    0x3ff2f684bda12f68ULL, 0x3ff2eb4ea1fed14bULL,
    0x3ff2e025c04b8097ULL, 0x3ff2d50a012d50a0ULL,
    0x3ff2c9fb4d812ca0ULL, 0x3ff2bef98e5a3711ULL,
    0x3ff2b404ad012b40ULL, 0x3ff2a91c92f3c105ULL,
    0x3ff29e4129e4129eULL, 0x3ff293725bb804a5ULL,
    0x3ff288b01288b013ULL, 0x3ff27dfa38a1ce4dULL,
    0x3ff27350b8812735ULL, 0x3ff268b37cd60127ULL,
    0x3ff25e22708092f1ULL, 0x3ff2539d7e9177b2ULL,
    0x3ff2492492492492ULL, 0x3ff23eb79717605bULL,
    0x3ff23456789abcdfULL, 0x3ff22a0122a0122aULL,
    0x3ff21fb78121fb78ULL, 0x3ff21579804855e6ULL,
    0x3ff20b470c67c0d9ULL, 0x3ff2012012012012ULL,
    0x3ff1f7047dc11f70ULL, 0x3ff1ecf43c7fb84cULL,
    0x3ff1e2ef3b3fb874ULL, 0x3ff1d8f5672e4abdULL,
    0x3ff1cf06ada2811dULL, 0x3ff1c522fc1ce059ULL,
    0x3ff1bb4a4046ed29ULL, 0x3ff1b17c67f2bae3ULL,
    0x3ff1a7b9611a7b96ULL, 0x3ff19e0119e0119eULL,
    0x3ff19453808ca29cULL, 0x3ff18ab083902bdbULL,
    0x3ff1811811811812ULL, 0x3ff1778a191bd684ULL,
    0x3ff16e0689427379ULL, 0x3ff1648d50fc3201ULL,
    0x3ff15b1e5f75270dULL, 0x3ff151b9a3fdd5c9ULL,
    0x3ff1485f0e0acd3bULL, 0x3ff13f0e8d344724ULL,
    0x3ff135c81135c811ULL, 0x3ff12c8b89edc0acULL,
    0x3ff12358e75d3033ULL, 0x3ff11a3019a74826ULL,
    0x3ff1111111111111ULL, 0x3ff107fbbe011080ULL,
    0x3ff0fef010fef011ULL, 0x3ff0f5edfab325a2ULL,
    0x3ff0ecf56be69c90ULL, 0x3ff0e40655826011ULL,
    0x3ff0db20a88f4696ULL, 0x3ff0d24456359e3aULL,
    0x3ff0c9714fbcda3bULL, 0x3ff0c0a7868b4171ULL,
    0x3ff0b7e6ec259dc8ULL, 0x3ff0af2f722eecb5ULL,
    0x3ff0a6810a6810a7ULL, 0x3ff09ddba6af8360ULL,
    0x3ff0953f39010954ULL, 0x3ff08cabb37565e2ULL,
    0x3ff0842108421084ULL, 0x3ff07b9f29b8eae2ULL,
    0x3ff073260a47f7c6ULL, 0x3ff06ab59c7912fbULL,
    0x3ff0624dd2f1a9fcULL, 0x3ff059eea0727586ULL,
    0x3ff05197f7d73404ULL, 0x3ff04949cc1664c5ULL,
    0x3ff0410410410410ULL, 0x3ff038c6b78247fcULL,
    0x3ff03091b51f5e1aULL, 0x3ff02864fc7729e9ULL,
    0x3ff0204081020408ULL, 0x3ff0182436517a37ULL,
    0x3ff0101010101010ULL, 0x3ff0080402010080ULL,
    0x3ff0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Mask1023[2] ALIGN32_USED = {
    0x00000000000003ffULL, 0x0000000000000000ULL,
};

static const uint64_t Mask1023F[2] ALIGN32_USED = {
    0xc08ff80000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Mask2045[2] ALIGN32_USED = {
    0x00000000000007fdULL, 0x0000000000000000ULL,
};

static const uint64_t MaskMant9[2] ALIGN32_USED = {
    0x0000080000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t MaskMantAll8[2] ALIGN32_USED = {
    0x000ff00000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Real1Over2[2] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Real1Over3[2] ALIGN32_USED = {
    0x3fd5555555555555ULL, 0x0000000000000000ULL,
};

static const uint64_t Real1Over4[2] ALIGN32_USED = {
    0x3fd0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t Real1Over5[2] ALIGN32_USED = {
    0x3fc999999999999aULL, 0x0000000000000000ULL,
};

static const uint64_t Real1Over6[2] ALIGN32_USED = {
    0x3fc5555555555555ULL, 0x0000000000000000ULL,
};

static const uint64_t RealCa1[2] ALIGN32_USED = {
    0x3fb55555555554e6ULL, 0x0000000000000000ULL,
};

static const uint64_t RealCa2[2] ALIGN32_USED = {
    0x3f89999999bac6d4ULL, 0x0000000000000000ULL,
};

static const uint64_t RealCa3[2] ALIGN32_USED = {
    0x3f62492307f1519fULL, 0x0000000000000000ULL,
};

static const uint64_t RealCa4[2] ALIGN32_USED = {
    0x3f3c8034c85dfff0ULL, 0x0000000000000000ULL,
};

static const uint64_t RealHalf[2] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealInf[2] ALIGN32_USED = {
    0x7ff0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealLog2Lead[2] ALIGN32_USED = {
    0x3fe62e42e0000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealLog2Tail[2] ALIGN32_USED = {
    0x3e6efa39ef35793cULL, 0x0000000000000000ULL,
};

static const uint64_t RealMant[2] ALIGN32_USED = {
    0x000fffffffffffffULL, 0x0000000000000000ULL,
};

static const uint64_t RealNinf[2] ALIGN32_USED = {
    0xfff0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealNotsign[2] ALIGN32_USED = {
    0x7fffffffffffffffULL, 0x0000000000000000ULL,
};

static const uint64_t RealOne[2] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealQnan[2] ALIGN32_USED = {
    0x7ff8000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealQnanbit[2] ALIGN32_USED = {
    0x0008000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealThreshold[2] ALIGN32_USED = {
    0x3fb0000000000000ULL, 0x0000000000000000ULL,
};

static const uint64_t RealTwo[2] ALIGN32_USED = {
    0x4000000000000000ULL, 0x0000000000000000ULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
double ALM_PROTO_FMA3(log)(double x)
{
    begin_asm()
#ifdef _WIN64
        sub(imm(104), rsp)
        movdqa(xmm6, mem(rsp, 32))
        movdqa(xmm7, mem(rsp, 48))
        movdqa(xmm14, mem(rsp, 64))
        movdqa(xmm15, mem(rsp, 80))
#endif
        xorq(rax, rax)
        vpsrlq(imm(52), xmm0, xmm3)
        vmovd(xmm0, rax)
        vpsubq(mem_rip(Mask1023), xmm3, xmm3)
        vcvtdq2pd(xmm3, xmm6)
        vpand(mem_rip(RealMant), xmm0, xmm2)
        vsubsd(mem_rip(RealOne), xmm0, xmm4)
        mov(rax, rdx)
        shr(imm(52), rdx)
        cmp(imm(0x7ff), rdx)
        jae(__special_cases)
        xorq(r8, r8)
        cmp(r8, rdx)
        ja(__continue_common)
        label_align(__special_cases, 4)
        vpand(mem_rip(RealInf), xmm0, xmm5)
        vcomisd(mem_rip(RealInf), xmm5)
        je(__x_is_inf_or_nan)
        vpxor(xmm5, xmm5, xmm5)
        vcomisd(xmm5, xmm0)
        jbe(__x_is_zero_or_neg)
        vcomisd(mem_rip(Mask1023F), xmm6)
        je(__denormal_adjust)
        label_align(__continue_common, 4)
        vpand(mem_rip(MaskMantAll8), xmm0, xmm14)
        vpand(mem_rip(MaskMant9), xmm0, xmm15)
        vpsllq(imm(1), xmm15, xmm15)
        vpaddq(xmm14, xmm15, xmm1)
        vmovd(xmm1, rax)
        vpand(mem_rip(RealNotsign), xmm4, xmm4)
        vcomisd(mem_rip(RealThreshold), xmm4)
        jb(__near_one)
        shr(imm(44), rax)
        vpor(mem_rip(RealHalf), xmm2, xmm2)
        vpor(mem_rip(RealHalf), xmm1, xmm1)
        lea(mem_rip(LogFInv), r9)
        vsubsd(xmm2, xmm1, xmm1)
        vmulsd(mem(r9, rax, 8), xmm1, xmm1)
        lea(mem_rip(Log256Lead), r9)
        vmulsd(xmm1, xmm1, xmm0)
        vmovsd(mem_rip(Real1Over6), xmm3)
        vmovsd(mem_rip(Real1Over3), xmm5)
        vfmadd213sd(mem_rip(Real1Over5), xmm1, xmm3)
        vfmadd213sd(mem_rip(Real1Over2), xmm1, xmm5)
        vmovapd(xmm0, xmm4)
        vfmadd213sd(mem_rip(Real1Over4), xmm1, xmm3)
        vmulsd(xmm0, xmm0, xmm4)
        vfmadd231sd(xmm0, xmm5, xmm1)
        vfmadd231sd(xmm4, xmm3, xmm1)
        vmovsd(mem_rip(RealLog2Tail), xmm5)
        vfmsub213sd(xmm1, xmm6, xmm5)
        vmovsd(mem(r9, rax, 8), xmm0)
        lea(mem_rip(Log256Tail), rdx)
        vmovsd(mem(rdx, rax, 8), xmm2)
        vaddsd(xmm5, xmm2, xmm2)
        vfmadd231sd(mem_rip(RealLog2Lead), xmm6, xmm0)
        vaddsd(xmm2, xmm0, xmm0)
#ifdef _WIN64
        movdqa(mem(rsp, 32), xmm6)
        movdqa(mem(rsp, 48), xmm7)
        movdqa(mem(rsp, 64), xmm14)
        movdqa(mem(rsp, 80), xmm15)
        add(imm(104), rsp)
#endif
        ret
        label_align(__near_one, 4)
        vmovapd(mem_rip(RealTwo), xmm2)
        vsubsd(mem_rip(RealOne), xmm0, xmm0)
        vaddsd(xmm0, xmm2, xmm2)
        vdivsd(xmm2, xmm0, xmm1)
        vmovapd(mem_rip(RealCa2), xmm4)
        vmovapd(mem_rip(RealCa4), xmm5)
        vmulsd(xmm1, xmm0, xmm6)
        vaddsd(xmm1, xmm1, xmm1)
        vmulsd(xmm1, xmm1, xmm2)
        vfmadd213sd(mem_rip(RealCa1), xmm2, xmm4)
        vfmadd213sd(mem_rip(RealCa3), xmm2, xmm5)
        vmulsd(xmm1, xmm2, xmm2)
        vmulsd(xmm2, xmm4, xmm4)
        vmulsd(xmm2, xmm2, xmm2)
        vmulsd(xmm1, xmm2, xmm2)
        vfmadd231sd(xmm2, xmm5, xmm4)
        vsubsd(xmm6, xmm4, xmm4)
        vaddsd(xmm4, xmm0, xmm0)
#ifdef _WIN64
        movdqa(mem(rsp, 32), xmm6)
        movdqa(mem(rsp, 48), xmm7)
        movdqa(mem(rsp, 64), xmm14)
        movdqa(mem(rsp, 80), xmm15)
        add(imm(104), rsp)
#endif
        ret
        label_align(__denormal_adjust, 4)
        vpor(mem_rip(RealOne), xmm2, xmm2)
        vsubsd(mem_rip(RealOne), xmm2, xmm2)
        vpsrlq(imm(52), xmm2, xmm5)
        vpand(mem_rip(RealMant), xmm2, xmm2)
        vmovapd(xmm2, xmm0)
        vpsubd(mem_rip(Mask2045), xmm5, xmm5)
        vcvtdq2pd(xmm5, xmm6)
        jmp(__continue_common)
        label_align(__x_is_zero_or_neg, 4)
        jne(__x_is_neg)
        vmovsd(mem_rip(RealNinf), xmm1)
#ifdef _WIN64
        mov(mem_rip(FlagXZero), r8d)
#else
        mov(mem_rip(FlagXZero), edi)
#endif
        call_fn(fname_special)
#ifdef _WIN64
        movdqa(mem(rsp, 32), xmm6)
        movdqa(mem(rsp, 48), xmm7)
        movdqa(mem(rsp, 64), xmm14)
        movdqa(mem(rsp, 80), xmm15)
        add(imm(104), rsp)
#endif
        ret
        label_align(__x_is_neg, 4)
        vmovsd(mem_rip(RealQnan), xmm1)
#ifdef _WIN64
        mov(mem_rip(FlagXNeg), r8d)
#else
        mov(mem_rip(FlagXNeg), edi)
#endif
        call_fn(fname_special)
#ifdef _WIN64
        movdqa(mem(rsp, 32), xmm6)
        movdqa(mem(rsp, 48), xmm7)
        movdqa(mem(rsp, 64), xmm14)
        movdqa(mem(rsp, 80), xmm15)
        add(imm(104), rsp)
#endif
        ret
        label_align(__x_is_inf_or_nan, 4)
        cmp(mem_rip(RealInf), rax)
        je(__finish)
        cmp(mem_rip(RealNinf), rax)
        je(__x_is_neg)
        mov(mem_rip(RealQnanbit), r9)
        andq(rax, r9)
        jnz(__finish)
        orq(mem_rip(RealQnanbit), rax)
        movd(rax, xmm1)
#ifdef _WIN64
        mov(mem_rip(FlagXNan), r8d)
#else
        mov(mem_rip(FlagXNan), edi)
#endif
        call_fn(fname_special)
        label_align(__finish, 4)
#ifdef _WIN64
        movdqa(mem(rsp, 32), xmm6)
        movdqa(mem(rsp, 48), xmm7)
        movdqa(mem(rsp, 64), xmm14)
        movdqa(mem(rsp, 80), xmm15)
        add(imm(104), rsp)
#endif
        ret
    end_asm( : : :)
}
