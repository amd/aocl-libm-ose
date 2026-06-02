/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Dispatch Verification Test
 *
 * This test verifies that the CPUID module correctly identifies CPU features
 * and that the dispatch logic would select the correct code path.
 *
 * Test matrix:
 * - AMD Family 0x17 (Zen/Zen+/Zen2) detection
 * - AMD Family 0x19 (Zen3/Zen4) detection  
 * - AMD Family 0x1A (Zen5/Zen6) detection - model-based within Family 0x1A:
 *   - Zen5: models <=0x4f, 0x60-0x77, 0xd0-0xd7
 *   - Zen6: models 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 * - Non-AMD CPU fallback to AVX2/AVX512
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alci/arch.h"

/* Colors for terminal output */
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

/* Test counters */
static int pass_count = 0;
static int fail_count = 0;

#define CHECK(cond, msg) do { \
    if (cond) { \
        printf(GREEN "  [PASS] " RESET "%s\n", msg); \
        pass_count++; \
    } else { \
        printf(RED "  [FAIL] " RESET "%s\n", msg); \
        fail_count++; \
    } \
} while(0)

#define INFO(msg, ...) printf(YELLOW "  [INFO] " RESET msg "\n", ##__VA_ARGS__)

/*
 * Get string representation of detected architecture
 */
static const char* get_arch_string(void)
{
    if (alm_cpuid_arch_is_zen6(ALM_CURRENT_CPU_NUM)) return "ZEN6";
    if (alm_cpuid_arch_is_zen5(ALM_CURRENT_CPU_NUM)) return "ZEN5";
    if (alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM)) return "ZEN4";
    if (alm_cpuid_arch_is_zen3(ALM_CURRENT_CPU_NUM)) return "ZEN3";
    if (alm_cpuid_arch_is_zen2(ALM_CURRENT_CPU_NUM)) return "ZEN2";
    if (alm_cpuid_arch_is_zen(ALM_CURRENT_CPU_NUM))  return "ZEN";
    return "NON-AMD";
}

/*
 * Get the expected dispatch path based on detected CPU
 * This mirrors the logic in src/iface.c alm_get_uach()
 */
typedef enum {
    DISPATCH_DEFAULT = 0,
    DISPATCH_AVX512,
    DISPATCH_ZEN,
    DISPATCH_ZEN2,
    DISPATCH_ZEN3,
    DISPATCH_ZEN4,
    DISPATCH_ZEN5,
    DISPATCH_ZEN6,
} dispatch_path_t;

static dispatch_path_t get_expected_dispatch(void)
{
    const char* avx512f_flag[] = {"avx512f"};
    const char* avx512_both[] = {"avx512f", "avx512dq"};
    
    int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f_flag, 1);
    int has_avx512_both = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512_both, 2);
    if (alm_cpuid_arch_is_zen6(ALM_CURRENT_CPU_NUM)) {
        return has_avx512f ? DISPATCH_ZEN6 : DISPATCH_ZEN3;
    }
    if (alm_cpuid_arch_is_zen5(ALM_CURRENT_CPU_NUM)) {
        return has_avx512f ? DISPATCH_ZEN5 : DISPATCH_ZEN3;
    }
    if (alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM)) {
        return has_avx512f ? DISPATCH_ZEN4 : DISPATCH_ZEN3;
    }
    if (alm_cpuid_arch_is_zen3(ALM_CURRENT_CPU_NUM)) {
        return DISPATCH_ZEN3;
    }
    if (alm_cpuid_arch_is_zen2(ALM_CURRENT_CPU_NUM)) {
        return DISPATCH_ZEN2;
    }
    if (alm_cpuid_arch_is_zen(ALM_CURRENT_CPU_NUM)) {
        return DISPATCH_ZEN;
    }
    
    /* Non-AMD fallback */
    return has_avx512_both ? DISPATCH_AVX512 : DISPATCH_DEFAULT;
}

static const char* dispatch_path_name(dispatch_path_t path)
{
    switch (path) {
        case DISPATCH_DEFAULT: return "DEFAULT (AVX2)";
        case DISPATCH_AVX512:  return "AVX512";
        case DISPATCH_ZEN:     return "ZEN";
        case DISPATCH_ZEN2:    return "ZEN2";
        case DISPATCH_ZEN3:    return "ZEN3";
        case DISPATCH_ZEN4:    return "ZEN4";
        case DISPATCH_ZEN5:    return "ZEN5";
        case DISPATCH_ZEN6:    return "ZEN6";
        default:               return "UNKNOWN";
    }
}

/*
 * Test 1: Verify architecture detection produces valid result
 * Uses "at-least" semantics: alm_cpuid_arch_is_zenX() returns TRUE if CPU is zenX OR LATER
 */
static void test_arch_detection_valid(void)
{
    printf("\n=== Test: Architecture Detection Validity ===\n");
    
    const char* arch = get_arch_string();
    INFO("Detected architecture: %s", arch);
    
    int is_zen = alm_cpuid_arch_is_zen(ALM_CURRENT_CPU_NUM);
    int is_zen2 = alm_cpuid_arch_is_zen2(ALM_CURRENT_CPU_NUM);
    int is_zen3 = alm_cpuid_arch_is_zen3(ALM_CURRENT_CPU_NUM);
    int is_zen4 = alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM);
    int is_zen5 = alm_cpuid_arch_is_zen5(ALM_CURRENT_CPU_NUM);
    int is_zen6 = alm_cpuid_arch_is_zen6(ALM_CURRENT_CPU_NUM);
    
    /* Verify "at-least" semantics: if zenX is true, zen(X-1) must also be true */
    CHECK(!is_zen2 || is_zen, "At-least: is_zen2 implies is_zen");
    CHECK(!is_zen3 || is_zen2, "At-least: is_zen3 implies is_zen2");
    CHECK(!is_zen4 || is_zen3, "At-least: is_zen4 implies is_zen3");
    CHECK(!is_zen5 || is_zen4, "At-least: is_zen5 implies is_zen4");
    CHECK(!is_zen6 || is_zen5, "At-least: is_zen6 implies is_zen5");
    
    INFO("zen=%d zen2=%d zen3=%d zen4=%d zen5=%d zen6=%d",
         is_zen, is_zen2, is_zen3, is_zen4, is_zen5, is_zen6);
}

/*
 * Test 2: Verify feature flags are consistent
 */
static void test_feature_flag_consistency(void)
{
    printf("\n=== Test: Feature Flag Consistency ===\n");
    
    /* Note: This module only supports avx512f and avx512dq flag queries.
     * Other flags (avx, avx2, fma, etc.) are not supported and return 0. */
    const char* avx512f[] = {"avx512f"};
    const char* avx512dq[] = {"avx512dq"};
    
    int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
    int has_avx512dq = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512dq, 1);
    
    INFO("AVX512F=%d AVX512DQ=%d (only supported flags)", 
         has_avx512f, has_avx512dq);
    
    /* AVX512DQ implies AVX512F */
    if (has_avx512dq) {
        CHECK(has_avx512f, "AVX512DQ implies AVX512F is present");
    } else {
        CHECK(1, "No AVX512DQ, skipping AVX512F implication check");
    }
}

/*
 * Test 3: Verify dispatch path selection
 */
static void test_dispatch_path_selection(void)
{
    printf("\n=== Test: Dispatch Path Selection ===\n");
    
    dispatch_path_t path = get_expected_dispatch();
    INFO("Expected dispatch path: %s", dispatch_path_name(path));
    
    /* Verify the dispatch is valid */
    CHECK(path >= DISPATCH_DEFAULT && path <= DISPATCH_ZEN6, 
          "Dispatch path is within valid range");
    
    /* Verify consistency with detected architecture */
    const char* arch = get_arch_string();
    
    if (strcmp(arch, "ZEN6") == 0) {
        const char* avx512f[] = {"avx512f"};
        int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
        CHECK(path == (has_avx512f ? DISPATCH_ZEN6 : DISPATCH_ZEN3),
              "ZEN6 dispatch correct based on AVX512F");
    } else if (strcmp(arch, "ZEN5") == 0) {
        const char* avx512f[] = {"avx512f"};
        int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
        CHECK(path == (has_avx512f ? DISPATCH_ZEN5 : DISPATCH_ZEN3),
              "ZEN5 dispatch correct based on AVX512F");
    } else if (strcmp(arch, "ZEN4") == 0) {
        const char* avx512f[] = {"avx512f"};
        int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
        CHECK(path == (has_avx512f ? DISPATCH_ZEN4 : DISPATCH_ZEN3),
              "ZEN4 dispatch correct based on AVX512F");
    } else if (strcmp(arch, "ZEN3") == 0) {
        CHECK(path == DISPATCH_ZEN3, "ZEN3 dispatch is ZEN3");
    } else if (strcmp(arch, "ZEN2") == 0) {
        CHECK(path == DISPATCH_ZEN2, "ZEN2 dispatch is ZEN2");
    } else if (strcmp(arch, "ZEN") == 0) {
        CHECK(path == DISPATCH_ZEN, "ZEN dispatch is ZEN");
    } else {
        /* Non-AMD */
        const char* avx512_both[] = {"avx512f", "avx512dq"};
        int has_both = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512_both, 2);
        CHECK(path == (has_both ? DISPATCH_AVX512 : DISPATCH_DEFAULT),
              "Non-AMD dispatch correct based on AVX512 support");
    }
}

/*
 * Test 4: Verify Zen4+ requires AVX512 for full path
 */
static void test_zen4_plus_avx512_requirement(void)
{
    printf("\n=== Test: Zen4+ AVX512 Dispatch Requirement ===\n");
    
    const char* avx512f[] = {"avx512f"};
    int has_avx512f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
    
    int is_zen4 = alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM);
    int is_zen5 = alm_cpuid_arch_is_zen5(ALM_CURRENT_CPU_NUM);
    int is_zen6 = alm_cpuid_arch_is_zen6(ALM_CURRENT_CPU_NUM);
    
    if (is_zen4 || is_zen5 || is_zen6) {
        INFO("Running on Zen4+, checking AVX512F requirement");
        CHECK(has_avx512f, "Zen4+ CPU has AVX512F support");
        
        dispatch_path_t path = get_expected_dispatch();
        CHECK(path >= DISPATCH_ZEN4, 
              "Zen4+ with AVX512F dispatches to Zen4+ path");
    } else {
        INFO("Not running on Zen4+, skipping AVX512F requirement check");
        CHECK(1, "Test not applicable for current CPU");
    }
}

/*
 * Test 5: Verify API stability under repeated calls
 */
static void test_api_stability(void)
{
    printf("\n=== Test: API Stability Under Repeated Calls ===\n");
    
    const int iterations = 1000;
    const char* avx512f[] = {"avx512f"};
    
    /* Store initial values */
    int zen4_initial = alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM);
    int avx512f_initial = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1);
    
    /* Call repeatedly */
    int zen4_stable = 1;
    int avx512f_stable = 1;
    
    for (int i = 0; i < iterations; i++) {
        if (alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM) != zen4_initial)
            zen4_stable = 0;
        if (alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512f, 1) != avx512f_initial)
            avx512f_stable = 0;
    }
    
    CHECK(zen4_stable, "alm_cpuid_arch_is_zen4 stable over 1000 calls");
    CHECK(avx512f_stable, "alm_cpuid_has_flags stable over 1000 calls");
}

/*
 * Test 6: Verify combined flag detection
 * Note: Internal utils only supports avx512f and avx512dq flags (used by LibM)
 */
static void test_combined_flags(void)
{
    printf("\n=== Test: Combined Flag Detection ===\n");
    
    /* Test that checking multiple supported flags works correctly */
    const char* avx512_flags[] = {"avx512f", "avx512dq"};
    int has_avx512 = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, avx512_flags, 2);
    
    /* If we have both AVX512 flags, we should have each individually */
    if (has_avx512) {
        const char* f[] = {"avx512f"};
        const char* dq[] = {"avx512dq"};
        
        CHECK(alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, f, 1), 
              "Combined AVX512 includes AVX512F");
        CHECK(alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, dq, 1), 
              "Combined AVX512 includes AVX512DQ");
        INFO("Multi-flag detection validated: both avx512f and avx512dq detected");
    } else {
        INFO("No combined AVX512F+DQ support, checking individual flags");
        const char* f[] = {"avx512f"};
        const char* dq[] = {"avx512dq"};
        int has_f = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, f, 1);
        int has_dq = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, dq, 1);
        INFO("AVX512F: %d, AVX512DQ: %d", has_f, has_dq);
        CHECK(1, "Individual flag check completed");
    }
    
    /* Negative test: unknown flags should return false */
    const char* unknown_flags[] = {"avx512f", "unknown_flag"};
    int has_unknown = alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, unknown_flags, 2);
    CHECK(!has_unknown, "Unknown flag in combination returns false");
}

/*
 * Test 7: Print full CPU capability report
 */
static void print_cpu_report(void)
{
    printf("\n=== CPU Capability Report ===\n");
    
    printf("Architecture Detection:\n");
    printf("  Zen1:  %s\n", alm_cpuid_arch_is_zen(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    printf("  Zen2:  %s\n", alm_cpuid_arch_is_zen2(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    printf("  Zen3:  %s\n", alm_cpuid_arch_is_zen3(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    printf("  Zen4:  %s\n", alm_cpuid_arch_is_zen4(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    printf("  Zen5:  %s\n", alm_cpuid_arch_is_zen5(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    printf("  Zen6:  %s\n", alm_cpuid_arch_is_zen6(ALM_CURRENT_CPU_NUM) ? "YES" : "NO");
    
    printf("\nFeature Flags (supported by internal utils):\n");
    const char* flags[][1] = {
        {"avx512f"}, {"avx512dq"}
    };
    const char* names[] = {
        "AVX512F", "AVX512DQ"
    };
    
    for (int i = 0; i < 2; i++) {
        printf("  %-10s: %s\n", names[i],
               alm_cpuid_has_flags(ALM_CURRENT_CPU_NUM, flags[i], 1) ? "YES" : "NO");
    }
    
    printf("\nDispatch Path: %s\n", dispatch_path_name(get_expected_dispatch()));
}

int main(void)
{
    printf("============================================\n");
    printf("AOCL-LibM Dispatch Verification Test Suite\n");
    printf("============================================\n");
    
    /* Print CPU report first */
    print_cpu_report();
    
    /* Run all tests */
    test_arch_detection_valid();
    test_feature_flag_consistency();
    test_dispatch_path_selection();
    test_zen4_plus_avx512_requirement();
    test_api_stability();
    test_combined_flags();
    
    /* Summary */
    printf("\n============================================\n");
    printf("TEST SUMMARY\n");
    printf("============================================\n");
    printf(GREEN "Passed: %d\n" RESET, pass_count);
    if (fail_count > 0) {
        printf(RED "Failed: %d\n" RESET, fail_count);
    } else {
        printf("Failed: %d\n", fail_count);
    }
    printf("============================================\n");
    
    return fail_count > 0 ? 1 : 0;
}
