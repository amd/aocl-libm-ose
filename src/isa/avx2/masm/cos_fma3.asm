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

FN_PROTOTYPE_FMA3 cos
fname_special   TEXTEQU <_cos_special>

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

cos_early_exit_s_1:

    CMP   r9,L_mask_3fe
    JG    cos_early_exit_s
	CMP   r9,L_mask_3f2
	JGE    compute_cos_pyby_4
	CMP   r9,L_mask_3e4
	JGE   compute_1_xx_5
	VMOVQ xmm0,QWORD PTR L_one
	JMP return_cos_c

compute_1_xx_5:

    VMULSD    xmm1,xmm0,QWORD PTR L_point_five                         ;
    VFNMADD213SD    xmm0,xmm1,L_one         ; xmm9 1.0 - x*x*(double2)0.5;
	JMP return_cos_c

compute_cos_pyby_4:
	VPXOR  xmm1,xmm1,xmm1
	cos_piby4_s_fma3
    JMP    return_cos_c

cos_early_exit_s:
	
	MOV   r8,L__inf_mask_64
	AND   rax,r8
	CMP   rax, r8
    JZ    cos_naninf	

range_reduce:

	vmovdqa      XMMWORD PTR [save_xmm6+rsp], xmm6
	vmovdqa      XMMWORD PTR [save_xmm7+rsp], xmm7
		
	VMOVAPD xmm6,xmm0
    VMOVQ   xmm0,r9                                ; r9 x with the sign cleared
	cmp  r9,L_e_5	
    JGE  cos_remainder_piby2

;cos_range_e_5_s:
    range_e_5_s_fma3
    JMP cos_exit_s

cos_remainder_piby2:
    call_remainder_piby2_fma3

cos_exit_s:

	VMOVQ rax,xmm7
	and   rax,01h
	cmp   rax,01h
	JZ    sin_piby4_compute

cos_piby4_compute:
    cos_piby4_s_fma3
    JMP cos_exit_s_1

sin_piby4_compute:
    sin_piby4_s_fma3

cos_exit_s_1:

    VPCMPEQQ    xmm1,xmm7,XMMWORD PTR L_int_one
    VPCMPEQQ    xmm2,xmm7,XMMWORD PTR L_int_two
    VORPD    xmm3,xmm2,xmm1

    VANDPD    xmm3,xmm3,L_signbit
    VXORPD    xmm0,xmm0,xmm3

return_cos_s:
	RestoreRegsAVX xmm6,  save_xmm6
	RestoreRegsAVX xmm7,  save_xmm7
    StackDeallocate stack_size
    ret

return_cos_c:
	StackDeallocate stack_size
    ret


cos_naninf:
    call    fname_special
    StackDeallocate stack_size
    ret

fname        endp
TEXT    ENDS   
END 
