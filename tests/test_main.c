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

#include <debug.h>
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
    {"verbose", 'v', "NUM", 0, "Increased verbosity value [1..5]"},
    {"range", 'r', "[start,end]", 0, "Range to populate input "
    "Accepts multiple ranges (upto 3)"},

    {0},
};

/*
 * parse_range : parse the following
 * --range=start,end,<type> - start,end, type = simple, linear, random
 *                                              (upto 3 significant letters)
 * --range=-234.234,23.23   - start,end (default simple population)
 * --range=234,243,rand     - random filling
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

    if ((range->stop - range->start) == 0.0)
        return -1;


    if (len) {
        range->type = LIBM_INPUT_RANGE_SIMPLE;

        /* We just compare first 3 letters to be safer */
        if (strncmp(arg, "random", 4) == 0)
            range->type = LIBM_INPUT_RANGE_RANDOM;
        else if (strncmp(arg, "linear", 3) == 0)
            range->type = LIBM_INPUT_RANGE_LINEAR;
        else if (strncmp(arg, "simple", 3)){
            /* Already taken care of */
        }

    }

    LIBM_TEST_DPRINTF(DBG1, "start:%LG stop:%Lg type:%d\n",
                      range->start, range->stop, range->type);

    return 0;
}

static error_t __enable_test_variants(const char *vrnt, struct libm_test_conf *conf)
{
    uint32_t *variant = &conf->variants;
    int n = strnlen(vrnt, 10);

    if (n == 0) {
        LIBM_TEST_DPRINTF(INFO, "No test variant runnable\n");
        return -1;
    }
    uint8_t all = strncmp(vrnt, "all", 3) == 0;
    uint8_t scalar = strncmp(vrnt, "scalar", 6) == 0;
    uint8_t vector = strncmp(vrnt, "vector", 6) == 0;

    if (strncmp(vrnt, "s1f", 3) == 0 || scalar || all) {
        *variant |= LIBM_FUNC_S_S;
    }
    if (strncmp(vrnt, "s1d", 3) == 0 || scalar || all) {
        *variant |= LIBM_FUNC_S_D;
    }
    if (strncmp(vrnt, "v2s", 3) == 0 || vector || all) {
        *variant |= LIBM_FUNC_V2S;
    }
    if (strncmp(vrnt, "v4s", 3) == 0 || vector || all) {
        *variant |= LIBM_FUNC_V4S;
    }
    if (strncmp(vrnt, "v8s", 3) == 0 || vector || all) {
        *variant |= LIBM_FUNC_V8S;
    }
    if (strncmp(vrnt, "v2d", 3) == 0 || vector || all) {
        *variant |= LIBM_FUNC_V2D;
    }
    if (strncmp(vrnt, "v4d", 3) == 0 || vector || all) {
        *variant |= LIBM_FUNC_V4D;
    }

    return 0;
}

uint32_t dbg_bits = DBG_DEFAULT;

static error_t __enable_tests_type(const char *type, struct libm_test_conf *conf)
{
    uint32_t *tp = &conf->test_types;
    int n = strnlen(type, 10);

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
        if (ret)
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
            if (ret)
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
        int32_t bit = strtol(arg, NULL, 0);
        int high = LIBM_TEST_DBG_VERBOSE3 - LIBM_TEST_DBG_INFO;
        /* adjust between 0-5 */
        bit = (bit < 0)? 0: bit;
        bit = (bit > high)? high: bit;
        dbg_bits = (1 << (bit + LIBM_TEST_DBG_INFO + 1)) - 1;
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
        else
            LIBM_TEST_DPRINTF(WARN, "Range is not valid input, ignoring\n");
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

        printf("%-12s %-12s %-12s %-12d %-12d %-12d %-12d",
               test_get_name(test), test_get_test_type(test),
               test_get_input_type(test),
               result->ntests, result->npass, result->nfail, result->nignored);

        if (test->test_type == TEST_TYPE_ACCU) {
            printf(" %-12s %-12g", " ", test->max_ulp_err);
        } else if (test->test_type == TEST_TYPE_PERF) {
            printf(" %-12g", result->mops);
        } else {
            printf(" %-12s", " ");
        }

        printf("\n");
    }

    printf("%s\n", &equal[0]);
}

static struct list_head test_list;

#if 0
static int __libm_test_get_input_type(struct libm_test *test)
{
    if (test->variant == LIBM_FUNC_S_S ||
        test->variant == LIBM_FUNC_V2S ||
        test->variant == LIBM_FUNC_V4S ||
        test->variant == LIBM_FUNC_V8S)
        return 1;

    if (test->variant == LIBM_FUNC_S_D ||
        test->variant == LIBM_FUNC_V2D ||
        test->variant == LIBM_FUNC_V4D)
        return 2;

    return 0;
}
#endif

static int libm_test_populate_expected(struct libm_test *test)
{
    for (uint32_t i = 0; i < test->test_data.nelem; i++) {
            //test->ops.libm_func_callback(test, i);
    }

    return 0;
}

static inline int is_libm_func_exists(struct libm_test *test)
{
	return (test->ops.callbacks.s1s != NULL) || (test->ops.callbacks.s1d != NULL);
}

static int libm_test_run_one(struct libm_test *test, struct libm_test_result *result)
{
    LIBM_TEST_DPRINTF(VERBOSE3,
                      "Starting Test: %s type:%s input:%s\n",
                      test_get_name(test), test_get_test_type(test),
                      test_get_input_type(test));

    /*
     * Supposed to allocate all buffers,
     * create any validation output
     */
    if (test->ops.setup)
        test->ops.setup(test);

    if (!test->test_data.input1 ||
        !test->test_data.output) {
        printf("Test:%s type:%s input:%s dont have data set\n", test_get_name(test),
               test_get_test_type(test), test_get_input_type(test));
        /*
         * TODO: delete self before going out, so that results
         * dont include this entry
         */
        goto out;
    }

    if (test->ops.run)
        test->ops.run(test);

    if (is_libm_func_exists(test))
        libm_test_populate_expected(test);

    if (test->ops.verify)
        test->ops.verify(test, result);

    if (test->ops.cleanup)
        test->ops.cleanup(test);

    LIBM_TEST_DPRINTF(VERBOSE3,
                      "Done Test: %s type:%s input:%s\n",
                      test_get_name(test), test_get_test_type(test),
                      test_get_input_type(test));

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

    LIBM_TEST_DPRINTF(VERBOSE3, "Request Add Test:%s type:%s input:%s\n",
                      test_get_name(test), test_get_test_type(test),
                      test_get_input_type(test));

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
               test_get_name(test), test_get_test_type(test),
               test_get_input_type(test));
        goto out;
    }

    if (test->ulp_threshold == 0.0 || test->ulp_threshold > 10.0) {
        test->ulp_threshold = 0.5;
    }

    bzero(&test->result, sizeof(test->result));
    /*
     * Only for performance tests we dont need a 'verify' method,
     * other tests should mandatory have it
     */
    if (test->conf->test_types != TEST_TYPE_PERF && !test->ops.verify) {
        printf("Test:%s type:%s input:%s dont have a verify method\n",
               test_get_name(test), test_get_test_type(test),
               test_get_input_type(test));
    }

    LIBM_TEST_DPRINTF(VERBOSE3, "Adding test:%s type:%s input:%s\n",
                      test_get_name(test), test_get_test_type(test),
                      test_get_input_type(test));
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

