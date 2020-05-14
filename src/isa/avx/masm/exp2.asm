;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

include exp_tables.inc
include fm.inc

FN_PROTOTYPE_BAS64 exp2

fname_special   TEXTEQU <_exp2_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG  

    movdqa                  xmm1, xmm0
    andpd                   xmm1, L__positive_infinity
    ucomisd                 xmm1, L__positive_infinity
    je                      L__x_is_nan_or_inf    
    
    ucomisd    						xmm0,	L__max_exp2_arg				
    ja         								L__y_is_inf
    ucomisd    						xmm0,	L__min_exp2_arg				
    jbe        								L__y_is_zero

    movdqa                  xmm1, xmm0
    psllq                   xmm1, 1
    psrlq                   xmm1, 1    
    ucomisd                 xmm1, L__real_x_near0_threshold
    jb                            L__y_is_one
    
    ;x * (64)
    movapd     						xmm2,   xmm0								
    mulsd      						xmm2,	L__real_64					

    																				;n = int( x * (64))
    cvttpd2dq  						xmm1,   xmm2									;xmm1 = (int)n
    cvtdq2pd   						xmm2,   xmm1									;xmm2 = (double)n
    movd       						ecx,	xmm1								

    																				;r = x - n * 1/64  
    																				;r *= ln2																																		;      
    mulsd   						xmm2,  L__one_by_64					
    addsd   						xmm2,  xmm0									;xmm2 = r    
    mulsd   						xmm2,  L__ln_2						

    																				;j = n & 0x3f    
    mov        						rax,   03fh								
    and        						eax,   ecx									;eax = j
    																				;m = (n - j) / 64      
    sar        						ecx,   6								 		;ecx = m

    																				;q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
    																				;q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
    movapd     						xmm3,  L__real_1_by_720					;xmm3 = 1/720
    mulsd      						xmm3,  xmm2									;xmm3 = r*1/720
    movapd     						xmm0,  L__real_1_by_6					;xmm0 = 1/6    
    movapd     						xmm1,  xmm2									;xmm1 = r            
    mulsd      						xmm0,  xmm2									;xmm0 = r*1/6
    addsd      						xmm3,  L__real_1_by_120					;xmm3 = 1/120 + (r*1/720)
    mulsd      						xmm1,  xmm2									;xmm1 = r*r    
    addsd      						xmm0,  L__real_1_by_2					;xmm0 = 1/2 + (r*1/6)        
    movapd     						xmm4,  xmm1									;xmm4 = r*r
    mulsd      						xmm4,  xmm1									;xmm4 = (r*r) * (r*r)    
    mulsd      						xmm3,  xmm2									;xmm3 = r * (1/120 + (r*1/720))
    mulsd      						xmm0,  xmm1									;xmm0 = (r*r)*(1/2 + (r*1/6))
    addsd      						xmm3,  L__real_1_by_24					;xmm3 = 1/24 + (r * (1/120 + (r*1/720)))
    addsd      						xmm0,  xmm2									;xmm0 = r + ((r*r)*(1/2 + (r*1/6)))
    mulsd      						xmm3,  xmm4									;xmm3 = ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))
    addsd      						xmm0,  xmm3									;xmm0 = r + ((r*r)*(1/2 + (r*1/6))) + ((r*r) * (r*r)) * (1/24 + (r * (1/120 + (r*1/720))))

    																				;(f)*(q) + f2 + f1
    cmp        						ecx,   0fffffc02h								;-1022    
    lea        						rdx,   L__two_to_jby64_table		
    lea        						r11,   L__two_to_jby64_tail_table	
    lea        						r10,   L__two_to_jby64_head_table	
    mulsd      						xmm0,  QWORD PTR [rdx + rax * 8]						
    addsd      						xmm0,  QWORD PTR [r11 + rax * 8]						
    addsd      						xmm0,  QWORD PTR [r10 + rax * 8]						

    jle        								L__process_denormal 
L__process_normal:
    shl         					rcx,   52								
    movd        					xmm2,	rcx								
    paddq       					xmm0,	xmm2								
    add         rsp, stack_size
    ret

ALIGN 16
L__process_denormal:
    jl          							L__process_true_denormal
    ucomisd     					xmm0,	L__real_one					
    jae         							L__process_normal
L__process_true_denormal:
    																				;here ( e^r < 1 and m = -1022 ) or m <= -1023
    add        						ecx,    1074								
    mov        						rax,    1									
    shl        						rax,	cl								
    movd       						xmm2,	rax								
    mulsd      						xmm0,	xmm2								
    jmp         L__finish      

L__y_is_one:
    movsd       xmm0, L__real_one
    jmp         L__finish
    
ALIGN 16 
L__x_is_nan_or_inf:
    movd        rax, xmm0    
    cmp         rax, L__positive_infinity
    je          L__finish
    cmp         rax, L__negative_infinity
    je          L__return_zero_without_exception    
    or          rax, L__real_qnanbit
    movd        xmm1, rax
    mov         r8d, 1
    call        fname_special
    jmp         L__finish 
    
ALIGN 16 
L__y_is_inf:
    mov         			rax,	07ff0000000000000h    						
    movd       				xmm1,	rax										
    mov        				r8d,	3											
    call        					fname_special
    jmp       L__finish   

ALIGN 16      
L__y_is_zero:
    pxor        			xmm1,	xmm1										
    mov         			r8d,	2											
    call        					fname_special
    jmp     L__finish
 
ALIGN 16    
L__return_zero_without_exception:
    pxor xmm0,xmm0 

ALIGN 16    
L__finish:
    add         rsp, stack_size
    ret
    
fname        endp
TEXT    ENDS
    
data SEGMENT READ

CONST    SEGMENT
  

ALIGN 16
L__max_exp2_arg            DQ 04090000000000000h
L__min_exp2_arg            DQ 0c090c80000000000h
L__real_64                 DQ 04050000000000000h    ; 64
L__ln_2                    DQ 03FE62E42FEFA39EFh
L__one_by_64               DQ 0bF90000000000000h
L__negative_infinity      DQ 0fff0000000000000h
L__positive_infinity      DQ 07ff0000000000000h
L__real_qnanbit           DQ 0008000000000000h    ; qnan set bit
L__real_x_near0_threshold DQ 3c00000000000000h

ALIGN 16
L__real_one                   DQ 03ff0000000000000h

CONST    ENDS
data ENDS

END
