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
