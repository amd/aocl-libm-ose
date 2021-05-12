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

ALM_PROTO_FMA3 exp2

fname_special   TEXTEQU <_exp2_special>
;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG
  
 vandpd                   xmm1, xmm0, L__positive_infinity
 vucomisd                 xmm1, L__positive_infinity
 je                      L__x_is_nan_or_inf  
	    
 vucomisd xmm0,L__max_exp2_arg
 ja L__y_is_inf
 vucomisd xmm0,L__min_exp2_arg
 jbe L__y_is_zero

 vpsllq                   xmm1, xmm0, 1
 vpsrlq                   xmm1, xmm1, 1    
 vucomisd                 xmm1, L__real_x_near0_threshold
 jb                            L__y_is_one

 ; x * (64)
 vmulsd xmm1,xmm0,L__real_64

 ; n = int( x * (64))
 vcvttpd2dq xmm2,xmm1 ;xmm2 = (int)n
 vcvtdq2pd xmm1,xmm2 ;xmm1 = (double)n
 vmovd ecx,xmm2

 ; r = x - n * 1/64 
 ;r *= ln2; 
 ;vfmaddsd xmm2,xmm1,L__one_by_64,xmm0
 vfmadd132sd  xmm1, xmm0,L__one_by_64 
 vmulsd xmm1,xmm1,L__ln_2
 vmovlhps xmm2,xmm1,xmm1 ;xmm2 = r,r
 
 ;j = n & 03fh 
 mov rax,03fh
 and eax,ecx ;eax = j
 ; m = (n - j) / 64 
 sar ecx,6 ;ecx = m

 ; q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
 ; q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
 vmovapd    xmm3,L__real_1_by_720
 ;vfmaddsd   xmm3,xmm2,xmm3,L__real_1_by_120
 ;vfmaddsd   xmm3,xmm2,xmm3,L__real_1_by_24
 ;vfmaddsd   xmm3,xmm2,xmm3,L__real_1_by_6
 ;vfmaddsd   xmm3,xmm2,xmm3,L__real_1_by_2
   vfmadd213sd      xmm3, xmm2,L__real_1_by_120
   vfmadd213sd     xmm3, xmm2, L__real_1_by_24
   vfmadd213sd     xmm3, xmm2,L__real_1_by_6 
   vfmadd213sd    xmm3, xmm2, L__real_1_by_2 
 
 vmulsd     xmm0,xmm2,xmm2
; vfmaddsd   xmm0,xmm0,xmm3,xmm2
vfmadd213sd    xmm0 , xmm3, xmm2
 
 ; (f)*(q) + f2 + f1
 cmp ecx,0fffffc02h ; -1022 
 lea rdx,L__two_to_jby64_table 
 lea r11,L__two_to_jby64_tail_table 
 lea r10,L__two_to_jby64_head_table 
 vmulsd xmm2,xmm0,QWORD PTR[rdx + rax * 8]
 vaddsd xmm1,xmm2,QWORD PTR[r11 + rax * 8]
 vaddsd xmm0,xmm1,QWORD PTR[r10 + rax * 8] 

 jle L__process_denormal 
L__process_normal:
 shl rcx,52 
 vmovq xmm2,rcx
 vpaddq xmm0,xmm0,xmm2
 add         rsp, stack_size
 ret 

ALIGN 16
L__process_denormal:
 jl L__process_true_denormal
 vucomisd xmm0,L__real_one
 jae L__process_normal
L__process_true_denormal:
 ; here ( e^r < 1 and m = -1022 ) or m <= -1023
 add ecx,1074
 mov rax,1 
 shl rax,cl
 vmovq xmm2,rax
 vmulsd xmm0,xmm0,xmm2
 jmp         L__finish      

L__y_is_one:
 vmovsd       xmm0, L__real_one
 jmp         L__finish
    
ALIGN 16 
L__x_is_nan_or_inf:
 vmovq        rax, xmm0    
 cmp         rax, L__positive_infinity
 je          L__finish
 cmp         rax, L__negative_infinity
 je          L__return_zero_without_exception    
 or          rax, L__real_qnanbit
 vmovq        xmm1, rax
 mov         r8d, 1
 call        fname_special
 jmp         L__finish 
    
ALIGN 16 
L__y_is_inf:
 mov         			rax,	07ff0000000000000h    						
 vmovq       				xmm1,	rax										
 mov        				r8d,	3											
 call        					fname_special
 jmp       L__finish   

ALIGN 16      
L__y_is_zero:
 vpxor        			xmm1, xmm1,	xmm1										
 mov         			r8d,	2											
 call        					fname_special
 jmp     L__finish
 
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
L__max_exp2_arg DQ 04090000000000000h
L__min_exp2_arg DQ 0c090c80000000000h
L__real_64 DQ 04050000000000000h ; 64
L__ln_2 DQ 03FE62E42FEFA39EFh
L__one_by_64 DQ 0bF90000000000000h
L__real_one DQ 03ff0000000000000h
L__negative_infinity      DQ 0fff0000000000000h
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit
L__positive_infinity      DQ 07ff0000000000000h
L__real_x_near0_threshold DQ 3c00000000000000h

CONST ENDS
data ENDS
END

