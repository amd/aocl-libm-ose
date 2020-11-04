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

FN_PROTOTYPE_FMA3 tan
fname_special   TEXTEQU <_tan_special>

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

tan_early_exit_s_1:

    CMP   r9,L_mask_3fe
    JG    tan_early_exit_s
	CMP   r9,L_mask_3f2
	JGE    compute_tan_pyby_4
	CMP   r9,L_mask_3e4
	JGE   compute_x_xxx_0_333	
	JMP return_tan_c
	
compute_x_xxx_0_333:

    VMULSD    xmm1,xmm0,xmm0                         ;
    VMULSD    xmm1,xmm1,xmm0                         ; xmm1 x3
    VFMADD231SD    xmm0 ,xmm1,L_point_333         ;  x + x*x*x*0.3333333333333333;
	JMP return_tan_c

compute_tan_pyby_4:
	VMOVAPD   [save_xmm6 + rsp ],xmm6
	VMOVAPD   [save_xmm7 + rsp ],xmm7
	tan_piby4_s_zero_fma3
    JMP    return_tan_s
	
tan_early_exit_s:
	MOV   r8,L__inf_mask_64
	AND   rax,r8
	CMP   rax, r8
    JZ    tan_naninf	

range_reduce:
	VMOVAPD   [save_xmm6 + rsp ],xmm6
	VMOVAPD   [save_xmm7 + rsp ],xmm7

    VMOVAPD    [store_input + rsp],xmm0
    VMOVQ   xmm0,r9                                ; r9 x with the sign cleared
	cmp  r9,L_e_5	
    JGE  tan_remainder_piby2
	


;tan_range_e_5_s:
    range_e_5_s_fma3
    JMP tan_exit_s
	
tan_remainder_piby2:
    call_remainder_piby2_fma3

tan_exit_s:
	VMOVAPD   [save_xmm8 + rsp ],xmm8
	VMOVAPD   [save_xmm9 + rsp ],xmm9
    VANDPD    xmm3,xmm7,XMMWORD PTR L_int_one
	VMOVQ    r8,xmm7
	AND      r8,01h
    tan_piby4_s_fma3
	VMOVAPD  xmm8,[save_xmm8 + rsp ]
	VMOVAPD  xmm9,[save_xmm9 + rsp ]
    VMOVAPD    xmm1,[store_input + rsp]
    VANDPD    xmm1,xmm1,L_signbit
    VXORPD    xmm0,xmm0,xmm1

return_tan_s:
	RestoreRegsAVX xmm6,  save_xmm6
	RestoreRegsAVX xmm7,  save_xmm7
	StackDeallocate stack_size
    ret

return_tan_c:
	StackDeallocate stack_size
    ret

tan_naninf:
    call    fname_special
	StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   
END 
