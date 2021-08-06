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

#define fname __amd_remainder_piby2d2f

#  rdi = x 
#  rsi = *r
#  rdx = *region

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    #get the unbiased exponent and the mantissa part of x
    movd %rdi, %xmm0
    lea  .L__2_by_pi_bits(%rip),%r9
    
    #xexp = (x >> 52) - 1023
    movd %xmm0, %r11
    mov  %r11,  %rcx    
    shr  $52,   %r11
    sub  $1023, %r11      #r11 = xexp = exponent of input x    

    #calculate the last byte from which to start multiplication
    #last = 134 - (xexp >> 3)    
    mov  %r11, %r10
    shr  $3,   %r10
    sub  $134, %r10       #r10 = -last
    neg  %r10             #r10 = last

    #load 64 bits of 2_by_pi
    mov  (%r9, %r10),%rax
    mov  %rdx, %rdi      # save address of region since mul modifies rdx
    
    #mantissa of x = ((x << 12) >> 12) | implied bit
    shl  $12,   %rcx
    shr  $12,   %rcx      #rcx = mantissa part of input x      
    bts  $52,   %rcx      #add the implied bit as well    

    #load next 128 bits of 2_by_pi        
    add  $8, %r10        #increment to next 8 bytes of 2_by_pi
    movdqu (%r9, %r10), %xmm0    

    #do three 64-bit multiplications with mant of x    
    mul  %rcx
    mov  %rax, %r8       #r8 = last 64 bits of multiplication = res1[2]    
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0, %rax
    mul  %rcx
    #resexp = xexp & 7    
    and  $7, %r11        #r11 = resexp = xexp & 7 = last 3 bits
    psrldq $8, %xmm0         
    add  %r10, %rax      # add the previous carry
    adc  $0,   %rdx
    mov  %rax, %r9       #r9 = next 64 bits of multiplication = res1[1]
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0,%rax
    mul  %rcx
    add  %rax, %r10      #r10 = most significant 64 bits = res1[0]
    
    #find the region  
    #last three bits ltb = most sig bits >> (54 - resexp)); // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
    #point_five = ltb & 0x1;
    #region = ((ltb >> 1) + point_five) & 3;    
    mov $54,  %rcx
    mov %r10, %rax
    sub %r11, %rcx
    xor %rdx, %rdx       #rdx = sign of x(i.e first part of x * 2bypi)    
    shr %cl,  %rax       
    jnc  .L__no_point_five
    ##if there is carry.. then negate the result of multiplication
    not  %r10
    not  %r9
    not  %r8
    mov $0x8000000000000000,%rdx

.p2align 4 
.L__no_point_five:
    adc $0,   %rax
    and $3,   %rax
    mov %eax, (%rdi)     #store region to memory

    #calculate the number of integer bits and zero them out
    mov %r11, %rcx  
    add $10,  %rcx  #rcx = no. of integer bits
    shl %cl,  %r10
    shr %cl,  %r10  #r10 contains only mant bits
    sub $64,  %rcx  #form the exponent
    mov %rcx, %r11
    
    #find the highest set bit
    bsr %r10,%rcx
    jnz  .L__form_mantissa
    mov %r9,%r10
    mov %r8,%r9
    bsr %r10,%rcx  #rcx = hsb
    sub $64, %r11
    
    
.p2align 4  
.L__form_mantissa:
    add %rcx,%r11  #for exp of x
    sub $52,%rcx #rcx = no. of bits to shift in r10    
    cmp $0,%rcx
    jl  .L__hsb_below_52
    je  .L__form_numbers
    #hsb above 52
    mov %r10,%r8 #previous contents of r8 not required
    shr %cl,%r10 #r10 = mantissa of x with hsb at 52
    jmp .L__form_numbers
    
.p2align 4 
.L__hsb_below_52:
    neg %rcx
    mov %r9,%rax
    shl %cl,%r10
    shl %cl,%r9
    sub $64,%rcx
    neg %rcx
    shr %cl,%rax
    or  %rax,%r10
    
.p2align 4
.L__form_numbers:
    add $1023, %r11
    btr $52,%r10     #remove the implied bit
    mov %r11,%rcx
    or  %rdx,%r10    #put the sign    
    shl $52, %rcx
    or  %rcx,%r10    #x is in r10
    
    movd %r10,%xmm0  #xmm0 = x
    mulsd .L__piby2(%rip),%xmm0
    movsd %xmm0,(%rsi)
    ret      
    
.section .rodata
.align 16
.type	.L__2_by_pi_bits, @object
.size	.L__2_by_pi_bits, 158
.L__2_by_pi_bits:
    .byte 224,241,27,193,12,88,33,116,53,126,196,126,237,175,169,75,74,41,222,231,28,244,236,197,151,175,31,235,158,212,181,168,127,121,154,253,24,61,221,38,44,159,60,251,217,180,125,180,41,104,45,70,188,188,63,96,22,120,255,95,226,127,236,160,228,247,46,126,17,114,210,231,76,13,230,88,71,230,4,249,125,209,154,192,113,166,19,18,237,186,212,215,8,162,251,156,166,196,114,172,119,248,115,72,70,39,168,187,36,25,128,75,55,9,233,184,145,220,134,21,239,122,175,142,69,249,7,65,14,241,100,86,138,109,3,119,211,212,71,95,157,240,167,84,16,57,185,13,230,139,2,0,0,0,0,0,0,0
.align 16
.L__piby2: .quad 0x3ff921fb54442d18
