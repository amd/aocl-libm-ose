/* Copyright (C) 2018, Advanced Micro Devices. All rights reserved
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 * Test framework for AMD math library
 */

#include <stdio.h>
#include <stdlib.h>                             /* for strtold() */
#include <argp.h>                               /* for argp */
#include <math.h>                               /* For all math functions */
#include <dlfcn.h>                              /* for dlopen and dlsym() */
#include <string.h>                             /* for strlen() and strtok() */

//#include "bench.h"
//#include "util.h"
#include <libm_tests.h>
#include <libm_test_macros.h>

#ifdef __LINUX
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
#endif

#define THOUSAND (1000)
#define MILLION (THOUSAND * THOUSAND)

#define NITER (100 * THOUSAND)

#define NELEM 5000                     /* Some odd number */

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#define PASTE2(a, b) a##b
#define BUILD_TEST_DOC(b) "AMD LIBM Test for " STRINGIFY(b)

extern char doc[];

static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    {"loop", 'l', "NUM", 0, "Number of iterations to perform"},
    {"count", 'c', "NUM", 0, "Number of input size, for accuracy tests this number of distinct values in each range"},
    {"input", 'i', "VARIANT", 0,
     "Comma separated list of: "
     "scalar, vector OR all(default) "
     "scalar - s1f, s1d "
     "vector - v2f, v4f, v8f, v2d, v4d"},
    {"coremask", 'm', "[bitmask]", 0, "Core bitmask to run on"},
    {"type", 't', "TYPES", 0,
     "Comma separated list of: "
     "performance, accuracy, special, corner "
     "OR all(default) "
     "first four characters necessary"},
    {"verbose", 'v', "NUM", 0, "Increased verbosity value <1-3>"},
    {"range", 'r', "[start,end]", 0, "Range to populate input "
    "Accepts multiple ranges (upto 3)"},

    {0},
};

/*
 * parse_range : parse the following
 * --range=-234.234,0.23    - -ve start,end,inc
 * --range=234,243,1.0      - -ve start, end, inc
 * --range=0.2343,0.0000234 - start,inc
 */
int parse_range(char *arg,
                struct libm_test_input_range *range)
{
    const char *test = arg;
    unsigned len = strlen(arg);

    if ((test = strtok(arg, ",;:")) != NULL) {
        range->start = strtold(test, NULL);
        arg += strlen(test) + 1; // skip the '\0', null char
        len -= strlen(test) + 1;
    }

    if ((test = strtok(arg, ",;:")) != NULL) {
        range->stop = strtold(test, NULL);
        arg += strlen(test) + 1; // skip the '\0', null char
        len -= strlen(test) + 1;
    }

    if (len) {
        range->inc = strtold(arg, NULL);
    } else {
        range->inc = range->stop;
        range->stop = 0;
    }

    printf("start:%LG stop:%Lg inc:%Lg\n", range->start, range->stop, range->inc);

    return 0;
}

static error_t __enable_test_variants(const char *vrnt, struct libm_test_conf *conf)
{
    uint32_t *variant = &conf->variants;
    int n = strnlen(vrnt, 10);

    if (n == 0) {
        printf("No test variant runnable\n");
        return -1;
    }
    uint8_t all = strncmp(vrnt, "all", 3) == 0;

    if (strncmp(vrnt, "s", 2) == 0 || all) {
        *variant |= LIBM_FUNC_S_S;
    }
    if (strncmp(vrnt, "d", 2) == 0 || all) {
        *variant |= LIBM_FUNC_S_D;
    }
    if (strncmp(vrnt, "v2s", 3) == 0 || all) {
        *variant |= LIBM_FUNC_V2S;
    }
    if (strncmp(vrnt, "v4s", 3) == 0 || all) {
        *variant |= LIBM_FUNC_V4S;
    }
    if (strncmp(vrnt, "v8s", 3) == 0 || all) {
        *variant |= LIBM_FUNC_V8S;
    }
    if (strncmp(vrnt, "v2d", 3) == 0 || all) {
        *variant |= LIBM_FUNC_V2D;
    }
    if (strncmp(vrnt, "v4d", 3) == 0 || all) {
        *variant |= LIBM_FUNC_V4D;
    }

    return 0;
}

uint32_t dbg_bits = DBG_VERBOSE1;

static error_t __enable_tests_type(const char *type, struct libm_test_conf *conf)
{
    uint32_t *tp = &conf->test_types;
    int n = strnlen(type, 10);

    /* Reset all, we have come here means no default */
    conf->test_types = 0;

    if (n == 0) {
        printf("No test type runnable\n");
        return -1;
    }
    uint8_t all = strncmp(type, "all", 3) == 0;

    if ((strncmp(type, "accu", 4) == 0) || all)
        *tp |= TEST_TYPE_ACCU;

    if ((strncmp(type, "perf", 4) == 0) || all)
        *tp |= TEST_TYPE_PERF;

    if ((strncmp(type, "spec", 4) == 0) || all)
        *tp |= TEST_TYPE_SPECIAL;

    if ((strncmp(type, "corn", 4) == 0) || all)
        *tp |= TEST_TYPE_CORNER;

    return 0;
}

static error_t parse_test_types(char *arg, struct libm_test_conf *conf)
{
    const char *test = arg;
    unsigned len = strlen(arg) + 1;

    /* Reset all, we have come here means no default */
    conf->test_types = 0;

    /*
     * Test variants can be specified with
     * scalar (both single and double)
     * accu, perf, special, corner
     */
    while(len && (test = strtok(arg, ",;:")) != NULL) {
        int ret = __enable_tests_type(test, conf);
        if (!ret)
            return -1;
        arg += strlen(test) + 1; // skip the '\0', null char
        len -= strlen(test) + 1;
    }

    if (len)
        return __enable_tests_type(arg, conf);

    return 0;
}


static error_t parse_variants(char *arg, struct libm_test_conf *conf)
{
        const char *test = arg;
        unsigned len = strlen(arg) + 1;         /* include the null, for now */

        /* Reset all, we have come here means no default */
        conf->variants = 0;

        /* Test variants can be specified with
         * scalar (both single and double)
         * s, d  - single and double precision respectively
         * v4s, v8s - 4(and 8)-vector single precision
         * v2d, v4d - 2(and 4)-vector double precision
         */
        while(len && (test = strtok(arg, ",;:")) != NULL) {
            int ret = __enable_test_variants(test, conf);
            if (!ret)
                return -1;
            arg += strlen(test) + 1; // skip the '\0', null char
            len -= strlen(test) + 1;
        }

        if (len)
            return __enable_test_variants(arg, conf);

        return 0;
}

static error_t parse_opts(int key, char *arg, struct argp_state *state)
{
    struct libm_test_conf *conf = state->input;
    static int ridx = 0;
    struct libm_test_input_range *range;
    int    ret = 0;

    switch (key)
    {
    case 'l':
        conf->niter = strtol(arg, NULL, 0);
        break;
    case 'm':
        conf->coremask = strtol(arg, NULL, 0);
        break;
    case 'c':
        conf->nelem = strtol(arg, NULL, 0);
        break;
    case 'i':
        parse_variants(arg, conf);
        break;
    case 'v': {
        int bit = strtol(arg, NULL, 0);
        bit = (bit < 0)? 0: bit;                /* adjust between 0-3 */
        bit = (bit > 3)? 3: bit;
        dbg_bits = 1 << (LIBM_TEST_DBG_INFO + bit);
    }
        break;
    case 'r':
        if (ridx >= 3) {
            printf("Too many ranges specified, ignoring\n");
            return 0;
        }
        range = &conf->inp_range[ridx];
        ret = parse_range(arg, range);
        if (ret == 0)
            ridx++;
        break;
    case 't':
        ret = parse_test_types(arg, conf);
        break;
    case ARGP_KEY_ARG:
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;

    }
    return 0;
}

static struct argp argp = {options, parse_opts, args_doc, doc, 0, 0, 0};

static const char *libm_test_variant_str(uint32_t variant)
{
    switch(variant) {
    case LIBM_FUNC_S_S:
        return "s1s";
    case LIBM_FUNC_S_D:
        return "s1d";
    case LIBM_FUNC_V2S:
        return "v2s";
    case LIBM_FUNC_V4S:
        return "v4s";
    case LIBM_FUNC_V8S:
        return "v8s";
    case LIBM_FUNC_V2D:
        return "v2d";
    case LIBM_FUNC_V4D:
        return "v4d";
    default:
        break;
    }

    return "unknown";
}

static void libm_test_print_report(struct list_head *test_list)
{
    struct list_head *pos;
    static const char equal[100] = {[0 ... 98] = '=', 0};

    printf("%s\n", &equal[0]);

    printf("%-12s %-12s %-12s %-12s %-12s %-12s %-12s %-12s\n",
           "TEST", "TYPE", "DATATYPE", "No.Tests", "Passed",
           "Failed", "Ignored", "MOPS");

    printf("%s\n", &equal[0]);

    list_for_each(pos, test_list) {
        struct libm_test *test = list_entry(pos, struct libm_test, list);
        struct libm_test_result *result = &test->result;

        printf("%-12s %-12s %-12s %-12d %-12d %-12d %-12d %-12f %-12f\n",
               test->name, test->type_name, libm_test_variant_str(test->variant),
               result->ntests, result->npass, result->nfail, result->nignored,
               result->mops, test->ulp_err);
    }

    printf("%s\n", &equal[0]);
}

static struct list_head test_list;

static int libm_test_run_one(struct libm_test *test, struct libm_test_result *result)
{
    printf("Starting Test: %s %s\n", test->name, test->type_name);

    /*
     * Supposed to allocate all buffers,
     * create any validation output
     */
    if (test->ops.setup)
        test->ops.setup(test);

    if (!test->test_data) {
        printf("Test:%s type:%s dont have data set\n", test->name,
               test->type_name);
        /*
         * TODO: delete self before going out, so that results
         * dont include this entry
         */
        goto out;
    }

    if (test->ops.run)
        test->ops.run(test);

    if (test->ops.verify)
        test->ops.verify(test, result);

    if (test->ops.cleanup)
        test->ops.cleanup(test);

    printf("Done Test:%s %s\n", test->name, test->type_name);
    return 0;

out:
    return -1;
}

static int libm_test_run(struct list_head *test_list)
{
    struct list_head *pos;

    list_for_each(pos, test_list) {
        struct libm_test *test = list_entry(pos, struct libm_test, list);
        struct libm_test_result *result = &test->result;

        libm_test_run_one(test, result);
        memset(&result, 0, sizeof(result));
    }

    return 0;

}

int libm_test_register(struct libm_test *test)
{
    if (!test) {
        printf("Test:%s type:%s pointer not valid\n", test->name,
               test->type_name);
        goto out;
    }

    if (test->nargs < 1 || test->nargs > 3) {
        printf("Test:%s type:%s no of args: %d is invalid\n", test->name,
               test->type_name, test->nargs);
        goto out;
    }

    if (!test->ops.run) {
        printf("Test:%s type:%s dont have a 'run' method \n", test->name,
               test->type_name);
        goto out;
    }

    if ((test->variant & test->conf->variants) == 0) {
        printf("Test:%s type:%s variant:%s not passed by user, skipping",
               test->name, test->type_name, libm_test_variant_str(test->variant));
        goto out;
    }

    bzero(&test->result, sizeof(test->result));
    /*
     * Only for performance tests we dont need a 'verify' method,
     * other tests should mandatory have it
     */
    if (test->conf->test_types != TEST_TYPE_PERF && !test->ops.verify) {
        printf("Test:%s type:%s dont have a verify method\n", test->name,
               test->type_name);
    }

    list_add(&test->list, &test_list);

    return 0;

 out:
    return -1;
}

#define LIBM_FUNC_ALL (LIBM_FUNC_S_S | LIBM_FUNC_S_D |                 \
                       LIBM_FUNC_V2S | LIBM_FUNC_V4S | LIBM_FUNC_V8S | \
                       LIBM_FUNC_V2D | LIBM_FUNC_V4D)

#define LIBM_TEST_TYPES_ALL (TEST_TYPE_ACCU | TEST_TYPE_PERF |          \
                             TEST_TYPE_SPECIAL | TEST_TYPE_CORNER)

int main(int argc, char *argv[])
{
    int ret;
    struct libm_test_conf conf = {
        .niter   = NITER,
        .nelem   = NELEM,
        .variants = LIBM_FUNC_ALL,
        .test_types = LIBM_TEST_TYPES_ALL,
    };

    ret = argp_parse(&argp, argc, argv, 0, 0, &conf);
    if (ret != 0)
        goto out;

    INIT_LIST_HEAD(&test_list);

    libm_test_init(&conf);

    libm_test_run(&test_list);

    libm_test_print_report(&test_list);

    return EXIT_SUCCESS;

 out:
    return EXIT_FAILURE;
}

