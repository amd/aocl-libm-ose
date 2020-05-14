;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

; remainderf.S
;
; An implementation of the fabs libm function.
;
; Prototype:
;
;     float remainderf(float x,float y);
;

;
;   Algorithm:
;

include fm.inc
FN_PROTOTYPE remainderf


;fname_special   TEXTEQU <_remainderf_special>

;Define name and any external functions being called
;EXTERN       fname_special      : PROC


save_xmm6       EQU     40h
stack_size      EQU     98h 

StackAllocate   MACRO size
    sub         rsp, size
    .ALLOCSTACK size
ENDM

SaveXmm         MACRO xmmreg, offset
                    movdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
                ENDM

fname_special   TEXTEQU <_remainderf_special>
EXTERN fname_special:PROC

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    .ENDPROLOG     
    

       mov      rcx, L__exp_mask_64  
       movapd   xmm5, L__sign_mask_64
      
       movd     r8d, xmm0 
       movd     r9d, xmm1
       and      r8d, dword PTR L__NotSign_mask
       and      r9d, dword PTR L__NotSign_mask
       jz       L__Y_is_Zero
       cmp      r8d, dword PTR L__exp_mask_32
       jge      L__InputIsNaN_Inf
       cmp      r9d, dword PTR L__exp_mask_32
       jg       L__InputYIsNaN_Inf
       cmp      r8d,r9d
       jz       L__Input_Is_Equal

       cvtss2sd xmm2, xmm0 ; double x
       cvtss2sd xmm3, xmm1 ; double y
       pand     xmm2, xmm5   
       pand     xmm3, xmm5   
       movd     rax, xmm2 
       movd     r8,  xmm3
       mov      r11, rax 
       mov      r9,  r8 
       movsd    xmm4, xmm2
       ;take the exponents of both x and y
       and    rax, rcx 
       and    r8,  rcx 
       ror    rax, 52 
       ror    r8,  52 
       ;ifeither of the exponents is infinity 
       ;cmp    rax, 07FFh
       ;jz     L__InputIsNaN
       ;cmp    r8,  07FFh 
       ;jz     L__InputIsNaNOrInf

       ;cmp    r8,  0
       ;jz     L__Divisor_Is_Zero

       cmp    r11, r9 
       ;jz     L__Input_Is_Equal
       jb     L__ReturnImmediate

       xor    rcx, rcx 
       mov    rdx, 24 
       movsd  xmm6, L__One_64 ; xmm6 = scale
       cmp    r8,  rax 
       jae    L__y_is_greater
       sub    rax, r8  
       div    dl         ; al = ntimes
       mov    cl,  al    ; cl = ntimes
       and    ax,  0FFh  ; set everything t o zero except al
       mul    dl         ; ax = dl * al = 24* ntimes
       add    rax, 1023 
       shl    rax, 52 
       movd   xmm6, rax  ; xmm6 = scale

ALIGN 16       
L__y_is_greater:
       mulsd  xmm6, xmm3 ; xmm6 = scale * dy
       movsd  xmm5, L__2pminus24_decimal  

ALIGN 16
L__Start_Loop:
       dec      cl
       js      L__End_Loop
       divsd   xmm4, xmm6      ; xmm6 = (dx / w)
       cvttsd2si rax, xmm4      
       cvtsi2sd  xmm4, rax     ; xmm4 = t = (double)((int)(dx / w))
       mulsd   xmm4, xmm6      ; xmm4 = w*t
       mulsd   xmm6, xmm5      ; w*= scale 
       subsd   xmm2, xmm4      ; xmm2 = dx -= w*t  
       movsd   xmm4, xmm2      ; xmm4 = dx
       jmp     L__Start_Loop
L__End_Loop:    
       divsd   xmm4, xmm6      ; xmm6 = (dx / w)
       cvttsd2si  rax, xmm4  
       cvtsi2sd  xmm4, rax     ; xmm4 = t = (double)((int)(dx / w))
       and     rax, 001h        ; todd = todd = ((int)(dx / w)) & 1 
       mulsd   xmm4, xmm6      ; xmm4 = w*t
       subsd   xmm2, xmm4      ; xmm2 = dx -= w*t  
       movsd   xmm5, xmm6      ; store w
       mulsd   xmm6, L__Zero_Point_Five64  ;xmm6 = 0.5*w

       cmp     rax, 001h     
       jnz     L__todd_is_even
       comisd  xmm6, xmm2     
       je      L__Subtract_w

L__todd_is_even: 
       comisd  xmm6, xmm2     
       jnb     L__Dont_Subtract_w

L__Subtract_w:    
       subsd   xmm2, xmm5     

L__Dont_Subtract_w:
       comiss  xmm0, DWORD PTR L__Zero_64
       jb      L__Negative
       cvtsd2ss xmm0, xmm2 

       movdqa   xmm6, XMMWORD PTR [save_xmm6+rsp]
       add      rsp, stack_size                       
       ret

L__Negative:
       movsd    xmm0, L__MinusZero_64    
       subsd    xmm0, xmm2      
       cvtsd2ss xmm0, xmm0  

       movdqa   xmm6, XMMWORD PTR [save_xmm6+rsp]
       add      rsp, stack_size                       
       ret

ALIGN 16
L__Input_Is_Equal:
       ;cmp    rax, 07FFh      
       ;jz     L__Dividend_Is_Infinity
       ;cmp    r8,  07FFh      
       ;jz     L__InputIsNaNOrInf

       movsd  xmm1, xmm0      
       pand   xmm1, xmmword PTR L__sign_bit_32    
       movss  xmm0, DWORD PTR L__Zero_64     
       por    xmm0, xmm1

       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size                       
       ret

ALIGN 16
L__InputIsNaN_Inf:
;Here x is nan so we move to xmm1
       movaps xmm1, xmm0
ALIGN 16
L__InputYIsNaN_Inf:
       ;Here y is nan 
       por    xmm1, XMMWORD PTR  L__Qnan
       mov    r8d, DWORD PTR L__flag_x_nan
       call   fname_special
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size       
       ret 


ALIGN 16
L__Dividend_Is_Infinity:
       movaps xmm1,xmm0
       por    xmm1, XMMWORD PTR L__Qnan
       mov    r8d, DWORD PTR L__flag_x_Dividend_Inf
       call   fname_special
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size       
       ret 



;Case when x < y
;xmm2 = dx
ALIGN 16       
L__ReturnImmediate:
       movsd   xmm5, xmm3      
       mulsd   xmm3, L__Zero_Point_Five64  ; xmm3 = 0.5*dy
       comisd  xmm2, xmm3      ; if (dx > 0.5*dy)
       jna     L__Finish_Immediate   ; xmm2 <= xmm3
       subsd   xmm2, xmm5      ;dx -= dy

ALIGN 16       
L__Finish_Immediate:
       comiss  xmm0, DWORD PTR L__Zero_64     
                    ;xmm0 contains the input and is the result
       jz      L__Zero
       ja      L__Positive
       movsd   xmm0, L__Zero_64     
       subsd   xmm0, xmm2      
       cvtsd2ss  xmm0, xmm0 
L__Zero:
       movdqa  xmm6, XMMWORD PTR [save_xmm6+rsp]
       add     rsp, stack_size                       
       ret

L__Positive:
       cvtsd2ss  xmm0, xmm2 
       movdqa    xmm6, XMMWORD PTR [save_xmm6+rsp]
       add       rsp, stack_size                       
       ret
 
ALIGN 16
L__Y_is_Zero:
       movaps xmm1, XMMWORD PTR L__Qnan
       mov    r8d, DWORD PTR L__flag_y_zero
	   call   fname_special
       movdqa xmm6, XMMWORD PTR [save_xmm6+rsp]
       add    rsp, stack_size       
       ret 
           
   
fname  endp
TEXT   ENDS
    
    
data SEGMENT  READ

CONST    SEGMENT


ALIGN 16    
L__flag_x_nan            DD 00000001
L__flag_y_zero           DD 00000002
L__flag_x_Dividend_Inf   DD 00000003

ALIGN 16   
L__sign_bit_32           DQ 08000000080000000h
                         DQ 00h
L__exp_mask_64           DQ 07FF0000000000000h
                         DQ 00h
L__exp_mask_32           DQ 0000000007F800000h
                         DQ 00h
L__27bit_andingmask_64   DQ 0fffffffff8000000h
                         DQ 00h
L__2p52_mask_64          DQ 04330000000000000h
                         DQ 00h
L__One_64                DQ 03FF0000000000000h 
                         DQ 00h
L__Zero_64               DQ 00h 
                         DQ 00h
L__MinusZero_64          DQ 08000000000000000h 
                         DQ 00h
L__QNaN_bit_32           DQ 00000000000400000h
                         DQ 00h
L__Qnan                  DQ 07fc000007fc00000h
                         DQ 07fc000007fc00000h
L__sign_mask_64          DQ 07FFFFFFFFFFFFFFFh
                         DQ 00h
L__2pminus24_decimal     DQ 03E70000000000000h
                         DQ 00h
L__Zero_Point_Five64     DQ 03FE0000000000000h
                         DQ 00h
L__NotSign_mask          DQ 07FFFFFFF7FFFFFFFh
                         DQ 07FFFFFFF7FFFFFFFh
CONST    ENDS
data ENDS

END
