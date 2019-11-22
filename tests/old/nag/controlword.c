#include "controlword.h"

/* Sets the floating point control word  for rounding precision in
   floating-point registers:
     p = 0 sets round to 24 bits.
     p = 1 sets round to 53 bits.
     p = 2 sets round to 64 bits.
*/
void setprecision(int p)
{
#if defined(IS_64BIT)
  /* There is no precision control on Hammer */
  if (p == -1)
    printf("Illegal call of setprecision_amd with p == -1\n");
#else
#if defined(WINDOWS)
  unsigned int cw;
  __asm fstcw cw;
  cw &= (~0x00000300); /* These two bits control rounding precision */
  if (p == 0)
    cw |= AMD_F_SINGLE;
  else if (p == 1)
    cw |= AMD_F_DOUBLE;
  else
    cw |= AMD_F_EXTENDED;
  __asm fldcw cw;
#elif defined(linux)
  unsigned int cw;
  asm volatile ("fstcw %0" : "=m" (cw));
  cw &= (~0x00000300); /* These two bits control rounding precision */
  if (p == 0)
    cw |= AMD_F_SINGLE;
  else if (p == 1)
    cw |= AMD_F_DOUBLE;
  else
    cw |= AMD_F_EXTENDED;
  asm volatile ("fldcw %0" : : "m" (cw));
#else
  /* Do nowt */
  if (p == -1)
    printf("Illegal call of setprecision_amd with p == -1\n");
  return;
#endif
#endif
}


void setprecision_(int *p)
{
  setprecision(*p);
}

/* Sets the floating point control word  for rounding mode in
   floating-point registers:
     r = 0 sets round to nearest.
     r = 1 sets round to -infinity.
     r = 2 sets round to +infinity.
     r = 3 sets round to zero.
*/
void setrounding(int r)
{
#if defined(WINDOWS)
#ifdef IS_64BIT
  /* Get the current floating-point control word */
  unsigned int cw = _mm_getcsr();
  cw &= (~0x00006000); /* These two bits control rounding mode on Hammer */
  if (r == 0)
    cw |= AMD_F_RC_NEAREST;
  else if (r == 1)
    cw |= AMD_F_RC_DOWN;
  else if (r == 2)
    cw |= AMD_F_RC_UP;
  else
    cw |= AMD_F_RC_ZERO;
  /* Load back into the control/status word */
  _mm_setcsr(cw);
#else
  unsigned int cw;
  /* Get the current floating-point control word */
  __asm fstcw cw;
  cw &= (~0x00000c00); /* These two bits control rounding mode on Athlon */
  if (r == 0)
    cw |= AMD_F_RC_NEAREST;
  else if (r == 1)
    cw |= AMD_F_RC_DOWN;
  else if (r == 2)
    cw |= AMD_F_RC_UP;
  else
    cw |= AMD_F_RC_ZERO;
  /* Load back into the control/status word */
  __asm fldcw cw;
#endif
#elif defined(linux)
  unsigned int cw;
  /* Get the current floating-point control word */
#if defined(IS_64BIT)
  asm volatile ("STMXCSR %0" : "=m" (cw));
  cw &= (~0x00006000); /* These two bits control rounding mode on Hammer */
#else
  asm volatile ("fstcw %0" : "=m" (cw));
  cw &= (~0x00000c00); /* These two bits control rounding mode on Athlon */
#endif
  if (r == 0)
    cw |= AMD_F_RC_NEAREST;
  else if (r == 1)
    cw |= AMD_F_RC_DOWN;
  else if (r == 2)
    cw |= AMD_F_RC_UP;
  else
    cw |= AMD_F_RC_ZERO;
  /* Load back into the control/status word */
#ifdef IS_64BIT
  asm volatile ("LDMXCSR %0" : : "m" (cw));
#else
  asm volatile ("fldcw %0" : : "m" (cw));
#endif
#else
  /* Do nowt */
  if (r == -1)
    printf("Illegal call of setrounding_amd with r == -1\n");
  return;
#endif
}

void setrounding_(int *r)
{
  setrounding(*r);
}

#ifdef TEST
int main()
{
  REAL x, y, z;
  x = 1.0;
  y = 3.0;
  printf("Computing z = x / y, where x = 1.0, y = 3.0\n");
  printf("Rounding to nearest\n");
  setrounding(0);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding to -infinity\n");
  setrounding(1);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding to +infinity\n");
  setrounding(2);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding to zero\n");
  setrounding(3);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding to nearest again\n");
  setrounding(0);
  printf("Rounding internally to 24 bits\n");
  setprecision(0);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding internally to 53 bits\n");
  setprecision(1);
  z = x / y;
  printf("z = %30.20lf\n", z);
  printf("Rounding internally to 64 bits\n");
  setprecision(2);
  z = x / y;
  printf("z = %30.20lf\n", z);
  return 0;
}
#endif
