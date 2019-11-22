#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef WIN64

#include <windows.h>

class Timer
{
    LARGE_INTEGER start, stop, freq;

public:
    Timer()
    {
        QueryPerformanceFrequency( &freq );
    }

    void Start()
    {
        QueryPerformanceCounter(&start);
    }

    double Sample()
    {
        QueryPerformanceCounter(&stop); 
        double time = ((double)(stop.QuadPart-start.QuadPart)) / ((double)freq.QuadPart);

        return time;
    }    
};

#else

#include <sys/time.h>

class Timer
{
    timeval tval;
    double start, stop;

public:
    void Start()
    {
        gettimeofday(&tval, NULL);
        start = (double)(tval.tv_sec * 1E6 + tval.tv_usec);
    }

    double Sample()
    {
        gettimeofday(&tval, NULL);
        stop = (double)(tval.tv_sec * 1E6 + tval.tv_usec);
        double time = (stop - start) * 1E-6;

        return time;
    }    
};

#endif // WIN64

#endif // __TIMER_H__

