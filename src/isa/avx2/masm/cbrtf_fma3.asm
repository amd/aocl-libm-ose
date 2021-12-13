;
; Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

;
; vrs4_cbrtf.s
;
; A vector implementation of the cbrtf libm function.
;  This routine implemented in single precision.  It is slightly
;  less accurate than the double precision version, but it will
;  be better for vectorizing.
;
; Prototype
;
;     float cbrtf(float x);
;
;   Computes the floating point single precision cube root of x.
;   Returns proper C99 values, but may not raise status flags properly.
;
;


include fm.inc

save_xmm6       EQU     00h
save_xmm7       EQU     020h
save_xmm8       EQU     040h
save_xmm9       EQU     060h
save_xmm10      EQU     080h
save_xmm11      EQU     0A0h
save_xmm12      EQU     0C0h
save_xmm13      EQU     0E0h
save_xmm14      EQU     0100h
save_xmm15      EQU     0120h

stack_size      EQU     0148h 		; We take 8 as the last nibble to allow for 
                              		; alligned data movement.




ALM_PROTO_FMA3 cbrtf

fname_specail TEXTEQU <_cbrtf_special>
EXTERN       fname_specail      : PROC


LOAD_INDEXED MACRO address, ind_reg, reg_val
    	lea  r11, address
    	VMOVD eax,ind_reg
    	CDQE
    	VMOVLPD  reg_val, reg_val, QWORD PTR [r11 + rax * 8]
ENDM

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveAllXMMRegs
    .ENDPROLOG   


	VMOVAPS xmm9,xmm0									;store the input for later use
	VMOVQ  rax,xmm0
	MOV   r9,rax
	AND   r9d,L__sign_mask
	MOV   r8d,L__inf_mask
	AND	  rax,r8
    CMP   eax, r8d
    JZ    cbrtf_naninf	
	CMP   r9d,L__zero
	JE return_cbrtf
	
	VPXOR xmm6,xmm6,xmm6
	VMOVD	r8d,xmm0
	AND  r8d,L__double_exp3_mask
	SHR	 r8,23				; r8d exponent value
	VMOVD xmm1,r8d	
	CMP r8d,0h
	JNE skip_denormal

denormal_processing:
    VMULPS xmm0,xmm0,XMMWORD PTR L__2_pow_23  					;xmm4 denormal f
	VANDPS xmm2,xmm0,XMMWORD PTR L__double_exp3_mask
	VPSRLD xmm1,xmm2,23				       				;xmm2 is exp
	VCMPEQPD xmm6,xmm6,xmm6
			
skip_denormal:
	
	VPSUBD xmm1,xmm1,XMMWORD PTR L__float_exp_bias 			;xmm1 has exp.

	VMOVD	r8d,xmm1
	VCVTSI2SS xmm3,xmm3,r8d

	VMULPS xmm2,xmm3,XMMWORD PTR L__float_oneby3
	VCVTTSS2SI r8d,xmm2
	MOV  r9,r8
	ADD	r8,r8
	ADD r8,r9
	VMOVD xmm4,r8d
	VMOVD xmm2,r9d

	VPSUBD xmm10,xmm1,xmm4								; xmm10 is remainder
	VPADDD xmm2,xmm2,XMMWORD PTR L__double_exp_bias
	VPMOVSXDQ xmm2,xmm2
 	VPSLLQ xmm2,xmm2,52
	VANDPS xmm0,xmm0,XMMWORD PTR L__double_exp1_mask
	VORPS xmm0,xmm0,XMMWORD PTR L__double_exp2_mask			;xmm0 is f
	VANDPS xmm3,xmm0,XMMWORD PTR L__double_exp4_mask
 	VPSRLD xmm3,xmm3,15									;xmm3 is index
	LOAD_INDEXED L__DoubleReciprocalTable_256,xmm3,xmm5  ; xmm5  is reciprocal
	VCVTSS2SD xmm0,xmm0,xmm0									; f to double
	VMOVAPD xmm4,L__double_n_real_one
	;VFMADDSD xmm4,xmm0,xmm5,xmm4   						; r  xmm4
	VFMADD231SD   xmm4, xmm0,xmm5
	
	VMULSD xmm5,xmm4,xmm4								; r^2 xmm5
	VMOVAPD xmm7,L__double_real_one
	;VFMADDSD xmm7,xmm4,L__coefficient_1,xmm7
	;VFMADDSD xmm7,xmm5,L__coefficient_2,xmm7	    ;xmm7 is oneplusrcuberoot
	VFMADD231SD    xmm7, xmm4,L__coefficient_1
	VFMADD231SD    xmm7, xmm5,L__coefficient_2	
	
	
	LOAD_INDEXED L__definedCubeRootTable_,xmm10,xmm5     ; xmm5 scale constant
	VMULSD xmm2,xmm2,xmm5										;xmm2  scale * constant
	LOAD_INDEXED L__DoubleCubeRootTable_256,xmm3,xmm5     ; xmm5 is CubeRootTable_
	VMULSD xmm2,xmm2,xmm5			 							 ;
	VMULSD xmm2,xmm2,xmm7									 ;resultcuberoot xmm2
	VMULSD xmm3,xmm2,L__203
	VCVTSD2SS xmm2,xmm2,xmm2
	VCVTSD2SS xmm3,xmm3,xmm3
	VANDPS xmm9,xmm9,XMMWORD PTR L__real_sign
	;VPCMOV xmm0,xmm3,xmm2,xmm6
	VANDPS  xmm3, xmm3, xmm6
	VANDNPS   xmm6, xmm6,xmm2
 	VORPS   xmm0, xmm6,xmm3
	VXORPS xmm0,xmm0,xmm9

return_cbrtf:

    RestoreAllXMMRegs
    StackDeallocate stack_size
    ret

cbrtf_naninf:
	CMP r9d,L__inf_mask
	JE return_cbrtf
	call fname_specail
    RestoreAllXMMRegs
    StackDeallocate stack_size
    ret


fname        endp
TEXT    ENDS   

data SEGMENT READ

CONST    SEGMENT         


ALIGN 16

L__zero  DD 00h
 DD 00h
 DQ 00h
L__inf_mask			DD 7F800000h
					DD 7F800000h
                        DQ 7F8000007F800000h

L__qnan_mask			DD 00400000h
						DD 00400000h
                        DQ 0040000000400000h

L__sign_mask          DD 7FFFFFFFh
					  DD 7FFFFFFFh
                        DQ 7FFFFFFF7FFFFFFFh

L__float_exp_bias     DQ 0000007F0000007Fh
                        DQ 0000007F0000007Fh

L__double_exp_bias    DQ 000003FF000003FFh
                        DQ 000003FF000003FFh

L__double_exp1_mask   DQ 3FFFFFFF3FFFFFFFh
                        DQ 3FFFFFFF3FFFFFFFh


L__double_exp2_mask   DQ 3F8000003F800000h
                        DQ 3F8000003F800000h

L__double_exp3_mask    DD 7F800000h
					   DD 7F800000h
                         DQ 7F8000007F800000h

L__double_exp4_mask    DQ 007F8000007F8000h
                         DQ 007F8000007F8000h

L__real_sign          DQ 8000000080000000h    ; sign bit
                        DQ 8000000080000000h

L__float_oneby3       DQ 3EAAAAAB3EAAAAABh     ; (1/3)
                        DQ 3EAAAAAB3EAAAAABh

L__2_pow_23 		    DQ 4B0000004B000000h
						DQ 4B0000004B000000h


L__203				DQ 3F7428A2F98D728Bh
						DQ 3F7428A2F98D728Bh
						DQ 3F7428A2F98D728Bh
						DQ 3F7428A2F98D728Bh

L__coefficient_1      DQ 3fd5555555555555h ; (1/3)
                        DQ 3fd5555555555555h
						DQ 3fd5555555555555h ; (1/3)
                        DQ 3fd5555555555555h

L__coefficient_2      DQ 0bFBC71C71C71C71Ch ;  (1/9)
                        DQ 0bFBC71C71C71C71Ch
						DQ 0bFBC71C71C71C71Ch ;  (1/9)
                        DQ 0bFBC71C71C71C71Ch

L__double_real_one    DQ 3ff0000000000000h ; 1.0
                        DQ 3ff0000000000000h
						DQ 3ff0000000000000h ; 1.0
                        DQ 3ff0000000000000h

L__double_n_real_one  DQ 0bff0000000000000h ; 1.0
                        DQ 0bff0000000000000h
                        DQ 0bff0000000000000h ; 1.0
                        DQ 0bff0000000000000h


L__DoubleReciprocalTable_256 DQ 3ff0000000000000h
            DQ 3fefe00000000000h
            DQ 3fefc00000000000h
            DQ 3fefa00000000000h
            DQ 3fef800000000000h
            DQ 3fef600000000000h
            DQ 3fef400000000000h
            DQ 3fef200000000000h
            DQ 3fef000000000000h
            DQ 3feee00000000000h
            DQ 3feec00000000000h
            DQ 3feea00000000000h
            DQ 3fee900000000000h
            DQ 3fee700000000000h
            DQ 3fee500000000000h
            DQ 3fee300000000000h
            DQ 3fee100000000000h
            DQ 3fee000000000000h
            DQ 3fede00000000000h
            DQ 3fedc00000000000h
            DQ 3feda00000000000h
            DQ 3fed900000000000h
            DQ 3fed700000000000h
            DQ 3fed500000000000h
            DQ 3fed400000000000h
            DQ 3fed200000000000h
            DQ 3fed000000000000h
            DQ 3fecf00000000000h
            DQ 3fecd00000000000h
            DQ 3fecb00000000000h
            DQ 3feca00000000000h
            DQ 3fec800000000000h
            DQ 3fec700000000000h
            DQ 3fec500000000000h
            DQ 3fec300000000000h
            DQ 3fec200000000000h
            DQ 3fec000000000000h
            DQ 3febf00000000000h
            DQ 3febd00000000000h
            DQ 3febc00000000000h
            DQ 3feba00000000000h
            DQ 3feb900000000000h
            DQ 3feb700000000000h
            DQ 3feb600000000000h
            DQ 3feb400000000000h
            DQ 3feb300000000000h
            DQ 3feb200000000000h
            DQ 3feb000000000000h
            DQ 3feaf00000000000h
            DQ 3fead00000000000h
            DQ 3feac00000000000h
            DQ 3feaa00000000000h
            DQ 3fea900000000000h
            DQ 3fea800000000000h
            DQ 3fea600000000000h
            DQ 3fea500000000000h
            DQ 3fea400000000000h
            DQ 3fea200000000000h
            DQ 3fea100000000000h
            DQ 3fea000000000000h
            DQ 3fe9e00000000000h
            DQ 3fe9d00000000000h
            DQ 3fe9c00000000000h
            DQ 3fe9a00000000000h
            DQ 3fe9900000000000h
            DQ 3fe9800000000000h
            DQ 3fe9700000000000h
            DQ 3fe9500000000000h
            DQ 3fe9400000000000h
            DQ 3fe9300000000000h
            DQ 3fe9200000000000h
            DQ 3fe9000000000000h
            DQ 3fe8f00000000000h
            DQ 3fe8e00000000000h
            DQ 3fe8d00000000000h
            DQ 3fe8b00000000000h
            DQ 3fe8a00000000000h
            DQ 3fe8900000000000h
            DQ 3fe8800000000000h
            DQ 3fe8700000000000h
            DQ 3fe8600000000000h
            DQ 3fe8400000000000h
            DQ 3fe8300000000000h
            DQ 3fe8200000000000h
            DQ 3fe8100000000000h
            DQ 3fe8000000000000h
            DQ 3fe7f00000000000h
            DQ 3fe7e00000000000h
            DQ 3fe7d00000000000h
            DQ 3fe7b00000000000h
            DQ 3fe7a00000000000h
            DQ 3fe7900000000000h
            DQ 3fe7800000000000h
            DQ 3fe7700000000000h
            DQ 3fe7600000000000h
            DQ 3fe7500000000000h
            DQ 3fe7400000000000h
            DQ 3fe7300000000000h
            DQ 3fe7200000000000h
            DQ 3fe7100000000000h
            DQ 3fe7000000000000h
            DQ 3fe6f00000000000h
            DQ 3fe6e00000000000h
            DQ 3fe6d00000000000h
            DQ 3fe6c00000000000h
            DQ 3fe6b00000000000h
            DQ 3fe6a00000000000h
            DQ 3fe6900000000000h
            DQ 3fe6800000000000h
            DQ 3fe6700000000000h
            DQ 3fe6600000000000h
            DQ 3fe6500000000000h
            DQ 3fe6400000000000h
            DQ 3fe6300000000000h
            DQ 3fe6200000000000h
            DQ 3fe6100000000000h
            DQ 3fe6000000000000h
            DQ 3fe5f00000000000h
            DQ 3fe5e00000000000h
            DQ 3fe5d00000000000h
            DQ 3fe5c00000000000h
            DQ 3fe5b00000000000h
            DQ 3fe5a00000000000h
            DQ 3fe5900000000000h
            DQ 3fe5800000000000h
            DQ 3fe5800000000000h
            DQ 3fe5700000000000h
            DQ 3fe5600000000000h
            DQ 3fe5500000000000h
            DQ 3fe5400000000000h
            DQ 3fe5300000000000h
            DQ 3fe5200000000000h
            DQ 3fe5100000000000h
            DQ 3fe5000000000000h
            DQ 3fe5000000000000h
            DQ 3fe4f00000000000h
            DQ 3fe4e00000000000h
            DQ 3fe4d00000000000h
            DQ 3fe4c00000000000h
            DQ 3fe4b00000000000h
            DQ 3fe4a00000000000h
            DQ 3fe4a00000000000h
            DQ 3fe4900000000000h
            DQ 3fe4800000000000h
            DQ 3fe4700000000000h
            DQ 3fe4600000000000h
            DQ 3fe4600000000000h
            DQ 3fe4500000000000h
            DQ 3fe4400000000000h
            DQ 3fe4300000000000h
            DQ 3fe4200000000000h
            DQ 3fe4200000000000h
            DQ 3fe4100000000000h
            DQ 3fe4000000000000h
            DQ 3fe3f00000000000h
            DQ 3fe3e00000000000h
            DQ 3fe3e00000000000h
            DQ 3fe3d00000000000h
            DQ 3fe3c00000000000h
            DQ 3fe3b00000000000h
            DQ 3fe3b00000000000h
            DQ 3fe3a00000000000h
            DQ 3fe3900000000000h
            DQ 3fe3800000000000h
            DQ 3fe3800000000000h
            DQ 3fe3700000000000h
            DQ 3fe3600000000000h
            DQ 3fe3500000000000h
            DQ 3fe3500000000000h
            DQ 3fe3400000000000h
            DQ 3fe3300000000000h
            DQ 3fe3200000000000h
            DQ 3fe3200000000000h
            DQ 3fe3100000000000h
            DQ 3fe3000000000000h
            DQ 3fe3000000000000h
            DQ 3fe2f00000000000h
            DQ 3fe2e00000000000h
            DQ 3fe2e00000000000h
            DQ 3fe2d00000000000h
            DQ 3fe2c00000000000h
            DQ 3fe2b00000000000h
            DQ 3fe2b00000000000h
            DQ 3fe2a00000000000h
            DQ 3fe2900000000000h
            DQ 3fe2900000000000h
            DQ 3fe2800000000000h
            DQ 3fe2700000000000h
            DQ 3fe2700000000000h
            DQ 3fe2600000000000h
            DQ 3fe2500000000000h
            DQ 3fe2500000000000h
            DQ 3fe2400000000000h
            DQ 3fe2300000000000h
            DQ 3fe2300000000000h
            DQ 3fe2200000000000h
            DQ 3fe2100000000000h
            DQ 3fe2100000000000h
            DQ 3fe2000000000000h
            DQ 3fe2000000000000h
            DQ 3fe1f00000000000h
            DQ 3fe1e00000000000h
            DQ 3fe1e00000000000h
            DQ 3fe1d00000000000h
            DQ 3fe1c00000000000h
            DQ 3fe1c00000000000h
            DQ 3fe1b00000000000h
            DQ 3fe1b00000000000h
            DQ 3fe1a00000000000h
            DQ 3fe1900000000000h
            DQ 3fe1900000000000h
            DQ 3fe1800000000000h
            DQ 3fe1800000000000h
            DQ 3fe1700000000000h
            DQ 3fe1600000000000h
            DQ 3fe1600000000000h
            DQ 3fe1500000000000h
            DQ 3fe1500000000000h
            DQ 3fe1400000000000h
            DQ 3fe1300000000000h
            DQ 3fe1300000000000h
            DQ 3fe1200000000000h
            DQ 3fe1200000000000h
            DQ 3fe1100000000000h
            DQ 3fe1100000000000h
            DQ 3fe1000000000000h
            DQ 3fe0f00000000000h
            DQ 3fe0f00000000000h
            DQ 3fe0e00000000000h
            DQ 3fe0e00000000000h
            DQ 3fe0d00000000000h
            DQ 3fe0d00000000000h
            DQ 3fe0c00000000000h
            DQ 3fe0c00000000000h
            DQ 3fe0b00000000000h
            DQ 3fe0a00000000000h
            DQ 3fe0a00000000000h
            DQ 3fe0900000000000h
            DQ 3fe0900000000000h
            DQ 3fe0800000000000h
            DQ 3fe0800000000000h
            DQ 3fe0700000000000h
            DQ 3fe0700000000000h
            DQ 3fe0600000000000h
            DQ 3fe0600000000000h
            DQ 3fe0500000000000h
            DQ 3fe0500000000000h
            DQ 3fe0400000000000h
            DQ 3fe0400000000000h
            DQ 3fe0300000000000h
            DQ 3fe0300000000000h
            DQ 3fe0200000000000h
            DQ 3fe0200000000000h
            DQ 3fe0100000000000h
            DQ 3fe0100000000000h
            DQ 3fe0000000000000h

ALIGN 16
L__DoubleCubeRootTable_256   DQ 3ff0000000000000h
                         DQ 3ff00558e6547c36h
                         DQ 3ff00ab8f9d2f374h
                         DQ 3ff010204b673fc7h
                         DQ 3ff0158eec36749bh
                         DQ 3ff01b04ed9fdb53h
                         DQ 3ff02082613df53ch
                         DQ 3ff0260758e78308h
                         DQ 3ff02b93e6b091f0h
                         DQ 3ff031281ceb8ea2h
                         DQ 3ff036c40e2a5e2ah
                         DQ 3ff03c67cd3f7ceah
                         DQ 3ff03f3c9fee224ch
                         DQ 3ff044ec379f7f79h
                         DQ 3ff04aa3cd578d67h
                         DQ 3ff0506374d40a3dh
                         DQ 3ff0562b4218a6e3h
                         DQ 3ff059123d3a9848h
                         DQ 3ff05ee6694e7166h
                         DQ 3ff064c2ee6e07c6h
                         DQ 3ff06aa7e19c01c5h
                         DQ 3ff06d9d8b1deccah
                         DQ 3ff0738f4b6cc8e2h
                         DQ 3ff07989af9f9f59h
                         DQ 3ff07c8a2611201ch
                         DQ 3ff08291a9958f03h
                         DQ 3ff088a208c3fe28h
                         DQ 3ff08bad91dd7d8bh
                         DQ 3ff091cb6588465eh
                         DQ 3ff097f24eab04a1h
                         DQ 3ff09b0932aee3f2h
                         DQ 3ff0a13de8970de4h
                         DQ 3ff0a45bc08a5ac7h
                         DQ 3ff0aa9e79bfa986h
                         DQ 3ff0b0eaa961ca5bh
                         DQ 3ff0b4145573271ch
                         DQ 3ff0ba6ee5f9aad4h
                         DQ 3ff0bd9fd0dbe02dh
                         DQ 3ff0c408fc1cfd4bh
                         DQ 3ff0c741430e2059h
                         DQ 3ff0cdb9442ea813h
                         DQ 3ff0d0f905168e6ch
                         DQ 3ff0d7801893d261h
                         DQ 3ff0dac772091bdeh
                         DQ 3ff0e15dd5c330abh
                         DQ 3ff0e4ace71080a4h
                         DQ 3ff0e7fe920f3037h
                         DQ 3ff0eea9c37e497eh
                         DQ 3ff0f203512f4314h
                         DQ 3ff0f8be68db7f32h
                         DQ 3ff0fc1ffa42d902h
                         DQ 3ff102eb3af9ed89h
                         DQ 3ff10654f1e29cfbh
                         DQ 3ff109c1679c189fh
                         DQ 3ff110a29f080b3dh
                         DQ 3ff114176891738ah
                         DQ 3ff1178f0099b429h
                         DQ 3ff11e86ac2cd7abh
                         DQ 3ff12206c7cf4046h
                         DQ 3ff12589c21fb842h
                         DQ 3ff12c986355d0d2h
                         DQ 3ff13024129645cfh
                         DQ 3ff133b2b13aa0ebh
                         DQ 3ff13ad8cdc48ba3h
                         DQ 3ff13e70544b1d4fh
                         DQ 3ff1420adb77c99ah
                         DQ 3ff145a867b1bfeah
                         DQ 3ff14ceca1189d6dh
                         DQ 3ff15093574284e9h
                         DQ 3ff1543d2473ea9bh
                         DQ 3ff157ea0d433a46h
                         DQ 3ff15f4d44462724h
                         DQ 3ff163039bd7cde6h
                         DQ 3ff166bd21c3a8e2h
                         DQ 3ff16a79dad1fb59h
                         DQ 3ff171fcf9aaac3dh
                         DQ 3ff175c3693980c3h
                         DQ 3ff1798d1f73f3efh
                         DQ 3ff17d5a2156e97fh
                         DQ 3ff1812a73ea2593h
                         DQ 3ff184fe1c406b8fh
                         DQ 3ff18caf82b8dba4h
                         DQ 3ff1908d4b38a510h
                         DQ 3ff1946e7e36f7e5h
                         DQ 3ff1985320ff72a2h
                         DQ 3ff19c3b38e975a8h
                         DQ 3ff1a026cb58453dh
                         DQ 3ff1a415ddbb2c10h
                         DQ 3ff1a808758d9e32h
                         DQ 3ff1aff84bac98eah
                         DQ 3ff1b3f5952e1a50h
                         DQ 3ff1b7f67a896220h
                         DQ 3ff1bbfb0178d186h
                         DQ 3ff1c0032fc3cf91h
                         DQ 3ff1c40f0b3eefc4h
                         DQ 3ff1c81e99cc193fh
                         DQ 3ff1cc31e15aae72h
                         DQ 3ff1d048e7e7b565h
                         DQ 3ff1d463b37e0090h
                         DQ 3ff1d8824a365852h
                         DQ 3ff1dca4b237a4f7h
                         DQ 3ff1e0caf1b71965h
                         DQ 3ff1e4f50ef85e61h
                         DQ 3ff1e923104dbe76h
                         DQ 3ff1ed54fc185286h
                         DQ 3ff1f18ad8c82efch
                         DQ 3ff1f5c4acdc91aah
                         DQ 3ff1fa027ee4105bh
                         DQ 3ff1fe44557cc808h
                         DQ 3ff2028a37548ccfh
                         DQ 3ff206d42b291a95h
                         DQ 3ff20b2237c8466ah
                         DQ 3ff20f74641030a6h
                         DQ 3ff213cab6ef77c7h
                         DQ 3ff2182537656c13h
                         DQ 3ff21c83ec824406h
                         DQ 3ff220e6dd675180h
                         DQ 3ff2254e114737d2h
                         DQ 3ff229b98f66228ch
                         DQ 3ff22e295f19fd31h
                         DQ 3ff2329d87caabb6h
                         DQ 3ff2371610f243f2h
                         DQ 3ff23b93021d47dah
                         DQ 3ff2401462eae0b8h
                         DQ 3ff2449a3b0d1b3fh
                         DQ 3ff2449a3b0d1b3fh
                         DQ 3ff2492492492492h
                         DQ 3ff24db370778844h
                         DQ 3ff25246dd846f45h
                         DQ 3ff256dee16fdfd4h
                         DQ 3ff25b7b844dfe71h
                         DQ 3ff2601cce474fd2h
                         DQ 3ff264c2c798fbe5h
                         DQ 3ff2696d789511e2h
                         DQ 3ff2696d789511e2h
                         DQ 3ff26e1ce9a2cd73h
                         DQ 3ff272d1233edcf3h
                         DQ 3ff2778a2dfba8d0h
                         DQ 3ff27c4812819c13h
                         DQ 3ff2810ad98f6e10h
                         DQ 3ff285d28bfa6d45h
                         DQ 3ff285d28bfa6d45h
                         DQ 3ff28a9f32aecb79h
                         DQ 3ff28f70d6afeb08h
                         DQ 3ff294478118ad83h
                         DQ 3ff299233b1bc38ah
                         DQ 3ff299233b1bc38ah
                         DQ 3ff29e040e03fdfbh
                         DQ 3ff2a2ea0334a07bh
                         DQ 3ff2a7d52429b556h
                         DQ 3ff2acc57a7862c2h
                         DQ 3ff2acc57a7862c2h
                         DQ 3ff2b1bb0fcf4190h
                         DQ 3ff2b6b5edf6b54ah
                         DQ 3ff2bbb61ed145cfh
                         DQ 3ff2c0bbac5bfa6eh
                         DQ 3ff2c0bbac5bfa6eh
                         DQ 3ff2c5c6a0aeb681h
                         DQ 3ff2cad705fc97a6h
                         DQ 3ff2cfece6945583h
                         DQ 3ff2cfece6945583h
                         DQ 3ff2d5084ce0a331h
                         DQ 3ff2da294368924fh
                         DQ 3ff2df4fd4cff7c3h
                         DQ 3ff2df4fd4cff7c3h
                         DQ 3ff2e47c0bd7d237h
                         DQ 3ff2e9adf35eb25ah
                         DQ 3ff2eee5966124e8h
                         DQ 3ff2eee5966124e8h
                         DQ 3ff2f422fffa1e92h
                         DQ 3ff2f9663b6369b6h
                         DQ 3ff2feaf53f61612h
                         DQ 3ff2feaf53f61612h
                         DQ 3ff303fe552aea57h
                         DQ 3ff309534a9ad7ceh
                         DQ 3ff309534a9ad7ceh
                         DQ 3ff30eae3fff6ff3h
                         DQ 3ff3140f41335c2fh
                         DQ 3ff3140f41335c2fh
                         DQ 3ff319765a32d7aeh
                         DQ 3ff31ee3971c2b5bh
                         DQ 3ff3245704302c13h
                         DQ 3ff3245704302c13h
                         DQ 3ff329d0add2bb20h
                         DQ 3ff32f50a08b48f9h
                         DQ 3ff32f50a08b48f9h
                         DQ 3ff334d6e9055a5fh
                         DQ 3ff33a6394110fe6h
                         DQ 3ff33a6394110fe6h
                         DQ 3ff33ff6aea3afedh
                         DQ 3ff3459045d8331bh
                         DQ 3ff3459045d8331bh
                         DQ 3ff34b3066efd36bh
                         DQ 3ff350d71f529dd8h
                         DQ 3ff350d71f529dd8h
                         DQ 3ff356847c9006b4h
                         DQ 3ff35c388c5f80bfh
                         DQ 3ff35c388c5f80bfh
                         DQ 3ff361f35ca116ffh
                         DQ 3ff361f35ca116ffh
                         DQ 3ff367b4fb5e0985h
                         DQ 3ff36d7d76c96d0ah
                         DQ 3ff36d7d76c96d0ah
                         DQ 3ff3734cdd40cd95h
                         DQ 3ff379233d4cd42ah
                         DQ 3ff379233d4cd42ah
                         DQ 3ff37f00a5a1ef96h
                         DQ 3ff37f00a5a1ef96h
                         DQ 3ff384e52521006ch
                         DQ 3ff38ad0cad80848h
                         DQ 3ff38ad0cad80848h
                         DQ 3ff390c3a602dc60h
                         DQ 3ff390c3a602dc60h
                         DQ 3ff396bdc60bdb88h
                         DQ 3ff39cbf3a8ca7a9h
                         DQ 3ff39cbf3a8ca7a9h
                         DQ 3ff3a2c8134ee2d1h
                         DQ 3ff3a2c8134ee2d1h
                         DQ 3ff3a8d8604cefe3h
                         DQ 3ff3aef031b2b706h
                         DQ 3ff3aef031b2b706h
                         DQ 3ff3b50f97de6de5h
                         DQ 3ff3b50f97de6de5h
                         DQ 3ff3bb36a36163d8h
                         DQ 3ff3bb36a36163d8h
                         DQ 3ff3c1656500d20ah
                         DQ 3ff3c79bedb6afb8h
                         DQ 3ff3c79bedb6afb8h
                         DQ 3ff3cdda4eb28aa2h
                         DQ 3ff3cdda4eb28aa2h
                         DQ 3ff3d420995a63c0h
                         DQ 3ff3d420995a63c0h
                         DQ 3ff3da6edf4b9061h
                         DQ 3ff3da6edf4b9061h
                         DQ 3ff3e0c5325b9fc2h
                         DQ 3ff3e723a499453fh
                         DQ 3ff3e723a499453fh
                         DQ 3ff3ed8a484d473ah
                         DQ 3ff3ed8a484d473ah
                         DQ 3ff3f3f92ffb72d8h
                         DQ 3ff3f3f92ffb72d8h
                         DQ 3ff3fa706e6394a4h
                         DQ 3ff3fa706e6394a4h
                         DQ 3ff400f01682764ah
                         DQ 3ff400f01682764ah
                         DQ 3ff407783b92e17ah
                         DQ 3ff407783b92e17ah
                         DQ 3ff40e08f10ea81ah
                         DQ 3ff40e08f10ea81ah
                         DQ 3ff414a24aafb1e6h
                         DQ 3ff414a24aafb1e6h
                         DQ 3ff41b445c710fa7h
                         DQ 3ff41b445c710fa7h
                         DQ 3ff421ef3a901411h
                         DQ 3ff421ef3a901411h
                         DQ 3ff428a2f98d728bh


L__definedCubeRootTable_TEMPDQ DQ 3fe428a2f98d728bh   ; 0.629961
              DQ 3fe965fea53d6e3dh   ; 0.79370
L__definedCubeRootTable_ DQ 3ff0000000000000h   ; 1.0
              DQ 3ff428a2f98d728bh   ; 1.25992
              DQ 3ff965fea53d6e3dh   ; 1.5874


CONST    ENDS	   
data ENDS		   
				   
END		
