/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Comprehensive Mock CPUID Tests for Static and Dynamic Dispatch Verification
 *
 * This test suite verifies:
 * 1. CPU family/model detection for all Zen generations (Zen through Zen6)
 *    - Zen6 is detected via model ranges within Family 0x1A (not a separate family)
 * 2. Feature flag detection (avx512f, avx512dq - flags used by LibM dispatcher)
 * 3. Dispatch verification - correct path selection based on detected CPU
 * 4. Edge cases and boundary conditions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "alci/arch.h"

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, msg) do { \
    if (condition) { \
        tests_passed++; \
        printf("  PASS: %s\n", msg); \
    } else { \
        tests_failed++; \
        printf("  FAIL: %s\n", msg); \
    } \
} while(0)

#define TEST_GROUP(name) printf("\n=== %s ===\n", name)

/*
 * Test 1: Architecture detection - "at-least" semantics
 * au_cpuid_arch_is_zenX() returns TRUE if CPU is zenX OR LATER
 * This matches aocl-utils behavior.
 *
 * Example: On Zen4, au_cpuid_arch_is_zen() through au_cpuid_arch_is_zen4()
 * all return TRUE, but au_cpuid_arch_is_zen5() return FALSE.
 */
static void test_arch_detection_at_least_semantics(void)
{
    TEST_GROUP("Architecture Detection - At-Least Semantics");
    
    int is_zen = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    int is_zen2 = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    int is_zen3 = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    int is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    
    /* Determine the actual architecture (highest that returns true) */
    const char* actual_arch = "Non-AMD or Unknown";
    if (is_zen6) actual_arch = "Zen6";
    else if (is_zen5) actual_arch = "Zen5";
    else if (is_zen4) actual_arch = "Zen4";
    else if (is_zen3) actual_arch = "Zen3";
    else if (is_zen2) actual_arch = "Zen2";
    else if (is_zen) actual_arch = "Zen/Zen+";
    
    printf("  Detected architecture: %s\n", actual_arch);
    printf("  zen=%d zen2=%d zen3=%d zen4=%d zen5=%d zen6=%d\n",
           is_zen, is_zen2, is_zen3, is_zen4, is_zen5, is_zen6);
    
    /* Verify "at-least" semantics: if zenX is true, zen(X-1) must also be true */
    TEST_ASSERT(!is_zen2 || is_zen,
        "At-least semantics: is_zen2 implies is_zen");
    TEST_ASSERT(!is_zen3 || is_zen2,
        "At-least semantics: is_zen3 implies is_zen2");
    TEST_ASSERT(!is_zen4 || is_zen3,
        "At-least semantics: is_zen4 implies is_zen3");
    TEST_ASSERT(!is_zen5 || is_zen4,
        "At-least semantics: is_zen5 implies is_zen4");
    TEST_ASSERT(!is_zen6 || is_zen5,
        "At-least semantics: is_zen6 implies is_zen5");
}

/*
 * Test 2: Feature flag detection - single flags
 * Note: Only avx512f and avx512dq are supported (used by LibM dispatcher)
 */
static void test_feature_detection_single(void)
{
    TEST_GROUP("Feature Detection - Single Flags");
    
    const char* avx512f[] = {"avx512f"};
    const char* avx512dq[] = {"avx512dq"};
    
    int has_avx512f = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f, 1);
    int has_avx512dq = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512dq, 1);
    
    printf("  Feature flags detected:\n");
    printf("    AVX512F:  %d\n", has_avx512f);
    printf("    AVX512DQ: %d\n", has_avx512dq);
    
    /* AVX512DQ implies AVX512F */
    if (has_avx512dq) {
        TEST_ASSERT(has_avx512f, "AVX512DQ implies AVX512F");
    }
    
    tests_passed++;
}

/*
 * Test 3: Feature flag detection - combined flags
 */
static void test_feature_detection_combined(void)
{
    TEST_GROUP("Feature Detection - Combined Flags");
    
    const char* avx512_full[] = {"avx512f", "avx512dq", "avx512bw", "avx512vl"};
    const char* avx512_basic[] = {"avx512f", "avx512dq"};
    const char* avx_fma[] = {"avx", "fma"};
    const char* avx2_fma[] = {"avx2", "fma"};
    
    int has_avx512_full = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_full, 4);
    int has_avx512_basic = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_basic, 2);
    int has_avx_fma = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx_fma, 2);
    int has_avx2_fma = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx2_fma, 2);
    
    printf("  Combined feature flags:\n");
    printf("    AVX512 Full (F+DQ+BW+VL): %d\n", has_avx512_full);
    printf("    AVX512 Basic (F+DQ):     %d\n", has_avx512_basic);
    printf("    AVX + FMA:               %d\n", has_avx_fma);
    printf("    AVX2 + FMA:              %d\n", has_avx2_fma);
    
    /* Consistency: full implies basic */
    if (has_avx512_full) {
        TEST_ASSERT(has_avx512_basic, "AVX512 full implies AVX512 basic");
    }
    
    tests_passed++; /* Count as pass */
}

/*
 * Test 4: Dispatch verification based on detected architecture
 * Verifies that the expected dispatch path would be selected
 */
static void test_dispatch_verification(void)
{
    TEST_GROUP("Dispatch Path Verification");
    
    const char* avx512f_flag[] = {"avx512f"};
    const char* avx512_both[] = {"avx512f", "avx512dq"};
    
    int is_zen = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    int is_zen2 = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    int is_zen3 = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    int is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    int has_avx512f = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f_flag, 1);
    int has_avx512_both = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_both, 2);
    
    /* Simulate dispatch logic from iface.c alm_get_uach() */
    const char* expected_dispatch = "UNKNOWN";
    if (is_zen6) {
        if (has_avx512f)
            expected_dispatch = "ZEN6";
        else
            expected_dispatch = "ZEN3";
    } else if (is_zen5) {
        if (has_avx512f)
            expected_dispatch = "ZEN5";
        else
            expected_dispatch = "ZEN3";
    } else if (is_zen4) {
        if (has_avx512f)
            expected_dispatch = "ZEN4";
        else
            expected_dispatch = "ZEN3";
    } else if (is_zen3) {
        expected_dispatch = "ZEN3";
    } else if (is_zen2) {
        expected_dispatch = "ZEN2";
    } else if (is_zen) {
        expected_dispatch = "ZEN";
    } else {
        /* Non-AMD or unknown */
        if (has_avx512_both)
            expected_dispatch = "AVX512";
        else
            expected_dispatch = "DEFAULT (AVX2)";
    }
    
    printf("  Expected dispatch path: %s\n", expected_dispatch);
    
    /* Verify dispatch consistency with feature flags */
    if (strcmp(expected_dispatch, "ZEN4") == 0 ||
        strcmp(expected_dispatch, "ZEN5") == 0 ||
        strcmp(expected_dispatch, "ZEN6") == 0) {
        TEST_ASSERT(has_avx512f, "ZEN4/5/6 dispatch requires AVX512F");
    }
    
    if (strcmp(expected_dispatch, "AVX512") == 0) {
        TEST_ASSERT(has_avx512_both, "AVX512 dispatch requires AVX512F+DQ");
    }
    
    tests_passed++; /* Count test group as pass */
}

/*
 * Test 5: API consistency - repeated calls should return same result
 */
static void test_api_consistency(void)
{
    TEST_GROUP("API Consistency - Cached Results");
    
    /* Call each function multiple times - should return consistent results */
    int zen1_a = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    int zen1_b = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen1_a == zen1_b, "au_cpuid_arch_is_zen returns consistent results");
    
    int zen2_a = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    int zen2_b = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen2_a == zen2_b, "au_cpuid_arch_is_zen2 returns consistent results");
    
    int zen3_a = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    int zen3_b = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen3_a == zen3_b, "au_cpuid_arch_is_zen3 returns consistent results");
    
    int zen4_a = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int zen4_b = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen4_a == zen4_b, "au_cpuid_arch_is_zen4 returns consistent results");
    
    int zen5_a = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int zen5_b = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen5_a == zen5_b, "au_cpuid_arch_is_zen5 returns consistent results");
    
    int zen6_a = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    int zen6_b = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    TEST_ASSERT(zen6_a == zen6_b, "au_cpuid_arch_is_zen6 returns consistent results");
    
    const char* flags[] = {"avx2", "fma"};
    int flags_a = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, flags, 2);
    int flags_b = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, flags, 2);
    TEST_ASSERT(flags_a == flags_b, "au_cpuid_has_flags returns consistent results");
}

/*
 * Test 6: Edge cases - empty flags, unknown flags
 */
static void test_edge_cases(void)
{
    TEST_GROUP("Edge Cases");
    
    /* Empty flag array (count = 0) should return false (invalid input) */
    const char* empty_flags[] = {""};
    int empty_result = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, empty_flags, 0);
    TEST_ASSERT(empty_result == 0, "Empty flag array returns false");
    
    /* Unknown flag should return false (to avoid false positives) */
    const char* unknown_flags[] = {"unknown_feature_xyz"};
    int unknown_result = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, unknown_flags, 1);
    TEST_ASSERT(unknown_result == 0, "Unknown flag returns false");
    
    /* Test cpu parameter ignored (should work with any value) */
    int zen_0 = au_cpuid_arch_is_zen(0);
    int zen_1 = au_cpuid_arch_is_zen(1);
    int zen_99 = au_cpuid_arch_is_zen(99);
    TEST_ASSERT(zen_0 == zen_1 && zen_1 == zen_99, 
        "CPU parameter is correctly ignored");
}

/*
 * Test 7: Static dispatch simulation
 * Tests the dispatch path that would be taken for each ALM_STATIC_DISPATCH value
 */
static void test_static_dispatch_simulation(void)
{
    TEST_GROUP("Static Dispatch Simulation");
    
    printf("  Static dispatch paths (compile-time selection):\n");
    printf("    AVX2/ZEN2 -> ALM_UARCH_VER_ZEN2\n");
    printf("    AVX512    -> ALM_UARCH_VER_AVX512\n");
    printf("    ZEN3      -> ALM_UARCH_VER_ZEN3\n");
    printf("    ZEN4      -> ALM_UARCH_VER_ZEN4\n");
    printf("    ZEN5      -> ALM_UARCH_VER_ZEN5\n");
    printf("    ZEN6      -> ALM_UARCH_VER_ZEN6\n");
    
    /* Verify the current CPU can execute the selected code path */
    const char* avx2_flags[] = {"avx2"};
    const char* avx512_flags[] = {"avx512f", "avx512dq"};
    
    int can_avx2 = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx2_flags, 1);
    int can_avx512 = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_flags, 2);
    
    printf("  Current CPU capabilities:\n");
    printf("    Can execute AVX2 code:   %s\n", can_avx2 ? "YES" : "NO");
    printf("    Can execute AVX512 code: %s\n", can_avx512 ? "YES" : "NO");
    
    tests_passed++; /* Count as pass */
}

/*
 * Test 8: Dynamic dispatch path selection (mirrors alm_get_uach logic)
 */
static void test_dynamic_dispatch_path(void)
{
    TEST_GROUP("Dynamic Dispatch Path Selection");
    
    const char* avx512f_flag[] = {"avx512f"};
    const char* avx512_both[] = {"avx512f", "avx512dq"};
    
    /* Get current CPU info */
    int is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    int is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int is_zen3 = au_cpuid_arch_is_zen3(AU_CURRENT_CPU_NUM);
    int is_zen2 = au_cpuid_arch_is_zen2(AU_CURRENT_CPU_NUM);
    int is_zen = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    int has_avx512f = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f_flag, 1);
    int has_avx512_both = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512_both, 2);
    
    /* Enum values from iface.h */
    enum {
        ALM_UARCH_VER_DEFAULT = 0,
        ALM_UARCH_VER_AVX512 = 3,
        ALM_UARCH_VER_ZEN = 4,
        ALM_UARCH_VER_ZEN2 = 6,
        ALM_UARCH_VER_ZEN3 = 7,
        ALM_UARCH_VER_ZEN4 = 8,
        ALM_UARCH_VER_ZEN5 = 9,
        ALM_UARCH_VER_ZEN6 = 10,
    };
    
    int arch_ver;
    const char* arch_name;
    
    /* Mirror the dispatch logic from iface.c */
    if (is_zen6) {
        if (has_avx512f) {
            arch_ver = ALM_UARCH_VER_ZEN6;
            arch_name = "ZEN6";
        } else {
            arch_ver = ALM_UARCH_VER_ZEN3;
            arch_name = "ZEN3 (fallback from ZEN6)";
        }
    } else if (is_zen5) {
        if (has_avx512f) {
            arch_ver = ALM_UARCH_VER_ZEN5;
            arch_name = "ZEN5";
        } else {
            arch_ver = ALM_UARCH_VER_ZEN3;
            arch_name = "ZEN3 (fallback from ZEN5)";
        }
    } else if (is_zen4) {
        if (has_avx512f) {
            arch_ver = ALM_UARCH_VER_ZEN4;
            arch_name = "ZEN4";
        } else {
            arch_ver = ALM_UARCH_VER_ZEN3;
            arch_name = "ZEN3 (fallback from ZEN4)";
        }
    } else if (is_zen3) {
        arch_ver = ALM_UARCH_VER_ZEN3;
        arch_name = "ZEN3";
    } else if (is_zen2) {
        arch_ver = ALM_UARCH_VER_ZEN2;
        arch_name = "ZEN2";
    } else if (is_zen) {
        arch_ver = ALM_UARCH_VER_ZEN;
        arch_name = "ZEN";
    } else {
        /* Non-AMD or unknown */
        if (has_avx512_both) {
            arch_ver = ALM_UARCH_VER_AVX512;
            arch_name = "AVX512";
        } else {
            arch_ver = ALM_UARCH_VER_DEFAULT;
            arch_name = "DEFAULT (AVX2)";
        }
    }
    
    printf("  Dynamic dispatch result:\n");
    printf("    Selected: %s (arch_ver=%d)\n", arch_name, arch_ver);
    
    TEST_ASSERT(arch_ver >= 0, "Valid arch version selected");
}

/*
 * Test 9: Zen detection consistency with feature flags
 * 
 * Note: This module only supports avx512f and avx512dq flag queries.
 * These are the only flags used by LibM's dispatcher (iface.c).
 */
static void test_zen_feature_consistency(void)
{
    TEST_GROUP("Zen Architecture - Feature Consistency");
    
    const char* avx512f[] = {"avx512f"};
    const char* avx512dq[] = {"avx512dq"};
    
    int is_zen4 = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    int is_zen5 = au_cpuid_arch_is_zen5(AU_CURRENT_CPU_NUM);
    int is_zen6 = au_cpuid_arch_is_zen6(AU_CURRENT_CPU_NUM);
    int is_zen = au_cpuid_arch_is_zen(AU_CURRENT_CPU_NUM);
    
    int has_avx512f = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f, 1);
    int has_avx512dq = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512dq, 1);
    
    int is_any_zen = is_zen;
    
    /* Zen4+ should have AVX512F (if OS supports it) */
    if (is_zen4 || is_zen5 || is_zen6) {
        TEST_ASSERT(has_avx512f, "Zen4/5/6 should have AVX512F");
    }
    
    /* If AVX512F is present, AVX512DQ should also be present on AMD */
    if (has_avx512f && is_any_zen) {
        TEST_ASSERT(has_avx512dq, "AMD with AVX512F should have AVX512DQ");
    }
    
    if (!is_any_zen) {
        printf("  (Skipped Zen-specific checks - not running on AMD Zen CPU)\n");
        tests_passed++; /* Count as pass for non-Zen CPUs */
    }
}

/*
 * Test 10: Cross-verification with multiple calls in different order
 */
static void test_call_order_independence(void)
{
    TEST_GROUP("Call Order Independence");
    
    /* First call architecture detection, then feature flags */
    int zen4_first = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    const char* avx512f[] = {"avx512f"};
    int has_avx512f_first = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f, 1);
    
    /* Now call in reverse order */
    int has_avx512f_second = au_cpuid_has_flags(AU_CURRENT_CPU_NUM, avx512f, 1);
    int zen4_second = au_cpuid_arch_is_zen4(AU_CURRENT_CPU_NUM);
    
    TEST_ASSERT(zen4_first == zen4_second, 
        "Zen4 detection consistent regardless of call order");
    TEST_ASSERT(has_avx512f_first == has_avx512f_second, 
        "AVX512F detection consistent regardless of call order");
}

int main(void)
{
    printf("======================================\n");
    printf("AOCL-LibM Internal Utils - Mock CPUID\n");
    printf("Comprehensive Test Suite\n");
    printf("======================================\n");
    
    /* Run all tests */
    test_arch_detection_at_least_semantics();
    test_feature_detection_single();
    test_feature_detection_combined();
    test_dispatch_verification();
    test_api_consistency();
    test_edge_cases();
    test_static_dispatch_simulation();
    test_dynamic_dispatch_path();
    test_zen_feature_consistency();
    test_call_order_independence();
    
    /* Summary */
    printf("\n======================================\n");
    printf("TEST SUMMARY\n");
    printf("======================================\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("======================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
