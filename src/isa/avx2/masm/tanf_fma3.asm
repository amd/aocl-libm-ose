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

FN_PROTOTYPE_FMA3 tanf
fname_special   TEXTEQU <_tanf_special>

;Define name and any external functions being called
EXTERN       fname_special      : PROC

text SEGMENT EXECUTE
PUBLIC fname
fname PROC FRAME
	StackAllocate stack_size
    .ENDPROLOG   
	
tan_early_exit_s:

	VMOVD  eax,xmm0
	MOV   r8d,L__inf_mask_32
	AND	  eax,r8d
	CMP   eax, r8d
	JZ    tanf_naninf	

	VCVTSS2SD    xmm5,xmm0,xmm0
	VMOVQ    r9,xmm5
	AND    r9,L__sign_mask            ;clear sign	   


tan_early_exit_s_1:

	CMP   r9,L_mask_3fe
	JG    range_reduce
	CMP   r9,L_mask_3f8
	JGE    compute_tanf_pyby_4
	CMP   r9,L_mask_3f2
	JGE   compute_x_xxx_0_333	
	JMP return_tanf_c

compute_x_xxx_0_333:
	VMULSD    xmm2,xmm5,xmm5                         ;
	VMULSD    xmm0,xmm2,xmm5                         ; xmm6 x3
	VFMADD132SD    xmm0 ,xmm5,L_point_333        ;  x + x*x*x*0.3333333333333333;
	JMP return_tanf_s


compute_tanf_pyby_4:

	VMOVSD xmm0,xmm5,xmm5
	tan_piby4_sf_zeor_fma3

	JMP    return_tanf_s


	
range_reduce:
    
    VMOVQ   xmm0,r9                                ; r9 x with the sign cleared
	cmp  r9,L_e_5	
    JGE  tanf_remainder_piby2
	
;tan_range_e_5_s:

    range_e_5_sf_fma3
    JMP tanf_exit_s

tanf_remainder_piby2:
    call_remainder_piby2_f


tanf_exit_s:
    VANDPD    xmm2,xmm4,XMMWORD PTR L_int_one
	movd eax,xmm2
    tan_piby4_sf_fma3 

tanf_exit_s_1:

    VANDPD    xmm5,xmm5,L_signbit
    VXORPD    xmm0,xmm0,xmm5


return_tanf_s:

    VCVTSD2SS    xmm0,xmm0,xmm0   	


return_tanf_c:
	StackDeallocate stack_size
    ret


tanf_naninf:
    call    fname_special
	StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   
END 
