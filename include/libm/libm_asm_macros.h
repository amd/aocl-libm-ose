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

/* %= expands to a unique integer per asm() instance so labels inside
 * inlined copies do not collide at link time. */
#  define LABEL_(name)             ".L" STRINGIFY_(name) "%="

#  define label(name)                LABEL_(name) ":\n\t"
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

#define jmp(t)  JMP_(jmp,  t)
#define jnz(t)  JMP_(jnz,  t)
#define jz(t)   JMP_(jz,   t)
#define je(t)   JMP_(je,   t)
#define jne(t)  JMP_(jne,  t)
#define jl(t)   JMP_(jl,   t)
#define jle(t)  JMP_(jle,  t)
#define jg(t)   JMP_(jg,   t)
#define jge(t)  JMP_(jge,  t)
#define jc(t)   JMP_(jc,   t)
#define jnc(t)  JMP_(jnc,  t)
#define jb(t)   JMP_(jb,   t)
#define jae(t)  JMP_(jae,  t)
#define jbe(t)  JMP_(jbe,  t)
#define ja(t)   JMP_(ja,   t)

/* --- 64-bit GPRs ----------------------------------------------------- */
#define RAX REGISTER_(rax)
#define RBX REGISTER_(rbx)
#define RCX REGISTER_(rcx)
#define RDX REGISTER_(rdx)
#define RSI REGISTER_(rsi)
#define RDI REGISTER_(rdi)
#define RBP REGISTER_(rbp)
#define RSP REGISTER_(rsp)
#define RIP REGISTER_(rip)
#define R8  REGISTER_(r8)
#define R9  REGISTER_(r9)
#define R10 REGISTER_(r10)
#define R11 REGISTER_(r11)
#define R12 REGISTER_(r12)
#define R13 REGISTER_(r13)
#define R14 REGISTER_(r14)
#define R15 REGISTER_(r15)

#define rax RAX
#define rbx RBX
#define rcx RCX
#define rdx RDX
#define rsi RSI
#define rdi RDI
#define rbp RBP
#define rsp RSP
#define rip RIP
#define r8  R8
#define r9  R9
#define r10 R10
#define r11 R11
#define r12 R12
#define r13 R13
#define r14 R14
#define r15 R15

/* --- 32-bit / 8-bit sub-GPRs we actually use ------------------------- */
#define EAX REGISTER_(eax)
#define EBX REGISTER_(ebx)
#define ECX REGISTER_(ecx)
#define EDX REGISTER_(edx)
#define EDI REGISTER_(edi)
#define AX  REGISTER_(ax)
#define CL  REGISTER_(cl)

#define eax EAX
#define ebx EBX
#define ecx ECX
#define edx EDX
#define edi EDI
#define ax  AX
#define cl  CL

/* --- XMM registers --------------------------------------------------- */
#define XMM0  REGISTER_(xmm0)
#define XMM1  REGISTER_(xmm1)
#define XMM2  REGISTER_(xmm2)
#define XMM3  REGISTER_(xmm3)
#define XMM4  REGISTER_(xmm4)
#define XMM5  REGISTER_(xmm5)
#define XMM6  REGISTER_(xmm6)
#define XMM7  REGISTER_(xmm7)
#define XMM8  REGISTER_(xmm8)
#define XMM9  REGISTER_(xmm9)
#define XMM10 REGISTER_(xmm10)
#define XMM11 REGISTER_(xmm11)
#define XMM12 REGISTER_(xmm12)
#define XMM13 REGISTER_(xmm13)
#define XMM14 REGISTER_(xmm14)
#define XMM15 REGISTER_(xmm15)

#define xmm0  XMM0
#define xmm1  XMM1
#define xmm2  XMM2
#define xmm3  XMM3
#define xmm4  XMM4
#define xmm5  XMM5
#define xmm6  XMM6
#define xmm7  XMM7
#define xmm8  XMM8
#define xmm9  XMM9
#define xmm10 XMM10
#define xmm11 XMM11
#define xmm12 XMM12
#define xmm13 XMM13
#define xmm14 XMM14
#define xmm15 XMM15

/* --- Integer instructions -------------------------------------------
 * `and`/`or`/`xor`/`not` are C++ alternative-token keywords, so we use
 * the GAS suffix-q forms (`andq` etc.) which are legal identifiers in
 * both C and C++.  The MNEM_*_ aliases (defined per-backend) map these
 * to the correct assembler mnemonic (GAS keeps the suffix; MASM drops
 * it because operand size is inferred from register width). */
#define mov(s, d)     INSTR_(mov,  s, d)
#define movl(s, d)    INSTR_(MNEM_movl_, s, d)
#define lea(s, d)     INSTR_(lea,  s, d)
#define push(r)       INSTR_(push, r)
#define pop(r)        INSTR_(pop,  r)
#define add(s, d)     INSTR_(add,  s, d)
#define sub(s, d)     INSTR_(sub,  s, d)
#define adc(s, d)     INSTR_(adc,  s, d)
#define sbb(s, d)     INSTR_(sbb,  s, d)
#define andq(s, d)    INSTR_(MNEM_andq_, s, d)
#define orq(s, d)     INSTR_(MNEM_orq_,  s, d)
#define xorq(s, d)    INSTR_(MNEM_xorq_, s, d)
#define shl(s, d)     INSTR_(shl,  s, d)
#define shr(s, d)     INSTR_(shr,  s, d)
#define neg(r)        INSTR_(neg,  r)
#define notq(r)       INSTR_(MNEM_notq_, r)
#define cmp(s, d)     INSTR_(cmp,  s, d)
#define test(s, d)    INSTR_(test, s, d)
#define bts(s, d)     INSTR_(bts,  s, d)
#define btr(s, d)     INSTR_(btr,  s, d)
#define bsr(s, d)     INSTR_(bsr,  s, d)
#define ror(s, d)     INSTR_(ror,  s, d)
#define andw(s, d)    INSTR_(MNEM_andw_, s, d)
#define call_fn(fn)   INSTR_(call, SYM_(fn))
/* INSTR_(ret) would pass an empty __VA_ARGS__ to GET_MACRO_; use INSTR_0_. */
#define ret           INSTR_0_(ret)

/* --- BMI2 (3-operand non-flag-clobbering) ---------------------------- */
#define mulx(s, l, h) INSTR_(mulx, s, l, h)
#define bzhi(i, s, d) INSTR_(bzhi, i, s, d)
#define shrx(i, s, d) INSTR_(shrx, i, s, d)
#define shlx(i, s, d) INSTR_(shlx, i, s, d)

/* --- FP / SIMD instructions ---------------------------------------- */
#define vmovq(s, d)              INSTR_(vmovq,  s, d)
#define vmovsd(s, d)             INSTR_(vmovsd, s, d)
#define vmovapd(s, d)            INSTR_(vmovapd, s, d)
#define vmulsd(a, b, d)          INSTR_(vmulsd, a, b, d)
#define vaddsd(a, b, d)          INSTR_(vaddsd, a, b, d)
#define vsubsd(a, b, d)          INSTR_(vsubsd, a, b, d)
#define vfmadd231sd(a, b, d)     INSTR_(vfmadd231sd, a, b, d)
#define vfmsub231sd(a, b, d)     INSTR_(vfmsub231sd, a, b, d)

#define movd(s, d)               INSTR_(movd,   s, d)
#define movdqu(s, d)             INSTR_(movdqu, s, d)
#define movdqa(s, d)             INSTR_(movdqa, s, d)
#define movsd(s, d)              INSTR_(movsd,  s, d)
#define movhlps(s, d)            INSTR_(movhlps, s, d)
#define movlhps(s, d)            INSTR_(movlhps, s, d)
#define mulq(s)                  INSTR_(MNEM_mulq_,   s)
#define mulpd(s, d)              INSTR_(mulpd,  s, d)
#define psrldq(s, d)             INSTR_(psrldq, s, d)
#define psrlq(s, d)              INSTR_(psrlq,  s, d)
#define psllq(s, d)              INSTR_(psllq,  s, d)
#define mulsd(s, d)              INSTR_(mulsd,  s, d)
#define addsd(s, d)              INSTR_(addsd,  s, d)
#define subsd(s, d)              INSTR_(subsd,  s, d)
#define divsd(s, d)              INSTR_(divsd,  s, d)
#define comisd(s, d)             INSTR_(comisd, s, d)
#define movapd(s, d)             INSTR_(movapd, s, d)
#define pand(s, d)               INSTR_(pand,   s, d)
#define por(s, d)                INSTR_(por,    s, d)
#define andpd(s, d)              INSTR_(andpd,  s, d)
#define orpd(s, d)               INSTR_(orpd,   s, d)
#define cvttsd2siq(s, d)         INSTR_(MNEM_cvttsd2siq_, s, d)
#define cvtsi2sdq(s, d)          INSTR_(MNEM_cvtsi2sdq_, s, d)

/* 64-bit GPR <-> XMM moves and 64-bit immediate load. */
#define movq(s, d)               INSTR_(movq, s, d)
#define movabs(s, d)             INSTR_(MNEM_movabs_, s, d)

#if defined(_MSC_VER) && !defined(__clang__)
#define ALIGN16_USED __declspec(align(16))
#else
#define ALIGN16_USED __attribute__((aligned(16), used))
#endif

#define ffree_st0  STRINGIFY_(ffree %%st(0)) "\n\t"
#define ffree_st1  STRINGIFY_(ffree %%st(1)) "\n\t"
#define fldl_(m)   "fldl " m "\n\t"
#define fstpl_(m)  "fstpl " m "\n\t"
#define fnclex_()  STRINGIFY_(fnclex) "\n\t"
#define fprem1_()  STRINGIFY_(fprem1) "\n\t"
#define fprem_()   STRINGIFY_(fprem) "\n\t"
#define fnstsw_(d) "fnstsw " d "\n\t"

#endif /* __LIBM_ASM_MACROS_H__ */
