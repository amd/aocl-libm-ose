include exp_tables.inc
include fm.inc

FN_PROTOTYPE_FMA3 vrda_exp10

stack_size      EQU     88h

save_rdi	equ		20h
save_rsi	equ		30h
save_xmm10  equ     40h
save_xmm9   equ     60h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi  
	SaveXmm xmm10,save_xmm10  
	SaveXmm xmm9,save_xmm9 
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
L__process_next2:
 sub rdi,02h
 cmp rdi, -1
 jle L__process_first1
 movdqu xmm0,[rsi + rdi * 8]
 jmp L__start
 
ALIGN 16 
L__process_first1:
 jl L__return ;even all,elements are processed rdi == -2
 mov rcx,00h
 movsd xmm0,QWORD PTR[rsi + rcx * 8] 
 
ALIGN 16 
L__start: 
 vmovdqa xmm10,xmm0 ; save for later use
 
 ; x * (64/log10(2))
 vmulpd xmm3,xmm0,L__real_64_by_log10of2 

 ; n = int( x * (64/log10(2)) )
 vcvttpd2dq xmm4,xmm3 ;xmm4 = 0,0,(int)n1,(int)n0
 vcvtdq2pd xmm3,xmm4 ;xmm3 = (double)n1,n0 
 vmovq rcx,xmm4 ;rcx = (int)n1,n0
 
 ; r1 = x - n * log10(2)/64 head 
 ; r2 = - n * log10(2)/64 tail 
 ; r1 *= ln10;; r2 *= ln10;
 ; r = r1+r2 
 vinsertf128 ymm3 ,ymm3,xmm3,1 ;ymm3 = (double n1,n0,)n n0,1
 vpor xmm0,xmm0,xmm0 ;ymm0 = zero in upper part
 vfmadd132pd ymm3,ymm0,L__log10of2_by_64_mtail_mhead
 vmulpd ymm3,ymm3,L__ln10
 vextractf128 xmm1,ymm3,1
 vaddpd xmm2,xmm1,xmm3
 
 ;j = n & 03fh 
 mov rax,00000003f0000003fh
 and rax,rcx ;rax = j1,j0
 mov ecx,eax ;rcx = j0
 shr rax,32 ;rax = j1 
 ; m = (n - j) / 64 
 vpsrad xmm5,xmm4,6 ;xmm5 = 0,0,m m0,1

 ; q = r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720
 ; q = r + r*r*(1/2 + r*(1/6+ r*(1/24 + r*(1/120 + r*(1/720)))))
 vmovapd    xmm3,L__real_1_by_720
  vfmadd213pd     xmm3, xmm2,L__real_1_by_120 
  vfmadd213pd     xmm3, xmm2,L__real_1_by_24  
  vfmadd213pd     xmm3, xmm2,L__real_1_by_6   
  vfmadd213pd     xmm3, xmm2,L__real_1_by_2   
 vmulpd     xmm0,xmm2,xmm2
  vfmadd213pd     xmm0, xmm3, xmm2                  
  
 ; (f)*(q) + f2 + f1
 lea r9,L__two_to_jby64_table 
 lea r11,L__two_to_jby64_tail_table 
 lea r10,L__two_to_jby64_head_table 
 vmovsd xmm2,QWORD PTR[r9 + rcx * 8]
 vmovhpd xmm1,xmm2,QWORD PTR[r9 + rax * 8] ;xmm1 = f f_0,_1 
 vmovsd xmm2,QWORD PTR[r11 + rcx * 8] 
 vmovhpd xmm3,xmm2,QWORD PTR[r11 + rax * 8] ;xmm3 = f2 f2_0,_1
 vmovsd xmm4,QWORD PTR[r10 + rcx * 8]
 vmovhpd xmm2,xmm4,QWORD PTR[r10 + rax * 8] ;xmm2 = f1 f1_0,_1 
 vfmadd213pd  xmm0,xmm1,xmm3 ;xmm0 = f*q + f2
 vaddpd xmm0,xmm2,xmm0

 ; normal results
 vpmovsxdq xmm4,xmm5 ;xmm4 = m1,m0
 vpsllq xmm5,xmm4,52 ;xmm5 = 2^m 2,1^m0
 vpaddq xmm1,xmm0,xmm5 ;xmm1 = normal results
 
 ;check and process denormals 
 vcmppd xmm3 ,xmm0,L__real_one,1 ;check e^r < 1
 vpcmpeqq xmm2,xmm5,XMMWORD PTR L__m1022 ;check m == -1022
 vpand xmm3,xmm3,xmm2 ;xmm3 = ( e^r < 1 and m = -1022 )
 vmovdqa xmm2, XMMWORD PTR L__m1022
 vpcmpgtq xmm2,xmm2,xmm5 ;check m < -1022 
 vpor xmm2,xmm2,xmm3 ;xmm2 = mask for inputs requiring denormal processing
 vpmovmskb   eax,xmm2
 cmp         eax,0
 je          L__check_min   ;jmp to avoid any accidental intermediate denormal results
 vpaddq xmm3,xmm4,L__ulong_1074 ;xmm3 = 1074 + m 
 vunpckhpd xmm9,xmm3,xmm3 ;store high to xmm9
 vmovdqa xmm5,XMMWORD PTR L__ulong_1
 vpsllq xmm4,xmm5,xmm3
 vpsllq xmm9,xmm5,xmm9
 vunpcklpd xmm4,xmm4,xmm9
 vmulpd xmm3,xmm0,xmm4 ;xmm3 = results for denormal numbers
 ;vpcmov xmm1 ,xmm3,xmm1,xmm2
 VANDNPD   xmm5,  xmm2, xmm1
 VANDPD    xmm1,  xmm3, xmm2
 VORPD     xmm1,  xmm1, xmm5
 
L__check_min:
 ;;special case for any x < min remove,_exp_arg this code if the above code takes care of this
 vcmppd xmm2 ,xmm10,L__min_exp10_arg,2 ;cmp x<= min_exp_arg
 vpxor xmm4,xmm3,xmm3 ;make zeros to put in place of x<= min_exp_arg
 ;vpcmov xmm1 ,xmm4,xmm1,xmm2 ;put zeros in place of any x <= min_exp_arg 
 VANDNPD   xmm5 ,  xmm2, xmm1
 VANDPD    xmm1 ,  xmm4, xmm2
 VORPD     xmm1 ,  xmm1, xmm5
 
 ;;special case for any x > max remove,_exp_arg this code if the above code takes care of this
 vcmppd xmm2 ,xmm10,L__max_exp10_arg,1 ;cmp x < max_exp_arg
 vmovdqa xmm3,XMMWORD PTR L__real_inf ;inf to put in place of any x >= max_exp_arg
 ;vpcmov xmm1 ,xmm1,xmm3,xmm2 ;xmm1 = normal results
 VANDPD   xmm1,  xmm1, xmm2 
 VANDNPD  xmm5,  xmm2, xmm3 
 VORPD    xmm1,  xmm1, xmm5 
 
 ;;special case for any x = na remove,n this code if the above code takes care of this
 vcmppd xmm2 ,xmm10,xmm10,00h 
 vaddpd xmm3,xmm10,xmm10 ;make qnan to put in place of any x =nan
 ;vpcmov xmm0 ,xmm1,xmm3,xmm2 ;xmm0 = normal results 
 VANDPD    xmm0,  xmm1, xmm2
 VANDNPD   xmm5,  xmm2, xmm3
 VORPD     xmm0,  xmm0, xmm5

 cmp rdi,-1
 je L__store1
 movdqu [rdx + rdi * 8],xmm0
 jmp L__process_next2 
 
ALIGN 16 
L__store1:
 inc rdi
 movsd QWORD PTR[rdx + rdi * 8],xmm0
 
L__return: 
 RestoreXmm xmm9, save_xmm9 
 RestoreXmm xmm10, save_xmm10 
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 
 ret 
 
fname endp
TEXT ENDS

data SEGMENT READ PAGE

ALIGN 16
L__max_exp10_arg DQ 040734413509f79ffh, 040734413509f79ffh
L__min_exp10_arg DQ 0c07434e6420f4374h, 0c07434e6420f4374h
L__real_64_by_log10of2 DQ 0406A934F0979A371h, 0406A934F0979A371h ; 64/log10(2)
L__ln10 DQ 040026BB1BBB55516h, 040026BB1BBB55516h, 040026BB1BBB55516h, 040026BB1BBB55516h
L__log10of2_by_64_mtail_mhead DQ 0bF73441350000000h, 0bF73441350000000h, 0bda3ef3fde623e25h, 0bda3ef3fde623e25h
L__real_one DQ 03ff0000000000000h, 03ff0000000000000h
L__real_inf DQ 07ff0000000000000h, 07ff0000000000000h
L__m1022 DQ 0c020000000000000h, 0c020000000000000h
L__ulong_1074 DQ 00000000000000432h, 00000000000000432h
L__ulong_1 DQ 00000000000000001h, 00000000000000001h

data ENDS
END

