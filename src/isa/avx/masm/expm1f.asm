;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
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

ALM_PROTO_BAS64 expm1f

fname_special   TEXTEQU <alm_expf_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG    
    
	movd     eax,xmm0
	and      eax,L__mask_sign
	cmp      eax,L__positive_infinity
	ja       L__y_is_nan

    ucomiss xmm0, L__max_expm1_arg         ;if(x > max_expm1_arg)
    jae L__y_is_inf
    ucomiss xmm0, L__log_OnePlus_OneByFour ;if(x < log_OnePlus_OneByFour)
    jae L__Normal_Flow
    ucomiss xmm0, L__log_OneMinus_OneByFour ;if(x > log_OneMinus_OneByFour)
    ja L__Small_Arg
    ucomiss xmm0, L__min_expm1_arg         ;if(x < min_expm1_arg)
    jb L__y_is_minusOne
    
ALIGN 16
L__Normal_Flow:
    ;x_double = (double)x;
    cvtps2pd     xmm0, xmm0     ;xmm0 = (double)x
    
    ; x * (64/ln(2))
    movapd      xmm3, xmm0      ;xmm3 = (xouble)x
    mulsd       xmm3, L__real_64_by_log2  ;xmm3 = x * (64/ln(2)
    
    ; n = int( x * (64/ln(2)) )
    cvtpd2dq    xmm4, xmm3   ;xmm4 = (int)n
    cvtdq2pd    xmm2, xmm4   ;xmm2 = (double)n
    
    ; r = x - n * ln(2)/64
    mulsd       xmm2, L__real_log2_by_64 ;xmm2 = n * ln(2)/64
    movd        ecx ,xmm4     ;ecx = n
    subsd       xmm0,xmm2     ;xmm0 = r
    movapd      xmm1,xmm0     ;xmm1 = r

    ;j = n & 03f
    mov         rax, 03fh      ;rax = 03f
    and         eax, ecx        ;eax = j = n & 03f
    ; m = (n - j) / 64    
    sar         ecx, 6         ;ecx = m
    
    ; q
    movsd       xmm3,L__real_1_by_6
    mulsd       xmm3, xmm0 ;xmm3 = 1/6 * r
    mulsd       xmm0, xmm1 ;xmm0 =  r  * r
    addsd       xmm3, L__real_1_by_2  ;xmm3 = 1/2 + (1/6 * r)
    mulsd       xmm0, xmm3  ;xmm0 = r*r*(1/2 + (1/6 * r))
    addsd       xmm0, xmm1  ;xmm0 = r+r*r*(1/2 + (1/6 * r))
    
    ;f
    lea         r10, L__two_to_jby64_table    
    movsd       xmm2, QWORD PTR [ r10 + rax  * 8 ]  ;xmm2 = f
    
    ;f*q
    mulsd       xmm0, xmm2  ;xmm0 = f*q
    
    ;twopmm.u64 = (1023 - (long)m) << 52;    
    ;f - twopmm
    mov         eax, 1023
    sub         eax, ecx
    shl         rax, 52   
    movd        xmm1,rax
    subsd       xmm2,xmm1
    
    ;z = f * q + (f - twopmm.f64) ;
    ;z = z* 2^m
    addsd       xmm0, xmm2     
    shl         rcx, 52 
    movd        xmm1, rcx  
    paddq       xmm1, xmm0 
    cvtpd2ps    xmm0, xmm1 
	add         rsp, stack_size
    ret
	
ALIGN 16
L__Small_Arg:
    ;log(1-1/4) < x < log(1+1/4)
    ;q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))));
    movdqa                  xmm1, xmm0
    pslld                   xmm1, 1
    psrld                   xmm1, 1    
    ucomiss xmm1,L__zero
    je L__finish
    ucomiss                 xmm1, L__real_x_near0_threshold
    jb                            L__y_is_x

	movdqa xmm1, xmm0
	mulss xmm1, L__A5_f
	addss xmm1, L__A4_f
	mulss xmm1, xmm0
	addss xmm1, L__A3_f
	mulss xmm1, xmm0
	addss xmm1, L__A2_f
	mulss xmm1, xmm0
	addss xmm1, L__A1_f
	mulss xmm1, xmm0
	mulss xmm1, xmm0
	mulss xmm1, xmm0
	cvtps2pd xmm2, xmm0
	movdqa xmm0, xmm2
	mulsd xmm2, xmm0
	mulsd xmm2, L__real_1_by_2
	addsd xmm0, xmm2
	cvtps2pd xmm2, xmm1
	addsd xmm2, xmm0
	cvtpd2ps xmm0, xmm2
    jmp L__finish
    
ALIGN 16
L__y_is_minusOne:
    mov         edx, 0BF800000h
    movd        xmm0, edx
    jmp L__finish

ALIGN 16
L__y_is_inf:
    mov         edx, 07f800000h
    movd        xmm1, edx
    mov        	r8d,	3											
    call        fname_special
    jmp L__finish

ALIGN 16
L__y_is_nan:
    movd        eax, xmm0    
    or          eax, L__real_qnanbit
    movd        xmm1, eax
	mov         r8d, 1
    call        fname_special
    jmp L__finish

ALIGN 16      
L__y_is_x:
    movdqa        			xmm1,	xmm0										
    mov         			r8d,	2											
    call        					fname_special
L__finish:
    add rsp,stack_size
    ret 

   
fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT

ALIGN 16
L__max_expm1_arg          DD 042B17218h
L__log_OnePlus_OneByFour  DD 03E647FBFh
L__log_OneMinus_OneByFour DD 0BE934B11h
L__min_expm1_arg          DD 0C18AA122h
L__real_64_by_log2        DQ 040571547652b82feh ; 64/ln(2)
L__real_log2_by_64        DQ 03f862e42fefa39efh ; log2_by_64
L__A1_f                   DD 03E2AAAAAh   
L__A2_f                   DD 03D2AAAA0h
L__A3_f                   DD 03C0889FFh     
L__A4_f                   DD 03AB64DE5h
L__A5_f                   DD 0394AB327h
L__zero                   DD 000000000h
L__positive_infinity      DD 07f800000h
L__mask_sign              DD 07fffffffh
L__real_qnanbit           DD 00400000h
L__real_x_near0_threshold DD 020000000h

CONST    ENDS
data ENDS

END
        
    

    

