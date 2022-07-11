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

ALM_PROTO_FMA3 cosf
fname_special   TEXTEQU <_cosf_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC


text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    .ENDPROLOG   

cosf_early_exit_s:

	VMOVD  eax,xmm0
	MOV   r8d,L__inf_mask_32
	AND   eax,r8d
	CMP   eax, r8d
	JZ    cosf_naninf	

	VCVTSS2SD    xmm5,xmm0,xmm0
	VMOVQ    r9,xmm5
	AND    r9,L__sign_mask            ;clear sign	   

cosf_early_exit_s_1:

    CMP   r9,L_mask_3fe
    JG    range_reduce
	CMP   r9,L_mask_3f8
	JGE   compute_cosf_pyby_4
	CMP   r9,L_mask_3f2
	JGE   compute_1_xx_5
	
	VMOVQ xmm0,QWORD PTR L_one
	JMP return_cosf_s

compute_1_xx_5:

    VMULSD    xmm0,xmm5,QWORD PTR L_point_five                         ;	
    VFNMADD213SD    xmm0,xmm5,L_one         ; xmm9 1.0 - x*x*(double2)0.5;
	JMP return_cosf_s

compute_cosf_pyby_4:
	MOVSD xmm0,xmm5
	cos_piby4_sf_fma3
    JMP    return_cosf_s



range_reduce:
	VMOVQ   xmm0,r9                                ; r9 x with the sign cleared
	cmp  r9,L_e_5	
	JGE  cosf_remainder_piby2


;cosff_range_e_5_s:
    range_e_5_sf_fma3
    JMP cosf_exit_s

cosf_remainder_piby2:
    call_remainder_piby2_f


cosf_exit_s:

	VMOVQ rax,xmm4
	and   rax,01h
	cmp   rax,01h

    JNZ    cosf_piby4_compute

;sinf_piby4_compute:
    sin_piby4_sf_fma3
    JMP cosf_exit_s_1

cosf_piby4_compute:
    cos_piby4_sf_fma3

cosf_exit_s_1:

    VPCMPEQQ    xmm1,xmm4,XMMWORD PTR L_int_one
	VPCMPEQQ    xmm2,xmm4,XMMWORD PTR L_int_two
    VORPD    xmm3,xmm2,xmm1

    VANDPD    xmm3,xmm3,L_signbit
    VXORPD    xmm0,xmm0,xmm3

return_cosf_s:

  VCVTSD2SS    xmm0,xmm0,xmm0
    StackDeallocate stack_size
    ret
    
cosf_naninf:
    call    fname_special
    StackDeallocate stack_size
    ret
	

fname        endp
TEXT    ENDS   
END 
