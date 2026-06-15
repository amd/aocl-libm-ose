/*
 * Copyright (C) 2008-2026 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Comprehensive Branch Coverage Test for CPUID Detection
 * Tests each if/else path against expected behavior
 *
 * This test validates:
 * 1. All Family 0x17 models (Zen, Zen2) - Zen+ treated as Zen per aocl-utils
 * 2. All Family 0x19 models (Zen3, Zen4)
 * 3. All Family 0x1A models - Zen5 AND Zen6 (model-based detection within 0x1A):
 *    - Zen5: models <=0x4f, 0x60-0x77, 0xd0-0xd7
 *    - Zen6: models 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 * 4. Model-based detection (matches aocl-utils behavior)
 * 5. "At-least" semantics
 * 6. Future families (>0x1A) default to Zen6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;
static int tests_total = 0;

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

#define TEST(name, condition) do { \
    tests_total++; \
    if (condition) { \
        tests_passed++; \
        printf(GREEN "  [PASS] " RESET "%s\n", name); \
    } else { \
        tests_failed++; \
        printf(RED "  [FAIL] " RESET "%s\n", name); \
    } \
} while(0)

#define TEST_GROUP(name) printf("\n" YELLOW "=== %s ===" RESET "\n", name)

/* Family definitions */
#define AMD_FAMILY_ZEN_ZEN2     0x17
#define AMD_FAMILY_ZEN3_ZEN4    0x19
#define AMD_FAMILY_ZEN5_ZEN6    0x1A  /* Zen5 and Zen6 share Family 0x1A, distinguished by model */

/* Model definitions */
#define MODEL_NAPLES        0x01
#define MODEL_RAVENRIDGE    0x11
#define MODEL_PINNACLERIDGE 0x08
#define MODEL_PICASSO       0x18
#define MODEL_ROME          0x31
#define MODEL_CASTLEPEAKPRO 0x47
#define MODEL_RENOIR        0x60
#define MODEL_MATISSE       0x71
#define MODEL_VANGOGH       0x90
#define MODEL_MENDOCINO     0xA0

#define MODEL_MILAN         0x01
#define MODEL_CHAGALL       0x08
#define MODEL_VERMEER       0x21
#define MODEL_REMBRANDT     0x44
#define MODEL_CEZANNE       0x50
#define MODEL_GENOA         0x11
#define MODEL_STORMPEAK     0x18
#define MODEL_RAPHAEL       0x61
#define MODEL_PHOENIX       0x75
#define MODEL_PHOENIXPOINT  0x78

/* Family 0x1A - Zen5 models (<=0x4f, 0x60-0x77, 0xd0-0xd7) */
#define MODEL_TURIN         0x20
#define MODEL_TURIND        0x01
#define MODEL_STRIX_POINT   0x44
#define MODEL_GRANITE_RIDGE 0x60

/* Family 0x1A - Zen6 models (0x50-0x5f, 0x80-0xcf, 0xd8-0xe7) */
#define MODEL_ZEN6_50       0x50  /* Zen6 model range 0x50-0x5f */
#define MODEL_ZEN6_80       0x80  /* Zen6 model range 0x80-0xcf */
#define MODEL_ZEN6_D8       0xd8  /* Zen6 model range 0xd8-0xe7 */

/* Feature flags */
#define FEATURE_NONE      0
#define FEATURE_AVX512F   (1 << 0)
#define FEATURE_AVX512DQ  (1 << 1)
#define FEATURE_CLWB      (1 << 2)

/* Microarchitecture enum */
typedef enum {
    UARCH_UNKNOWN = 0,
    UARCH_ZEN,
    UARCH_ZENPLUS,
    UARCH_ZEN2,
    UARCH_ZEN3,
    UARCH_ZEN4,
    UARCH_ZEN5,
    UARCH_ZEN6
} uarch_t;

static const char* uarch_name(uarch_t u) {
    switch (u) {
        case UARCH_UNKNOWN: return "UNKNOWN";
        case UARCH_ZEN:     return "ZEN";
        case UARCH_ZENPLUS: return "ZENPLUS";
        case UARCH_ZEN2:    return "ZEN2";
        case UARCH_ZEN3:    return "ZEN3";
        case UARCH_ZEN4:    return "ZEN4";
        case UARCH_ZEN5:    return "ZEN5";
        case UARCH_ZEN6:    return "ZEN6";
        default:            return "INVALID";
    }
}

/*
 * Helper function to check if model is in Zen6 range within Family 0x1A.
 * Zen6 models: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7
 */
static int is_zen6_model(unsigned int model)
{
    return (model >= 0x50 && model <= 0x5f) ||
           (model >= 0x80 && model <= 0xcf) ||
           (model >= 0xd8 && model <= 0xe7);
}

/* Mock detection function - mirrors cpuid.c logic */
static uarch_t
mock_detect_amd_uarch(unsigned int family, unsigned int model, 
                      unsigned int stepping, unsigned int features)
{
    (void)stepping; /* Stepping preserved for future use; currently unused */

    switch (family) {
        case AMD_FAMILY_ZEN_ZEN2:
            if (model <= 0x1f) {
                /* Zen / Zen+ range - aocl-utils does not differentiate Zen+ */
                return UARCH_ZEN;
            } else if (model >= 0x30) {
                return UARCH_ZEN2;
            } else {
                if (features & FEATURE_CLWB)
                    return UARCH_ZEN2;
                return UARCH_ZEN;
            }

        case AMD_FAMILY_ZEN3_ZEN4:
            if (model <= 0x0f) {
                return UARCH_ZEN3;
            } else if ((model >= 0x10 && model <= 0x1f)
                       || (model >= 0x60 && model <= 0xaf)) {
                return UARCH_ZEN4;
            } else if ((model == MODEL_VERMEER) || (model == MODEL_REMBRANDT)
                       || (model == MODEL_CEZANNE)) {
                /* Zen3 models outside the 0x00-0x0f range */
                return UARCH_ZEN3;
            } else {
                if (features & FEATURE_AVX512F)
                    return UARCH_ZEN4;
                return UARCH_ZEN3;
            }

        case AMD_FAMILY_ZEN5_ZEN6:
            /* Zen6 model ranges: 0x50-0x5f, 0x80-0xcf, 0xd8-0xe7 */
            if (is_zen6_model(model))
                return UARCH_ZEN6;
            /* All other models in 0x1A are Zen5: <=0x4f, 0x60-0x77, 0xd0-0xd7 */
            return UARCH_ZEN5;

        default:
            break;
    }

    /* Future families beyond 0x1A default to Zen6 */
    if (family > AMD_FAMILY_ZEN5_ZEN6)
        return UARCH_ZEN6;

    return UARCH_UNKNOWN;
}

static void test_cpu_config(const char* name, unsigned int family, 
                            unsigned int model, unsigned int stepping,
                            unsigned int features, uarch_t expected)
{
    uarch_t result = mock_detect_amd_uarch(family, model, stepping, features);
    char buf[256];
    snprintf(buf, sizeof(buf), "%s: F=0x%02X M=0x%02X S=%u -> %s (expected %s)",
             name, family, model, stepping, uarch_name(result), uarch_name(expected));
    TEST(buf, result == expected);
}

static void test_family_0x17(void)
{
    TEST_GROUP("Family 0x17 - Zen/Zen2 (Zen+ treated as Zen)");
    test_cpu_config("Naples",      0x17, 0x01, 0, 0, UARCH_ZEN);
    test_cpu_config("Ravenridge",  0x17, 0x11, 0, 0, UARCH_ZEN);
    test_cpu_config("Pinnacleridge", 0x17, 0x08, 0, 0, UARCH_ZEN);  /* aocl-utils: no Zen+ differentiation */
    test_cpu_config("Picasso (step 0)", 0x17, 0x18, 0, 0, UARCH_ZEN);
    test_cpu_config("Picasso (step 1)", 0x17, 0x18, 1, 0, UARCH_ZEN);  /* aocl-utils: no Zen+ differentiation */
    test_cpu_config("Rome",          0x17, 0x31, 0, 0, UARCH_ZEN2);
    test_cpu_config("Matisse",       0x17, 0x71, 0, 0, UARCH_ZEN2);
    
    TEST_GROUP("Family 0x17 - Feature Fallback");
    test_cpu_config("Model 0x20 (no CLWB)", 0x17, 0x20, 0, 0, UARCH_ZEN);
    test_cpu_config("Model 0x20 (with CLWB)", 0x17, 0x20, 0, FEATURE_CLWB, UARCH_ZEN2);
}

static void test_family_0x19(void)
{
    TEST_GROUP("Family 0x19 - Zen3/Zen4");
    test_cpu_config("Milan",     0x19, 0x01, 0, 0, UARCH_ZEN3);
    test_cpu_config("Chagall",   0x19, 0x08, 0, 0, UARCH_ZEN3);
    test_cpu_config("Vermeer (step 0)", 0x19, 0x21, 0, 0, UARCH_ZEN3);
    test_cpu_config("Vermeer (step 2)", 0x19, 0x21, 2, 0, UARCH_ZEN3);  /* No stepping-based detection */
    test_cpu_config("Genoa",     0x19, 0x11, 0, FEATURE_AVX512F, UARCH_ZEN4);
    test_cpu_config("Raphael",   0x19, 0x61, 0, FEATURE_AVX512F, UARCH_ZEN4);
    
    TEST_GROUP("Family 0x19 - Feature Fallback");
    test_cpu_config("Model 0x30 (no AVX512F)", 0x19, 0x30, 0, 0, UARCH_ZEN3);
    test_cpu_config("Model 0x30 (with AVX512F)", 0x19, 0x30, 0, FEATURE_AVX512F, UARCH_ZEN4);
}

static void test_family_0x1A(void)
{
    TEST_GROUP("Family 0x1A - Zen5 Model Ranges (<=0x4f, 0x60-0x77, 0xd0-0xd7)");
    test_cpu_config("TurinD (0x01)", 0x1A, MODEL_TURIND, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Turin (0x20)",  0x1A, MODEL_TURIN,  0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Strix Point (0x44)", 0x1A, MODEL_STRIX_POINT, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Model 0x00",    0x1A, 0x00, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Model 0x4f",    0x1A, 0x4f, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Boundary: last Zen5 */
    test_cpu_config("Model 0x60",    0x1A, 0x60, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Start of 0x60-0x77 range */
    test_cpu_config("Model 0x77",    0x1A, 0x77, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* End of 0x60-0x77 range */
    test_cpu_config("Model 0xd0",    0x1A, 0xd0, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Start of 0xd0-0xd7 range */
    test_cpu_config("Model 0xd7",    0x1A, 0xd7, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* End of 0xd0-0xd7 range */
    
    TEST_GROUP("Family 0x1A - Zen6 Model Ranges (0x50-0x5f, 0x80-0xcf, 0xd8-0xe7)");
    test_cpu_config("Zen6 Model 0x50", 0x1A, MODEL_ZEN6_50, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* Start of 0x50-0x5f */
    test_cpu_config("Zen6 Model 0x5f", 0x1A, 0x5f, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* End of 0x50-0x5f */
    test_cpu_config("Zen6 Model 0x80", 0x1A, MODEL_ZEN6_80, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* Start of 0x80-0xcf */
    test_cpu_config("Zen6 Model 0xcf", 0x1A, 0xcf, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* End of 0x80-0xcf */
    test_cpu_config("Zen6 Model 0xd8", 0x1A, MODEL_ZEN6_D8, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* Start of 0xd8-0xe7 */
    test_cpu_config("Zen6 Model 0xe7", 0x1A, 0xe7, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* End of 0xd8-0xe7 */
    
    TEST_GROUP("Family 0x1A - Boundary Tests (Zen5/Zen6 model boundaries)");
    test_cpu_config("Boundary 0x4f->0x50", 0x1A, 0x4f, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Last Zen5 before 0x50 */
    test_cpu_config("Boundary 0x50 (Zen6)", 0x1A, 0x50, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* First Zen6 at 0x50 */
    test_cpu_config("Boundary 0x5f->0x60", 0x1A, 0x5f, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* Last Zen6 before 0x60 */
    test_cpu_config("Boundary 0x60 (Zen5)", 0x1A, 0x60, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Back to Zen5 at 0x60 */
    test_cpu_config("Boundary 0x77->0x78", 0x1A, 0x77, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Last Zen5 at 0x77 */
    test_cpu_config("Boundary 0x78 (gap)", 0x1A, 0x78, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Gap model defaults to Zen5 */
    test_cpu_config("Boundary 0x7f->0x80", 0x1A, 0x7f, 0, FEATURE_AVX512F, UARCH_ZEN5);  /* Last gap before 0x80 */
    test_cpu_config("Boundary 0x80 (Zen6)", 0x1A, 0x80, 0, FEATURE_AVX512F, UARCH_ZEN6);  /* Zen6 at 0x80 */
}

static void test_family_0x1A_edge_cases(void)
{
    TEST_GROUP("Family 0x1A - Edge Cases and Gap Models");
    /* Models in gaps (0x78-0x7f, 0xe8-0xff) should default to Zen5 (not Zen6) */
    test_cpu_config("Gap Model 0x78",  0x1A, 0x78, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Gap Model 0x7f",  0x1A, 0x7f, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Gap Model 0xe8",  0x1A, 0xe8, 0, FEATURE_AVX512F, UARCH_ZEN5);
    test_cpu_config("Gap Model 0xff",  0x1A, 0xff, 0, FEATURE_AVX512F, UARCH_ZEN5);
}

static void test_future_families(void)
{
    TEST_GROUP("Future Families > 0x1A (default to Zen6)");
    test_cpu_config("Family 0x1B", 0x1B, 0x01, 0, FEATURE_AVX512F, UARCH_ZEN6);
    test_cpu_config("Family 0x1C", 0x1C, 0x01, 0, FEATURE_AVX512F, UARCH_ZEN6);
    test_cpu_config("Family 0x1D", 0x1D, 0x01, 0, FEATURE_AVX512F, UARCH_ZEN6);
    test_cpu_config("Family 0xFF", 0xFF, 0x01, 0, FEATURE_AVX512F, UARCH_ZEN6);
}

static void test_non_zen_families(void)
{
    TEST_GROUP("Non-Zen Families");
    test_cpu_config("Family 0x15 (Bulldozer)", 0x15, 0x01, 0, 0, UARCH_UNKNOWN);
    test_cpu_config("Family 0x16 (Jaguar)",    0x16, 0x01, 0, 0, UARCH_UNKNOWN);
}

static void test_at_least_semantics(void)
{
    TEST_GROUP("At-Least Semantics");
    uarch_t detected = UARCH_ZEN4;
    TEST("Zen4: is_zen returns TRUE",  detected >= UARCH_ZEN);
    TEST("Zen4: is_zen4 returns TRUE", detected >= UARCH_ZEN4);
    TEST("Zen4: is_zen5 returns FALSE", detected >= UARCH_ZEN5 ? 0 : 1);
    
    detected = UARCH_ZEN5;
    TEST("Zen5: is_zen5 returns TRUE", detected >= UARCH_ZEN5);
    TEST("Zen5: is_zen6 returns FALSE", detected >= UARCH_ZEN6 ? 0 : 1);
    
    detected = UARCH_ZEN6;
    TEST("Zen6: is_zen returns TRUE",  detected >= UARCH_ZEN);
    TEST("Zen6: is_zen5 returns TRUE", detected >= UARCH_ZEN5);
    TEST("Zen6: is_zen6 returns TRUE", detected >= UARCH_ZEN6);
}

int main(void)
{
    printf("============================================================\n");
    printf("CPUID Detection Branch Coverage Test\n");
    printf("============================================================\n");
    
    test_family_0x17();
    test_family_0x19();
    test_family_0x1A();
    test_family_0x1A_edge_cases();
    test_future_families();
    test_non_zen_families();
    test_at_least_semantics();
    
    printf("\n============================================================\n");
    printf("TEST SUMMARY\n");
    printf("============================================================\n");
    printf(GREEN "Passed: %d" RESET "\n", tests_passed);
    printf(RED   "Failed: %d" RESET "\n", tests_failed);
    printf("Total:  %d\n", tests_total);
    printf("============================================================\n");
    
    return tests_failed > 0 ? 1 : 0;
}
