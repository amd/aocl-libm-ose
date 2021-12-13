;
; Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without modification,
; are permitted provided that the following conditions are met:
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
; 3. Neither the name of the copyright holder nor the names of its contributors
;    may be used to endorse or promote products derived from this software without
;    specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
; IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
; OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
; WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.
;

include exp_tables.inc
include fm.inc

ALM_PROTO_FMA3 exp10f

fname_special   TEXTEQU <_exp10f_special>
;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
	.ENDPROLOG
    
 vandps                   xmm1, xmm0,L__positive_infinity
 vucomiss                 xmm1, L__positive_infinity
 je                      L__x_is_nan_or_inf  

 vucomiss xmm0,L__max_exp10f_arg
 jae L__y_is_inf
 vucomiss xmm0,L__min_exp10f_arg
 jb L__y_is_zero

 vcvtps2pd xmm2,xmm0 ;xmm2 = (double)x

 ; x * (64/log10of(2))
 vmulsd xmm3,xmm2,L__real_64_by_log10of2 ;xmm3 = x * (64/ln(2)

 ; n = int( x * (64/log10of(2)) )
 vcvtpd2dq xmm4,xmm3 ;xmm4 = (int)n
 vcvtdq2pd xmm0,xmm4 ;xmm0 = (double)n

 ; r = x - n * ln(2)/64
 ; r *= ln(10)
 ;vfmaddsd xmm3 ,xmm0,L__real_mlog10of2_by_64,xmm2 ;xmm3 = r 
 vfmadd132sd     xmm0, xmm2,  L__real_mlog10of2_by_64
 vmovd ecx,xmm4 ;ecx = n
 vmulsd xmm1,xmm0,L__real_ln10 ;xmm1 = r = r*ln(2) 

 ; q = r + r*r(1/2 + r*1/6)
 vmovhpd xmm3,xmm1,L__real_1_by_6 ;xmm3 =1/6,r 
 vmovlhps xmm1,xmm1,xmm3 ;xmm1 = r,r 
 ;vfmaddpd xmm2 ,xmm3,xmm1,L__real_1_by_2_zero ;xmm2 = r*r,1/2 + (1/6 * r)
 vfmadd213pd  xmm3, xmm1,L__real_1_by_2_zero
 vmovdqa xmm5, xmm3
 vmovhlps xmm3,xmm1,xmm5 ;xmm3 = r,r*r
 ;vfmaddsd xmm0 ,xmm2,xmm3,xmm1 ;xmm0 = q
 vfmadd213sd  xmm3,xmm5,xmm1
 
 ;j = n & 03fh
 mov rax,03fh ;rax = 03fh
 and eax,ecx ;eax = j = n & 03fh

 ; f + (f*q)
 lea r10,L__two_to_jby64_table 
 vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 ;vfmaddsd xmm3 ,xmm0,xmm2,xmm2
 vfmadd231sd     xmm2, xmm3, xmm2

 ALIGN 16
 ; m = (n - j) / 64 
 vpsrad xmm1,xmm4,6
 vpsllq xmm4,xmm1,52
 vpaddq xmm2,xmm4,xmm2
 vcvtpd2ps xmm0,xmm2
 add         rsp, stack_size
 ret 

ALIGN 16 
L__x_is_nan_or_inf:
 vmovd        eax, xmm0    
 cmp         eax, L__positive_infinity
 je          L__finish
 cmp         eax, L__negative_infinity
 je          L__return_zero_without_exception    
 or          eax, L__real_qnanbit
 vmovd        xmm1, eax
 mov         r8d, 1
 call        fname_special
 jmp         L__finish 
    
ALIGN 16
L__y_is_zero:
 vpxor        		xmm1, xmm1,    	xmm1									;return value in xmm1,input in xmm0 before calling
 mov         		r8d,        2							 			;code in edi
 call        					fname_special
 jmp                 L__finish        

ALIGN 16
L__y_is_inf:
 mov         		edx,		07f800000h					
 vmovd        		xmm1,		edx				
 mov         		r8d,		3							 	
 call        					fname_special
 jmp                 L__finish  

ALIGN 16    
L__return_zero_without_exception:
 vpxor xmm0,xmm0,xmm0
        
ALIGN 16
L__finish:    
 add         rsp, stack_size
 ret 
 
fname endp
TEXT ENDS

data SEGMENT READ
CONST SEGMENT

ALIGN 16
L__max_exp10f_arg DD 0421A209Bh
L__min_exp10f_arg DD 0C23369F4h
L__real_64_by_log10of2 DQ 0406A934F0979A371h ; 64/log10(2)
L__real_mlog10of2_by_64 DQ 0bF734413509F79FFh ; log10of2_by_64
L__real_ln10 DQ 040026BB1BBB55516h ; ln(10)
L__real_1_by_2_zero DQ 00000000000000000h, 03fe0000000000000h
L__positive_infinity          DD 07f800000h
L__negative_infinity          DD 0ff800000h
L__real_qnanbit               DD 00400000h


CONST ENDS
data ENDS
END

