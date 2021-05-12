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

include exp_tables.inc
include fm.inc

ALM_PROTO_FMA3 vrsa_expf

stack_size      EQU     88h

save_rdi	equ		20h
save_rsi	equ		30h
save_xmm10  equ     40h

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    StackAllocate stack_size
    SaveReg rdi,save_rdi
    SaveReg rsi,save_rsi  
    SaveXmm xmm10,save_xmm10	  
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
 vmovdqa xmm10,xmm0 ; save for later use
 vminps xmm0,xmm0,XMMWORD PTR L__max_expf_arg
 vcvtps2pd ymm2,xmm0 ;ymm2 = (double x0 ,x1,x2,)x3

 ; x * (64/ln(2)) 
 vmulpd ymm3,ymm2,L__real_64_by_log2 ;ymm3 = x3 * (64/ln x2,(2) * (64/ln(2),x1 * (64/ln(2),x0 * (64/ln(2)

 ; n = int( x * (64/ln(2)) )
 vcvtpd2dq xmm4,ymm3 ;xmm4 = (int n0 ,n1,n2,)n3 
 vcvtdq2pd ymm0,xmm4 ;ymm0 = (double n0 ,n1,n2,)n3 

 ; r = x - n * ln(2)/64
 
 
 vfmadd132pd ymm0,ymm2,L__real_mlog2_by_64 ;ymm0 = r r1,r2,3,r0
 vmovdqa ymm3,YMMWORD PTR L__real_4_1_by_6s
 vfmadd213pd  ymm3 ,ymm0,L__real_4_1_by_2s             
 
 vmulpd       ymm2,ymm0,ymm0   
 
 vfmadd213pd   ymm3,ymm2,ymm0          
 
 vpsrad      xmm1,xmm4,6    
 vpslld      xmm0,xmm4,26   
 vpsrld      xmm4,xmm0,26          
 
 
 lea r10,L__two_to_jby64_table
 vmovd   eax,xmm4      
 vpsrldq  xmm0,xmm4,4  
 vmovd    ecx,xmm0     
 vmovsd xmm2,QWORD PTR[r10 + rax * 8]
 vmovhpd xmm2, xmm2,QWORD PTR[r10 + rcx * 8]    
 vpsrldq  xmm4,xmm0,4 
 vmovd    eax,xmm4     
 vpsrldq  xmm0, xmm4,4 
 vmovd    ecx,xmm0      
 vmovsd  xmm0,QWORD PTR[r10 + rax * 8] 
 vmovhpd xmm0,xmm0,QWORD PTR[r10 + rcx * 8]    
 vinsertf128 ymm2 ,ymm2,xmm0,1
 vfmadd213pd ymm3, ymm2,  ymm2   

 ; m = (n - j) / 64 
 vpmovsxdq xmm2,xmm1 ;xmm2 = m1,m0 
 vpsllq xmm0,xmm2,52 ;xmm0 = 2^m 2,1^m0
 vpsrldq xmm2,xmm1,8 ;xmm2 = X m2 ,m3,XX,X
 vpmovsxdq xmm1,xmm2 ;xmm1 = m3,m2
 vpsllq xmm2,xmm1,52 ;xmm2 = 2^m 2,3^m2 
 
 ;2 ^m * (f + (f*q))
 vextractf128 xmm1,ymm3,1
 vpaddq xmm0,xmm0,xmm3
 vpaddq xmm1,xmm2,xmm1
 vinsertf128 ymm2 ,ymm0,xmm1,1
 vcvtpd2ps xmm0,ymm2
 
 ;;special case for any x < min_exp_arg
 ;;remove this code if the above code takes care of this
 vmovdqa xmm1,XMMWORD PTR L__min_expf_arg 
 vcmpps xmm2 ,xmm1,xmm10,1
 vpand xmm1,xmm0,xmm2 ;put zeros in place of any x < min_exp2_arg 
 
 ;;special case for any x = nan
 ;;remove this code if the above code takes care of this
 vcmpps xmm2 ,xmm10,xmm10,00h
 vaddps xmm3,xmm10,xmm10 ;make qnan to put in place of any x =nan 
 ;vpcmov xmm0 ,xmm1,xmm3,xmm2 
 vandnpd  xmm5, xmm2,xmm3
 vandpd   xmm0, xmm1,xmm2
 vorpd    xmm0, xmm0,xmm5  

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
 RestoreXmm xmm10, save_xmm10
 RestoreReg rsi,save_rsi
 RestoreReg rdi,save_rdi
 StackDeallocate stack_size 
 ret 
 
fname endp
TEXT ENDS

data SEGMENT READ PAGE

ALIGN 16
L__max_expf_arg DQ 042B1721842B17218h, 042B1721842B17218h
L__min_expf_arg DQ 0C2CE8ED0C2CE8ED0h, 0C2CE8ED0C2CE8ED0h

ALIGN 32 
L__real_64_by_log2 DQ 040571547652b82feh, 040571547652b82feh ; 64/ln(2)
                   DQ 040571547652b82feh, 040571547652b82feh ; 64/ln(2)
L__real_mlog2_by_64 DQ 0bf862e42fefa39efh, 0bf862e42fefa39efh ; log2_by_64
                    DQ 0bf862e42fefa39efh, 0bf862e42fefa39efh ; log2_by_64
L__real_4_1_by_2s DQ 03fe0000000000000h, 03fe0000000000000h
                  DQ 03fe0000000000000h, 03fe0000000000000h 
L__real_4_1_by_6s DQ 03fc5555555555555h, 03fc5555555555555h ; 1/6
                  DQ 03fc5555555555555h, 03fc5555555555555h

data ENDS
END

