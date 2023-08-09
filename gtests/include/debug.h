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


#ifndef __LIBM_DEBUG_H__
#define __LIBM_DEBUG_H__

#define LIBM_TEST_DEBUG

#ifdef LIBM_TEST_DEBUG

extern unsigned int  dbg_bits;

enum {
    LIBM_TEST_DBG_PANIC, LIBM_TEST_DBG_CRIT, LIBM_TEST_DBG_WARN, /* error level */
    LIBM_TEST_DBG_INFO, /* info level */
    LIBM_TEST_DBG_DBG1, LIBM_TEST_DBG_DBG2,
    LIBM_TEST_DBG_VERBOSE1, LIBM_TEST_DBG_VERBOSE2, LIBM_TEST_DBG_VERBOSE3, /* verbosity */
};

#define DBG_BIT(bit)    (1 << LIBM_TEST_DBG_##bit)

#define IS_DBG_ENABLED(bit) (dbg_bits & (1 << LIBM_TEST_DBG_##bit))

#define DBG_DEFAULT  (DBG_BIT(PANIC) | DBG_BIT(CRIT) | DBG_BIT(WARN))
#define DBG_DEBUG1   (DBG_DEFAULT | DBG_BIT(DBG1))
#define DBG_DEBUG2   (DBG_DEBUG2 | DBG_BIT(DBG2))
#define DBG_VERBOSE2 (DBG_VERBOSE1 | DBG_BIT(DBG1))
#define DBG_VERBOSE3 (DBG_VERBOSE2 | DBG_BIT(DBG2))
#define DBG_VERBOSE4 (DBG_VERBOSE3 | DBG_BIT(INFO))

#if 0
template<typename... Args>
void print(const char* file, int line, Args... args) {
  (std::clog << "[" << file << ":" << line << "] "
            << ... << args) << std::endl;
}
#else
template <typename A, typename... Args>
void printarg(A arg, Args... args) {
  std::string sep = " ";
  std::cout << arg;
  if constexpr (sizeof...(Args) > 0) {
    std::cout << sep;
    printarg(args...);
  }
}
template<typename... Args>
void print(const char* file, int line, Args... args) {
  std::clog << "[" << file << ":" << line << "] ";
  printarg(args...);
  std::cout << " " << std::endl;
}  
#endif

#define LIBM_TEST_DPRINTF(lvl, fmt, ...)				  \
	do {								\
		if (dbg_bits & DBG_BIT(lvl) &&				           \
		    (DBG_BIT(lvl) <= dbg_bits)) {			         \
        print(__FILE__, __LINE__, __VA_ARGS__); \
		}							\
	} while (0) 

#else

#define IS_DBG_ENABLED(bit) false
#define LIBM_TEST_DPRINTF(lvl, fmt, ...)

#endif  /* LIBM_TEST_DEBUG */

#endif  /* __LIBM_DEBUG_H__ */
