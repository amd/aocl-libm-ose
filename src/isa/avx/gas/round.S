#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# round.S
#
# An implementation of the round libm function.
#
# Prototype:
#
#     double round(double x);
#

#
#   Algorithm: First get the exponent of the input 
#              double precision number. 
#              IF exponent is greater than 51 then return the 
#              input as is. 
#              IF exponent is less than 0 then force an overflow
#              by adding a huge number and subtracting with the 
#              same number. 
#              IF exponent is greater than 0 then add 0.5 and 
#              and shift the mantissa bits based on the exponent
#              value to discard the fractional component. 
#              
#

#include "fn_macros.h"
#include "weak_macros.h"

#define fname ALM_PROTO_BAS64(round)
#define fname_special _round_special



# local variable storage offsets


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function

#in sse5 there is a roundss,roundsd instruction
fname:
    movsd .L__2p52_plus_one(%rip),%xmm4
    movsd .L__sign_mask_64(%rip),%xmm5
    mov $52,%r10
    #take 3 copies of the input xmm0
    movsd %xmm0,%xmm1
    movsd %xmm0,%xmm2
    movsd %xmm0,%xmm3
    #get the Most signifacnt half word of the input number in r9
    pand .L__exp_mask_64(%rip), %xmm1
    pextrw $3,%xmm1,%r9
    cmp $0X7FF0,%r9
    #Check for infinity inputs
    jz .L__is_infinity
    movsd .L__sign_mask_64(%rip), %xmm1
    pandn %xmm2,%xmm1 # xmm1 now stores the sign of the input number 
    #On shifting r9 and subtracting with 0x3FF 
    #r9 stores the exponent.
    shr   $0X4,%r9
    sub $0x3FF,%r9
    cmp $0x00, %r9
    jl .L__number_less_than_zero

    #IF exponent is greater than 0
.L__number_greater_than_zero:
    cmp $51,%r9
    jg .L__is_greater_than_2p52
    
    #IF exponent is greater than 0 and less than 2^52
    pand .L__sign_mask_64(%rip),%xmm0
    #add with 0.5
    addsd .L__zero_point_5(%rip),%xmm0
    movsd %xmm0,%xmm5

    pand  .L__exp_mask_64(%rip),%xmm5
    pand  .L__mantissa_mask_64(%rip),%xmm0
    #r10 = r9(input exponent) - r10(52=mantissa length) 
    sub %r9,%r10
    movd %r10, %xmm2
    #do right and left shift by (input exp - mantissa length)
    psrlq %xmm2,%xmm0
    psllq %xmm2,%xmm0
    #OR the input exponent with the input sign
    por  %xmm1,%xmm5
    #finally OR with the matissa
    por %xmm5,%xmm0
    ret

    #IF exponent is less than 0
.L__number_less_than_zero:
    pand %xmm5,%xmm3 # xmm3 =abs(input) 
    addsd %xmm4,%xmm3# add (2^52 + 1)
    subsd %xmm4,%xmm3# sub (2^52 + 1)
    por %xmm1, %xmm3 # OR with the sign of the input number 
    movsd %xmm3,%xmm0 
    ret

    #IF the input is infinity
.L__is_infinity:
    ucomisd %xmm4,%xmm0
    jnp .L__is_zero #parity flag is raised 
                    #IF one of theinputs is a Nan
.L__is_nan :
    por .L__qnan_mask_64(%rip),%xmm0 # set the QNan Bit
.L__is_zero :
.L__is_greater_than_2p52:    
    ret

.section .rodata

.align 16
.L__sign_mask_64:          .quad 0x7FFFFFFFFFFFFFFF
                           .quad 0

.L__qnan_mask_64:          .quad 0x0008000000000000
                           .quad 0
.L__exp_mask_64:           .quad 0x7FF0000000000000
                           .quad 0
.L__mantissa_mask_64:      .quad 0x000FFFFFFFFFFFFF
                           .quad 0
.L__zero:                  .quad 0x0000000000000000
                           .quad 0
.L__2p52_plus_one:         .quad 0x4330000000000001  # = 4503599627370497.0
                           .quad 0
.L__zero_point_5:          .quad 0x3FE0000000000001  # = 00.5
                           .quad 0



