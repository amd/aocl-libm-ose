;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

include fm.inc

include trig_func.inc

FN_PROTOTYPE_FMA3 vrsa_cosf




text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
	SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi
	SaveReg rbx,save_rbx
    SaveAllXMMRegs
        SaveRegsGPR r12,save_r12
        SaveRegsGPR r13,save_r13

    .ENDPROLOG

    ;movupd xmm0, XMMWORD PTR [rcx]
 mov rdi,rcx
 mov rsi, rdx
 mov rbx, r8
 
 cmp edi,00h 
 jle L__return
 cmp rsi,00h
 je L__return
 cmp rbx,00h
 je L__return
 
ALIGN 16 
L__process_next4:
 sub rdi,04h
 cmp rdi,-1
 jle L__process_first123
 movdqu xmm0,XMMWORD PTR[rsi + rdi * 4]
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
   

cos_early_exit_s:

    VCVTPS2PD    ymm0,xmm0

    VCMPNEQPD    ymm1,ymm0,ymm0                       ;nan mask ymm1
    VANDPD    ymm8,ymm0,L__sign_mask            ; ymm8 clear sign
    VEXTRACTF128    xmm3,ymm8,1
    VPCMPEQQ    xmm2,xmm8,L__inf_mask_64       ;
    VPCMPEQQ    xmm3,xmm3,L__inf_mask_64       ;
    VINSERTF128    ymm2 ,ymm2,xmm3,1                   ;ymm2 has inf mask
    VORPD    ymm11,ymm2,ymm1                        ; ymm11 nan and inf mask


    VADDPD    ymm3,ymm0,ymm0                        ;nan value
    ;VPCMOV    ymm4 ,ymm3,ymm1,ymm1                    ;ymm4 nan value
    VANDPD  ymm4  ,  ymm3,   ymm1 
    VANDNPD  ymm10  ,  ymm1,  ymm1
    VORPD  ymm4   ,  ymm4,  ymm10 
    
    VMOVAPD    ymm3,L_nan
    ;VPCMOV    ymm10 ,ymm3,ymm4,ymm2                ;ymm10 nan and inf values
    VANDPD    ymm10  ,  ymm3,  ymm2
    VANDNPD   ymm7  ,  ymm2, ymm4 
    VORPD     ymm10  ,  ymm10, ymm7


cos_early_exit_s_1:

    VEXTRACTF128    xmm7,ymm8,1
	VMOVDQA xmm13,XMMWORD PTR L_mask_3f_9
	VMOVDQA xmm5,XMMWORD PTR L_mask_3f_9
    VPCMPGTQ    xmm13,xmm13,xmm8
    VPCMPGTQ    xmm5,xmm5,xmm7
    VINSERTF128    ymm13 ,ymm13,xmm5,1
    VORPD    ymm12,ymm13,ymm11
    VPTEST    ymm12,ymm12
    JZ range_reduce

    VMULPD    ymm1,ymm0,L_point_five                         ;
    ;VFNMADDPD    ymm9 ,ymm1,ymm0,L_one         ; ymm9 1.0 - x*x*(double2)0.5;
    VFNMADD213PD  ymm1 , ymm0,L_one
    VMOVDQA  ymm9,  ymm1
    
    cos_piby4_s4f_fma3

	VMOVDQA xmm3,XMMWORD PTR L_mask_3e4
	VMOVDQA xmm6,XMMWORD PTR L_mask_3e4
    VPCMPGTQ    xmm3,xmm3,xmm8
    VPCMPGTQ    xmm6,xmm6,xmm7
    VINSERTF128    ymm3 ,ymm3,xmm6,1
	VMOVDQA xmm4,XMMWORD PTR L_mask_3f2
	VMOVDQA xmm5,XMMWORD PTR L_mask_3f2 ;xmm5 used instead of xmm7
    VPCMPGTQ    xmm4,xmm4,xmm8		
    VPCMPGTQ    xmm7,xmm5,xmm7
    VINSERTF128    ymm4 ,ymm4,xmm7,1

    VANDNPD    ymm1,ymm4,ymm13
    VANDPD    ymm1,ymm1,ymm0                        ; res2
    VANDNPD    ymm5,ymm3,ymm13
    VANDPD    ymm5,ymm4,ymm5
    VANDPD    ymm5,ymm5,ymm9
    VANDPD    ymm3,ymm3,ymm13
    VANDPD    ymm3,ymm3,L_one
    VORPD    ymm5,ymm5,ymm3                        ; res1 amm5
    VORPD    ymm0,ymm5,ymm1                        ; res_pi_4
    ;VPCMOV    ymm0 ,ymm10,ymm0,ymm11
    VANDNPD   ymm12  ,  ymm11,   ymm0 
    VANDPD    ymm0   ,  ymm10,   ymm11
    VORPD     ymm0   ,  ymm0,    ymm12
    
    VPTEST    ymm12,YMMWORD PTR L__allone_mask
    JC return_cos_s

    VMOVUPD    [res_pi_4 + rsp],ymm0

range_reduce:

    VORPD    ymm13,ymm13,ymm11
    VMOVUPD    [mas_res_pi_4 + rsp],ymm13
    VANDNPD    ymm0,ymm13,ymm8                     ; ymm0 x with the sign cleared
    VMOVUPD    [input + rsp],ymm0

    VCMPGEPD    ymm3,ymm0,L_e_5
    VPTEST    ymm3,ymm3
    JZ call_range_e5

    call_remainder_2fpiby2_4f_4d_fma3

    VMOVUPD    [r1 + rsp],ymm0
    VMOVUPD    [region1 + rsp],ymm11
    VMOVUPD    ymm0,[input + rsp]

    VCMPLTPD    ymm3,ymm0,L_e_5
    VPTEST    ymm3,ymm3
    JZ if_not_remainder

call_range_e5:

    range_e_5_s4f_fma3

if_not_remainder:


    VMOVUPD    ymm2,[input + rsp]
    VCMPLTPD    ymm3,ymm2,L_e_5

    ;VPCMOV    ymm7 ,ymm0,[r1 + rsp],ymm3
    ;VPCMOV    ymm11 ,ymm11,[region1 + rsp],ymm3
    
    VANDPD ymm7 , ymm0,   ymm3 
    VANDNPD ymm1 , ymm3,   [r1 + rsp] 
    VORPD ymm7 , ymm7,   ymm1 
    VANDNPD ymm0 , ymm3,   [region1 + rsp]
    VANDPD ymm11 , ymm11,  ymm3 
    VORPD ymm11 , ymm11,  ymm0 

    VMOVAPD    ymm0,ymm7

    sin_piby4_s4f_fma3

ret_sin_2fpiby4_s:

    VMOVAPD    ymm8,ymm0        ; store sin piby4
    VMOVAPD    ymm0,ymm7
    cos_piby4_s4f_fma3

ret_cos_2fpiby4_s:
    VEXTRACTF128    xmm6,ymm11,1

    VPCMPEQQ    xmm1,xmm11,XMMWORD PTR L_int_one
    VPCMPEQQ    xmm4,xmm6,XMMWORD PTR L_int_one
    VINSERTF128    ymm1 ,ymm1,xmm4,1

    VPCMPEQQ    xmm2,xmm11,XMMWORD PTR L_int_two
    VPCMPEQQ    xmm5,xmm6,XMMWORD PTR L_int_two
    VINSERTF128    ymm2 ,ymm2,xmm5,1

    VPCMPEQQ    xmm3,xmm11,XMMWORD PTR L_int_three
    VPCMPEQQ    xmm6,xmm6,XMMWORD PTR L_int_three
    VINSERTF128    ymm3 ,ymm3,xmm6,1


    VORPD    ymm4,ymm3,ymm1
    ;VPCMOV    ymm5 ,ymm8,ymm0,ymm4
    VANDPD   ymm5  ,  ymm8,   ymm4  
    VANDNPD  ymm11 ,  ymm4,   ymm0  
    VORPD    ymm5  ,  ymm5,   ymm11 

    VORPD    ymm4,ymm2,ymm1
    VANDPD    ymm4,ymm4,L_signbit
    VXORPD    ymm4,ymm5,ymm4

    VMOVUPD    ymm11,[mas_res_pi_4 + rsp]
    VMOVUPD    ymm0,[res_pi_4 + rsp]

    ;VPCMOV    ymm0 ,ymm0,ymm4,ymm11
    VANDNPD   ymm1  ,  ymm11,  ymm4 
    VANDPD    ymm0  ,  ymm0,   ymm11
    VORPD     ymm0  ,  ymm0,   ymm1 
    

return_cos_s:
    VCVTPD2PS    xmm0,ymm0
    	cmp rdi,-1
 jle L__store123
 movdqu [rbx + rdi * 4],xmm0
 jmp L__process_next4 
 
ALIGN 16 
L__store123:
 je L__store3
 cmp rdi,-3 
 je L__store1 
 ;store 2 
 add rdi,02h
 movsd QWORD PTR[rbx + rdi * 4],xmm0 
 jmp L__return

ALIGN 16 
L__store3:
 movdqa xmm1,xmm0
 psrldq xmm0,4
 inc rdi
 movss DWORD PTR[rbx + rdi * 4],xmm1
 inc rdi
 movsd QWORD PTR[rbx + rdi * 4],xmm0 
 jmp L__return
 
ALIGN 16 
L__store1:
 mov rdi,00h
 movss DWORD PTR[rbx + rdi * 4],xmm0 

L__return:
 RestoreReg rbx,save_rbx
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 
 ret 



fname        endp
TEXT    ENDS   
END 

