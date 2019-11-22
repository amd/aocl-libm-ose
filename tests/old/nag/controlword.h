#ifndef CONTROLWORD_H_INCLUDED
#define CONTROLWORD_H_INCLUDED 1

#include <stdio.h>
#include "precision.h"

#ifndef IS_64BIT
/* Type representing x87 floating-point environment.
   This structure corresponds to the layout of the block
   written by the fstenv instruction.  */
typedef struct
  {
    unsigned short control_word;
    unsigned short unused1;
    unsigned short status_word;
    unsigned short unused2;
    unsigned short tags;
    unsigned short unused3;
    unsigned int   eip;
    unsigned short cs_selector;
    unsigned int   opcode:11;
    unsigned int   unused4:5;
    unsigned int   data_offset;
    unsigned short data_selector;
    unsigned short unused5;
  }
fpenv_type;
#endif

/* Processor-dependent floating-point status flags */
#define AMD_F_INEXACT 0x00000020
#define AMD_F_UNDERFLOW 0x00000010
#define AMD_F_OVERFLOW 0x00000008
#define AMD_F_DIVBYZERO 0x00000004
#define AMD_F_INVALID 0x00000001

/* Processor-dependent floating-point precision-control flags */
#define AMD_F_EXTENDED 0x00000300
#define AMD_F_DOUBLE   0x00000200
#define AMD_F_SINGLE   0x00000000

/* Processor-dependent floating-point rounding-control flags */
#ifdef IS_64BIT
/* These are for Hammer */
#define AMD_F_RC_NEAREST 0x00000000
#define AMD_F_RC_DOWN    0x00002000
#define AMD_F_RC_UP      0x00004000
#define AMD_F_RC_ZERO    0x00006000
#else
/* These are for Athlon */
#define AMD_F_RC_NEAREST 0x00000000
#define AMD_F_RC_DOWN    0x00000400
#define AMD_F_RC_UP      0x00000800
#define AMD_F_RC_ZERO    0x00000C00
#endif

/* Sets the floating point control word  for rounding precision in
   floating-point registers:
     p = 0 sets round to 24 bits.
     p = 1 sets round to 53 bits.
     p = 2 sets round to 64 bits.
*/
void setprecision(int p);
void setprecision_(int *p);

/* Sets the floating point control word  for rounding mode in
   floating-point registers:
     p = 0 sets round to nearest.
     p = 1 sets round to -infinity.
     p = 2 sets round to +infinity.
     p = 3 sets round to zero.
*/
void setrounding(int r);
void setrounding_(int *r);

#endif
