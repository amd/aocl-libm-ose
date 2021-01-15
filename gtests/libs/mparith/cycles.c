#include "cycles.h"
#include "mparith_c.h"

int
rdist(int a, int b) {
    int d;

    if (a < 0) { a = -a; }

    if (a < -2147483647) { a = 0; }

    if (b < 0) { b = -b; }

    if (b < -2147483647) { b = 0; }

    if (b > a) { d = b - a; }
    else      { d = a - b; }

    srand((int)myrand());
    return a + (rand() % (d + 1));
}

/* Returns a value logarithmically distributed in [a,b] */
REAL
logdist(int i, int n, REAL a, REAL b, REAL logba) {
    REAL x, tx;

    if (i <= 0)
    { tx = 0.0; }
    else if (i >= n - 1)
    { tx = 1.0; }
    else
    { tx = (REAL)((i + myrand() - 0.5) / (n - 1)); }

    /* tx is uniformly distributed in [0,1] */
    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0))) {
        /* x is logarithmically distributed in [a,b] */
        if (a < 0.0) {
            /* Negative interval: negate b, tx and x so that
               the numbers are distributed in a similar fashion to
               the equivalent positive interval. Doesn't really
               make much difference */
            if (tx == 0.0)
            { x = b; }
            else if (tx == 1.0)
            { x = a; }
            else
            { x = b * myexp(-tx * logba); }
        } else {
            if (tx == 0.0)
            { x = a; }
            else if (tx == 1.0)
            { x = b; }
            else
            { x = a * myexp(tx * logba); }
        }
    } else {
        /* x is uniformly distributed in [a,b] */
        if (tx == 0.0)
        { x = a; }
        else if (tx == 1.0)
        { x = b; }
        else
        { x = a + tx * (b - a); }
    }

    if (x < a)
    { x = a; }
    else if (x > b)
    { x = b; }

    return x;
}

/* Returns a value logarithmically distributed in [a,b].
   Same function as logdist, but uses an independent
   stream of random numbers. */
REAL
logdist2(int i, int n, REAL a, REAL b, REAL logba) {
    REAL x, tx;

    if (i <= 0)
    { tx = 0.0; }
    else if (i >= n - 1)
    { tx = 1.0; }
    else
    { tx = (REAL)((i + myrand2() - 0.5) / (n - 1)); }

    /* tx is uniformly distributed in [0,1] */
    if (a != 0.0 && b != 0.0 && ((a < 0.0) == (b < 0.0))) {
        /* x is logarithmically distributed in [a,b] */
        if (a < 0.0) {
            /* Negative interval: negate b, tx and x so that
               the numbers are distributed in a similar fashion to
               the equivalent positive interval. Doesn't really
               make much difference */
            if (tx == 0.0)
            { x = b; }
            else if (tx == 1.0)
            { x = a; }
            else
            { x = b * myexp(-tx * logba); }
        } else {
            if (tx == 0.0)
            { x = a; }
            else if (tx == 1.0)
            { x = b; }
            else
            { x = a * myexp(tx * logba); }
        }
    } else {
        /* x is uniformly distributed in [a,b] */
        if (tx == 0.0)
        { x = a; }
        else if (tx == 1.0)
        { x = b; }
        else
        { x = a + tx * (b - a); }
    }

    if (x < a)
    { x = a; }
    else if (x > b)
    { x = b; }

    return x;
}


/* Compute the overhead of the timing function */
REAL
computeTimerOverhead(void) {
    REAL overhead;
    MY_ULARGE_INTEGER t1, t2;
    int i;
#if 1
    overhead = 0.0;

    for (i = 0; i < 1000; i++) {
        t1 = RDTSC();
        t2 = RDTSC();
        overhead += mylog(DIFTSC(t1, t2));
    }

    overhead = myexp(overhead / 1000);
    return overhead;
#else
    REAL dt;
    createInfinity(0, &overhead);

    for (i = 0; i < 1000; i++) {
        t1 = RDTSC();
        t2 = RDTSC();
        dt = DIFTSC(t1, t2);
        overhead = mymin(overhead, dt);
    }

    return overhead;
#endif
}

#ifdef TEST
#include <stdio.h>
#include <math.h>

double
mylog(double x) {
    return log(x);
}
double
myexp(double x) {
    return exp(x);
}
int
main(int argc, char *argv[]) {
    MY_ULARGE_INTEGER t1, t2;
    int i, cycles;
    REAL x, y, r;
    const int spins = 1000000;
    x = 3.14;
    t1 = RDTSC();

    for (i = 0; i < spins; i++)
#ifdef DOUBLE
        y = exp(x);

#else
        y = expf(x);
#endif
    t2 = RDTSC();
    r = DIFTSC(t1, t2);
    r = r / spins;
    cycles = (int)(r);
    printf("That took %d cycles\n", cycles);
    return 0;
}
#endif
