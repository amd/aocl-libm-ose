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

/* Include the mallocDebug header, but then undef the things it
   defines because we need to use the genuine versions of malloc et al
   in this file. The #defined malloc is for use by users. */
#include "mallocDebug.h"
#undef malloc
#undef calloc
#undef realloc
#undef free
#undef strdup
#undef mallocInfoDamage

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct mallocDebugPointer *mallocDebugPointerList = NULL;
int mallocDebugNPointers = 0;

void mallocInfoSingle(int i);

/* For reasons I can't fathom, the regular memset on SGI
 * didn't work right when compiled with -mips3 -n32, so use this
 * replacement. (The regular one seemed to set half the bytes to
 * the wrong thing, e.g. memset(ptr, (int)0xef, 8) would set
 * ptr to 377, 377, 377, 377, 357, 357, 357, 357 instead of
 * eight lots of 357. Weird.)
 */
void *mdMemset(void *s, int c, size_t n)
{
  int i;
  unsigned char *ptr = (unsigned char *)s;
  unsigned char cc = (unsigned char)c;
  for (i = 0; i < (int)n; i++)
    {
      *ptr = cc;
      ptr++;
    }
  return s;
}


/* Allocate a memory block and maintain internal information about it.
   Arguments file and line should describe the file and line number
   from which mallocDebug is called. */
void *mallocDebug(size_t size, const char *file, int line)
{
  char *ptr;
  void *tmp;
  size_t internalSize;
  int failed;

  /* Increase the space for the pointer list by 1 element */
  tmp = realloc((void *)mallocDebugPointerList,
          (mallocDebugNPointers + 1) * sizeof(struct mallocDebugPointer));

  if (!tmp)
    return NULL;

  mallocDebugPointerList = (struct mallocDebugPointer *)tmp;

  /* The check on internalSize < size below is in case size is so
     large that we wrap around. This can happen if a small negative
     integer is (illegally) passed as argument size. */
  internalSize = size + 2 * EDGESIZE;
  ptr = NULL;
  failed = internalSize < size;
  if (!failed)
    {
      ptr = (char *)malloc(internalSize);
      failed = (ptr == NULL);
    }
    
  if (failed)
    {
      /* Failed - chop the pointer list back to its original size */
      mallocDebugPointerList = (struct mallocDebugPointer *)
        realloc(mallocDebugPointerList,
                mallocDebugNPointers * sizeof(struct mallocDebugPointer));
      return NULL;
    }

  /* Store the information in the list */
  mallocDebugPointerList[mallocDebugNPointers].ptr = (void *)(ptr + EDGESIZE);
  mallocDebugPointerList[mallocDebugNPointers].size = size;
  mallocDebugPointerList[mallocDebugNPointers].file = strdup(file);
  mallocDebugPointerList[mallocDebugNPointers].line = line;
  mallocDebugPointerList[mallocDebugNPointers].watchFree = 0;
  mallocDebugNPointers++;

  /* Initialise the memory to something recognisable
     with EDGESIZE extra bytes at each end */
  mdMemset(ptr, (int)EDGEBYTE, EDGESIZE);
  mdMemset(ptr+EDGESIZE, (int)FILLBYTE, size);
  mdMemset(ptr+EDGESIZE+size, (int)EDGEBYTE, EDGESIZE);

  /* Return a pointer to the memory the caller can actually use */
  return mallocDebugPointerList[mallocDebugNPointers - 1].ptr;
}


/* Allocate memory for "num" objects of size "size", and fill with zeros */
void *callocDebug(size_t num, size_t size, const char *file, int line)
{
  size_t s = num * size;
  void *ptr = mallocDebug(s, file, line);
  if (ptr)
    mdMemset(ptr, 0, s);
  return ptr;
}


/* Checks whether a pointer was allocated by mallocDebug.
   Return value -1 means "no", otherwise we return
   0, 1, 2, ... , the number of the pointer in our list. */
int mallocIsValidPointer(void *ptr)
{
  int i;

  i = 0;
  while (i < mallocDebugNPointers)
    {
      if (ptr == mallocDebugPointerList[i].ptr)
        break;
      i++;
    }
  if (i == mallocDebugNPointers)
    i = -1;

  return i;
}


/* Check that pointer number p's memory hasn't been damaged at the ends.
   Issue an error message if it has. */
void mallocCheckDamage(int p, const char *caller, int line, const char *file)
{
  unsigned char *bpt, *ept;
  int j, frontDamage, endDamage;

  frontDamage = 0;
  endDamage = 0;
  bpt = (unsigned char *)mallocDebugPointerList[p].ptr - EDGESIZE;
  ept = (unsigned char *)mallocDebugPointerList[p].ptr + mallocDebugPointerList[p].size;
  for (j = 0; j < EDGESIZE; j++)
    {
      if (*bpt++ != (unsigned char)EDGEBYTE)
        frontDamage = 1;
      if (*ept++ != (unsigned char)EDGEBYTE)
        endDamage = 1;
    }

  if (frontDamage)
    fprintf(stderr, "%s (called at line %d of file \"%s\"):\n"
            "  front boundary of allocated memory at 0x%lx damaged;\n",
            caller, line, file, (unsigned long)mallocDebugPointerList[p].ptr);

  if (endDamage)
    fprintf(stderr, "%s (called at line %d of file \"%s\"):\n"
            "  end boundary of allocated memory at 0x%lx damaged;\n",
            caller, line, file, (unsigned long)mallocDebugPointerList[p].ptr);

  if (frontDamage || endDamage)
    fprintf(stderr, "  this memory was allocated at line %d of file \"%s\".\n",
            mallocDebugPointerList[p].line, mallocDebugPointerList[p].file);
}


/* Reallocate a memory block allocated by mallocDebug */
void *reallocDebug(void *memblock, size_t size, const char *file, int line)
{
  int i, failed;
  char *ptr;
  size_t oldSize, internalSize;

  if (!memblock)
    {
      if (size == 0)
        /* Defined behaviour for NULL memblock and size = 0 */
        return NULL;
      else
        /* A NULL argument with size > 0 means we just need to
           call mallocDebug */
        return mallocDebug(size, file, line);
    }

  /* See if we allocated this pointer */
  i = mallocIsValidPointer(memblock);
  if (i == -1)
    {
      fprintf(stderr,
              "reallocDebug (called at line %d of file \"%s\"):\n"
              "  pointer 0x%lx was not allocated by mallocDebug, "
              "or has already been freed.\n", line, file,
              (unsigned long)memblock);
      /* Reallocate it anyway - perhaps it was allocated by some other
         function that we know nothing of (like cxFilenameExpand) */
      return realloc(memblock, size);
    }

  /* First issue a message if the pointer's boundary memory got damaged */
  mallocCheckDamage(i, "reallocDebug", line, file);

  /* Defined behaviour when memblock not NULL and size = 0 */
  if (size == 0)
    freeDebug(memblock, file, line);

  /* Get the pointer that the genuine malloc routine knows about */
  ptr = ((char *)mallocDebugPointerList[i].ptr - EDGESIZE);

  /* Try to reallocate it.
     The check on internalSize < size below is in case size is so
     large that we wrap around. This can happen if a small negative
     integer is (illegally) passed as argument size. */
  internalSize = size + 2 * EDGESIZE;

  failed = internalSize < size;
  if (!failed)
    {
      ptr = (char *)realloc(ptr,internalSize);
      failed = (ptr == NULL);
    }
    
  if (failed)
    {
      /* Failed - leave the original block unchanged and return NULL */
      return NULL;
    }

  /* Set filler bytes */
  oldSize = mallocDebugPointerList[i].size;
  if (size > oldSize)
    {
      /* The memory block has grown; fill the new bytes with FILLBYTE */
      size_t nBytes = size - oldSize;
      mdMemset(ptr+EDGESIZE+oldSize, (int)FILLBYTE, nBytes);
    }
  /* Replace the terminating edge bytes */
  mdMemset(ptr+EDGESIZE+size, (int)EDGEBYTE, EDGESIZE);

  /* Update the information in the list we maintain */
  mallocDebugPointerList[i].ptr = (void *)(ptr + EDGESIZE);
  mallocDebugPointerList[i].size = size;
  free(mallocDebugPointerList[i].file);
  mallocDebugPointerList[i].file = strdup(file);
  mallocDebugPointerList[i].line = line;

  /* Return a pointer to the memory the caller can actually use */
  return mallocDebugPointerList[i].ptr;
}


/* Free a memory block allocated by mallocDebug or reallocDebug */
void freeDebug(void *memblock, const char *file, int line)
{
  int i;
  size_t nBytes;

  /* Check the malloced pointer list to see if we're allowed
     to free this pointer */
  i = mallocIsValidPointer(memblock);
  if (i == -1)
    {
      fprintf(stderr,
              "freeDebug (called at line %d of file \"%s\"):\n"
              "  pointer 0x%lx was not allocated by mallocDebug, "
              "or has already been freed.\n", line, file,
              (unsigned long)memblock);
      /* Free it anyway - perhaps it was allocated by some other
         function that we know nothing of (like cxFilenameExpand) */
      free(memblock);
      return;
    }

  /* OK - we found the pointer in the list. Let's remove it. */

  /* First issue a message if the pointer's boundary memory got damaged */
  mallocCheckDamage(i, "freeDebug", line, file);

  /* If we had a call of watchFree for this pointer, tell the user */
  if (mallocDebugPointerList[i].watchFree)
    {
      printf("freeDebug: freeing watched pointer 0x%lx\n",
             (unsigned long)mallocDebugPointerList[i].ptr);
      mallocInfoSingle(i);
    }

  /* Fill the memory with garbage, then really free it */
  mdMemset(mallocDebugPointerList[i].ptr, (int)FREEBYTE,
           mallocDebugPointerList[i].size);
  free((char *)(mallocDebugPointerList[i].ptr) - EDGESIZE);
  free(mallocDebugPointerList[i].file);

  /* Now remove it from our list */
  nBytes = (mallocDebugNPointers - i - 1) * sizeof(struct mallocDebugPointer);
  if (nBytes > 0)
    {
      void *dest = &mallocDebugPointerList[i];
      void *src = &mallocDebugPointerList[i+1];
      memmove(dest, (const void *)src, nBytes);
    }
  mallocDebugNPointers--;

  /* Make the list smaller */
  mallocDebugPointerList = (struct mallocDebugPointer *)
    realloc((void *)mallocDebugPointerList,
            mallocDebugNPointers * sizeof(struct mallocDebugPointer));

}


/* mallocDebug-dependent version of strdup */
char *strdupDebug(const char *string, const char *file, int line)
{
  char *ptr;
  if (!string)
    {
      fprintf(stderr, "strdupDebug: error: argument \"string\" is NULL.\n");
      fprintf(stderr, "  (called at line %d of file \"%s\")\n", line, file);
      return NULL;
    }
  ptr = (char *)mallocDebug(strlen(string) + 1, file, line);
  if (!ptr)
    return NULL;
  strcpy(ptr, string);
  return ptr;
}


/* Computes the total number of bytes currently allocated */
int mallocInfoBytes(void)
{
  int i, nBytes = 0;
  for (i = 0; i < mallocDebugNPointers; i++)
    nBytes += (int)mallocDebugPointerList[i].size;
  return nBytes;
}


/* Prints info about a currently allocated block */
void mallocInfoSingle(int i)
{
  int j, k, nPrint;
  unsigned char c;
  float f;
  REAL d;

  nPrint = (int)mallocDebugPointerList[i].size;
  if (nPrint > 24)
    nPrint = 24;

  printf("  block %d at 0x%lx size %d bytes. First %d bytes:\n", i,
         (unsigned long)mallocDebugPointerList[i].ptr,
         (int)mallocDebugPointerList[i].size, nPrint);

  printf("  (as bytes):");
  for (j = 0; j < nPrint; j++)
    {
      c = ((unsigned char *)(mallocDebugPointerList[i].ptr))[j];
      k = (int)c;
      printf(" %d", k);
    }

  printf("\n");
  printf("  (as ints):");
  for (j = 0; j < nPrint/(int)sizeof(int); j++)
    {
      k = ((int *)(mallocDebugPointerList[i].ptr))[j];
      printf(" %d", k);
    }
  printf("\n");

  printf("  (as hex):");
  for (j = 0; j < nPrint/(int)sizeof(unsigned long); j++)
    {
      k = ((unsigned long *)(mallocDebugPointerList[i].ptr))[j];
      printf(" %8.8x", k);
    }
  printf("\n");

  printf("  (as character string): \"");
  for (j = 0; j < nPrint; j++)
    {
      c = ((unsigned char *)(mallocDebugPointerList[i].ptr))[j];
      if (c < 32 || c > 127)
        c = '.';
      printf("%c", c);
    }
  printf("\"\n");

  printf("  (as floats):");
  for (j = 0; j < nPrint/(int)sizeof(float); j++)
    {
      f = ((float *)(mallocDebugPointerList[i].ptr))[j];
      printf(" %g", f);
    }
  printf("\n");

  printf("  (as REALs):");
  for (j = 0; j < nPrint/(int)sizeof(REAL); j++)
    {
      d = ((REAL *)(mallocDebugPointerList[i].ptr))[j];
      printf(" %g", d);
    }
  printf("\n");

  printf("  allocated at line %d of file \"%s\".\n",
         mallocDebugPointerList[i].line, mallocDebugPointerList[i].file);
}


/* Prints info about all currently allocated blocks */
void mallocInfo(void)
{
  int i;

  if (mallocDebugNPointers > 0)
    {
      printf("mallocInfo: %d memory block%s currently allocated:\n",
             mallocDebugNPointers, mallocDebugNPointers == 1? "": "s");
      for (i = 0; i < mallocDebugNPointers; i++)
        mallocInfoSingle(i);
    }
  printf("mallocInfo: %d bytes allocated in total.\n", mallocInfoBytes());
}


/*
   Prints messages about any currently allocated block
   whose boundaries are damaged.

   N.B. This function should get called by the user program as
   mallocInfoDamage() to get the correct file name and line number
   - see the #define in mallocDebug.h
 */
void mallocInfoDamageInternal(const char *file, int line)
{
  int i;

  for (i = 0; i < mallocDebugNPointers; i++)
    mallocCheckDamage(i, "mallocInfoDamage", line, file);
}


/* Tells freeDebug to issue a message when a pointer is freed */
void watchFree(void *ptr)
{
  int i, wf;
  wf = 0;
  for (i = 0; i < mallocDebugNPointers; i++)
    {
      if (ptr == mallocDebugPointerList[i].ptr)
        {
          wf = 1;
          mallocDebugPointerList[i].watchFree = 1;
          break;
        }
    }
  if (wf)
    printf("watchFree: watching for when pointer 0x%lx is freed\n",
           (unsigned long)ptr);
  else
    printf("watchFree: cannot find pointer 0x%lx in malloc list\n",
           (unsigned long)ptr);
}
