;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

; powf.asm
;
; An implementation of the powf libm function.
;
; Prototype:
;
;     float powf(float x, float y);
;
;
;
;   Algorithm:
;       x^y = e^(y*ln(x))
;
;       Look in exp, log for the respective algorithms
;

include fm.inc
FN_PROTOTYPE_FMA3 powf


fname_special   TEXTEQU <_powf_special>


save_xmm6       EQU     0b0h
save_xmm7       EQU     0c0h

stack_size      EQU     0D8h

; external function
EXTERN fname_special:PROC

; macros

StackAllocate   MACRO size
                    sub         rsp, size
                    .ALLOCSTACK size
                ENDM

SaveXmm         MACRO xmmreg, offset
                    vmovdqa      XMMWORD PTR [offset+rsp], xmmreg
                    .SAVEXMM128 xmmreg, offset
                ENDM


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
	StackAllocate stack_size
    SaveXmm xmm6, save_xmm6
    SaveXmm xmm7, save_xmm7
    .ENDPROLOG

    vpunpckldq  xmm1, xmm1,xmm0  ;xmm1 = XX,XX,x,y
    vpxor       xmm5, xmm5,xmm5  ;xmm5 = will contain sign of final result
    vmovd       eax,xmm1  ;eax  = y
    vmovd       ecx,xmm0  ;ecx  = x
    mov        r9d,ecx 
    
    and        eax,07fffffffh	 ;sign of y removed
    and        ecx,07fffffffh	;sign of x removed    
    cmp        eax,07f800000h
    jae        L__y_is_inf_or_nan
    cmp        eax,03F800000h
    jle        L__check_if_y_is_zero_or_pos_one
L__y_is_not_zero_or_pos_one:   
    cmp        ecx,07f800000h
    jae        L__x_is_inf_or_nan

    ;calculate mx and my to check if result goes above inf
    ;mx *2 power my 
    ;
    ; -----------------------------
    ; compute log(x) here
    ; -----------------------------


    ;dy = (double)y  ,  dx = (double)x
    vcvtps2pd xmm0,xmm1	;xmm0 = x,y
    cmp        r9d,03F880000h
    jl         L__x_is_neg_or_zero_or_near_one
    
L__compute_logx:
    ;calculate F and Y
    ;eight_mant_bits = dx.u64 & 0000ff00000000000h;
    ;ninth_mant_bit = dx.u64 &  00000080000000000h;
    ;index = eight_mant_bits + (ninth_mant_bit << 1);
    ;F.u64 = index | 03fe0000000000000h; // 1/2
    ;index = index >> 44;
    ;uxexp = ((dx.u64 & 07ff0000000000000h) >> 52) - 1023;
    ;xmantissa_bits = dx.u64 & 0000FFFFFFFFFFFFFh;
    ;dxexp = (double)uxexp;
    ;Y.u64 = xmantissa_bits | 03fe0000000000000h;//1/2

    vpshufd     xmm2,xmm0,238		;xmm2 = x,x 
    vpand       xmm2, xmm2, XMMWORD PTR L__xexp_bits_xmantissa_bits	;xmm2 = xexp_bits, xmantissa_bits  
    vmovq       rcx,xmm2
    shr         rcx,44
    adc         ecx,0
    mov         eax,ecx
    or          ecx,00003fe00h
    shl         rcx,44
    vmovq       xmm1,rcx ;F
    vpor        xmm2,xmm2, XMMWORD PTR L__1_by_2 ;xexp_bits, Y
    vpshufd     xmm3,xmm2,238		;xmm3 = xexp_bits,xexp_bits
    vpsrlq      xmm3,xmm3,52

    vpsubq      xmm3,xmm3,L__1023 
    vcvtdq2pd   xmm3,xmm3
    lea         rcx, QWORD PTR L__log_F_inv
    
    ;xmm2 = Y, xmm1 = F, xmm3 = double m 
    ;
    ; = F - Y;

    vsubsd     xmm1, xmm1,xmm2    ;xmm1 = f
    
    ;r = f * L__log_F_inv[index]
    vmulsd     xmm1, xmm1, QWORD PTR [rcx+rax*8]     ;xmm1 = XX,r
    vmovapd    xmm2,xmm1    ;xmm2 = r
    
    ;q = poly = r + r*r*(1/2 + 1/3 * r);
    vmovsd       xmm4,L__1_by_3
    vmulsd       xmm4, xmm4, xmm1
    vmulsd       xmm1, xmm1,xmm2	;xmm1 =  r  * r
	vaddsd       xmm4, xmm4,L__1_by_2
     
      
	vfmadd213sd  xmm1, xmm4,xmm2
    
    ;logx = L__log2 * m + L__log_256_table[index] - poly;
   
    lea       rcx, QWORD PTR L__log_256_table
  
	vmovsd     xmm7, L__log2
	vfmadd213sd xmm3, xmm7, QWORD PTR [rcx+rax*8]
    vsubsd     xmm3, xmm3,xmm1

L__return_from_near_one:    
    ;ylogx = ((double)y) * logx;
    vmulsd    xmm0, xmm0,xmm3     ;xmm0 = ylogx

    ; -----------------------------
    ; compute exp( y * ln(x) ) here
    ; xmm0 = XX,ylogx
    ; -----------------------------
    
    vucomisd      xmm0,L__max_exp_arg 
    ja           L__y_is_inf
    vucomisd      xmm0,L__min_exp_arg 
    jbe          L__y_is_zero
    
    ; x * (64/ln(2))
   
    vmulpd       xmm3, xmm0,L__real_64_by_log2   ;xmm3 = x * (64/ln(2)

    ; n = int( x * (64/ln(2)) )
    vcvtpd2dq    xmm4,xmm3  ;xmm4 = (int)n
    vcvtdq2pd    xmm2,xmm4  ;xmm2 = (double)n

    ; r = x - n * ln(2)/64
    
    vmovd        ecx ,xmm4    ;ecx = n
   
	vfnmadd231sd   xmm0, xmm2, L__real_log2_by_64
    vmovapd      xmm1,xmm0    ;xmm1 = r

    ; q = r+r*r*(1/2 + (1/6 * r))
    vmovsd       xmm3,L__1_by_6 
  
    vmulsd       xmm0, xmm0,xmm1  ;xmm0 =  r  * r
 
	vfmadd213sd  xmm3, xmm0,L__1_by_2
    
	vfmadd213sd  xmm0, xmm3, xmm1
    
    ;j = n & 03fh
    mov         rax,03fh     ;rax = 03fh
    and         eax,ecx       ;eax = j = n & 03fh

    ; (f)*(1+q)
    lea         rcx,L__two_to_jby64_table   
   
   
	vmovsd        xmm7, QWORD PTR [rcx+rax*8]
	vfmadd213sd   xmm0, xmm7,QWORD PTR [rcx+rax*8]
    
     
    ALIGN 16
    vpsrad       xmm4, xmm4,6  
    vpsllq       xmm4, xmm4,52    
    vpaddq       xmm4, xmm4,xmm0
    vcvtsd2ss    xmm0, xmm0,xmm4
    vorps        xmm0, xmm0,xmm5  ;negate the result if x negative 
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret
    
ALIGN 16
L__x_is_neg_or_zero_or_near_one:
    cmp r9d,00h
    jg  L__x_near_one
    cmp ecx,00h
    je L__x_is_zero

    ;;else x is negative
    ;;check if y is int or not
    and eax,07f800000h
    cmp eax,04b000000h    
    jg L__x_near_one	;=>y is even int > 2p23       
    vcvtss2si edx ,xmm1   
    vcvtsi2ss xmm2, xmm2,edx 
    vucomiss  xmm2,xmm1
    jne L__return_qnan ;pow(-,y not an integer)=qnan 
    ;;here => y is int, check odd or even

    rcr edx,1 ;rotate through carry to check last bit
    jnc L__x_near_one ;;here => y is even int
    ;;else y is odd int, just negate x
    vmovss xmm5, DWORD PTR L__neg_zero ;set the sign bit

L__x_near_one:
    ; special case divert
    ;cmp        $03F800000h,%r9d #if x=1
    ;je         L__return_pos_one   
    ;check for x=+1 or -1
    ;pow(+1,any y) = +1, pow(-1,odd int y)=-1,pow(-1,even int y)=+1
         
    vpshufd		xmm3,xmm0,238 ;xmm3 = double x,x
    
    ; r = x - 1.0
    vsubsd       xmm3, xmm3,L__real_one	; r
    
    ;check if x is within the threshold
    vmovq       rdx,xmm3
    btr         rdx,63  
    cmp         rdx,L__real_threshold
    jae         L__compute_logx		;proceed in normal path.. x not near one
    
    vmovapd      xmm8,L__real_ca31   
    
    
    vaddsd       xmm1, xmm3,L__real_two; 2+r
    vdivsd       xmm6, xmm3,xmm1		  ; r/(2+r) = u/2    
    	 
    		 
    vaddsd       xmm2, xmm6,xmm6		  ; u
    
    vmulsd       xmm1, xmm2,xmm2		  ; u^2
    vpshufd      xmm4,xmm1,68 ; u^2,u^2    

                 ; u^2 * ca4,        u^2 * ca2
    vmulsd       xmm2, xmm2,xmm1                ; u^3    
                 
	vfmadd132pd  xmm4, xmm8, L__real_ca42
    vmulsd       xmm1, xmm1,xmm1                ; u^4
    vmulsd       xmm1, xmm1,xmm2                ; u^7
    vunpcklpd    xmm2, xmm2,xmm1                ; u^7, u^3
    vmulpd       xmm2, xmm2,xmm4                ; u^7 * (u^2 * ca4 + ca3),  u^3 * (u^2 * ca2 + ca1)

    vpshufd      xmm1,xmm2,238            ; XX,u^7 * (u^2 * ca4 + ca3)    

    vaddsd       xmm1, xmm1,xmm2                 ; u^7 * (u^2 * ca4 + ca3) + u^3 * (u^2 * ca2 + ca1)  
	vfnmadd231sd   xmm2, xmm3,xmm6 
    vaddsd       xmm3, xmm3,xmm2                 ; r + u^7 * (u^2 * ca4 + ca3) + u^3 * (u^2 * ca2 + ca1) - r*u/2    
    jmp         L__return_from_near_one

ALIGN 16
L__x_is_zero:
    mov  r8d,eax     
    vmovd edx,xmm1
    mov  eax,00h 
    mov  edi,00h 
    mov  ecx,07f800000h
    ;check if y < 0 or y>0

    cmp   edx,00h
    cmovl edx,ecx
    cmovl edi,ecx    
    cmovg edx,eax
    mov  ecx,080000000h
    ;check if y is int or not

    and r8d,07f800000h 
    cmp r8d,04b000000h     
    cmovg ecx,eax

    jg L__or_sign_and_retvalue_and_ret1    
    vcvtss2si r8d,xmm1
    vcvtsi2ss xmm2, xmm2,r8d 
    vucomiss  xmm2,xmm1

    cmovne ecx,eax
    jne L__or_sign_and_retvalue_and_ret1    
    rcr r8d,1			;rotate through carry to check last bit    
    cmovnc ecx,eax
    jnc L__or_sign_and_retvalue_and_ret1
    mov ecx,r9d
    and  ecx,080000000h
L__or_sign_and_retvalue_and_ret1:    
    or ecx,edx    
    vmovd xmm0,ecx
    cmp edi,00h
    je L__return
    ;;else raise div by zero exception
    mov r9d,2
    vmovdqa xmm2,xmm0
    call fname_special    
L__return:    
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret
  
ALIGN 16
L__x_is_inf_or_nan:    
    cmp eax,0h			;pow(+/-inf,+/-0)=1,pow(+/-nan,+/-0)=1
    je  L__return_pos_one
    vmovd ecx,xmm0
    vmovd edx,xmm1    
    cmp ecx,07f800000h
    je L__x_is_pos_inf
    cmp ecx,0ff800000h
    je L__x_is_neg_inf
    ;;else x is nan return nan
    jmp L__return_nan
    
ALIGN 16
L__x_is_pos_inf:
    ;check if y < 0 or y>0
    cmp edx ,0h
    jl L__return_pos_zero ;;pow(+inf,y<0)= +0
    ;;else
    jmp L__return_pos_inf ;;pow(+inf,y>0)= +inf
    
ALIGN 16
L__x_is_neg_inf:
    mov r8d,eax    
    mov eax,0h
    mov ecx,07f800000h
    ;check if y < 0 or y>0
    cmp edx, 0h
    cmovl edx,eax
    cmovg edx,ecx
    mov ecx,080000000h

    ;check if y is int or not	
    and r8d,07f800000h
    cmp r8d,04b000000h   
    cmovg ecx,eax
    jg L__or_sign_and_retvalue_and_ret    
    vcvtss2si r8d,xmm1 
    vcvtsi2ss xmm2, xmm2,r8d 
    vucomiss xmm2,xmm1
    cmovne   ecx,eax  
    jne L__or_sign_and_retvalue_and_ret
    rcr r8d,1		;rotate through carry to check last bit    
    cmovnc ecx,eax
L__or_sign_and_retvalue_and_ret:    
    or ecx,edx
    vmovd xmm0,ecx
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret
  
ALIGN 16
L__y_is_inf_or_nan:
    ja  L__y_is_nan     

;;else y is +/-inf
    ;check if x is nan
    cmp        ecx,07f800000h
    ;;x is nan return nan
    ja L__return_nan    
    cmp ecx,03F800000h    ;;pow(+/-1,any y, even nan or inf) = +1
    je  L__return_x_is_one_y_is_pninf  
    vmovd eax,xmm1
    bt eax,31			;;check the sign of y
    jc  L__y_is_neg_inf
    
;;else y_is_pos_inf
    cmp ecx,03f800000h
    jb  L__return_pos_zero ;;pow(|x| < 1, +inf) = +0
    ;;else return +inf
    jmp L__return_pos_inf  ;;pow(|x| > 1, +inf) = +inf

ALIGN 16
L__y_is_neg_inf:
    mov edx,1h
    mov eax,0h           ;;pow(|x| > 1, -inf) = +0
    mov r8d,07f800000h    ;pow(|x| < 1, -inf) = +inf
    cmp ecx,0h
    cmovne edx,eax        ;edx indicates if exception needs to be raised
    cmp ecx,03f800000h
    cmovb eax  ,r8d        ;eax contains result
    vmovd  xmm0 ,eax
    cmp   edx  ,00h

    je  L__return1
    ;;else raise exception
    vmovdqa xmm2,xmm0
    mov r9d,2
    call fname_special
L__return1:  
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]  
	add         rsp, stack_size
    ret

ALIGN 16
L__y_is_nan:
    cmp r9d,3F800000h
    je  L__return_pos_one  ;;pow(+1,any y, even nan or inf) = +1
    ;;else return nan, check exception
    vmovdqa xmm0,xmm1     ;;pow(any x other than 1, nan) = qnan
    jmp L__return_nan

ALIGN 16
L__check_if_y_is_zero_or_pos_one:
	cmp  eax,0h
    ;je  L__return_pos_one ;pow(any x, even nan or inf,+/-0) = +1
	je   L__check_x_is_nan
    vmovd edx,xmm1
    cmp  edx,3f800000h
    jne  L__y_is_not_zero_or_pos_one
    ;;else    
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret  ;;y=+1 so return x ##pow(x,1) = x

ALIGN 16
L__check_x_is_nan:
	cmp ecx,07f800000h
	ja  L__return_nan

        
ALIGN 16
L__return_pos_one:
    mov   edx,3f800000h
    vmovd  xmm0,edx    
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret
	
ALIGN 16
L__return_x_is_one_y_is_pninf:
	
	vorpd     xmm1,xmm1, XMMWORD PTR L__clear_sign
	vandpd    xmm0,xmm0,xmm1     
	vmovdqa   xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa   xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret


ALIGN 16
L__return_pos_zero:
    vpxor       xmm0, xmm0,xmm0
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret
    
   
ALIGN 16
L__return_pos_inf:
    mov         edx,07f800000h
    vmovd       xmm0,edx
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret   

ALIGN 16
L__return_nan:
    vaddss      xmm0, xmm0,xmm0
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret

ALIGN 16
L__y_is_zero:
    vpxor        xmm2, xmm2,xmm2
    vorps        xmm2, xmm2,xmm5
    mov         r9d ,7   
    call fname_special       
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]     
	add         rsp, stack_size
    ret  

ALIGN 16
L__y_is_inf:
    mov         edx,7f800000h
    vmovd        xmm2,edx 
    vorps        xmm2,xmm2,xmm5
    mov         r9d,9   
    call fname_special        
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret  

ALIGN 16
L__return_qnan:
    mov edx,07fc00000h
    vmovd xmm2,edx
    mov r9d,3
    call fname_special    
	vmovdqa     xmm6 , XMMWORD PTR [save_xmm6+rsp]  
	vmovdqa     xmm7,  XMMWORD PTR [save_xmm7+rsp]
	add         rsp, stack_size
    ret

fname endp

text ENDS

data SEGMENT READ

ALIGN 16

L__xexp_bits_xmantissa_bits		DQ 0000fffffffffffffh
					DQ 07ff0000000000000h
					   
L__1_by_2              DQ 03fe0000000000000h
                       DQ 00000000000000000h ;zeros required here
L__1023                DQ 000000000000003ffh
L__1_by_3              DQ 03fd5555555555555h
L__log2                DQ 03fe62e42fefa39efh
L__max_exp_arg         DQ 040562E4300000000h
L__min_exp_arg         DQ 0C059D1DA00000000h
L__real_log2_by_64     DQ 03f862e42fefa39efh
L__real_64_by_log2     DQ 040571547652b82feh 
                        DQ 00000000000000000h ;zeros required here
L__1_by_6              DQ 03fc5555555555555h
L__real_threshold  DQ 03fb0000000000000h ;.0625
L__real_one        DQ 03ff0000000000000h ;1
L__real_two        DQ 04000000000000000h ;2
L__neg_zero        DQ 08000000080000000h ;2 
ALIGN 16
L__real_ca42       DQ 03f89999999bac6d4h ;1.25000000037717509602e-02
L__real_ca4        DQ 03f3c8034c85dfff0h ;4.34887777707614552256e-04
L__real_ca31       DQ 03fb55555555554e6h ;8.33333333333317923934e-02
L__real_ca3        DQ 03f62492307f1519fh ;2.23213998791944806202e-03
L__clear_sign	   DQ 07fffffff7fffffffh
				   DQ 07fffffff7fffffffh

ALIGN 16
L__two_to_jby64_table:
     DQ 03ff0000000000000h
     DQ 03ff02c9a3e778061h
     DQ 03ff059b0d3158574h
     DQ 03ff0874518759bc8h
     DQ 03ff0b5586cf9890fh
     DQ 03ff0e3ec32d3d1a2h
     DQ 03ff11301d0125b51h
     DQ 03ff1429aaea92de0h
     DQ 03ff172b83c7d517bh
     DQ 03ff1a35beb6fcb75h
     DQ 03ff1d4873168b9aah
     DQ 03ff2063b88628cd6h
     DQ 03ff2387a6e756238h
     DQ 03ff26b4565e27cddh
     DQ 03ff29e9df51fdee1h
     DQ 03ff2d285a6e4030bh
     DQ 03ff306fe0a31b715h
     DQ 03ff33c08b26416ffh
     DQ 03ff371a7373aa9cbh
     DQ 03ff3a7db34e59ff7h
     DQ 03ff3dea64c123422h
     DQ 03ff4160a21f72e2ah
     DQ 03ff44e086061892dh
     DQ 03ff486a2b5c13cd0h
     DQ 03ff4bfdad5362a27h
     DQ 03ff4f9b2769d2ca7h
     DQ 03ff5342b569d4f82h
     DQ 03ff56f4736b527dah
     DQ 03ff5ab07dd485429h
     DQ 03ff5e76f15ad2148h
     DQ 03ff6247eb03a5585h
     DQ 03ff6623882552225h
     DQ 03ff6a09e667f3bcdh
     DQ 03ff6dfb23c651a2fh
     DQ 03ff71f75e8ec5f74h
     DQ 03ff75feb564267c9h
     DQ 03ff7a11473eb0187h
     DQ 03ff7e2f336cf4e62h
     DQ 03ff82589994cce13h
     DQ 03ff868d99b4492edh
     DQ 03ff8ace5422aa0dbh
     DQ 03ff8f1ae99157736h
     DQ 03ff93737b0cdc5e5h
     DQ 03ff97d829fde4e50h
     DQ 03ff9c49182a3f090h
     DQ 03ffa0c667b5de565h
     DQ 03ffa5503b23e255dh
     DQ 03ffa9e6b5579fdbfh
     DQ 03ffae89f995ad3adh
     DQ 03ffb33a2b84f15fbh
     DQ 03ffb7f76f2fb5e47h
     DQ 03ffbcc1e904bc1d2h
     DQ 03ffc199bdd85529ch
     DQ 03ffc67f12e57d14bh
     DQ 03ffcb720dcef9069h
     DQ 03ffd072d4a07897ch
     DQ 03ffd5818dcfba487h
     DQ 03ffda9e603db3285h
     DQ 03ffdfc97337b9b5fh
     DQ 03ffe502ee78b3ff6h
     DQ 03ffea4afa2a490dah
     DQ 03ffefa1bee615a27h
     DQ 03fff50765b6e4540h
     DQ 03fffa7c1819e90d8h


                    
ALIGN 16
L__log_F_inv: 
        DQ 04000000000000000h                    
        DQ 03fffe01fe01fe020h
        DQ 03fffc07f01fc07f0h
        DQ 03fffa11caa01fa12h
        DQ 03fff81f81f81f820h
        DQ 03fff6310aca0dbb5h
        DQ 03fff44659e4a4271h
        DQ 03fff25f644230ab5h
        DQ 03fff07c1f07c1f08h
        DQ 03ffee9c7f8458e02h
        DQ 03ffecc07b301ecc0h
        DQ 03ffeae807aba01ebh
        DQ 03ffe9131abf0b767h
        DQ 03ffe741aa59750e4h
        DQ 03ffe573ac901e574h
        DQ 03ffe3a9179dc1a73h
        DQ 03ffe1e1e1e1e1e1eh
        DQ 03ffe01e01e01e01eh
        DQ 03ffde5d6e3f8868ah
        DQ 03ffdca01dca01dcah
        DQ 03ffdae6076b981dbh
        DQ 03ffd92f2231e7f8ah
        DQ 03ffd77b654b82c34h
        DQ 03ffd5cac807572b2h
        DQ 03ffd41d41d41d41dh
        DQ 03ffd272ca3fc5b1ah
        DQ 03ffd0cb58f6ec074h
        DQ 03ffcf26e5c44bfc6h
        DQ 03ffcd85689039b0bh
        DQ 03ffcbe6d9601cbe7h
        DQ 03ffca4b3055ee191h
        DQ 03ffc8b265afb8a42h
        DQ 03ffc71c71c71c71ch
        DQ 03ffc5894d10d4986h
        DQ 03ffc3f8f01c3f8f0h
        DQ 03ffc26b5392ea01ch
        DQ 03ffc0e070381c0e0h
        DQ 03ffbf583ee868d8bh
        DQ 03ffbdd2b899406f7h
        DQ 03ffbc4fd65883e7bh
        DQ 03ffbacf914c1bad0h
        DQ 03ffb951e2b18ff23h
        DQ 03ffb7d6c3dda338bh
        DQ 03ffb65e2e3beee05h
        DQ 03ffb4e81b4e81b4fh
        DQ 03ffb37484ad806ceh
        DQ 03ffb2036406c80d9h
        DQ 03ffb094b31d922a4h
        DQ 03ffaf286bca1af28h
        DQ 03ffadbe87f94905eh
        DQ 03ffac5701ac5701bh
        DQ 03ffaaf1d2f87ebfdh
        DQ 03ffa98ef606a63beh
        DQ 03ffa82e65130e159h
        DQ 03ffa6d01a6d01a6dh
        DQ 03ffa574107688a4ah
        DQ 03ffa41a41a41a41ah
        DQ 03ffa2c2a87c51ca0h
        DQ 03ffa16d3f97a4b02h
        DQ 03ffa01a01a01a01ah
        DQ 03ff9ec8e951033d9h
        DQ 03ff9d79f176b682dh
        DQ 03ff9c2d14ee4a102h
        DQ 03ff9ae24ea5510dah
        DQ 03ff999999999999ah
        DQ 03ff9852f0d8ec0ffh
        DQ 03ff970e4f80cb872h
        DQ 03ff95cbb0be377aeh
        DQ 03ff948b0fcd6e9e0h
        DQ 03ff934c67f9b2ce6h
        DQ 03ff920fb49d0e229h
        DQ 03ff90d4f120190d5h
        DQ 03ff8f9c18f9c18fah
        DQ 03ff8e6527af1373fh
        DQ 03ff8d3018d3018d3h
        DQ 03ff8bfce8062ff3ah
        DQ 03ff8acb90f6bf3aah
        DQ 03ff899c0f601899ch
        DQ 03ff886e5f0abb04ah
        DQ 03ff87427bcc092b9h
        DQ 03ff8618618618618h
        DQ 03ff84f00c2780614h
        DQ 03ff83c977ab2beddh
        DQ 03ff82a4a0182a4a0h
        DQ 03ff8181818181818h
        DQ 03ff8060180601806h
        DQ 03ff7f405fd017f40h
        DQ 03ff7e225515a4f1dh
        DQ 03ff7d05f417d05f4h
        DQ 03ff7beb3922e017ch
        DQ 03ff7ad2208e0ecc3h
        DQ 03ff79baa6bb6398bh
        DQ 03ff78a4c8178a4c8h
        DQ 03ff77908119ac60dh
        DQ 03ff767dce434a9b1h
        DQ 03ff756cac201756dh
        DQ 03ff745d1745d1746h
        DQ 03ff734f0c541fe8dh
        DQ 03ff724287f46debch
        DQ 03ff713786d9c7c09h
        DQ 03ff702e05c0b8170h
        DQ 03ff6f26016f26017h
        DQ 03ff6e1f76b4337c7h
        DQ 03ff6d1a62681c861h
        DQ 03ff6c16c16c16c17h
        DQ 03ff6b1490aa31a3dh
        DQ 03ff6a13cd1537290h
        DQ 03ff691473a88d0c0h
        DQ 03ff6816816816817h
        DQ 03ff6719f3601671ah
        DQ 03ff661ec6a5122f9h
        DQ 03ff6524f853b4aa3h
        DQ 03ff642c8590b2164h
        DQ 03ff63356b88ac0deh
        DQ 03ff623fa77016240h
        DQ 03ff614b36831ae94h
        DQ 03ff6058160581606h
        DQ 03ff5f66434292dfch
        DQ 03ff5e75bb8d015e7h
        DQ 03ff5d867c3ece2a5h
        DQ 03ff5c9882b931057h
        DQ 03ff5babcc647fa91h
        DQ 03ff5ac056b015ac0h
        DQ 03ff59d61f123ccaah
        DQ 03ff58ed2308158edh
        DQ 03ff5805601580560h
        DQ 03ff571ed3c506b3ah
        DQ 03ff56397ba7c52e2h
        DQ 03ff5555555555555h
        DQ 03ff54725e6bb82feh
        DQ 03ff5390948f40febh
        DQ 03ff52aff56a8054bh
        DQ 03ff51d07eae2f815h
        DQ 03ff50f22e111c4c5h
        DQ 03ff5015015015015h
        DQ 03ff4f38f62dd4c9bh
        DQ 03ff4e5e0a72f0539h
        DQ 03ff4d843bedc2c4ch
        DQ 03ff4cab88725af6eh
        DQ 03ff4bd3edda68fe1h
        DQ 03ff4afd6a052bf5bh
        DQ 03ff4a27fad76014ah
        DQ 03ff49539e3b2d067h
        DQ 03ff4880522014880h
        DQ 03ff47ae147ae147bh
        DQ 03ff46dce34596066h
        DQ 03ff460cbc7f5cf9ah
        DQ 03ff453d9e2c776cah
        DQ 03ff446f86562d9fbh
        DQ 03ff43a2730abee4dh
        DQ 03ff42d6625d51f87h
        DQ 03ff420b5265e5951h
        DQ 03ff4141414141414h
        DQ 03ff40782d10e6566h
        DQ 03ff3fb013fb013fbh
        DQ 03ff3ee8f42a5af07h
        DQ 03ff3e22cbce4a902h
        DQ 03ff3d5d991aa75c6h
        DQ 03ff3c995a47babe7h
        DQ 03ff3bd60d9232955h
        DQ 03ff3b13b13b13b14h
        DQ 03ff3a524387ac822h
        DQ 03ff3991c2c187f63h
        DQ 03ff38d22d366088eh
        DQ 03ff3813813813814h
        DQ 03ff3755bd1c945eeh
        DQ 03ff3698df3de0748h
        DQ 03ff35dce5f9f2af8h
        DQ 03ff3521cfb2b78c1h
        DQ 03ff34679ace01346h
        DQ 03ff33ae45b57bcb2h
        DQ 03ff32f5ced6a1dfah
        DQ 03ff323e34a2b10bfh
        DQ 03ff3187758e9ebb6h
        DQ 03ff30d190130d190h
        DQ 03ff301c82ac40260h
        DQ 03ff2f684bda12f68h
        DQ 03ff2eb4ea1fed14bh
        DQ 03ff2e025c04b8097h
        DQ 03ff2d50a012d50a0h
        DQ 03ff2c9fb4d812ca0h
        DQ 03ff2bef98e5a3711h
        DQ 03ff2b404ad012b40h
        DQ 03ff2a91c92f3c105h
        DQ 03ff29e4129e4129eh
        DQ 03ff293725bb804a5h
        DQ 03ff288b01288b013h
        DQ 03ff27dfa38a1ce4dh
        DQ 03ff27350b8812735h
        DQ 03ff268b37cd60127h
        DQ 03ff25e22708092f1h
        DQ 03ff2539d7e9177b2h
        DQ 03ff2492492492492h
        DQ 03ff23eb79717605bh
        DQ 03ff23456789abcdfh
        DQ 03ff22a0122a0122ah
        DQ 03ff21fb78121fb78h
        DQ 03ff21579804855e6h
        DQ 03ff20b470c67c0d9h
        DQ 03ff2012012012012h
        DQ 03ff1f7047dc11f70h
        DQ 03ff1ecf43c7fb84ch
        DQ 03ff1e2ef3b3fb874h
        DQ 03ff1d8f5672e4abdh
        DQ 03ff1cf06ada2811dh
        DQ 03ff1c522fc1ce059h
        DQ 03ff1bb4a4046ed29h
        DQ 03ff1b17c67f2bae3h
        DQ 03ff1a7b9611a7b96h
        DQ 03ff19e0119e0119eh
        DQ 03ff19453808ca29ch
        DQ 03ff18ab083902bdbh
        DQ 03ff1811811811812h
        DQ 03ff1778a191bd684h
        DQ 03ff16e0689427379h
        DQ 03ff1648d50fc3201h
        DQ 03ff15b1e5f75270dh
        DQ 03ff151b9a3fdd5c9h
        DQ 03ff1485f0e0acd3bh
        DQ 03ff13f0e8d344724h
        DQ 03ff135c81135c811h
        DQ 03ff12c8b89edc0ach
        DQ 03ff12358e75d3033h
        DQ 03ff11a3019a74826h
        DQ 03ff1111111111111h
        DQ 03ff107fbbe011080h
        DQ 03ff0fef010fef011h
        DQ 03ff0f5edfab325a2h
        DQ 03ff0ecf56be69c90h
        DQ 03ff0e40655826011h
        DQ 03ff0db20a88f4696h
        DQ 03ff0d24456359e3ah
        DQ 03ff0c9714fbcda3bh
        DQ 03ff0c0a7868b4171h
        DQ 03ff0b7e6ec259dc8h
        DQ 03ff0af2f722eecb5h
        DQ 03ff0a6810a6810a7h
        DQ 03ff09ddba6af8360h
        DQ 03ff0953f39010954h
        DQ 03ff08cabb37565e2h
        DQ 03ff0842108421084h
        DQ 03ff07b9f29b8eae2h
        DQ 03ff073260a47f7c6h
        DQ 03ff06ab59c7912fbh
        DQ 03ff0624dd2f1a9fch
        DQ 03ff059eea0727586h
        DQ 03ff05197f7d73404h
        DQ 03ff04949cc1664c5h
        DQ 03ff0410410410410h
        DQ 03ff038c6b78247fch
        DQ 03ff03091b51f5e1ah
        DQ 03ff02864fc7729e9h
        DQ 03ff0204081020408h
        DQ 03ff0182436517a37h
        DQ 03ff0101010101010h
        DQ 03ff0080402010080h
        DQ 03ff0000000000000h


L__log_256_table: 
        DQ 00000000000000000h
        DQ 03f6ff00aa2b10bc0h
        DQ 03f7fe02a6b106789h
        DQ 03f87dc475f810a77h
        DQ 03f8fc0a8b0fc03e4h
        DQ 03f93cea44346a575h
        DQ 03f97b91b07d5b11bh
        DQ 03f9b9fc027af9198h
        DQ 03f9f829b0e783300h
        DQ 03fa1b0d98923d980h
        DQ 03fa39e87b9febd60h
        DQ 03fa58a5bafc8e4d5h
        DQ 03fa77458f632dcfch
        DQ 03fa95c830ec8e3ebh
        DQ 03fab42dd711971bfh
        DQ 03fad276b8adb0b52h
        DQ 03faf0a30c01162a6h
        DQ 03fb075983598e471h
        DQ 03fb16536eea37ae1h
        DQ 03fb253f62f0a1417h
        DQ 03fb341d7961bd1d1h
        DQ 03fb42edcbea646f0h
        DQ 03fb51b073f06183fh
        DQ 03fb60658a93750c4h
        DQ 03fb6f0d28ae56b4ch
        DQ 03fb7da766d7b12cdh
        DQ 03fb8c345d6319b21h
        DQ 03fb9ab42462033adh
        DQ 03fba926d3a4ad563h
        DQ 03fbb78c82bb0eda1h
        DQ 03fbc5e548f5bc743h
        DQ 03fbd4313d66cb35dh
        DQ 03fbe27076e2af2e6h
        DQ 03fbf0a30c01162a6h
        DQ 03fbfec9131dbeabbh
        DQ 03fc0671512ca596eh
        DQ 03fc0d77e7cd08e59h
        DQ 03fc14785846742ach
        DQ 03fc1b72ad52f67a0h
        DQ 03fc2266f190a5acbh
        DQ 03fc29552f81ff523h
        DQ 03fc303d718e47fd3h
        DQ 03fc371fc201e8f74h
        DQ 03fc3dfc2b0ecc62ah
        DQ 03fc44d2b6ccb7d1eh
        DQ 03fc4ba36f39a55e5h
        DQ 03fc526e5e3a1b438h
        DQ 03fc59338d9982086h
        DQ 03fc5ff3070a793d4h
        DQ 03fc66acd4272ad51h
        DQ 03fc6d60fe719d21dh
        DQ 03fc740f8f54037a5h
        DQ 03fc7ab890210d909h
        DQ 03fc815c0a14357ebh
        DQ 03fc87fa06520c911h
        DQ 03fc8e928de886d41h
        DQ 03fc9525a9cf456b4h
        DQ 03fc9bb362e7dfb83h
        DQ 03fca23bc1fe2b563h
        DQ 03fca8becfc882f19h
        DQ 03fcaf3c94e80bff3h
        DQ 03fcb5b519e8fb5a4h
        DQ 03fcbc286742d8cd6h
        DQ 03fcc2968558c18c1h
        DQ 03fcc8ff7c79a9a22h
        DQ 03fccf6354e09c5dch
        DQ 03fcd5c216b4fbb91h
        DQ 03fcdc1bca0abec7dh
        DQ 03fce27076e2af2e6h
        DQ 03fce8c0252aa5a60h
        DQ 03fcef0adcbdc5936h
        DQ 03fcf550a564b7b37h
        DQ 03fcfb9186d5e3e2bh
        DQ 03fd00e6c45ad501dh
        DQ 03fd0402594b4d041h
        DQ 03fd071b85fcd590dh
        DQ 03fd0a324e27390e3h
        DQ 03fd0d46b579ab74bh
        DQ 03fd1058bf9ae4ad5h
        DQ 03fd136870293a8b0h
        DQ 03fd1675cababa60eh
        DQ 03fd1980d2dd4236fh
        DQ 03fd1c898c16999fbh
        DQ 03fd1f8ff9e48a2f3h
        DQ 03fd22941fbcf7966h
        DQ 03fd2596010df763ah
        DQ 03fd2895a13de86a3h
        DQ 03fd2b9303ab89d25h
        DQ 03fd2e8e2bae11d31h
        DQ 03fd31871c9544185h
        DQ 03fd347dd9a987d55h
        DQ 03fd3772662bfd85bh
        DQ 03fd3a64c556945eah
        DQ 03fd3d54fa5c1f710h
        DQ 03fd404308686a7e4h
        DQ 03fd432ef2a04e814h
        DQ 03fd4618bc21c5ec2h
        DQ 03fd49006804009d1h
        DQ 03fd4be5f957778a1h
        DQ 03fd4ec973260026ah
        DQ 03fd51aad872df82dh
        DQ 03fd548a2c3add263h
        DQ 03fd5767717455a6ch
        DQ 03fd5a42ab0f4cfe2h
        DQ 03fd5d1bdbf5809cah
        DQ 03fd5ff3070a793d4h
        DQ 03fd62c82f2b9c795h
        DQ 03fd659b57303e1f3h
        DQ 03fd686c81e9b14afh
        DQ 03fd6b3bb2235943eh
        DQ 03fd6e08eaa2ba1e4h
        DQ 03fd70d42e2789236h
        DQ 03fd739d7f6bbd007h
        DQ 03fd7664e1239dbcfh
        DQ 03fd792a55fdd47a2h
        DQ 03fd7bede0a37afc0h
        DQ 03fd7eaf83b82afc3h
        DQ 03fd816f41da0d496h
        DQ 03fd842d1da1e8b17h
        DQ 03fd86e919a330ba0h
        DQ 03fd89a3386c1425bh
        DQ 03fd8c5b7c858b48bh
        DQ 03fd8f11e873662c7h
        DQ 03fd91c67eb45a83eh
        DQ 03fd947941c2116fbh
        DQ 03fd972a341135158h
        DQ 03fd99d958117e08bh
        DQ 03fd9c86b02dc0863h
        DQ 03fd9f323ecbf984ch
        DQ 03fda1dc064d5b995h
        DQ 03fda484090e5bb0ah
        DQ 03fda72a4966bd9eah
        DQ 03fda9cec9a9a084ah
        DQ 03fdac718c258b0e4h
        DQ 03fdaf1293247786bh
        DQ 03fdb1b1e0ebdfc5bh
        DQ 03fdb44f77bcc8f63h
        DQ 03fdb6eb59d3cf35eh
        DQ 03fdb9858969310fbh
        DQ 03fdbc1e08b0dad0ah
        DQ 03fdbeb4d9da71b7ch
        DQ 03fdc149ff115f027h
        DQ 03fdc3dd7a7cdad4dh
        DQ 03fdc66f4e3ff6ff8h
        DQ 03fdc8ff7c79a9a22h
        DQ 03fdcb8e0744d7acah
        DQ 03fdce1af0b85f3ebh
        DQ 03fdd0a63ae721e64h
        DQ 03fdd32fe7e00ebd5h
        DQ 03fdd5b7f9ae2c684h
        DQ 03fdd83e7258a2f3eh
        DQ 03fddac353e2c5954h
        DQ 03fddd46a04c1c4a1h
        DQ 03fddfc859906d5b5h
        DQ 03fde24881a7c6c26h
        DQ 03fde4c71a8687704h
        DQ 03fde744261d68788h
        DQ 03fde9bfa659861f5h
        DQ 03fdec399d2468cc0h
        DQ 03fdeeb20c640ddf4h
        DQ 03fdf128f5faf06edh
        DQ 03fdf39e5bc811e5ch
        DQ 03fdf6123fa7028ach
        DQ 03fdf884a36fe9ec2h
        DQ 03fdfaf588f78f31fh
        DQ 03fdfd64f20f61572h
        DQ 03fdffd2e0857f498h
        DQ 03fe011fab125ff8ah
        DQ 03fe02552a5a5d0ffh
        DQ 03fe0389eefce633bh
        DQ 03fe04bdf9da926d2h
        DQ 03fe05f14bd26459ch
        DQ 03fe0723e5c1cdf40h
        DQ 03fe0855c884b450eh
        DQ 03fe0986f4f573521h
        DQ 03fe0ab76bece14d2h
        DQ 03fe0be72e4252a83h
        DQ 03fe0d163ccb9d6b8h
        DQ 03fe0e44985d1cc8ch
        DQ 03fe0f7241c9b497dh
        DQ 03fe109f39e2d4c97h
        DQ 03fe11cb81787ccf8h
        DQ 03fe12f719593efbch
        DQ 03fe1422025243d45h
        DQ 03fe154c3d2f4d5eah
        DQ 03fe1675cababa60eh
        DQ 03fe179eabbd899a1h
        DQ 03fe18c6e0ff5cf06h
        DQ 03fe19ee6b467c96fh
        DQ 03fe1b154b57da29fh
        DQ 03fe1c3b81f713c25h
        DQ 03fe1d610fe677003h
        DQ 03fe1e85f5e7040d0h
        DQ 03fe1faa34b87094ch
        DQ 03fe20cdcd192ab6eh
        DQ 03fe21f0bfc65beech
        DQ 03fe23130d7bebf43h
        DQ 03fe2434b6f483934h
        DQ 03fe2555bce98f7cbh
        DQ 03fe26762013430e0h
        DQ 03fe2795e1289b11bh
        DQ 03fe28b500df60783h
        DQ 03fe29d37fec2b08bh
        DQ 03fe2af15f02640adh
        DQ 03fe2c0e9ed448e8ch
        DQ 03fe2d2b4012edc9eh
        DQ 03fe2e47436e40268h
        DQ 03fe2f62a99509546h
        DQ 03fe307d7334f10beh
        DQ 03fe3197a0fa7fe6ah
        DQ 03fe32b1339121d71h
        DQ 03fe33ca2ba328995h
        DQ 03fe34e289d9ce1d3h
        DQ 03fe35fa4edd36ea0h
        DQ 03fe37117b54747b6h
        DQ 03fe38280fe58797fh
        DQ 03fe393e0d3562a1ah
        DQ 03fe3a5373e7ebdfah
        DQ 03fe3b68449fffc23h
        DQ 03fe3c7c7fff73206h
        DQ 03fe3d9026a7156fbh
        DQ 03fe3ea33936b2f5ch
        DQ 03fe3fb5b84d16f42h
        DQ 03fe40c7a4880dce9h
        DQ 03fe41d8fe84672aeh
        DQ 03fe42e9c6ddf80bfh
        DQ 03fe43f9fe2f9ce67h
        DQ 03fe4509a5133bb0ah
        DQ 03fe4618bc21c5ec2h
        DQ 03fe472743f33aaadh
        DQ 03fe48353d1ea88dfh
        DQ 03fe4942a83a2fc07h
        DQ 03fe4a4f85db03ebbh
        DQ 03fe4b5bd6956e274h
        DQ 03fe4c679afccee3ah
        DQ 03fe4d72d3a39fd00h
        DQ 03fe4e7d811b75bb1h
        DQ 03fe4f87a3f5026e9h
        DQ 03fe50913cc01686bh
        DQ 03fe519a4c0ba3446h
        DQ 03fe52a2d265bc5abh
        DQ 03fe53aad05b99b7dh
        DQ 03fe54b2467999498h
        DQ 03fe55b9354b40bcdh
        DQ 03fe56bf9d5b3f399h
        DQ 03fe57c57f336f191h
        DQ 03fe58cadb5cd7989h
        DQ 03fe59cfb25fae87eh
        DQ 03fe5ad404c359f2dh
        DQ 03fe5bd7d30e71c73h
        DQ 03fe5cdb1dc6c1765h
        DQ 03fe5ddde57149923h
        DQ 03fe5ee02a9241675h
        DQ 03fe5fe1edad18919h
        DQ 03fe60e32f44788d9h
        DQ 03fe61e3efda46467h
        DQ 03fe62e42fefa39efh
END                    
