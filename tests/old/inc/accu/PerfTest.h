#ifndef __PERF_TEST_H__
#define __PERF_TEST_H__

#include <cassert>

#include "DataTypes.h"
#include "Buffer.h"
#include "Timer.h"
#include "Constants.h"

#if   defined(ACML_LIBM)
#include "../CompareVCandICC/ACML/libm_acml.h"
#elif defined(NAG_LIBM)
#include "../CompareVCandICC/NAG/libm_amd.h"
#else
#include <math.h>
#endif

#define REPEAT_COUNT 10

template <typename T, typename TI>
class PerfTest
{
protected:
    union TU
    {
        TI ti;
        T t;
    };

    const size_t LENGTH;
    Timer t;
    double min_time;
    Buffer<T> buf_dst;
    T *ptr_dst;

    PerfTest() : LENGTH(1024 * 1024)
    {}

    inline void InitDst()
    {
        buf_dst.Init(LENGTH);
        ptr_dst = buf_dst.Data();
    }

    inline void TimerReset()
    {
        min_time = MAX_TIME;
    }

    inline void TimerStart()
    {
        t.Start();
    }

    inline void TimerSample()
    {
        double e_time = t.Sample();
        min_time = (min_time < e_time) ? min_time : e_time;
    }

    inline void Report(size_t k, const char *fnName)
    {
        double mlogs = ((double)LENGTH)/(min_time * MILLION);
        std::cout << "TestInput: " << k << ", M-" << fnName << "-per-sec: " << mlogs << std::endl;
    }
};

template <typename T, typename TI, T (*libmFn)(T)>
class PerfTestParam1 : public PerfTest<T, TI>
{
    Buffer<T> buf_src1;
    T *ptr_src1;

public:

    PerfTestParam1()
    {
        PerfTest<T, TI>::InitDst();

        buf_src1.Init(LENGTH);
        ptr_src1 = buf_src1.Data();
    }

    void Run(const TI *testVector1, size_t vector1Length, const char *fnName)
    {
        const TU *pV1 = (TU *)testVector1;
        for(size_t k=0; k<vector1Length; k++)
        {
            size_t count = buf_dst.Count();
            for(size_t i=0; i<count; i++)
            {
                ptr_src1[i] = pV1[k].t;
            }

            PerfTest<T, TI>::TimerReset();
            for(size_t j=0; j<REPEAT_COUNT; j++)
            {
                PerfTest<T, TI>::TimerStart();
                for(size_t i=0; i<count; i++)
                {
                    ptr_dst[i] = libmFn(ptr_src1[i]);
                }
                PerfTest<T, TI>::TimerSample();
            }

            PerfTest<T, TI>::Report(k, fnName);
        }
    }
};

template <typename T, typename TI, T (*libmFn)(T, T)>
class PerfTestParam2 : public PerfTest<T, TI>
{
    Buffer<T> buf_src1;
    Buffer<T> buf_src2;
    T *ptr_src1;
    T *ptr_src2;

public:

    PerfTestParam2()
    {
        PerfTest<T, TI>::InitDst();

        buf_src1.Init(LENGTH);
        buf_src2.Init(LENGTH);
        ptr_src1 = buf_src1.Data();
        ptr_src2 = buf_src2.Data();
    }

    void Run(const TI *testVector1, const TI *testVector2, size_t vector1Length, size_t vector2Length, const char *fnName)
    {
        assert(vector1Length == vector2Length);

        const TU *pV1 = (TU *)testVector1;
        const TU *pV2 = (TU *)testVector2;
        for(size_t k=0; k<vector1Length; k++)
        {
            size_t count = buf_dst.Count();
            for(size_t i=0; i<count; i++)
            {
                ptr_src1[i] = pV1[k].t;
                ptr_src2[i] = pV2[k].t;
            }

            PerfTest<T, TI>::TimerReset();
            for(size_t j=0; j<REPEAT_COUNT; j++)
            {
                PerfTest<T, TI>::TimerStart();
                for(size_t i=0; i<count; i++)
                {
                    ptr_dst[i] = libmFn(ptr_src1[i], ptr_src2[i]);
                }
                PerfTest<T, TI>::TimerSample();
            }

            PerfTest<T, TI>::Report(k, fnName);
        }
    }
};

template <typename T, typename TI, T (*libmFn)(T, T*)>
class PerfTestParam1P : public PerfTest<T, TI>
{
    Buffer<T> buf_src1;
    T *ptr_src1;
	T result;

public:

    PerfTestParam1P()
    {
        PerfTest<T, TI>::InitDst();

        buf_src1.Init(LENGTH);
        ptr_src1 = buf_src1.Data();
    }

    void Run(const TI *testVector1, size_t vector1Length, const char *fnName)
    {
        const TU *pV1 = (TU *)testVector1;
        for(size_t k=0; k<vector1Length; k++)
        {
            size_t count = buf_dst.Count();
            for(size_t i=0; i<count; i++)
            {
                ptr_src1[i] = pV1[k].t;
            }

            PerfTest<T, TI>::TimerReset();
            for(size_t j=0; j<REPEAT_COUNT; j++)
            {
                PerfTest<T, TI>::TimerStart();
                for(size_t i=0; i<count; i++)
                {
                    ptr_dst[i] = libmFn(ptr_src1[i], &result);
                }
                PerfTest<T, TI>::TimerSample();
            }

            PerfTest<T, TI>::Report(k, fnName);
        }
    }
};

template <typename T, typename TI, int (*libmFn)(T)>
class PerfTestParam1int : public PerfTest<T, TI>
{
    Buffer<T> buf_src1;
    T *ptr_src1;

public:

    PerfTestParam1int()
    {
        PerfTest<T, TI>::InitDst();

        buf_src1.Init(LENGTH);
        ptr_src1 = buf_src1.Data();
    }

    void Run(const TI *testVector1, size_t vector1Length, const char *fnName)
    {
        const TU *pV1 = (TU *)testVector1;
		int dst;

        for(size_t k=0; k<vector1Length; k++)
        {
            size_t count = buf_dst.Count();
            for(size_t i=0; i<count; i++)
            {
                ptr_src1[i] = pV1[k].t;
            }

            PerfTest<T, TI>::TimerReset();
            for(size_t j=0; j<REPEAT_COUNT; j++)
            {
                PerfTest<T, TI>::TimerStart();
                for(size_t i=0; i<count; i++)
                {
                    dst = libmFn(ptr_src1[i]);
                }
                PerfTest<T, TI>::TimerSample();
            }

            PerfTest<T, TI>::Report(k, fnName);
        }
    }
};

#endif // __PERF_TEST_H__
