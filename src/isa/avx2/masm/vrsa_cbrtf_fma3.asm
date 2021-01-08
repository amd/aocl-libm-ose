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
; vrsa_cbrtf.s
;
; A vector implementation of the cbrtf libm function.
;  This routine implemented in single precision.  It is slightly
;  less accurate than the double precision version, but it will
;  be better for vectorizing.
;
; Prototype:
;
;     __m128 vrsa_cbrtf(__m128 x);
;
;   Computes the floating point single precision cube root of x.
;   Returns proper C99 values, but may not raise status flags properly.
;
;

include fm.inc

FN_PROTOTYPE_FMA3 vrsa_cbrtf

LOAD_INDEXED MACRO address, ind_reg, reg_val, scratch_pad
    	lea r11,address

    	VMOVD eax,ind_reg
    	CDQE
    	VMOVLPD reg_val, reg_val, QWORD PTR [r11 + rax * 8]

    	VSHUFPS	ind_reg,ind_reg,ind_reg,225

        VMOVD eax,ind_reg
        CDQE
    	VMOVHPD reg_val, reg_val,QWORD PTR [r11 + rax * 8]

		VSHUFPS	ind_reg,ind_reg,ind_reg,30
    	VMOVD  eax,ind_reg
    	CDQE
    	VMOVLPD scratch_pad, scratch_pad,QWORD PTR [r11 + rax * 8]

		VSHUFPS	ind_reg,ind_reg,ind_reg,225
        VMOVD eax,ind_reg
        CDQE
    	VMOVHPD  scratch_pad, scratch_pad,QWORD PTR [r11 + rax * 8]

    	VSHUFPS	ind_reg,ind_reg,ind_reg,30
ENDM 

save_rdi	    equ		00h
save_rsi	    equ		10h
save_xmm6       EQU     020h
save_xmm7       EQU     040h
save_xmm8       EQU     060h
save_xmm9       EQU     080h
save_xmm10      EQU     0A0h
save_xmm11      EQU     0C0h
save_xmm12      EQU     0E0h
save_xmm13      EQU     0100h
save_xmm14      EQU     0120h
save_xmm15      EQU     0140h

stack_size      EQU     0168h 		; We take 8 as the last nibble to allow for 
                             		; alligned data movement.



text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi    
    SaveAllXMMRegs
    .ENDPROLOG


    
 mov rdi,rcx
 mov rsi, rdx
 mov rdx, r8
 
 cmp edi,00h 
 jle L__return
 cmp rsi,00h
 je L__return
 cmp rdx,00h
 je L__return
 
ALIGN 16 
L__process_next4:
 sub rdi,04h
 cmp rdi,-1
 jle L__process_first123
 movdqu xmm0,XMMWORD PTR [rsi + rdi * 4]
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

   


	VMOVAPS xmm9,xmm0									;store the input for later use

	VCMPNEQPS xmm11,xmm0,xmm0   						;nan mask xmm11
	VPXOR xmm6,xmm6,xmm6
	VANDPS xmm2,xmm0,XMMWORD PTR L__sign_mask		 		;clear the sign
	VCMPEQPS xmm8,xmm2,xmm6        					;zero mask xmm8
	VPCMPEQD xmm4,xmm2,XMMWORD PTR L__inf_mask    			;xmm4 has inf mask
	VANDPS xmm3,xmm11,XMMWORD PTR L__qnan_mask 			;xmm3 is nan mask value

	VORPS xmm4,xmm4,xmm8    							;
	VORPS xmm11,xmm4,xmm11    						;xmm11 has nan zero and inf mask
	;VPCMOV xmm12,xmm0,xmm11,xmm11					;xmm12 has nan zero and inf values
	;VPCMOV xmm12,xmm3,xmm12,xmm3					;convert nan to qnan
	
	VANDPD    xmm12  , xmm0,     xmm11
	VANDNPD   xmm1   , xmm11,    xmm11
	VORPD     xmm12  , xmm12,    xmm1 
	VANDNPD   xmm1   , xmm3,     xmm12
	VANDPD    xmm12  , xmm3,     xmm3 
	VORPD     xmm12  , xmm12,    xmm1 
	
	
	
	VANDNPS xmm0,xmm11,xmm0							;clear the input value in case of invalid input
	VANDPS xmm1,xmm0,XMMWORD PTR L__double_exp3_mask
	VPSRLD xmm1,xmm1,23				       				;xmm1 is exp
    VMULPS xmm4,xmm0,XMMWORD PTR L__2_pow_23  					;xmm4 denormal f
	VANDPS xmm2,xmm4,XMMWORD PTR L__double_exp3_mask
	VPSRLD xmm2,xmm2,23				       				;xmm2 is exp
	VPCMPEQD xmm6,xmm1,xmm6								;xmm6   exp zero mask
	;VPCMOV xmm1,xmm2,xmm1,xmm6
	;VPCMOV xmm0,xmm4,xmm0,xmm6							;xmm0 is f xmm1 is exp
	
	VANDNPD  xmm10  ,  xmm6,  xmm1 
	VANDPD   xmm1   ,  xmm2,  xmm6 
	VORPD    xmm1   ,  xmm1,  xmm10
	VANDNPD  xmm10  ,  xmm6,  xmm0 
	VANDPD   xmm0   ,  xmm4,  xmm6 
	VORPD    xmm0   ,  xmm0,  xmm10
	
	
	
	
	VPSUBD xmm1,xmm1,XMMWORD PTR L__float_exp_bias 			;xmm1 has exp.
	VCVTDQ2PS xmm3,xmm1
	VMULPS xmm2,xmm3,XMMWORD PTR L__float_oneby3
	VCVTTPS2DQ xmm2,xmm2									;xmm2 scale
	VPADDD xmm4,xmm2,xmm2
	VPADDD xmm4,xmm4,xmm2
	VPSUBD xmm10,xmm1,xmm4								; xmm10 is remainder
	VPADDD xmm2,xmm2,XMMWORD PTR L__double_exp_bias
	VPUNPCKHQDQ xmm3,xmm2,xmm2
	VPMOVSXDQ xmm2,xmm2
	VPMOVSXDQ xmm3,xmm3
 	VPSLLQ xmm2,xmm2,52
 	VPSLLQ xmm3,xmm3,52
 	VINSERTF128 ymm2,ymm2,xmm3,1							;ymm2 is scale
	VANDPS xmm0,xmm0,XMMWORD PTR L__double_exp1_mask
	VORPS xmm0,xmm0,XMMWORD PTR L__double_exp2_mask			;xmm0 is f
	VANDPS xmm3,xmm0,XMMWORD PTR L__double_exp4_mask
 	VPSRLD xmm3,xmm3,15									;xmm3 is index
	LOAD_INDEXED L__DoubleReciprocalTable_256,xmm3,xmm5,xmm8    ; xmm5 xmm8 is reciprocal
	VINSERTF128 ymm5,ymm5,xmm8,1
	VCVTPS2PD ymm0,xmm0									; f to double
	VMOVAPD ymm4,L__double_n_real_one
	;VFMADDPD ymm4,ymm0,ymm5,ymm4   						; r  ymm4
	VFMADD231PD ymm4  ,ymm0,ymm5
	
	VMULPD ymm5,ymm4,ymm4								; r^2 ymm5
	VMOVAPD ymm7,L__double_real_one
	;VFMADDPD ymm7,ymm4,L__coefficient_1,ymm7
	;VFMADDPD ymm7,ymm5,L__coefficient_2,ymm7	    ;ymm7 is oneplusrcuberoot
	
	VFMADD231PD ymm7  , ymm4,  L__coefficient_1
	VFMADD231PD ymm7, ymm5,	   L__coefficient_2
	
	VANDNPS xmm10,xmm11,xmm10							;clear the input value in case of invalid input
	LOAD_INDEXED L__definedCubeRootTable_,xmm10,xmm5,xmm8    ; xmm5 scale constant
	VINSERTF128 ymm5,ymm5,xmm8,1
	VMULPD ymm2,ymm2,ymm5										;ymm2  scale * constant
	LOAD_INDEXED L__DoubleCubeRootTable_256,xmm3,xmm5,xmm8    ; xmm5 is CubeRootTable_
	VINSERTF128 ymm5,ymm5,xmm8,1
	VMULPD ymm2,ymm2,ymm5			 							 ;
	VMULPD ymm2,ymm2,ymm7									 ;resultcuberoot ymm2
	VMULPD ymm3,ymm2,L__203
	VCVTPD2PS xmm2,ymm2
	VCVTPD2PS xmm3,ymm3
	VANDPS xmm9,xmm9,XMMWORD PTR L__real_sign
	;VPCMOV xmm0,xmm3,xmm2,xmm6
	VANDPD    xmm0    ,  xmm3,   xmm6
	VANDNPD   xmm1    ,  xmm6,   xmm2
	VORPD     xmm0    ,  xmm0,   xmm1
	
	VXORPS xmm0,xmm0,xmm9
	;VPCMOV xmm0,xmm12,xmm0,xmm11						; handling of NAN and inf
	VANDNPD   xmm0 ,  xmm11,   xmm0 
	VANDPD    xmm1 ,  xmm12,   xmm11
	VORPD    xmm0,  xmm0, 	   xmm1 


 cmp rdi,-1
 jle L__store123
 movdqu XMMWORD PTR[rdx + rdi * 4],xmm0
 jmp L__process_next4 
 
ALIGN 16 
L__store123:
 je L__store3
 cmp rdi,-3 
 je L__store1 
 ;store 2 
 add rdi,02h
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return

ALIGN 16 
L__store3:
 movdqa xmm1,xmm0
 psrldq xmm0,4
 inc rdi
 movss DWORD PTR[rdx + rdi * 4],xmm1
 inc rdi
 movsd QWORD PTR[rdx + rdi * 4],xmm0 
 jmp L__return
 
ALIGN 16 
L__store1:
 mov rdi,00h
 movss DWORD PTR[rdx + rdi * 4],xmm0 
 
L__return:
 RestoreAllXMMRegs 
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 


  ret


fname        endp
TEXT    ENDS   

data SEGMENT READONLY PAGE 


 

ALIGN 32

L__203				DQ 03F7428A2F98D728Bh
						DQ 03F7428A2F98D728Bh
						DQ 03F7428A2F98D728Bh
						DQ 03F7428A2F98D728Bh

L__coefficient_1      DQ 03fd5555555555555h ; (1/3)
                        DQ 03fd5555555555555h
						DQ 03fd5555555555555h ; (1/3)
                        DQ 03fd5555555555555h

L__coefficient_2      DQ 0bFBC71C71C71C71Ch ;  (1/9)
                        DQ 0bFBC71C71C71C71Ch
						DQ 0bFBC71C71C71C71Ch ;  (1/9)
                        DQ 0bFBC71C71C71C71Ch

L__double_real_one    DQ 03ff0000000000000h ; 1.0
                        DQ 03ff0000000000000h
						DQ 03ff0000000000000h ; 1.0
                        DQ 03ff0000000000000h

L__double_n_real_one  DQ 0bff0000000000000h ; 1.0
                        DQ 0bff0000000000000h
                        DQ 0bff0000000000000h ; 1.0
                        DQ 0bff0000000000000h

ALIGN 16
L__inf_mask			DQ 07F8000007F800000h
                        DQ 07F8000007F800000h

L__qnan_mask			DQ 00040000000400000h
                        DQ 00040000000400000h

L__sign_mask          DQ 07FFFFFFF7FFFFFFFh
                        DQ 07FFFFFFF7FFFFFFFh

L__float_exp_bias     DQ 00000007F0000007Fh
                        DQ 00000007F0000007Fh

L__double_exp_bias    DQ 0000003FF000003FFh
                        DQ 0000003FF000003FFh

L__double_exp1_mask   DQ 03FFFFFFF3FFFFFFFh
                        DQ 03FFFFFFF3FFFFFFFh


L__double_exp2_mask   DQ 03F8000003F800000h
                        DQ 03F8000003F800000h

L__double_exp3_mask    DQ 07F8000007F800000h
                         DQ 07F8000007F800000h

L__double_exp4_mask    DQ 0007F8000007F8000h
                         DQ 0007F8000007F8000h

L__real_sign          DQ 08000000080000000h    ; sign bit
                        DQ 08000000080000000h

L__float_oneby3       DQ 03EAAAAAB3EAAAAABh     ; (1/3)
                        DQ 03EAAAAAB3EAAAAABh

L__2_pow_23 		    DQ 04B0000004B000000h
						DQ 04B0000004B000000h




ALIGN 16
L__DoubleReciprocalTable_256 DQ 03ff0000000000000h
            DQ 03fefe00000000000h
            DQ 03fefc00000000000h
            DQ 03fefa00000000000h
            DQ 03fef800000000000h
            DQ 03fef600000000000h
            DQ 03fef400000000000h
            DQ 03fef200000000000h
            DQ 03fef000000000000h
            DQ 03feee00000000000h
            DQ 03feec00000000000h
            DQ 03feea00000000000h
            DQ 03fee900000000000h
            DQ 03fee700000000000h
            DQ 03fee500000000000h
            DQ 03fee300000000000h
            DQ 03fee100000000000h
            DQ 03fee000000000000h
            DQ 03fede00000000000h
            DQ 03fedc00000000000h
            DQ 03feda00000000000h
            DQ 03fed900000000000h
            DQ 03fed700000000000h
            DQ 03fed500000000000h
            DQ 03fed400000000000h
            DQ 03fed200000000000h
            DQ 03fed000000000000h
            DQ 03fecf00000000000h
            DQ 03fecd00000000000h
            DQ 03fecb00000000000h
            DQ 03feca00000000000h
            DQ 03fec800000000000h
            DQ 03fec700000000000h
            DQ 03fec500000000000h
            DQ 03fec300000000000h
            DQ 03fec200000000000h
            DQ 03fec000000000000h
            DQ 03febf00000000000h
            DQ 03febd00000000000h
            DQ 03febc00000000000h
            DQ 03feba00000000000h
            DQ 03feb900000000000h
            DQ 03feb700000000000h
            DQ 03feb600000000000h
            DQ 03feb400000000000h
            DQ 03feb300000000000h
            DQ 03feb200000000000h
            DQ 03feb000000000000h
            DQ 03feaf00000000000h
            DQ 03fead00000000000h
            DQ 03feac00000000000h
            DQ 03feaa00000000000h
            DQ 03fea900000000000h
            DQ 03fea800000000000h
            DQ 03fea600000000000h
            DQ 03fea500000000000h
            DQ 03fea400000000000h
            DQ 03fea200000000000h
            DQ 03fea100000000000h
            DQ 03fea000000000000h
            DQ 03fe9e00000000000h
            DQ 03fe9d00000000000h
            DQ 03fe9c00000000000h
            DQ 03fe9a00000000000h
            DQ 03fe9900000000000h
            DQ 03fe9800000000000h
            DQ 03fe9700000000000h
            DQ 03fe9500000000000h
            DQ 03fe9400000000000h
            DQ 03fe9300000000000h
            DQ 03fe9200000000000h
            DQ 03fe9000000000000h
            DQ 03fe8f00000000000h
            DQ 03fe8e00000000000h
            DQ 03fe8d00000000000h
            DQ 03fe8b00000000000h
            DQ 03fe8a00000000000h
            DQ 03fe8900000000000h
            DQ 03fe8800000000000h
            DQ 03fe8700000000000h
            DQ 03fe8600000000000h
            DQ 03fe8400000000000h
            DQ 03fe8300000000000h
            DQ 03fe8200000000000h
            DQ 03fe8100000000000h
            DQ 03fe8000000000000h
            DQ 03fe7f00000000000h
            DQ 03fe7e00000000000h
            DQ 03fe7d00000000000h
            DQ 03fe7b00000000000h
            DQ 03fe7a00000000000h
            DQ 03fe7900000000000h
            DQ 03fe7800000000000h
            DQ 03fe7700000000000h
            DQ 03fe7600000000000h
            DQ 03fe7500000000000h
            DQ 03fe7400000000000h
            DQ 03fe7300000000000h
            DQ 03fe7200000000000h
            DQ 03fe7100000000000h
            DQ 03fe7000000000000h
            DQ 03fe6f00000000000h
            DQ 03fe6e00000000000h
            DQ 03fe6d00000000000h
            DQ 03fe6c00000000000h
            DQ 03fe6b00000000000h
            DQ 03fe6a00000000000h
            DQ 03fe6900000000000h
            DQ 03fe6800000000000h
            DQ 03fe6700000000000h
            DQ 03fe6600000000000h
            DQ 03fe6500000000000h
            DQ 03fe6400000000000h
            DQ 03fe6300000000000h
            DQ 03fe6200000000000h
            DQ 03fe6100000000000h
            DQ 03fe6000000000000h
            DQ 03fe5f00000000000h
            DQ 03fe5e00000000000h
            DQ 03fe5d00000000000h
            DQ 03fe5c00000000000h
            DQ 03fe5b00000000000h
            DQ 03fe5a00000000000h
            DQ 03fe5900000000000h
            DQ 03fe5800000000000h
            DQ 03fe5800000000000h
            DQ 03fe5700000000000h
            DQ 03fe5600000000000h
            DQ 03fe5500000000000h
            DQ 03fe5400000000000h
            DQ 03fe5300000000000h
            DQ 03fe5200000000000h
            DQ 03fe5100000000000h
            DQ 03fe5000000000000h
            DQ 03fe5000000000000h
            DQ 03fe4f00000000000h
            DQ 03fe4e00000000000h
            DQ 03fe4d00000000000h
            DQ 03fe4c00000000000h
            DQ 03fe4b00000000000h
            DQ 03fe4a00000000000h
            DQ 03fe4a00000000000h
            DQ 03fe4900000000000h
            DQ 03fe4800000000000h
            DQ 03fe4700000000000h
            DQ 03fe4600000000000h
            DQ 03fe4600000000000h
            DQ 03fe4500000000000h
            DQ 03fe4400000000000h
            DQ 03fe4300000000000h
            DQ 03fe4200000000000h
            DQ 03fe4200000000000h
            DQ 03fe4100000000000h
            DQ 03fe4000000000000h
            DQ 03fe3f00000000000h
            DQ 03fe3e00000000000h
            DQ 03fe3e00000000000h
            DQ 03fe3d00000000000h
            DQ 03fe3c00000000000h
            DQ 03fe3b00000000000h
            DQ 03fe3b00000000000h
            DQ 03fe3a00000000000h
            DQ 03fe3900000000000h
            DQ 03fe3800000000000h
            DQ 03fe3800000000000h
            DQ 03fe3700000000000h
            DQ 03fe3600000000000h
            DQ 03fe3500000000000h
            DQ 03fe3500000000000h
            DQ 03fe3400000000000h
            DQ 03fe3300000000000h
            DQ 03fe3200000000000h
            DQ 03fe3200000000000h
            DQ 03fe3100000000000h
            DQ 03fe3000000000000h
            DQ 03fe3000000000000h
            DQ 03fe2f00000000000h
            DQ 03fe2e00000000000h
            DQ 03fe2e00000000000h
            DQ 03fe2d00000000000h
            DQ 03fe2c00000000000h
            DQ 03fe2b00000000000h
            DQ 03fe2b00000000000h
            DQ 03fe2a00000000000h
            DQ 03fe2900000000000h
            DQ 03fe2900000000000h
            DQ 03fe2800000000000h
            DQ 03fe2700000000000h
            DQ 03fe2700000000000h
            DQ 03fe2600000000000h
            DQ 03fe2500000000000h
            DQ 03fe2500000000000h
            DQ 03fe2400000000000h
            DQ 03fe2300000000000h
            DQ 03fe2300000000000h
            DQ 03fe2200000000000h
            DQ 03fe2100000000000h
            DQ 03fe2100000000000h
            DQ 03fe2000000000000h
            DQ 03fe2000000000000h
            DQ 03fe1f00000000000h
            DQ 03fe1e00000000000h
            DQ 03fe1e00000000000h
            DQ 03fe1d00000000000h
            DQ 03fe1c00000000000h
            DQ 03fe1c00000000000h
            DQ 03fe1b00000000000h
            DQ 03fe1b00000000000h
            DQ 03fe1a00000000000h
            DQ 03fe1900000000000h
            DQ 03fe1900000000000h
            DQ 03fe1800000000000h
            DQ 03fe1800000000000h
            DQ 03fe1700000000000h
            DQ 03fe1600000000000h
            DQ 03fe1600000000000h
            DQ 03fe1500000000000h
            DQ 03fe1500000000000h
            DQ 03fe1400000000000h
            DQ 03fe1300000000000h
            DQ 03fe1300000000000h
            DQ 03fe1200000000000h
            DQ 03fe1200000000000h
            DQ 03fe1100000000000h
            DQ 03fe1100000000000h
            DQ 03fe1000000000000h
            DQ 03fe0f00000000000h
            DQ 03fe0f00000000000h
            DQ 03fe0e00000000000h
            DQ 03fe0e00000000000h
            DQ 03fe0d00000000000h
            DQ 03fe0d00000000000h
            DQ 03fe0c00000000000h
            DQ 03fe0c00000000000h
            DQ 03fe0b00000000000h
            DQ 03fe0a00000000000h
            DQ 03fe0a00000000000h
            DQ 03fe0900000000000h
            DQ 03fe0900000000000h
            DQ 03fe0800000000000h
            DQ 03fe0800000000000h
            DQ 03fe0700000000000h
            DQ 03fe0700000000000h
            DQ 03fe0600000000000h
            DQ 03fe0600000000000h
            DQ 03fe0500000000000h
            DQ 03fe0500000000000h
            DQ 03fe0400000000000h
            DQ 03fe0400000000000h
            DQ 03fe0300000000000h
            DQ 03fe0300000000000h
            DQ 03fe0200000000000h
            DQ 03fe0200000000000h
            DQ 03fe0100000000000h
            DQ 03fe0100000000000h
            DQ 03fe0000000000000h

ALIGN 16
L__DoubleCubeRootTable_256   DQ 03ff0000000000000h
                         DQ 03ff00558e6547c36h
                         DQ 03ff00ab8f9d2f374h
                         DQ 03ff010204b673fc7h
                         DQ 03ff0158eec36749bh
                         DQ 03ff01b04ed9fdb53h
                         DQ 03ff02082613df53ch
                         DQ 03ff0260758e78308h
                         DQ 03ff02b93e6b091f0h
                         DQ 03ff031281ceb8ea2h
                         DQ 03ff036c40e2a5e2ah
                         DQ 03ff03c67cd3f7ceah
                         DQ 03ff03f3c9fee224ch
                         DQ 03ff044ec379f7f79h
                         DQ 03ff04aa3cd578d67h
                         DQ 03ff0506374d40a3dh
                         DQ 03ff0562b4218a6e3h
                         DQ 03ff059123d3a9848h
                         DQ 03ff05ee6694e7166h
                         DQ 03ff064c2ee6e07c6h
                         DQ 03ff06aa7e19c01c5h
                         DQ 03ff06d9d8b1deccah
                         DQ 03ff0738f4b6cc8e2h
                         DQ 03ff07989af9f9f59h
                         DQ 03ff07c8a2611201ch
                         DQ 03ff08291a9958f03h
                         DQ 03ff088a208c3fe28h
                         DQ 03ff08bad91dd7d8bh
                         DQ 03ff091cb6588465eh
                         DQ 03ff097f24eab04a1h
                         DQ 03ff09b0932aee3f2h
                         DQ 03ff0a13de8970de4h
                         DQ 03ff0a45bc08a5ac7h
                         DQ 03ff0aa9e79bfa986h
                         DQ 03ff0b0eaa961ca5bh
                         DQ 03ff0b4145573271ch
                         DQ 03ff0ba6ee5f9aad4h
                         DQ 03ff0bd9fd0dbe02dh
                         DQ 03ff0c408fc1cfd4bh
                         DQ 03ff0c741430e2059h
                         DQ 03ff0cdb9442ea813h
                         DQ 03ff0d0f905168e6ch
                         DQ 03ff0d7801893d261h
                         DQ 03ff0dac772091bdeh
                         DQ 03ff0e15dd5c330abh
                         DQ 03ff0e4ace71080a4h
                         DQ 03ff0e7fe920f3037h
                         DQ 03ff0eea9c37e497eh
                         DQ 03ff0f203512f4314h
                         DQ 03ff0f8be68db7f32h
                         DQ 03ff0fc1ffa42d902h
                         DQ 03ff102eb3af9ed89h
                         DQ 03ff10654f1e29cfbh
                         DQ 03ff109c1679c189fh
                         DQ 03ff110a29f080b3dh
                         DQ 03ff114176891738ah
                         DQ 03ff1178f0099b429h
                         DQ 03ff11e86ac2cd7abh
                         DQ 03ff12206c7cf4046h
                         DQ 03ff12589c21fb842h
                         DQ 03ff12c986355d0d2h
                         DQ 03ff13024129645cfh
                         DQ 03ff133b2b13aa0ebh
                         DQ 03ff13ad8cdc48ba3h
                         DQ 03ff13e70544b1d4fh
                         DQ 03ff1420adb77c99ah
                         DQ 03ff145a867b1bfeah
                         DQ 03ff14ceca1189d6dh
                         DQ 03ff15093574284e9h
                         DQ 03ff1543d2473ea9bh
                         DQ 03ff157ea0d433a46h
                         DQ 03ff15f4d44462724h
                         DQ 03ff163039bd7cde6h
                         DQ 03ff166bd21c3a8e2h
                         DQ 03ff16a79dad1fb59h
                         DQ 03ff171fcf9aaac3dh
                         DQ 03ff175c3693980c3h
                         DQ 03ff1798d1f73f3efh
                         DQ 03ff17d5a2156e97fh
                         DQ 03ff1812a73ea2593h
                         DQ 03ff184fe1c406b8fh
                         DQ 03ff18caf82b8dba4h
                         DQ 03ff1908d4b38a510h
                         DQ 03ff1946e7e36f7e5h
                         DQ 03ff1985320ff72a2h
                         DQ 03ff19c3b38e975a8h
                         DQ 03ff1a026cb58453dh
                         DQ 03ff1a415ddbb2c10h
                         DQ 03ff1a808758d9e32h
                         DQ 03ff1aff84bac98eah
                         DQ 03ff1b3f5952e1a50h
                         DQ 03ff1b7f67a896220h
                         DQ 03ff1bbfb0178d186h
                         DQ 03ff1c0032fc3cf91h
                         DQ 03ff1c40f0b3eefc4h
                         DQ 03ff1c81e99cc193fh
                         DQ 03ff1cc31e15aae72h
                         DQ 03ff1d048e7e7b565h
                         DQ 03ff1d463b37e0090h
                         DQ 03ff1d8824a365852h
                         DQ 03ff1dca4b237a4f7h
                         DQ 03ff1e0caf1b71965h
                         DQ 03ff1e4f50ef85e61h
                         DQ 03ff1e923104dbe76h
                         DQ 03ff1ed54fc185286h
                         DQ 03ff1f18ad8c82efch
                         DQ 03ff1f5c4acdc91aah
                         DQ 03ff1fa027ee4105bh
                         DQ 03ff1fe44557cc808h
                         DQ 03ff2028a37548ccfh
                         DQ 03ff206d42b291a95h
                         DQ 03ff20b2237c8466ah
                         DQ 03ff20f74641030a6h
                         DQ 03ff213cab6ef77c7h
                         DQ 03ff2182537656c13h
                         DQ 03ff21c83ec824406h
                         DQ 03ff220e6dd675180h
                         DQ 03ff2254e114737d2h
                         DQ 03ff229b98f66228ch
                         DQ 03ff22e295f19fd31h
                         DQ 03ff2329d87caabb6h
                         DQ 03ff2371610f243f2h
                         DQ 03ff23b93021d47dah
                         DQ 03ff2401462eae0b8h
                         DQ 03ff2449a3b0d1b3fh
                         DQ 03ff2449a3b0d1b3fh
                         DQ 03ff2492492492492h
                         DQ 03ff24db370778844h
                         DQ 03ff25246dd846f45h
                         DQ 03ff256dee16fdfd4h
                         DQ 03ff25b7b844dfe71h
                         DQ 03ff2601cce474fd2h
                         DQ 03ff264c2c798fbe5h
                         DQ 03ff2696d789511e2h
                         DQ 03ff2696d789511e2h
                         DQ 03ff26e1ce9a2cd73h
                         DQ 03ff272d1233edcf3h
                         DQ 03ff2778a2dfba8d0h
                         DQ 03ff27c4812819c13h
                         DQ 03ff2810ad98f6e10h
                         DQ 03ff285d28bfa6d45h
                         DQ 03ff285d28bfa6d45h
                         DQ 03ff28a9f32aecb79h
                         DQ 03ff28f70d6afeb08h
                         DQ 03ff294478118ad83h
                         DQ 03ff299233b1bc38ah
                         DQ 03ff299233b1bc38ah
                         DQ 03ff29e040e03fdfbh
                         DQ 03ff2a2ea0334a07bh
                         DQ 03ff2a7d52429b556h
                         DQ 03ff2acc57a7862c2h
                         DQ 03ff2acc57a7862c2h
                         DQ 03ff2b1bb0fcf4190h
                         DQ 03ff2b6b5edf6b54ah
                         DQ 03ff2bbb61ed145cfh
                         DQ 03ff2c0bbac5bfa6eh
                         DQ 03ff2c0bbac5bfa6eh
                         DQ 03ff2c5c6a0aeb681h
                         DQ 03ff2cad705fc97a6h
                         DQ 03ff2cfece6945583h
                         DQ 03ff2cfece6945583h
                         DQ 03ff2d5084ce0a331h
                         DQ 03ff2da294368924fh
                         DQ 03ff2df4fd4cff7c3h
                         DQ 03ff2df4fd4cff7c3h
                         DQ 03ff2e47c0bd7d237h
                         DQ 03ff2e9adf35eb25ah
                         DQ 03ff2eee5966124e8h
                         DQ 03ff2eee5966124e8h
                         DQ 03ff2f422fffa1e92h
                         DQ 03ff2f9663b6369b6h
                         DQ 03ff2feaf53f61612h
                         DQ 03ff2feaf53f61612h
                         DQ 03ff303fe552aea57h
                         DQ 03ff309534a9ad7ceh
                         DQ 03ff309534a9ad7ceh
                         DQ 03ff30eae3fff6ff3h
                         DQ 03ff3140f41335c2fh
                         DQ 03ff3140f41335c2fh
                         DQ 03ff319765a32d7aeh
                         DQ 03ff31ee3971c2b5bh
                         DQ 03ff3245704302c13h
                         DQ 03ff3245704302c13h
                         DQ 03ff329d0add2bb20h
                         DQ 03ff32f50a08b48f9h
                         DQ 03ff32f50a08b48f9h
                         DQ 03ff334d6e9055a5fh
                         DQ 03ff33a6394110fe6h
                         DQ 03ff33a6394110fe6h
                         DQ 03ff33ff6aea3afedh
                         DQ 03ff3459045d8331bh
                         DQ 03ff3459045d8331bh
                         DQ 03ff34b3066efd36bh
                         DQ 03ff350d71f529dd8h
                         DQ 03ff350d71f529dd8h
                         DQ 03ff356847c9006b4h
                         DQ 03ff35c388c5f80bfh
                         DQ 03ff35c388c5f80bfh
                         DQ 03ff361f35ca116ffh
                         DQ 03ff361f35ca116ffh
                         DQ 03ff367b4fb5e0985h
                         DQ 03ff36d7d76c96d0ah
                         DQ 03ff36d7d76c96d0ah
                         DQ 03ff3734cdd40cd95h
                         DQ 03ff379233d4cd42ah
                         DQ 03ff379233d4cd42ah
                         DQ 03ff37f00a5a1ef96h
                         DQ 03ff37f00a5a1ef96h
                         DQ 03ff384e52521006ch
                         DQ 03ff38ad0cad80848h
                         DQ 03ff38ad0cad80848h
                         DQ 03ff390c3a602dc60h
                         DQ 03ff390c3a602dc60h
                         DQ 03ff396bdc60bdb88h
                         DQ 03ff39cbf3a8ca7a9h
                         DQ 03ff39cbf3a8ca7a9h
                         DQ 03ff3a2c8134ee2d1h
                         DQ 03ff3a2c8134ee2d1h
                         DQ 03ff3a8d8604cefe3h
                         DQ 03ff3aef031b2b706h
                         DQ 03ff3aef031b2b706h
                         DQ 03ff3b50f97de6de5h
                         DQ 03ff3b50f97de6de5h
                         DQ 03ff3bb36a36163d8h
                         DQ 03ff3bb36a36163d8h
                         DQ 03ff3c1656500d20ah
                         DQ 03ff3c79bedb6afb8h
                         DQ 03ff3c79bedb6afb8h
                         DQ 03ff3cdda4eb28aa2h
                         DQ 03ff3cdda4eb28aa2h
                         DQ 03ff3d420995a63c0h
                         DQ 03ff3d420995a63c0h
                         DQ 03ff3da6edf4b9061h
                         DQ 03ff3da6edf4b9061h
                         DQ 03ff3e0c5325b9fc2h
                         DQ 03ff3e723a499453fh
                         DQ 03ff3e723a499453fh
                         DQ 03ff3ed8a484d473ah
                         DQ 03ff3ed8a484d473ah
                         DQ 03ff3f3f92ffb72d8h
                         DQ 03ff3f3f92ffb72d8h
                         DQ 03ff3fa706e6394a4h
                         DQ 03ff3fa706e6394a4h
                         DQ 03ff400f01682764ah
                         DQ 03ff400f01682764ah
                         DQ 03ff407783b92e17ah
                         DQ 03ff407783b92e17ah
                         DQ 03ff40e08f10ea81ah
                         DQ 03ff40e08f10ea81ah
                         DQ 03ff414a24aafb1e6h
                         DQ 03ff414a24aafb1e6h
                         DQ 03ff41b445c710fa7h
                         DQ 03ff41b445c710fa7h
                         DQ 03ff421ef3a901411h
                         DQ 03ff421ef3a901411h
                         DQ 03ff428a2f98d728bh


L__definedCubeRootTable_TEMP DQ 03fe428a2f98d728bh   ; 0.629961
              DQ 03fe965fea53d6e3dh   ; 0.79370
L__definedCubeRootTable_ DQ 03ff0000000000000h   ; 1.0
              DQ 03ff428a2f98d728bh   ; 1.25992
              DQ 03ff965fea53d6e3dh   ; 1.5874


 
data ENDS		   
				   
END		
