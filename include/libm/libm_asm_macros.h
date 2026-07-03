/*
 * Copyright (C) 2026, Advanced Micro Devices, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef __LIBM_ASM_MACROS_H__
#define __LIBM_ASM_MACROS_H__

/*
 * Include this header after any system/libm header that pulls in
 * <immintrin.h> (and thus <uintrintrin.h>). The lowercase rip/rsp
 * register aliases below would otherwise corrupt uintr's struct fields.
 *
 * LIBM_ASM_SYNTAX_ATT  - AT&T operand order:  mov(src, dst)
 * LIBM_ASM_SYNTAX_INTEL - Intel operand order: mov(dst, src)
 *
 * libm sources use AT&T operand order; define LIBM_ASM_SYNTAX_ATT before
 * including this header (or rely on the default below).
 */
#if !defined(LIBM_ASM_SYNTAX_ATT) && !defined(LIBM_ASM_SYNTAX_INTEL)
#define LIBM_ASM_SYNTAX_ATT
#endif

#define STRINGIFY_IMPL_(...) #__VA_ARGS__
#define STRINGIFY_(...) STRINGIFY_IMPL_(__VA_ARGS__)
#define GET_MACRO_(_1_,_2_,_3_,_4_,NAME,...) NAME


/* ------------------------------------------------------------------ */
/*  GCC/clang extended __asm__ volatile ( ... )                    */
/* ------------------------------------------------------------------ */

#  define BEGIN_ASM()                __asm__ volatile (
#  define END_ASM(...)               __VA_ARGS__ );

#  define REGISTER_(r)               "%%" #r
#  define imm(x)                     "$" #x
#  define var(x)                     "%[" #x "]"
#  define SYM_(s)                     STRINGIFY_(s)

/* GAS accepts size-suffixed mnemonics natively. */
#  define MNEM_movl_                  movl
#  define MNEM_andq_                  andq
#  define MNEM_orq_                   orq
#  define MNEM_xorq_                  xorq
#  define MNEM_notq_                  notq
#  define MNEM_mulq_                  mulq
#  define MNEM_andw_                  andw
#  define MNEM_cvttsd2siq_            cvttsd2siq
#  define MNEM_cvtsi2sdq_             cvtsi2sdq
#  define MNEM_movabs_                movabs

#  define MEM_1_(reg)                "(" reg ")"
#  define MEM_2_(reg,disp)           STRINGIFY_(disp) "(" reg ")"
#  define MEM_3_(reg,off,scale)      "(" reg "," off "," STRINGIFY_(scale) ")"
#  define MEM_4_(reg,off,scale,disp) STRINGIFY_(disp) "(" reg "," off "," STRINGIFY_(scale) ")"

/* %= yields a unique id per asm() instance so inlined labels don't collide. */
#  define LABEL_(name)             ".L" STRINGIFY_(name) "%="

#  define label(name)                LABEL_(name) ":\n\t"
#  define label_align(name, align)   ".p2align " STRINGIFY_(align) "\n\t" LABEL_(name) ":\n\t"
#  define JMP_(insn, target)         STRINGIFY_(insn) " " LABEL_(target) "\n\t"

#  ifdef LIBM_ASM_SYNTAX_ATT

#    define INSTR_0_(name)              STRINGIFY_(name) "\n\t"
#    define INSTR_1_(name,_0)           STRINGIFY_(name) " " _0 "\n\t"
#    define INSTR_2_(name,_0,_1)        STRINGIFY_(name) " " _0 ", " _1 "\n\t"
#    define INSTR_3_(name,_0,_1,_2)     STRINGIFY_(name) " " _0 ", " _1 ", " _2 "\n\t"
#    define INSTR_4_(name,_0,_1,_2,_3)  STRINGIFY_(name) " " _0 ", " _1 ", " _2 ", " _3 "\n\t"

#  else

#    define INSTR_0_(name)              STRINGIFY_(name) "\n\t"
#    define INSTR_1_(name,_0)           STRINGIFY_(name) " " _0 "\n\t"
#    define INSTR_2_(name,_0,_1)        STRINGIFY_(name) " " _1 ", " _0 "\n\t"
#    define INSTR_3_(name,_0,_1,_2)     STRINGIFY_(name) " " _2 ", " _1 ", " _0 "\n\t"
#    define INSTR_4_(name,_0,_1,_2,_3)  STRINGIFY_(name) " " _3 ", " _2 ", " _1 ", " _0 "\n\t"

#  endif

/* PC-relative rodata access (BLIS uses LEA_RIP with hardcoded (%%rip) likewise). */
#  define mem_rip(sym)               mem(RIP, sym)
#  define lea_rip(sym, reg)          lea(mem_rip(sym), reg)


/* --- Unified front-door macros (work on both backends) --------------- */
#define begin_asm()   BEGIN_ASM()
#define end_asm(...)  END_ASM(__VA_ARGS__)

#define INSTR_(name,...) GET_MACRO_(__VA_ARGS__,INSTR_4_,INSTR_3_,INSTR_2_,INSTR_1_,INSTR_0_)(name,__VA_ARGS__)

/* Variadic mem(...) dispatch -- 1 to 4 args. */
#define GET_MEM_(_1_,_2_,_3_,_4_,NAME,...) NAME
#define mem(...) GET_MEM_(__VA_ARGS__, MEM_4_, MEM_3_, MEM_2_, MEM_1_)(__VA_ARGS__)



/* ================================================================== */
/*  REGISTERS                                                      */
/* ================================================================== */
/* Upper-case name is canonical (REGISTER_());
 * lower-case alias forwards to it (libm sources use AT&T lower-case register names). */

/* --- 64-bit general-purpose registers ---------------------------- */
#define RAX  REGISTER_(rax)
#define RBX  REGISTER_(rbx)
#define RCX  REGISTER_(rcx)
#define RDX  REGISTER_(rdx)
#define RSI  REGISTER_(rsi)
#define RDI  REGISTER_(rdi)
#define RBP  REGISTER_(rbp)
#define RSP  REGISTER_(rsp)
#define RIP  REGISTER_(rip)
#define R8   REGISTER_(r8)
#define R9   REGISTER_(r9)
#define R10  REGISTER_(r10)
#define R11  REGISTER_(r11)
#define R12  REGISTER_(r12)
#define R13  REGISTER_(r13)
#define R14  REGISTER_(r14)
#define R15  REGISTER_(r15)

#define rax  RAX
#define rbx  RBX
#define rcx  RCX
#define rdx  RDX
#define rsi  RSI
#define rdi  RDI
#define rbp  RBP
#define rsp  RSP
#define rip  RIP
#define r8   R8
#define r9   R9
#define r10  R10
#define r11  R11
#define r12  R12
#define r13  R13
#define r14  R14
#define r15  R15

/* --- 32-bit general-purpose registers ---------------------------- */
#define EAX   REGISTER_(eax)
#define EBX   REGISTER_(ebx)
#define ECX   REGISTER_(ecx)
#define EDX   REGISTER_(edx)
#define ESI   REGISTER_(esi)
#define EDI   REGISTER_(edi)
#define EBP   REGISTER_(ebp)
#define ESP   REGISTER_(esp)
#define R8D   REGISTER_(r8d)
#define R9D   REGISTER_(r9d)
#define R10D  REGISTER_(r10d)
#define R11D  REGISTER_(r11d)
#define R12D  REGISTER_(r12d)
#define R13D  REGISTER_(r13d)
#define R14D  REGISTER_(r14d)
#define R15D  REGISTER_(r15d)

#define eax   EAX
#define ebx   EBX
#define ecx   ECX
#define edx   EDX
#define esi   ESI
#define edi   EDI
#define ebp   EBP
#define esp   ESP
#define r8d   R8D
#define r9d   R9D
#define r10d  R10D
#define r11d  R11D
#define r12d  R12D
#define r13d  R13D
#define r14d  R14D
#define r15d  R15D

/* --- 16-bit / 8-bit general-purpose registers -------------------- */
#define AX  REGISTER_(ax)
#define CL  REGISTER_(cl)

#define ax  AX
#define cl  CL

/* --- 128-bit XMM registers --------------------------------------- */
#define XMM0   REGISTER_(xmm0)
#define XMM1   REGISTER_(xmm1)
#define XMM2   REGISTER_(xmm2)
#define XMM3   REGISTER_(xmm3)
#define XMM4   REGISTER_(xmm4)
#define XMM5   REGISTER_(xmm5)
#define XMM6   REGISTER_(xmm6)
#define XMM7   REGISTER_(xmm7)
#define XMM8   REGISTER_(xmm8)
#define XMM9   REGISTER_(xmm9)
#define XMM10  REGISTER_(xmm10)
#define XMM11  REGISTER_(xmm11)
#define XMM12  REGISTER_(xmm12)
#define XMM13  REGISTER_(xmm13)
#define XMM14  REGISTER_(xmm14)
#define XMM15  REGISTER_(xmm15)

#define xmm0   XMM0
#define xmm1   XMM1
#define xmm2   XMM2
#define xmm3   XMM3
#define xmm4   XMM4
#define xmm5   XMM5
#define xmm6   XMM6
#define xmm7   XMM7
#define xmm8   XMM8
#define xmm9   XMM9
#define xmm10  XMM10
#define xmm11  XMM11
#define xmm12  XMM12
#define xmm13  XMM13
#define xmm14  XMM14
#define xmm15  XMM15

/* --- 256-bit YMM registers --------------------------------------- */
#define YMM0   REGISTER_(ymm0)
#define YMM1   REGISTER_(ymm1)
#define YMM2   REGISTER_(ymm2)
#define YMM3   REGISTER_(ymm3)
#define YMM4   REGISTER_(ymm4)
#define YMM5   REGISTER_(ymm5)
#define YMM6   REGISTER_(ymm6)
#define YMM7   REGISTER_(ymm7)
#define YMM8   REGISTER_(ymm8)
#define YMM9   REGISTER_(ymm9)
#define YMM10  REGISTER_(ymm10)
#define YMM11  REGISTER_(ymm11)
#define YMM12  REGISTER_(ymm12)
#define YMM13  REGISTER_(ymm13)
#define YMM14  REGISTER_(ymm14)
#define YMM15  REGISTER_(ymm15)

#define ymm0   YMM0
#define ymm1   YMM1
#define ymm2   YMM2
#define ymm3   YMM3
#define ymm4   YMM4
#define ymm5   YMM5
#define ymm6   YMM6
#define ymm7   YMM7
#define ymm8   YMM8
#define ymm9   YMM9
#define ymm10  YMM10
#define ymm11  YMM11
#define ymm12  YMM12
#define ymm13  YMM13
#define ymm14  YMM14
#define ymm15  YMM15


/* ================================================================== */
/*  DATA ALIGNMENT ATTRIBUTES                                      */
/* ================================================================== */
#if defined(_MSC_VER) && !defined(__clang__)
#define ALIGN16_USED __declspec(align(16))
#define ALIGN32_USED __declspec(align(32))
#else
#define ALIGN16_USED __attribute__((aligned(16), used))
#define ALIGN32_USED __attribute__((aligned(32), used))
#endif


/* ================================================================== */
/*  INSTRUCTIONS                                                   */
/* ================================================================== */
/*
 * Operand-taking wrappers are variadic; INSTR_() dispatches on operand count
 * and (for AT&T) preserves source order.
 * Entries are sorted alphabetically within each group.
 */

/* --- Control transfer: conditional & unconditional jumps --------- */
#define ja(t)  JMP_(ja, t)
#define jae(t) JMP_(jae, t)
#define jb(t)  JMP_(jb, t)
#define jbe(t) JMP_(jbe, t)
#define jc(t)  JMP_(jc, t)
#define je(t)  JMP_(je, t)
#define jg(t)  JMP_(jg, t)
#define jge(t) JMP_(jge, t)
#define jl(t)  JMP_(jl, t)
#define jle(t) JMP_(jle, t)
#define jmp(t) JMP_(jmp, t)
#define jnc(t) JMP_(jnc, t)
#define jne(t) JMP_(jne, t)
#define jno(t) JMP_(jno, t)
#define jnp(t) JMP_(jnp, t)
#define jns(t) JMP_(jns, t)
#define jnz(t) JMP_(jnz, t)
#define jo(t)  JMP_(jo, t)
#define jp(t)  JMP_(jp, t)
#define js(t)  JMP_(js, t)
#define jz(t)  JMP_(jz, t)

/* --- Control transfer: call / return ----------------------------- */
#define call_fn(fn)   INSTR_(call, SYM_(fn))
/* INSTR_(ret) would pass empty __VA_ARGS__ to GET_MACRO_; use INSTR_0_. */
#define ret           INSTR_0_(ret)

/* --- Scalar integer / general-purpose (GPR) ---------------------- */
#define adc(...)    INSTR_(adc, __VA_ARGS__)
#define add(...)    INSTR_(add, __VA_ARGS__)
#define andq(...)   INSTR_(MNEM_andq_, __VA_ARGS__)
#define andw(...)   INSTR_(MNEM_andw_, __VA_ARGS__)
#define bsr(...)    INSTR_(bsr, __VA_ARGS__)
#define btr(...)    INSTR_(btr, __VA_ARGS__)
#define bts(...)    INSTR_(bts, __VA_ARGS__)
#define cmp(...)    INSTR_(cmp, __VA_ARGS__)
#define dec(...)    INSTR_(dec, __VA_ARGS__)
#define inc(...)    INSTR_(inc, __VA_ARGS__)
#define lea(...)    INSTR_(lea, __VA_ARGS__)
#define mov(...)    INSTR_(mov, __VA_ARGS__)
#define movabs(...) INSTR_(MNEM_movabs_, __VA_ARGS__)
#define movl(...)   INSTR_(MNEM_movl_, __VA_ARGS__)
#define mulq(...)   INSTR_(MNEM_mulq_, __VA_ARGS__)
#define neg(...)    INSTR_(neg, __VA_ARGS__)
#define notq(...)   INSTR_(MNEM_notq_, __VA_ARGS__)
#define orq(...)    INSTR_(MNEM_orq_, __VA_ARGS__)
#define pop(...)    INSTR_(pop, __VA_ARGS__)
#define push(...)   INSTR_(push, __VA_ARGS__)
#define ror(...)    INSTR_(ror, __VA_ARGS__)
#define sbb(...)    INSTR_(sbb, __VA_ARGS__)
#define shl(...)    INSTR_(shl, __VA_ARGS__)
#define shr(...)    INSTR_(shr, __VA_ARGS__)
#define sub(...)    INSTR_(sub, __VA_ARGS__)
#define test(...)   INSTR_(test, __VA_ARGS__)
#define xorq(...)   INSTR_(MNEM_xorq_, __VA_ARGS__)

/* zero-operand sign/size-extension instructions */
#define cdq  INSTR_0_(cdq)
#define cdqe INSTR_0_(cdqe)
#define cqo  INSTR_0_(cqo)
#define cwde INSTR_0_(cwde)

/* --- BMI2 (3-operand, non-flag-clobbering) ----------------------- */
#define bzhi(...) INSTR_(bzhi, __VA_ARGS__)
#define mulx(...) INSTR_(mulx, __VA_ARGS__)
#define shlx(...) INSTR_(shlx, __VA_ARGS__)
#define shrx(...) INSTR_(shrx, __VA_ARGS__)

/* --- x87 FPU ----------------------------------------------------- */
#define ffree_st0  STRINGIFY_(ffree %%st(0)) "\n\t"
#define ffree_st1  STRINGIFY_(ffree %%st(1)) "\n\t"
#define fldl_(m)   "fldl " m "\n\t"
#define fnclex_()  STRINGIFY_(fnclex) "\n\t"
#define fnstsw_(d) "fnstsw " d "\n\t"
#define fprem1_()  STRINGIFY_(fprem1) "\n\t"
#define fprem_()   STRINGIFY_(fprem) "\n\t"
#define fstpl_(m)  "fstpl " m "\n\t"

/* --- Legacy SSE / SSE2 (non-VEX, 128-bit) ------------------------ */
#define addpd(...)      INSTR_(addpd, __VA_ARGS__)
#define addsd(...)      INSTR_(addsd, __VA_ARGS__)
#define andpd(...)      INSTR_(andpd, __VA_ARGS__)
#define comisd(...)     INSTR_(comisd, __VA_ARGS__)
#define cvtsi2sdq(...)  INSTR_(MNEM_cvtsi2sdq_, __VA_ARGS__)
#define cvttsd2siq(...) INSTR_(MNEM_cvttsd2siq_, __VA_ARGS__)
#define divsd(...)      INSTR_(divsd, __VA_ARGS__)
#define movapd(...)     INSTR_(movapd, __VA_ARGS__)
#define movd(...)       INSTR_(movd, __VA_ARGS__)
#define movdqa(...)     INSTR_(movdqa, __VA_ARGS__)
#define movdqu(...)     INSTR_(movdqu, __VA_ARGS__)
#define movhlps(...)    INSTR_(movhlps, __VA_ARGS__)
#define movlhps(...)    INSTR_(movlhps, __VA_ARGS__)
#define movq(...)       INSTR_(movq, __VA_ARGS__)
#define movsd(...)      INSTR_(movsd, __VA_ARGS__)
#define mulpd(...)      INSTR_(mulpd, __VA_ARGS__)
#define mulsd(...)      INSTR_(mulsd, __VA_ARGS__)
#define orpd(...)       INSTR_(orpd, __VA_ARGS__)
#define pand(...)       INSTR_(pand, __VA_ARGS__)
#define por(...)        INSTR_(por, __VA_ARGS__)
#define psllq(...)      INSTR_(psllq, __VA_ARGS__)
#define psrldq(...)     INSTR_(psrldq, __VA_ARGS__)
#define psrlq(...)      INSTR_(psrlq, __VA_ARGS__)
#define subpd(...)      INSTR_(subpd, __VA_ARGS__)
#define subsd(...)      INSTR_(subsd, __VA_ARGS__)
#define ucomisd(...)    INSTR_(ucomisd, __VA_ARGS__)
#define ucomiss(...)    INSTR_(ucomiss, __VA_ARGS__)

/* --- AVX / AVX1 + FMA (VEX-encoded) ------------------------------ */
#define vaddpd(...)         INSTR_(vaddpd, __VA_ARGS__)
#define vaddps(...)         INSTR_(vaddps, __VA_ARGS__)
#define vaddsd(...)         INSTR_(vaddsd, __VA_ARGS__)
#define vaddss(...)         INSTR_(vaddss, __VA_ARGS__)
#define vandnpd(...)        INSTR_(vandnpd, __VA_ARGS__)
#define vandnps(...)        INSTR_(vandnps, __VA_ARGS__)
#define vandpd(...)         INSTR_(vandpd, __VA_ARGS__)
#define vandps(...)         INSTR_(vandps, __VA_ARGS__)
#define vblendpd(...)       INSTR_(vblendpd, __VA_ARGS__)
#define vblendvpd(...)      INSTR_(vblendvpd, __VA_ARGS__)
#define vbroadcastf128(...) INSTR_(vbroadcastf128, __VA_ARGS__)
#define vbroadcastsd(...)   INSTR_(vbroadcastsd, __VA_ARGS__)
#define vbroadcastss(...)   INSTR_(vbroadcastss, __VA_ARGS__)
#define vcmpeqpd(...)       INSTR_(vcmpeqpd, __VA_ARGS__)
#define vcmpeqps(...)       INSTR_(vcmpeqps, __VA_ARGS__)
#define vcmpgepd(...)       INSTR_(vcmpgepd, __VA_ARGS__)
#define vcmpgtpd(...)       INSTR_(vcmpgtpd, __VA_ARGS__)
#define vcmplepd(...)       INSTR_(vcmplepd, __VA_ARGS__)
#define vcmpleps(...)       INSTR_(vcmpleps, __VA_ARGS__)
#define vcmpltpd(...)       INSTR_(vcmpltpd, __VA_ARGS__)
#define vcmpltps(...)       INSTR_(vcmpltps, __VA_ARGS__)
#define vcmpneqpd(...)      INSTR_(vcmpneqpd, __VA_ARGS__)
#define vcmpneqps(...)      INSTR_(vcmpneqps, __VA_ARGS__)
#define vcmpnlepd(...)      INSTR_(vcmpnlepd, __VA_ARGS__)
#define vcmpnltpd(...)      INSTR_(vcmpnltpd, __VA_ARGS__)
#define vcmpordpd(...)      INSTR_(vcmpordpd, __VA_ARGS__)
#define vcmppd(...)         INSTR_(vcmppd, __VA_ARGS__)
#define vcmpps(...)         INSTR_(vcmpps, __VA_ARGS__)
#define vcmpsd(...)         INSTR_(vcmpsd, __VA_ARGS__)
#define vcmpss(...)         INSTR_(vcmpss, __VA_ARGS__)
#define vcmpunordpd(...)    INSTR_(vcmpunordpd, __VA_ARGS__)
#define vcomisd(...)        INSTR_(vcomisd, __VA_ARGS__)
#define vcomiss(...)        INSTR_(vcomiss, __VA_ARGS__)
#define vcvtdq2pd(...)      INSTR_(vcvtdq2pd, __VA_ARGS__)
#define vcvtdq2ps(...)      INSTR_(vcvtdq2ps, __VA_ARGS__)
#define vcvtpd2dq(...)      INSTR_(vcvtpd2dq, __VA_ARGS__)
#define vcvtpd2ps(...)      INSTR_(vcvtpd2ps, __VA_ARGS__)
#define vcvtps2dq(...)      INSTR_(vcvtps2dq, __VA_ARGS__)
#define vcvtps2pd(...)      INSTR_(vcvtps2pd, __VA_ARGS__)
#define vcvtsd2ss(...)      INSTR_(vcvtsd2ss, __VA_ARGS__)
#define vcvtsi2sd(...)      INSTR_(vcvtsi2sd, __VA_ARGS__)
#define vcvtsi2ss(...)      INSTR_(vcvtsi2ss, __VA_ARGS__)
#define vcvtss2sd(...)      INSTR_(vcvtss2sd, __VA_ARGS__)
#define vcvttpd2dq(...)     INSTR_(vcvttpd2dq, __VA_ARGS__)
#define vcvttps2dq(...)     INSTR_(vcvttps2dq, __VA_ARGS__)
#define vcvttsd2si(...)     INSTR_(vcvttsd2si, __VA_ARGS__)
#define vcvttss2si(...)     INSTR_(vcvttss2si, __VA_ARGS__)
#define vdivpd(...)         INSTR_(vdivpd, __VA_ARGS__)
#define vdivps(...)         INSTR_(vdivps, __VA_ARGS__)
#define vdivsd(...)         INSTR_(vdivsd, __VA_ARGS__)
#define vdivss(...)         INSTR_(vdivss, __VA_ARGS__)
#define vextractf128(...)   INSTR_(vextractf128, __VA_ARGS__)
#define vfmadd132pd(...)    INSTR_(vfmadd132pd, __VA_ARGS__)
#define vfmadd132sd(...)    INSTR_(vfmadd132sd, __VA_ARGS__)
#define vfmadd132ss(...)    INSTR_(vfmadd132ss, __VA_ARGS__)
#define vfmadd213pd(...)    INSTR_(vfmadd213pd, __VA_ARGS__)
#define vfmadd213ps(...)    INSTR_(vfmadd213ps, __VA_ARGS__)
#define vfmadd213sd(...)    INSTR_(vfmadd213sd, __VA_ARGS__)
#define vfmadd213ss(...)    INSTR_(vfmadd213ss, __VA_ARGS__)
#define vfmadd231pd(...)    INSTR_(vfmadd231pd, __VA_ARGS__)
#define vfmadd231sd(...)    INSTR_(vfmadd231sd, __VA_ARGS__)
#define vfmadd231ss(...)    INSTR_(vfmadd231ss, __VA_ARGS__)
#define vfmsub132sd(...)    INSTR_(vfmsub132sd, __VA_ARGS__)
#define vfmsub213pd(...)    INSTR_(vfmsub213pd, __VA_ARGS__)
#define vfmsub213sd(...)    INSTR_(vfmsub213sd, __VA_ARGS__)
#define vfmsub231pd(...)    INSTR_(vfmsub231pd, __VA_ARGS__)
#define vfmsub231sd(...)    INSTR_(vfmsub231sd, __VA_ARGS__)
#define vfnmadd213pd(...)   INSTR_(vfnmadd213pd, __VA_ARGS__)
#define vfnmadd213sd(...)   INSTR_(vfnmadd213sd, __VA_ARGS__)
#define vfnmadd231pd(...)   INSTR_(vfnmadd231pd, __VA_ARGS__)
#define vfnmadd231sd(...)   INSTR_(vfnmadd231sd, __VA_ARGS__)
#define vinsertf128(...)    INSTR_(vinsertf128, __VA_ARGS__)
#define vmaxpd(...)         INSTR_(vmaxpd, __VA_ARGS__)
#define vmaxps(...)         INSTR_(vmaxps, __VA_ARGS__)
#define vmaxsd(...)         INSTR_(vmaxsd, __VA_ARGS__)
#define vmaxss(...)         INSTR_(vmaxss, __VA_ARGS__)
#define vminpd(...)         INSTR_(vminpd, __VA_ARGS__)
#define vminps(...)         INSTR_(vminps, __VA_ARGS__)
#define vminsd(...)         INSTR_(vminsd, __VA_ARGS__)
#define vminss(...)         INSTR_(vminss, __VA_ARGS__)
#define vmovapd(...)        INSTR_(vmovapd, __VA_ARGS__)
#define vmovaps(...)        INSTR_(vmovaps, __VA_ARGS__)
#define vmovd(...)          INSTR_(vmovd, __VA_ARGS__)
#define vmovddup(...)       INSTR_(vmovddup, __VA_ARGS__)
#define vmovdqa(...)        INSTR_(vmovdqa, __VA_ARGS__)
#define vmovdqu(...)        INSTR_(vmovdqu, __VA_ARGS__)
#define vmovhlps(...)       INSTR_(vmovhlps, __VA_ARGS__)
#define vmovhpd(...)        INSTR_(vmovhpd, __VA_ARGS__)
#define vmovlhps(...)       INSTR_(vmovlhps, __VA_ARGS__)
#define vmovlpd(...)        INSTR_(vmovlpd, __VA_ARGS__)
#define vmovmskpd(...)      INSTR_(vmovmskpd, __VA_ARGS__)
#define vmovmskps(...)      INSTR_(vmovmskps, __VA_ARGS__)
#define vmovq(...)          INSTR_(vmovq, __VA_ARGS__)
#define vmovsd(...)         INSTR_(vmovsd, __VA_ARGS__)
#define vmovss(...)         INSTR_(vmovss, __VA_ARGS__)
#define vmovupd(...)        INSTR_(vmovupd, __VA_ARGS__)
#define vmovups(...)        INSTR_(vmovups, __VA_ARGS__)
#define vmulpd(...)         INSTR_(vmulpd, __VA_ARGS__)
#define vmulps(...)         INSTR_(vmulps, __VA_ARGS__)
#define vmulsd(...)         INSTR_(vmulsd, __VA_ARGS__)
#define vmulss(...)         INSTR_(vmulss, __VA_ARGS__)
#define vorpd(...)          INSTR_(vorpd, __VA_ARGS__)
#define vorps(...)          INSTR_(vorps, __VA_ARGS__)
#define vperm2f128(...)     INSTR_(vperm2f128, __VA_ARGS__)
#define vpermilpd(...)      INSTR_(vpermilpd, __VA_ARGS__)
#define vpermilps(...)      INSTR_(vpermilps, __VA_ARGS__)
#define vptest(...)         INSTR_(vptest, __VA_ARGS__)
#define vshufpd(...)        INSTR_(vshufpd, __VA_ARGS__)
#define vshufps(...)        INSTR_(vshufps, __VA_ARGS__)
#define vsubpd(...)         INSTR_(vsubpd, __VA_ARGS__)
#define vsubps(...)         INSTR_(vsubps, __VA_ARGS__)
#define vsubsd(...)         INSTR_(vsubsd, __VA_ARGS__)
#define vsubss(...)         INSTR_(vsubss, __VA_ARGS__)
#define vucomisd(...)       INSTR_(vucomisd, __VA_ARGS__)
#define vucomiss(...)       INSTR_(vucomiss, __VA_ARGS__)
#define vunpckhpd(...)      INSTR_(vunpckhpd, __VA_ARGS__)
#define vunpckhps(...)      INSTR_(vunpckhps, __VA_ARGS__)
#define vunpcklpd(...)      INSTR_(vunpcklpd, __VA_ARGS__)
#define vunpcklps(...)      INSTR_(vunpcklps, __VA_ARGS__)
#define vxorpd(...)         INSTR_(vxorpd, __VA_ARGS__)
#define vxorps(...)         INSTR_(vxorps, __VA_ARGS__)

/* --- AVX2 (VEX-encoded 256-bit integer SIMD) --------------------- */
#define vextracti128(...) INSTR_(vextracti128, __VA_ARGS__)
#define vinserti128(...)  INSTR_(vinserti128, __VA_ARGS__)
#define vpaddd(...)       INSTR_(vpaddd, __VA_ARGS__)
#define vpaddq(...)       INSTR_(vpaddq, __VA_ARGS__)
#define vpand(...)        INSTR_(vpand, __VA_ARGS__)
#define vpandn(...)       INSTR_(vpandn, __VA_ARGS__)
#define vpcmpeqd(...)     INSTR_(vpcmpeqd, __VA_ARGS__)
#define vpcmpeqq(...)     INSTR_(vpcmpeqq, __VA_ARGS__)
#define vpcmpgtd(...)     INSTR_(vpcmpgtd, __VA_ARGS__)
#define vpcmpgtq(...)     INSTR_(vpcmpgtq, __VA_ARGS__)
#define vpmovmskb(...)    INSTR_(vpmovmskb, __VA_ARGS__)
#define vpmovsxdq(...)    INSTR_(vpmovsxdq, __VA_ARGS__)
#define vpmovzxdq(...)    INSTR_(vpmovzxdq, __VA_ARGS__)
#define vpor(...)         INSTR_(vpor, __VA_ARGS__)
#define vpshufd(...)      INSTR_(vpshufd, __VA_ARGS__)
#define vpshufhw(...)     INSTR_(vpshufhw, __VA_ARGS__)
#define vpshuflw(...)     INSTR_(vpshuflw, __VA_ARGS__)
#define vpslld(...)       INSTR_(vpslld, __VA_ARGS__)
#define vpslldq(...)      INSTR_(vpslldq, __VA_ARGS__)
#define vpsllq(...)       INSTR_(vpsllq, __VA_ARGS__)
#define vpsrad(...)       INSTR_(vpsrad, __VA_ARGS__)
#define vpsraq(...)       INSTR_(vpsraq, __VA_ARGS__)
#define vpsrld(...)       INSTR_(vpsrld, __VA_ARGS__)
#define vpsrldq(...)      INSTR_(vpsrldq, __VA_ARGS__)
#define vpsrlq(...)       INSTR_(vpsrlq, __VA_ARGS__)
#define vpsubd(...)       INSTR_(vpsubd, __VA_ARGS__)
#define vpsubq(...)       INSTR_(vpsubq, __VA_ARGS__)
#define vpunpckhdq(...)   INSTR_(vpunpckhdq, __VA_ARGS__)
#define vpunpckhqdq(...)  INSTR_(vpunpckhqdq, __VA_ARGS__)
#define vpunpckldq(...)   INSTR_(vpunpckldq, __VA_ARGS__)
#define vpunpcklqdq(...)  INSTR_(vpunpcklqdq, __VA_ARGS__)
#define vpxor(...)        INSTR_(vpxor, __VA_ARGS__)

/* zero-operand VEX instructions */
#define vzeroall   INSTR_0_(vzeroall)
#define vzeroupper INSTR_0_(vzeroupper)


#endif /* __LIBM_ASM_MACROS_H__ */
