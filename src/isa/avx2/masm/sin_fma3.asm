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

ALM_PROTO_FMA3 sin

fname_special   TEXTEQU <_sin_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

text SEGMENT EXECUTE
PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG   

	VMOVQ  r9,xmm0
	MOV   rax,r9
	AND   r9,L__sign_mask            ;clear sign	
	
sin_early_exit_s_1:

	CMP   r9,L_mask_3fe
	JG    sin_early_exit_s
	CMP   r9,L_mask_3f2
	JGE   compute_sin_pyby_4
	CMP   r9,L_mask_3e4
	JGE   compute_x_xxx_0_1666	
	JMP   return_sin_c

compute_x_xxx_0_1666:

	VMULSD    xmm1,xmm0,xmm0                         ;
	VMULSD    xmm1,xmm1,xmm0                         ; xmm1 x3
	VFNMADD231SD xmm0,xmm1,L_point_166         ;  x - x*x*x*0.166666666666666666;
	JMP return_sin_c

compute_sin_pyby_4:
	sin_piby4_s_xx_zero_fma3
	JMP    return_sin_c

sin_early_exit_s:

	MOV   r8,L__inf_mask_64
	AND   rax,r8
	CMP   rax, r8
	JZ    sin_naninf	

range_reduce:
	vmovdqa      XMMWORD PTR [save_xmm6+rsp], xmm6
	vmovdqa      XMMWORD PTR [save_xmm7+rsp], xmm7

	VMOVAPD xmm6,xmm0
	VMOVQ   xmm0,r9                                ; r9 x with the sign cleared
	cmp  r9,L_e_5	
	JGE  sin_remainder_piby2

;sin_range_e_5_s:
	range_e_5_s_fma3
	JMP sin_exit_s
	
sin_remainder_piby2:

    call_remainder_piby2_fma3

sin_exit_s:

	VMOVQ rax,xmm7
	and   rax,01h
	cmp   rax,01h
	JZ   cos_piby4_compute
	
sin_piby4_compute:
	sin_piby4_s_fma3
	JMP sin_exit_s_1
	
cos_piby4_compute:
	cos_piby4_s_fma3

sin_exit_s_1:

	VPCMPEQQ xmm2,xmm7,XMMWORD PTR L_int_two
	VPCMPEQQ xmm3,xmm7,XMMWORD PTR L_int_three
	VORPD     xmm3,xmm2,xmm3
	VANDNPD   xmm3,xmm3,L_signbit
	VXORPD    xmm0,xmm0,xmm3
	VANDNPD   xmm6,xmm6,L_signbit
	VXORPD    xmm0,xmm6,xmm0
	
return_sin_s:
	RestoreRegsAVX xmm6,  save_xmm6
	RestoreRegsAVX xmm7,  save_xmm7
	StackDeallocate stack_size
    ret

	
return_sin_c:
	StackDeallocate stack_size
    ret
	
sin_naninf:
    call    fname_special
    StackDeallocate stack_size
    ret

fname        endp
TEXT    ENDS   
END 
