#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#include "fn_macros.h"
#include "exp_tables.h"

#define fname ALM_PROTO_BAS64(vrd2_expm1)

#ifdef __ELF__
    .section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    movdqa       %xmm0, %xmm10   # save for later use
    # x * (64/ln(2))
    movapd      %xmm0,%xmm1        
    mulpd       .L__real_64_by_log2(%rip), %xmm1

    # n = int( x * (64/ln(2)) )
    cvttpd2dq    %xmm1, %xmm2   #xmm2 = (int)n1,n0
    cvtdq2pd    %xmm2, %xmm1   #xmm1 = (double)n1,n0
    movd        %xmm2, %rcx    #rcx = (int)n1,n0
    movapd      %xmm2, %xmm12  #xmm12 = (int)n1,n0
    movapd     %xmm1,%xmm2     #xmm2 = (double)n1,n0
    
    # r1 = x - n * ln(2)/64 head    
    mulpd    .L__log2_by_64_mhead(%rip),%xmm1
        
    #j = n & 0x3f    
    mov         $0x0000003f0000003f, %rax
    and         %rcx, %rax     #rax = j1,j0
    mov         %rax, %r8
    shl         $32, %rax
    shr         $32, %rax      #rax = j0
    shr         $32, %r8      #r8 = j1
        
    # m = (n - j) / 64      
    psrad         $6, %xmm12      #xmm12 = m1,m0            

    # r2 = - n * ln(2)/64 tail
    mulpd    .L__log2_by_64_mtail(%rip),%xmm2 #xmm2 = r21,r20
    addpd    %xmm1,%xmm0   #xmm0 = r11,r10

    # r1+r2
    addpd       %xmm0, %xmm2 #xmm2 = r

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd       .L__real_1_by_720(%rip), %xmm3  #xmm3 = 1/720
    mulpd       %xmm2, %xmm3    #xmm3 = r*1/720
    movapd       .L__real_1_by_6(%rip), %xmm0    #xmm0 = 1/6    
    movapd      %xmm2, %xmm1 #xmm1 = r            
    mulpd       %xmm2, %xmm0    #xmm0 = r*1/6
    addpd       .L__real_1_by_120(%rip), %xmm3  #xmm3 = 1/120 + (r*1/720)
    mulpd       %xmm2, %xmm1    #xmm1 = r*r    
    addpd       .L__real_1_by_2(%rip), %xmm0  #xmm0 = 1/2 + (r*1/6)        
    movapd       %xmm1, %xmm4   #xmm4 = r*r
    mulpd       %xmm1, %xmm4    #xmm4 = (r*r) * (r*r)    
    mulpd       %xmm2, %xmm3    #xmm3 = r * (1/120 + (r*1/720))
    mulpd       %xmm1, %xmm0    #xmm0 = (r*r)*(1/2 + (r*1/6))
    addpd       .L__real_1_by_24(%rip), %xmm3  #xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addpd       %xmm2, %xmm0   #xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulpd       %xmm4, %xmm3   #xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addpd       %xmm3, %xmm0   #xmm0 = q= r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    
    #seperate processing for each element
    movhlps  %xmm0, %xmm11  #save higher part
    movd     %xmm12, %rcx     #rcx = m1,m0        
    mov      $2, %r9d
    jmp      .L__next

.p2align 4    
.L__check_n_prepare_next:
    dec      %r9d
    jz       .L__ret
    movlhps  %xmm0,%xmm11  # save previous result
    movdqa   %xmm11, %xmm0 #xmm0 = q1
    psrldq   $8,%xmm10     #next x
    mov      %r8,%rax
    movd     %xmm12, %rcx
    shr      $32,%rcx     #rcx = m1    

.p2align 4    
.L__next:
    ucomisd .L__max_expm1_arg(%rip),%xmm10  #check if(x > 709.8)
    ja .L__y_is_inf
    jp .L__y_is_nan  
    ucomisd .L__min_expm1_arg(%rip),%xmm10  #if(x < -37.42994775023704)
    jb .L__y_is_minusOne
    ucomisd .L__log_OneMinus_OneByFour(%rip),%xmm10
    jbe .L__Normal_Flow
    ucomisd .L__log_OnePlus_OneByFour(%rip),%xmm10
    jb .L__Small_Arg 
    
.p2align 4        
.L__Normal_Flow:
    
    # load f, f1,f2
    lea         .L__two_to_jby64_table(%rip), %r10        
    movsd       (%r10,%rax,8), %xmm1   #xmm1 = f    
    lea         .L__two_to_jby64_tail_table(%rip), %r11    
    lea         .L__two_to_jby64_head_table(%rip), %r10
    movsd       (%r10,%rax,8), %xmm2   #xmm2 = f1  
    movsd       (%r11,%rax,8), %xmm3   #xmm3 = f2
            
    #twopmm.u64 = (1023 - (long)m) << 52;
    mov $1023,%eax
    sub %ecx,%eax
    shl $52, %rax #rax = twopmm
    
    cmp $52,%ecx        #check m > 52
    jg .L__M_Above_52
    cmp $-7,%ecx        #check if m < -7
    jl .L__M_Below_Minus7
    #(-8 < m) && (m < 53)
    movapd %xmm0, %xmm1        #xmm1 = q
    addsd .L__One(%rip),%xmm1  #xmm1 = 1+q
    mulsd %xmm3,%xmm1          #xmm1 = f2.f64 *(1+q)    
    mulsd %xmm2,%xmm0          #xmm0 = f1.f64*q
    addsd %xmm1,%xmm0          #xmm0 = (f1.f64*q+ f2.f64 *(1+q))    
    movd  %rax, %xmm1          #xmm1 = twopmm
    subsd %xmm1,%xmm2          #xmm2 = f1 - twopmm
    addsd %xmm2,%xmm0
    shl   $52,%rcx             #rcx = 2^m,m at exponent
    movd  %rcx, %xmm1
    paddq %xmm1,%xmm0
    jmp  .L__check_n_prepare_next   
    
    .p2align 4  
.L__M_Above_52:
    cmp $1024,%ecx #check if m = 1024
    je .L__M_Equals_1024
    #twopm.f64 * (f1.f64 + (f*q+(f2.f64 - twopmm.f64)));// 2^-m should not be calculated if m>105
    movd %rax,%xmm4            #xmm4 = twopmm
    subsd %xmm4,%xmm3  #xmm3 = f2 - twopmm
    mulsd %xmm1,%xmm0  #xmm0 = f*q
    addsd %xmm3,%xmm0  #xmm0 = (f*q+(f2.f64 - twopmm.f64)))
    addsd %xmm2,%xmm0  #xmm0 = (f1.f64 + (f*q+(f2.f64 - twopmm.f64)))
    shl $52,%rcx
    movd %rcx,%xmm1    #xmm1 = twopm
    paddq %xmm1,%xmm0
    jmp  .L__check_n_prepare_next   
      
    .p2align 4    
.L__M_Below_Minus7:
    #twopm.f64 * (f1.f64 + (f*q + f2.f64)) - 1;
    mulsd %xmm1,%xmm0   #xmm0 = f*q
    addsd %xmm3,%xmm0   #xmm0 = (f*q + f2.f64)
    addsd %xmm2,%xmm0   #xmm0 = f1 + (f*q + f2.f64)
    shl $52,%rcx
    movd %rcx,%xmm1     #xmm1 = twopm
    paddq %xmm1,%xmm0   #xmm0 = twopm *(xmm0)
    subsd .L__One(%rip),%xmm0    
    jmp  .L__check_n_prepare_next   
    
    .p2align 4
.L__M_Equals_1024:
    mov $0x4000000000000000,%rax #1024 at exponent
    mulsd %xmm1,%xmm0 #xmm0 = f*q
    addsd %xmm3,%xmm0 #xmm0 = (f*q) + f2
    addsd %xmm2,%xmm0 #xmm0 = f1 + (f*q) + f2
    movd %rax,%xmm1   #xmm1 = twopm
    paddq %xmm1,%xmm0
    movd %xmm0,%rax
    mov $0x7FF0000000000000,%rcx
    and %rcx,%rax
    cmp %rcx,%rax #check if we reached inf
    je .L__y_is_inf
    jmp  .L__check_n_prepare_next   
    
    .p2align 4
.L__Small_Arg:
    movapd  %xmm10,%xmm0
    ucomisd .L__minus_zero(%rip),%xmm0
    je  .L__check_n_prepare_next
    mov $0x01E0000000000000,%rax #30 in exponents place
    #u = (twop30.f64 * x + x) - twop30.f64 * x;    
    movd %rax,%xmm1
    paddq %xmm0,%xmm1  #xmm1 = twop30.f64 * x
    movapd %xmm1,%xmm2
    addsd %xmm0,%xmm2 #xmm2 = (twop30.f64 * x + x)
    subsd %xmm1,%xmm2 #xmm2 = u
    movapd %xmm0,%xmm1
    subsd %xmm2,%xmm1 #xmm1 = v = x-u
    movapd %xmm2,%xmm3 #xmm3 = u
    mulsd %xmm2,%xmm3 #xmm3 = u*u
    mulsd .L__real_1_by_2(%rip),%xmm3 #xmm3 = y = u*u*0.5
    #z = v * (x + u) * 0.5;
    movapd %xmm0,%xmm4
    addsd %xmm2,%xmm4
    mulsd %xmm1,%xmm4
    mulsd .L__real_1_by_2(%rip),%xmm4 #xmm4 = z   
    
    #q = x*x*x*(A1.f64 + x*(A2.f64 + x*(A3.f64 + x*(A4.f64 + x*(A5.f64 + x*(A6.f64 + x*(A7.f64 + x*(A8.f64 + x*(A9.f64)))))))));
    movapd %xmm0,%xmm5
    mulsd .L__B9(%rip),%xmm5
    addsd .L__B8(%rip),%xmm5
    mulsd %xmm0,%xmm5
    addsd .L__B7(%rip),%xmm5
    mulsd %xmm0,%xmm5
    addsd .L__B6(%rip),%xmm5
    mulsd %xmm0,%xmm5           
    addsd .L__B5(%rip),%xmm5
    mulsd %xmm0,%xmm5    
    addsd .L__B4(%rip),%xmm5
    mulsd %xmm0,%xmm5    
    addsd .L__B3(%rip),%xmm5
    mulsd %xmm0,%xmm5    
    addsd .L__B2(%rip),%xmm5
    mulsd %xmm0,%xmm5   
    addsd .L__B1(%rip),%xmm5
    mulsd %xmm0,%xmm5  
    mulsd %xmm0,%xmm5  
    mulsd %xmm0,%xmm5   #xmm5 = q 
     
    ucomisd .L__TwopM7(%rip),%xmm3    
    jb .L__returnNext
    addsd %xmm4,%xmm1  #xmm1 = v+z
    addsd %xmm5,%xmm1  #xmm1 = q+(v+z)
    addsd %xmm3,%xmm2  #xmm2 = u+y
    addsd %xmm2,%xmm1
    movapd %xmm1,%xmm0
    jmp  .L__check_n_prepare_next
.p2align 4
.L__returnNext:
    addsd %xmm5,%xmm4  #xmm4 = q +z
    addsd %xmm4,%xmm3  #xmm3 = y+(q+z)
    addsd %xmm3,%xmm0    
    jmp  .L__check_n_prepare_next
    
.p2align 4
.L__y_is_inf:
    mov         $0x7ff0000000000000,%rax
    movd       %rax, %xmm0
    jmp  .L__check_n_prepare_next

.p2align 4
.L__y_is_nan:
    movapd      %xmm10,%xmm0
    addsd       %xmm0,%xmm0
    jmp  .L__check_n_prepare_next

.p2align 4
.L__y_is_minusOne:
    mov $0xBFF0000000000000,%rax   #return -1
    movd %rax, %xmm0
    jmp  .L__check_n_prepare_next

.p2align 4
.L__ret:
    movdqa %xmm0,%xmm1
    movhlps %xmm11,%xmm0
    movlhps %xmm1,%xmm0    
    ret   
         
.section .rodata
.align 16
.L__max_expm1_arg:           .quad 0x40862E6666666666    
.L__min_expm1_arg:           .quad 0xC042B708872320E1
.L__log_OneMinus_OneByFour:  .quad 0xBFD269621134DB93
.L__log_OnePlus_OneByFour:   .quad 0x3FCC8FF7C79A9A22
.L__real_64_by_log2:         .octa 0x40571547652b82fe40571547652b82fe    # 64/ln(2)
.L__log2_by_64_mhead:        .octa 0xbf862e42fefa0000bf862e42fefa0000
.L__log2_by_64_mtail:        .octa 0xbd1cf79abc9e3b39bd1cf79abc9e3b39
.L__TwopM7:                  .quad 0x3F80000000000000
.L__One:                     .quad 0x3FF0000000000000
.L__minus_zero:              .quad 0x8000000000000000
.L__B9:                      .quad 0x3E5A2836AA646B96
.L__B8:                      .quad 0x3E928295484734EA
.L__B7:                      .quad 0x3EC71E14BFE3DB59
.L__B6:                      .quad 0x3EFA019F635825C4
.L__B5:                      .quad 0x3F2A01A01159DD2D
.L__B4:                      .quad 0x3F56C16C16CE14C6
.L__B3:                      .quad 0x3F8111111111A9F3
.L__B2:                      .quad 0x3FA55555555554B6
.L__B1:                      .quad 0x3FC5555555555549
