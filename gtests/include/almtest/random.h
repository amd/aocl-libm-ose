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


#ifndef __RAND_H__
#define __RAND_H__

#include <sys/types.h> /*            */
#include <sys/stat.h>  /* for open() */
#include <fcntl.h>     /*            */
#if defined(_WIN64) || (_WIN32)
  #include <Windows.h>
#else
  #include <unistd.h> /* for read() */
#endif
#include "defs.h"

using namespace ALM;

namespace ALMTest {

template <typename T>
class Random {
 private:
  int randfd;
  ALM::RangeType dist;
  T min, max, cur;

  int fillSimple(T *data, uint32_t nelem, T min, T max);
  int fillLinear(T *data, uint32_t nelem, T min, T max);
  int fillRandom(T *data, uint32_t nelem, T min, T max);

  int init_randfd(void) {
    uint64_t rand_val = 0xC001BEAFDEADBEAF;

    if (this->randfd > 0) {
      if (read(randfd, &rand_val, 8) != 8) rand_val = 0xD00BEEC001BEAF;
      srand(rand_val);
    }
    return 0;
  }

 public:
  Random() {
    randfd = open("/dev/urandom", O_RDONLY);
    if (randfd == -1) throw "Unable to open file /dev/urandom";
  }

  Random(ALM::RangeType d) : dist(d){};

#if defined(__GNUC__)
  ~Random() { close(randfd); }
#else
  ~Random() {}
#endif

  /*
      * Returns the next random number
      */
  T Next();

  /*
      * Fills the Random number to a distination buffer of size nelem
      */
  int Fill(T *data, uint32_t nelem, T min, T max,
           ALM::RangeType r = ALM::RangeType::E_Simple);
};
}
#endif
