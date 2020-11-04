;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

FN_PROTOTYPE_BAS64 expm1

fname_special   TEXTEQU <_expm1_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG 

	movd     rax,xmm0
	and      rax,L__mask_sign
	cmp      rax,L__positive_infinity
	ja       L__y_is_nan
	    
    ucomisd xmm0,L__max_expm1_arg  ;check if(x > 709.8)
    ja L__y_is_inf
    ucomisd xmm0, L__min_expm1_arg ;if(x < -37.42994775023704)
    jb L__y_is_minusOne
    ucomisd xmm0, L__log_OneMinus_OneByFour
    jbe L__Normal_Flow
    ucomisd xmm0, L__log_OnePlus_OneByFour
    jb L__Small_Arg 
    
ALIGN 16
L__Normal_Flow:
    ; x * (64/ln(2))
    movapd      xmm1, xmm0
    mulsd       xmm1, L__real_64_by_log2

    ; n = int( x * (64/ln(2)) )
    cvttpd2dq   xmm2, xmm1;xmm2 = (int)n
    cvtdq2pd    xmm1, xmm2;xmm1 = (double)n
    movd        ecx, xmm2
    movapd      xmm2, xmm1
    
    ; r1 = x - n * ln(2)/64 head    
    mulsd    xmm1, L__log2_by_64_mhead
        
    ;j = n & 03fh    
    mov          rax,03fh
    and          eax,ecx ;eax = j
    ; m = (n - j) / 64      
    sar          ecx, 6  ;ecx = m        

    ; r2 = - n * ln(2)/64 tail
    mulsd    xmm2, L__log2_by_64_mtail
    addsd    xmm0, xmm1    ;xmm0 = r1

    ; r1+r2
    addsd        xmm2, xmm0;xmm2 = r

    ; q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    ; q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd        xmm3, L__real_1_by_720;xmm3 = 1/720
    mulsd        xmm3,  xmm2;xmm3 = r*1/720
    movapd        xmm0, L__real_1_by_6;xmm0 = 1/6    
    movapd       xmm1,  xmm2;xmm1 = r            
    mulsd        xmm0,  xmm2;xmm0 = r*1/6
    addsd        xmm3,  L__real_1_by_120;xmm3 = 1/120 + (r*1/720)
    mulsd        xmm1,  xmm2;xmm1 = r*r    
    addsd        xmm0,  L__real_1_by_2;xmm0 = 1/2 + (r*1/6)        
    movapd        xmm4, xmm1;xmm4 = r*r
    mulsd        xmm4,  xmm1;xmm4 = (r*r) * (r*r)    
    mulsd        xmm3,  xmm2;xmm3 = r * (1/120 + (r*1/720))
    mulsd        xmm0,  xmm1;xmm0 = (r*r)*(1/2 + (r*1/6))
    addsd        xmm3,  L__real_1_by_24;xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addsd        xmm0,  xmm2;xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulsd        xmm3,  xmm4;xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addsd        xmm0,  xmm3;xmm0 = q= r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    
    ; load  f1,f,f2
    lea          rdx, L__two_to_jby64_table
    lea          r10, L__two_to_jby64_head_table
    lea          r11, L__two_to_jby64_tail_table
    movsd        xmm1, QWORD PTR [ rdx + rax  * 8 ]   ;xmm1 = f
    movsd        xmm2, QWORD PTR [ r10 + rax  * 8 ]   ;xmm2 = f1
    movsd        xmm3, QWORD PTR [ r11 + rax  * 8 ]   ;xmm3 = f2                
    
    ;twopmm.u64 = (1023 - (long)m) << 52;
    mov eax, 1023
    sub eax, ecx
    shl  rax, 52   ;rax = twopmm
    
    cmp ecx, 52;check m > 52
    jg L__M_Above_52
    cmp ecx, -7    ;check if m < -7
    jl L__M_Below_Minus7
    ;(-8 < m) && (m < 53)
    movapd  xmm1, xmm0  ;xmm1 = q
    addsd xmm1, L__One  ;xmm1 = 1+q
    mulsd xmm1, xmm3    ;xmm1 = f2.f64 *(1+q)    
    mulsd xmm0, xmm2    ;xmm0 = f1.f64*q
    addsd xmm0, xmm1    ;xmm0 = (f1.f64*q+ f2.f64 *(1+q))    
    movd   xmm1, rax    ;xmm1 = twopmm
    subsd xmm2, xmm1    ;xmm2 = f1 - twopmm
    addsd xmm0,xmm2
    shl   rcx,52        ;rcx = 2^m ,mat exponent
    movd   xmm1, rcx
    paddq xmm0, xmm1
    jmp L__finish  
    
    ALIGN 16  
L__M_Above_52:
    cmp ecx ,1024    ;check if m = 1024
    je L__M_Equals_1024
    ;twopm.f64 * (f1.f64 + (f*q+(f2.f64 - twopmm.f64)));// 2^-m should not be calculated if m>105
    movd xmm4,rax    ;xmm4 = twopmm
    subsd xmm3,xmm4  ;xmm3 = f2 - twopmm
    mulsd xmm0,xmm1  ;xmm0 = f*q
    addsd xmm0,xmm3  ;xmm0 = (f*q+(f2.f64 - twopmm.f64)))
    addsd xmm0,xmm2  ;xmm0 = (f1.f64 + (f*q+(f2.f64 - twopmm.f64)))
    shl rcx, 52
    movd xmm1, rcx   ;xmm1 = twopm
    paddq xmm0,xmm1
    jmp L__finish
      
    ALIGN 16    
L__M_Below_Minus7:
    ;twopm.f64 * (f1.f64 + (f*q + f2.f64)) - 1;
    mulsd xmm0,xmm1  ;xmm0 = f*q
    addsd xmm0,xmm3  ;xmm0 = (f*q + f2.f64)
    addsd xmm0,xmm2  ;xmm0 = f1 + (f*q + f2.f64)
    shl rcx,52
    movd xmm1,rcx    ;xmm1 = twopm
    paddq xmm0,xmm1  ;xmm0 = twopm *(xmm0)
    subsd xmm0,L__One
    jmp L__finish
    
    ALIGN 16
L__M_Equals_1024:
    mov rax ,04000000000000000h   ;1024 at exponent
    mulsd xmm0 ,xmm1;xmm0 = f*q
    addsd xmm0 ,xmm3;xmm0 = (f*q) + f2
    addsd xmm0 ,xmm2;xmm0 = f1 + (f*q) + f2
    movd xmm1   ,rax;xmm1 = twopm
    paddq xmm0,xmm1
    movd rax,xmm0
    mov rcx,07FF0000000000000h
    and rax,rcx
    cmp rax ,rcx              ;check if we reached inf
    je L__y_is_inf
    jmp L__finish
    
    ALIGN 16
L__Small_Arg:
    movdqa                  xmm1, xmm0
    psllq                   xmm1, 1
    psrlq                   xmm1, 1    
    ucomisd xmm1,L__zero
    je L__finish
    ucomisd                 xmm1, L__real_x_near0_threshold
    jb                            L__y_is_x

    mov rax ,001E0000000000000h     ;30 in exponents place
    ;u = (twop30.f64 * x + x) - twop30.f64 * x;    
    movd xmm1,rax
    paddq xmm1  ,xmm0;xmm1 = twop30.f64 * x
    movapd xmm2,xmm1
    addsd xmm2 ,xmm0;xmm2 = (twop30.f64 * x + x)
    subsd xmm2 ,xmm1;xmm2 = u
    movapd xmm1,xmm0
    subsd xmm1 ,xmm2;xmm1 = v = x-u
    movapd xmm3 ,xmm2;xmm3 = u
    mulsd xmm3 ,xmm2;xmm3 = u*u
    mulsd xmm3 ,L__real_1_by_2;xmm3 = y = u*u*0.5
    ;z = v * (x + u) * 0.5;
    movapd xmm4,xmm0
    addsd xmm4,xmm2
    mulsd xmm4,xmm1
    mulsd xmm4 ,L__real_1_by_2;xmm4 = z   
    
    ;q = x*x*x*(A1.f64 + x*(A2.f64 + x*(A3.f64 + x*(A4.f64 + x*(A5.f64 + x*(A6.f64 + x*(A7.f64 + x*(A8.f64 + x*(A9.f64)))))))));
    movapd xmm5,xmm0
    mulsd xmm5,L__B9
    addsd xmm5,L__B8
    mulsd xmm5,xmm0
    addsd xmm5,L__B7
    mulsd xmm5,xmm0
    addsd xmm5,L__B6
    mulsd xmm5           ,xmm0
    addsd xmm5,L__B5
    mulsd xmm5    ,xmm0
    addsd xmm5,L__B4
    mulsd xmm5    ,xmm0
    addsd xmm5,L__B3
    mulsd xmm5    ,xmm0
    addsd xmm5,L__B2
    mulsd xmm5   ,xmm0
    addsd xmm5,L__B1
    mulsd xmm5  ,xmm0
    mulsd xmm5  ,xmm0
    mulsd xmm5   ,xmm0;xmm5 = q 
     
    ucomisd xmm3    ,L__TwopM7
    jb L__returnNext
    addsd xmm1  ,xmm4;xmm1 = v+z
    addsd xmm1  ,xmm5;xmm1 = q+(v+z)
    addsd xmm2  ,xmm3;xmm2 = u+y
    addsd xmm1,xmm2
    movapd xmm0,xmm1
    jmp L__finish
	      
ALIGN 16
L__returnNext:
    addsd xmm4  ,xmm5;xmm4 = q +z
    addsd xmm3  ,xmm4;xmm3 = y+(q+z)
    addsd xmm0    ,xmm3
    jmp L__finish
    
ALIGN 16
L__y_is_inf:
    mov         rax,07ff0000000000000h
    movd        xmm1,rax
    mov        				r8d,	3											
    call        					fname_special
    jmp L__finish   

ALIGN 16
L__y_is_nan:
    movd      rax,xmm0
    or        rax, L__real_qnanbit
	movd      xmm1,rax
	mov         r8d,1
    call        fname_special
    jmp L__finish

ALIGN 16
L__y_is_minusOne:
    mov rax   ,0BFF0000000000000h;return -1
    movd  xmm0,rax
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
L__max_expm1_arg           DQ 040862E6666666666h    
L__min_expm1_arg           DQ 0C042B708872320E1h
L__log_OneMinus_OneByFour  DQ 0BFD269621134DB93h
L__log_OnePlus_OneByFour   DQ 03FCC8FF7C79A9A22h
L__real_64_by_log2         DQ 040571547652b82feh   ; 64/ln(2)
L__log2_by_64_mhead        DQ 0bf862e42fefa0000h
L__log2_by_64_mtail        DQ 0bd1cf79abc9e3b39h
L__TwopM7                  DQ 03F80000000000000h
L__One                     DQ 03FF0000000000000h
L__zero                    DQ 00000000000000000h
L__B9                      DQ 03E5A2836AA646B96h
L__B8                      DQ 03E928295484734EAh
L__B7                      DQ 03EC71E14BFE3DB59h
L__B6                      DQ 03EFA019F635825C4h
L__B5                      DQ 03F2A01A01159DD2Dh
L__B4                      DQ 03F56C16C16CE14C6h
L__B3                      DQ 03F8111111111A9F3h
L__B2                      DQ 03FA55555555554B6h
L__B1                      DQ 03FC5555555555549h
L__positive_infinity      DQ 07ff0000000000000h
L__mask_sign              DQ 07fffffffffffffffh
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit
L__real_x_near0_threshold DQ 3c00000000000000h

CONST    ENDS
data ENDS

END
    
