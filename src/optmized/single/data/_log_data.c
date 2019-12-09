#include <stdint.h>

struct logf_table {
    uint32_t f_inv, f_128_head, f_128_tail;
} logf_lookup[257]= {
{0x40000000, 0x0, 0x0},
{0x3fff00ff, 0x3b7f8000, 0x32aa2b11},
{0x3ffe03f8, 0x3bff0000, 0x3429ac42},
{0x3ffd08e5, 0x3c3ee000, 0x350ebf02},
{0x3ffc0fc1, 0x3c7e0000, 0x35a8b0fc},
{0x3ffb1885, 0x3c9e7000, 0x36244347},
{0x3ffa232d, 0x3cbdc000, 0x368d83eb},
{0x3ff92fb2, 0x3cdcf000, 0x36e013d8},
{0x3ff83e10, 0x3cfc1000, 0x361b0e78},
{0x3ff74e40, 0x3d0d8000, 0x36d98924},
{0x3ff6603e, 0x3d1cf000, 0x3687b9ff},
{0x3ff57404, 0x3d2c5000, 0x36375f92},
{0x3ff4898d, 0x3d3ba000, 0x3631ec66},
{0x3ff3a0d5, 0x3d4ae000, 0x36830ec9},
{0x3ff2b9d6, 0x3d5a1000, 0x36dd7119},
{0x3ff1d48c, 0x3d693000, 0x3735c56e},
{0x3ff0f0f1, 0x3d785000, 0x35c30046},
{0x3ff00f01, 0x3d83a000, 0x37cc1acc},
{0x3fef2eb7, 0x3d8b2000, 0x379b7752},
{0x3fee500f, 0x3d929000, 0x37fb1785},
{0x3fed7304, 0x3d9a0000, 0x37ebcb0e},
{0x3fec9791, 0x3da17000, 0x375cbea6},
{0x3febbdb3, 0x3da8d000, 0x37839f83},
{0x3feae564, 0x3db03000, 0x36b1526f},
{0x3fea0ea1, 0x3db78000, 0x37528ae5},
{0x3fe93965, 0x3dbed000, 0x36ecdaf6},
{0x3fe865ac, 0x3dc61000, 0x37a2eb19},
{0x3fe79373, 0x3dcd5000, 0x37a12310},
{0x3fe6c2b4, 0x3dd49000, 0x36da7496},
{0x3fe5f36d, 0x3ddbc000, 0x37482bb1},
{0x3fe52598, 0x3de2f000, 0x36a91eb8},
{0x3fe45933, 0x3dea1000, 0x3789eb36},
{0x3fe38e39, 0x3df13000, 0x3783b715},
{0x3fe2c4a7, 0x3df85000, 0x36430046},
{0x3fe1fc78, 0x3dff6000, 0x371131dc},
{0x3fe135aa, 0x3e033000, 0x380a8965},
{0x3fe07038, 0x3e06b000, 0x383f3e68},
{0x3fdfac1f, 0x3e0a3000, 0x3842c234},
{0x3fdee95c, 0x3e0db000, 0x38156a98},
{0x3fde27eb, 0x3e113000, 0x375e3215},
{0x3fdd67c9, 0x3e14a000, 0x38297c10},
{0x3fdca8f1, 0x3e181000, 0x386b8c72},
{0x3fdbeb62, 0x3e1b8000, 0x387e100f},
{0x3fdb2f17, 0x3e1ef000, 0x38615876},
{0x3fda740e, 0x3e226000, 0x3815b666},
{0x3fd9ba42, 0x3e25d000, 0x36dbce69},
{0x3fd901b2, 0x3e293000, 0x37e5e3a2},
{0x3fd84a5a, 0x3e2c9000, 0x381c6ccc},
{0x3fd79436, 0x3e2ff000, 0x38183854},
{0x3fd6df44, 0x3e335000, 0x37cd4273},
{0x3fd62b81, 0x3e36b000, 0x35fe719d},
{0x3fd578e9, 0x3e3a0000, 0x37f8f540},
{0x3fd4c77b, 0x3e3d5000, 0x38448108},
{0x3fd41733, 0x3e40a000, 0x386050a2},
{0x3fd3680d, 0x3e43f000, 0x38503290},
{0x3fd2ba08, 0x3e474000, 0x38146f44},
{0x3fd20d21, 0x3e4a9000, 0x373539e9},
{0x3fd16154, 0x3e4dd000, 0x381b173f},
{0x3fd0b6a0, 0x3e511000, 0x385e0ff1},
{0x3fd00d01, 0x3e545000, 0x38767e44},
{0x3fcf6475, 0x3e579000, 0x3864a740},
{0x3fcebcf9, 0x3e5ad000, 0x3828cf48},
{0x3fce168a, 0x3e5e1000, 0x3786742e},
{0x3fcd7127, 0x3e614000, 0x38342ac6},
{0x3fcccccd, 0x3e647000, 0x387be3cd},
{0x3fcc2978, 0x3e67b000, 0x36d53827},
{0x3fcb8728, 0x3e6ae000, 0x3685ad3f},
{0x3fcae5d8, 0x3e6e0000, 0x385e5056},
{0x3fca4588, 0x3e713000, 0x3803b715},
{0x3fc9a634, 0x3e746000, 0x3494aa97},
{0x3fc907da, 0x3e778000, 0x37adcbdc},
{0x3fc86a79, 0x3e7aa000, 0x38052b26},
{0x3fc7ce0c, 0x3e7dc000, 0x380c36af},
{0x3fc73294, 0x3e807000, 0x37d88b5b},
{0x3fc6980c, 0x3e820000, 0x371652d3},
{0x3fc5fe74, 0x3e838000, 0x38dc2fe7},
{0x3fc565c8, 0x3e851000, 0x3892713a},
{0x3fc4ce08, 0x3e86a000, 0x37d6af35},
{0x3fc43730, 0x3e882000, 0x38c5fcd7},
{0x3fc3a13e, 0x3e89b000, 0x38070294},
{0x3fc30c31, 0x3e8b3000, 0x38ae55d6},
{0x3fc27806, 0x3e8cc000, 0x3652dd42},
{0x3fc1e4bc, 0x3e8e4000, 0x3818c16a},
{0x3fc15250, 0x3e8fc000, 0x387f9e49},
{0x3fc0c0c1, 0x3e914000, 0x38a0fde8},
{0x3fc0300c, 0x3e92c000, 0x38b00870},
{0x3fbfa030, 0x3e944000, 0x38ad09ef},
{0x3fbf112b, 0x3e95c000, 0x38981d5c},
{0x3fbe82fa, 0x3e974000, 0x3862bae1},
{0x3fbdf59d, 0x3e98c000, 0x37e392a9},
{0x3fbd6910, 0x3e9a3000, 0x38eecd4c},
{0x3fbcdd53, 0x3e9bb000, 0x38933160},
{0x3fbc5264, 0x3e9d3000, 0x3798aad3},
{0x3fbbc841, 0x3e9ea000, 0x38a7d2e1},
{0x3fbb3ee7, 0x3ea02000, 0x37421a1b},
{0x3fbab656, 0x3ea19000, 0x386f2a05},
{0x3fba2e8c, 0x3ea30000, 0x38c5e10e},
{0x3fb9a786, 0x3ea48000, 0x35d00801},
{0x3fb92144, 0x3ea5f000, 0x37bf2aef},
{0x3fb89bc3, 0x3ea76000, 0x38173260},
{0x3fb81703, 0x3ea8d000, 0x382d872e},
{0x3fb79301, 0x3eaa4000, 0x3822c3ae},
{0x3fb70fbb, 0x3eabb000, 0x37ee2e8b},
{0x3fb68d31, 0x3ead2000, 0x372ac3d3},
{0x3fb60b61, 0x3eae8000, 0x38dedfac},
{0x3fb58a48, 0x3eaff000, 0x38983854},
{0x3fb509e7, 0x3eb16000, 0x3802f2ba},
{0x3fb48a3a, 0x3eb2c000, 0x38dab982},
{0x3fb40b41, 0x3eb43000, 0x38481e9b},
{0x3fb38cfa, 0x3eb59000, 0x38dd911b},
{0x3fb30f63, 0x3eb70000, 0x380eaa2c},
{0x3fb2927c, 0x3eb86000, 0x38a1713c},
{0x3fb21643, 0x3eb9c000, 0x38ebfb5e},
{0x3fb19ab6, 0x3ebb3000, 0x379c2474},
{0x3fb11fd4, 0x3ebc9000, 0x38255fdd},
{0x3fb0a59b, 0x3ebdf000, 0x385e0a38},
{0x3fb02c0b, 0x3ebf5000, 0x38783b83},
{0x3fafb322, 0x3ec0b000, 0x38741da1},
{0x3faf3ade, 0x3ec21000, 0x3851da1f},
{0x3faec33e, 0x3ec37000, 0x38119a33},
{0x3fae4c41, 0x3ec4d000, 0x374e1b05},
{0x3fadd5e6, 0x3ec62000, 0x38dbe42c},
{0x3fad602b, 0x3ec78000, 0x388f439b},
{0x3faceb10, 0x3ec8e000, 0x37cfd68b},
{0x3fac7692, 0x3eca3000, 0x38ca0e11},
{0x3fac02b0, 0x3ecb9000, 0x38234113},
{0x3fab8f6a, 0x3ecce000, 0x38cac08c},
{0x3fab1cbe, 0x3ece4000, 0x37d605b8},
{0x3faaaaab, 0x3ecf9000, 0x3891f660},
{0x3faa392f, 0x3ed0e000, 0x38e0326b},
{0x3fa9c84a, 0x3ed24000, 0x378121cb},
{0x3fa957fb, 0x3ed39000, 0x3824966c},
{0x3fa8e83f, 0x3ed4e000, 0x386c9a9a},
{0x3fa87917, 0x3ed63000, 0x388c612c},
{0x3fa80a81, 0x3ed78000, 0x38949924},
{0x3fa79c7b, 0x3ed8d000, 0x388f075f},
{0x3fa72f05, 0x3eda2000, 0x38777bcd},
{0x3fa6c21e, 0x3edb7000, 0x38359d3d},
{0x3fa655c4, 0x3edcc000, 0x37b12d26},
{0x3fa5e9f7, 0x3ede0000, 0x38f04587},
{0x3fa57eb5, 0x3edf5000, 0x38a6ced4},
{0x3fa513fd, 0x3ee0a000, 0x381ff116},
{0x3fa4a9cf, 0x3ee1e000, 0x38ebd3e7},
{0x3fa44029, 0x3ee33000, 0x3874e3ff},
{0x3fa3d70a, 0x3ee47000, 0x38fbe3cd},
{0x3fa36e72, 0x3ee5c000, 0x3860744d},
{0x3fa3065e, 0x3ee70000, 0x38d785c3},
{0x3fa29ecf, 0x3ee85000, 0x37c75ce4},
{0x3fa237c3, 0x3ee99000, 0x387e7e01},
{0x3fa1d13a, 0x3eead000, 0x38bfcd71},
{0x3fa16b31, 0x3eec1000, 0x38f392c5},
{0x3fa105a9, 0x3eed6000, 0x3754f8b1},
{0x3fa0a0a1, 0x3eeea000, 0x37d40984},
{0x3fa03c17, 0x3eefe000, 0x38059907},
{0x3f9fd80a, 0x3ef12000, 0x38081a7c},
{0x3f9f747a, 0x3ef26000, 0x37e350d1},
{0x3f9f1166, 0x3ef3a000, 0x3784c3ad},
{0x3f9eaecd, 0x3ef4d000, 0x38fd32cc},
{0x3f9e4cad, 0x3ef61000, 0x38cce923},
{0x3f9deb07, 0x3ef75000, 0x38906320},
{0x3f9d89d9, 0x3ef89000, 0x380f5faf},
{0x3f9d2922, 0x3ef9c000, 0x38f2de41},
{0x3f9cc8e1, 0x3efb0000, 0x3891fd38},
{0x3f9c6917, 0x3efc4000, 0x37946dfd},
{0x3f9c09c1, 0x3efd7000, 0x38ac47bc},
{0x3f9baadf, 0x3efeb000, 0x379e41ec},
{0x3f9b4c70, 0x3effe000, 0x3897042c},
{0x3f9aee73, 0x3f008000, 0x397d5893},
{0x3f9a90e8, 0x3f012000, 0x392952d3},
{0x3f9a33cd, 0x3f01c000, 0x389eefce},
{0x3f99d723, 0x3f025000, 0x396fced5},
{0x3f997ae7, 0x3f02f000, 0x390a5e93},
{0x3f991f1a, 0x3f039000, 0x37f97073},
{0x3f98c3bb, 0x3f042000, 0x392e4426},
{0x3f9868c8, 0x3f04c000, 0x385e9eae},
{0x3f980e41, 0x3f055000, 0x393b5f67},
{0x3f97b426, 0x3f05f000, 0x3865c84a},
{0x3f975a75, 0x3f068000, 0x3931e65d},
{0x3f97012e, 0x3f072000, 0x38130ba4},
{0x3f96a850, 0x3f07b000, 0x39120e4e},
{0x3f964fda, 0x3f084000, 0x3979cf17},
{0x3f95f7cc, 0x3f08e000, 0x38b81788},
{0x3f95a025, 0x3f097000, 0x3938caca},
{0x3f9548e5, 0x3f0a1000, 0x37809491},
{0x3f94f209, 0x3f0aa000, 0x38c3d2f5},
{0x3f949b93, 0x3f0b3000, 0x392e55d6},
{0x3f944581, 0x3f0bc000, 0x39755dec},
{0x3f93efd2, 0x3f0c6000, 0x385c1fec},
{0x3f939a86, 0x3f0cf000, 0x38e6b468},
{0x3f93459c, 0x3f0d8000, 0x392a5abf},
{0x3f92f114, 0x3f0e1000, 0x395c0fb9},
{0x3f929cec, 0x3f0eb000, 0x3707f33c},
{0x3f924925, 0x3f0f4000, 0x383ebce1},
{0x3f91f5bd, 0x3f0fd000, 0x38a34b87},
{0x3f91a2b4, 0x3f106000, 0x38dcd193},
{0x3f915009, 0x3f10f000, 0x3905fe33},
{0x3f90fdbc, 0x3f118000, 0x39186bdf},
{0x3f90abcc, 0x3f121000, 0x3925b7a4},
{0x3f905a38, 0x3f12a000, 0x392de74c},
{0x3f900901, 0x3f133000, 0x3931009a},
{0x3f8fb824, 0x3f13c000, 0x392f0945},
{0x3f8f67a2, 0x3f145000, 0x392806fb},
{0x3f8f177a, 0x3f14e000, 0x391bff61},
{0x3f8ec7ab, 0x3f157000, 0x390af813},
{0x3f8e7835, 0x3f160000, 0x38e9ed45},
{0x3f8e2918, 0x3f169000, 0x38b4012f},
{0x3f8dda52, 0x3f172000, 0x38686dc8},
{0x3f8d8be3, 0x3f17b000, 0x37aa6542},
{0x3f8d3dcb, 0x3f183000, 0x396b99a8},
{0x3f8cf009, 0x3f18c000, 0x393d07d4},
{0x3f8ca29c, 0x3f195000, 0x39099c89},
{0x3f8c5584, 0x3f19e000, 0x38a2ba33},
{0x3f8c08c1, 0x3f1a7000, 0x37a27674},
{0x3f8bbc51, 0x3f1af000, 0x395276ea},
{0x3f8b7034, 0x3f1b8000, 0x390bdaa4},
{0x3f8b246b, 0x3f1c1000, 0x3880fe58},
{0x3f8ad8f3, 0x3f1c9000, 0x397069ab},
{0x3f8a8dcd, 0x3f1d2000, 0x391b9f3f},
{0x3f8a42f8, 0x3f1db000, 0x38844a00},
{0x3f89f874, 0x3f1e3000, 0x3963fffc},
{0x3f89ae41, 0x3f1ec000, 0x39013539},
{0x3f89645c, 0x3f1f5000, 0x37ce4dad},
{0x3f891ac7, 0x3f1fd000, 0x392dc269},
{0x3f88d181, 0x3f206000, 0x38749102},
{0x3f888889, 0x3f20e000, 0x3947f423},
{0x3f883fde, 0x3f217000, 0x389c6de0},
{0x3f87f781, 0x3f21f000, 0x394ff17d},
{0x3f87af70, 0x3f228000, 0x389a5134},
{0x3f8767ab, 0x3f230000, 0x3945e10e},
{0x3f872033, 0x3f239000, 0x38687e67},
{0x3f86d905, 0x3f241000, 0x3929e8f5},
{0x3f869223, 0x3f24a000, 0x37aa0e8c},
{0x3f864b8a, 0x3f252000, 0x38f85db0},
{0x3f86053c, 0x3f25a000, 0x395eb4ab},
{0x3f85bf37, 0x3f263000, 0x38735f9a},
{0x3f85797c, 0x3f26b000, 0x39169d1d},
{0x3f853408, 0x3f273000, 0x396c08dc},
{0x3f84eedd, 0x3f27c000, 0x38747ea0},
{0x3f84a9fa, 0x3f284000, 0x3909e601},
{0x3f84655e, 0x3f28c000, 0x3952605d},
{0x3f842108, 0x3f295000, 0x37b4996f},
{0x3f83dcf9, 0x3f29d000, 0x38ad05ba},
{0x3f839930, 0x3f2a5000, 0x391233cd},
{0x3f8355ad, 0x3f2ad000, 0x3949aa5a},
{0x3f83126f, 0x3f2b5000, 0x397ceada},
{0x3f82cf75, 0x3f2be000, 0x382fe66e},
{0x3f828cc0, 0x3f2c6000, 0x38adb5cd},
{0x3f824a4e, 0x3f2ce000, 0x38fb25fb},
{0x3f820821, 0x3f2d6000, 0x3920261b},
{0x3f81c636, 0x3f2de000, 0x393e9874},
{0x3f81848e, 0x3f2e6000, 0x3958ee36},
{0x3f814328, 0x3f2ee000, 0x396f2b8a},
{0x3f810204, 0x3f2f7000, 0x35aa4906},
{0x3f80c122, 0x3f2ff000, 0x3776d68c},
{0x3f808081, 0x3f307000, 0x37cbd11e},
{0x3f804020, 0x3f30f000, 0x37fbf692},
{0x3f800000, 0x3f317000, 0x3805fdf4},
};


