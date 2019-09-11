include exp_tables.inc
include fm.inc

FN_PROTOTYPE_BAS64 exp10f

fname_special   TEXTEQU <_exp10f_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

stack_size      EQU     88h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG  
    
	movdqa                  xmm1, xmm0
    andps                   xmm1, L__positive_infinity
    ucomiss                 xmm1, L__positive_infinity
    je                      L__x_is_nan_or_inf  
	    
    ucomiss 			xmm0,		L__max_exp_arg							
    jae								L__y_is_inf
    ucomiss				xmm0,		L__min_exp_arg							
    jb 								L__y_is_zero

    cvtps2pd    		xmm0,    	xmm0									;xmm0 = (double)x

    																		; x * (64/log10of(2))
    movapd      		xmm3,      	xmm0									;xmm3 = (xouble)x
    mulsd       		xmm3,  		L__real_64_by_log10of2					;xmm3 = x * (64/ln(2)

    																		; n = int( x * (64/log10of(2)) )
    cvtpd2dq    		xmm4,  		xmm3									;xmm4 = (int)n
    cvtdq2pd    		xmm2,  		xmm4									;xmm2 = (double)n

    																		; r = x - n * ln(2)/64
    																		; r *= ln(10)
    mulsd       		xmm2, 		L__real_log10of2_by_64					;xmm2 = n * log10of(2)/64
    movd        		ecx,     	xmm4									;ecx = n
    subsd       		xmm0,    	xmm2									;xmm0 = r
    mulsd       		xmm0, 		L__real_ln10							;xmm0 = r = r*ln10
    movapd      		xmm1,    	xmm0									;xmm1 = r

    																		; q = r + r*r(1/2 + r*1/6)
    movapd      		xmm3, 		L__real_1_by_6							
    mulsd       		xmm3, 		xmm0									;xmm3 = 1/6 * r
    mulsd       		xmm0, 		xmm1									;xmm0 =  r  * r
    addsd       		xmm3, 		L__real_1_by_2							;xmm3 = 1/2 + (1/6 * r)
    mulsd       		xmm0,  		xmm3									;xmm0 = r*r*(1/2 + (1/6 * r))
    addsd       		xmm0,  		xmm1									;xmm0 = r+r*r*(1/2 + (1/6 * r))

    																		;j = n & 0x3f
    mov         		rax,     	03fh									;rax = 0x3f
    and         		eax,      	ecx										;eax = j = n & 0x3f

    																		; f + (f*q)
    lea         		r10,    	L__two_to_jby64_table					
    mulsd       		xmm0,		QWORD PTR [r10+rax*8]					
    addsd       		xmm0,		QWORD PTR [r10+rax*8]					

    ALIGN 16
    																		; m = (n - j) / 64        
    psrad       		xmm4,		6										
    psllq       		xmm4,		52									
    paddq       		xmm4,		xmm0									
    cvtpd2ps    		xmm0,		xmm4									
    add         rsp, stack_size
    ret       

ALIGN 16 
L__x_is_nan_or_inf:
    movd        eax, xmm0    
    cmp         eax, L__positive_infinity
    je          L__finish
    cmp         eax, L__negative_infinity
    je          L__return_zero_without_exception    
    or          eax, L__real_qnanbit
    movd        xmm1, eax
    mov         r8d, 1
    call        fname_special
    jmp         L__finish 

ALIGN 16			
L__y_is_zero:
    pxor       		xmm1,   	xmm1				;return value in xmm1,input in xmm0 before calling
    mov        		r8d,    	2					;code in edi
    call      				  fname_special
    jmp                 L__finish       			

ALIGN 16
L__y_is_inf:
    mov        		edx,		07f800000h		
    movd       		xmm1,		edx				
    mov        	r8d,		3					
    call      				  fname_special
    jmp                 L__finish     

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
L__max_exp_arg                 DD 0421A209Bh
L__min_exp_arg                 DD 0C23369F4h
L__real_64_by_log10of2        DQ 0406A934F0979A371h ; 64/log10(2)
L__real_log10of2_by_64        DQ 03F734413509F79FFh ; log10of2_by_64
L__real_ln10                  DQ 040026BB1BBB55516h ; ln(10)
L__positive_infinity          DD 07f800000h
L__negative_infinity          DD 0ff800000h
L__real_qnanbit               DD 00400000h


CONST    ENDS
data ENDS

END

