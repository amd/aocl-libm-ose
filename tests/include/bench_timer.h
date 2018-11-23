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

static inline double timer_span(struct timer *tm)
{
    struct timespec *start=&tm->t_start, *stop=&tm->t_stop;
    return (double)(stop->tv_sec - start->tv_sec) +
        (double)(stop->tv_nsec - start->tv_nsec) / 1e9;
}

static inline double sec2mps(double s, long n)
{
    return (double)n / (s * 1e6);
}

#endif /* __BENCH_TIMER_H__ */
