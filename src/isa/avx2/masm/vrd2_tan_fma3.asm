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

ALM_PROTO_FMA3 vrd2_tan

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveAllXMMRegs
        SaveRegsGPR r12,save_r12
        SaveRegsGPR r13,save_r13

    .ENDPROLOG

    vmovupd xmm0, XMMWORD PTR [rcx]
   

tan_early_exit_s:

    VMOVAPD    [store_input + rsp],xmm0


    VCMPNEQPD    xmm1,xmm0,xmm0                       ;nan mask xmm1
    VANDPD    xmm13,xmm0,L__sign_mask            ; xmm13 clear sign
    VPCMPEQQ    xmm2,xmm13,L__inf_mask_64       ;xmm2 has inf mask
    VADDPD    xmm3,xmm0,xmm0                        ;nan value
    ;xmm4 nan value
    VANDNPD   xmm11  ,  xmm1,   xmm1 
    VANDPD    xmm4   ,  xmm3,   xmm1 
    VORPD     xmm4   ,  xmm4,   xmm11
    
    VMOVAPD    xmm3,L_nan
    ;xmm12 nan and inf values
    VANDNPD  xmm11  ,  xmm2,    xmm4 
    VANDPD   xmm12  ,  xmm3,    xmm2 
    VORPD     xmm12 ,  xmm12,   xmm11
    
    VORPD    xmm11,xmm2,xmm1                        ; xmm11 nan and inf mask


tan_early_exit_s_1:
	VMOVDQA xmm7,XMMWORD PTR L_mask_3f_9
    VPCMPGTQ xmm7,xmm7,xmm13
    VORPD    xmm2,xmm7,xmm11
    VPTEST    xmm2,xmm2
    JZ range_reduce

    VMOVAPD    xmm10,xmm0
    VMULPD    xmm14,xmm0,xmm0                         ;
    VMULPD    xmm14,xmm14,xmm0                         ; xmm1 x3
    VFMADD132PD xmm14,xmm0,L_point_333				 ;  x + x*x*x*0.3333333333333333;

    VXORPD    xmm3,xmm3,xmm3
	VXORPD    xmm1,xmm1,xmm1
    tan_2fpiby4_s_fma3

	VMOVDQA xmm3,XMMWORD PTR L_mask_3e4
	VMOVDQA xmm4,XMMWORD PTR L_mask_3f2
    VPCMPGTQ    xmm3,xmm3,xmm13
    VPCMPGTQ    xmm4,xmm4,xmm13

    VANDNPD    xmm1,xmm4,xmm7
    VANDPD    xmm1,xmm1,xmm0                ; res2

    VANDNPD    xmm5,xmm3,xmm7
    VANDPD    xmm5,xmm4,xmm5
    VANDPD    xmm5,xmm5,xmm14

    VANDPD    xmm3,xmm3,xmm7
    VANDPD    xmm3,xmm3,xmm10
    VORPD    xmm5,xmm5,xmm3                ; res1 amm5
    VORPD    xmm0,xmm5,xmm1                ; res_pi_4

    VANDNPD  xmm3 ,  xmm11,  xmm0 
    VANDPD   xmm0 ,  xmm12,  xmm11
    VORPD    xmm0 ,  xmm0,  xmm3 
                    
range_reduce:

    VORPD    xmm7,xmm7,xmm11
    VMOVAPD    [res_pi_4 + rsp],xmm0
    VMOVAPD    [mas_res_pi_4 + rsp],xmm7

    VANDNPD    xmm0,xmm7,xmm13                     ; xmm0 x with the sign cleared
    VMOVAPD    [input + rsp],xmm0

    VCMPGEPD    xmm3,xmm0,L_e_5
    VPTEST    xmm3,xmm3

    JZ call_range_e5

    call_remainder_2d_piby2_fma3

    VMOVAPD    [r1 + rsp],xmm0
    VMOVAPD    [rr1 + rsp],xmm1
    VMOVAPD    [region1 + rsp],xmm11

call_range_e5:

    VMOVAPD    xmm0,[input + rsp]
    range_e_5_2f_s_fma3

if_not_remainder:


    VMOVAPD    xmm2,[input + rsp]
    VCMPLTPD    xmm3,xmm2,L_e_5
  
	 VANDNPD xmm2 , xmm3,    [r1 + rsp] 
	 VANDPD xmm0 , xmm0,     xmm3 
	 VORPD xmm0, xmm0, xmm2 

	 VANDNPD xmm2 , xmm3,    [rr1 + rsp] 
	 VANDPD xmm1 , xmm1,     xmm3 
	 VORPD xmm1, xmm2, xmm1 


	 VANDNPD xmm2 , xmm3,    [region1 + rsp]
	 VANDPD xmm11 , xmm11,   xmm3 
	 VORPD xmm11 , xmm11,    xmm2 

     VANDPD    xmm3,xmm11,L_int_one

     tan_2fpiby4_s_fma3

ret_tan_2fpiby4_s:

    VMOVAPD    xmm1,[store_input + rsp]
    VANDPD    xmm1,xmm1,L_signbit
    VXORPD    xmm4,xmm1,xmm0

    VMOVAPD    xmm8,[mas_res_pi_4 + rsp]
    VMOVAPD    xmm0,[res_pi_4 + rsp]
 

    VANDNPD   xmm4   ,  xmm8,  xmm4
    VANDPD    xmm0   ,  xmm8,  xmm0 
    VORPD     xmm0  ,  xmm0,  xmm4 

return_cos_s:

    RestoreAllXMMRegs
        RestoreRegsGPR  r12,save_r12
        RestoreRegsGPR  r13,save_r13

     StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   
END 

