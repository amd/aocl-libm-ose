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

static const uint64_t L1stbitSet[2] ALIGN32_USED = {
    0x0000000000000001ULL, 0x0000000000000001ULL,
};

static const uint64_t L9thbitSet[2] ALIGN32_USED = {
    0x0000000000000100ULL, 0x0000000000000100ULL,
};

static const uint64_t CBRTFH256[257] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff0055380000000ULL,
    0x3ff00aa390000000ULL, 0x3ff00ff010000000ULL,
    0x3ff0153920000000ULL, 0x3ff01a7eb0000000ULL,
    0x3ff01fc0d0000000ULL, 0x3ff024ff80000000ULL,
    0x3ff02a3ad0000000ULL, 0x3ff02f72b0000000ULL,
    0x3ff034a750000000ULL, 0x3ff039d880000000ULL,
    0x3ff03f0670000000ULL, 0x3ff0443110000000ULL,
    0x3ff0495870000000ULL, 0x3ff04e7c80000000ULL,
    0x3ff0539d60000000ULL, 0x3ff058bb00000000ULL,
    0x3ff05dd570000000ULL, 0x3ff062ecc0000000ULL,
    0x3ff06800e0000000ULL, 0x3ff06d11e0000000ULL,
    0x3ff0721fc0000000ULL, 0x3ff0772a80000000ULL,
    0x3ff07c3230000000ULL, 0x3ff08136d0000000ULL,
    0x3ff0863860000000ULL, 0x3ff08b36f0000000ULL,
    0x3ff0903280000000ULL, 0x3ff0952b10000000ULL,
    0x3ff09a20a0000000ULL, 0x3ff09f1340000000ULL,
    0x3ff0a402f0000000ULL, 0x3ff0a8efc0000000ULL,
    0x3ff0add990000000ULL, 0x3ff0b2c090000000ULL,
    0x3ff0b7a4b0000000ULL, 0x3ff0bc85f0000000ULL,
    0x3ff0c16450000000ULL, 0x3ff0c63fe0000000ULL,
    0x3ff0cb18b0000000ULL, 0x3ff0cfeeb0000000ULL,
    0x3ff0d4c1e0000000ULL, 0x3ff0d99250000000ULL,
    0x3ff0de6010000000ULL, 0x3ff0e32b00000000ULL,
    0x3ff0e7f340000000ULL, 0x3ff0ecb8d0000000ULL,
    0x3ff0f17bb0000000ULL, 0x3ff0f63bf0000000ULL,
    0x3ff0faf970000000ULL, 0x3ff0ffb460000000ULL,
    0x3ff1046cb0000000ULL, 0x3ff1092250000000ULL,
    0x3ff10dd560000000ULL, 0x3ff11285e0000000ULL,
    0x3ff11733d0000000ULL, 0x3ff11bdf30000000ULL,
    0x3ff1208800000000ULL, 0x3ff1252e40000000ULL,
    0x3ff129d210000000ULL, 0x3ff12e7350000000ULL,
    0x3ff1331210000000ULL, 0x3ff137ae60000000ULL,
    0x3ff13c4840000000ULL, 0x3ff140dfa0000000ULL,
    0x3ff1457490000000ULL, 0x3ff14a0710000000ULL,
    0x3ff14e9730000000ULL, 0x3ff15324e0000000ULL,
    0x3ff157b030000000ULL, 0x3ff15c3920000000ULL,
    0x3ff160bfc0000000ULL, 0x3ff16543f0000000ULL,
    0x3ff169c5d0000000ULL, 0x3ff16e4560000000ULL,
    0x3ff172c2a0000000ULL, 0x3ff1773d90000000ULL,
    0x3ff17bb630000000ULL, 0x3ff1802c90000000ULL,
    0x3ff184a0a0000000ULL, 0x3ff1891270000000ULL,
    0x3ff18d8210000000ULL, 0x3ff191ef60000000ULL,
    0x3ff1965a80000000ULL, 0x3ff19ac360000000ULL,
    0x3ff19f2a10000000ULL, 0x3ff1a38e90000000ULL,
    0x3ff1a7f0e0000000ULL, 0x3ff1ac5100000000ULL,
    0x3ff1b0af00000000ULL, 0x3ff1b50ad0000000ULL,
    0x3ff1b96480000000ULL, 0x3ff1bdbc10000000ULL,
    0x3ff1c21180000000ULL, 0x3ff1c664d0000000ULL,
    0x3ff1cab610000000ULL, 0x3ff1cf0530000000ULL,
    0x3ff1d35230000000ULL, 0x3ff1d79d30000000ULL,
    0x3ff1dbe620000000ULL, 0x3ff1e02cf0000000ULL,
    0x3ff1e471d0000000ULL, 0x3ff1e8b490000000ULL,
    0x3ff1ecf550000000ULL, 0x3ff1f13410000000ULL,
    0x3ff1f570d0000000ULL, 0x3ff1f9ab90000000ULL,
    0x3ff1fde450000000ULL, 0x3ff2021b20000000ULL,
    0x3ff2064ff0000000ULL, 0x3ff20a82c0000000ULL,
    0x3ff20eb3b0000000ULL, 0x3ff212e2a0000000ULL,
    0x3ff2170fb0000000ULL, 0x3ff21b3ac0000000ULL,
    0x3ff21f63f0000000ULL, 0x3ff2238b40000000ULL,
    0x3ff227b0a0000000ULL, 0x3ff22bd420000000ULL,
    0x3ff22ff5c0000000ULL, 0x3ff2341570000000ULL,
    0x3ff2383350000000ULL, 0x3ff23c4f60000000ULL,
    0x3ff2406980000000ULL, 0x3ff24481d0000000ULL,
    0x3ff2489850000000ULL, 0x3ff24cad00000000ULL,
    0x3ff250bfe0000000ULL, 0x3ff254d0e0000000ULL,
    0x3ff258e020000000ULL, 0x3ff25ced90000000ULL,
    0x3ff260f940000000ULL, 0x3ff2650320000000ULL,
    0x3ff2690b40000000ULL, 0x3ff26d1190000000ULL,
    0x3ff2711630000000ULL, 0x3ff2751900000000ULL,
    0x3ff2791a20000000ULL, 0x3ff27d1980000000ULL,
    0x3ff2811720000000ULL, 0x3ff2851310000000ULL,
    0x3ff2890d50000000ULL, 0x3ff28d05d0000000ULL,
    0x3ff290fca0000000ULL, 0x3ff294f1c0000000ULL,
    0x3ff298e530000000ULL, 0x3ff29cd700000000ULL,
    0x3ff2a0c710000000ULL, 0x3ff2a4b580000000ULL,
    0x3ff2a8a250000000ULL, 0x3ff2ac8d70000000ULL,
    0x3ff2b076f0000000ULL, 0x3ff2b45ec0000000ULL,
    0x3ff2b84500000000ULL, 0x3ff2bc29a0000000ULL,
    0x3ff2c00c90000000ULL, 0x3ff2c3ee00000000ULL,
    0x3ff2c7cdc0000000ULL, 0x3ff2cbabf0000000ULL,
    0x3ff2cf8880000000ULL, 0x3ff2d36390000000ULL,
    0x3ff2d73d00000000ULL, 0x3ff2db14d0000000ULL,
    0x3ff2deeb20000000ULL, 0x3ff2e2bfe0000000ULL,
    0x3ff2e69310000000ULL, 0x3ff2ea64b0000000ULL,
    0x3ff2ee34d0000000ULL, 0x3ff2f20360000000ULL,
    0x3ff2f5d070000000ULL, 0x3ff2f99bf0000000ULL,
    0x3ff2fd65f0000000ULL, 0x3ff3012e70000000ULL,
    0x3ff304f570000000ULL, 0x3ff308baf0000000ULL,
    0x3ff30c7ef0000000ULL, 0x3ff3104180000000ULL,
    0x3ff3140280000000ULL, 0x3ff317c210000000ULL,
    0x3ff31b8020000000ULL, 0x3ff31f3cd0000000ULL,
    0x3ff322f7f0000000ULL, 0x3ff326b1b0000000ULL,
    0x3ff32a69f0000000ULL, 0x3ff32e20c0000000ULL,
    0x3ff331d620000000ULL, 0x3ff3358a20000000ULL,
    0x3ff3393ca0000000ULL, 0x3ff33cedc0000000ULL,
    0x3ff3409d70000000ULL, 0x3ff3444bc0000000ULL,
    0x3ff347f8a0000000ULL, 0x3ff34ba420000000ULL,
    0x3ff34f4e30000000ULL, 0x3ff352f6f0000000ULL,
    0x3ff3569e40000000ULL, 0x3ff35a4430000000ULL,
    0x3ff35de8c0000000ULL, 0x3ff3618bf0000000ULL,
    0x3ff3652dd0000000ULL, 0x3ff368ce40000000ULL,
    0x3ff36c6d60000000ULL, 0x3ff3700b30000000ULL,
    0x3ff373a7a0000000ULL, 0x3ff37742b0000000ULL,
    0x3ff37adc70000000ULL, 0x3ff37e74e0000000ULL,
    0x3ff3820c00000000ULL, 0x3ff385a1c0000000ULL,
    0x3ff3893640000000ULL, 0x3ff38cc960000000ULL,
    0x3ff3905b40000000ULL, 0x3ff393ebd0000000ULL,
    0x3ff3977b10000000ULL, 0x3ff39b0910000000ULL,
    0x3ff39e95c0000000ULL, 0x3ff3a22120000000ULL,
    0x3ff3a5ab40000000ULL, 0x3ff3a93410000000ULL,
    0x3ff3acbbb0000000ULL, 0x3ff3b04200000000ULL,
    0x3ff3b3c700000000ULL, 0x3ff3b74ad0000000ULL,
    0x3ff3bacd60000000ULL, 0x3ff3be4eb0000000ULL,
    0x3ff3c1ceb0000000ULL, 0x3ff3c54d90000000ULL,
    0x3ff3c8cb20000000ULL, 0x3ff3cc4770000000ULL,
    0x3ff3cfc2a0000000ULL, 0x3ff3d33c80000000ULL,
    0x3ff3d6b530000000ULL, 0x3ff3da2cb0000000ULL,
    0x3ff3dda2f0000000ULL, 0x3ff3e11800000000ULL,
    0x3ff3e48be0000000ULL, 0x3ff3e7fe80000000ULL,
    0x3ff3eb7000000000ULL, 0x3ff3eee040000000ULL,
    0x3ff3f24f60000000ULL, 0x3ff3f5bd40000000ULL,
    0x3ff3f92a00000000ULL, 0x3ff3fc9590000000ULL,
    0x3ff3fffff0000000ULL, 0x3ff4036930000000ULL,
    0x3ff406d140000000ULL, 0x3ff40a3830000000ULL,
    0x3ff40d9df0000000ULL, 0x3ff4110290000000ULL,
    0x3ff4146600000000ULL, 0x3ff417c850000000ULL,
    0x3ff41b2980000000ULL, 0x3ff41e8990000000ULL,
    0x3ff421e880000000ULL, 0x3ff4254640000000ULL,
    0x3ff428a2f0000000ULL,
};

static const uint64_t CBRTFT256[257] ALIGN32_USED = {
    0x0000000000000000ULL, 0x3e6e6a24c81e4294ULL,
    0x3e58548511e3a785ULL, 0x3e64eb9336ec07f6ULL,
    0x3e40ea64b8b750e1ULL, 0x3e461637cff8a53cULL,
    0x3e40733bf7bd1943ULL, 0x3e5666911345ccedULL,
    0x3e477b7a3f592f14ULL, 0x3e6f18d3dd1a5402ULL,
    0x3e2be2f5a58ee9a4ULL, 0x3e68901f8f085fa7ULL,
    0x3e5c68b8cd5b5d69ULL, 0x3e5a6b0e8624be42ULL,
    0x3dbc4b22b06f68e7ULL, 0x3e60f3f0afcabe9bULL,
    0x3e548495bca4e1b7ULL, 0x3e66107f1abdfdc3ULL,
    0x3e6e67261878288aULL, 0x3e5a6bc155286f1eULL,
    0x3e58a759c64a85f2ULL, 0x3e45fce70a4a8d09ULL,
    0x3e32f9cbf373fe1dULL, 0x3e590564ce4ac359ULL,
    0x3e5ac29ce761b02fULL, 0x3e5cb752f497381cULL,
    0x3e68bb9e1cfb35e0ULL, 0x3e65b4917099de90ULL,
    0x3e5cc77ac9c65ef2ULL, 0x3e57a0f3e7be3dbaULL,
    0x3e66ec851ee0c16fULL, 0x3e689449bf2946daULL,
    0x3e698f25301ba223ULL, 0x3e347d5ec651f549ULL,
    0x3e6c33ec9a86007aULL, 0x3e5e0b6653e92649ULL,
    0x3e3bd64ac09d755fULL, 0x3e2f537506f78167ULL,
    0x3e62c382d1b3735eULL, 0x3e6e20ed659f99e1ULL,
    0x3e586b633a9c182aULL, 0x3e445cfd5a65e777ULL,
    0x3e60c8770f58bca4ULL, 0x3e6739e44b0933c5ULL,
    0x3e027dc3d9ce7bd8ULL, 0x3e63c53c7c5a7b64ULL,
    0x3e69669683830cecULL, 0x3e68d772c39bdcc4ULL,
    0x3e69b0008bcf6d7bULL, 0x3e3bbb305825ce4fULL,
    0x3e6da3f4af13a406ULL, 0x3e5f36b96f74ce86ULL,
    0x3e165c002303f790ULL, 0x3e682f84095ba7d5ULL,
    0x3e6d46433541b2c6ULL, 0x3e671c3d56e93a89ULL,
    0x3e598dcef4e40012ULL, 0x3e4530ebef17fe03ULL,
    0x3e4e8b8fa3715066ULL, 0x3e6ab26eb3b211dcULL,
    0x3e454dd4dc906307ULL, 0x3e5c9f962387984eULL,
    0x3e6c62a959afec09ULL, 0x3e6638d9ac6a866aULL,
    0x3e338704eca8a22dULL, 0x3e4e6c9e1db14f8fULL,
    0x3e58744b7f9c9eaaULL, 0x3e66c2893486373bULL,
    0x3e5b36bce31699b7ULL, 0x3e671e3813d200c7ULL,
    0x3e699755ab40aa88ULL, 0x3e6b45ca0e4bcfc0ULL,
    0x3e32dd090d869c5dULL, 0x3e64fe0516b917daULL,
    0x3e694563226317a2ULL, 0x3e653d8fafc2c851ULL,
    0x3e5dcbd41fbd41a3ULL, 0x3e5862ff5285f59cULL,
    0x3e63072ea97a1e1cULL, 0x3e52839075184805ULL,
    0x3e64b0323e9eff42ULL, 0x3e6b158893c45484ULL,
    0x3e3149ef0fc35826ULL, 0x3e5f2e77ea96acaaULL,
    0x3e5200074c471a95ULL, 0x3e63f8cc517f6f04ULL,
    0x3e660ba2e311bb55ULL, 0x3e64b788730bbec3ULL,
    0x3e657090795ee20cULL, 0x3e6d9ffe983670b1ULL,
    0x3e62a463ff61bfdaULL, 0x3e69d1bc6a5e65cfULL,
    0x3e68718abaa9e922ULL, 0x3e63c2f52ffa342eULL,
    0x3e60fae13ff42c80ULL, 0x3e65440f0ef00d57ULL,
    0x3e46fcd22d4e3c1eULL, 0x3e4e0c60b409e863ULL,
    0x3e6f9cab5a5f0333ULL, 0x3e630f24744c333dULL,
    0x3e4b50622a76b2feULL, 0x3e6fdb94ba595375ULL,
    0x3e3861b9b945a171ULL, 0x3e654348015188c4ULL,
    0x3e6b54d149865523ULL, 0x3e6a0bb783d9de33ULL,
    0x3e6629d12b1a2157ULL, 0x3e6467fe35d179dfULL,
    0x3e69763f3e26c8f7ULL, 0x3e53f798bb9f7679ULL,
    0x3e552e577e855898ULL, 0x3e6fde47e5502c3aULL,
    0x3e5cbd0b548d96a0ULL, 0x3e6a9cd9f7be8de8ULL,
    0x3e522bbe704886deULL, 0x3e6e3dea8317f020ULL,
    0x3e6e812085ac8855ULL, 0x3e5c87144f24cb07ULL,
    0x3e61e128ee311fa2ULL, 0x3e5b5c163d61a2d3ULL,
    0x3e47d97e7fb90633ULL, 0x3e6efe899d50f6a7ULL,
    0x3e6d0333eb75de5aULL, 0x3e40e590be73a573ULL,
    0x3e68ce8dcac3cdd2ULL, 0x3e6ee8a48954064bULL,
    0x3e6aa62f18461e09ULL, 0x3e601e5940986a15ULL,
    0x3e3b082f4f9b8d4cULL, 0x3e6876e0e5527f5aULL,
    0x3e63617080831e6bULL, 0x3e681b26e34aa4a2ULL,
    0x3e552ee66dfab0c1ULL, 0x3e5d85a5329e8819ULL,
    0x3e5105c1b646b5d1ULL, 0x3e6bb6690c1a379cULL,
    0x3e586aeba73ce3a9ULL, 0x3e6dd16198294dd4ULL,
    0x3e6454e675775e83ULL, 0x3e63842e026197eaULL,
    0x3e6f1ce0e70c44d2ULL, 0x3e6ad636441a5627ULL,
    0x3e54c205d7212abbULL, 0x3e6167c86c116419ULL,
    0x3e638ec3ef16e294ULL, 0x3e6473fceace9321ULL,
    0x3e67af53a836dba7ULL, 0x3e1a51f3c383b652ULL,
    0x3e63696da190822dULL, 0x3e62f9adec77074bULL,
    0x3e38190fd5bee55fULL, 0x3e4bfee8fac68e55ULL,
    0x3e331c9d6bc5f68aULL, 0x3e689d0523737edfULL,
    0x3e5a295943bf47bbULL, 0x3e396be32e5b3207ULL,
    0x3e6e44c7d909fa0eULL, 0x3e2b2505da94d9eaULL,
    0x3e60c851f46c9c98ULL, 0x3e5da71f7d9aa3b7ULL,
    0x3e6f1b605d019ef1ULL, 0x3e4386e8a2189563ULL,
    0x3e3b19fa5d306ba7ULL, 0x3e6dd749b67aef76ULL,
    0x3e676ff6f1dc04b0ULL, 0x3e635a33d0b232a6ULL,
    0x3e64bdc80024a4e1ULL, 0x3e6ebd61770fd723ULL,
    0x3e64769fc537264dULL, 0x3e69021f429f3b98ULL,
    0x3e5ee7083efbd606ULL, 0x3e6ad985552a6b1aULL,
    0x3e6e3df778772160ULL, 0x3e6ca5d76ddc9b34ULL,
    0x3e691154ffdbaf74ULL, 0x3e667bdd57fb306aULL,
    0x3e67dc255ac40886ULL, 0x3df219f38e8afafeULL,
    0x3e62416bf9669a04ULL, 0x3e611c96b2b3987fULL,
    0x3e6f99ed447e1177ULL, 0x3e13245826328a11ULL,
    0x3e66f56dd1e645f8ULL, 0x3e46164946945535ULL,
    0x3e5e37d59d190028ULL, 0x3e668671f12bf828ULL,
    0x3e6e8ecbca6aabbdULL, 0x3e53f49e109a5912ULL,
    0x3e6b8a0e11ec3043ULL, 0x3e65fae00aed691aULL,
    0x3e6c0569bece3e4aULL, 0x3e605e26744efbfeULL,
    0x3e65b570a94be5c5ULL, 0x3e5d6f156ea0e063ULL,
    0x3e6e0ca7612fc484ULL, 0x3e4963c927b25258ULL,
    0x3e547930aa725a5cULL, 0x3e58a79fe3af43b3ULL,
    0x3e5e6dc29c41bdafULL, 0x3e657a2e76f863a5ULL,
    0x3e2ae3b61716354dULL, 0x3e665fb5df6906b1ULL,
    0x3e66177d7f588f7bULL, 0x3e3ad55abd091b67ULL,
    0x3e155337b2422d76ULL, 0x3e6084ebe86972d5ULL,
    0x3e656395808e1ea3ULL, 0x3e61bce21b40fba7ULL,
    0x3e5006f94605b515ULL, 0x3e6aa676aceb1f7dULL,
    0x3e58229f76554ce6ULL, 0x3e6eabfc6cf57330ULL,
    0x3e64daed9c0ce8bcULL, 0x3e60ff1768237141ULL,
    0x3e6575f83051b085ULL, 0x3e42667deb523e29ULL,
    0x3e1816996954f4fdULL, 0x3e587cfccf4d9cd4ULL,
    0x3e52c5d018198353ULL, 0x3e6a7a898dcc34aaULL,
    0x3e2cead6dadc36d1ULL, 0x3e2a55759c498bdfULL,
    0x3e6c414a9ef6de04ULL, 0x3e63e2108a6e58faULL,
    0x3e5587fd7643d77cULL, 0x3e3901eb1d3ff3dfULL,
    0x3e6f2ccd7c812fc6ULL, 0x3e21c8ee70a01049ULL,
    0x3e563e8d02831eecULL, 0x3e6f61a42a92c7ffULL,
    0x3dda917399c84d24ULL, 0x3e5e9197c8eec2f0ULL,
    0x3e5e6f842f5a1378ULL, 0x3e2fac242a90a0fcULL,
    0x3e535ed726610227ULL, 0x3e50e0d64804b15bULL,
    0x3e0560675daba814ULL, 0x3e637388c8768032ULL,
    0x3e3ee3c89f9e01f5ULL, 0x3e639f6f0d09747cULL,
    0x3e4322c327abb8f0ULL, 0x3e6961b347c8ac80ULL,
    0x3e63711fbbd0f118ULL, 0x3e64fad8d7718ffbULL,
    0x3e6fffffffffffffULL, 0x3e667efa79ec35b4ULL,
    0x3e6a737687a254a8ULL, 0x3e5bace0f87d924dULL,
    0x3e629e37c237e392ULL, 0x3e557ce7ac3f3012ULL,
    0x3e682829359f8fbdULL, 0x3e6cc9be42d14676ULL,
    0x3e6a8f001c137d0bULL, 0x3e636127687dda05ULL,
    0x3e524dba322646f0ULL, 0x3e6dc43f1ed210b4ULL,
    0x3e631ae515c447bbULL,
};

static const uint64_t CBRTREMH[5] ALIGN32_USED = {
    0x3fe428a2f0000000ULL, 0x3fe965fea0000000ULL, /* indices -2, -1 */
    0x3ff0000000000000ULL, 0x3ff428a2f0000000ULL,
    0x3ff965fea0000000ULL,
};

static const uint64_t CBRTREML[5] ALIGN32_USED = {
    0x3e531ae515c447bbULL, 0x3e44f5b8f20ac166ULL, /* indices -2, -1 */
    0x0000000000000000ULL, 0x3e631ae515c447bbULL,
    0x3e54f5b8f20ac166ULL,
};

static const uint64_t INVTAB256[257] ALIGN32_USED = {
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
    0x3ff0000000000000ULL,
};

static const uint64_t BiasConstant1022[2] ALIGN32_USED = {
    0x00000000000003feULL, 0x00000000000003feULL,
};

static const uint64_t BiasConstant1023[2] ALIGN32_USED = {
    0x00000000000003ffULL, 0x00000000000003ffULL,
};

static const uint64_t Coefficients1[2] ALIGN32_USED = {
    0x3fd5555555555555ULL, 0x3fd5555555555555ULL,
};

static const uint64_t Coefficients2[2] ALIGN32_USED = {
    0xbfbc71c71c71c71cULL, 0xbfbc71c71c71c71cULL,
};

static const uint64_t Coefficients3[2] ALIGN32_USED = {
    0x3faf9add3c0ca458ULL, 0x3faf9add3c0ca458ULL,
};

static const uint64_t Coefficients4[2] ALIGN32_USED = {
    0xbfa511e8d2b3183bULL, 0xbfa511e8d2b3183bULL,
};

static const uint64_t Coefficients5[2] ALIGN32_USED = {
    0x3f9ee7113506ac13ULL, 0x3f9ee7113506ac13ULL,
};

static const uint64_t Coefficients6[2] ALIGN32_USED = {
    0xbf98090d6221a247ULL, 0xbf98090d6221a247ULL,
};

static const uint64_t DenormalNumber[2] ALIGN32_USED = {
    0xfffffffffffffc01ULL, 0xfffffffffffffc01ULL,
};

static const uint64_t DoubleOneby3[2] ALIGN32_USED = {
    0x3fd5555555555555ULL, 0x3fd5555555555555ULL,
};

static const uint64_t InfMask64[2] ALIGN32_USED = {
    0x7ff0000000000000ULL, 0x7ff0000000000000ULL,
};

static const uint64_t Integer256[2] ALIGN32_USED = {
    0x0000000000000100ULL, 0x0000000000000100ULL,
};

static const uint64_t MantissaMask64[2] ALIGN32_USED = {
    0x000fffffffffffffULL, 0x000fffffffffffffULL,
};

static const uint64_t OneBy512[2] ALIGN32_USED = {
    0x3f60000000000000ULL, 0x3f60000000000000ULL,
};

static const uint64_t OneMask64[2] ALIGN32_USED = {
    0x3ff0000000000000ULL, 0x3ff0000000000000ULL,
};

static const uint64_t QnanMask64[2] ALIGN32_USED = {
    0x0008000000000000ULL, 0x0008000000000000ULL,
};

static const uint64_t SignBit64[2] ALIGN32_USED = {
    0x8000000000000000ULL, 0x8000000000000000ULL,
};

static const uint64_t SignMask64[2] ALIGN32_USED = {
    0x7fffffffffffffffULL, 0x7fffffffffffffffULL,
};

static const uint64_t ZeroPointFive[2] ALIGN32_USED = {
    0x3fe0000000000000ULL, 0x3fe0000000000000ULL,
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((naked, aligned(32)))
#endif
v_f64x2_t ALM_PROTO_FMA3(vrd2_cbrt)(v_f64x2_t x)
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
        vcmpneqpd(xmm0, xmm0, xmm11)
        vxorpd(xmm7, xmm7, xmm7)
        vandpd(mem_rip(SignMask64), xmm0, xmm2)
        vcmpeqpd(xmm7, xmm2, xmm7)
        vpcmpeqq(mem_rip(InfMask64), xmm2, xmm4)
        vandpd(mem_rip(QnanMask64), xmm11, xmm3)
        vorpd(xmm7, xmm4, xmm4)
        vorpd(xmm11, xmm4, xmm11)
        vandpd(xmm11, xmm0, xmm12)
        vandnpd(xmm11, xmm11, xmm1)
        vorpd(xmm1, xmm12, xmm12)
        vandnpd(xmm12, xmm3, xmm1)
        vorpd(xmm3, xmm12, xmm12)
        vorpd(xmm1, xmm12, xmm12)
        vandnpd(xmm2, xmm11, xmm2)
        vandpd(mem_rip(SignBit64), xmm0, xmm1)
        vpsrlq(imm(52), xmm2, xmm3)
        vpsubq(mem_rip(BiasConstant1023), xmm3, xmm3)
        vorpd(mem_rip(OneMask64), xmm2, xmm4)
        vsubpd(mem_rip(OneMask64), xmm4, xmm4)
        vpsrlq(imm(52), xmm4, xmm5)
        vpsubq(mem_rip(BiasConstant1022), xmm5, xmm5)
        vpaddq(xmm3, xmm5, xmm5)
        vpcmpeqq(mem_rip(DenormalNumber), xmm3, xmm6)
        vandnpd(xmm2, xmm6, xmm0)
        vandpd(xmm6, xmm4, xmm2)
        vorpd(xmm0, xmm2, xmm2)
        vandnpd(xmm3, xmm6, xmm0)
        vandpd(xmm6, xmm5, xmm3)
        vorpd(xmm0, xmm3, xmm3)
        vshufps(imm(8), xmm3, xmm3, xmm4)
        vcvtdq2pd(xmm4, xmm4)
        vmulpd(mem_rip(DoubleOneby3), xmm4, xmm5)
        vcvttpd2dq(xmm5, xmm5)
        vpaddd(xmm5, xmm5, xmm6)
        vpaddd(xmm6, xmm5, xmm6)
        vpmovsxdq(xmm5, xmm5)
        vpmovsxdq(xmm6, xmm6)
        vpsubq(xmm6, xmm3, xmm0)
        vpaddq(mem_rip(BiasConstant1023), xmm5, xmm3)
        vpsllq(imm(52), xmm3, xmm3)
        vandpd(mem_rip(MantissaMask64), xmm2, xmm2)
        vorpd(mem_rip(ZeroPointFive), xmm2, xmm4)
        vpsrlq(imm(43), xmm2, xmm5)
        vpsrlq(imm(44), xmm2, xmm6)
        vandpd(mem_rip(L1stbitSet), xmm5, xmm5)
        vorpd(mem_rip(L9thbitSet), xmm6, xmm6)
        vpaddq(xmm5, xmm6, xmm2)
        vshufps(imm(8), xmm2, xmm2, xmm5)
        vcvtdq2pd(xmm5, xmm5)
        vfnmadd231pd(mem_rip(OneBy512), xmm5, xmm4)
        vpsubq(mem_rip(Integer256), xmm2, xmm2)
        lea(mem_rip(INVTAB256), rax)
        vmovd(xmm2, r11)
        vmovlpd(mem(rax, r11, 8), xmm5, xmm5)
        vpunpckhqdq(xmm2, xmm2, xmm6)
        vmovd(xmm6, r11)
        vmovhpd(mem(rax, r11, 8), xmm5, xmm5)
        vmulpd(xmm4, xmm5, xmm5)
        vmulpd(xmm5, xmm5, xmm6)
        vmulpd(xmm6, xmm5, xmm7)
        vmulpd(xmm7, xmm5, xmm8)
        vmulpd(xmm8, xmm5, xmm9)
        vmulpd(xmm9, xmm5, xmm10)
        vmulpd(mem_rip(Coefficients6), xmm10, xmm10)
        vfmadd231pd(mem_rip(Coefficients5), xmm9, xmm10)
        vfmadd231pd(mem_rip(Coefficients4), xmm8, xmm10)
        vfmadd231pd(mem_rip(Coefficients3), xmm7, xmm10)
        vfmadd231pd(mem_rip(Coefficients2), xmm6, xmm10)
        vfmadd132pd(mem_rip(Coefficients1), xmm10, xmm5)
        lea(mem_rip(CBRTREMH+16), rax)
        vmovd(xmm0, r11)
        vmovlpd(mem(rax, r11, 8), xmm8, xmm8)
        vpunpckhqdq(xmm0, xmm0, xmm6)
        vmovd(xmm6, r11)
        vmovhpd(mem(rax, r11, 8), xmm8, xmm8)
        lea(mem_rip(CBRTREML+16), rax)
        vmovd(xmm0, r11)
        vmovlpd(mem(rax, r11, 8), xmm9, xmm9)
        vpunpckhqdq(xmm0, xmm0, xmm6)
        vmovd(xmm6, r11)
        vmovhpd(mem(rax, r11, 8), xmm9, xmm9)
        lea(mem_rip(CBRTFH256), rax)
        vmovd(xmm2, r11)
        vmovlpd(mem(rax, r11, 8), xmm6, xmm6)
        vpunpckhqdq(xmm2, xmm2, xmm4)
        vmovd(xmm4, r11)
        vmovhpd(mem(rax, r11, 8), xmm6, xmm6)
        lea(mem_rip(CBRTFT256), rax)
        vmovd(xmm2, r11)
        vmovlpd(mem(rax, r11, 8), xmm7, xmm7)
        vpunpckhqdq(xmm2, xmm2, xmm4)
        vmovd(xmm4, r11)
        vmovhpd(mem(rax, r11, 8), xmm7, xmm7)
        vmulpd(xmm6, xmm8, xmm2)
        vmulpd(xmm7, xmm9, xmm0)
        vfmadd231pd(xmm7, xmm8, xmm0)
        vfmadd231pd(xmm9, xmm6, xmm0)
        vfmadd231pd(xmm5, xmm0, xmm0)
        vfmadd231pd(xmm5, xmm2, xmm0)
        vaddpd(xmm2, xmm0, xmm0)
        vmulpd(xmm0, xmm3, xmm0)
        vorpd(xmm0, xmm1, xmm0)
        vandpd(xmm11, xmm12, xmm12)
        vandnpd(xmm0, xmm11, xmm11)
        vorpd(xmm12, xmm11, xmm0)
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
