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

include fm.inc

include trig_func.inc

ALM_PROTO_FMA3 vrd2_cos

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveAllXMMRegs
        SaveRegsGPR r12,save_r12
        SaveRegsGPR r13,save_r13

    .ENDPROLOG

    movupd xmm0, XMMWORD PTR [rcx]
   

cos_early_exit_s:


    VCMPNEQPD    xmm1,xmm0,xmm0                       ;nan mask xmm1
    VANDPD    xmm8,xmm0,L__sign_mask            ; xmm8 clear sign
    VPCMPEQQ    xmm2,xmm8,L__inf_mask_64       ;xmm2 has inf mask
    VADDPD    xmm3,xmm0,xmm0                        ;nan value
    ;VPCMOV    xmm4 ,xmm3,xmm1,xmm1                    ;xmm4 nan value
    VANDPD  xmm4         ,xmm3,  xmm1 
    VANDNPD  xmm11    ,xmm1,          xmm1 
    VORPD   xmm4         ,xmm4,  xmm11
    
    
    
    VMOVAPD    xmm3,L_nan
    ;VPCMOV    xmm10 ,xmm3,xmm4,xmm2                ;xmm10 nan and inf values
    
    VANDPD  xmm10   ,xmm3,    xmm2 
    VANDNPD xmm11   ,xmm2,    xmm4 
    VORPD   xmm10   ,xmm10,   xmm11
    
    
    VORPD    xmm11,xmm2,xmm1                        ; xmm11 nan and inf mask

cos_early_exit_s_1:

    VMOVDQA xmm7,XMMWORD PTR L_mask_3f_9
	;VPCOMLEUQ   xmm7,xmm8,XMMWORD PTR L_mask_3fe
	VPCMPGTQ xmm7,xmm7,xmm8

    VORPD    xmm2,xmm7,xmm11
    VPTEST    xmm2,xmm2
    JZ range_reduce

    VMULPD    xmm1,xmm0,L_point_five                         ;
    ;VFNMADDPD    xmm9 ,xmm1,xmm0,L_one         ; xmm9 1.0 - x*x*(double2)0.5;
    
    VFNMADD213PD xmm1,xmm0,L_one
    VMOVDQA xmm9 ,xmm1
    
    VXORPD    xmm1,xmm1,xmm1
    cos_2fpiby4_s_fma3


    ;VPCOMLTUQ    xmm3,xmm8,XMMWORD PTR L_mask_3e4
	VMOVDQA xmm3,XMMWORD PTR L_mask_3e4
	VMOVDQA xmm4,XMMWORD PTR L_mask_3f2
	VPCMPGTQ xmm3,xmm3,xmm8
	VPCMPGTQ xmm4,xmm4,xmm8
    ;VPCOMLTUQ    xmm4,xmm8,XMMWORD PTR L_mask_3f2
    VANDNPD    xmm1,xmm4,xmm7
    VANDPD    xmm1,xmm1,xmm0                        ; res2
    VANDNPD    xmm5,xmm3,xmm7
    VANDPD    xmm5,xmm4,xmm5
    VANDPD    xmm5,xmm5,xmm9
    VANDPD    xmm3,xmm3,xmm7
    VANDPD    xmm3,xmm3,L_one
    VORPD    xmm5,xmm5,xmm3                        ; res1 amm5
    VORPD    xmm0,xmm5,xmm1                        ; res_pi_4
    ;VPCMOV    xmm0 ,xmm10,xmm0,xmm11
    
    VANDNPD xmm0    , xmm11,     xmm0
    VANDPD  xmm2    , xmm10,    xmm11
    VORPD   xmm0    , xmm0,     xmm2 
    
    

    VPTEST    xmm2,XMMWORD PTR L__allone_mask
    JC return_cos_s


range_reduce:

    VORPD    xmm7,xmm7,xmm11
    VMOVAPD    [res_pi_4 + rsp],xmm0
    VMOVAPD    [mas_res_pi_4 + rsp],xmm7
    VANDNPD    xmm0,xmm7,xmm8                     ; xmm0 x with the sign cleared
    VMOVAPD    [input + rsp],xmm0

    VCMPGEPD    xmm3,xmm0,L_e_5
    VPTEST    xmm3,xmm3

    JZ call_range_e5

    call_remainder_2d_piby2_fma3 
	

    VMOVAPD    [r1 + rsp],xmm0
    VMOVAPD    [rr1 + rsp],xmm1
    VMOVAPD    [region1 + rsp],xmm11
    ;JMP if_not_remainder

call_range_e5:

    VMOVAPD    xmm0,[input + rsp]
    range_e_5_2f_s_fma3



if_not_remainder:

    VMOVAPD    xmm2,[input + rsp]
    VCMPLTPD    xmm3,xmm2,L_e_5

   ; VPCMOV    xmm7 ,xmm0,[r1 + rsp],xmm3
   ; VPCMOV    xmm8 ,xmm1,[rr1 + rsp],xmm3
   ; VPCMOV    xmm11 ,xmm11,[region1 + rsp],xmm3
   
   
   VANDPD  xmm7   ,xmm0,     xmm3        
   VANDNPD xmm2   ,xmm3,     [r1 + rsp]    
   VORPD   xmm7   ,xmm7,     xmm2        
   
   VANDPD  xmm8   ,xmm1,     xmm3        
   VANDNPD xmm2   ,xmm3,     [rr1 + rsp]    
   VORPD   xmm8   ,xmm8,     xmm2        
   
   
   VANDNPD xmm2   ,xmm3  ,  [region1 + rsp]
   VANDPD  xmm11  ,xmm11,   xmm3        
   VORPD   xmm11  ,xmm11,    xmm2        
   
   
   


    VPCMPEQQ    xmm1,xmm11,XMMWORD PTR L_int_one
    VPCMPEQQ    xmm2,xmm11,XMMWORD PTR L_int_two
    VPCMPEQQ    xmm3,xmm11,XMMWORD PTR L_int_three

    VORPD    xmm10,xmm3,xmm1
    VORPD    xmm9,xmm2,xmm1

    VMOVAPD    xmm0,xmm7
    VMOVAPD    xmm1,xmm8

    VMOVD    r8d,xmm10
    VPUNPCKHQDQ    xmm10,xmm10,xmm10
    VMOVD    r9d,xmm10

    cmp    r8d,0FFFFFFFFh
    je compute_xxx_sin

    cmp    r9d,0FFFFFFFFh
    je compute_sin_cos


both_cos_cos:
        cos_2fpiby4_s_fma3

        jmp ret_cos_2fpiby4_s

compute_sin_cos:

        cos_piby4_s_fma3
        VMOVAPD    xmm10,xmm0
        VPUNPCKHQDQ    xmm0,xmm7,xmm7
        VPUNPCKHQDQ    xmm1,xmm8,xmm8
        sin_piby4_s_fma3
        VPUNPCKLQDQ    xmm0,xmm10,xmm0

        jmp ret_cos_2fpiby4_s

compute_xxx_sin:
    cmp    r9d,0FFFFFFFFh
    je compute_sin_sin

        sin_piby4_s_fma3
        VMOVAPD    xmm10,xmm0
        VPUNPCKHQDQ    xmm0,xmm7,xmm7
        VPUNPCKHQDQ    xmm1,xmm8,xmm8
        cos_piby4_s_fma3
        VPUNPCKLQDQ    xmm0,xmm10,xmm0
        jmp ret_cos_2fpiby4_s

compute_sin_sin:

    sin_2fpiby4_s_fma3

ret_cos_2fpiby4_s:

    VANDPD    xmm6,xmm9,L_signbit
    VXORPD    xmm6,xmm0,xmm6

    VMOVAPD    xmm8,[mas_res_pi_4 + rsp]
    VMOVAPD    xmm0,[res_pi_4 + rsp]

    ;VPCMOV    xmm0 ,xmm0,xmm6,xmm8
    VANDNPD  xmm1  , xmm8,  xmm6
    VANDPD   xmm0  , xmm0,  xmm8
    VORPD    xmm0  , xmm0,  xmm1


return_cos_s:


     
    RestoreAllXMMRegs
        RestoreRegsGPR  r12,save_r12
        RestoreRegsGPR  r13,save_r13

     StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   
END 

