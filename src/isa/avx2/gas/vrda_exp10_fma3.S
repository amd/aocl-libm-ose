#
# Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#define fname FN_PROTOTYPE_FMA3(vrda_exp10)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    cmp         $0x0, %edi
    jle         .L__return    
    cmp         $0x0, %rsi
    je         .L__return
    cmp         $0x0, %rdx
    je         .L__return
    

.p2align 4    
.L__process_next2:
    sub         $0x2, %rdi
    cmp         $-1,  %rdi
    jle          .L__process_first1
    movdqu      (%rsi, %rdi, 8), %xmm0
    jmp         .L__start
    
.p2align 4    
.L__process_first1:
    jl          .L__return #even elements, all are processed %rdi == -2
    mov         $0x0, %rcx
    movsd       (%rsi, %rcx, 8), %xmm0    
    
.p2align 4    
.L__start:    
    vmovdqa      %xmm0, %xmm10   # save for later use
    
    # x * (64/log10(2))
    vmulpd       .L__real_64_by_log10of2(%rip), %xmm0,%xmm3    

    # n = int( x * (64/log10(2)) )
    vcvttpd2dq  %xmm3, %xmm4   #xmm4 = 0,0,(int)n1,(int)n0
    vcvtdq2pd   %xmm4, %xmm3   #xmm3 = (double)n1,n0    
    vmovd       %xmm4, %rcx    #rcx = (int)n1,n0
    
    # r1 = x - n * log10(2)/64 head    
    # r2 = - n * log10(2)/64 tail    
    # r1 *= ln10;# r2 *= ln10;
    # r = r1+r2 
    vinsertf128 $1,%xmm3,%ymm3,%ymm3 #ymm3 = (double)n1,n0,n1,n0
    vpor     %xmm0,%xmm0,%xmm0       #ymm0 = zero in upper part
    vfmadd132pd .L__log10of2_by_64_mtail_mhead(%rip),%ymm0,%ymm3
    vmulpd   .L__ln10(%rip),%ymm3,%ymm3
    vextractf128 $1,%ymm3,%xmm1
    vaddpd   %xmm3,%xmm1,%xmm2
    
    #j = n & 0x3f    
    mov         $0x0000003f0000003f, %rax
    and         %rcx, %rax     #rax = j1,j0
    mov         %eax, %ecx     #rcx = j0
    shr         $32, %rax      #rax = j1        
    # m = (n - j) / 64      
    vpsrad         $6, %xmm4,%xmm5      #xmm5 = 0,0,m1,m0

    # q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    # q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    vmovapd    .L__real_1_by_720(%rip),%xmm3
     vfmadd213pd   .L__real_1_by_120(%rip),%xmm2,%xmm3
     vfmadd213pd   .L__real_1_by_24(%rip),%xmm2,%xmm3
     vfmadd213pd   .L__real_1_by_6(%rip),%xmm2,%xmm3
     vfmadd213pd   .L__real_1_by_2(%rip),%xmm2,%xmm3
     vmulpd     %xmm2,%xmm2,%xmm0
     vfmadd213pd   %xmm2,%xmm3,%xmm0 
    
    # (f)*(q) + f2 + f1
    lea         .L__two_to_jby64_table(%rip), %r9        
    lea         .L__two_to_jby64_tail_table(%rip), %r11       
    lea         .L__two_to_jby64_head_table(%rip), %r10    
    vmovsd      (%r9,%rcx,8), %xmm2
    vmovhpd     (%r9,%rax,8),%xmm2,%xmm1  #xmm1 = f_1,f_0    
    vmovsd      (%r11,%rcx,8), %xmm2    
    vmovhpd     (%r11,%rax,8), %xmm2,%xmm3 #xmm3 = f2_1,f2_0
    vmovsd      (%r10,%rcx,8), %xmm4
    vmovhpd     (%r10,%rax,8), %xmm4,%xmm2 #xmm2 = f1_1,f1_0           
     vfmadd213pd    %xmm3,%xmm1,%xmm0    #xmm0 = f*q + f2
    vaddpd      %xmm0,%xmm2,%xmm0   

    # normal results
    vpmovsxdq   %xmm5,%xmm4       #xmm4 = m1,m0
    vpsllq      $52,%xmm4,%xmm5   #xmm5 = 2^m1,2^m0
    vpaddq      %xmm5,%xmm0,%xmm1 #xmm1 = normal results
  
    #check and process denormals    
    vcmppd      $1,.L__real_one(%rip),%xmm0,%xmm3 #check e^r < 1
    vpcmpeqq    .L__m1022(%rip),%xmm5,%xmm2   #check m == -1022
    vpand       %xmm2,%xmm3,%xmm3  #xmm3 = ( e^r < 1 and m = -1022 )     
	
   # vpcomltq    .L__m1022(%rip),%xmm5,%xmm2   #check m < -1022    
     vmovdqa  .L__m1022(%rip),%xmm2
     vpcmpgtq %xmm5,%xmm2,%xmm2  #check m < -1022
   
    vpor        %xmm3,%xmm2,%xmm2   #xmm2 = mask for inputs requiring denormal processing
    vpmovmskb   %xmm2,%eax
    cmp         $0,%eax
    je          .L__check_min   #jmp to avoid any accidental intermediate denormal results
    vpaddq      .L__ulong_1074(%rip),%xmm4,%xmm3 #xmm3 = 1074 + m     
	
	vunpckhpd %xmm3,%xmm3,%xmm9 #store high to xmm9
	vmovdqa .L__ulong_1(%rip),%xmm5
	vpsllq %xmm3,%xmm5,%xmm4
	vpsllq %xmm9,%xmm5,%xmm9
	vunpcklpd %xmm9,%xmm4,%xmm4
	vmulpd %xmm4,%xmm0,%xmm3 #xmm3 = results for denormal numbers
	vandnpd %xmm1,%xmm2,%xmm5
	vandpd  %xmm2,%xmm3,%xmm1
	vorpd   %xmm5,%xmm1,%xmm1
    
    
.L__check_min:    
    ##special case for any x < min_exp_arg,remove this code if the above code takes care of this
    vcmppd       $2,.L__min_exp10_arg(%rip),%xmm10,%xmm2 #cmp x<= min_exp_arg
    vpxor        %xmm3,%xmm3,%xmm4  #make zeros to put in place of x<= min_exp_arg
    #vpcmov       %xmm2,%xmm1,%xmm4,%xmm1 #put zeros in place of any x <= min_exp_arg 
    VANDNPD %xmm1, %xmm2, %xmm5
    VANDPD %xmm2, %xmm4, %xmm1
    VORPD %xmm5, %xmm1, %xmm1
    
        
    ##special case for any x > max_exp_arg,remove this code if the above code takes care of this
    vcmppd       $1,.L__max_exp10_arg(%rip),%xmm10,%xmm2 #cmp x < max_exp_arg
    vmovdqa     .L__real_inf(%rip), %xmm3  #inf to put in place of any x >= max_exp_arg
    #vpcmov       %xmm2,%xmm3,%xmm1,%xmm1 #xmm1 = normal results
    VANDPD %xmm2, %xmm1, %xmm1
    VANDNPD %xmm3, %xmm2, %xmm5
    VORPD %xmm5, %xmm1, %xmm1
    
    ##special case for any x = nan,remove this code if the above code takes care of this
    vcmppd       $0x0,%xmm10,%xmm10,%xmm2    
    vaddpd       %xmm10, %xmm10,%xmm3  #make qnan to put in place of any x =nan
    #vpcmov       %xmm2,%xmm3,%xmm1,%xmm0 #xmm0 = normal results    
    
     VANDPD %xmm2, %xmm1, %xmm0
        VANDNPD %xmm3, %xmm2, %xmm5
    VORPD %xmm5, %xmm0, %xmm0
    
    
    
    

    cmp         $-1,  %rdi
    je          .L__store1
    movdqu       %xmm0, (%rdx, %rdi, 8)
    jmp         .L__process_next2    
    
.p2align 4    
.L__store1:
    inc        %rdi
    movsd      %xmm0, (%rdx,%rdi,8)
    
.L__return:    
    ret         
    
.section .rodata
.align 16
.L__max_exp10_arg:          .octa 0x40734413509f79ff40734413509f79ff
.L__min_exp10_arg:          .octa 0xc07434e6420f4374c07434e6420f4374
.L__real_64_by_log10of2:    .octa 0x406A934F0979A371406A934F0979A371    # 64/log10(2)
.L__ln10:                   .octa 0x40026BB1BBB5551640026BB1BBB55516
                            .octa 0x40026BB1BBB5551640026BB1BBB55516
.L__log10of2_by_64_mtail_mhead: .octa 0xbF73441350000000bF73441350000000
                                .octa 0xbda3ef3fde623e25bda3ef3fde623e25
.L__real_one:             .octa 0x3ff00000000000003ff0000000000000
.L__real_inf:             .octa 0x7ff00000000000007ff0000000000000
.L__m1022:                        .octa 0xc020000000000000c020000000000000
.L__ulong_1074:                   .octa 0x00000000000004320000000000000432
.L__ulong_1:                      .octa 0x00000000000000010000000000000001
