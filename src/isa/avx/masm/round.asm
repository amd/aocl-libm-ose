;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

; round.S
;
; An implementation of the round libm function.
;
; Prototype:
;
;     double round(double x);
;

;
;   Algorithm:
;




include fm.inc
FN_PROTOTYPE round

fname_special   TEXTEQU <_round_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG   
								movsd			xmm4,				L__2p52_plus_one			
								movsd			xmm5,				L__sign_mask_64				
								mov				r10,				52							
								movsd			xmm1,				xmm0						
								movsd			xmm2,				xmm0						
								movsd			xmm3,				xmm0						

								pand			xmm1,				XMMWORD PTR L__exp_mask_64						
								pextrw			r9,xmm1,			3								
								cmp				r9,					07FF0h						
								jz									L__is_infinity_or_nan
                                movsd			xmm1,				QWORD PTR L__sign_mask_64	
								pandn			xmm1,				xmm2						
								shr				r9,					04h						
								sub				r9,					03FFh						
								cmp				r9,					00h						
								jl									L__number_less_than_zero

L__number_greater_than_zero:
								cmp				r9,					51							
								jg									L__is_greater_than_2p52

								pand			xmm0,	xmmword PTR L__sign_mask_64				
								addsd			xmm0,				L__zero_point_5				
								movsd			xmm5,				xmm0						

								pand			xmm5,				XMMWORD PTR L__exp_mask_64						
								pand			xmm0,	xmmword PTR L__mantissa_mask_64			
								sub				r10,				r9							
								movd			xmm2,				r10		
								psrlq			xmm0,				xmm2						
								psllq			xmm0,				xmm2						
								por				xmm5,				xmm1						
								por				xmm0,				xmm5						
								add      rsp, stack_size
	                            ret		

L__number_less_than_zero:
								pand			xmm3,				xmm5							
								addsd			xmm3,				xmm4						
								subsd			xmm3,				xmm4						
								por				xmm3,				xmm1						
								movsd			xmm0,				xmm3						
								add      rsp, stack_size
								ret		

L__is_infinity_or_nan:
								movd     rax,xmm0
								and      rax,L__mask_sign
								cmp      rax,L__positive_infinity
								ja       L__x_is_nan
								movdqa      xmm1, xmm0
								call        fname_special
								add         rsp, stack_size
								ret

L__x_is_nan :
								movd        rax, xmm0    
								or          rax, L__real_qnanbit
								movd        xmm1, rax
								call        fname_special
								add         rsp, stack_size
								ret
L__is_zero :
L__is_greater_than_2p52:    
								add      rsp, stack_size
								ret		

fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT

ALIGN 16
L__sign_mask_64          DQ 07FFFFFFFFFFFFFFFh
                           DQ 00h

L__qnan_mask_64          DQ 00008000000000000h
                           DQ 00h
L__exp_mask_64           DQ 07FF0000000000000h
                           DQ 00h
L__mantissa_mask_64      DQ 0000FFFFFFFFFFFFFh
                           DQ 00h
L__zero                  DQ 00000000000000000h
                           DQ 00h
L__2p52_plus_one         DQ 04330000000000001h  ; = 4503599627370497.0
                           DQ 00h
L__zero_point_5          DQ 03FE0000000000001h  ; = 00.5
L__positive_infinity      DQ 07ff0000000000000h
L__mask_sign              DQ 07fffffffffffffffh
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit


CONST    ENDS
data ENDS

END


