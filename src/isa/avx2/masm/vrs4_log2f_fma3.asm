;
; Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
;

; vrs4_log2f_bdozr.S
;
; An implementation of the log2f libm function.
;
; Prototype:
;
; float log2f(float x);
;

;
; Algorithm:
; Similar to one presnted in log.S
;


include fm.inc

FN_PROTOTYPE_FMA3 vrs4_log2f

; local variable storage offsets
save_xmm6 EQU 00h
save_xmm7 EQU 020h
save_xmm8 EQU 040h
save_xmm9 EQU 060h
save_xmm10 EQU 080h
save_xmm11 EQU 0A0h
save_xmm12 EQU 0C0h
save_xmm13 EQU 0E0h
save_xmm14 EQU 0100h
save_xmm15 EQU 0120h
stack_size EQU 0148h ; We take 8 as the last nibble to allow for 
 ; alligned data movement.

 
text SEGMENT EXECUTE
ALIGN 16
PUBLIC fname
fname PROC FRAME
 StackAllocate stack_size
 SaveAllXMMRegs
 .ENDPROLOG

 movupd xmm0, XMMWORD PTR [rcx]
 


 vmovaps xmm15,DWORD PTR L__real_one
 vmovaps xmm13,DWORD PTR L__real_inf
 vmovaps xmm10,DWORD PTR L__real_nan
 ; compute exponent part
 xor eax,eax ;eax=0
 vpsrld xmm3,xmm0,23 ;xmm3 = (ux>>23)
 vpsubd xmm3,xmm3,DWORD PTR L__mask_127 ; xmm3 = (ux>>23) - 127
 vandps xmm12,xmm13,xmm0
 ;speacial case processing Nans Infs and,Negatives e's 
 ; NaN or inf
 vpcmpeqd xmm6,xmm13,xmm12 ; if((ux & 07f800000h) == 07f800000h) go to DWORD PTR L__x_is_inf_or_nan
;xmm6 stores the mask for Nans and Infs
 vaddps xmm11,xmm0,xmm0
 vpand xmm11,xmm6,xmm11 ;xmm11 stores the result for Nan's and INfs
;check for negative numbers
 vxorps xmm1,xmm1,xmm1
 vpcmpgtd xmm7,xmm1,xmm0 ; if( x <= 0.0 )
;xmm7 stores the negative mask for negative numbers
 vpor xmm6,xmm7,xmm6
 ;vpcmov xmm7 ,xmm10,xmm11,xmm7
 vandnpd xmm2	 , xmm7, xmm11
 vandpd xmm7 , xmm10,	 xmm7 
 vorpd xmm7	 , xmm7, xmm2 
 
 ; xmm6 = Mask for Nan Infs and negatives
 ; xmm7 = result of Nan Infs and negatives

 vpand xmm2,xmm0,DWORD PTR L__real_mant ; xmm2 = ux & 0007FFFFFh
 vsubps xmm4,xmm0,xmm15 ; xmm4 = x - 1.0
 vpcmpeqd xmm8,xmm3,DWORD PTR L__mask_neg_127 ; if (xexp == 127) 
;xmm8 stores the mask for denormal numbers 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; commenting this part of the code will not process the denormal inputs 
;if 1
 vpor xmm11,xmm15,xmm2
 vsubps xmm10,xmm11,xmm15
 vpand xmm11,xmm10,DWORD PTR L__real_mant
 ;vpcmov xmm2 ,xmm11,xmm2,xmm8
 
 vandnpd xmm1	 , xmm8, xmm2
 vandpd xmm2	 , xmm11, xmm8
 vorpd xmm2	 , xmm2, xmm1
 
 vpsrld xmm10,xmm10,23
 vpsubd xmm10,xmm10,DWORD PTR L__mask_253 
 ;vpcmov xmm3 ,xmm10,xmm3,xmm8
 vandnpd xmm1	 , xmm8, xmm3
 vandpd xmm3 , xmm10,	 xmm8
 vorpd xmm3 , xmm3,	 xmm1
 
;endif 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 vcvtdq2ps xmm5,xmm3 ; xmm5 = xexp
 ; compute the index into the log tables
 vpand xmm11,xmm2,DWORD PTR L__mask_mant_all7 ;xmm11 = ux & 0007f0000h 
 vpand xmm8,xmm2,DWORD PTR L__mask_mant8 ; xmm8 = ux & 0007f0000h 
 vpslld xmm8,xmm8,1 ;xmm8 = (ux & 000008000h) << 1
 vpaddd xmm1,xmm11,xmm8 ;xmm1 = (ux & 0007f0000h) + ((ux & 000008000h) << 1)

 ; near one codepath
 vpand xmm4,xmm4,DWORD PTR L__real_notsign ; xmm4 = fabs (x - 1.0)
 vcmpltps xmm4,xmm4,DWORD PTR L__real_threshold ; if (xmm4 < real_threshold) go to near_one
;xmm4 stores the mask for nearone values
 ; F,Y

 vpsrld xmm9,xmm1,16
 vpor xmm2,xmm2,DWORD PTR L__real_half ; xmm2 = Y = ((ux & 0007FFFFFh) | 03f000000h)
 vpor xmm1,xmm1,DWORD PTR L__real_half ; xmm1 = F = ((ux & 0007f0000h) + ((ux & 000008000h) << 1) | 03f000000h)
 lea r9,DWORD PTR L__log_F_inv ;
 lea r10,DWORD PTR L__log2_128_lead_tail
 xor r8,r8 ; can be removed

;1st
 vmovd r8d,xmm9
 vpsrldq xmm9,xmm9,4 
 vmovss xmm10,DWORD PTR[r9 + r8 * 4]
 vmovsd xmm11,QWORD PTR[r10 + r8 * 8]
 vpslldq xmm10,xmm10,4
;2nd 
 vmovd r8d,xmm9
 vpsrldq xmm9,xmm9,4 
 vmovss xmm13,DWORD PTR[r9 + r8 * 4]
 vmovss xmm10,xmm10,xmm13
 vmovsd xmm14,QWORD PTR[r10 + r8 * 8]
 vpslldq xmm14,xmm14,8
 vmovsd xmm11,xmm14,xmm11
 vpslldq xmm10,xmm10,4
;3rd
 vmovd r8d,xmm9
 vpsrldq xmm9,xmm9,4
 vmovss xmm13,DWORD PTR[r9 + r8 * 4]
 vmovss xmm10,xmm10,xmm13
 vmovsd xmm12,QWORD PTR[r10 + r8 * 8]
 vpslldq xmm10,xmm10,4
;4th
 vmovd r8d,xmm9
 vmovss xmm13,DWORD PTR[r9 + r8 * 4]
 vmovss xmm10,xmm10,xmm13
 vmovsd xmm14,QWORD PTR[r10 + r8 * 8]
 vpslldq xmm14,xmm14,8
 vmovsd xmm12,xmm14,xmm12
 
 vpermilps xmm10,xmm10,01Bh ; 1B = 00011011
 ; f = F - Y,r = f * inv
 vsubps xmm1,xmm1,xmm2 ; f = F - Y
 vmulps xmm1,xmm1,xmm10 ; r = f * log_F_inv[index]
 
 ;poly
 vmovaps xmm3,DWORD PTR L__real_1_over_3
 ;vfmaddps xmm2 ,xmm1,xmm3,DWORD PTR L__real_1_over_2 ; 1/3*r + 1/2
 vfmadd213ps xmm3,xmm1,DWORD PTR L__real_1_over_2
 
 vmulps xmm14,xmm1,xmm1 ; r*r
 ;vmovaps xmm3,DWORD PTR L__real_log2_tail ;xmm3 = log2_tail
 ;vmovaps xmm10,DWORD PTR L__real_log2_lead ;xmm10 = log2_lead
 ;vfmaddps xmm1 ,xmm14,xmm2,xmm1 ;poly = r + 1/2*r*r + 1/3*r*r*r
 vfmadd231ps xmm1, xmm14, xmm3
 
 vmulps xmm1,xmm1,DWORD PTR L__real_log2_e ; poly = -log2_e*poly
 ;vfmsubps xmm3 ,xmm5,xmm3,xmm1 ; z2 = (xexp * log2_tail) - poly
 ; m*log(2) + log(G) - poly
 ;vmulps xmm10,xmm10,xmm5 ; z1 = xexp * log2_lead
; xexp = 4321
; poly = 4321
; xex poly,p = 44332211
; t,h = thththth 
; add the two 
; xmm1 = poly, xmm5 = xex xmm11,p=lead,tail xmm12=lead/tail
 
 vpxor xmm13,xmm13,xmm13
 vunpcklps xmm9,xmm5,xmm1 ; xmm9 = 2211 in xexp,poly order
 vunpckhps xmm10,xmm5,xmm1 ; xmm10 = 4433 in xexp,poly order
 vinsertf128 ymm9 ,ymm9,xmm10,1 ; ymm9 = pol xexp,y - 44332211 in poly,xexp order
 
 vinsertf128 ymm10 ,ymm11,xmm12,1 ; thththth in z1,z2 order
 vaddps ymm11,ymm10,ymm9 ; z z1,2 = (- poly + log_128_tail[index]),(xexp + log_128_lead[index])
 vhaddps ymm13,ymm11,ymm13
 vextractf128 xmm14,ymm13,1 ;
 vmovlhps xmm9,xmm13,xmm14 ;xmm9 stores the result
 ;Now calculate for values near one
 ; r = x - 1.0;
 vmovaps xmm2,DWORD PTR L__real_two ; xmm2 = 2.0
 vpand xmm11,xmm0,DWORD PTR L__real_notsign
 ;get the zero input masks
 vpxor xmm12,xmm12,xmm12
 vsubps xmm0,xmm0,xmm15 ; xmm0 = r = = x - 1.0
 vpcmpeqd xmm12,xmm11,xmm12 ; xmm12 stores the mask for zero
 
 ; u = r / (2.0 + r)
 vaddps xmm2,xmm2,xmm0 ; (r+2.0)
 vdivps xmm1,xmm0,xmm2 ; u = r / (2.0 + r)

 ; correction = r * u
 vmulps xmm10,xmm0,xmm1 ; correction = u*r

 ; u = u + u;
 vaddps xmm1,xmm1,xmm1 ; u = u+u 
 vmulps xmm2,xmm1,xmm1 ; v = u^2

 ; r2 = (u * v * (ca_1 + v * ca_2) - correction)
 vmulps xmm3,xmm1,xmm2 ; u^3
 vmovaps xmm5,DWORD PTR L__real_ca2 ; xmm5 = ca_2 
 ;vfmaddps xmm2 ,xmm5,xmm2,DWORD PTR L__real_ca1 ; ca1 + ca2 * v
 ;vfmsubps xmm2 ,xmm2,xmm3,xmm10 ; r2 = (ca1 + ca2 * v) * u^3 - correction 
 
 vfmadd213ps xmm2, xmm5, DWORD PTR L__real_ca1
 vfmsub213ps xmm2	, xmm3,	 xmm10
 
 
 ; r + r2
 vpand xmm5,xmm0,DWORD PTR L__mask_lower
 vsubps xmm0,xmm0,xmm5 
 vaddps xmm2,xmm2,xmm0
 
 vmulps xmm1,xmm2,DWORD PTR L__real_log2_e_lead
 vmulps xmm0,xmm5,DWORD PTR L__real_log2_e_tail
 vmulps xmm5,xmm5,DWORD PTR L__real_log2_e_lead

 ;vfmaddps xmm0 ,xmm2,DWORD PTR L__real_log2_e_tail,xmm0
 vfmadd231ps xmm0, xmm2, DWORD PTR L__real_log2_e_tail
 vaddps xmm0,xmm0,xmm1
 vaddps xmm0,xmm0,xmm5

; till here I have 
; xmm0 = DWORD PTR L__near_one results 
; xmm9 = away from 1.0 results
; Masks 
;xmm4 stores the mask for nearone values
;xmm12 stores the mask for 0.0
;xmm6 = Mask for Nan Infs e's and negatives
;xmm7 = result of Nan Infs e's and negatives

 vmovaps xmm5,DWORD PTR L__real_ninf
;vmovaps xmm14,DWORD PTR L__real_inf
 ;vpcmov xmm0 ,xmm0,xmm9,xmm4 ; xmm0 stores nearone and away from one results
 ;vpcmov xmm0 ,xmm7,xmm0,xmm6
 ;vpcmov xmm0 ,xmm5,xmm0,xmm12
 
 
 vandnpd xmm13  , xmm4,  xmm9 
 vandpd xmm0  , xmm0,  xmm4 
 vorpd xmm0  , xmm0, xmm13 
 vandnpd xmm13  , xmm6,  xmm0 
 vandpd xmm0  , xmm7,  xmm6 
 vorpd xmm0  , xmm0, xmm13 
 vandnpd xmm13 , xmm12,  xmm0 
 vandpd xmm0  , xmm5, xmm12 
 vorpd xmm0  , xmm0, xmm13 
 
 
 
 
 
 
 RestoreAllXMMRegs
 StackDeallocate stack_size 
 ret
 



fname endp
TEXT ENDS

data SEGMENT READ
CONST SEGMENT


ALIGN 16
L__real_one DQ 03f8000003f800000h ; 1.0
 DQ 03f8000003f800000h
L__real_two DQ 04000000040000000h ; 1.0
 DQ 04000000040000000h
L__real_ninf DQ 0ff800000ff800000h ; -inf
 DQ 0ff800000ff800000h
L__real_inf DQ 07f8000007f800000h ; +inf
 DQ 07f8000007f800000h
L__real_nan DQ 07fc000007fc00000h ; NaN
 DQ 07fc000007fc00000h
L__real_ef DQ 0402DF854402DF854h ; float e
 DQ 0402DF854402DF854h
L__real_neg_qnan DQ 0ffc00000ffc00000h
 DQ 0ffc00000ffc00000h

L__real_sign DQ 08000000080000000h ; sign bit
 DQ 08000000080000000h
L__real_notsign DQ 07ffFFFFF7ffFFFFFh ; ^sign bit
 DQ 07ffFFFFF7ffFFFFFh
L__real_qnanbit DQ 00040000000400000h ; quiet nan bit
 DQ 00040000000400000h
L__real_mant DQ 0007FFFFF007FFFFFh ; mantissa bits
 DQ 0007FFFFF007FFFFFh
L__mask_127 DQ 00000007f0000007fh ; 
 DQ 00000007f0000007fh
L__mask_neg_127 DQ 0FFFFFF81FFFFFF81h ; 
 DQ 0FFFFFF81FFFFFF81h

L__mask_mant_all7 DQ 0007f0000007f0000h
 DQ 0007f0000007f0000h
L__mask_mant8 DQ 00000800000008000h
 DQ 00000800000008000h

L__real_ca1 DQ 03DAAAAAB3DAAAAABh ; 8.33333333333317923934e-02
 DQ 03DAAAAAB3DAAAAABh
L__real_ca2 DQ 03C4CCCCD3C4CCCCDh ; 1.25000000037717509602e-02
 DQ 03C4CCCCD3C4CCCCDh

L__real_log2_lead DQ 03F3170003F317000h ; 0.693115234375
 DQ 03F3170003F317000h
L__real_log2_tail DQ 03805FDF43805FDF4h ; 0.000031946183
 DQ 03805FDF43805FDF4h
L__real_half DQ 03f0000003f000000h ; 1/2
 DQ 03f0000003f000000h
L__real_log2_e DQ 0bfb8aa3bbfb8aa3bh
 DQ 0bfb8aa3bbfb8aa3bh
L__real_log2_e_lead DQ 03FB800003FB80000h ; 1.4375000000
 DQ 03FB800003FB80000h
L__real_log2_e_tail DQ 03BAA3B293BAA3B29h ; 0.0051950408889633
 DQ 03BAA3B293BAA3B29h
L__mask_lower DQ 0ffff0000ffff0000h
 DQ 0ffff0000ffff0000h
ALIGN 16
L__mask_253 DQ 0000000fd000000fdh
 DQ 0000000fd000000fdh

L__real_threshold DQ 03d8000003d800000h
 DQ 03d8000003d800000h

L__real_1_over_3 DQ 03eaaaaab3eaaaaabh
 DQ 03eaaaaab3eaaaaabh

L__real_1_over_2 DQ 03f0000003f000000h
 DQ 03f0000003f000000h



ALIGN 16
;L__log_128_lead L__log_128_tail 
L__log2_128_lead_tail DD 000000000h ;lead
 DD 000000000h ;tail
 DD 03c37c000h ;lead
 DD 0374a16ddh ;tail
 DD 03cb70000h
 DD 037f2d0b8h
 DD 03d08c000h
 DD 0381a3aa2h
 DD 03d35c000h
 DD 037b4dd63h
 DD 03d624000h
 DD 0383f5721h
 DD 03d874000h
 DD 0384e27e8h
 DD 03d9d4000h
 DD 0380bf749h
 DD 03db30000h
 DD 0387dbeb2h
 DD 03dc8c000h
 DD 037216e46h
 DD 03dde4000h
 DD 03684815bh
 DD 03df38000h
 DD 0383b045fh
 DD 03e044000h
 DD 0390b119bh
 DD 03e0ec000h
 DD 0391a32eah
 DD 03e194000h
 DD 038ba789eh
 DD 03e238000h
 DD 039553f30h
 DD 03e2e0000h
 DD 03651cfdeh
 DD 03e380000h
 DD 039685a9dh
 DD 03e424000h
 DD 039057a05h
 DD 03e4c4000h
 DD 0395ba0efh
 DD 03e564000h
 DD 0396bc5b6h
 DD 03e604000h
 DD 03936d9bbh
 DD 03e6a4000h
 DD 038772619h
 DD 03e740000h
 DD 039017ce9h
 DD 03e7dc000h
 DD 03902d720h
 DD 03e83c000h
 DD 038856dd8h
 DD 03e888000h
 DD 03941f6b4h
 DD 03e8d4000h
 DD 03980b652h
 DD 03e920000h
 DD 03980f561h
 DD 03e96c000h
 DD 039443f13h
 DD 03e9b8000h
 DD 038926752h
 DD 03ea00000h
 DD 039c8c763h
 DD 03ea4c000h
 DD 0391e12f3h
 DD 03ea94000h
 DD 039b7bf89h
 DD 03eae0000h
 DD 036d1cfdeh
 DD 03eb28000h
 DD 038c7f233h
 DD 03eb70000h
 DD 039087367h
 DD 03ebb8000h
 DD 038e95d3fh
 DD 03ec00000h
 DD 038256316h
 DD 03ec44000h
 DD 039d38e5ch
 DD 03ec8c000h
 DD 0396ea247h
 DD 03ecd4000h
 DD 0350e4788h
 DD 03ed18000h
 DD 0395d829fh
 DD 03ed5c000h
 DD 039c30f2fh
 DD 03eda0000h
 DD 039fd7ee7h
 DD 03ede8000h
 DD 03872e9e7h
 DD 03ee2c000h
 DD 03897d694h
 DD 03ee70000h
 DD 03824923ah
 DD 03eeb0000h
 DD 039ea7c06h
 DD 03eef4000h
 DD 039a7fa88h
 DD 03ef38000h
 DD 0391aa879h
 DD 03ef78000h
 DD 039dace65h
 DD 03efbc000h
 DD 039215a32h
 DD 03effc000h
 DD 039af3350h
 DD 03f01c000h
 DD 03a7b5172h
 DD 03f040000h
 DD 0389cf27fh
 DD 03f060000h
 DD 03902806bh
 DD 03f080000h
 DD 03909d8a9h
 DD 03f0a0000h
 DD 038c9faa1h
 DD 03f0c0000h
 DD 037a33dcah
 DD 03f0dc000h
 DD 03a6623d2h
 DD 03f0fc000h
 DD 03a3c7a61h
 DD 03f11c000h
 DD 03a083a84h
 DD 03f13c000h
 DD 039930161h
 DD 03f15c000h
 DD 035d1cfdeh
 DD 03f178000h
 DD 03a2d0ebdh
 DD 03f198000h
 DD 0399f1aadh
 DD 03f1b4000h
 DD 03a67ff6dh
 DD 03f1d4000h
 DD 039ecfea8h
 DD 03f1f0000h
 DD 03a7b26f3h
 DD 03f210000h
 DD 039ec1fa6h
 DD 03f22c000h
 DD 03a675314h
 DD 03f24c000h
 DD 0399e12f3h
 DD 03f268000h
 DD 03a2d4b66h
 DD 03f288000h
 DD 0370c3845h
 DD 03f2a4000h
 DD 0399ba329h
 DD 03f2c0000h
 DD 03a1044d3h
 DD 03f2dc000h
 DD 03a49a196h
 DD 03f2f8000h
 DD 03a79fe83h
 DD 03f318000h
 DD 03905c7aah
 DD 03f334000h
 DD 039802391h
 DD 03f350000h
 DD 039abe796h
 DD 03f36c000h
 DD 039c65a9dh
 DD 03f388000h
 DD 039cfa6c5h
 DD 03f3a4000h
 DD 039c7f593h
 DD 03f3c0000h
 DD 039af6ff7h
 DD 03f3dc000h
 DD 039863e4dh
 DD 03f3f8000h
 DD 0391910c1h
 DD 03f414000h
 DD 0369d5be7h
 DD 03f42c000h
 DD 03a541616h
 DD 03f448000h
 DD 03a1ee960h
 DD 03f464000h
 DD 039c38ed2h
 DD 03f480000h
 DD 038e61600h
 DD 03f498000h
 DD 03a4fedb4h
 DD 03f4b4000h
 DD 039f6b4abh
 DD 03f4d0000h
 DD 038f8d3b0h
 DD 03f4e8000h
 DD 03a3b3faah
 DD 03f504000h
 DD 0399fb693h
 DD 03f51c000h
 DD 03a5cfe71h
 DD 03f538000h
 DD 039c5740bh
 DD 03f550000h
 DD 03a611eb0h
 DD 03f56c000h
 DD 039b079c4h
 DD 03f584000h
 DD 03a4824d7h
 DD 03f5a0000h
 DD 039439a54h
 DD 03f5b8000h
 DD 03a1291eah
 DD 03f5d0000h
 DD 03a6d3673h
 DD 03f5ec000h
 DD 03981c731h
 DD 03f604000h
 DD 03a0da88fh
 DD 03f61c000h
 DD 03a53945ch
 DD 03f638000h
 DD 03895ae91h
 DD 03f650000h
 DD 03996372ah
 DD 03f668000h
 DD 039f9a832h
 DD 03f680000h
 DD 03a27eda4h
 DD 03f698000h
 DD 03a4c764fh
 DD 03f6b0000h
 DD 03a6a7c06h
 DD 03f6cc000h
 DD 0370321ebh
 DD 03f6e4000h
 DD 03899ab3fh
 DD 03f6fc000h
 DD 038f02086h
 DD 03f714000h
 DD 0390a1707h
 DD 03f72c000h
 DD 039031e44h
 DD 03f744000h
 DD 038c6b362h
 DD 03f75c000h
 DD 0382bf195h
 DD 03f770000h
 DD 03a768e36h
 DD 03f788000h
 DD 03a5c503bh
 DD 03f7a0000h
 DD 03a3c1179h
 DD 03f7b8000h
 DD 03a15de1dh
 DD 03f7d0000h
 DD 039d3845dh
 DD 03f7e8000h
 DD 0395f263fh
 DD 03f800000h
 DD 000000000h 

ALIGN 16
L__log_F_inv DD 040000000h
 DD 03ffe03f8h
 DD 03ffc0fc1h
 DD 03ffa232dh
 DD 03ff83e10h
 DD 03ff6603eh
 DD 03ff4898dh
 DD 03ff2b9d6h
 DD 03ff0f0f1h
 DD 03fef2eb7h
 DD 03fed7304h
 DD 03febbdb3h
 DD 03fea0ea1h
 DD 03fe865ach
 DD 03fe6c2b4h
 DD 03fe52598h
 DD 03fe38e39h
 DD 03fe1fc78h
 DD 03fe07038h
 DD 03fdee95ch
 DD 03fdd67c9h
 DD 03fdbeb62h
 DD 03fda740eh
 DD 03fd901b2h
 DD 03fd79436h
 DD 03fd62b81h
 DD 03fd4c77bh
 DD 03fd3680dh
 DD 03fd20d21h
 DD 03fd0b6a0h
 DD 03fcf6475h
 DD 03fce168ah
 DD 03fcccccdh
 DD 03fcb8728h
 DD 03fca4588h
 DD 03fc907dah
 DD 03fc7ce0ch
 DD 03fc6980ch
 DD 03fc565c8h
 DD 03fc43730h
 DD 03fc30c31h
 DD 03fc1e4bch
 DD 03fc0c0c1h
 DD 03fbfa030h
 DD 03fbe82fah
 DD 03fbd6910h
 DD 03fbc5264h
 DD 03fbb3ee7h
 DD 03fba2e8ch
 DD 03fb92144h
 DD 03fb81703h
 DD 03fb70fbbh
 DD 03fb60b61h
 DD 03fb509e7h
 DD 03fb40b41h
 DD 03fb30f63h
 DD 03fb21643h
 DD 03fb11fd4h
 DD 03fb02c0bh
 DD 03faf3adeh
 DD 03fae4c41h
 DD 03fad602bh
 DD 03fac7692h
 DD 03fab8f6ah
 DD 03faaaaabh
 DD 03fa9c84ah
 DD 03fa8e83fh
 DD 03fa80a81h
 DD 03fa72f05h
 DD 03fa655c4h
 DD 03fa57eb5h
 DD 03fa4a9cfh
 DD 03fa3d70ah
 DD 03fa3065eh
 DD 03fa237c3h
 DD 03fa16b31h
 DD 03fa0a0a1h
 DD 03f9fd80ah
 DD 03f9f1166h
 DD 03f9e4cadh
 DD 03f9d89d9h
 DD 03f9cc8e1h
 DD 03f9c09c1h
 DD 03f9b4c70h
 DD 03f9a90e8h
 DD 03f99d723h
 DD 03f991f1ah
 DD 03f9868c8h
 DD 03f97b426h
 DD 03f97012eh
 DD 03f964fdah
 DD 03f95a025h
 DD 03f94f209h
 DD 03f944581h
 DD 03f939a86h
 DD 03f92f114h
 DD 03f924925h
 DD 03f91a2b4h
 DD 03f90fdbch
 DD 03f905a38h
 DD 03f8fb824h
 DD 03f8f177ah
 DD 03f8e7835h
 DD 03f8dda52h
 DD 03f8d3dcbh
 DD 03f8ca29ch
 DD 03f8c08c1h
 DD 03f8b7034h
 DD 03f8ad8f3h
 DD 03f8a42f8h
 DD 03f89ae41h
 DD 03f891ac7h
 DD 03f888889h
 DD 03f87f781h
 DD 03f8767abh
 DD 03f86d905h
 DD 03f864b8ah
 DD 03f85bf37h
 DD 03f853408h
 DD 03f84a9fah
 DD 03f842108h
 DD 03f839930h
 DD 03f83126fh
 DD 03f828cc0h
 DD 03f820821h
 DD 03f81848eh
 DD 03f810204h
 DD 03f808081h
 DD 03f800000h


CONST ENDS
data ENDS
END

