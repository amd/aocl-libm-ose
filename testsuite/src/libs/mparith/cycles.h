/*
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef CYCLES_H_INCLUDED
#define CYCLES_H_INCLUDED 1

#define NTRIALS 10

#include "precision.h"

#if defined(WINDOWS)

#undef inline
#define inline __inline

#ifdef IS_64BIT
/* 64-bit Windows */

typedef unsigned long  DWORD;
typedef unsigned __int64 DWORDLONG;
typedef unsigned __int64  MY_ULARGE_INTEGER;

#define _SHIFT32 (4.0 * 1024.0 * 1024.0 * 1024.0)

inline MY_ULARGE_INTEGER RDTSC()
{
  __int64 tsc;
  tsc = __rdtsc();
  return tsc;
}
inline REAL DIFTSC( MY_ULARGE_INTEGER t0, MY_ULARGE_INTEGER t1)
{
    return ( (REAL) ( t1 - t0));
}

#else
/* 32-bit Windows */

/*=============================================================================
   Base type definitions. We do it ourselves, because we are used in both
   user-mode WIN32 pgms and Kernel-mode drivers.
*/
#if (!defined(_WINDEF_)) && (!defined(_NTDEF_))

/*  #define ULONG unsigned long int */
  typedef unsigned long  DWORD;

/* __int64 is only supported by 2.0 and later midl. */
/* __midl is set by the 2.0 midl and not by 1.0 midl. */
  #if (!defined(MIDL_PASS) || defined(__midl)) && (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
        typedef unsigned __int64 DWORDLONG;
  #else
        typedef REAL DWORDLONG;
  #endif

#if defined(MIDL_PASS)
        typedef struct _MY_ULARGE_INTEGER {
#else /* MIDL_PASS */
        typedef union _MY_ULARGE_INTEGER {
            struct {
                DWORD LowPart;
                DWORD HighPart;
            } w;
            struct {
                DWORD LowPart;
                DWORD HighPart;
            } u;
#endif /* MIDL_PASS */
            DWORDLONG QuadPart;
        } MY_ULARGE_INTEGER;
#endif

#define _SHIFT32 (4.0 * 1024.0 * 1024.0 * 1024.0)

#define _cpuid __asm _emit 0x0F __asm _emit 0xA2
#define _rdtsc __asm _emit 0x0F __asm _emit 0x31

static inline MY_ULARGE_INTEGER RDTSC(void)
{ // Basic routine to read TSC with synchronization beforehand
  MY_ULARGE_INTEGER x;

  // The change to eax, ebx, ecx, edx should be visible to compiler and
  // so it should deal with saving / resorting as needed

  __asm{
      xor eax, eax
      xor ebx, ebx
      xor ecx, ecx
      xor edx, edx
      _cpuid
      _rdtsc
      mov x.w.HighPart, edx
      mov x.w.LowPart, eax
      }

  return x;
}

static inline REAL DIFTSC( MY_ULARGE_INTEGER t0, MY_ULARGE_INTEGER t1)
{
  unsigned long int t1h;

  t1h = (t0.w.LowPart > t1.w.LowPart) ? t1.w.HighPart-1 : t1.w.HighPart;

  return ( (REAL) ( t1.w.LowPart - t0.w.LowPart ) ) +
    ( (REAL) ( t1h - t0.w.HighPart ) ) * (REAL)_SHIFT32;

}
#endif

#elif defined(linux)

/* Linux */

typedef struct _MY_ULARGE_INTEGER {
  unsigned long int LowPart;
  unsigned long int HighPart;
} MY_ULARGE_INTEGER;


static inline MY_ULARGE_INTEGER RDTSC(void)
{ // Basic routine to read TSC with synchronization beforehand
  MY_ULARGE_INTEGER x;

  // The change to eax, ebx, ecx, edx should be visible to compiler and
  // so it should deal with saving / resorting as needed

  unsigned long int eax = 0, ebx = 0, ecx = 0, edx = 0;
  unsigned long int op = 0;

  __asm__ volatile ("cpuid"
                    : "=a" (eax),
                    "=b" (ebx),
                    "=c" (ecx),
                    "=d" (edx)
                    : "a" (op)
                    : "cc");

  __asm__ volatile ("rdtsc"
                    : "=a" (x.LowPart),
                    "=d" (x.HighPart)
                    );

  return x;
}


static inline REAL DIFTSC( MY_ULARGE_INTEGER t0, MY_ULARGE_INTEGER t1)
{
  const REAL _SHIFT32 = (4.0 * 1024.0 * 1024.0 * 1024.0);
  unsigned long int t1h,tlow,thi;
  REAL result;

  t1h = (t0.LowPart > t1.LowPart) ? t1.HighPart-1 : t1.HighPart;

  tlow = (t1.LowPart-t0.LowPart);
  thi  = (t1h - t0.HighPart);
  result = (REAL) tlow + ((REAL)thi *_SHIFT32);

  return result;
}

#else

/* Other UNIX.  */

#include <sys/resource.h>

typedef struct _MY_ULARGE_INTEGER {
  unsigned long int LowPart;
  unsigned long int HighPart;
} MY_ULARGE_INTEGER;

/* Don't know how to compute machine cycles, so do something else.
   Just return 1000,000,000 times the cpu time in seconds. */

static inline MY_ULARGE_INTEGER RDTSC(void)
{
  struct rusage russ;
  REAL secs;
  const REAL _SHIFT32 = (4.0 * 1024.0 * 1024.0 * 1024.0);
  MY_ULARGE_INTEGER retval;
  getrusage(RUSAGE_SELF, &russ);
  secs = russ.ru_utime.tv_sec + russ.ru_utime.tv_usec / 1000000.0 +
    russ.ru_stime.tv_sec + russ.ru_stime.tv_usec / 1000000.0;

  secs = secs * 1000000000.0;
  retval.HighPart = (unsigned long int)(secs / _SHIFT32);
  retval.LowPart = secs - _SHIFT32 * retval.HighPart;

  return retval;
}


static inline REAL DIFTSC( MY_ULARGE_INTEGER t0, MY_ULARGE_INTEGER t1)
{
  const REAL _SHIFT32 = (4.0 * 1024.0 * 1024.0 * 1024.0);
  unsigned long int tlow,thi;
  REAL result;

  if (t1.LowPart < t0.LowPart)
    {
      tlow = _SHIFT32 + t1.LowPart - t0.LowPart;
      thi = t1.HighPart - 1 - t0.HighPart;
    }
  else
    {
      tlow = t1.LowPart - t0.LowPart;
      thi = t1.HighPart - t0.HighPart;
    }
  result = (REAL) tlow + ((REAL)thi *_SHIFT32);

  return result;
}

#endif

int rdist(int a, int b);

/* Returns a value logarithmically distributed in [a,b] */
REAL logdist(int i, int n, REAL a, REAL b, REAL logba);

/* Returns a value logarithmically distributed in [a,b] */
REAL logdist2(int i, int n, REAL a, REAL b, REAL logba);

/* Compute the overhead of the timing function */
REAL computeTimerOverhead(void);

#endif
