#ifndef MALLOCDEBUG_H
#define MALLOCDEBUG_H 1

#include "precision.h"

/*
   Debugging malloc routines
   -------------------------
   These routines can act as a replacement for functions
     malloc()
     calloc()
     realloc()
     free()
     strdup()
   To use them, write your program as usual to use malloc, calloc,
   free et al, but #include "mallocDebug.h" at the top of your
   calling program. All calls to malloc et al will then be replaced
   by calls to mallocDebug etc. When you are happy that your program
   works correctly with the debugging malloc routines, simply
   omit the #include "mallocDebug" line, and remove any calls of
   mallocInfo() and mallocInfoBytes(), and your program will
   use the regular functions.

   The replacement routines maintain extra information along with
   each allocated pointer:
     (i) the line number and name of the file where a call to one of
         the allocation routines occurred.
     (ii) the size of each allocated block.
     (iii) boundary bytes at each end of an allocated block which
         contain known values. If these bytes are accidentally
        overwritten, the free function will notice the fact and
        issue a warning.
   Anywhere in your program, a call of mallocInfo will give
   details of all currently allocated blocks. This is particularly
   useful at the end of your program, to detect memory leaks.
   At the single function level, memory leaks may be detected
   by calls of mallocInfoBytes() on entry and on exit; a comparison
   of the two returned values will uncover a leak.

*/


#ifdef __cplusplus
extern "C" {
#endif

/*
   Make sure these are included first, so that if a user program
   also includes them they won't get included again (if they DO
   get included again, they will cause errors because of the
   defines below)
*/
#include <malloc.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#undef malloc
#define malloc(s) mallocDebug(s, __FILE__, __LINE__)
#undef calloc
#define calloc(n, s) callocDebug(n, s, __FILE__, __LINE__)
#undef realloc
#define realloc(p, s) reallocDebug(p, s, __FILE__, __LINE__)
#undef free
#define free(p) freeDebug(p, __FILE__, __LINE__)
#undef strdup
#define strdup(s) strdupDebug(s, __FILE__, __LINE__)
#undef mallocInfoDamage
#define mallocInfoDamage() mallocInfoDamageInternal(__FILE__, __LINE__)

/* Used to store the list of pointers we've allocated */
struct mallocDebugPointer
  {
    void *ptr;
    size_t size;
    char *file;
    int line;
    int watchFree;
  };

#define FILLBYTE 0xcd
#define FREEBYTE 0xee
#define EDGEBYTE 0xef
#define EDGESIZE 16

/* N.B. To make a dll, put "__declspec(dllexport)" in front of
   every declaration */

/* Allocate a memory block and maintain internal information about it.
   Arguments file and line should describe the file and line number
   from which mallocDebug is called. */
void *mallocDebug(size_t size, const char *file, int line);

/* Allocate memory for num objects of size "size", and fill with zeros */
void *callocDebug(size_t num, size_t size, const char *file, int line);

/* Reallocate a memory block allocated by mallocDebug */
void *reallocDebug(void *memblock, size_t size, const char *file, int line);

/* Free a memory block allocated by mallocDebug or reallocDebug */
void freeDebug(void *memblock, const char *file, int line);

/* mallocDebug-dependent version of strdup */
char *strdupDebug(const char *string, const char *file, int line);

/* Prints info about currently allocated blocks */
void mallocInfo(void);

void mallocInfoDamageInternal(const char *file, int line);

/* Tells freeDebug to issue a message when a pointer is freed */
void watchFree(void *ptr);

#ifdef __cplusplus
           }
#endif

#endif
