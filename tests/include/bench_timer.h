/*
 * Copyright (C) 2018, Advanced Micro Devices. All rights reserved.
 *
 * Author: Prem Mallappa <pmallapp@amd.com>
 */

/*
 * Timing calculations ....
 */

#ifndef __BENCH_TIMER_H__
#define __BENCH_TIMER_H__

#include <time.h>

#define BENCH_CONST_1_MILLION             1e6
#define BENCH_CONST_NANOSEC_PER_SEC       1e9

struct timer {
    struct timespec t_start, t_stop;
};
typedef struct timer bench_timer_t;

static inline void timer_start(struct timer *tm)
{
    struct timespec *t=&tm->t_start;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
}

static inline void timer_stop(struct timer *tm)
{
    struct timespec *t=&tm->t_stop;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
}

/*
 * Returns total time spent in seconds
 */
static inline double timer_span(struct timer *tm)
{
    struct timespec *start=&tm->t_start, *stop=&tm->t_stop;
    return (double)(stop->tv_sec - start->tv_sec) +
        (stop->tv_nsec - start->tv_nsec) / (double)BENCH_CONST_NANOSEC_PER_SEC;
}

static inline double timer_add(struct timer *tm1, struct timer *tm2)
{
    return timer_span(tm1) + timer_span(tm2);
}

static inline double timer_sub(struct timer *tm1, struct timer *tm2)
{
    return timer_span(tm1) - timer_span(tm2);
}

/*
 * convert seconds to Million ops per second
 * s is the time spent in seconds
 * n is the number of operations performed
 */
static inline double sec2mps(double s, uint64_t n)
{
    return (double)n / (s * BENCH_CONST_1_MILLION);
}

#endif /* __BENCH_TIMER_H__ */
