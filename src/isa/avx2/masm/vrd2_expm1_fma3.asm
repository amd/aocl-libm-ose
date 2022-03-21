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

ALM_PROTO_FMA3 vrd2_expm1

text SEGMENT EXECUTE

stack_size      EQU     088h
save_xmm10 equ      20h
save_xmm11  equ     30h
save_xmm12  equ     40h

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
	SaveXmm xmm10,save_xmm10  
    SaveXmm xmm11,save_xmm11	   
    SaveXmm xmm12,save_xmm12
    .ENDPROLOG

 movupd xmm0, XMMWORD PTR [rcx]
 vmovdqa xmm10,xmm0 ; save for later use

 ; x * (64/ln(2))
 vmulpd xmm3,xmm0,L__real_64_by_log2
 
 ; n = int( x * (64/ln(2)) )
 vcvttpd2dq xmm4,xmm3 ;xmm4 = 0,0,(int)n1,(int)n0
 vcvtdq2pd xmm3,xmm4 ;xmm3 = (double)n1,n0 
 vmovq rax,xmm4 ;rax = (int)n1,n0
 
 ; r1 = x - n * ln(2)/64 head 
 ; r2 = - n * ln(2)/64 tail 
 ; r = r1+r2 
 ;vfmaddpd xmm2 ,xmm3,L__log2_by_64_mhead,xmm0
 ;vfmaddpd xmm2 ,xmm3,L__log2_by_64_mtail,xmm2 ;xmm2 = r r_0,_1
 
 vfmadd231pd    xmm0, xmm3,L__log2_by_64_mhead
 vfmadd231pd  xmm0, xmm3, L__log2_by_64_mtail 
 vmovdqa  xmm2,  xmm0  	
 
 ;j = n & 03fh 
 mov r8,00000003f0000003fh
 and r8,rax ;r8 = j1,j0
 mov eax,r8d ;rax = j0
 shr r8,32 ;r8 = j1
 
 ; m = (n - j) / 64 
 vpsrad xmm12,xmm4,6 ;xmm12 = 0,0,m m0,1

 ; q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
 ; q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
 vmovapd    xmm3,L__real_1_by_720
 ;vfmaddpd   xmm3,xmm2,xmm3,L__real_1_by_120
 ;vfmaddpd   xmm3,xmm2,xmm3,L__real_1_by_24
 ;vfmaddpd   xmm3,xmm2,xmm3,L__real_1_by_6
 ;vfmaddpd   xmm3,xmm2,xmm3,L__real_1_by_2
 ;vmulpd     xmm0,xmm2,xmm2
 ;vfmaddpd   xmm0,xmm0,xmm3,xmm2 
 
 vfmadd213pd      xmm3	 , xmm2,  L__real_1_by_120
 vfmadd213pd      xmm3	 , xmm2,  L__real_1_by_24 
 vfmadd213pd      xmm3	 , xmm2,  L__real_1_by_6  
 vfmadd213pd      xmm3	 , xmm2,  L__real_1_by_2  
 vmulpd           xmm0	 , xmm2,  xmm2            
 vfmadd213pd      xmm0 	 , xmm3,  xmm2            
 
 
 
 
 ;seperate processing for each element
 vmovhlps xmm11,xmm11,xmm0 ;save higher part
 vmovq rcx,xmm12 ;rcx = m1,m0 
 mov r9d,2
 jmp L__next

ALIGN 16 
L__check_n_prepare_next:
 dec r9d
 jz L__ret
 vmovlhps xmm11,xmm11,xmm0 ; save previous result
 vmovdqa xmm0,xmm11 ;xmm0 = q1
 vpsrldq xmm10,xmm10,8 ;next x
 mov rax,r8
 vmovq rcx,xmm12
 shr rcx,32 ;rcx = m1 

ALIGN 16 
L__next:
 vucomisd xmm10,L__max_expm1_arg ;check if(x > 709.8)
 ja L__y_is_inf
 jp L__y_is_nan 
 vucomisd xmm10,L__min_expm1_arg ;if(x < -37.42994775023704)
 jb L__y_is_minusOne
 vucomisd xmm10,L__log_OneMinus_OneByFour
 jbe L__Normal_Flow
 vucomisd xmm10,L__log_OnePlus_OneByFour
 jb L__Small_Arg 
 
ALIGN 16 
L__Normal_Flow:
 
 ; load f2,f1,f
 lea rdx,L__two_to_jby64_table 
 lea r10,L__two_to_jby64_head_table 
 lea r11,L__two_to_jby64_tail_table
 vmovsd xmm1,QWORD PTR[rdx + rax * 8] ;xmm1 = f
 vmovsd xmm2,QWORD PTR[r10 + rax * 8] ;xmm2 = f1
 vmovsd xmm3,QWORD PTR[r11 + rax * 8] ;xmm3 = f2 
 
 ;twopmm.u64 = (1023 - (long)m) << 52;
 mov eax,1023
 sub eax,ecx
 shl rax,52 ;rax = twopmm
 
 cmp ecx,52 ;check m > 52
 jg L__M_Above_52
 cmp ecx,-7 ;check if m < -7
 jl L__M_Below_Minus7
 ;(-8 < m) && (m < 53)
 shl   rcx,52             ;rcx = 2^m,m at exponent    
 vmovq  xmm4,rcx
 vaddsd xmm1,xmm0,L__One ;xmm1 = 1+q
 vmulsd xmm1,xmm1,xmm3   ;f2*(1+q)    
 vmovq  xmm3,rax         ;xmm3 = twopmm        
 ;vfmaddsd xmm0,xmm0,xmm2,xmm1 ;f1*q + f2*(1+q))
 vfmadd213sd xmm0,xmm2, xmm1
 vsubsd xmm2,xmm2,xmm3          ;xmm2 = f1 - twopmm    
 vaddsd xmm0,xmm0,xmm2
 vpaddq xmm0,xmm0,xmm4
 jmp L__check_n_prepare_next 
 
 ALIGN 16 
L__M_Above_52:
 cmp ecx,1024 ;check if m = 1024
 je L__M_Equals_1024
 ;twopm.f64 * (f1.f64 + (f*q+(f2.f64 - twopmm.f64)));// 2^-m should not be calculated if m>105
 vmovq xmm4,rax ;xmm4 = twopmm
 vsubsd xmm3,xmm3,xmm4 ;xmm3 = f2 - twopmm
 ;vfmaddpd xmm4 ,xmm0,xmm1,xmm3 ;xmm4 = (f*q+(f2 - twopmm)))
 vfmadd213pd xmm0,xmm1,xmm3
 vaddpd xmm0,xmm0,xmm2 ;xmm0 = (f1 + (f*q+(f2 - twopmm))) 
 shl rcx,52
 vmovq xmm1,rcx ;xmm1 = twopm
 vpaddq xmm0,xmm0,xmm1
 jmp L__check_n_prepare_next 
 
 ALIGN 16 
L__M_Below_Minus7:
 ;twopm.f64 * (f1.f64 + (f*q + f2.f64)) - 1;
 ;vfmaddpd xmm4 ,xmm0,xmm1,xmm3 ;xmm4 = (f*q + f2.f64)
  vfmadd213pd  xmm0, xmm1, xmm3
 
 vaddpd xmm0,xmm0,xmm2 ;xmm0 = f1 + (f*q + f2.f64)
 shl rcx,52
 vmovq xmm1,rcx ;xmm1 = twopm
 vpaddq xmm0,xmm0,xmm1 ;xmm0 = twopm *(xmm0)
 vsubpd xmm0,xmm0,L__zero_One 
 jmp L__check_n_prepare_next 
 
 ALIGN 16
L__M_Equals_1024:
 mov rax,04000000000000000h ;1024 at exponent
 ;vfmaddpd xmm4 ,xmm0,xmm1,xmm3 ;xmm4 = (f*q) + f2 
 vfmadd213pd  xmm0, xmm1, xmm3   
 vmovdqa  xmm4,  xmm0
 vaddpd xmm0,xmm4,xmm2 ;xmm0 = f1 + (f*q) + f2
 vmovq xmm1,rax ;xmm1 = twopm
 vpaddq xmm0,xmm0,xmm1
 vmovq rax,xmm0
 mov rcx,07FF0000000000000h
 and rax,rcx
 cmp rax,rcx ;check if we reached inf
 je L__y_is_inf
 jmp L__check_n_prepare_next 
 
 ALIGN 16
L__Small_Arg:
 vmovapd xmm0,xmm10
 vucomisd xmm0,L__minus_zero
 je L__check_n_prepare_next
 mov rax,001E0000000000000h ;30 in exponents place
 ;u = (twop30.f64 * x + x) - twop30.f64 * x; 
 vmovq xmm1,rax
 vpaddq xmm1,xmm1,xmm0 ;xmm1 = twop30.f64 * x
 vaddsd xmm2,xmm1,xmm0 ;xmm2 = (twop30.f64 * x + x)
 vsubsd xmm2,xmm2,xmm1 ;xmm2 = u
 vsubsd xmm1,xmm0,xmm2 ;xmm1 = v = x-u
 vmulsd xmm3,xmm2,xmm2 ;xmm3 = u*u
 vmulsd xmm3,xmm3,L__real_1_by_2 ;xmm3 = y = u*u*0.5
 
 ;q = x*x*x*(A1.f64 + x*(A2.f64 + x*(A3.f64 + x*(A4.f64 + x*(A5.f64 + x*(A6.f64 + x*(A7.f64 + x*(A8.f64 + x*(A9.f64)))))))));
 vmulsd xmm5,xmm0,L__B9
 vaddsd xmm5,xmm5,L__B8
 ;vfmaddsd xmm4 ,xmm5,xmm0,L__B7
 ;vfmaddsd xmm5 ,xmm4,xmm0,L__B6 
 ;vfmaddsd xmm4 ,xmm5,xmm0,L__B5 
 ;vfmaddsd xmm5 ,xmm4,xmm0,L__B4 
 ;vfmaddsd xmm4 ,xmm5,xmm0,L__B3 
 ;vfmaddsd xmm5 ,xmm4,xmm0,L__B2 
 ;vfmaddsd xmm4 ,xmm5,xmm0,L__B1 
 vfmadd213sd    xmm5  , xmm0, L__B7
 vfmadd213sd    xmm5  , xmm0, L__B6
 vfmadd213sd    xmm5  , xmm0, L__B5
 vfmadd213sd    xmm5  , xmm0, L__B4
 vfmadd213sd    xmm5  , xmm0, L__B3
 vfmadd213sd    xmm5  , xmm0, L__B2
 vfmadd213sd    xmm5  , xmm0, L__B1
 vmovdqa   xmm4, xmm5
 
 
 
 
 vmulsd xmm5,xmm4,xmm0 
 vmulsd xmm4,xmm5,xmm0 
 vmulsd xmm5,xmm4,xmm0 ;xmm5 = q 
 
 ;z = v * (x + u) * 0.5;
 vaddsd xmm4,xmm2,xmm0
 vmulsd xmm4,xmm4,xmm1
 vmulsd xmm4,xmm4,L__real_1_by_2 ;xmm4 = z 
 
 vucomisd xmm3,L__TwopM7 
 jb L__returnNext
 vaddsd xmm1,xmm1,xmm4 ;xmm1 = v+z
 vaddsd xmm1,xmm1,xmm5 ;xmm1 = q+(v+z)
 vaddsd xmm2,xmm2,xmm3 ;xmm2 = u+y
 vaddsd xmm1,xmm1,xmm2
 vmovapd xmm0,xmm1
 jmp L__check_n_prepare_next
 
ALIGN 16
L__returnNext:
 vaddsd xmm4,xmm4,xmm5 ;xmm4 = q +z
 vaddsd xmm3,xmm3,xmm4 ;xmm3 = y+(q+z)
 vaddsd xmm0,xmm0,xmm3 
 jmp L__check_n_prepare_next
 
ALIGN 16
L__y_is_inf:
 mov rax,07ff0000000000000h
 vmovq xmm0,rax
 jmp L__check_n_prepare_next

ALIGN 16
L__y_is_nan:
 vaddsd xmm0,xmm0,xmm0
 jmp L__check_n_prepare_next

ALIGN 16
L__y_is_minusOne:
 mov rax,0BFF0000000000000h ;return -1
 vmovq xmm0,rax
 jmp L__check_n_prepare_next

ALIGN 16
L__ret:
 vmovdqa xmm1,xmm0
 vmovhlps xmm0,xmm0,xmm11
 vmovlhps xmm0,xmm0,xmm1 
 RestoreXmm xmm12, save_xmm12
 RestoreXmm xmm11, save_xmm11
 RestoreXmm xmm10, save_xmm10
 StackDeallocate stack_size 
 ret 
 
fname endp
TEXT ENDS

data SEGMENT READ PAGE

ALIGN 16
L__max_expm1_arg DQ 040862E6666666666h 
L__min_expm1_arg DQ 0C042B708872320E1h
L__log_OneMinus_OneByFour DQ 0BFD269621134DB93h
L__log_OnePlus_OneByFour DQ 03FCC8FF7C79A9A22h
L__real_64_by_log2 DQ 040571547652b82feh, 040571547652b82feh ; 64/ln(2) 
L__log2_by_64_mhead DQ 0bf862e42fefa0000h, 0bf862e42fefa0000h
L__log2_by_64_mtail DQ 0bd1cf79abc9e3b39h, 0bd1cf79abc9e3b39h
L__TwopM7 DQ 03F80000000000000h
L__zero_One DQ 03FF0000000000000h, 00000000000000000h
L__minus_zero DQ 08000000000000000h
L__B9 DQ 03E5A2836AA646B96h
L__B8 DQ 03E928295484734EAh
L__B7 DQ 03EC71E14BFE3DB59h
L__B6 DQ 03EFA019F635825C4h
L__B5 DQ 03F2A01A01159DD2Dh
L__B4 DQ 03F56C16C16CE14C6h
L__B3 DQ 03F8111111111A9F3h
L__B2 DQ 03FA55555555554B6h
L__B1 DQ 03FC5555555555549h
L__One DQ 03FF0000000000000h

data ENDS
END

