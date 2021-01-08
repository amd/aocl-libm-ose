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

FN_PROTOTYPE_FMA3 vrs4_exp10f

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    .ENDPROLOG

    movupd xmm0, XMMWORD PTR [rcx]



 vmovdqa xmm5,xmm0 ; save for later use
 vminps xmm0,xmm0,XMMWORD PTR L__max_exp10f_arg
 vcvtps2pd ymm2,xmm0 ;ymm2 = (double x0 ,x1,x2,)x3

 ; x * (64/log10of(2))
 vmulpd ymm3,ymm2,L__real_64_by_log10of2 ;ymm3 = x3 * (64/ln x2,(2) * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)

 ; n = int( x * (64/log10of(2)) )
 vcvtpd2dq xmm4,ymm3 ;xmm4 = (int n0 ,n1,n2,)n3 
 vcvtdq2pd ymm0,xmm4 ;ymm0 = (double n0 ,n1,n2,)n3 

 ; r = x - n * ln(2)/64
 ; r *= ln(10)
 ;vfmaddpd ymm1 ,ymm0,L__real_mlog10of2_by_64,ymm2 ;ymm1 = r r1,r2,3,r0
 vfmadd132pd  ymm0 ,ymm2,L__real_mlog10of2_by_64
 vmulpd ymm1,ymm0,L__real_ln10 ;ymm1 = r = r*ln(2) 

 ; q = r + r*r(1/2 + r*1/6)
 vmovdqa ymm3,YMMWORD PTR L__real_4_1_by_6s
 ;vfmaddpd ymm2 ,ymm1,ymm3,L__real_4_1_by_2s ;ymm2 = (1/2 + (1/6 * r))
 vfmadd213pd   ymm3  ,ymm1,L__real_4_1_by_2s
 vmulpd ymm2,ymm1,ymm1 ;ymm2 = r3*r r2,3*r2,r1*r1,r0*r0
 ;vfmaddpd ymm0 ,ymm2,ymm3,ymm1 ;ymm0 = q q1,q2,3,q0 
 vfmadd213pd     ymm3 ,ymm2,ymm1; ymm3

 ;j = n & 03fh
 vpsrad xmm1,xmm4,6 ;xmm1 = m m0 ,m1,m2,3
 vpslld xmm0,xmm4,26 
 vpsrld xmm4,xmm0,26 ;xmm4 = j j0 ,j1,j2,3 
 
 ; f + (f*q)
 lea r10,L__two_to_jby64_table
 vmovd eax,xmm4 ;eax = j0
 vpsrldq xmm0,xmm4,4 ;xmm3 = X j1 ,j2,j3,X
 vmovd ecx,xmm0 ;ecx = j1
 vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 vmovhpd xmm2,xmm2,QWORD PTR[r10 + rcx * 8] ;xmm2 = f f0,1 
 vpsrldq xmm4,xmm0,4 ;xmm4 = X j2 ,j3,XX,X
 vmovd eax,xmm4 ;eax = j2
 vpsrldq xmm0,xmm4,4 ;xmm3 = X j3 ,XX,XX,X
 vmovd ecx,xmm0 ;ecx = j3
 vmovsd xmm0,QWORD PTR[r10 + rax * 8]
 vmovhpd xmm0,xmm0,QWORD PTR[r10 + rcx * 8] ;xmm3 = f f2,3 
 vinsertf128 ymm2 ,ymm2,xmm0,1 ;ymm2 = f f1,f2,3,f0
 ;vfmaddpd ymm3 ,ymm2,ymm0,ymm2 ;ymm3 = f + (f*q)
 vfmadd213pd ymm3 ,ymm2,ymm2

 ; m = (n - j) / 64 
 vpmovsxdq xmm2,xmm1 ;xmm2 = m1,m0 
 vpsllq xmm0,xmm2,52 ;xmm0 = 2^m 2,1^m0
 vpsrldq xmm2,xmm1,8 ;xmm2 = X m2 ,m3,XX,X
 vpmovsxdq xmm1,xmm2 ;xmm1 = m3,m2
 vpsllq xmm2,xmm1,52 ;xmm2 = 2^m 2,3^m2 

 ;2 ^m * (f + (f*q)) 
 vextractf128 xmm1,ymm3,1
 vpaddq xmm0,xmm0,xmm3
 vpaddq xmm1,xmm2,xmm1
 vinsertf128 ymm2 ,ymm0,xmm1,1
 vcvtpd2ps xmm0,ymm2

 ;;special case for any x < min_exp_arg
 ;;remove this code if the above code takes care of this
 vmovdqa xmm1,XMMWORD PTR L__min_exp10f_arg 
 vcmpps xmm2 ,xmm1,xmm5,1
 vpand xmm1,xmm0,xmm2 ;put zeros in place of any x < min_exp2_arg 
 
 ;;special case for any x = nan
 ;;remove this code if the above code takes care of this
 vcmpps xmm2 ,xmm5,xmm5,00h
 vaddps xmm3,xmm5,xmm5 ;make qnan to put in place of any x =nan 
 ;vpcmov xmm0 ,xmm1,xmm3,xmm2
 VANDPD  xmm0, xmm1,  xmm2
 VANDNPD xmm4, xmm2,  xmm3
 VORPD   xmm0, xmm0,  xmm4
 ret 
 
fname endp
TEXT ENDS

data SEGMENT READ PAGE

ALIGN 16
L__max_exp10f_arg DQ 0421A209B421A209Bh, 0421A209B421A209Bh
L__min_exp10f_arg DQ 0C23369F4C23369F4h, 0C23369F4C23369F4h

ALIGN 32
L__real_64_by_log10of2 DQ 0406A934F0979A371h, 0406A934F0979A371h ; 64/log10(2)
                       DQ 0406A934F0979A371h, 0406A934F0979A371h ; 64/log10(2)
L__real_mlog10of2_by_64 DQ 0bF734413509F79FFh, 0bF734413509F79FFh ; log10of2_by_64
                        DQ 0bF734413509F79FFh, 0bF734413509F79FFh ; log10of2_by_64
L__real_ln10 DQ 040026BB1BBB55516h, 040026BB1BBB55516h ; ln(10)
             DQ 040026BB1BBB55516h, 040026BB1BBB55516h ; ln(10)
L__real_4_1_by_2s DQ 03fe0000000000000h, 03fe0000000000000h
                  DQ 03fe0000000000000h, 03fe0000000000000h 
L__real_4_1_by_6s DQ 03fc5555555555555h, 03fc5555555555555h ; 1/6
                  DQ 03fc5555555555555h, 03fc5555555555555h

data ENDS
END

