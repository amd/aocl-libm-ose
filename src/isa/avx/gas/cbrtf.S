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

# cbrtf.S
#
# An implementation of the cbrtf libm function.
#
# Prototype:
#
#     float cbrtf(float x);
#

#
#   Algorithm:
#

#include "fn_macros.h"
#define fname ALM_PROTO_BAS64(cbrtf)


# local variable storage offsets

.equ   store_input, 0x0 
.equ   stack_size, 0x20 


#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 32
.p2align 4,,15
.globl fname
.type fname,@function
fname:
    xor   %rcx,%rcx
    sub   $stack_size, %rsp
    movss %xmm0, store_input(%rsp)
    movss %xmm0,%xmm1
    mov   store_input(%rsp),%r8
    mov   $0x7F800000,%r10
    mov   $0x007FFFFF,%r11
    mov   %r8,%r9
    and   %r10,%r8 # r8 = stores the exponent
    and   %r11,%r9 # r9 = stores the mantissa
    cmp   $0X7F800000,%r8
    jz    .L__cbrtf_is_nan_infinite
    cmp   $0X0,%r8
    jz    .L__cbrtf_is_denormal
.align 32
.L__cbrtf_is_normal:   
    cvtps2pd %xmm1,%xmm1
    shr   $23,%r8  # exp value
    mov   $3,%rdx # check whether always dx is set to 3
    mov   %r8,%rax
    movsd %xmm1,%xmm6
    shr   $15,%r9  # index for the reciprocal
    sub   $0x7F,%ax
    idiv  %dl # Accumulator is divided by dl=3
    mov   %ax,%dx
    shr   $8,%dx #dx = Contains the remainder
    add   $2,%dl
                 #ax = Contains the quotient, Scale factor
    cbw          # sign extend al to ax
    add   $0x3FF,%ax
    shl   $52,%rax
    pand .L__mantissa_mask_64(%rip),%xmm1    
    mov   %rax,store_input(%rsp)
    movsd store_input(%rsp),%xmm7
    movsd  .L__sign_mask_64(%rip),%xmm2
    por .L__one_mask_64(%rip),%xmm1
    movapd .L__coefficients(%rip),%xmm0
    pandn %xmm1,%xmm2
    pand  .L__sign_mask_64(%rip),%xmm6 # has the sign
    lea .L__DoubleReciprocalTable_256(%rip),%r8
    lea .L__CubeRootTable_256(%rip),%rax
    movsd (%r8,%r9,8),%xmm3#reciprocal, Size of double is 8
    movsd (%rax,%r9,8),%xmm4#cuberoot
    mulsd %xmm2,%xmm3
    subsd .L__one_mask_64(%rip),%xmm3
    
    # movddup %xmm3,%xmm3
    shufpd $0,%xmm3,%xmm3 # replacing movddup
    
    mulsd %xmm3,%xmm3
    mulpd %xmm3,%xmm0
#######################################################################    
#haddpd is an SSE3 instruction On using this instruction it gives a better performance    
    #haddpd %xmm0,%xmm0
#Following has to be commented and the above haddpd has to be uncommented if we can
#use the SSE3 instructions
    movapd %xmm0,%xmm3
    unpckhpd %xmm3,%xmm3
    addsd %xmm3,%xmm0
#######################################################################    
    addsd .L__one_mask_64(%rip),%xmm0
    mulsd %xmm7,%xmm0
    lea .L__defined_cuberoot(%rip),%rax
    mulsd (%rax,%rdx,8),%xmm0
    
    mulsd %xmm4,%xmm0
    cmp $1,%cx
    jnz .L__final_result
    mulsd .L__denormal_factor(%rip),%xmm0

.align 32
.L__final_result:
    por %xmm6, %xmm0 
    cvtsd2ss %xmm0,%xmm0
    add   $stack_size, %rsp
    ret
     
         
.align 32
.L__cbrtf_is_denormal:   
    cmp $0,%r9
    jz .L__cbrtf_is_zero
    mulss .L__2_pow_23(%rip),%xmm1
    movss  %xmm1, store_input(%rsp)
    mov   $1,%cx
    mov   store_input(%rsp),%r8
    mov   %r8,%r9
    and   %r10,%r8 # r8 = stores the exponent
    and   %r11,%r9 # r9 = stores the mantissa
    jmp .L__cbrtf_is_normal 

.align 32
.L__cbrtf_is_nan_infinite:
    cmp $0,%r9
    jz .L__cbrtf_is_infinite
    addss %xmm0,%xmm0 #this multiplication will raise an invalid exception
    por  .L__qnan_mask_32(%rip),%xmm0

.L__cbrtf_is_infinite:
.L__cbrtf_is_one:    
.L__cbrtf_is_zero:    
    add   $stack_size, %rsp
    ret

.section .rodata
.align 32 
.L__mantissa_mask_32:      .long 0x007FFFFF
                           .long 0          #this zero is necessary
.align 16                           
.L__qnan_mask_32:          .long 0x00400000
                           .long 0
.L__exp_mask_32:           .long 0x7F800000
                           .long 0
.L__zero:                  .long 0x00000000
                           .long 0
.align 16
.L__mantissa_mask_64:      .quad 0x000FFFFFFFFFFFFF
.L__2_pow_23:              .long 0x4B000000


.align 16
.L__sign_mask_64:          .quad 0x8000000000000000 
                           .quad 0
.L__one_mask_64:           .quad 0x3FF0000000000000 
                           .quad 0

.align 16
.L__denormal_factor:       .quad 0x3F7428A2F98D728B 
                           .quad 0
.align 16
.L__coefficients:
    .quad 0xbFBC71C71C71C71C
    .quad 0x3fd5555555555555
.align 16
.L__defined_cuberoot:   .quad 0x3FE428A2F98D728B
                        .quad 0x3FE965FEA53D6E3D
                        .quad 0x3FF0000000000000
                        .quad 0x3FF428A2F98D728B
                        .quad 0x3FF965FEA53D6E3D
                         
.align 32 
.L__DoubleReciprocalTable_256: .quad 0X3ff0000000000000
            .quad 0X3fefe00000000000
            .quad 0X3fefc00000000000
            .quad 0X3fefa00000000000
            .quad 0X3fef800000000000
            .quad 0X3fef600000000000
            .quad 0X3fef400000000000
            .quad 0X3fef200000000000
            .quad 0X3fef000000000000
            .quad 0X3feee00000000000
            .quad 0X3feec00000000000
            .quad 0X3feea00000000000
            .quad 0X3fee900000000000
            .quad 0X3fee700000000000
            .quad 0X3fee500000000000
            .quad 0X3fee300000000000
            .quad 0X3fee100000000000
            .quad 0X3fee000000000000
            .quad 0X3fede00000000000
            .quad 0X3fedc00000000000
            .quad 0X3feda00000000000
            .quad 0X3fed900000000000
            .quad 0X3fed700000000000
            .quad 0X3fed500000000000
            .quad 0X3fed400000000000
            .quad 0X3fed200000000000
            .quad 0X3fed000000000000
            .quad 0X3fecf00000000000
            .quad 0X3fecd00000000000
            .quad 0X3fecb00000000000
            .quad 0X3feca00000000000
            .quad 0X3fec800000000000
            .quad 0X3fec700000000000
            .quad 0X3fec500000000000
            .quad 0X3fec300000000000
            .quad 0X3fec200000000000
            .quad 0X3fec000000000000
            .quad 0X3febf00000000000
            .quad 0X3febd00000000000
            .quad 0X3febc00000000000
            .quad 0X3feba00000000000
            .quad 0X3feb900000000000
            .quad 0X3feb700000000000
            .quad 0X3feb600000000000
            .quad 0X3feb400000000000
            .quad 0X3feb300000000000
            .quad 0X3feb200000000000
            .quad 0X3feb000000000000
            .quad 0X3feaf00000000000
            .quad 0X3fead00000000000
            .quad 0X3feac00000000000
            .quad 0X3feaa00000000000
            .quad 0X3fea900000000000
            .quad 0X3fea800000000000
            .quad 0X3fea600000000000
            .quad 0X3fea500000000000
            .quad 0X3fea400000000000
            .quad 0X3fea200000000000
            .quad 0X3fea100000000000
            .quad 0X3fea000000000000
            .quad 0X3fe9e00000000000
            .quad 0X3fe9d00000000000
            .quad 0X3fe9c00000000000
            .quad 0X3fe9a00000000000
            .quad 0X3fe9900000000000
            .quad 0X3fe9800000000000
            .quad 0X3fe9700000000000
            .quad 0X3fe9500000000000
            .quad 0X3fe9400000000000
            .quad 0X3fe9300000000000
            .quad 0X3fe9200000000000
            .quad 0X3fe9000000000000
            .quad 0X3fe8f00000000000
            .quad 0X3fe8e00000000000
            .quad 0X3fe8d00000000000
            .quad 0X3fe8b00000000000
            .quad 0X3fe8a00000000000
            .quad 0X3fe8900000000000
            .quad 0X3fe8800000000000
            .quad 0X3fe8700000000000
            .quad 0X3fe8600000000000
            .quad 0X3fe8400000000000
            .quad 0X3fe8300000000000
            .quad 0X3fe8200000000000
            .quad 0X3fe8100000000000
            .quad 0X3fe8000000000000
            .quad 0X3fe7f00000000000
            .quad 0X3fe7e00000000000
            .quad 0X3fe7d00000000000
            .quad 0X3fe7b00000000000
            .quad 0X3fe7a00000000000
            .quad 0X3fe7900000000000
            .quad 0X3fe7800000000000
            .quad 0X3fe7700000000000
            .quad 0X3fe7600000000000
            .quad 0X3fe7500000000000
            .quad 0X3fe7400000000000
            .quad 0X3fe7300000000000
            .quad 0X3fe7200000000000
            .quad 0X3fe7100000000000
            .quad 0X3fe7000000000000
            .quad 0X3fe6f00000000000
            .quad 0X3fe6e00000000000
            .quad 0X3fe6d00000000000
            .quad 0X3fe6c00000000000
            .quad 0X3fe6b00000000000
            .quad 0X3fe6a00000000000
            .quad 0X3fe6900000000000
            .quad 0X3fe6800000000000
            .quad 0X3fe6700000000000
            .quad 0X3fe6600000000000
            .quad 0X3fe6500000000000
            .quad 0X3fe6400000000000
            .quad 0X3fe6300000000000
            .quad 0X3fe6200000000000
            .quad 0X3fe6100000000000
            .quad 0X3fe6000000000000
            .quad 0X3fe5f00000000000
            .quad 0X3fe5e00000000000
            .quad 0X3fe5d00000000000
            .quad 0X3fe5c00000000000
            .quad 0X3fe5b00000000000
            .quad 0X3fe5a00000000000
            .quad 0X3fe5900000000000
            .quad 0X3fe5800000000000
            .quad 0X3fe5800000000000
            .quad 0X3fe5700000000000
            .quad 0X3fe5600000000000
            .quad 0X3fe5500000000000
            .quad 0X3fe5400000000000
            .quad 0X3fe5300000000000
            .quad 0X3fe5200000000000
            .quad 0X3fe5100000000000
            .quad 0X3fe5000000000000
            .quad 0X3fe5000000000000
            .quad 0X3fe4f00000000000
            .quad 0X3fe4e00000000000
            .quad 0X3fe4d00000000000
            .quad 0X3fe4c00000000000
            .quad 0X3fe4b00000000000
            .quad 0X3fe4a00000000000
            .quad 0X3fe4a00000000000
            .quad 0X3fe4900000000000
            .quad 0X3fe4800000000000
            .quad 0X3fe4700000000000
            .quad 0X3fe4600000000000
            .quad 0X3fe4600000000000
            .quad 0X3fe4500000000000
            .quad 0X3fe4400000000000
            .quad 0X3fe4300000000000
            .quad 0X3fe4200000000000
            .quad 0X3fe4200000000000
            .quad 0X3fe4100000000000
            .quad 0X3fe4000000000000
            .quad 0X3fe3f00000000000
            .quad 0X3fe3e00000000000
            .quad 0X3fe3e00000000000
            .quad 0X3fe3d00000000000
            .quad 0X3fe3c00000000000
            .quad 0X3fe3b00000000000
            .quad 0X3fe3b00000000000
            .quad 0X3fe3a00000000000
            .quad 0X3fe3900000000000
            .quad 0X3fe3800000000000
            .quad 0X3fe3800000000000
            .quad 0X3fe3700000000000
            .quad 0X3fe3600000000000
            .quad 0X3fe3500000000000
            .quad 0X3fe3500000000000
            .quad 0X3fe3400000000000
            .quad 0X3fe3300000000000
            .quad 0X3fe3200000000000
            .quad 0X3fe3200000000000
            .quad 0X3fe3100000000000
            .quad 0X3fe3000000000000
            .quad 0X3fe3000000000000
            .quad 0X3fe2f00000000000
            .quad 0X3fe2e00000000000
            .quad 0X3fe2e00000000000
            .quad 0X3fe2d00000000000
            .quad 0X3fe2c00000000000
            .quad 0X3fe2b00000000000
            .quad 0X3fe2b00000000000
            .quad 0X3fe2a00000000000
            .quad 0X3fe2900000000000
            .quad 0X3fe2900000000000
            .quad 0X3fe2800000000000
            .quad 0X3fe2700000000000
            .quad 0X3fe2700000000000
            .quad 0X3fe2600000000000
            .quad 0X3fe2500000000000
            .quad 0X3fe2500000000000
            .quad 0X3fe2400000000000
            .quad 0X3fe2300000000000
            .quad 0X3fe2300000000000
            .quad 0X3fe2200000000000
            .quad 0X3fe2100000000000
            .quad 0X3fe2100000000000
            .quad 0X3fe2000000000000
            .quad 0X3fe2000000000000
            .quad 0X3fe1f00000000000
            .quad 0X3fe1e00000000000
            .quad 0X3fe1e00000000000
            .quad 0X3fe1d00000000000
            .quad 0X3fe1c00000000000
            .quad 0X3fe1c00000000000
            .quad 0X3fe1b00000000000
            .quad 0X3fe1b00000000000
            .quad 0X3fe1a00000000000
            .quad 0X3fe1900000000000
            .quad 0X3fe1900000000000
            .quad 0X3fe1800000000000
            .quad 0X3fe1800000000000
            .quad 0X3fe1700000000000
            .quad 0X3fe1600000000000
            .quad 0X3fe1600000000000
            .quad 0X3fe1500000000000
            .quad 0X3fe1500000000000
            .quad 0X3fe1400000000000
            .quad 0X3fe1300000000000
            .quad 0X3fe1300000000000
            .quad 0X3fe1200000000000
            .quad 0X3fe1200000000000
            .quad 0X3fe1100000000000
            .quad 0X3fe1100000000000
            .quad 0X3fe1000000000000
            .quad 0X3fe0f00000000000
            .quad 0X3fe0f00000000000
            .quad 0X3fe0e00000000000
            .quad 0X3fe0e00000000000
            .quad 0X3fe0d00000000000
            .quad 0X3fe0d00000000000
            .quad 0X3fe0c00000000000
            .quad 0X3fe0c00000000000
            .quad 0X3fe0b00000000000
            .quad 0X3fe0a00000000000
            .quad 0X3fe0a00000000000
            .quad 0X3fe0900000000000
            .quad 0X3fe0900000000000
            .quad 0X3fe0800000000000
            .quad 0X3fe0800000000000
            .quad 0X3fe0700000000000
            .quad 0X3fe0700000000000
            .quad 0X3fe0600000000000
            .quad 0X3fe0600000000000
            .quad 0X3fe0500000000000
            .quad 0X3fe0500000000000
            .quad 0X3fe0400000000000
            .quad 0X3fe0400000000000
            .quad 0X3fe0300000000000
            .quad 0X3fe0300000000000
            .quad 0X3fe0200000000000
            .quad 0X3fe0200000000000
            .quad 0X3fe0100000000000
            .quad 0X3fe0100000000000
            .quad 0X3fe0000000000000
    
.align 32
.L__CubeRootTable_256:   .quad 0X3ff0000000000000 
                         .quad 0X3ff00558e6547c36 
                         .quad 0X3ff00ab8f9d2f374 
                         .quad 0X3ff010204b673fc7 
                         .quad 0X3ff0158eec36749b 
                         .quad 0X3ff01b04ed9fdb53 
                         .quad 0X3ff02082613df53c 
                         .quad 0X3ff0260758e78308 
                         .quad 0X3ff02b93e6b091f0 
                         .quad 0X3ff031281ceb8ea2 
                         .quad 0X3ff036c40e2a5e2a 
                         .quad 0X3ff03c67cd3f7cea 
                         .quad 0X3ff03f3c9fee224c 
                         .quad 0X3ff044ec379f7f79 
                         .quad 0X3ff04aa3cd578d67 
                         .quad 0X3ff0506374d40a3d 
                         .quad 0X3ff0562b4218a6e3 
                         .quad 0X3ff059123d3a9848 
                         .quad 0X3ff05ee6694e7166 
                         .quad 0X3ff064c2ee6e07c6 
                         .quad 0X3ff06aa7e19c01c5 
                         .quad 0X3ff06d9d8b1decca 
                         .quad 0X3ff0738f4b6cc8e2 
                         .quad 0X3ff07989af9f9f59 
                         .quad 0X3ff07c8a2611201c 
                         .quad 0X3ff08291a9958f03 
                         .quad 0X3ff088a208c3fe28 
                         .quad 0X3ff08bad91dd7d8b 
                         .quad 0X3ff091cb6588465e 
                         .quad 0X3ff097f24eab04a1 
                         .quad 0X3ff09b0932aee3f2 
                         .quad 0X3ff0a13de8970de4 
                         .quad 0X3ff0a45bc08a5ac7 
                         .quad 0X3ff0aa9e79bfa986 
                         .quad 0X3ff0b0eaa961ca5b 
                         .quad 0X3ff0b4145573271c 
                         .quad 0X3ff0ba6ee5f9aad4 
                         .quad 0X3ff0bd9fd0dbe02d 
                         .quad 0X3ff0c408fc1cfd4b 
                         .quad 0X3ff0c741430e2059 
                         .quad 0X3ff0cdb9442ea813 
                         .quad 0X3ff0d0f905168e6c 
                         .quad 0X3ff0d7801893d261 
                         .quad 0X3ff0dac772091bde 
                         .quad 0X3ff0e15dd5c330ab 
                         .quad 0X3ff0e4ace71080a4 
                         .quad 0X3ff0e7fe920f3037 
                         .quad 0X3ff0eea9c37e497e 
                         .quad 0X3ff0f203512f4314 
                         .quad 0X3ff0f8be68db7f32 
                         .quad 0X3ff0fc1ffa42d902 
                         .quad 0X3ff102eb3af9ed89 
                         .quad 0X3ff10654f1e29cfb 
                         .quad 0X3ff109c1679c189f 
                         .quad 0X3ff110a29f080b3d 
                         .quad 0X3ff114176891738a 
                         .quad 0X3ff1178f0099b429 
                         .quad 0X3ff11e86ac2cd7ab 
                         .quad 0X3ff12206c7cf4046 
                         .quad 0X3ff12589c21fb842 
                         .quad 0X3ff12c986355d0d2 
                         .quad 0X3ff13024129645cf 
                         .quad 0X3ff133b2b13aa0eb 
                         .quad 0X3ff13ad8cdc48ba3 
                         .quad 0X3ff13e70544b1d4f 
                         .quad 0X3ff1420adb77c99a 
                         .quad 0X3ff145a867b1bfea 
                         .quad 0X3ff14ceca1189d6d 
                         .quad 0X3ff15093574284e9 
                         .quad 0X3ff1543d2473ea9b 
                         .quad 0X3ff157ea0d433a46 
                         .quad 0X3ff15f4d44462724 
                         .quad 0X3ff163039bd7cde6 
                         .quad 0X3ff166bd21c3a8e2 
                         .quad 0X3ff16a79dad1fb59 
                         .quad 0X3ff171fcf9aaac3d 
                         .quad 0X3ff175c3693980c3 
                         .quad 0X3ff1798d1f73f3ef 
                         .quad 0X3ff17d5a2156e97f 
                         .quad 0X3ff1812a73ea2593 
                         .quad 0X3ff184fe1c406b8f 
                         .quad 0X3ff18caf82b8dba4 
                         .quad 0X3ff1908d4b38a510 
                         .quad 0X3ff1946e7e36f7e5 
                         .quad 0X3ff1985320ff72a2 
                         .quad 0X3ff19c3b38e975a8 
                         .quad 0X3ff1a026cb58453d 
                         .quad 0X3ff1a415ddbb2c10 
                         .quad 0X3ff1a808758d9e32 
                         .quad 0X3ff1aff84bac98ea 
                         .quad 0X3ff1b3f5952e1a50 
                         .quad 0X3ff1b7f67a896220 
                         .quad 0X3ff1bbfb0178d186 
                         .quad 0X3ff1c0032fc3cf91 
                         .quad 0X3ff1c40f0b3eefc4 
                         .quad 0X3ff1c81e99cc193f 
                         .quad 0X3ff1cc31e15aae72 
                         .quad 0X3ff1d048e7e7b565 
                         .quad 0X3ff1d463b37e0090 
                         .quad 0X3ff1d8824a365852 
                         .quad 0X3ff1dca4b237a4f7 
                         .quad 0X3ff1e0caf1b71965 
                         .quad 0X3ff1e4f50ef85e61 
                         .quad 0X3ff1e923104dbe76 
                         .quad 0X3ff1ed54fc185286 
                         .quad 0X3ff1f18ad8c82efc 
                         .quad 0X3ff1f5c4acdc91aa 
                         .quad 0X3ff1fa027ee4105b 
                         .quad 0X3ff1fe44557cc808 
                         .quad 0X3ff2028a37548ccf 
                         .quad 0X3ff206d42b291a95 
                         .quad 0X3ff20b2237c8466a 
                         .quad 0X3ff20f74641030a6 
                         .quad 0X3ff213cab6ef77c7 
                         .quad 0X3ff2182537656c13 
                         .quad 0X3ff21c83ec824406 
                         .quad 0X3ff220e6dd675180 
                         .quad 0X3ff2254e114737d2 
                         .quad 0X3ff229b98f66228c 
                         .quad 0X3ff22e295f19fd31 
                         .quad 0X3ff2329d87caabb6 
                         .quad 0X3ff2371610f243f2 
                         .quad 0X3ff23b93021d47da 
                         .quad 0X3ff2401462eae0b8 
                         .quad 0X3ff2449a3b0d1b3f 
                         .quad 0X3ff2449a3b0d1b3f 
                         .quad 0X3ff2492492492492 
                         .quad 0X3ff24db370778844 
                         .quad 0X3ff25246dd846f45 
                         .quad 0X3ff256dee16fdfd4 
                         .quad 0X3ff25b7b844dfe71 
                         .quad 0X3ff2601cce474fd2 
                         .quad 0X3ff264c2c798fbe5 
                         .quad 0X3ff2696d789511e2 
                         .quad 0X3ff2696d789511e2 
                         .quad 0X3ff26e1ce9a2cd73 
                         .quad 0X3ff272d1233edcf3 
                         .quad 0X3ff2778a2dfba8d0 
                         .quad 0X3ff27c4812819c13 
                         .quad 0X3ff2810ad98f6e10 
                         .quad 0X3ff285d28bfa6d45 
                         .quad 0X3ff285d28bfa6d45 
                         .quad 0X3ff28a9f32aecb79 
                         .quad 0X3ff28f70d6afeb08 
                         .quad 0X3ff294478118ad83 
                         .quad 0X3ff299233b1bc38a 
                         .quad 0X3ff299233b1bc38a 
                         .quad 0X3ff29e040e03fdfb 
                         .quad 0X3ff2a2ea0334a07b 
                         .quad 0X3ff2a7d52429b556 
                         .quad 0X3ff2acc57a7862c2 
                         .quad 0X3ff2acc57a7862c2 
                         .quad 0X3ff2b1bb0fcf4190 
                         .quad 0X3ff2b6b5edf6b54a 
                         .quad 0X3ff2bbb61ed145cf 
                         .quad 0X3ff2c0bbac5bfa6e 
                         .quad 0X3ff2c0bbac5bfa6e 
                         .quad 0X3ff2c5c6a0aeb681 
                         .quad 0X3ff2cad705fc97a6 
                         .quad 0X3ff2cfece6945583 
                         .quad 0X3ff2cfece6945583 
                         .quad 0X3ff2d5084ce0a331 
                         .quad 0X3ff2da294368924f 
                         .quad 0X3ff2df4fd4cff7c3 
                         .quad 0X3ff2df4fd4cff7c3 
                         .quad 0X3ff2e47c0bd7d237 
                         .quad 0X3ff2e9adf35eb25a 
                         .quad 0X3ff2eee5966124e8 
                         .quad 0X3ff2eee5966124e8 
                         .quad 0X3ff2f422fffa1e92 
                         .quad 0X3ff2f9663b6369b6 
                         .quad 0X3ff2feaf53f61612 
                         .quad 0X3ff2feaf53f61612 
                         .quad 0X3ff303fe552aea57 
                         .quad 0X3ff309534a9ad7ce 
                         .quad 0X3ff309534a9ad7ce 
                         .quad 0X3ff30eae3fff6ff3 
                         .quad 0X3ff3140f41335c2f 
                         .quad 0X3ff3140f41335c2f 
                         .quad 0X3ff319765a32d7ae 
                         .quad 0X3ff31ee3971c2b5b 
                         .quad 0X3ff3245704302c13 
                         .quad 0X3ff3245704302c13 
                         .quad 0X3ff329d0add2bb20 
                         .quad 0X3ff32f50a08b48f9 
                         .quad 0X3ff32f50a08b48f9 
                         .quad 0X3ff334d6e9055a5f 
                         .quad 0X3ff33a6394110fe6 
                         .quad 0X3ff33a6394110fe6 
                         .quad 0X3ff33ff6aea3afed 
                         .quad 0X3ff3459045d8331b 
                         .quad 0X3ff3459045d8331b 
                         .quad 0X3ff34b3066efd36b 
                         .quad 0X3ff350d71f529dd8 
                         .quad 0X3ff350d71f529dd8 
                         .quad 0X3ff356847c9006b4 
                         .quad 0X3ff35c388c5f80bf 
                         .quad 0X3ff35c388c5f80bf 
                         .quad 0X3ff361f35ca116ff 
                         .quad 0X3ff361f35ca116ff 
                         .quad 0X3ff367b4fb5e0985 
                         .quad 0X3ff36d7d76c96d0a 
                         .quad 0X3ff36d7d76c96d0a 
                         .quad 0X3ff3734cdd40cd95 
                         .quad 0X3ff379233d4cd42a 
                         .quad 0X3ff379233d4cd42a 
                         .quad 0X3ff37f00a5a1ef96 
                         .quad 0X3ff37f00a5a1ef96 
                         .quad 0X3ff384e52521006c 
                         .quad 0X3ff38ad0cad80848 
                         .quad 0X3ff38ad0cad80848 
                         .quad 0X3ff390c3a602dc60 
                         .quad 0X3ff390c3a602dc60 
                         .quad 0X3ff396bdc60bdb88 
                         .quad 0X3ff39cbf3a8ca7a9 
                         .quad 0X3ff39cbf3a8ca7a9 
                         .quad 0X3ff3a2c8134ee2d1 
                         .quad 0X3ff3a2c8134ee2d1 
                         .quad 0X3ff3a8d8604cefe3 
                         .quad 0X3ff3aef031b2b706 
                         .quad 0X3ff3aef031b2b706 
                         .quad 0X3ff3b50f97de6de5 
                         .quad 0X3ff3b50f97de6de5 
                         .quad 0X3ff3bb36a36163d8 
                         .quad 0X3ff3bb36a36163d8 
                         .quad 0X3ff3c1656500d20a 
                         .quad 0X3ff3c79bedb6afb8 
                         .quad 0X3ff3c79bedb6afb8 
                         .quad 0X3ff3cdda4eb28aa2 
                         .quad 0X3ff3cdda4eb28aa2 
                         .quad 0X3ff3d420995a63c0 
                         .quad 0X3ff3d420995a63c0 
                         .quad 0X3ff3da6edf4b9061 
                         .quad 0X3ff3da6edf4b9061 
                         .quad 0X3ff3e0c5325b9fc2 
                         .quad 0X3ff3e723a499453f 
                         .quad 0X3ff3e723a499453f 
                         .quad 0X3ff3ed8a484d473a 
                         .quad 0X3ff3ed8a484d473a 
                         .quad 0X3ff3f3f92ffb72d8 
                         .quad 0X3ff3f3f92ffb72d8 
                         .quad 0X3ff3fa706e6394a4 
                         .quad 0X3ff3fa706e6394a4 
                         .quad 0X3ff400f01682764a 
                         .quad 0X3ff400f01682764a 
                         .quad 0X3ff407783b92e17a 
                         .quad 0X3ff407783b92e17a 
                         .quad 0X3ff40e08f10ea81a 
                         .quad 0X3ff40e08f10ea81a 
                         .quad 0X3ff414a24aafb1e6 
                         .quad 0X3ff414a24aafb1e6 
                         .quad 0X3ff41b445c710fa7 
                         .quad 0X3ff41b445c710fa7 
                         .quad 0X3ff421ef3a901411 
                         .quad 0X3ff421ef3a901411 
                         .quad 0X3ff428a2f98d728b 






