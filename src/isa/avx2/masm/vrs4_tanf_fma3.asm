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

include fm.inc

include trig_func.inc

FN_PROTOTYPE_FMA3 vrs4_tanf

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveAllXMMRegs
        SaveRegsGPR r12,save_r12
        SaveRegsGPR r13,save_r13

    .ENDPROLOG

    movupd xmm0, XMMWORD PTR [rcx]
   
    
    VCVTPS2PD ymm0,xmm0
tan_early_exit_s:

    VMOVUPD    [store_input + rsp],ymm0


    VCMPNEQPD    ymm1,ymm0,ymm0                       ;nan mask ymm1
    VANDPD    ymm8,ymm0,L__sign_mask            ; ymm8 clear sign

    VEXTRACTF128    xmm3,ymm8,1
    VPCMPEQQ    xmm2,xmm8,L__inf_mask_64       ;
    VPCMPEQQ    xmm3,xmm3,L__inf_mask_64       ;
    VINSERTF128    ymm2 ,ymm2,xmm3,1                   ;ymm2 has inf mask

    VADDPD    ymm3,ymm0,ymm0                        ;nan value
    ;VPCMOV    ymm4 ,ymm3,ymm1,ymm1                    ;ymm4 nan value
    VANDNPD    ymm7  ,  ymm1, ymm1
    VANDPD     ymm4  ,  ymm3, ymm1
    VORPD      ymm4  ,  ymm4, ymm7
    
    VMOVUPD    ymm3,L_nan
    ;VPCMOV    ymm12 ,ymm3,ymm4,ymm2                ;ymm12 nan and inf values
    VANDNPD ymm7,ymm2,ymm4
    VANDPD  ymm12, ymm3, ymm2
    VORPD  ymm12  , ymm12,  ymm7
    
    VORPD    ymm11,ymm2,ymm1                        ; ymm11 nan and inf mask

tan_early_exit_s_1:

    VEXTRACTF128    xmm7,ymm8,1
	VMOVDQA xmm13,XMMWORD PTR L_mask_3f_9
	VMOVDQA xmm5,XMMWORD PTR L_mask_3f_9

    VPCMPGTQ    xmm13,xmm13,xmm8
    VPCMPGTQ    xmm5,xmm5,xmm7
    VINSERTF128    ymm13 ,ymm13,xmm5,1

    VORPD    ymm14,ymm13,ymm11
    VPTEST    ymm14,ymm14
    JZ range_reduce


    VMOVAPD    ymm10,ymm0
    VMULPD    ymm1,ymm0,ymm0                         ;
    VMULPD    ymm1,ymm1,ymm0                         ; ymm1 x3
    ;VFMADDPD    ymm14 ,ymm1,L_point_333,ymm0         ;  x + x*x*x*0.3333333333333333;
    VFMADD132PD  ymm1 , ymm0,L_point_333
    VMOVDQA  ymm14,  ymm1
    VXORPD    ymm2,ymm2,ymm2
    tan_piby4_s4f_fma3


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
    VANDPD    ymm1,ymm1,ymm0                ; res2

    VANDNPD    ymm5,ymm3,ymm13
    VANDPD    ymm5,ymm4,ymm5
    VANDPD    ymm5,ymm5,ymm14

    VANDPD    ymm3,ymm3,ymm13
    VANDPD    ymm3,ymm3,ymm10
    VORPD    ymm5,ymm5,ymm3                ; res1 amm5
    VORPD    ymm0,ymm5,ymm1                ; res_pi_4
    ;VPCMOV    ymm0 ,ymm12,ymm0,ymm11
    VANDNPD    ymm14  ,  ymm11, ymm0 
    VANDPD     ymm0   ,  ymm12, ymm11
    VORPD     ymm0  ,  ymm0,  ymm14

    VPTEST    ymm14,YMMWORD PTR L__allone_mask
    JC return_tanf_s

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

    ;VPCMOV    ymm0 ,ymm0,[r1 + rsp],ymm3
    ;VPCMOV    ymm11 ,ymm11,[region1 + rsp],ymm3
    
    VANDNPD  ymm2     ,  ymm3,    [r1 + rsp]     
    VANDPD   ymm0     ,  ymm0,     ymm3          
    VORPD    ymm0     ,  ymm0,     ymm2          
    VANDNPD  ymm2     ,  ymm3,    [region1 + rsp]
    VANDPD    ymm11   ,  ymm11,    ymm3          
    VORPD     ymm11   ,  ymm11,    ymm2          
    
    


    VANDPD    ymm2,ymm11,L_int_one

    tan_piby4_s4f_fma3

ret_tan_2fpiby4_s:

    VMOVUPD    ymm1,[store_input + rsp]
    VANDPD    ymm1,ymm1,L_signbit
    VXORPD    ymm4,ymm1,ymm0

    VMOVUPD    ymm11,[mas_res_pi_4 + rsp]
    VMOVUPD    ymm0,[res_pi_4 + rsp]

    ;VPCMOV    ymm0 ,ymm0,ymm4,ymm11
    VANDNPD    ymm2   ,  ymm11,   ymm4  
    VANDPD    ymm0    ,  ymm0,    ymm11 
    VORPD     ymm0    ,  ymm0,    ymm2  
return_tanf_s:
    VCVTPD2PS    xmm0,ymm0
     
    RestoreAllXMMRegs
        RestoreRegsGPR  r12,save_r12
        RestoreRegsGPR  r13,save_r13

     StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   
END 
