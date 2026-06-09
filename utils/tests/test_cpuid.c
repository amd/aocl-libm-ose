/*
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Unit tests for alm_utils CPUID module
 *
 * Note: au_cpuid_arch_is_zenX() uses "at-least" semantics (matching aocl-utils):
 * - au_cpuid_arch_is_zen() returns TRUE if Zen or later
 * - au_cpuid_arch_is_zen4() returns TRUE if Zen4 or later
 * So on a Zen4 CPU, zen/zen2/zen3/zen4 all return TRUE.
 *
 * Usage:
 *   ./test_cpuid                    # Run tests, print results (always passes semantic checks)
 *   ./test_cpuid <expected_arch>    # Validate against expected architecture
 *
 * Where <expected_arch> is one of: zen, zen+, zen2, zen3, zen4, zen5, zen6, unknown
 *
 * Example for CI/CD on a Zen4 machine:
 *   ./test_cpuid zen4    # Verifies this machine is detected as Zen4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alci/arch.h"

static void print_usage(const char* prog)
{
    printf("Usage: %s [expected_arch]\n", prog);
    printf("  expected_arch: zen, zen+, zen2, zen3, zen4, zen5, zen6, unknown\n");
    printf("  If not provided, just prints detected values.\n");
}

int main(int argc, char* argv[])
{
    int result = 0;
    const char* expected_arch = NULL;

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        expected_arch = argv[1];
    }

    printf("=== ALM Utils CPUID Test ===\n\n");

    /* Architecture detection - uses "at-least" semantics */
    int is_zen = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    int is_zen2 = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    int is_zen3 = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    int is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);

    printf("Architecture: zen=%d zen2=%d zen3=%d zen4=%d zen5=%d zen6=%d\n",
           is_zen, is_zen2, is_zen3, is_zen4, is_zen5, is_zen6);

    /* Determine detected architecture */
    const char* detected = "unknown";
    if (is_zen6) detected = "zen6";
    else if (is_zen5) detected = "zen5";
    else if (is_zen4) detected = "zen4";
    else if (is_zen3) detected = "zen3";
    else if (is_zen2) detected = "zen2";
    else if (is_zen) detected = "zen";

    printf("Detected architecture: %s\n", detected);

    /* Verify "at-least" semantics: if zenX is true, zen(X-1) must also be true */
    if (is_zen2 && !is_zen) { printf("FAIL: is_zen2 but not is_zen\n"); result = 1; }
    if (is_zen3 && !is_zen2) { printf("FAIL: is_zen3 but not is_zen2\n"); result = 1; }
    if (is_zen4 && !is_zen3) { printf("FAIL: is_zen4 but not is_zen3\n"); result = 1; }
    if (is_zen5 && !is_zen4) { printf("FAIL: is_zen5 but not is_zen4\n"); result = 1; }
    if (is_zen6 && !is_zen5) { printf("FAIL: is_zen6 but not is_zen5\n"); result = 1; }

    /* Feature detection */
    const char* avx512f[] = {"avx512f"};
    const char* avx512_both[] = {"avx512f", "avx512dq"};
    int has_avx512f = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f, 1);
    int has_avx512dq = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_both, 2);

    printf("Features: avx512f=%d avx512f+dq=%d\n", has_avx512f, has_avx512dq);

    /* Validate against expected architecture if provided */
    if (expected_arch != NULL) {
        int match = 0;

        if (strcmp(expected_arch, "unknown") == 0)
            match = !is_zen;
        else if (strcmp(expected_arch, "zen") == 0 || strcmp(expected_arch, "zen+") == 0)
            match = is_zen && !is_zen2;
        else if (strcmp(expected_arch, "zen2") == 0)
            match = is_zen2 && !is_zen3;
        else if (strcmp(expected_arch, "zen3") == 0)
            match = is_zen3 && !is_zen4;
        else if (strcmp(expected_arch, "zen4") == 0)
            match = is_zen4 && !is_zen5;
        else if (strcmp(expected_arch, "zen5") == 0)
            match = is_zen5 && !is_zen6;
        else if (strcmp(expected_arch, "zen6") == 0)
            match = is_zen6;
        else {
            printf("FAIL: Unknown expected architecture '%s'\n", expected_arch);
            print_usage(argv[0]);
            return 1;
        }

        if (match) {
            printf("PASS: Detected '%s' matches expected '%s'\n", detected, expected_arch);
        } else {
            printf("FAIL: Detected '%s' does not match expected '%s'\n", detected, expected_arch);
            result = 1;
        }
    }

    printf("\n=== Test %s ===\n", result == 0 ? "PASSED" : "FAILED");
    return result;
}
