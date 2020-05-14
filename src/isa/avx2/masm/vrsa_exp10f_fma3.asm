;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

include exp_tables.inc
include fm.inc

FN_PROTOTYPE_FMA3 vrsa_exp10f

stack_size      EQU     88h

save_rdi	equ		20h
save_rsi	equ		30h
save_xmm10  equ     40h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi 
    SaveXmm xmm10,save_xmm10	   
    .ENDPROLOG

 mov rdi,rcx
 mov rsi, rdx
 mov rdx, r8
 
 cmp edi,00h 
 jle L__return
 cmp rsi,00h
 je L__return
 cmp rdx,00h
 je L__return
 
ALIGN 16 
L__process_next4:
 sub rdi,04h
 cmp rdi,-1
 jle L__process_first123
 movdqu xmm0,[rsi + rdi * 4]
 jmp L__start
 
ALIGN 16 
L__process_first123:
 je L__process_first3
 cmp rdi,-3
 jl L__return ;multiple of all,4elements are processed rdi == -4
 je L__process_first1 
 ;process first 2 
 mov rcx,00h
 movsd xmm0,QWORD PTR[rsi + rcx * 4]
 jmp L__start

ALIGN 16
L__process_first1:
 mov rcx,00h
 movss xmm0,DWORD PTR[rsi + rcx * 4]
 jmp L__start 

ALIGN 16 
L__process_first3:
 mov rcx,01h
 movsd xmm0,QWORD PTR[rsi + rcx * 4] 
 dec rcx ; zero
 movss xmm1,DWORD PTR[rsi + rcx * 4]
 pslldq xmm0,4
 por xmm0,xmm1 
 
ALIGN 16 
L__start: 
 vmovdqa xmm10,xmm0 ; save for later use
 vminps xmm0,xmm0,XMMWORD PTR L__max_exp10f_arg
 vcvtps2pd ymm2,xmm0 ;ymm2 = (double x0 ,x1,x2,)x3

 ; x * (64/log10of(2))
 vmulpd ymm3,ymm2,L__real_64_by_log10of2 ;ymm3 = x3 * (64/ln x2,(2) * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)

 ; n = int( x * (64/log10of(2)) )
 vcvtpd2dq xmm4,ymm3 ;xmm4 = (int n0 ,n1,n2,)n3 
 vcvtdq2pd ymm0,xmm4 ;ymm0 = (double n0 ,n1,n2,)n3 

 ; r = x - n * ln(2)/64
 ; r *= ln(10)
 vfmadd132pd ymm0,ymm2,L__real_mlog10of2_by_64 ;ymm0 = r r1,r2,3,r0
 vmulpd ymm1,ymm0,L__real_ln10 ;ymm1 = r = r*ln(2) 

 ; q = r + r*r(1/2 + r*1/6)
 vmovdqa ymm3,YMMWORD PTR L__real_4_1_by_6s
 ;vfmaddpd ymm2 ,ymm1,ymm3,L__real_4_1_by_2s ;ymm2 = (1/2 + (1/6 * r))
 ;vmulpd ymm3,ymm1,ymm1 ;ymm3 = r3*r r2,3*r2,r1*r1,r0*r0
 ;vfmaddpd ymm0 ,ymm2,ymm3,ymm1 ;ymm0 = q q1,q2,3,q0 
 
 ;j = n & 03fh
 ;vpsrad xmm1,xmm4,6 ;xmm1 = m m0 ,m1,m2,3
 ;vpslld xmm3,xmm4,26 
 ;vpsrld xmm4,xmm3,26 ;xmm4 = j j0 ,j1,j2,3 
 
 ; f + (f*q)
 ;lea r10,L__two_to_jby64_table
 ;vmovd eax,xmm4 ;eax = j0
 ;vpsrldq xmm3,xmm4,4 ;xmm3 = X j1 ,j2,j3,X
 ;vmovd ecx,xmm3 ;ecx = j1
 ;vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 ;vmovhpd xmm2,xmm2,QWORD PTR[r10 + rcx * 8] ;xmm2 = f f0,1 
 ;vpsrldq xmm4,xmm3,4 ;xmm4 = X j2 ,j3,XX,X
 ;vmovd eax,xmm4 ;eax = j2
 ;vpsrldq xmm3,xmm4,4 ;xmm3 = X j3 ,XX,XX,X
 ;vmovd ecx,xmm3 ;ecx = j3
 ;vmovsd xmm3,QWORD PTR[r10 + rax * 8]
 ;vmovhpd xmm3,xmm3,QWORD PTR[r10 + rcx * 8] ;xmm3 = f f2,3 
 ;vinsertf128 ymm2 ,ymm2,xmm3,1 ;ymm2 = f f1,f2,3,f0
 ;vfmaddpd ymm3 ,ymm2,ymm0,ymm2 ;ymm3 = f + (f*q)
 vmovdqa     ymm2,ymm3
 vfmadd213pd  ymm3 ,ymm1,L__real_4_1_by_2s             
 
 vmulpd       ymm2,ymm1,ymm1   
 
 vfmadd213pd   ymm3,ymm2,ymm1          
 
 vpsrad      xmm1,xmm4,6    
 vpslld      xmm0,xmm4,26   
 vpsrld      xmm4,xmm0,26          
 
 
 lea r10,L__two_to_jby64_table
 vmovd   eax,xmm4      
 vpsrldq  xmm0,xmm4,4  
 vmovd    ecx,xmm0     
 vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 vmovhpd xmm2, xmm2,QWORD PTR[r10 + rcx * 8]    
 vpsrldq  xmm4,xmm0,4 
 vmovd    eax,xmm4     
 vpsrldq  xmm0, xmm4,4 
 vmovd    ecx,xmm0      
 vmovsd  xmm0,QWORD PTR[r10 + rax * 8] 
 vmovhpd xmm0,xmm0,QWORD PTR[r10 + rcx * 8]    
 vinsertf128 ymm2 ,ymm2,xmm0,1
 vfmadd213pd ymm3, ymm2,  ymm2   
 
 
 
 
 
 

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
 vcmpps xmm2 ,xmm1,xmm10,1
 vpand xmm1,xmm0,xmm2 ;put zeros in place of any x < min_exp2_arg 
 
 ;;special case for any x = nan
 ;;remove this code if the above code takes care of this
 vcmpps xmm2 ,xmm10,xmm10,00h
 vaddps xmm3,xmm10,xmm10 ;make qnan to put in place of any x =nan 
 ;vpcmov xmm0 ,xmm1,xmm3,xmm2 
 VANDPD    xmm0 ,  xmm1, xmm2
 VANDNPD   xmm4 ,  xmm2, xmm3
 VORPD     xmm0 ,  xmm0, xmm4

 cmp rdi,-1
 jle L__store123
 movdqu XMMWORD PTR[rdx + rdi * 4],xmm0
 jmp L__process_next4 
 
ALIGN 16 
L__store123:
 je L__store3
 cmp rdi,-3 
 je L__store1 
 ;store 2 
 add rdi,02h
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return

ALIGN 16 
L__store3:
 movdqa xmm1,xmm0
 psrldq xmm0,4
 inc rdi
 movss DWORD PTR[rdx + rdi * 4],xmm1
 inc rdi
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return
 
ALIGN 16 
L__store1:
 mov rdi,00h
 movss DWORD PTR[rdx + rdi * 4],xmm0 
 
L__return:
 RestoreXmm xmm10, save_xmm10
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 
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

