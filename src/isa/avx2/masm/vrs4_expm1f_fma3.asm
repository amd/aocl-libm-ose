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

ALM_PROTO_FMA3 vrs4_expm1f

stack_size      EQU     0A8h

save_rdi	equ		20h
save_rsi	equ		30h
save_xmm6  equ		40h
save_xmm7  equ		50h
save_xmm8  equ		60h
save_xmm9  equ		70h
save_xmm10 equ      80h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi
    SaveXmm xmm6, save_xmm6
    SaveXmm xmm7, save_xmm7
    SaveXmm xmm8, save_xmm8
    SaveXmm xmm9, save_xmm9	
	SaveXmm xmm10,save_xmm10  	    
    .ENDPROLOG

 movupd xmm0, XMMWORD PTR [rcx]
 vmovdqa xmm10,xmm0 ; save for later use
 
 ;x_double = (double)x;
 vcvtps2pd ymm2,xmm0 ;ymm2 = (double x0 ,x1,x2,)x3
 
 ; x * (64/ln(2)) 
 vmulpd ymm3,ymm2,L__real_64_by_log2 ;ymm3 = x3 * (64/ln x2,(2) * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)
 
 ; n = int( x * (64/ln(2)) )
 vcvtpd2dq xmm4,ymm3 ;xmm4 = (int n0 ,n1,n2,)n3 
 vcvtdq2pd ymm0,xmm4 ;ymm0 = (double n0 ,n1,n2,)n3 
 
 ; r = x - n * ln(2)/64
; vfmaddpd ymm1 ,ymm0,L__real_mlog2_by_64,ymm2 ;ymm1 = r r1,r2,3,r0

 ; q = r+r*r*(1/2 + (1/6 * r))
 ;vmovdqa ymm3,YMMWORD PTR L__real_4_1_by_6s
 ;vfmaddpd ymm2 ,ymm1,ymm3,L__real_4_1_by_2s ;ymm2 = (1/2 + (1/6 * r))
 ;vmulpd ymm3,ymm1,ymm1 ;ymm3 = r3*r r2,3*r2,r1*r1,r0*r0
 ;vfmaddpd ymm0 ,ymm2,ymm3,ymm1 ;ymm0 = q q1,q2,3,q0 
 
 vfmadd132pd  ymm0 ,ymm2,L__real_mlog2_by_64 
 vmovdqa  ymm3,YMMWORD PTR L__real_4_1_by_6s
 vfmadd213pd  ymm3,ymm0,L__real_4_1_by_2s
 vmovdqa ymm2,ymm3 
 vmulpd      ymm3  ,ymm0, ymm0
 vfmadd231pd   ymm0 ,ymm2,  ymm3
 
 

 ; m = (n - j) / 64 
 ;j = n & 03fh
 vpsrad xmm5,xmm4,6 ;xmm5 = m m0 ,m1,m2,3
 vpslld xmm3,xmm4,26 
 vpsrld xmm6,xmm3,26 ;xmm6 = j j0 ,j1,j2,3 
 
 ;process each input seperately
 lea r10,L__two_to_jby64_table 
 vmovd eax,xmm6
 vmovd ecx,xmm5
 vextractf128 xmm7,ymm0,1
 vmovhlps xmm8,xmm8,xmm0 
 mov r11d,4
 jmp L__start

ALIGN 16
L__check_n_prepare_next:
 dec r11d
 jz L__ret
 vpsrldq xmm6,xmm6,4
 vmovd eax,xmm6
 vpsrldq xmm5,xmm5,4
 vmovd ecx,xmm5 
 vpsrldq xmm10,xmm10,4 
 cmp r11d,3
 je L__output1_inputq2
 cmp r11d,2
 je L__output2_inputq3
 cmp r11d,1
 je L__output3_inputq4
 
ALIGN 16
L__output1_inputq2:
 vpxor xmm9,xmm9,xmm9
 vpslldq xmm0,xmm0,12
 vpsrldq xmm0,xmm0,12
 vpor xmm9,xmm9,xmm0
 movdqa xmm0,xmm8 
 jmp L__start
 
ALIGN 16
L__output2_inputq3: 
 vpslldq xmm0,xmm0,12
 vpsrldq xmm0,xmm0,8
 vpor xmm9,xmm9,xmm0
 vmovdqa xmm0,xmm7 
 jmp L__start
 
ALIGN 16
L__output3_inputq4:
 vpslldq xmm0,xmm0,12
 vpsrldq xmm0,xmm0,4
 vpor xmm9,xmm9,xmm0
 vmovhlps xmm0,xmm0,xmm7 

ALIGN 16
L__start:
 ucomiss xmm10,L__max_expm1_arg ;;if(x > max_expm1_arg)
 ja L__y_is_inf
 jp L__y_is_nan
 ucomiss xmm10,L__log_OnePlus_OneByFour ;;if(x < log_OnePlus_OneByFour)
 jae L__Normal_Flow
 ucomiss xmm10,L__log_OneMinus_OneByFour ;;if(x > log_OneMinus_OneByFour)
 ja L__Small_Arg
 ucomiss xmm10,L__min_expm1_arg ;;if(x < min_expm1_arg)
 jb L__y_is_minusOne

ALIGN 16 
L__Normal_Flow: 
 ;f
 vmovsd xmm3,QWORD PTR[r10 + rax * 8] ;xmm3 = f
 
 ;f*q
 ;vmulsd xmm2,xmm0,xmm3 ;xmm2 = f*q 
 
 ;twopmm.u64 = (1023 - (long)m) << 52; 
 ;f - twopmm
 mov eax,1023
 sub eax,ecx
 shl rax,52 
 vmovq xmm1,rax
 vsubsd xmm2,xmm3,xmm1 ;xmm2 = f - twopmm
 
 ;z = f * q + (f - twopmm.f64) ;
 ;z = z* 2^m
 ;vfmaddsd xmm1 ,xmm0,xmm3,xmm2 ;xmm1 = z
 vfmadd213sd xmm3,xmm0,xmm2
 shl rcx,52 
 movd xmm2,rcx
 vpaddq xmm3,xmm2,xmm3
 vcvtpd2ps xmm0,xmm3
 jmp L__check_n_prepare_next
 
ALIGN 16
L__Small_Arg:
 ;log(1-1/4) < x < log(1+1/4)
 ;q = x*x*x*(A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))));
 movapd xmm0,xmm10
 vucomiss xmm0,L__minus_zero
 je L__check_n_prepare_next
 vmulss xmm3,xmm0,L__A5_f
 vaddss xmm1,xmm3,L__A4_f 
 ;vfmaddss xmm3 ,xmm1,xmm0,L__A3_f
 ;vfmaddss xmm1 ,xmm3,xmm0,L__A2_f
 ;vfmaddss xmm3 ,xmm1,xmm0,L__A1_f ;xmm3 = (A1 + x*(A2 + x*(A3 + x*(A4 + x*(A5)))))
 vfmadd213ss xmm1,xmm0,	L__A3_f
 vfmadd213ss xmm1,xmm0,	L__A2_f
 vfmadd213ss xmm1,xmm0,	L__A1_f
 
 vmulss xmm1,xmm1,xmm0
 vmulss xmm3,xmm1,xmm0 
 vmulss xmm1,xmm3,xmm0 ;xmm1 = q 
 vcvtps2pd xmm2,xmm0 ;xmm2 = (double)x
 vmulsd xmm0,xmm2,xmm2 ;xmm0 = x*x - in double
 ;vfmaddsd xmm3 ,xmm0,L__real_1_by_2,xmm2
 vfmadd231sd xmm2,xmm0,L__real_1_by_2
 vcvtps2pd xmm0,xmm1
 vaddsd xmm2,xmm0,xmm2
 vcvtpd2ps xmm0,xmm2 
 jmp L__check_n_prepare_next 
 
ALIGN 16
L__y_is_minusOne:
 mov eax,0BF800000h
 vmovd xmm0,eax
 jmp L__check_n_prepare_next 
 
ALIGN 16
L__y_is_inf:
 mov eax,07f800000h
 vmovd xmm0,eax
 jmp L__check_n_prepare_next 
 
ALIGN 16
L__y_is_nan:
 vaddss xmm0,xmm10,xmm10
 jmp L__check_n_prepare_next 
 
L__ret:
 vpslldq xmm0,xmm0,12
 vpor xmm9,xmm9,xmm0
 vmovdqa xmm0,xmm9
 RestoreXmm xmm10, save_xmm10
 RestoreXmm xmm9, save_xmm9
 RestoreXmm xmm8, save_xmm8
 RestoreXmm xmm7, save_xmm7
 RestoreXmm xmm6, save_xmm6
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size   
 ret
 
fname endp
TEXT ENDS

data SEGMENT READ PAGE

ALIGN 16
L__max_expm1_arg DD 042B19999h
L__log_OnePlus_OneByFour DD 03E647FBFh
L__log_OneMinus_OneByFour DD 0BE934B11h
L__min_expm1_arg DD 0C18AA122h
L__A1_f DD 03E2AAAAAh 
L__A2_f DD 03D2AAAA0h
L__A3_f DD 03C0889FFh 
L__A4_f DD 03AB64DE5h
L__A5_f DD 0394AB327h
L__minus_zero DD 080000000h
ALIGN 32 
L__real_64_by_log2 DQ 040571547652b82feh, 040571547652b82feh ; 64/ln(2)
                   DQ 040571547652b82feh, 040571547652b82feh ; 64/ln(2)
L__real_mlog2_by_64 DQ 0bf862e42fefa39efh, 0bf862e42fefa39efh ; log2_by_64
                    DQ 0bf862e42fefa39efh, 0bf862e42fefa39efh ; log2_by_64
L__real_4_1_by_2s DQ 03fe0000000000000h, 03fe0000000000000h
                  DQ 03fe0000000000000h, 03fe0000000000000h 
L__real_4_1_by_6s DQ 03fc5555555555555h, 03fc5555555555555h ; 1/6
                  DQ 03fc5555555555555h, 03fc5555555555555h  

data ENDS
END

