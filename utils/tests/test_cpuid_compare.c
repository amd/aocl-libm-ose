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

/*
 * CPUID Comparison Test
 *
 * This test can be compiled against EITHER:
 *   1. Internal alm_utils (default)
 *   2. External aocl-utils (with -DUSE_EXTERNAL_AOCLUTILS)
 *
 * Both versions output results in the same format for easy comparison via diff.
 *
 * Build and run:
 *   # Internal utils version:
 *   gcc -I../include test_cpuid_compare.c -L.. -lalm_utils -o test_cpuid_internal
 *   ./test_cpuid_internal > internal.txt
 *
 *   # External aocl-utils version:
 *   gcc -DUSE_EXTERNAL_AOCLUTILS test_cpuid_compare.c $(pkg-config --cflags --libs aocl-utils) -o test_cpuid_external
 *   ./test_cpuid_external > external.txt
 *
 *   # Compare outputs:
 *   diff internal.txt external.txt
 */

#include <stdio.h>
#include <stdbool.h>

#ifdef USE_EXTERNAL_AOCLUTILS
    #include <alci/arch.h>
    #define UTILS_SOURCE "external aocl-utils"
#else
    #include "alci/arch.h"
    #define UTILS_SOURCE "internal alm_utils"
#endif

int main(void)
{
    printf("# CPUID comparison output (%s)\n", UTILS_SOURCE);
    printf("# Format: function_name: result\n");
    printf("#\n");

    /*
     * Architecture detection functions.
     * These are the common subset supported by both internal and external utils.
     */
    printf("au_cpuid_arch_is_zen: %d\n",
           au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM) ? 1 : 0);

    printf("au_cpuid_arch_is_zen2: %d\n",
           au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM) ? 1 : 0);

    printf("au_cpuid_arch_is_zen3: %d\n",
           au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM) ? 1 : 0);

    printf("au_cpuid_arch_is_zen4: %d\n",
           au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM) ? 1 : 0);

    printf("au_cpuid_arch_is_zen5: %d\n",
           au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM) ? 1 : 0);

    printf("au_cpuid_arch_is_zen6: %d\n",
           au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM) ? 1 : 0);

    /*
     * Feature flag detection.
     * Test the flags that LibM actually uses.
     */
    const char* avx512f_flag[] = {"avx512f"};
    printf("au_cpuid_has_flags[avx512f]: %d\n",
           au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f_flag, 1) ? 1 : 0);

    const char* avx512dq_flag[] = {"avx512dq"};
    printf("au_cpuid_has_flags[avx512dq]: %d\n",
           au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512dq_flag, 1) ? 1 : 0);

    const char* avx512_both[] = {"avx512f", "avx512dq"};
    printf("au_cpuid_has_flags[avx512f,avx512dq]: %d\n",
           au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_both, 2) ? 1 : 0);

    /*
     * Note: Internal utils only implements avx512f and avx512dq flags
     * (the only flags LibM actually uses). Testing additional flags would
     * show expected differences since they're not implemented internally.
     */

    printf("# End of comparison output\n");

    return 0;
}
