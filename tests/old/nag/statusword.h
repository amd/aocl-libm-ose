#ifndef STATUSWORD_H_INCLUDED
#define STATUSWORD_H_INCLUDED 1

#include "precision.h"
#include "controlword.h"

/* Return the current floating-point status word */
unsigned int get_fpsw(void);

/* Clear all exceptions from the floating-point status word */
void clear_fpsw(void);

/* Returns 1 if the inexact flag has been raised in sw, otherwise 0 */
int sw_inexact_raised(unsigned int sw);

/* Returns 1 if the underflow flag has been raised in sw, otherwise 0 */
int sw_underflow_raised(unsigned int sw);

/* Returns 1 if the overflow flag has been raised in sw, otherwise 0 */
int sw_overflow_raised(unsigned int sw);

/* Returns 1 if the division-by-zero flag has been raised in sw, otherwise 0 */
int sw_divbyzero_raised(unsigned int sw);

/* Returns 1 if the invalid flag has been raised in sw, otherwise 0 */
int sw_invalid_raised(unsigned int sw);

/* Shows all exceptions signalled in status word sw */
void show_status(unsigned int sw, FILE *outFile);

/* Gets all exceptions signalled in status word sw as a bit mask */
unsigned int get_sw_flags(void);

#endif
