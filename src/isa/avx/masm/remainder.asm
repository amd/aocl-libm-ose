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

; remainder.S
;
; An implementation of the fabs libm function.
;
; Prototype:
;
;     double remainder(double x,double y);
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE remainder

;fname_special   TEXTEQU <_remainder_special>

;Define name and any external functions being called
;EXTERN       fname_special      : PROC


temp_x          EQU     20h
temp_y          EQU     30h
save_xmm6       EQU     40h
save_xmm7       EQU     50h
stack_size      EQU     0a8h 

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

SaveXmm         MACRO xmmreg, offset
                    movdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
                ENDM

fname_special   TEXTEQU <_remainder_special>
EXTERN fname_special:PROC

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    SaveXmm xmm7, save_xmm7
    .ENDPROLOG   
         
         movd   r8,   xmm0       
         movd   r9,   xmm1
         and    r8,   qword PTR L__Nan_64
         and    r9,   qword PTR L__Nan_64
		 cmp    r8,   qword PTR L__exp_mask_64
		 jge    L__InputXIsNaN_Inf
		 cmp    r9,   qword PTR L__exp_mask_64
		 jg     L__InputYIsNaN_Inf
		 cmp    r9,   qword PTR L__Zero_64
		 je     L__Y_is_Zero
		 cmp    r8,r9
		 jz     L__Input_Is_Equal

         movd   r8,   xmm0       
         movd   r9,   xmm1       
		 movsd  xmm2, xmm0       
         movsd  xmm3, xmm1       
         movsd  xmm4, xmm0       
         movsd  xmm5, xmm1       
         mov    r10,  L__exp_mask_64     
         and    r8,   r10        
         and    r9,   r10        
         xor    r10,  r10        
         ror    r8,   52        
         ror    r9,   52        
         cmp    r8,   0        
         jz     L__LargeExpDiffComputation
         cmp    r9,   0        
         jz     L__LargeExpDiffComputation
         sub    r8,   r9        ;
         cmp    r8,   52        
         jge    L__LargeExpDiffComputation
         pand   xmm4,   xmmword PTR  L__Nan_64      
         pand   xmm5, xmmword PTR L__Nan_64      
         comisd xmm4,  xmm5       
         jbe    L__ReturnImmediate
         cmp    r8,   07FFh       
		 jz     L__Dividend_Is_Infinity

ALIGN 16
L__DirectComputation:
         movapd xmm2,  xmm4       
         movapd xmm3,  xmm5       
         divsd  xmm2,  xmm3       
         cvttsd2si  r8,   xmm2       
         mov    r10,  r8        
         and    r10,  001h       
         cvtsi2sd  xmm2,  r8        

;multiplication in QUAD Precision
;Since the below commented multiplication resulted in an error
;we had to implement a quad precision multiplication
;logic behind Quad Precision Multiplication
;x = hx + tx   by setting x's last 27 bits to null
;y = hy + ty   similar to x
         movapd  xmm4,   L__27bit_andingmask_64   
         movapd  xmm1,   xmm5       ; x
         movapd  xmm6,   xmm2       ; y
         movapd  xmm7,   xmm2       ; z = xmm7
         mulpd   xmm7,   xmm5       ; z = x*y
         andpd   xmm1,   xmm4       
         andpd   xmm2,   xmm4       
         subsd   xmm5,   xmm1       ; xmm1 = hx   xmm5 = tx
         subsd   xmm6,   xmm2       ; xmm2 = hy   xmm6 = ty

         movapd  xmm4,   xmm1       ; copy hx
         mulsd   xmm4,   xmm2       ; xmm4 = hx*hy
         subsd   xmm4,   xmm7       ; xmm4 = (hx*hy - z)
         mulsd   xmm1,   xmm6       ; xmm1 = hx * ty
         addsd   xmm4,   xmm1       ; xmm4 = ((hx * hy - *z) + hx * ty)
         mulsd   xmm2,   xmm5       ; xmm2 = tx * hy
         addsd   xmm4,   xmm2       ; xmm4 = (((hx * hy - *z) + hx * ty) + tx * hy)
         mulsd   xmm6,   xmm5       ; xmm6 = tx * ty
         addsd   xmm6,   xmm4       ; xmm6 = (((hx * hy - *z) + hx * ty) + tx * hy) + tx * ty;
                                    ;xmm6 and xmm7 contain the quad precision result
                                    ;v = dx - c 
         movapd  xmm1,   xmm0       ; copy the input number
         pand    xmm1, xmmword PTR  L__Nan_64      
         movapd  xmm2,   xmm1       ; xmm2 = dx = xmm1
         subsd   xmm1,   xmm7       ; v = dx - c
         subsd   xmm2,   xmm1       ; (dx - v)
         subsd   xmm2,   xmm7       ; ((dx - v) - c)
         subsd   xmm2,   xmm6       ; (((dx - v) - c) - cc)
         addsd   xmm2,   xmm1       ; xmm2 = dx = v + (((dx - v) - c) - cc) 
                                    ; xmm3 = w
         movapd  xmm4,   xmm2       
         movapd  xmm5,   xmm3       
         addsd   xmm4,   xmm4       ; xmm4 = dx + dx
         comisd  xmm3,   xmm4       ; if (dx + dx > w)
         jb      L__Substractw
         mulpd   xmm5,   L__ZeroPointFive    ; xmm5 = 0.5 * w
         comisd  xmm5,   xmm2       ; if (dx > 0.5 * w)
         jb      L__Substractw
         cmp     r10,      001h     ; If the quotient is an odd number
         jnz     L__Finish
         comisd  xmm3,   xmm4       ;if (todd && (dx + dx == w)) then subtract w
         jz      L__Substractw
         comisd  xmm5,   xmm0       ;if (todd && (dx == 0.5 * w)) then subtract w
         jnz     L__Finish

ALIGN 16
L__Substractw:
         subsd   xmm2,    xmm3      ; dx -= w

; The following code checks the sign of the input number and then calculate the return Value
;  return x < 0.0? -dx : dx ;
L__Finish:
         comisd  xmm0,   L__Zero_64      
         ja      L__Not_Negative_Number1

ALIGN 16
L__Negative_Number1:
         movapd  xmm0,  L__Zero_64      
         subsd   xmm0,  xmm2       
         movdqa    xmm7, XMMWORD PTR [save_xmm7+rsp]
         movdqa    xmm6, XMMWORD PTR [save_xmm6+rsp]
         add       rsp, stack_size       
         ret  

ALIGN 16   
L__Not_Negative_Number1:
         movapd  xmm0,   xmm2       
         movdqa    xmm7, XMMWORD PTR [save_xmm7+rsp]
         movdqa    xmm6, XMMWORD PTR [save_xmm6+rsp]
         add       rsp, stack_size       
         ret    

    ;calculation using the x87 FPU
    ;For numbers whose exponent of either of the divisor,
    ;or dividends are 0. Or for numbers whose exponential 
    ;diff is grater than 52
ALIGN 16
L__LargeExpDiffComputation:
       movsd  QWORD PTR [temp_x+rsp], xmm0        
       movsd  QWORD PTR [temp_y+rsp], xmm1
       ffree   st(0)
       ffree   st(1)         
       fld  QWORD PTR [temp_y+rsp]
       fld  QWORD PTR [temp_x+rsp]
       fnclex

ALIGN 16
L__repeat:    
       fprem1 ;Calculate remainder by dividing st(0) with st(1)
              ;fprem operation sets x87 condition codes, 
              ;it will set the C2 code to 1 if a partial remainder is calculated
       fnstsw ax
       and    ax,   00400h ; Stores Floating-Point Store Status Word into the accumulator
                           ; we need to check only the C2 bit of the Condition codes
       cmp    ax,   00400h ; Checks whether the bit 10(C2) is set or not 
                           ;IF its set then a partial remainder was calculated
       jz     L__repeat
       ;store the result from the FPU stack to memory
       fstp   QWORD PTR [temp_x+rsp]
       fstp   QWORD PTR [temp_y+rsp]
       movsd  xmm0,   QWORD PTR [temp_x+rsp]
       movdqa xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size
       ret     

ALIGN 16
;IF both the inputs are equal
L__Input_Is_Equal:
       movsd  xmm1,  xmm0       
       pand   xmm1, xmmword PTR L__sign_mask_64     
       movsd  xmm0,  L__Zero_64      
       por    xmm0,  xmm1       
       movdqa xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size
       ret

ALIGN 16
L__InputXIsNaN_Inf:
;Here x is nan or infinity
      je  L__Dividend_Is_Infinity
;Check if either x is a signalling nan
	  cmp r8, L__Qnan
	  jl  L__InputIsSNaN
;Check if y is a NaN
	  cmp    r9,   qword PTR L__exp_mask_64
	  jg     L__InputYIsNaN_Inf
	  add    rsp, stack_size
      ret

ALIGN 16
L__InputYIsNaN_Inf:
;Check if y is a signalling snan
	cmp r9, L__Qnan
	jl  L__InputIsSNaN
    movapd xmm0, xmm1
	add    rsp, stack_size
    ret

ALIGN 16
L__InputIsSNaN:
;Raise exception and return snan
       mov    r8d, DWORD PTR L__flag_x_nan
	   call   fname_special
       por    xmm0, xmmword PTR L__Snan
       add    rsp, stack_size
       ret


ALIGN 16
L__Dividend_Is_Infinity:
       movapd xmm1,xmm0
       por    xmm1, XMMWORD PTR L__QNaN_mask_64
       mov    r8d, DWORD PTR L__flag_x_Dividend_Inf
	   call   fname_special
       movdqa xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size       
       ret 

ALIGN 16
;Case when x < y
L__ReturnImmediate:
       movapd  xmm7,   xmm5       
       mulpd   xmm5,   L__ZeroPointFive    ; 
       comisd  xmm5,  xmm4       
       jae     L__FoundResult1
       subsd   xmm4,  xmm7       
       comisd  xmm0,  L__Zero_64      
       ja      L__Not_Negative_Number

ALIGN 16
L__Negative_Number:
       movapd  xmm0,  L__Zero_64      
       subsd   xmm0,  xmm4       
       movdqa  xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa  xmm6, XMMWORD PTR [save_xmm6+rsp]
       add     rsp, stack_size       
       ret    

ALIGN 16
L__Not_Negative_Number:
       movapd  xmm0,  xmm4       
       movdqa  xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa  xmm6, XMMWORD PTR [save_xmm6+rsp]
       add     rsp, stack_size       
       ret 

ALIGN 16   
L__FoundResult1:
       movdqa  xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa  xmm6, XMMWORD PTR [save_xmm6+rsp]
       add     rsp, stack_size       
       ret 

ALIGN 16
L__Y_is_Zero:
       movapd xmm1, QWORD PTR L__Qnan
       mov    r8d, DWORD PTR L__flag_y_zero
	   call   fname_special
       movdqa xmm7, XMMWORD PTR [save_xmm7+rsp]
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size       
       ret 
           

fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT

ALIGN 16    
L__flag_x_nan            DD 00000001
L__flag_y_zero           DD 00000002
L__flag_x_Dividend_Inf   DD 00000003

ALIGN 16   
L__sign_mask_64          DQ 08000000000000000h
                         DQ 00h
L__exp_mask_64           DQ 07FF0000000000000h
                         DQ 00h
L__27bit_andingmask_64   DQ 0fffffffff8000000h
                         DQ 00h
L__2p52_mask_64          DQ 04330000000000000h 
                         DQ 00h
L__Zero_64               DQ 00h 
                         DQ 00h
L__QNaN_mask_64          DQ 00008000000000000h 
                         DQ 00h
L__Nan_64                DQ 07FFFFFFFFFFFFFFFh
                         DQ 00h
L__ZeroPointFive         DQ 03FE0000000000000h
                         DQ 00h
L__Qnan                  DQ 07FF8000000000000h
                         DQ 07FF8000000000000h
L__Snan                  DQ 07FF0000000000001h
						 DQ 00h
CONST    ENDS
data ENDS

END
