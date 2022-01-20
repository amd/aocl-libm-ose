#ifndef __RAND_H__
#define __RAND_H__

/*
 * Copyright (C) 2019-2020, AMD. All rights are reserved
 *
 * Author: Prem Mallappa<pmallapp@amd.com>
 */

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
