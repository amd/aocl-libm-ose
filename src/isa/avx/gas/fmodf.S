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

# fmodf.S
#
# An implementation of the fabs libm function.
#
# Prototype:
#
#     float fmodf(float x,float y);
#

#
#   Algorithm:
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname ALM_PROTO_BAS64(fmodf)
#define fname_special _fmodf_special@PLT



#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function

fname:
    mov .L__exp_mask_64(%rip), %rdi
    movapd .L__sign_mask_64(%rip),%xmm6
    movd     %xmm0,%r8d
    movd     %xmm1,%r9d
    and      .L__NotSign_mask(%rip),%r8d 
    and      .L__NotSign_mask(%rip),%r9d 
    jz       .L__Y_is_Zero
    cmp      .L__exp_mask_32(%rip),%r8d
    jge      .L__InputXIsNaN_Inf
    cmp      .L__exp_mask_32(%rip),%r9d
    jg       .L__InputYIsNaN_Inf
    cmp      %r9d,%r8d
    jz       .L__Input_Is_Equal

    cvtss2sd %xmm0,%xmm2 # double x
    cvtss2sd %xmm1,%xmm3 # double y
    pand %xmm6,%xmm2
    pand %xmm6,%xmm3
    movd %xmm2,%rax
    movd %xmm3,%r8
    mov %rax,%r11
    mov %r8,%r9
    movsd %xmm2,%xmm4
    #take the exponents of both x and y
    and %rdi,%rax
    and %rdi,%r8
    ror $52, %rax
    ror $52, %r8
    # ifeither of the exponents is infinity 
    #cmp $0X7FF,%rax 
    #jz  .L__InputIsNaN 
    #cmp $0X7FF,%r8 
    #jz  .L__InputIsNaNOrInf

    #cmp $0,%r8
    #jz  .L__Divisor_Is_Zero

    cmp %r9, %r11
    #jz  .L__Input_Is_Equal
    jb  .L__ReturnImmediate

    xor %rcx,%rcx
    mov $24,%rdx
    movsd .L__One_64(%rip),%xmm7 # xmm7 = scale
    cmp %rax,%r8 
    jae .L__y_is_greater
    #xmm3 = dy
    sub %r8,%rax
    div %dl       # al = ntimes
    mov %al,%cl   # cl = ntimes
    and $0xFF,%ax # set everything t o zero except al
    mul %dl       # ax = dl * al = 24* ntimes
    add $1023, %rax
    shl $52,%rax
    movd %rax,%xmm7 # xmm7 = scale
.align 16
.L__y_is_greater:
    mulsd %xmm3,%xmm7 # xmm7 = scale * dy
    movsd .L__2pminus24_decimal(%rip),%xmm6

.align 16
.L__Start_Loop:
    dec %cl
    js .L__End_Loop
    divsd %xmm7,%xmm4     # xmm7 = (dx / w)
    cvttsd2siq %xmm4,%rax 
    cvtsi2sdq %rax,%xmm4  # xmm4 = t = (double)((int)(dx / w))
    mulsd  %xmm7,%xmm4    # xmm4 = w*t
    mulsd %xmm6,%xmm7     # w*= scale 
    subsd  %xmm4,%xmm2    # xmm2 = dx -= w*t  
    movsd %xmm2,%xmm4     # xmm4 = dx
    jmp .L__Start_Loop

.align 16
.L__End_Loop:    
    divsd %xmm7,%xmm4     # xmm7 = (dx / w)
    cvttsd2siq %xmm4,%rax 
    cvtsi2sdq %rax,%xmm4   # xmm4 = t = (double)((int)(dx / w))
    mulsd  %xmm7,%xmm4    # xmm4 = w*t
    subsd  %xmm4,%xmm2    # xmm2 = dx -= w*t  
    comiss .L__Zero_64(%rip),%xmm0 
    jb .L__Negative

.align 16
.L__Positive:
    cvtsd2ss %xmm2,%xmm0 
    ret

.align 16
.L__Negative:
    movsd .L__MinusZero_64(%rip),%xmm0
    subsd %xmm2,%xmm0
    cvtsd2ss %xmm0,%xmm0 
    ret

.align 16
.L__Input_Is_Equal:
    #cmp $0x7FF,%rax
    #jz .L__Dividend_Is_Infinity
    #cmp $0x7FF,%r8
    #jz .L__InputIsNaNOrInf
    movsd %xmm0,%xmm1
    pand .L__sign_bit_32(%rip),%xmm1
    movss .L__Zero_64(%rip),%xmm0
    por  %xmm1,%xmm0
    ret

.align 16
.L__InputXIsNaN_Inf:
    je     .L__Dividend_Is_Infinity
    cmp    .L__exp_mask_32(%rip),%r9d
    jle    .L__InputXIsNaN
    cmp    .L__Qnan(%rip),%r9d
    jl     .L__InputYIsSnan
.align 16
.L__InputXIsNaN:    
    movaps %xmm0,%xmm1
    por    .L__Qnan(%rip),%xmm1
    mov    .L__flag_x_nan(%rip),%edi
    call   fname_special
    ret

#Here x is nan so we move to xmm1
.align 16
.L__InputYIsNaN_Inf:
    #Here y is nan
    cmp    .L__Qnan(%rip),%r9d # r9d contains 
    jl     .L__InputYIsSnan
    movaps %xmm1,%xmm0
    por    .L__Qnan(%rip),%xmm1
    mov    .L__flag_x_nan(%rip),%edi
    call   fname_special
    ret 

.align 16
.L__InputYIsSnan:
    movaps %xmm1,%xmm0
    por    .L__Qnan(%rip),%xmm1
    mov    .L__flag_x_nan(%rip),%edi
    call   fname_special
    ret

.align 16
.L__Dividend_Is_Infinity:
    cmp    .L__exp_mask_32(%rip),%r9d
    jg       .L__InputYIsNaN_Inf    
    movapd %xmm0,%xmm1
    por    .L__Qnan(%rip),%xmm1
    mov    .L__flag_x_Dividend_Inf(%rip),%edi
    call   fname_special
    ret

#Case when x < y
.align 16
.L__ReturnImmediate:
    #xmm0 contains the input and is the result
    ret

.align 16
.L__Y_is_Zero:
       movaps .L__Qnan(%rip),%xmm1
       mov    .L__flag_y_zero(%rip),%edi
	   call   fname_special
       ret 
           
 
.section .rodata

.align 16    
.L__flag_x_nan:            .long 00000001
.L__flag_y_zero:           .long 00000002
.L__flag_x_Dividend_Inf:   .long 00000003



.align 32    
.L__sign_bit_32:           .quad 0x8000000080000000
                           .quad 0x0
.L__exp_mask_64:           .quad 0x7FF0000000000000
                           .quad 0x0
.L__exp_mask_32:           .quad 0x000000007F800000
                           .quad 0x0
.L__27bit_andingmask_64:   .quad 0xfffffffff8000000
                           .quad 0
.L__2p52_mask_64:          .quad 0x4330000000000000 
                           .quad 0
.L__One_64:                .quad 0x3FF0000000000000 
                           .quad 0
.L__Zero_64:               .quad 0x0 
                           .quad 0
.L__MinusZero_64:          .quad 0x8000000000000000 
                           .quad 0
.L__QNaN_mask_32:          .quad 0x0000000000400000
                           .quad 0
.L__Qnan:                  .quad 0x7fc000007fc00000
                           .quad 0x7fc000007fc00000
.L__sign_mask_64:          .quad 0x7FFFFFFFFFFFFFFF
                           .quad 0
.L__2pminus24_decimal:     .quad 0x3E70000000000000
                           .quad 0x0
.L__NotSign_mask:          .quad 0x7FFFFFFF7FFFFFFF
                           .quad 0x7FFFFFFF7FFFFFFF


