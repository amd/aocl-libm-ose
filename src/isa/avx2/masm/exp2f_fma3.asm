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

;
;  (C) 2008-2012 Advanced Micro Devices, Inc. All Rights Reserved.
;
;  This file is part of AMD LibM 3.0.2.
;
;  AMD LibM 3.0.2 is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public
;  License as published by the Free Software Foundation; either
;  version 2.1 of the License, or (at your option) any later version.
;
;  AMD LibM 3.0.2 is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with AMD LibM 3.0.2.  If not, see
;  <http://www.gnu.org/licenses/>.
;
;


include exp_tables.inc
include fm.inc

ALM_PROTO_FMA3 exp2f

fname_special   TEXTEQU <_exp2f_special>
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

 vucomiss xmm0,L__max_exp2f_arg
 jae L__y_is_inf
 vucomiss xmm0,L__min_exp2f_arg
 jb L__y_is_zero

 vcvtps2pd xmm2,xmm0 ;xmm2 = (double)x

 ; x * (64)
 vpaddq xmm3,xmm2,L__sixtyfour ;xmm3 = x * (64)

 ; n = int( x * (64)
 vcvtpd2dq xmm4,xmm3 ;xmm4 = (int)n
 vcvtdq2pd xmm0,xmm4 ;xmm0 = (double)n

 ; r = x - n * 1/64
 ; r *= ln(2)
 ;vfmaddsd xmm3 ,xmm0,L__mone_by_64,xmm2 ;xmm3 = r 
 vfmadd132sd xmm0, xmm2, L__mone_by_64
 vmovd ecx,xmm4 ;ecx = n
 vmulsd xmm1,xmm0,L__ln2 ;xmm1 = r = r*ln(2) 

 ; q = r+r*r*(1/2 + (1/6 * r))
 vmovhpd xmm3,xmm1,L__real_1_by_6 ;xmm3 =1/6,r 
 vmovlhps xmm1,xmm1,xmm3 ;xmm1 = r,r 
 ;vfmaddpd xmm2 ,xmm3,xmm1,L__real_1_by_2_zero ;xmm2 = r*r,1/2 + (1/6 * r)
 vfmadd213pd xmm3, xmm1, L__real_1_by_2_zero
 vmovdqa xmm2, xmm3
 vmovhlps xmm3,xmm1,xmm2 ;xmm3 = r,r*r
 ;vfmaddsd xmm0 ,xmm2,xmm3,xmm1 ;xmm0 = q
 vfmadd231sd xmm1, xmm3, xmm2
 
 ;j = n & 03fh
 mov rax,03fh ;rax = 03fh
 and eax,ecx ;eax = j = n & 03fh

 ; f + (f*q)
 lea r10,L__two_to_jby64_table 
 vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 ;vfmaddsd xmm3 ,xmm1,xmm2,xmm2
 vfmadd213sd  xmm2,xmm1,xmm2
 vmovdqa xmm3, xmm2

 ALIGN 16
 ; m = (n - j) / 64 
 vpsrad xmm1,xmm4,6
 vpsllq xmm4,xmm1,52
 vpaddq xmm2,xmm4,xmm3
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
L__max_exp2f_arg DD 043000000h
L__min_exp2f_arg DD 0c3150000h
ALIGN 16
L__real_1_by_2_zero DQ 00000000000000000h, 03fe0000000000000h
L__sixtyfour DQ 00060000000000000h ; 64
L__mone_by_64 DQ 0bF90000000000000h ; 1/64
L__ln2 DQ 03FE62E42FEFA39EFh ; ln(2)
L__negative_infinity          DD 0ff800000h
L__real_qnanbit               DD 00400000h
L__positive_infinity          DD 07f800000h

CONST ENDS
data ENDS
END

