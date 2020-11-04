/*
 * Copyright (C) 2008-2020 Advanced Micro Devices, Inc. All rights reserved.
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

#############  Vector float    ymm0=r input  ymm0=output ##############
.macro  sin_piby4_s4f_fma3

    VMOVAPD .L_Sin+0x40(%rip),%ymm4
    VMULPD      %ymm0,%ymm0,%ymm3                   #x * x  x2 ymm3
    VFMADD231PD .L_Sin+0x60(%rip),%ymm3,%ymm4
    VFMADD213PD .L_Sin+0x20(%rip),%ymm3,%ymm4
    VFMADD213PD .L_Sin(%rip),%ymm3,%ymm4
    VMULPD    %ymm3,%ymm0,%ymm3                 #x2 * x x3  ymm4
    VFMADD231PD  %ymm3,%ymm4,%ymm0
    .endm

.macro   cos_piby4_s4f_fma3

    VMOVAPD .L_one(%rip),%ymm2
    VMULPD      %ymm0,%ymm0,%ymm3                    # x * x ymm3
    VMULPD      .L_point_five(%rip),%ymm3,%ymm4      # r ymm4
    VSUBPD      %ymm4,%ymm2,%ymm2
    VMOVAPD .L_Cos+0x40(%rip),%ymm4
    VFMADD231PD .L_Cos+0x60(%rip),%ymm3,%ymm4
    VFMADD213PD .L_Cos+0x20(%rip),%ymm3,%ymm4
    VFMADD213PD .L_Cos(%rip),%ymm3,%ymm4
    VMULPD  %ymm3,%ymm3,%ymm3
    VFMADD213PD  %ymm2,%ymm3,%ymm4
    VMOVDQA %ymm4, %ymm0
    .endm

.macro  tan_piby4_s4f_fma3

    VMULPD  %ymm0,%ymm0,%ymm3                   # x * x ymm3 (r)
    VMOVAPD .L_tanf+0x20(%rip),%ymm4
    VFNMADD213PD .L_tanf(%rip),%ymm3,%ymm4
    VMOVAPD .L_tanf+0x80(%rip),%ymm5
    VFMADD213PD .L_tanf+0x60(%rip),%ymm3,%ymm5
    VFMADD213PD .L_tanf+0x40(%rip),%ymm3,%ymm5
    VDIVPD  %ymm5,%ymm4,%ymm4
    VMULPD  %ymm0,%ymm3,%ymm3
    VFMADD231PD %ymm4,%ymm3,%ymm0
    VEXTRACTF128 $1,%ymm2,%xmm3
    VPCMPEQQ .L_int_one(%rip),%xmm2,%xmm2      # recp mask ymm2
    VPCMPEQQ .L_int_one(%rip),%xmm3,%xmm3      # recp mask ymm2
    VINSERTF128 $1,%xmm3,%ymm2,%ymm2
    VMOVAPD .L_n_one(%rip),%ymm1
    VDIVPD  %ymm0,%ymm1,%ymm1
    VANDNPD %ymm0, %ymm2, %ymm3
    VANDPD %ymm2, %ymm1, %ymm0
    VORPD %ymm3, %ymm0, %ymm0
    .endm

############# End  vector float ##############


#############  scalar float   xmm0=r input  xmm0=output ##############

.macro  sin_piby4_sf_fma3

	VMOVAPD .L_Sin+0x40(%rip),%xmm1
	VMULSD      %xmm0,%xmm0,%xmm3                   #x * x  x2 xmm3
	VFMADD231SD .L_Sin+0x60(%rip),%xmm3,%xmm1
	VFMADD213SD .L_Sin+0x20(%rip),%xmm3,%xmm1
	VFMADD213SD .L_Sin(%rip),%xmm3,%xmm1
	VMULSD    %xmm3,%xmm0,%xmm3                 #x2 * x x3  xmm4
	VFMADD231SD  %xmm3,%xmm1,%xmm0
    .endm

.macro  cos_piby4_sf_fma3

	VMOVAPD .L_one(%rip),%xmm2
	VMULSD      %xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULSD      .L_point_five(%rip),%xmm3,%xmm1      # r xmm4
	VSUBSD      %xmm1,%xmm2,%xmm2
	VMOVAPD .L_Cos+0x40(%rip),%xmm1
	VFMADD231SD .L_Cos+0x60(%rip),%xmm3,%xmm1
	VFMADD213SD .L_Cos+0x20(%rip),%xmm3,%xmm1
	VFMADD213SD .L_Cos(%rip),%xmm3,%xmm1
	VMULSD  %xmm3,%xmm3,%xmm3
	VMOVDQA %xmm2, %xmm0
	VFMADD231SD %xmm3,%xmm1,%xmm0
    .endm


.macro  sincosf_piby4_s_fma3

    VUNPCKLPD   %xmm0,%xmm0,%xmm0

    VMOVAPD .L_one(%rip),%xmm2
    VMULPD      %xmm0,%xmm0,%xmm3                    # x * x xmm3
    VMULSD      .L_point_five(%rip),%xmm3,%xmm4      # 0.5 * x2
    VSUBSD      %xmm4,%xmm2,%xmm2                    #1.0 - 0.5 * x2

    VMOVAPD .L_Sincos+0x20(%rip),%xmm4

    VFMADD231PD .L_Sincos+0x30(%rip),%xmm3,%xmm4
    VFMADD213PD .L_Sincos+0x10(%rip),%xmm3,%xmm4
    VFMADD213PD .L_Sincos(%rip),%xmm3,%xmm4          #(c1 + x2 * (c2 + x2 * (c3 + x2 * c4)

    VUNPCKHPD   %xmm4,%xmm4,%xmm6
    #sin
    VMULSD  %xmm3,%xmm0,%xmm5             #x * x2
    VFMADD231SD  %xmm5,%xmm4,%xmm0


    VMULSD  %xmm3,%xmm3,%xmm3
    VFMADD213SD  %xmm2,%xmm3,%xmm6
    VMOVDQA %xmm6, %xmm1

  .endm


.macro tan_piby4_sf_zeor_fma3


  VMULSD %xmm0,%xmm0,%xmm1 # x * x xmm1 (r)
  VMOVSD .L_tanf+0x20(%rip),%xmm3
  VFNMADD213SD .L_tanf(%rip),%xmm1,%xmm3
  VMOVSD .L_tanf+0x80(%rip),%xmm2
  VFMADD213SD .L_tanf+0x60(%rip),%xmm1,%xmm2
  VFMADD213SD .L_tanf+0x40(%rip),%xmm1,%xmm2
  VDIVSD %xmm2,%xmm3,%xmm3
  VMULSD %xmm0,%xmm1,%xmm1
  VFMADD231SD %xmm3,%xmm1,%xmm0

  .endm



.macro   tan_piby4_sf_fma3

    VMULSD  %xmm0,%xmm0,%xmm1                  # x * x xmm3 (r)
    VMOVSD  .L_tanf+0x20(%rip),%xmm3
    VFNMADD213SD .L_tanf(%rip),%xmm1,%xmm3
    VMOVSD .L_tanf+0x80(%rip),%xmm2
    VFMADD213SD .L_tanf+0x60(%rip),%xmm1,%xmm2
    VFMADD213SD .L_tanf+0x40(%rip),%xmm1,%xmm2
    VDIVSD  %xmm2,%xmm3,%xmm3
    VMULSD  %xmm0,%xmm1,%xmm1
    VFMADD231SD %xmm3,%xmm1,%xmm0
  cmp  $0x1,%eax
  je   tan_recip
  jmp  exit_tanpiby4

tan_recip :
  VMOVQ .L_n_one(%rip),%xmm3
  VDIVSD %xmm0,%xmm3,%xmm0

exit_tanpiby4 :

    .endm

############# End  scalar float ##############


#############  vector double   xmm0=r xmm1=rr input  xmm3=recp xmm0=output ##############

.macro   cos_2fpiby4_s_fma3


    VMOVAPD .L_Cos+0x80(%rip),%xmm5
    VMOVAPD .L_one(%rip),%xmm2
    VMULPD      %xmm0,%xmm0,%xmm3                    # x * x xmm3
    VMULPD      .L_point_five(%rip),%xmm3,%xmm4      # r xmm4
    VSUBPD      %xmm4,%xmm2,%xmm6                    # t xmm6
    VSUBPD      %xmm6,%xmm2,%xmm2                    # 1-t
    VSUBPD      %xmm4,%xmm2,%xmm2                    #  (1-t) -r xmm2
    VFNMADD231PD    %xmm1,%xmm0,%xmm2                # (1.0 - t) - r) - x * xx) xmm2
    VMULPD      %xmm3,%xmm3,%xmm4                    # x2 * x2 xmm4
    VFMADD231PD .L_Cos+0xA0(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Cos+0x60(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Cos+0x40(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Cos+0x20(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Cos(%rip),%xmm3,%xmm5
    VFMADD213PD %xmm2,%xmm4,%xmm5
    VADDPD      %xmm6,%xmm5,%xmm0
    .endm


.macro   sin_2fpiby4_s_xx_zero_fma3

    VMOVAPD .L_Sin+0x80(%rip),%xmm5
    VMULPD      %xmm0,%xmm0,%xmm3                   #x * x  x2 xmm3

    VFMADD231PD .L_Sin+0xA0(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x60(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x40(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x20(%rip),%xmm3,%xmm5   #xmm5 r

    VMULPD      %xmm3,%xmm0,%xmm4                   #x2 * x x3  xmm4
    VFMADD213PD .L_Sin(%rip), %xmm3, %xmm5
    VFMADD231PD %xmm5, %xmm4, %xmm0

    .endm





.macro   sin_2fpiby4_s_fma3

    VMOVAPD .L_Sin+0x80(%rip),%xmm5
    VMULPD      %xmm0,%xmm0,%xmm3                   #x * x  x2 xmm3

    VFMADD231PD .L_Sin+0xA0(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x60(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x40(%rip),%xmm3,%xmm5
    VFMADD213PD .L_Sin+0x20(%rip),%xmm3,%xmm5   #xmm5 r

    VMULPD      %xmm3,%xmm0,%xmm4                   #x2 * x x3  xmm4

    VMULPD  %xmm5,%xmm4,%xmm2                       # xx,-(x3 * r)
    VMULPD  .L_point_five(%rip),%xmm1,%xmm5         # (0.5*xx)-(x3 * r)
    VSUBPD  %xmm2,%xmm5,%xmm2
    VMULPD %xmm2,%xmm3,%xmm2
    VSUBPD %xmm1,%xmm2,%xmm2
    VFNMADD231PD .L_Sin(%rip),%xmm4,%xmm2
    VSUBPD  %xmm2,%xmm0,%xmm0                       #res2 %xmm2
    .endm


.macro   tan_2fpiby4_s_fma3


    VCMPGTPD .L_point_68(%rip),%xmm0,%xmm2
    VCMPLTPD .L_n_point_68(%rip),%xmm0,%xmm9
    VANDPD  .L_signbit(%rip),%xmm2,%xmm15
    VXORPD  %xmm15,%xmm0,%xmm5               # x-
    VXORPD  %xmm15,%xmm1,%xmm6               # xx-
    VANDPD  .L_signbit(%rip),%xmm9,%xmm15    # transform -1 if xmm9 set
    VADDPD  .L_piby4_lead(%rip),%xmm5,%xmm5
    VADDPD  .L_piby4_tail(%rip),%xmm6,%xmm6
    VADDPD  %xmm6,%xmm5,%xmm5               # x
    VORPD   %xmm2,%xmm9,%xmm8               # transform mask xmm8
    VANDNPD %xmm0, %xmm8, %xmm9
    VANDPD %xmm8, %xmm5, %xmm0

    VORPD %xmm9, %xmm0, %xmm0
    VXORPD  %xmm9,%xmm9,%xmm9
    VANDNPD %xmm1, %xmm8, %xmm4
    VANDPD %xmm8, %xmm9, %xmm1
    VORPD %xmm4, %xmm1, %xmm1

    VMULPD  %xmm0,%xmm0,%xmm9               # x2 xmm9
    VMULPD  %xmm0,%xmm1,%xmm4        # x * xx xmm4
    VFMADD231PD .L_two(%rip),%xmm4,%xmm9       # xmm9 r
    VMOVDQA %xmm9, %xmm2               # xmm2 r
    VMOVAPD  .L_tan_c3(%rip),%xmm9



    VFMADD213PD .L_tan_c2(%rip),%xmm2,%xmm9
    VFMADD213PD .L_tan_c1(%rip),%xmm2,%xmm9
 	VMOVAPD   .L_tan_c7(%rip),%xmm4
    VFNMADD213PD .L_tan_c6(%rip),%xmm2,%xmm4
    VFMADD213PD .L_tan_c5(%rip),%xmm2,%xmm4
    VFMADD213PD .L_tan_c4(%rip),%xmm2,%xmm4
    VDIVPD  %xmm4,%xmm9,%xmm9
    VMULPD  %xmm2,%xmm9,%xmm9
    VFMADD213PD %xmm1,%xmm0,%xmm9           # t2
    VADDPD      %xmm9,%xmm0,%xmm1               # t = t1+ t2 xmm1 res3
    VMULPD      .L_two(%rip),%xmm1,%xmm6
    VSUBPD      .L_one(%rip),%xmm1,%xmm4
    VDIVPD      %xmm4,%xmm6,%xmm5
    VSUBPD      .L_one(%rip),%xmm5,%xmm5
    VXORPD      %xmm5,%xmm15,%xmm5
    VADDPD      .L_one(%rip),%xmm1,%xmm4
    VDIVPD      %xmm4,%xmm6,%xmm6
    VMOVAPD     .L_one(%rip),%xmm4
    VSUBPD      %xmm6,%xmm4,%xmm6
    VXORPD      %xmm6,%xmm15,%xmm6
    VANDPD  .L_half_mask(%rip),%xmm1,%xmm2      # xmm2 z1
    VSUBPD  %xmm0,%xmm2,%xmm4
    VSUBPD  %xmm4,%xmm9,%xmm4                   # xmm4 z2
    VMOVAPD .L_n_one(%rip),%xmm9
    VDIVPD   %xmm1,%xmm9,%xmm9                  # xmm9 trec
    VANDPD  .L_half_mask(%rip),%xmm9,%xmm15      # xmm7 trec_top
    VFMADD213PD .L_one(%rip),%xmm15,%xmm2        #
    VFMADD231PD  %xmm15,%xmm4,%xmm2          #
    VFMADD213PD  %xmm15,%xmm9,%xmm2          # res2

    VPCMPEQQ .L_int_one(%rip),%xmm3,%xmm3      # recp mask xmm11
    VANDNPD %xmm6, %xmm3, %xmm15
    VANDPD %xmm3, %xmm5, %xmm6
    VORPD %xmm15, %xmm6, %xmm6
    VANDNPD %xmm1, %xmm3, %xmm15
    VANDPD %xmm3, %xmm2, %xmm2
    VORPD %xmm15, %xmm2, %xmm2
    VANDNPD %xmm2, %xmm8, %xmm15
    VANDPD %xmm8, %xmm6, %xmm0
    VORPD %xmm15, %xmm0, %xmm0

    .endm

############# End  vector double ##############


#############  scalar double   xmm0=r xmm1=rr input  xmm0=output ##############
.macro   cos_piby4_s_fma3


	VMOVAPD   .L_one(%rip),%xmm2
	VMULSD    %xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULSD    .L_point_five(%rip),%xmm3,%xmm5         # r xmm5
	VSUBSD    %xmm5,%xmm2,%xmm4                     # t xmm4
	VSUBSD    %xmm4,%xmm2,%xmm2                    # 1-t
	VSUBSD    %xmm5,%xmm2,%xmm2                     #     (1-t) -r xmm2
	VMOVAPD   .L_Cos+0x80(%rip),%xmm5
	VFNMADD231SD %xmm1,%xmm0,%xmm2                 # (1.0 - t) - r) - x * xx) xmm2
	VMULSD    %xmm3,%xmm3,%xmm1                     # x2 * x2 xmm1
	VFMADD231SD  .L_Cos+0xA0(%rip),%xmm3,%xmm5
	VFMADD213SD  .L_Cos+0x60(%rip),%xmm3,%xmm5
	VFMADD213SD  .L_Cos+0x40(%rip),%xmm3,%xmm5
	VFMADD213SD  .L_Cos+0x20(%rip),%xmm3,%xmm5
	VFMADD213SD  .L_Cos(%rip),%xmm3,%xmm5
	VFMADD213SD  %xmm2,%xmm1,%xmm5
	VADDSD      %xmm4,%xmm5,%xmm0
    .endm


.macro   sin_piby4_s_xx_zero_fma3

    VMOVAPD .L_Sin+0x80(%rip),%xmm5
    VMULSD      %xmm0,%xmm0,%xmm3                   #x * x  x2 xmm3

    VFMADD231SD .L_Sin+0xA0(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x60(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x40(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x20(%rip),%xmm3,%xmm5   #xmm5 r

    VMULSD      %xmm3,%xmm0,%xmm4                   #x2 * x x3  xmm4
    VFMADD213SD .L_Sin(%rip),%xmm3,%xmm5
    VFMADD231SD %xmm5,%xmm4,%xmm0               # xmm6 res1

    .endm


.macro   sin_piby4_s_fma3


    VMOVAPD .L_Sin+0x80(%rip),%xmm5
    VMULSD      %xmm0,%xmm0,%xmm3                   #x * x  x2 xmm3
    VFMADD231SD .L_Sin+0xA0(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x60(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x40(%rip),%xmm3,%xmm5
    VFMADD213SD .L_Sin+0x20(%rip),%xmm3,%xmm5   #xmm5 r

    VMULSD      %xmm3,%xmm0,%xmm4                   #x2 * x x3  xmm4
    VMULSD  %xmm5,%xmm4,%xmm2                       # xx,-(x3 * r)
    VMULSD  .L_point_five(%rip),%xmm1,%xmm5         # (0.5*xx)-(x3 * r)
    VSUBSD  %xmm2,%xmm5,%xmm2
    VMULSD %xmm2,%xmm3,%xmm2
    VSUBSD %xmm1,%xmm2,%xmm2
    VFNMADD231SD .L_Sin(%rip),%xmm4,%xmm2
    VSUBSD  %xmm2,%xmm0,%xmm0                       #res2 %xmm2
    .endm


.macro   sincos_piby4_s_fma3

    VUNPCKLPD   %xmm0,%xmm0,%xmm0
    VMULPD      %xmm0,%xmm0,%xmm2                   #x * x  x2 xmm3
    VMOVAPD .L_Sincos+0x40(%rip),%xmm5
    VFMADD231PD .L_Sincos+0x50(%rip),%xmm2,%xmm5
    VFMADD213PD .L_Sincos+0x30(%rip),%xmm2,%xmm5
    VFMADD213PD .L_Sincos+0x20(%rip),%xmm2,%xmm5
    VMOVDQA %xmm2, %xmm7                       #Exchange 2 and 7 till endm
    VFMADD213PD .L_Sincos+0x10(%rip),%xmm5,%xmm7           #xmm7 r
    VMOVDQA %xmm2, %xmm5
    VFMADD213PD .L_Sincos(%rip),%xmm7,%xmm5            #

    #cos
    VUNPCKHPD %xmm5,%xmm5,%xmm5
    VMULSD %xmm2,%xmm2,%xmm8   #  x2 * x2
    VMULSD  .L_point_five(%rip),%xmm2,%xmm4  # r
    VMOVAPD .L_one(%rip),%xmm3
    VSUBSD  %xmm4,%xmm3,%xmm6     # t
    VSUBSD %xmm6,%xmm3,%xmm3          # 1-t
    VSUBSD %xmm4,%xmm3,%xmm3          # 1-t-r
    VFNMADD231SD  %xmm1,%xmm0,%xmm3         # x * xx
    VFMADD213SD  %xmm3,%xmm8,%xmm5    #((((1.0 - t) - r) - x * xx) + x2 * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))))))


    #sin
    VMULSD %xmm2,%xmm0,%xmm3    # x3
    VMULSD %xmm3,%xmm7,%xmm4     #x3 * r
    VFMSUB231SD .L_point_five(%rip),%xmm1,%xmm4  # 0.5 * xx - x3 * r
    VFMSUB213SD  %xmm1,%xmm2,%xmm4   # (x2 * (0.5 * xx - x3 * r) - xx)
    VFNMADD231SD  .L_Sincos(%rip),%xmm3,%xmm4
    VSUBSD  %xmm4,%xmm0,%xmm0    # sin
    VADDSD %xmm6,%xmm5,%xmm1        # cos

    .endm



.macro   sincos_piby4_s_xx_zero_fma3

    VUNPCKLPD   %xmm0,%xmm0,%xmm0
    VMULPD      %xmm0,%xmm0,%xmm2                   #x * x  x2 xmm3
    VMOVAPD .L_Sincos+0x40(%rip),%xmm5
    VFMADD231PD .L_Sincos+0x50(%rip),%xmm2,%xmm5
    VFMADD213PD .L_Sincos+0x30(%rip),%xmm2,%xmm5
    VFMADD213PD .L_Sincos+0x20(%rip),%xmm2,%xmm5
    VFMADD213PD .L_Sincos+0x10(%rip),%xmm2,%xmm5           #xmm7 r
    VMOVDQA %xmm5, %xmm7
    VFMADD213PD .L_Sincos(%rip),%xmm2,%xmm5            #

    #sin
    VMULSD %xmm2,%xmm0,%xmm3    # x3
    VFMADD231SD %xmm3,%xmm5,%xmm0     #x3 * r  x + x3 * (c1 + x2 * r);


    #cos
    VUNPCKHPD %xmm5,%xmm5,%xmm5
    VMULSD %xmm2,%xmm2,%xmm7   #  x2 * x2
    VMULSD  .L_point_five(%rip),%xmm2,%xmm4  # r
    VMOVAPD .L_one(%rip),%xmm3
    VSUBSD  %xmm4,%xmm3,%xmm6     # t
    VSUBSD %xmm6,%xmm3,%xmm3          # 1-t
    VSUBSD %xmm4,%xmm3,%xmm3          # 1-t-r
    VFMADD213SD  %xmm3,%xmm7,%xmm5    #((((1.0 - t) - r) - x * xx) + x2 * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))))))
    VADDSD %xmm6,%xmm5,%xmm1        # cos

    .endm



.macro  tan_piby4_s_fma3

    VCMPGTSD  .L_point_68(%rip),%xmm0,%xmm4
    VCMPLTSD  .L_n_point_68(%rip),%xmm0,%xmm2
    VANDPD  .L_signbit(%rip),%xmm4,%xmm7
    VXORPD  %xmm7,%xmm0,%xmm5               # x-
    VXORPD  %xmm7,%xmm1,%xmm6               # xx-
    VANDPD  .L_signbit(%rip),%xmm2,%xmm7    # transform -1 if xmm9 set
    VADDPD  .L_piby4_lead(%rip),%xmm5,%xmm5
    VADDPD  .L_piby4_tail(%rip),%xmm6,%xmm6
    VADDPD  %xmm6,%xmm5,%xmm5               # x
    VORPD   %xmm4,%xmm2,%xmm8               # transform mask xmm8
    VANDNPD %xmm0, %xmm8, %xmm2
    VANDPD %xmm8, %xmm5, %xmm0
    VORPD %xmm2, %xmm0, %xmm0
    VXORPD  %xmm2,%xmm2,%xmm2
    VANDNPD %xmm1, %xmm8, %xmm4
    VANDPD %xmm8, %xmm2, %xmm1
   VORPD %xmm4, %xmm1, %xmm1
    VMULSD  %xmm0,%xmm0,%xmm2               # x2 xmm2
    VMULSD  %xmm0,%xmm1,%xmm5        # x * xx xmm4
    VFMADD132SD .L_two(%rip),%xmm2,%xmm5       # xmm5 r
    VMOVSD    .L_tan_c3(%rip),%xmm2
    VFMADD213SD .L_tan_c2(%rip),%xmm5,%xmm2
    VFMADD213SD .L_tan_c1(%rip),%xmm5,%xmm2
    VMOVSD   .L_tan_c7(%rip),%xmm4
    VFNMADD213SD .L_tan_c6(%rip),%xmm5,%xmm4
    VFMADD213SD .L_tan_c5(%rip),%xmm5,%xmm4
    VFMADD213SD .L_tan_c4(%rip),%xmm5,%xmm4
    VDIVSD  %xmm4,%xmm2,%xmm2
    VMULSD  %xmm5,%xmm2,%xmm2
    VFMADD213SD %xmm1,%xmm0,%xmm2          # t2
    VADDSD      %xmm2,%xmm0,%xmm1               # t = t1+ t2 xmm1 res3

    VPCMPEQQ .L_int_one(%rip),%xmm3,%xmm3      # recp mask xmm11

    VMOVQ    %xmm8,%rax
     cmp  $0x0,%rax
    JZ if_recip_set


if_transfor_set:

    VMULSD      .L_two(%rip),%xmm1,%xmm6 # 2*t
    VMOVSD     .L_one(%rip),%xmm4

    VSUBSD      %xmm4,%xmm1,%xmm5       # t-1
    VADDSD      %xmm4,%xmm1,%xmm9      # t+1

    VANDNPD %xmm9, %xmm3, %xmm2
    VANDPD %xmm3, %xmm5, %xmm5

    VORPD %xmm2, %xmm5, %xmm5

    VDIVSD     %xmm5,%xmm6,%xmm5            # 2*t/(t-1) or 2*t/(t+1)

    VSUBSD      %xmm5,%xmm4,%xmm6      #(1.0 - 2*t/(1+t)
    VSUBSD      %xmm4,%xmm5,%xmm5       #(2*t/(t-1) - 1.0)

    VANDNPD %xmm6, %xmm3, %xmm2
    VANDPD %xmm3, %xmm5, %xmm6

    VORPD %xmm2,%xmm6, %xmm6

    VXORPD      %xmm6,%xmm7,%xmm6       # res1

    JMP exit_tanpiby4

if_recip_set:

    cmp $0x0,%r8
    JZ exit_tanpiby4

    VANDPD  .L_half_mask(%rip),%xmm1,%xmm7      # xmm7 z1
    VSUBSD  %xmm0,%xmm7,%xmm4

    VSUBSD  %xmm4,%xmm2,%xmm4                   # xmm4 z2
    VMOVSD .L_n_one(%rip),%xmm2

    VDIVSD   %xmm1,%xmm2,%xmm2                  # xmm2 trec
    VANDPD  .L_half_mask(%rip),%xmm2,%xmm5      # xmm7 trec_top

    VFMADD213SD .L_one(%rip),%xmm5,%xmm7        #
    VFMADD231SD  %xmm5,%xmm4,%xmm7          #

    VFMADD213SD  %xmm5,%xmm2,%xmm7          # xmm7 res2

exit_tanpiby4:
    VANDNPD %xmm1, %xmm3, %xmm4
    VANDPD %xmm3, %xmm7, %xmm0
    VORPD %xmm4, %xmm0, %xmm0
    VANDNPD %xmm0, %xmm8, %xmm4
    VANDPD %xmm8, %xmm6, %xmm0
    VORPD %xmm4, %xmm0, %xmm0

    .endm


 .macro  tan_piby4_s_zero_fma3

    VCMPGTSD .L_point_68(%rip),%xmm0,%xmm2
    VCMPLTSD .L_n_point_68(%rip),%xmm0,%xmm3

    VANDPD  .L_signbit(%rip),%xmm2,%xmm7
    VXORPD  %xmm7,%xmm0,%xmm5               # x-

    VANDPD  .L_signbit(%rip),%xmm3,%xmm7    # transform -1 if xmm9 set
    VADDPD  .L_piby4_lead(%rip),%xmm5,%xmm5

    VADDPD  .L_piby4_tail(%rip),%xmm5,%xmm5 # x
    VORPD   %xmm2,%xmm3,%xmm1               # transform mask xmm8
    VANDNPD %xmm0, %xmm1, %xmm3
    VANDPD %xmm1, %xmm5, %xmm0
    VORPD %xmm3, %xmm0, %xmm0
    VMOVQ %xmm1,%rax
    VXORPD  %xmm3,%xmm3,%xmm3
    VMULSD  %xmm0,%xmm0,%xmm2               #  xmm2 r
    VMOVSD  .L_tan_c3(%rip),%xmm3
    VFMADD213SD .L_tan_c2(%rip),%xmm2,%xmm3
    VFMADD213SD .L_tan_c1(%rip),%xmm2,%xmm3
    VMOVAPD   .L_tan_c7(%rip),%xmm4
    VFNMADD213SD .L_tan_c6(%rip),%xmm2,%xmm4
    VFMADD213SD .L_tan_c5(%rip),%xmm2,%xmm4
    VFMADD213SD .L_tan_c4(%rip),%xmm2,%xmm4
    VDIVSD  %xmm4,%xmm3,%xmm3
    VMULSD  %xmm2,%xmm3,%xmm3
    VMULSD  %xmm0,%xmm3,%xmm3           # t2
    VADDSD      %xmm3,%xmm0,%xmm0               # t = t1+ t2 xmm1 res3
    cmp		$0x0,%rax
    JZ ext_tanpiby4_zero
    VMULSD      .L_two(%rip),%xmm0,%xmm6
    VADDSD      .L_one(%rip),%xmm0,%xmm4
    VDIVSD      %xmm4,%xmm6,%xmm6
    VMOVSD       .L_one(%rip),%xmm4
    VSUBSD      %xmm6,%xmm4,%xmm6
    VXORPD      %xmm6,%xmm7,%xmm6
    VANDNPD %xmm0, %xmm1, %xmm7
    VANDPD %xmm1, %xmm6, %xmm0   # xmm0 changed to 1 ##RK
    VORPD %xmm7, %xmm0, %xmm0
ext_tanpiby4_zero:

    .endm


############# End  scalar double ##############




.macro  call_remainder_2d_piby2_fma3

 vmaxpd .L_two(%rip),%xmm0,%xmm0
 lea .L__Pibits(%rip),%r9
 #xexp = (x >> 52) 1023
 movd %xmm0,%r11
 cmp  .L_e_5(%rip),%r11
 jl .L__high_compute

 mov %r11,%rcx
 shr $52,%r11
 sub $1023,%r11 #r11 = xexp = exponent of input x
 #calculate the last byte from which to start multiplication
 #last = 134 (xexp >> 3)
 mov %r11,%r10
 shr $3,%r10
 sub $134,%r10 #r10 = last
 neg %r10 #r10 = last
 #load 64 bits of 2_by_pi
 mov (%r9,%r10),%rax
 #mantissa of x = ((x << 12) >> 12) | implied bit
 shl $12,%rcx
 shr $12,%rcx #rcx = mantissa part of input x
 bts $52,%rcx #add the implied bit as well
 #load next 128 bits of 2_by_pi
 add $8,%r10 #increment to next 8 bytes of 2_by_pi
 movdqu (%r9,%r10),%xmm1
 #do three 64bit multiplications with mant of x
 mul %rcx
 mov %rax,%r8 #r8 = last 64 bits of multiplication = res1[2]
 mov %rdx,%r10 #r10 = carry
 movd %xmm1,%rax
 mul %rcx
 #resexp = xexp & 7
 and $7,%r11 #r11 = resexp = xexp & 7 = last 3 bits
 psrldq $8,%xmm1
 add %r10,%rax # add the previous carry
 adc $0,%rdx
 mov %rax,%r9 #r9 = next 64 bits of multiplication = res1[1]
 mov %rdx,%r10 #r10 = carry
 movd %xmm1,%rax
 mul %rcx
 add %rax,%r10 #r10 = most significant 64 bits = res1[0]
 #find the region
 #last three bits ltb = most sig bits >> (54 resexp)); // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
 #point_five = ltb & 01h;
 #region = ((ltb >> 1) + point_five) & 3;
 mov $54,%rcx
 mov %r10,%rax
 sub %r11,%rcx
 xor %rdx,%rdx #rdx = sign of x(i.e first part of x * 2bypi)
 shr %cl,%rax
 jnc .L__no_point_five
 ##if there is carry.. then negate the result of multiplication
 not %r10
 not %r9
 not %r8
 mov $0x8000000000000000,%rdx

.align 16

 .L__no_point_five:
 adc $0,%rax
 and $3,%rax
 VMOVD %eax,%xmm11 #store region to xmm11
 #calculate the number of integer bits and zero them out
 mov %r11,%rcx
 add $10,%rcx #rcx = no. of integer bits
 shl %cl,%r10
 shr %cl,%r10 #r10 contains only mant bits
 sub $64,%rcx #form the exponent
 mov %rcx,%r11
 #find the highest set bit
 bsr %r10,%rcx
 jnz .L__form_mantissa
 mov %r9,%r10
 mov %r8,%r9
 mov $0,%r8
 bsr %r10,%rcx #rcx = hsb
 sub $64,%r11

.align 16
 .L__form_mantissa:
 add %rcx,%r11 #for exp of x
 sub $52,%rcx #rcx = no. of bits to shift in r10
 cmp $0,%rcx
 jl .L__hsb_below_52
 je .L__form_numbers
 #hsb above 52
 mov %r10,%r8 #previous contents of r8 not required
 shr %cl,%r10 #r10 = mantissa of x with hsb at 52
 shr %cl,%r9 #make space for bits from r10
 sub $64,%rcx
 neg %rcx #rvx = no of bits to shift r10 to move those bits to r9
 shl %cl,%r8
 or %r8,%r9 #r9 = mantissa bits of xx
 jmp .L__form_numbers

 .align 16
 .L__hsb_below_52:
 neg %rcx
 mov %r9,%rax
 shl %cl,%r10
 shl %cl,%r9
 sub $64,%rcx
 neg %rcx
 shr %cl,%rax
 or  %rax,%r10
 shr %cl,%r8
 or  %r8,%r9


 .align 16
 .L__form_numbers:
 add $1023,%r11
 btr $52,%r10 #remove the implied bit
 mov %r11,%rcx
 or %rdx,%r10 #put the sign
 shl $52,%rcx
 or %rcx,%r10 #x is in r10
 movd %r10,%xmm2 #xmm2 = x
 #form xx
 xor %rcx,%rcx
 bsr %r9,%rcx
 sub $64,%rcx #to shift the implied bit as well
 neg %rcx
 shl %cl,%r9
 shr $12,%r9
 add $52,%rcx
 sub %rcx,%r11
 shl $52,%r11
 or %rdx,%r9
 or %r11,%r9
 movd %r9,%xmm1 #xmm1 = xx


 # second part
.L__high_compute:
 lea .L__Pibits(%rip),%r9
 #xexp = (x >> 52) 1023
 vunpckhpd %xmm0,%xmm0,%xmm0
 movd %xmm0,%r11
 cmp  .L_e_5(%rip),%r11
 jl .L__rem_end

 mov %r11,%rcx
 shr $52,%r11
 sub $1023,%r11 #r11 = xexp = exponent of input x
 #calculate the last byte from which to start multiplication
 #last = 134 (xexp >> 3)
 mov %r11,%r10
 shr $3,%r10
 sub $134,%r10 #r10 = last
 neg %r10 #r10 = last
 #load 64 bits of 2_by_pi
 mov (%r9,%r10),%rax
 #mantissa of x = ((x << 12) >> 12) | implied bit
 shl $12,%rcx
 shr $12,%rcx #rcx = mantissa part of input x
 bts $52,%rcx #add the implied bit as well
 #load next 128 bits of 2_by_pi
 add $8,%r10 #increment to next 8 bytes of 2_by_pi
 movdqu (%r9,%r10),%xmm0
 #do three 64bit multiplications with mant of x
 mul %rcx
 mov %rax,%r8 #r8 = last 64 bits of multiplication = res1[2]
 mov %rdx,%r10 #r10 = carry
 movd %xmm0,%rax
 mul %rcx
 #resexp = xexp & 7
 and $7,%r11 #r11 = resexp = xexp & 7 = last 3 bits
 psrldq $8,%xmm0
 add %r10,%rax # add the previous carry
 adc $0,%rdx
 mov %rax,%r9 #r9 = next 64 bits of multiplication = res1[1]
 mov %rdx,%r10 #r10 = carry
 movd %xmm0,%rax
 mul %rcx
 add %rax,%r10 #r10 = most significant 64 bits = res1[0]
 #find the region
 #last three bits ltb = most sig bits >> (54 resexp)); // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
 #point_five = ltb & 01h;
 #region = ((ltb >> 1) + point_five) & 3;
 mov $54,%rcx
 mov %r10,%rax
 sub %r11,%rcx
 xor %rdx,%rdx #rdx = sign of x(i.e first part of x * 2bypi)
 shr %cl,%rax
 jnc .L__no_point_five_2
 # ; if there is carry.. then negate the result of multiplication
 not %r10
 not %r9
 not %r8
 mov $0x8000000000000000,%rdx

 .align 16

 .L__no_point_five_2:
 adc $0,%rax
 and $3,%rax
  VMOVD %eax,%xmm7 #store region to xmm7
 #calculate the number of integer bits and zero them out
 mov %r11,%rcx
 add $10,%rcx #rcx = no. of integer bits
 shl %cl,%r10
 shr %cl,%r10 #r10 contains only mant bits
 sub $64,%rcx #form the exponent
 mov %rcx,%r11
 #find the highest set bit
 bsr %r10,%rcx
 jnz .L__form_mantissa_2
 mov %r9,%r10
 mov %r8,%r9
 mov $0,%r8
 bsr %r10,%rcx #rcx = hsb
 sub $64,%r11

 .align 16
 .L__form_mantissa_2:
 add %rcx,%r11 #for exp of x
 sub $52,%rcx #rcx = no. of bits to shift in r10
 cmp $0,%rcx
 jl .L__hsb_below_52_2
 je .L__form_numbers_2
 #hsb above 52
 mov %r10,%r8 #previous contents of r8 not required
 shr %cl,%r10 #r10 = mantissa of x with hsb at 52
 shr %cl,%r9 #make space for bits from r10
 sub $64,%rcx
 neg %rcx #rvx = no of bits to shift r10 to move those bits to r9
 shl %cl,%r8
 or %r8,%r9 #r9 = mantissa bits of xx
 jmp .L__form_numbers_2

 .align 16
 .L__hsb_below_52_2:
 neg %rcx
 mov %r9,%rax
 shl %cl,%r10
 shl %cl,%r9
 sub $64,%rcx
 neg %rcx
 shr %cl,%rax
 or %rax,%r10
 shr %cl,%r8
 or %r8,%r9


.align 16
 .L__form_numbers_2:
 add $1023,%r11
 btr $52,%r10 #remove the implied bit
 mov %r11,%rcx
 or %rdx,%r10 #put the sign
 shl $52,%rcx
 or %rcx,%r10 #x is in r10
 movd %r10,%xmm3 #xmm2 = x
 #form xx
 xor %rcx,%rcx
 bsr %r9,%rcx
 sub $64,%rcx #to shift the implied bit as well
 neg %rcx
 shl %cl,%r9
 shr $12,%r9
 add $52,%rcx
 sub %rcx,%r11
 shl $52,%r11
 or %rdx,%r9
 or %r11,%r9
 movd %r9,%xmm4 #xmm1 = xx

.L__rem_end:
 vunpcklpd  %xmm4,%xmm1,%xmm1
 vunpcklpd  %xmm7,%xmm11,%xmm11
 vunpcklpd  %xmm3,%xmm2,%xmm2



  # tx
  VANDPD .L_fff800(%rip),%xmm2,%xmm4 # xmm4 hx
  VSUBPD %xmm4,%xmm2,%xmm0 # xmm0 tx
 # c
  VMULPD .L_piby2_lead(%rip),%xmm2,%xmm5 # xmm5 c
 # cc
  VMULPD .L_piby2_part1(%rip),%xmm4,%xmm3
  VSUBPD %xmm5,%xmm3,%xmm3
  VFMADD231PD .L_piby2_part1(%rip),%xmm0,%xmm3
  VFMADD231PD .L_piby2_part2(%rip),%xmm4,%xmm3
  VFMADD231PD .L_piby2_part2(%rip),%xmm0,%xmm3
  VMULPD .L_piby2_lead(%rip),%xmm1,%xmm4
  VFMADD231PD .L_piby2_part3(%rip),%xmm2,%xmm4
  VADDPD %xmm4,%xmm3,%xmm3 # xmm3 cc
  VADDPD %xmm3,%xmm5,%xmm0 # r
  VSUBPD %xmm0,%xmm5,%xmm1
 VADDPD %xmm3,%xmm1,%xmm1 # rr
 .endm



.macro   call_remainder_piby2_fma3

    lea  .L__Pibits(%rip),%r9
    #xexp = (x >> 52) - 1023
    movd %xmm0, %r11
    mov  %r11,  %rcx
    shr  $52,   %r11
    sub  $1023, %r11      #r11 = xexp = exponent of input x
    #calculate the last byte from which to start multiplication
    #last = 134 - (xexp >> 3)
    mov  %r11, %r10
    shr  $3,   %r10
    sub  $134, %r10       #r10 = -last
    neg  %r10             #r10 = last
    #load 64 bits of 2_by_pi
    mov  (%r9,%r10),%rax
    #mov  %rdx, %rdi      # save address of region since mul modifies rdx
    #mantissa of x = ((x << 12) >> 12) | implied bit
    shl  $12,   %rcx
    shr  $12,   %rcx      #rcx = mantissa part of input x
    bts  $52,   %rcx      #add the implied bit as well
    #load next 128 bits of 2_by_pi
    add  $8, %r10        #increment to next 8 bytes of 2_by_pi
    movdqu (%r9, %r10), %xmm0
    #do three 64-bit multiplications with mant of x
    mul  %rcx
    mov  %rax, %r8       #r8 = last 64 bits of multiplication = res1[2]
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0, %rax
    mul  %rcx
    #resexp = xexp & 7
    and  $7, %r11        #r11 = resexp = xexp & 7 = last 3 bits
    psrldq $8, %xmm0
    add  %r10, %rax      # add the previous carry
    adc  $0,   %rdx
    mov  %rax, %r9       #r9 = next 64 bits of multiplication = res1[1]
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0,%rax
    mul  %rcx
    add  %rax, %r10      #r10 = most significant 64 bits = res1[0]
    #find the region
    #last three bits ltb = most sig bits >> (54 - resexp)); // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
    #point_five = ltb & 0x1;
    #region = ((ltb >> 1) + point_five) & 3;
    mov $54,  %rcx
    mov %r10, %rax
    sub %r11, %rcx
    xor %rdx, %rdx       #rdx = sign of x(i.e first part of x * 2bypi)
    shr %cl,  %rax
    jnc  .L__no_point_five
    ##if there is carry.. then negate the result of multiplication
    not  %r10
    not  %r9
    not  %r8
    mov $0x8000000000000000,%rdx

.align 16     # earlier was #.p2align 4

.L__no_point_five:
    adc $0,   %rax
    and $3,   %rax
    VMOVD %eax, %xmm7   #store region to xmm7
    #calculate the number of integer bits and zero them out
    mov %r11, %rcx
    add $10,  %rcx  #rcx = no. of integer bits
    shl %cl,  %r10
    shr %cl,  %r10  #r10 contains only mant bits
    sub $64,  %rcx  #form the exponent
    mov %rcx, %r11
    #find the highest set bit
    bsr %r10,%rcx
    jnz  .L__form_mantissa
    mov %r9,%r10
    mov %r8,%r9
    mov $0, %r8
    bsr %r10,%rcx  #rcx = hsb
    sub $64, %r11

.align 16     # earlier was #.p2align 4
.L__form_mantissa:
    add %rcx,%r11  #for exp of x
    sub $52,%rcx #rcx = no. of bits to shift in r10
    cmp $0,%rcx
    jl  .L__hsb_below_52
    je  .L__form_numbers
    #hsb above 52
    mov %r10,%r8 #previous contents of r8 not required
    shr %cl,%r10 #r10 = mantissa of x with hsb at 52
    shr %cl,%r9  #make space for bits from r10
    sub $64,%rcx
    neg %rcx     #rvx = no of bits to shift r10 to move those bits to r9
    shl %cl,%r8
    or  %r8,%r9  #r9 = mantissa bits of xx
    jmp .L__form_numbers

.align 16     # earlier was #.p2align 4
.L__hsb_below_52:
    neg %rcx
    mov %r9,%rax
    shl %cl,%r10
    shl %cl,%r9
    sub $64,%rcx
    neg %rcx
    shr %cl,%rax
    or  %rax,%r10
    shr %cl,%r8
    or  %r8,%r9


.align 16     # earlier was #.p2align 4
.L__form_numbers:
    add $1023, %r11
    btr $52,%r10     #remove the implied bit
    mov %r11,%rcx
    or  %rdx,%r10    #put the sign
    shl $52, %rcx
    or  %rcx,%r10    #x is in r10
    movd %r10,%xmm2  #xmm8 = x
    #form xx
    xor %rcx,%rcx
    bsr %r9,%rcx
    sub $64, %rcx   #to shift the implied bit as well
    neg %rcx
    shl %cl,%r9
    shr $12,%r9
    add $52,%rcx
    sub %rcx, %r11
    shl $52, %r11
    or  %rdx,%r9
    or  %r11,%r9
    movd %r9,%xmm1  #xmm1 = xx
    # tx
    VANDPD .L_fff800(%rip),%xmm2,%xmm4   # xmm4 hx
    VSUBSD  %xmm4,%xmm2,%xmm0     # xmm6 tx
    # c
    VMULSD  .L_piby2_lead(%rip),%xmm2,%xmm5  # xmm5 c
    # cc
    VMULSD  .L_piby2_part1(%rip),%xmm4,%xmm3
    VSUBSD  %xmm5,%xmm3,%xmm3
    VFMADD231SD  .L_piby2_part1(%rip),%xmm0,%xmm3
    VFMADD231SD  .L_piby2_part2(%rip),%xmm4,%xmm3
    VFMADD231SD  .L_piby2_part2(%rip),%xmm0,%xmm3
    VMULSD  .L_piby2_lead(%rip),%xmm1,%xmm4
    VFMADD231SD  .L_piby2_part3(%rip),%xmm2,%xmm4
    VADDSD  %xmm4,%xmm3,%xmm3   # xmm7 cc
    VADDSD %xmm3,%xmm5,%xmm0        # r
    VSUBSD %xmm0,%xmm5,%xmm1
    VADDSD %xmm3,%xmm1,%xmm1        # rr
    .endm





.macro   range_e_5_2f_s_fma3


    VANDPD .L__sign_mask(%rip),%xmm0,%xmm1          #clear the sign

    VMOVAPD  .L_twobypi(%rip),%xmm8
    VFMADD213PD .L_point_five(%rip),%xmm1,%xmm8
    VCVTTPD2DQ %xmm8,%xmm8
    VPMOVSXDQ %xmm8,%xmm6

    VANDPD  .L_int_three(%rip),%xmm6,%xmm11         # region xmm11
    VSHUFPS $8,%xmm6,%xmm6,%xmm6
    VCVTDQ2PD   %xmm6,%xmm6
    VMOVDQA %xmm6, %xmm2                            #Exchange xmm2 and xmm6
    VFNMADD132PD .L_piby2_1(%rip),%xmm0,%xmm6       # xmm2 rhead
    VMULPD  .L_piby2_1tail(%rip),%xmm2,%xmm3            # xmm3 rtail
    VANDPD .L__inf_mask_64(%rip),%xmm6,%xmm4
    VPSRLQ $52,%xmm4,%xmm4
    VPSRLQ $52,%xmm0,%xmm5
    VPSUBQ  %xmm4,%xmm5,%xmm4                       # expdiff xmm4

    VPCMPGTQ  .L_val_15(%rip),%xmm4,%xmm8          # mask_expdiff15 xmm8
    VPTEST %xmm8,%xmm8
    JZ exit_range_e5

    VMOVAPD %xmm6,%xmm5
    VMULPD  .L_piby2_2(%rip),%xmm2,%xmm3            # rtail_1 xmm3
    VSUBPD  %xmm3,%xmm5,%xmm6                       # rhead_1 xmm2

    VSUBPD  %xmm6,%xmm5,%xmm1
    VSUBPD  %xmm3,%xmm1,%xmm1
    VXORPD  .L_signbit(%rip),%xmm1,%xmm1
    VFMADD231PD  .L_piby2_2tail(%rip),%xmm2,%xmm1   # rtail_1 xmm5
    VMOVDQA %xmm1, %xmm3

    VPCMPGTQ  .L_val_48(%rip),%xmm4,%xmm8          # mask_expdiff15 xmm8
    VPTEST %xmm8,%xmm8
    JZ exit_range_e5

    VMOVAPD %xmm6,%xmm5
    VMULPD  .L_piby2_3(%rip),%xmm2,%xmm3            # rtail_1 xmm3
    VSUBPD  %xmm3,%xmm5,%xmm6                       # rhead_1 xmm2

    VSUBPD  %xmm6,%xmm5,%xmm1
    VSUBPD  %xmm3,%xmm1,%xmm1
    VXORPD  .L_signbit(%rip),%xmm1,%xmm1
    VFMADD231PD  .L_piby2_3tail(%rip),%xmm2,%xmm1   # rtail_1 xmm5
    VMOVDQA %xmm1, %xmm3

exit_range_e5:
    VSUBPD %xmm3,%xmm6,%xmm10                       # r_1  xmm10
    VSUBPD %xmm10,%xmm6,%xmm4
    VSUBPD %xmm3,%xmm4,%xmm1                        # rr_1 xmm4
    VMOVAPD %xmm10,%xmm0

    .endm


.macro   range_e_5_s4f_fma3


    VANDPD .L__sign_mask(%rip),%ymm0,%ymm1          #clear the sign

    VMOVAPD  .L_twobypi(%rip),%ymm8
    VFMADD213PD .L_point_five(%rip),%ymm1,%ymm8
    VCVTTPD2DQ %ymm8,%xmm8
    VPMOVSXDQ %xmm8,%xmm9
    VPUNPCKHQDQ %xmm8,%xmm8,%xmm8
    VPMOVSXDQ %xmm8,%xmm8

    VINSERTF128 $1,%xmm8,%ymm9,%ymm6

    VANDPD  .L_int_three(%rip),%ymm6,%ymm11         # region ymm11
    VEXTRACTF128 $1,%ymm6,%xmm7
    VSHUFPS $8,%xmm6,%xmm6,%xmm6
    VSHUFPS $8,%xmm7,%xmm7,%xmm7
    VPUNPCKLQDQ %xmm7,%xmm6,%xmm6
    VCVTDQ2PD   %xmm6,%ymm6
    VMOVDQA %ymm6, %ymm2                #exchange ymm6 and ymm2
    VFNMADD132PD .L_piby2_1(%rip),%ymm0,%ymm6       # ymm2 rhead
    VMULPD  .L_piby2_1tail(%rip),%ymm2,%ymm3            # ymm3 rtail

    VSUBPD %ymm3,%ymm6,%ymm10                       # r_1  xmm10
    VSUBPD %ymm10,%ymm6,%ymm4
    VSUBPD %ymm3,%ymm4,%ymm1                        # rr_1 xmm4
    VMOVAPD %ymm10,%ymm0


    .endm



.macro   range_e_5_sf_fma3

    VANDPD .L__sign_mask(%rip),%xmm0,%xmm1          #clear the sign
    VMOVAPD  .L_twobypi(%rip),%xmm2
    VFMADD213SD .L_point_five(%rip),%xmm1,%xmm2
    VCVTTPD2DQ %xmm2,%xmm2
    VPMOVSXDQ %xmm2,%xmm1
    VANDPD  .L_int_three(%rip),%xmm1,%xmm4         # region xmm4
    VSHUFPS $8,%xmm1,%xmm1,%xmm1
    VCVTDQ2PD   %xmm1,%xmm1
    VMOVDQA %xmm0, %xmm2                    #exchange xmm0 and xmm2
    VFNMADD231SD .L_piby2_1(%rip),%xmm1,%xmm2       # xmm2 rhead
    VMULSD  .L_piby2_1tail(%rip),%xmm1,%xmm3            # xmm3 rtail
    VSUBSD %xmm3,%xmm2,%xmm0                       # r_1  xmm0
    VSUBSD %xmm0,%xmm2,%xmm2
    VSUBSD %xmm3,%xmm2,%xmm1
    .endm

.macro   range_e_5_s_fma3


    VANDPD  .L__sign_mask(%rip),%xmm0,%xmm1          #clear the sign

    VMOVAPD  .L_twobypi(%rip),%xmm2
    VFMADD213SD .L_point_five(%rip),%xmm1,%xmm2
    VCVTTPD2DQ %xmm2,%xmm2
    VPMOVSXDQ %xmm2,%xmm4

    VANDPD  .L_int_three(%rip),%xmm4,%xmm7         # region xmm7
    VSHUFPS $8,%xmm4,%xmm4,%xmm4
    VCVTDQ2PD   %xmm4,%xmm4
    VMOVAPD  %xmm4, %xmm2                    #exchange xmm2 and xmm6

    VFNMADD132SD .L_piby2_1(%rip),%xmm0,%xmm2       # xmm2 rhead
    VMULSD  .L_piby2_1tail(%rip),%xmm4,%xmm3            # xmm3 rtail
    VANDPD .L__inf_mask_64(%rip),%xmm2,%xmm1

    VPSRLQ $52,%xmm1,%xmm1
    VPSRLQ $52,%xmm0,%xmm5
    VPSUBQ  %xmm1,%xmm5,%xmm1                       # expdiff xmm4

    VMOVQ %xmm1,%rax

    cmp .L_val_15(%rip),%eax
    JB exit_range_e5

    VMOVAPD %xmm2,%xmm5
    VMULSD  .L_piby2_2(%rip),%xmm4,%xmm3            # rtail_1 xmm3
    VSUBSD  %xmm3,%xmm5,%xmm2                       # rhead_1 xmm2

    VSUBSD  %xmm2,%xmm5,%xmm1
    VSUBSD  %xmm3,%xmm1,%xmm1
    VXORPD  .L_signbit(%rip),%xmm1,%xmm1
    VMOVAPD %xmm4,%xmm3
    VFMADD132SD   .L_piby2_2tail(%rip),%xmm1,%xmm3   # rtail_1 xmm5


    cmp .L_val_48(%rip),%eax
    JB exit_range_e5

    VMOVAPD %xmm2,%xmm5
    VMULSD  .L_piby2_3(%rip),%xmm4,%xmm3            # rtail_1 xmm3
    VSUBSD  %xmm3,%xmm5,%xmm2                       # rhead_1 xmm2

    VSUBSD  %xmm2,%xmm5,%xmm1
    VSUBSD  %xmm3,%xmm1,%xmm1
    VXORPD  .L_signbit(%rip),%xmm1,%xmm1

    VMOVAPD    %xmm4,%xmm3
    VFMADD132SD  .L_piby2_3tail(%rip),%xmm1,%xmm3   # rtail_1 xmm5


exit_range_e5:
    VSUBSD %xmm3,%xmm2,%xmm0                       # r_1  xmm10
    VSUBSD %xmm0,%xmm2,%xmm4
    VSUBSD %xmm3,%xmm4,%xmm1                        # rr_1 xmm4

    .endm



.macro call_remainder_2fpiby2_4f_4d_fma3

VMOVDQA %ymm0,%ymm14

L__first:
movd %xmm0,%r11
cmp .L_e_5(%rip),%r11
jl L__second
	call_remainder_piby2_f fma3_one
	VMOVDQA %xmm0,%xmm15
	VMOVDQA %xmm4,%xmm11
L__second:
	VMOVDQA   %ymm14,%ymm0
	VUNPCKHPD %xmm0,%xmm0,%xmm0
	movd %xmm0,%r11
	cmp  .L_e_5(%rip),%r11
	jl L__third
	call_remainder_piby2_f fma3_two
	VUNPCKLPD %xmm0,%xmm15,%xmm15
	VUNPCKLPD %xmm4,%xmm11,%xmm11

L__third:
	VEXTRACTF128 $1,%ymm14,%xmm0
	movd %xmm0 ,%r11
	cmp  .L_e_5(%rip),%r11
	jl L__fourth
	call_remainder_piby2_f fma3_three
	VINSERTF128 $1,%xmm0,%ymm15,%ymm15
	VINSERTF128 $1,%xmm4,%ymm11,%ymm11

L__fourth:
	VEXTRACTF128 $1,%ymm14,%xmm0
	VUNPCKHPD %xmm0,%xmm0,%xmm0
	movd %xmm0 ,%r11
	cmp  .L_e_5(%rip),%r11
	jl L__end
	call_remainder_piby2_f fma3_four

	VEXTRACTF128 $1,%ymm15,%xmm14
	VUNPCKLPD %xmm0,%xmm14,%xmm0
	VINSERTF128 $1,%xmm0,%ymm15,%ymm15

	VEXTRACTF128 $1,%ymm11,%xmm14
	VUNPCKLPD %xmm4,%xmm14,%xmm0
	VINSERTF128 $1,%xmm0,%ymm11,%ymm11

L__end:
	VMOVDQA %ymm15,%ymm0
	.endm





#############  Vector float    ymm0=r input  ymm0=output ##############
.macro sin_piby4_s4f

	VMOVAPD .L_Sin+0x40(%rip),%ymm4
	VMULPD		%ymm0,%ymm0,%ymm3                	#x * x  x2 ymm3
	VFMADDPD %ymm4,.L_Sin+0x60(%rip),%ymm3,%ymm4
	VFMADDPD .L_Sin+0x20(%rip),%ymm4,%ymm3,%ymm4
	VFMADDPD .L_Sin(%rip),%ymm4,%ymm3,%ymm4
	VMULPD 	  %ymm3,%ymm0,%ymm3					#x2 * x x3  ymm4
	VFMADDPD  %ymm0,%ymm3,%ymm4,%ymm0
	.endm

.macro  cos_piby4_s4f

	VMOVAPD	.L_one(%rip),%ymm2
	VMULPD		%ymm0,%ymm0,%ymm3                    # x * x ymm3
	VMULPD		.L_point_five(%rip),%ymm3,%ymm4		 # r ymm4
	VSUBPD		%ymm4,%ymm2,%ymm2
	VMOVAPD .L_Cos+0x40(%rip),%ymm4
	VFMADDPD %ymm4,.L_Cos+0x60(%rip),%ymm3,%ymm4
	VFMADDPD .L_Cos+0x20(%rip),%ymm4,%ymm3,%ymm4
	VFMADDPD .L_Cos(%rip),%ymm4,%ymm3,%ymm4
	VMULPD	%ymm3,%ymm3,%ymm3
	VFMADDPD  %ymm2,%ymm3,%ymm4,%ymm0
	.endm

.macro tan_piby4_s4f

	VMULPD	%ymm0,%ymm0,%ymm3					# x * x ymm3 (r)
	VMOVAPD	.L_tanf+0x20(%rip),%ymm4
	VFNMADDPD .L_tanf(%rip),%ymm4,%ymm3,%ymm4
	VMOVAPD	.L_tanf+0x80(%rip),%ymm5
	VFMADDPD .L_tanf+0x60(%rip),%ymm5,%ymm3,%ymm5
	VFMADDPD .L_tanf+0x40(%rip),%ymm5,%ymm3,%ymm5
	VDIVPD	%ymm5,%ymm4,%ymm4
	VMULPD	%ymm0,%ymm3,%ymm3
	VFMADDPD %ymm0,%ymm4,%ymm3,%ymm0
	VEXTRACTF128 $1,%ymm2,%xmm3
	VPCOMEQUQ .L_int_one(%rip),%xmm2,%xmm2		# recp mask ymm2
	VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3		# recp mask ymm2
	VINSERTF128 $1,%xmm3,%ymm2,%ymm2
	VMOVAPD	.L_n_one(%rip),%ymm1
	VDIVPD	%ymm0,%ymm1,%ymm1
	VPCMOV	%ymm2,%ymm0,%ymm1,%ymm0
	.endm

############# End  vector float ##############


#############  scalar float   xmm0=r input  xmm0=output ##############

.macro sin_piby4_sf

	VMOVAPD .L_Sin+0x40(%rip),%xmm1
	VMULSD		%xmm0,%xmm0,%xmm3                	#x * x  x2 xmm3
	VFMADDSD %xmm1,.L_Sin+0x60(%rip),%xmm3,%xmm1
	VFMADDSD .L_Sin+0x20(%rip),%xmm1,%xmm3,%xmm1
	VFMADDSD .L_Sin(%rip),%xmm1,%xmm3,%xmm1
	VMULSD 	  %xmm3,%xmm0,%xmm3					#x2 * x x3  xmm1
	VFMADDSD  %xmm0,%xmm3,%xmm1,%xmm0
	.endm

.macro cos_piby4_sf

	VMOVAPD	.L_one(%rip),%xmm2
	VMULSD		%xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULSD		.L_point_five(%rip),%xmm3,%xmm1		 # r xmm1
	VSUBSD		%xmm1,%xmm2,%xmm2
	VMOVAPD .L_Cos+0x40(%rip),%xmm1
	VFMADDSD %xmm1,.L_Cos+0x60(%rip),%xmm3,%xmm1
	VFMADDSD .L_Cos+0x20(%rip),%xmm1,%xmm3,%xmm1
	VFMADDSD .L_Cos(%rip),%xmm1,%xmm3,%xmm1
	VMULSD	%xmm3,%xmm3,%xmm3
	VFMADDSD  %xmm2,%xmm3,%xmm1,%xmm0
	.endm


.macro sincosf_piby4_s

    VUNPCKLPD   %xmm0,%xmm0,%xmm0

	VMOVAPD	.L_one(%rip),%xmm2
	VMULPD		%xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULSD		.L_point_five(%rip),%xmm3,%xmm4		 # 0.5 * x2
	VSUBSD		%xmm4,%xmm2,%xmm2					 #1.0 - 0.5 * x2

	VMOVAPD .L_Sincos+0x20(%rip),%xmm4

	VFMADDPD %xmm4,.L_Sincos+0x30(%rip),%xmm3,%xmm4
	VFMADDPD .L_Sincos+0x10(%rip),%xmm4,%xmm3,%xmm4
	VFMADDPD .L_Sincos(%rip),%xmm4,%xmm3,%xmm4          #(c1 + x2 * (c2 + x2 * (c3 + x2 * c4)

    VUNPCKHPD   %xmm4,%xmm4,%xmm6
	#sin
	VMULSD  %xmm3,%xmm0,%xmm5             #x * x2
	VFMADDSD  %xmm0,%xmm5,%xmm4,%xmm0


	VMULSD	%xmm3,%xmm3,%xmm3
	VFMADDSD  %xmm2,%xmm3,%xmm6,%xmm1
  .endm


.macro  tan_piby4_sf_zeor


  VMULSD %xmm0,%xmm0,%xmm1 # x * x xmm1 (r)
  VMOVSD .L_tanf+0x20(%rip),%xmm3
  VFNMADDSD .L_tanf(%rip),%xmm3,%xmm1,%xmm3
  VMOVSD .L_tanf+0x80(%rip),%xmm2
  VFMADDSD .L_tanf+0x60(%rip),%xmm2,%xmm1,%xmm2
  VFMADDSD .L_tanf+0x40(%rip),%xmm2,%xmm1,%xmm2
  VDIVSD %xmm2,%xmm3,%xmm3
  VMULSD %xmm0,%xmm1,%xmm1
  VFMADDSD %xmm0,%xmm3,%xmm1,%xmm0

  .endm



.macro  tan_piby4_sf

	VMULSD	%xmm0,%xmm0,%xmm1					# x * x xmm3 (r)
	VMOVSD	.L_tanf+0x20(%rip),%xmm3
	VFNMADDSD .L_tanf(%rip),%xmm3,%xmm1,%xmm3
	VMOVSD	.L_tanf+0x80(%rip),%xmm2
	VFMADDSD .L_tanf+0x60(%rip),%xmm2,%xmm1,%xmm2
	VFMADDSD .L_tanf+0x40(%rip),%xmm2,%xmm1,%xmm2
	VDIVSD	%xmm2,%xmm3,%xmm3
	VMULSD	%xmm0,%xmm1,%xmm1
	VFMADDSD %xmm0,%xmm3,%xmm1,%xmm0
	cmp  $0x1,%eax
	je   tan_recip
	jmp  exit_tanpiby4

tan_recip :
	  VMOVQ .L_n_one(%rip),%xmm3
	  VDIVSD %xmm0,%xmm3,%xmm0

exit_tanpiby4 :

	.endm

############# End  scalar float ##############


#############  vector double   xmm0=r xmm1=rr input  xmm3=recp xmm0=output ##############

.macro  cos_2fpiby4_s


	VMOVAPD .L_Cos+0x80(%rip),%xmm5
	VMOVAPD	.L_one(%rip),%xmm2
	VMULPD		%xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULPD		.L_point_five(%rip),%xmm3,%xmm4		 # r xmm4
	VSUBPD		%xmm4,%xmm2,%xmm6					 # t xmm6
	VSUBPD		%xmm6,%xmm2,%xmm2                    # 1-t
	VSUBPD		%xmm4,%xmm2,%xmm2					 # 	(1-t) -r xmm2
	VFNMADDPD	%xmm2,%xmm1,%xmm0,%xmm2				 # (1.0 - t) - r) - x * xx) xmm2
	VMULPD 		%xmm3,%xmm3,%xmm4					 # x2 * x2 xmm4
	VFMADDPD %xmm5,.L_Cos+0xA0(%rip),%xmm3,%xmm5
	VFMADDPD .L_Cos+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Cos+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Cos+0x20(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Cos(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD %xmm2,%xmm5,%xmm4,%xmm5
	VADDPD		%xmm6,%xmm5,%xmm0
	.endm


.macro  sin_2fpiby4_s_xx_zero

	VMOVAPD .L_Sin+0x80(%rip),%xmm5
	VMULPD		%xmm0,%xmm0,%xmm3                	#x * x  x2 xmm3

	VFMADDPD %xmm5,.L_Sin+0xA0(%rip),%xmm3,%xmm5
	VFMADDPD .L_Sin+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Sin+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Sin+0x20(%rip),%xmm5,%xmm3,%xmm5	#xmm5 r

	VMULPD 		%xmm3,%xmm0,%xmm4					#x2 * x x3  xmm4
	VFMADDPD .L_Sin(%rip),%xmm3,%xmm5,%xmm6
	VFMADDPD %xmm0,%xmm6,%xmm4,%xmm0				# xmm6 res1

	.endm





.macro  sin_2fpiby4_s

	VMOVAPD .L_Sin+0x80(%rip),%xmm5
	VMULPD		%xmm0,%xmm0,%xmm3                	#x * x  x2 xmm3

	VFMADDPD %xmm5,.L_Sin+0xA0(%rip),%xmm3,%xmm5
	VFMADDPD .L_Sin+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Sin+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDPD .L_Sin+0x20(%rip),%xmm5,%xmm3,%xmm5	#xmm5 r

	VMULPD 		%xmm3,%xmm0,%xmm4					#x2 * x x3  xmm4

	VMULPD	%xmm5,%xmm4,%xmm2		  	        	# xx,-(x3 * r)
	VMULPD	.L_point_five(%rip),%xmm1,%xmm5			# (0.5*xx)-(x3 * r)
	VSUBPD	%xmm2,%xmm5,%xmm2
	VMULPD %xmm2,%xmm3,%xmm2
	VSUBPD %xmm1,%xmm2,%xmm2
	VFNMADDPD %xmm2,.L_Sin(%rip),%xmm4,%xmm2
	VSUBPD	%xmm2,%xmm0,%xmm0						#res2 %xmm2
	.endm


.macro  tan_2fpiby4_s

    VCMPGTPD .L_point_68(%rip),%xmm0,%xmm2
    VCMPLTPD .L_n_point_68(%rip),%xmm0,%xmm9
    VANDPD  .L_signbit(%rip),%xmm2,%xmm15
    VXORPD  %xmm15,%xmm0,%xmm5               # x-
    VXORPD  %xmm15,%xmm1,%xmm6               # xx-
    VANDPD  .L_signbit(%rip),%xmm9,%xmm15    # transform -1 if xmm9 set
    VADDPD  .L_piby4_lead(%rip),%xmm5,%xmm5
    VADDPD  .L_piby4_tail(%rip),%xmm6,%xmm6
    VADDPD  %xmm6,%xmm5,%xmm5               # x
    VORPD   %xmm2,%xmm9,%xmm8               # transform mask xmm8
    VPCMOV  %xmm8,%xmm0,%xmm5,%xmm0
    VXORPD  %xmm9,%xmm9,%xmm9
    VPCMOV  %xmm8,%xmm1,%xmm9,%xmm1     # xmm0 x xmm1 xx
    VMULPD  %xmm0,%xmm0,%xmm9               # x2 xmm9
    VMULPD  %xmm0,%xmm1,%xmm4        # x * xx xmm4
    VFMADDPD    %xmm9,.L_two(%rip),%xmm4,%xmm2     # xmm2 r
    VMOVAPD  .L_tan_c3(%rip),%xmm9
    VFMADDPD .L_tan_c2(%rip),%xmm9,%xmm2,%xmm9
    VFMADDPD .L_tan_c1(%rip),%xmm9,%xmm2,%xmm9
    VMOVAPD   .L_tan_c7(%rip),%xmm4
    VFNMADDPD .L_tan_c6(%rip),%xmm4,%xmm2,%xmm4
    VFMADDPD .L_tan_c5(%rip),%xmm4,%xmm2,%xmm4
    VFMADDPD .L_tan_c4(%rip),%xmm4,%xmm2,%xmm4
    VDIVPD  %xmm4,%xmm9,%xmm9
    VMULPD  %xmm2,%xmm9,%xmm9
    VFMADDPD %xmm1,%xmm0,%xmm9,%xmm9            # t2
    VADDPD      %xmm9,%xmm0,%xmm1               # t = t1+ t2 xmm1 res3
    VMULPD      .L_two(%rip),%xmm1,%xmm6
    VSUBPD      .L_one(%rip),%xmm1,%xmm4
    VDIVPD      %xmm4,%xmm6,%xmm5
    VSUBPD      .L_one(%rip),%xmm5,%xmm5
    VXORPD      %xmm5,%xmm15,%xmm5
    VADDPD      .L_one(%rip),%xmm1,%xmm4
    VDIVPD      %xmm4,%xmm6,%xmm6
    VMOVAPD     .L_one(%rip),%xmm4
    VSUBPD      %xmm6,%xmm4,%xmm6
    VXORPD      %xmm6,%xmm15,%xmm6
    VANDPD  .L_half_mask(%rip),%xmm1,%xmm2      # xmm2 z1
    VSUBPD  %xmm0,%xmm2,%xmm4
    VSUBPD  %xmm4,%xmm9,%xmm4                   # xmm4 z2
    VMOVAPD .L_n_one(%rip),%xmm9
    VDIVPD   %xmm1,%xmm9,%xmm9                  # xmm9 trec
    VANDPD  .L_half_mask(%rip),%xmm9,%xmm15      # xmm15 trec_top
    VFMADDPD .L_one(%rip),%xmm15,%xmm2,%xmm2     #
    VFMADDPD  %xmm2,%xmm15,%xmm4,%xmm2           #
    VFMADDPD  %xmm15,%xmm9,%xmm2,%xmm2           # res2
    VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3      # recp mask xmm3
    VPCMOV %xmm3,%xmm6,%xmm5,%xmm6              # xmm6 res1
    VPCMOV  %xmm3,%xmm1,%xmm2,%xmm2
    VPCMOV  %xmm8,%xmm2,%xmm6,%xmm0
	.endm

############# End  vector double ##############


#############  scalar double   xmm0=r xmm1=rr input  xmm0=output ##############
.macro  cos_piby4_s


	VMOVAPD	.L_one(%rip),%xmm2
	VMULSD		%xmm0,%xmm0,%xmm3                    # x * x xmm3
	VMULSD		.L_point_five(%rip),%xmm3,%xmm5		 # r xmm4
	VSUBSD		%xmm5,%xmm2,%xmm4					 # t xmm6
	VSUBSD		%xmm4,%xmm2,%xmm2                    # 1-t
	VSUBSD		%xmm5,%xmm2,%xmm2
	VMOVAPD .L_Cos+0x80(%rip),%xmm5					 # 	(1-t) -r xmm2
	VFNMADDSD	%xmm2,%xmm1,%xmm0,%xmm2				 # (1.0 - t) - r) - x * xx) xmm2
	VMULSD 		%xmm3,%xmm3,%xmm1					 # x2 * x2 xmm4
	VFMADDSD %xmm5,.L_Cos+0xA0(%rip),%xmm3,%xmm5
	VFMADDSD .L_Cos+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Cos+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Cos+0x20(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Cos(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD %xmm2,%xmm5,%xmm1,%xmm5
	VADDSD		%xmm4,%xmm5,%xmm0
	.endm


.macro  sin_piby4_s_xx_zero

	VMOVSD .L_Sin+0x80(%rip),%xmm5
	VMULSD		%xmm0,%xmm0,%xmm3                	#x * x  x2 xmm3
	VFMADDSD %xmm5,.L_Sin+0xA0(%rip),%xmm3,%xmm5
	VFMADDSD .L_Sin+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Sin+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Sin+0x20(%rip),%xmm5,%xmm3,%xmm5	#xmm5 r
	VMULSD 		%xmm3,%xmm0,%xmm4					#x2 * x x3  xmm4
	VFMADDSD    .L_Sin(%rip),%xmm3,%xmm5,%xmm2
	VFMADDSD %xmm0,%xmm2,%xmm4,%xmm0				# xmm6 res1
	.endm


.macro  sin_piby4_s


	VMOVAPD .L_Sin+0x80(%rip),%xmm5
	VMULSD		%xmm0,%xmm0,%xmm3                	#x * x  x2 xmm3
	VFMADDSD %xmm5,.L_Sin+0xA0(%rip),%xmm3,%xmm5
	VFMADDSD .L_Sin+0x60(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Sin+0x40(%rip),%xmm5,%xmm3,%xmm5
	VFMADDSD .L_Sin+0x20(%rip),%xmm5,%xmm3,%xmm5	#xmm5 r

	VMULSD 		%xmm3,%xmm0,%xmm4					#x2 * x x3  xmm4
	VMULSD	%xmm5,%xmm4,%xmm2		  	        	# xx,-(x3 * r)
	VMULSD	.L_point_five(%rip),%xmm1,%xmm5			# (0.5*xx)-(x3 * r)
	VSUBSD	%xmm2,%xmm5,%xmm2
	VMULSD %xmm2,%xmm3,%xmm2
	VSUBSD %xmm1,%xmm2,%xmm2
	VFNMADDSD %xmm2,.L_Sin(%rip),%xmm4,%xmm2
	VSUBSD	%xmm2,%xmm0,%xmm0						#res2 %xmm2
	.endm


.macro  sincos_piby4_s

    VUNPCKLPD   %xmm0,%xmm0,%xmm0
    VMULPD      %xmm0,%xmm0,%xmm2                   #x * x  x2 xmm3
    VMOVAPD .L_Sincos+0x40(%rip),%xmm5
    VFMADDPD %xmm5,.L_Sincos+0x50(%rip),%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x30(%rip),%xmm5,%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x20(%rip),%xmm5,%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x10(%rip),%xmm5,%xmm2,%xmm7           #xmm7 r
    VFMADDPD .L_Sincos(%rip),%xmm7,%xmm2,%xmm5            #

    #cos
    VUNPCKHPD %xmm5,%xmm5,%xmm5
    VMULSD %xmm2,%xmm2,%xmm8   #  x2 * x2
    VMULSD  .L_point_five(%rip),%xmm2,%xmm4  # r
    VMOVAPD .L_one(%rip),%xmm3
    VSUBSD  %xmm4,%xmm3,%xmm6     # t
    VSUBSD %xmm6,%xmm3,%xmm3          # 1-t
    VSUBSD %xmm4,%xmm3,%xmm3          # 1-t-r
    VFNMADDSD  %xmm3,%xmm1,%xmm0,%xmm3         # x * xx
    VFMADDSD  %xmm3,%xmm8,%xmm5,%xmm5    #((((1.0 - t) - r) - x * xx) + x2 * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))))))


    #sin
    VMULSD %xmm2,%xmm0,%xmm3    # x3
    VMULSD %xmm3,%xmm7,%xmm4     #x3 * r
    VFMSUBSD %xmm4,.L_point_five(%rip),%xmm1,%xmm4  # 0.5 * xx - x3 * r
    VFMSUBSD  %xmm1,%xmm2,%xmm4,%xmm4   # (x2 * (0.5 * xx - x3 * r) - xx)
    VFNMADDSD  %xmm4,.L_Sincos(%rip),%xmm3,%xmm4
    VSUBSD  %xmm4,%xmm0,%xmm0    # sin
    VADDSD %xmm6,%xmm5,%xmm1        # cos

    .endm



.macro  sincos_piby4_s_xx_zero

    VUNPCKLPD   %xmm0,%xmm0,%xmm0
    VMULPD      %xmm0,%xmm0,%xmm2                   #x * x  x2 xmm3
    VMOVAPD .L_Sincos+0x40(%rip),%xmm5
    VFMADDPD %xmm5,.L_Sincos+0x50(%rip),%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x30(%rip),%xmm5,%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x20(%rip),%xmm5,%xmm2,%xmm5
    VFMADDPD .L_Sincos+0x10(%rip),%xmm5,%xmm2,%xmm7           #xmm7 r
    VFMADDPD .L_Sincos(%rip),%xmm7,%xmm2,%xmm5            #

    #sin
    VMULSD %xmm2,%xmm0,%xmm3    # x3
    VFMADDSD %xmm0,%xmm3,%xmm5,%xmm0     #x3 * r  x + x3 * (c1 + x2 * r);


    #cos
    VUNPCKHPD %xmm5,%xmm5,%xmm5
    VMULSD %xmm2,%xmm2,%xmm7   #  x2 * x2
    VMULSD  .L_point_five(%rip),%xmm2,%xmm4  # r
    VMOVAPD .L_one(%rip),%xmm3
    VSUBSD  %xmm4,%xmm3,%xmm6     # t
    VSUBSD %xmm6,%xmm3,%xmm3          # 1-t
    VSUBSD %xmm4,%xmm3,%xmm3          # 1-t-r
#    VFNMADDSD  %xmm3,%xmm0,%xmm1,%xmm3         # x * xx
    VFMADDSD  %xmm3,%xmm7,%xmm5,%xmm5    #((((1.0 - t) - r) - x * xx) + x2 * x2 * (c1 + x2 * (c2 + x2 * (c3 + x2 * (c4 + x2 * (c5 + x2 * c6))))))
    VADDSD %xmm6,%xmm5,%xmm1        # cos
    .endm



.macro tan_piby4_s

	VCMPGTPD .L_point_68(%rip),%xmm0,%xmm2
	VCMPLTPD .L_n_point_68(%rip),%xmm0,%xmm9
	VANDPD	.L_signbit(%rip),%xmm2,%xmm7
	VXORPD	%xmm7,%xmm0,%xmm5				# x-
	VXORPD	%xmm7,%xmm1,%xmm6				# xx-
	VANDPD	.L_signbit(%rip),%xmm9,%xmm7	# transform -1 if xmm9 set
	VADDPD	.L_piby4_lead(%rip),%xmm5,%xmm5
	VADDPD	.L_piby4_tail(%rip),%xmm6,%xmm6
	VADDPD	%xmm6,%xmm5,%xmm5				# x
	VORPD	%xmm2,%xmm9,%xmm8				# transform mask xmm8
	VPCMOV	%xmm8,%xmm0,%xmm5,%xmm0
	VXORPD	%xmm9,%xmm9,%xmm9
	VPCMOV	%xmm8,%xmm1,%xmm9,%xmm1		# xmm0 x xmm1 xx
	VMULSD	%xmm0,%xmm0,%xmm9				# x2 xmm9
	VMULSD	%xmm0,%xmm1,%xmm4        # x * xx xmm4
	VFMADDSD	%xmm9,.L_two(%rip),%xmm4,%xmm2	   # xmm2 r
	VMOVAPD	 .L_tan_c3(%rip),%xmm9
	VFMADDSD .L_tan_c2(%rip),%xmm9,%xmm2,%xmm9
	VFMADDSD .L_tan_c1(%rip),%xmm9,%xmm2,%xmm9
	VMOVAPD	  .L_tan_c7(%rip),%xmm4
	VFNMADDSD .L_tan_c6(%rip),%xmm4,%xmm2,%xmm4
	VFMADDSD .L_tan_c5(%rip),%xmm4,%xmm2,%xmm4
	VFMADDSD .L_tan_c4(%rip),%xmm4,%xmm2,%xmm4
	VDIVSD	%xmm4,%xmm9,%xmm9
	VMULSD	%xmm2,%xmm9,%xmm9
	VFMADDSD %xmm1,%xmm0,%xmm9,%xmm9			# t2
	VADDSD		%xmm9,%xmm0,%xmm1				# t = t1+ t2 xmm1 res3

	VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3		# recp mask xmm3

	VMOVDDUP	%xmm8,%xmm8
	VPTEST %xmm8,%xmm8
	JZ if_recip_set

if_transfor_set:

	VMULSD		.L_two(%rip),%xmm1,%xmm6 # 2*t
	VMOVAPD		.L_one(%rip),%xmm4
	VSUBSD		%xmm4,%xmm1,%xmm5		# t-1
	VADDSD		%xmm4,%xmm1,%xmm10		# t+1
	VPCMOV	   %xmm3,%xmm10,%xmm5,%xmm5
	VDIVSD	   %xmm5,%xmm6,%xmm5			# 2*t/(t-1) or 2*t/(t+1)
	VSUBSD		%xmm5,%xmm4,%xmm6      #(1.0 - 2*t/(1+t)
	VSUBSD		%xmm4,%xmm5,%xmm5       #(2*t/(t-1) - 1.0)
	VPCMOV	   %xmm3,%xmm6,%xmm5,%xmm6
	VXORPD		%xmm6,%xmm7,%xmm6		# res1
	JMP exit_tanpiby4

if_recip_set:
	VMOVDDUP	%xmm3,%xmm3
	VPTEST %xmm3,%xmm3
	JZ exit_tanpiby4

	VANDPD  .L_half_mask(%rip),%xmm1,%xmm2		# xmm2 z1
	VSUBSD	%xmm0,%xmm2,%xmm4
	VSUBSD	%xmm4,%xmm9,%xmm4					# xmm4 z2
	VMOVAPD	.L_n_one(%rip),%xmm9
	VDIVSD	 %xmm1,%xmm9,%xmm9					# xmm9 trec
	VANDPD  .L_half_mask(%rip),%xmm9,%xmm7		# xmm7 trec_top
	VFMADDSD .L_one(%rip),%xmm7,%xmm2,%xmm2		#
	VFMADDSD  %xmm2,%xmm7,%xmm4,%xmm2			#
	VFMADDSD  %xmm7,%xmm9,%xmm2,%xmm2			# xmm2 res2

exit_tanpiby4:
	VPCMOV %xmm3,%xmm1,%xmm2,%xmm0
	VPCMOV %xmm8,%xmm0,%xmm6,%xmm0				# xmm6 res1

	.endm



.macro tan_piby4_s_zero

	VCMPGTPD .L_point_68(%rip),%xmm0,%xmm2
	VCMPLTPD .L_n_point_68(%rip),%xmm0,%xmm9
	VANDPD	.L_signbit(%rip),%xmm2,%xmm7
	VXORPD	%xmm7,%xmm0,%xmm5				# x-
	VANDPD	.L_signbit(%rip),%xmm9,%xmm7	# transform -1 if xmm9 set
	VADDPD	.L_piby4_lead(%rip),%xmm5,%xmm5
	VADDPD	.L_piby4_tail(%rip),%xmm5,%xmm5	# x
	VORPD	%xmm2,%xmm9,%xmm8				# transform mask xmm8
	VPCMOV	%xmm8,%xmm0,%xmm5,%xmm0
	VXORPD	%xmm9,%xmm9,%xmm9
	VMULSD	%xmm0,%xmm0,%xmm2				#  xmm2 r
	VMOVAPD	 .L_tan_c3(%rip),%xmm9
	VFMADDSD .L_tan_c2(%rip),%xmm9,%xmm2,%xmm9
	VFMADDSD .L_tan_c1(%rip),%xmm9,%xmm2,%xmm9
	VMOVAPD	  .L_tan_c7(%rip),%xmm4
	VFNMADDSD .L_tan_c6(%rip),%xmm4,%xmm2,%xmm4
	VFMADDSD .L_tan_c5(%rip),%xmm4,%xmm2,%xmm4
	VFMADDSD .L_tan_c4(%rip),%xmm4,%xmm2,%xmm4
	VDIVSD	%xmm4,%xmm9,%xmm9
	VMULSD	%xmm2,%xmm9,%xmm9
	VMULSD  %xmm0,%xmm9,%xmm9			# t2
	VADDSD		%xmm9,%xmm0,%xmm0				# t = t1+ t2 xmm1 res3
	VMOVDDUP %xmm8,%xmm8
	VPTEST %xmm8,%xmm8
	JZ ext_tanpiby4_zero
	VMULSD		.L_two(%rip),%xmm0,%xmm6
	VADDSD		.L_one(%rip),%xmm0,%xmm4
	VDIVSD		%xmm4,%xmm6,%xmm6
	VMOVAPD		.L_one(%rip),%xmm4
	VSUBSD		%xmm6,%xmm4,%xmm6
	VXORPD		%xmm6,%xmm7,%xmm6
	VPCMOV	%xmm8,%xmm0,%xmm6,%xmm0
ext_tanpiby4_zero:

	.endm


############# End  scalar double ##############


.macro  call_remainder_piby2


    lea  .L__Pibits(%rip),%r9
    #xexp = (x >> 52) - 1023

    movd %xmm0, %r11
    mov  %r11,  %rcx

    shr  $52,   %r11
    sub  $1023, %r11      #r11 = xexp = exponent of input x

    #calculate the last byte from which to start multiplication
    #last = 134 - (xexp >> 3)

    mov  %r11, %r10
    shr  $3,   %r10

    sub  $134, %r10       #r10 = -last
    neg  %r10             #r10 = last

    #load 64 bits of 2_by_pi
    mov  (%r9, %r10),%rax

    #mov  %rdx, %rdi      # save address of region since mul modifies rdx

    #mantissa of x = ((x << 12) >> 12) | implied bit
    shl  $12,   %rcx

    shr  $12,   %rcx      #rcx = mantissa part of input x
    bts  $52,   %rcx      #add the implied bit as well

    #load next 128 bits of 2_by_pi
    add  $8, %r10        #increment to next 8 bytes of 2_by_pi
    movdqu (%r9, %r10), %xmm0
    #do three 64-bit multiplications with mant of x

    mul  %rcx

    mov  %rax, %r8       #r8 = last 64 bits of multiplication = res1[2]
    mov  %rdx, %r10      #r10 = carry

    movd %xmm0, %rax
    mul  %rcx

    #resexp = xexp & 7
    and  $7, %r11        #r11 = resexp = xexp & 7 = last 3 bits

    psrldq $8, %xmm0
    add  %r10, %rax      # add the previous carry

    adc  $0,   %rdx
    mov  %rax, %r9       #r9 = next 64 bits of multiplication = res1[1]

    mov  %rdx, %r10      #r10 = carry
    movd %xmm0,%rax

    mul  %rcx
    add  %rax, %r10      #r10 = most significant 64 bits = res1[0]

    #find the region
    #last three bits ltb = most sig bits >> (54 - resexp)) // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
    #point_five = ltb & 0x1

   #region = ((ltb >> 1) + point_five) & 3
    mov $54,  %rcx

    mov %r10, %rax
    sub %r11, %rcx

    xor %rdx, %rdx       #rdx = sign of x(i.e first part of x * 2bypi)
    shr %cl,  %rax

    jnc  .L__no_point_five
    ##if there is carry.. then negate the result of multiplication

    not  %r10
    not  %r9

    not  %r8
    mov $0x8000000000000000,%rdx


#.p2align 4
.L__no_point_five:
    adc $0,   %rax
    and $3,   %rax
    VMOVD %eax, %xmm7   #store region to xmm7
    #calculate the number of integer bits and zero them out

    mov %r11, %rcx
    add $10,  %rcx  #rcx = no. of integer bits
    shl %cl,  %r10
    shr %cl,  %r10  #r10 contains only mant bits
    sub $64,  %rcx  #form the exponent
    mov %rcx, %r11
    #find the highest set bit
    bsr %r10,%rcx
    jnz  .L__form_mantissa
    mov %r9,%r10
    mov %r8,%r9
    mov $0, %r8
    bsr %r10,%rcx  #rcx = hsb
    sub $64, %r11

#.p2align 4
.L__form_mantissa:
    add %rcx,%r11  #for exp of x
    sub $52,%rcx #rcx = no. of bits to shift in r10
    cmp $0,%rcx
    jl  .L__hsb_below_52
    je  .L__form_numbers
    #hsb above 52


    mov %r10,%r8 #previous contents of r8 not required
    shr %cl,%r10 #r10 = mantissa of x with hsb at 52
    shr %cl,%r9  #make space for bits from r10
    sub $64,%rcx
    neg %rcx     #rvx = no of bits to shift r10 to move those bits to r9
    shl %cl,%r8
    or  %r8,%r9  #r9 = mantissa bits of xx
    jmp .L__form_numbers

#.p2align 4
.L__hsb_below_52:
    neg %rcx
    mov %r9,%rax
    shl %cl,%r10
    shl %cl,%r9
    sub $64,%rcx
    neg %rcx
    shr %cl,%rax
    or  %rax,%r10
    shr %cl,%r8
    or  %r8,%r9


#.p2align 4
.L__form_numbers:
    add $1023, %r11
    btr $52,%r10     #remove the implied bit
    mov %r11,%rcx
    or  %rdx,%r10    #put the sign
    shl $52, %rcx
    or  %rcx,%r10    #x is in r10




    movd %r10,%xmm2  #xmm8 = x
    #form xx
    xor %rcx,%rcx
    bsr %r9,%rcx
    sub $64, %rcx   #to shift the implied bit as well
    neg %rcx
    shl %cl,%r9
    shr $12,%r9
    add $52,%rcx
    sub %rcx, %r11
    shl $52, %r11
    or  %rdx,%r9
    or  %r11,%r9
    movd %r9,%xmm1  #xmm1 = xx
	# tx
	VANDPD .L_fff800(%rip),%xmm2,%xmm4   # xmm4 hx
	VSUBSD	%xmm4,%xmm2,%xmm0     # xmm6 tx
	# c
	VMULSD	.L_piby2_lead(%rip),%xmm2,%xmm5  # xmm5 c
	# cc
	VMULSD	.L_piby2_part1(%rip),%xmm4,%xmm3
	VSUBSD	%xmm5,%xmm3,%xmm3



	VFMADDSD  %xmm3,.L_piby2_part1(%rip),%xmm0,%xmm3
	VFMADDSD  %xmm3,.L_piby2_part2(%rip),%xmm4,%xmm3

	VFMADDSD  %xmm3,.L_piby2_part2(%rip),%xmm0,%xmm3
	VMULSD	.L_piby2_lead(%rip),%xmm1,%xmm4

	VFMADDSD  %xmm4,.L_piby2_part3(%rip),%xmm2,%xmm4
	VADDSD	%xmm4,%xmm3,%xmm3	# xmm7 cc
	VADDSD %xmm3,%xmm5,%xmm0		# r
	VSUBSD %xmm0,%xmm5,%xmm1
	VADDSD %xmm3,%xmm1,%xmm1		# rr
	.endm


.macro   call_remainder_2fpiby2

	push %r12
	push %r13

	VMAXPD	.L_two(%rip),%xmm0,%xmm0
	VANDPD .L__inf_mask_64(%rip),%xmm0,%xmm1
	VPSRLQ	$52,%xmm1,%xmm1
	VPSUBQ	.L_1023(%rip),%xmm1,%xmm1			# xmm1 xexp
	VANDPD .L__mant_mask_64(%rip),%xmm0,%xmm2
	VORPD .L__imp_mask_64(%rip),%xmm2,%xmm2		# xmm2 ux
	VPSRLQ	$3,%xmm1,%xmm3
	VMOVAPD	.L_157(%rip),%xmm4
	VPSUBQ  %xmm3,%xmm4,%xmm5			# xmm5 first
	VPSLLQ	$3,%xmm3,%xmm3
	VPSUBQ %xmm3,%xmm1,%xmm1			# xmm1 resexp
	VPSUBQ .L_23(%rip),%xmm5,%xmm3			# xmm3 last
	VMOVQ %xmm3,%r12					# index lo
	VPUNPCKHQDQ %xmm3,%xmm3,%xmm3
	VMOVQ %xmm3,%r13					# index hi
	lea .L__Pibits(%rip),%r11
	MOV (%r11,%r12),%rax
	VMOVQ %xmm2,%rcx		#ux
	MUL	%rcx
	VMOVQ	%rax,%xmm9
	MOV	%rdx,%r9			# carry
	VPUNPCKHQDQ %xmm2,%xmm2,%xmm2
	VMOVQ %xmm2,%r8		#ux
	MOV (%r11,%r13),%rax
	MUL	%r8
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm9,%xmm9	# res[2] xmm9
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm7		# res[1] xmm7
	MOV	%rdx,%r9			# cary
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7	# res[1] xmm7
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm6		# res[0] xmm6
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm8
	VPUNPCKLQDQ	%xmm8,%xmm6,%xmm6	# res[0] xmm6
	#ltb
	VPSUBQ	.L_53(%rip),%xmm1,%xmm3
	VPSHLQ	%xmm3,%xmm6,%xmm4		# ltb xmm4
	VPAND	.L_int_one(%rip),%xmm4,%xmm3   # xmm3 	point_five
	VPSRLQ	$1,%xmm4,%xmm4
	VPADDQ	%xmm3,%xmm4,%xmm4
	VPAND	.L_int_three(%rip),%xmm4,%xmm11	# xmm11 region
	VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3
	VXORPD	%xmm3,%xmm6,%xmm6		#res[0]
	VXORPD	%xmm3,%xmm7,%xmm7		#res[1]
	VXORPD	%xmm3,%xmm9,%xmm9		#res[2]
	VANDPD	.L_signbit(%rip),%xmm3,%xmm3  					#sign xmm3
	VPADDQ	.L_10(%rip),%xmm1,%xmm4	 # initbits xmm4
	# remove all the integer bits
	VPSHLQ %xmm4,%xmm9,%xmm10	# new res[2] 10
	VPSUBQ	.L_64(%rip),%xmm4,%xmm5
	VPSHLQ %xmm5,%xmm9,%xmm9
	VPSHLQ %xmm4,%xmm7,%xmm8
	VORPD	%xmm8,%xmm9,%xmm9	#new res[1] 9
	VPSHLQ %xmm5,%xmm7,%xmm7
	VPSHLQ %xmm4,%xmm6,%xmm6
	VORPD	%xmm6,%xmm7,%xmm8	#new res[0] 8
	#leading bit set
	VMOVQ	%xmm8,%r10
	VPUNPCKHQDQ %xmm8,%xmm8,%xmm7
	VMOVQ %xmm7,%r9
	bsr %r10,%rcx
	bsr %r9,%rax
	VMOVQ %rcx,%xmm7
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7
	VMOVAPD .L_64(%rip),%xmm12
	VPSUBQ %xmm7,%xmm12,%xmm6	# xmm6 shift bits xmm7 highest set bit
	VPXOR %xmm13,%xmm13,%xmm13   # zero
	VPSUBQ %xmm7,%xmm13,%xmm13
	VPSHLQ	%xmm6,%xmm10,%xmm2	# new res[2] xmm2
	VPSHLQ	%xmm13,%xmm10,%xmm10
	VPSHLQ	%xmm6,%xmm9,%xmm4
	VPOR	%xmm10,%xmm4,%xmm4	# new res[1] xmm4
	VPSHLQ	%xmm13,%xmm9,%xmm9
	VPSHLQ	%xmm6,%xmm8,%xmm5
	VPOR	%xmm9,%xmm5,%xmm5	# new res[0] xmm5
	# construct x;
	VPADDQ	.L_959(%rip),%xmm7,%xmm7	# xmm7 rexp
	VPSLLQ	$52,%xmm7,%xmm6
	VPSRLQ $12,%xmm5,%xmm8				# xmm8 mant
	VPOR	%xmm8,%xmm6,%xmm8
	VPOR	%xmm8,%xmm3,%xmm8		# xmm8 x
	# for xx
	VPSLLQ $52,%xmm5,%xmm6
	VPSRLQ $12,%xmm4,%xmm9
	VPOR %xmm6,%xmm9,%xmm6		# xmm6 mant
	VMOVQ	%xmm6,%r10
	VPUNPCKHQDQ %xmm6,%xmm6,%xmm1
	VMOVQ %xmm1,%r9
	bsr %r10,%rcx
	bsr %r9,%rax
	VMOVQ %rcx,%xmm1
	VMOVQ %rax,%xmm13
	VPUNPCKLQDQ	%xmm13,%xmm1,%xmm1   # high set bit xmm1
	VPSUBQ %xmm1,%xmm12,%xmm13	# xmm13 shift bits
	VPSLLQ	$52,%xmm4,%xmm9
	VPSRLQ	$12,%xmm2,%xmm10
	VPOR	%xmm10,%xmm9,%xmm9	# temp
	VPXOR	%xmm12,%xmm12,%xmm12
	VPSUBQ	%xmm1,%xmm12,%xmm12			# change sign to shift right
	VPSHLQ	%xmm12,%xmm9,%xmm9
	VPSHLQ	%xmm13,%xmm6,%xmm10
	VPOR	%xmm10,%xmm9,%xmm9	# mant
	VPSRLQ	$12,%xmm9,%xmm9		# mant
	# construct xx
	VPSUBQ .L_116(%rip),%xmm7,%xmm7
	VPADDQ %xmm7,%xmm1,%xmm1
	VPSLLQ	$52,%xmm1,%xmm1
	VPOR	%xmm9,%xmm1,%xmm1
	VPOR	%xmm3,%xmm1,%xmm1	#xx
	# tx
	VANDPD .L_fff800(%rip),%xmm8,%xmm4   # xmm4 hx
	VSUBPD	%xmm4,%xmm8,%xmm6     # xmm6 tx
	# c
	VMULPD	.L_piby2_lead(%rip),%xmm8,%xmm5  # xmm5 c
	# cc
	VMULPD	.L_piby2_part1(%rip),%xmm4,%xmm7
	VSUBPD	%xmm5,%xmm7,%xmm7
	VFMADDPD  %xmm7,.L_piby2_part1(%rip),%xmm6,%xmm7
	VFMADDPD  %xmm7,.L_piby2_part2(%rip),%xmm4,%xmm7
	VFMADDPD  %xmm7,.L_piby2_part2(%rip),%xmm6,%xmm7
	VMULPD	.L_piby2_lead(%rip),%xmm1,%xmm9
	VFMADDPD  %xmm9,.L_piby2_part3(%rip),%xmm8,%xmm9
	VADDPD	%xmm9,%xmm7,%xmm7	# xmm7 cc
	VADDPD %xmm7,%xmm5,%xmm0		# r
	VSUBPD %xmm0,%xmm5,%xmm1
	VADDPD %xmm7,%xmm1,%xmm1		# rr

	pop %r13
	pop %r12
	.endm

.macro  call_remainder_2fpiby2_4f
	push %r12
	push %r13

	VMOVAPD	 %ymm0,%ymm14
	VMAXPD	.L_two(%rip),%xmm0,%xmm0
	VANDPD .L__inf_mask_64(%rip),%xmm0,%xmm1
	VPSRLQ	$52,%xmm1,%xmm1
	VPSUBQ	.L_1023(%rip),%xmm1,%xmm1			# xmm1 xexp
	VANDPD .L__mant_mask_64(%rip),%xmm0,%xmm2
	VORPD .L__imp_mask_64(%rip),%xmm2,%xmm2		# xmm2 ux
	VPSRLQ	$3,%xmm1,%xmm3
	VMOVAPD	.L_157(%rip),%xmm4
	VPSUBQ  %xmm3,%xmm4,%xmm5			# xmm5 first
	VPSLLQ	$3,%xmm3,%xmm3
	VPSUBQ %xmm3,%xmm1,%xmm1			# xmm1 resexp
	VPSUBQ .L_23(%rip),%xmm5,%xmm3			# xmm3 last
	VMOVQ %xmm3,%r12					# index lo
	VPUNPCKHQDQ %xmm3,%xmm3,%xmm3
	VMOVQ %xmm3,%r13					# index hi
	lea .L__Pibits(%rip),%r11
	MOV (%r11,%r12),%rax
	VMOVQ %xmm2,%rcx		#ux
	MUL	%rcx
	VMOVQ	%rax,%xmm9
	MOV	%rdx,%r9			# carry
	VPUNPCKHQDQ %xmm2,%xmm2,%xmm2
	VMOVQ %xmm2,%r8		#ux
	MOV (%r11,%r13),%rax
	MUL	%r8
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm9,%xmm9	# res[2] xmm9
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm7		# res[1] xmm7
	MOV	%rdx,%r9			# cary
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7	# res[1] xmm7
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm6		# res[0] xmm6
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm8
	VPUNPCKLQDQ	%xmm8,%xmm6,%xmm6	# res[0] xmm6
	#ltb
	VPSUBQ	.L_53(%rip),%xmm1,%xmm3
	VPSHLQ	%xmm3,%xmm6,%xmm4		# ltb xmm4
	VPAND	.L_int_one(%rip),%xmm4,%xmm3   # xmm3 	point_five
	VPSRLQ	$1,%xmm4,%xmm4
	VPADDQ	%xmm3,%xmm4,%xmm4
	VPAND	.L_int_three(%rip),%xmm4,%xmm11	# xmm11 region
	VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3
	VXORPD	%xmm3,%xmm6,%xmm6		#res[0]
	VXORPD	%xmm3,%xmm7,%xmm7		#res[1]
	VXORPD	%xmm3,%xmm9,%xmm9		#res[2]
	VANDPD	.L_signbit(%rip),%xmm3,%xmm3  					#sign xmm3
	VPADDQ	.L_10(%rip),%xmm1,%xmm4	 # initbits xmm4
	# remove all the integer bits
	VPSHLQ %xmm4,%xmm9,%xmm10	# new res[2] 10
	VPSUBQ	.L_64(%rip),%xmm4,%xmm5
	VPSHLQ %xmm5,%xmm9,%xmm9
	VPSHLQ %xmm4,%xmm7,%xmm8
	VORPD	%xmm8,%xmm9,%xmm9	#new res[1] 9
	VPSHLQ %xmm5,%xmm7,%xmm7
	VPSHLQ %xmm4,%xmm6,%xmm6
	VORPD	%xmm6,%xmm7,%xmm8	#new res[0] 8
	#leading bit set
	VMOVQ	%xmm8,%r10
	VPUNPCKHQDQ %xmm8,%xmm8,%xmm7
	VMOVQ %xmm7,%r9
	bsr %r10,%rcx
	bsr %r9,%rax
	VMOVQ %rcx,%xmm7
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7
	VMOVAPD .L_64(%rip),%xmm12
	VPSUBQ %xmm7,%xmm12,%xmm6	# xmm6 shift bits xmm7 highest set bit
	VPXOR %xmm13,%xmm13,%xmm13   # zero
	VPSUBQ %xmm7,%xmm13,%xmm13
	VPSHLQ	%xmm6,%xmm10,%xmm2	# new res[2] xmm2
	VPSHLQ	%xmm13,%xmm10,%xmm10
	VPSHLQ	%xmm6,%xmm9,%xmm4
	VPOR	%xmm10,%xmm4,%xmm4	# new res[1] xmm4
	VPSHLQ	%xmm13,%xmm9,%xmm9
	VPSHLQ	%xmm6,%xmm8,%xmm5
	VPOR	%xmm9,%xmm5,%xmm5	# new res[0] xmm5
	# construct x;
	VPADDQ	.L_959(%rip),%xmm7,%xmm7	# xmm7 rexp
	VPSLLQ	$52,%xmm7,%xmm6
	VPSRLQ $12,%xmm5,%xmm8				# xmm8 mant
	VPOR	%xmm8,%xmm6,%xmm8
	VEXTRACTF128 $1,%ymm14,%xmm0
	VPOR	%xmm8,%xmm3,%xmm14		# xmm14 x
	VMAXPD	.L_two(%rip),%xmm0,%xmm0
	VANDPD .L__inf_mask_64(%rip),%xmm0,%xmm1
	VPSRLQ	$52,%xmm1,%xmm1
	VPSUBQ	.L_1023(%rip),%xmm1,%xmm1			# xmm1 xexp
	VANDPD .L__mant_mask_64(%rip),%xmm0,%xmm2
	VORPD .L__imp_mask_64(%rip),%xmm2,%xmm2		# xmm2 ux
	VPSRLQ	$3,%xmm1,%xmm3
	VMOVAPD	.L_157(%rip),%xmm4
	VPSUBQ  %xmm3,%xmm4,%xmm5			# xmm5 first
	VPSLLQ	$3,%xmm3,%xmm3
	VPSUBQ %xmm3,%xmm1,%xmm1			# xmm1 resexp
	VPSUBQ .L_23(%rip),%xmm5,%xmm3			# xmm3 last
	VMOVQ %xmm3,%r12					# index lo
	VPUNPCKHQDQ %xmm3,%xmm3,%xmm3
	VMOVQ %xmm3,%r13					# index hi
	lea .L__Pibits(%rip),%r11
	MOV (%r11,%r12),%rax
	VMOVQ %xmm2,%rcx		#ux
	MUL	%rcx
	VMOVQ	%rax,%xmm9
	MOV	%rdx,%r9			# carry
	VPUNPCKHQDQ %xmm2,%xmm2,%xmm2
	VMOVQ %xmm2,%r8		#ux
	MOV (%r11,%r13),%rax
	MUL	%r8
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm9,%xmm9	# res[2] xmm9
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm7		# res[1] xmm7
	MOV	%rdx,%r9			# cary
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7	# res[1] xmm7
	MOV	%rdx,%r10					# carry
	add $8,%r12
	add $8,%r13
	MOV (%r11,%r12),%rax
	MUL	%rcx
	ADD	%r9,%rax
	ADC	$0,%rdx
	VMOVQ	%rax,%xmm6		# res[0] xmm6
	MOV (%r11,%r13),%rax
	MUL	%r8
	ADD	%r10,%rax
	ADC	$0,%rdx
	VMOVQ %rax,%xmm8
	VPUNPCKLQDQ	%xmm8,%xmm6,%xmm6	# res[0] xmm6
	#ltb
	VPSUBQ	.L_53(%rip),%xmm1,%xmm3
	VPSHLQ	%xmm3,%xmm6,%xmm4		# ltb xmm4
	VPAND	.L_int_one(%rip),%xmm4,%xmm3   # xmm3 	point_five
	VPSRLQ	$1,%xmm4,%xmm4
	VPADDQ	%xmm3,%xmm4,%xmm4
	VPAND	.L_int_three(%rip),%xmm4,%xmm4	# xmm11 region
	VINSERTF128 $1,%xmm4,%ymm11,%ymm11
	VPCOMEQUQ .L_int_one(%rip),%xmm3,%xmm3
	VXORPD	%xmm3,%xmm6,%xmm6		#res[0]
	VXORPD	%xmm3,%xmm7,%xmm7		#res[1]
	VXORPD	%xmm3,%xmm9,%xmm9		#res[2]
	VANDPD	.L_signbit(%rip),%xmm3,%xmm3  					#sign xmm3
	VPADDQ	.L_10(%rip),%xmm1,%xmm4	 # initbits xmm4
	# remove all the integer bits
	VPSHLQ %xmm4,%xmm9,%xmm10	# new res[2] 10
	VPSUBQ	.L_64(%rip),%xmm4,%xmm5
	VPSHLQ %xmm5,%xmm9,%xmm9
	VPSHLQ %xmm4,%xmm7,%xmm8
	VORPD	%xmm8,%xmm9,%xmm9	#new res[1] 9
	VPSHLQ %xmm5,%xmm7,%xmm7
	VPSHLQ %xmm4,%xmm6,%xmm6
	VORPD	%xmm6,%xmm7,%xmm8	#new res[0] 8
	#leading bit set
	VMOVQ	%xmm8,%r10
	VPUNPCKHQDQ %xmm8,%xmm8,%xmm7
	VMOVQ %xmm7,%r9
	bsr %r10,%rcx
	bsr %r9,%rax
	VMOVQ %rcx,%xmm7
	VMOVQ %rax,%xmm6
	VPUNPCKLQDQ	%xmm6,%xmm7,%xmm7
	VMOVAPD .L_64(%rip),%xmm12
	VPSUBQ %xmm7,%xmm12,%xmm6	# xmm6 shift bits xmm7 highest set bit
	VPXOR %xmm13,%xmm13,%xmm13   # zero
	VPSUBQ %xmm7,%xmm13,%xmm13
	VPSHLQ	%xmm6,%xmm10,%xmm2	# new res[2] xmm2
	VPSHLQ	%xmm13,%xmm10,%xmm10
	VPSHLQ	%xmm6,%xmm9,%xmm4
	VPOR	%xmm10,%xmm4,%xmm4	# new res[1] xmm4
	VPSHLQ	%xmm13,%xmm9,%xmm9
	VPSHLQ	%xmm6,%xmm8,%xmm5
	VPOR	%xmm9,%xmm5,%xmm5	# new res[0] xmm5
	# construct x;
	VPADDQ	.L_959(%rip),%xmm7,%xmm7	# xmm7 rexp
	VPSLLQ	$52,%xmm7,%xmm6
	VPSRLQ $12,%xmm5,%xmm8				# xmm8 mant
	VPOR	%xmm8,%xmm6,%xmm8
	VPOR	%xmm8,%xmm3,%xmm8		# xmm8 x
	VINSERTF128 $1,%xmm8,%ymm14,%ymm0
   	VMULPD .L_piby2_lead(%rip),%ymm0,%ymm0

	pop %r13
	pop %r12

	.endm





.macro  call_remainder_piby2_f  Lname

    lea  .L__Pibits(%rip),%r9
    #xexp = (x >> 52) - 1023
    movd %xmm0, %r11
    mov  %r11,  %rcx
    shr  $52,   %r11
    sub  $1023, %r11      #r11 = xexp = exponent of input x
    #calculate the last byte from which to start multiplication
    #last = 134 - (xexp >> 3)
    mov  %r11, %r10
    shr  $3,   %r10
    sub  $134, %r10       #r10 = -last
    neg  %r10             #r10 = last
    #load 64 bits of 2_by_pi
    mov  (%r9, %r10),%rax
    #mov  %rdx, %rdi      # save address of region since mul modifies rdx
    #mantissa of x = ((x << 12) >> 12) | implied bit
    shl  $12,   %rcx
    shr  $12,   %rcx      #rcx = mantissa part of input x
    bts  $52,   %rcx      #add the implied bit as well
    #load next 128 bits of 2_by_pi
    add  $8, %r10        #increment to next 8 bytes of 2_by_pi
    movdqu (%r9, %r10), %xmm0
    #do three 64-bit multiplications with mant of x
    mul  %rcx
    mov  %rax, %r8       #r8 = last 64 bits of multiplication = res1[2]
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0, %rax
    mul  %rcx
    #resexp = xexp & 7
    and  $7, %r11        #r11 = resexp = xexp & 7 = last 3 bits
    psrldq $8, %xmm0
    add  %r10, %rax      # add the previous carry
    adc  $0,   %rdx
    mov  %rax, %r9       #r9 = next 64 bits of multiplication = res1[1]
    mov  %rdx, %r10      #r10 = carry
    movd %xmm0,%rax
    mul  %rcx
    add  %rax, %r10      #r10 = most significant 64 bits = res1[0]
    #find the region
    #last three bits ltb = most sig bits >> (54 - resexp)) // decimal point in last 18 bits == 8 lsb's in first 64 bits and 8 msb's in next 64 bits
    #point_five = ltb & 0x1
    #region = ((ltb >> 1) + point_five) & 3
    mov $54,  %rcx
    mov %r10, %rax
    sub %r11, %rcx
    xor %rdx, %rdx       #rdx = sign of x(i.e first part of x * 2bypi)
    shr %cl,  %rax
    jnc  .L__no_point_five_f\Lname
    ##if there is carry.. then negate the result of multiplication
    not  %r10
    not  %r9
    not  %r8
    mov $0x8000000000000000,%rdx
.p2align 4
.L__no_point_five_f\Lname:
    adc $0,   %rax
    and $3,   %rax
    VMOVD %eax, %xmm4   #store region to xmm4
    #calculate the number of integer bits and zero them out
    mov %r11, %rcx
    add $10,  %rcx  #rcx = no. of integer bits
    shl %cl,  %r10
    shr %cl,  %r10  #r10 contains only mant bits
    sub $64,  %rcx  #form the exponent
    mov %rcx, %r11
    #find the highest set bit
    bsr %r10,%rcx
    jnz  .L__form_mantissa_f\Lname
    mov %r9,%r10
    mov %r8,%r9
    mov $0, %r8
    bsr %r10,%rcx  #rcx = hsb
    sub $64, %r11
.align 16
.L__form_mantissa_f\Lname:
    add %rcx,%r11  #for exp of x
    sub $52,%rcx #rcx = no. of bits to shift in r10
    cmp $0,%rcx
    jl  .L__hsb_below_52_f\Lname
    je  .L__form_numbers_f\Lname
    #hsb above 52
    mov %r10,%r8 #previous contents of r8 not required
    shr %cl,%r10 #r10 = mantissa of x with hsb at 52
    shr %cl,%r9  #make space for bits from r10
    sub $64,%rcx
    neg %rcx     #rvx = no of bits to shift r10 to move those bits to r9
    shl %cl,%r8
    or  %r8,%r9  #r9 = mantissa bits of xx
    jmp .L__form_numbers_f\Lname
.align 16
.L__hsb_below_52_f\Lname:
    neg %rcx
    mov %r9,%rax
    shl %cl,%r10
    shl %cl,%r9
    sub $64,%rcx
    neg %rcx
    shr %cl,%rax
    or  %rax,%r10
    shr %cl,%r8
    or  %r8,%r9
.align 16
.L__form_numbers_f\Lname:
    add $1023, %r11
    btr $52,%r10     #remove the implied bit
    mov %r11,%rcx
    or  %rdx,%r10    #put the sign
    shl $52, %rcx
    or  %rcx,%r10    #x is in r10
    movd %r10,%xmm0  #xmm0 = x
    VMULSD .L_piby2_lead(%rip),%xmm0,%xmm0
	.endm


.macro  range_e_5_2f_s


	VANDPD .L__sign_mask(%rip),%xmm0,%xmm1		 	#clear the sign

	VMOVAPD	 .L_twobypi(%rip),%xmm8
	VFMADDPD .L_point_five(%rip),%xmm8,%xmm1,%xmm8
	VCVTTPD2DQ %xmm8,%xmm8
	VPMOVSXDQ %xmm8,%xmm6

	VANDPD	.L_int_three(%rip),%xmm6,%xmm11			# region xmm11
	VSHUFPS $8,%xmm6,%xmm6,%xmm6
	VCVTDQ2PD	%xmm6,%xmm6
	VFNMADDPD %xmm0,.L_piby2_1(%rip),%xmm6,%xmm2		# xmm2 rhead
	VMULPD	.L_piby2_1tail(%rip),%xmm6,%xmm3			# xmm3 rtail
	VANDPD .L__inf_mask_64(%rip),%xmm2,%xmm4
	VPSRLQ $52,%xmm4,%xmm4
	VPSRLQ $52,%xmm0,%xmm5
	VPSUBQ	%xmm4,%xmm5,%xmm4						# expdiff xmm4

	VPCOMGTUQ  .L_val_15(%rip),%xmm4,%xmm8			# mask_expdiff15 xmm8
	VPTEST %xmm8,%xmm8
	JZ exit_range_e5

	VMOVAPD %xmm2,%xmm5
	VMULPD	.L_piby2_2(%rip),%xmm6,%xmm3 			# rtail_1 xmm3
	VSUBPD	%xmm3,%xmm5,%xmm2						# rhead_1 xmm2

	VSUBPD	%xmm2,%xmm5,%xmm1
	VSUBPD	%xmm3,%xmm1,%xmm1
	VXORPD	.L_signbit(%rip),%xmm1,%xmm1
	VFMADDPD  %xmm1,.L_piby2_2tail(%rip),%xmm6,%xmm3	# rtail_1 xmm5


	VPCOMGTUQ  .L_val_48(%rip),%xmm4,%xmm8			# mask_expdiff15 xmm8
	VPTEST %xmm8,%xmm8
	JZ exit_range_e5

	VMOVAPD %xmm2,%xmm5
	VMULPD	.L_piby2_3(%rip),%xmm6,%xmm3 			# rtail_1 xmm3
	VSUBPD	%xmm3,%xmm5,%xmm2						# rhead_1 xmm2

	VSUBPD	%xmm2,%xmm5,%xmm1
	VSUBPD	%xmm3,%xmm1,%xmm1
	VXORPD	.L_signbit(%rip),%xmm1,%xmm1
	VFMADDPD  %xmm1,.L_piby2_3tail(%rip),%xmm6,%xmm3	# rtail_1 xmm5

exit_range_e5:
	VSUBPD %xmm3,%xmm2,%xmm10						# r_1  xmm10
	VSUBPD %xmm10,%xmm2,%xmm4
	VSUBPD %xmm3,%xmm4,%xmm1						# rr_1 xmm4
	VMOVAPD %xmm10,%xmm0

	.endm


.macro  range_e_5_s4f


	VANDPD .L__sign_mask(%rip),%ymm0,%ymm1		 	#clear the sign

	VMOVAPD	 .L_twobypi(%rip),%ymm8
	VFMADDPD .L_point_five(%rip),%ymm8,%ymm1,%ymm8
	VCVTTPD2DQ %ymm8,%xmm8
	VPMOVSXDQ %xmm8,%xmm9
	VPUNPCKHQDQ %xmm8,%xmm8,%xmm8
	VPMOVSXDQ %xmm8,%xmm8

	VINSERTF128 $1,%xmm8,%ymm9,%ymm6

	VANDPD	.L_int_three(%rip),%ymm6,%ymm11			# region ymm11
	VEXTRACTF128 $1,%ymm6,%xmm7
	VSHUFPS $8,%xmm6,%xmm6,%xmm6
	VSHUFPS $8,%xmm7,%xmm7,%xmm7
	VPUNPCKLQDQ %xmm7,%xmm6,%xmm6
	VCVTDQ2PD	%xmm6,%ymm6
	VFNMADDPD %ymm0,.L_piby2_1(%rip),%ymm6,%ymm2		# ymm2 rhead
	VMULPD	.L_piby2_1tail(%rip),%ymm6,%ymm3			# ymm3 rtail

	VSUBPD %ymm3,%ymm2,%ymm10						# r_1  xmm10
	VSUBPD %ymm10,%ymm2,%ymm4
	VSUBPD %ymm3,%ymm4,%ymm1						# rr_1 xmm4
	VMOVAPD %ymm10,%ymm0


	.endm



.macro  range_e_5_sf

	VANDPD .L__sign_mask(%rip),%xmm0,%xmm1		 	#clear the sign
	VMOVAPD	 .L_twobypi(%rip),%xmm2
	VFMADDSD .L_point_five(%rip),%xmm2,%xmm1,%xmm2
	VCVTTPD2DQ %xmm2,%xmm2
	VPMOVSXDQ %xmm2,%xmm1
	VANDPD   .L_int_three(%rip),%xmm1,%xmm4            # region xmm4
	VSHUFPS    $8, %xmm1,%xmm1,%xmm1
	VCVTDQ2PD    %xmm1,%xmm1
	VFNMADDSD  %xmm0,.L_piby2_1(%rip), %xmm1,%xmm2        # xmm2 rhead
	VMULSD    .L_piby2_1tail(%rip), %xmm1,%xmm3           # xmm3 rtail
	VSUBSD    %xmm3,%xmm2,%xmm0                        # r_1  xmm0
	VSUBSD    %xmm0,%xmm2,%xmm2
	VSUBSD    %xmm3,%xmm2,%xmm1

	.endm

.macro  range_e_5_s


	VANDPD .L__sign_mask(%rip),%xmm0,%xmm1		 	#clear the sign
	VMOVAPD	 .L_twobypi(%rip),%xmm2

	VFMADDSD .L_point_five(%rip),%xmm2,%xmm1,%xmm2
	VCVTTPD2DQ %xmm2,%xmm2

	VPMOVSXDQ %xmm2,%xmm4
	VANDPD	.L_int_three(%rip),%xmm4,%xmm7			# region xmm7

	VSHUFPS $8,%xmm4,%xmm4,%xmm4
	VCVTDQ2PD	%xmm4,%xmm4

	VFNMADDSD %xmm0,.L_piby2_1(%rip),%xmm4,%xmm2		# xmm2 rhead
	VMULSD	.L_piby2_1tail(%rip),%xmm4,%xmm3			# xmm3 rtail

	VANDPD .L__inf_mask_64(%rip),%xmm2,%xmm1
	VPSRLQ $52,%xmm1,%xmm1
	VPSRLQ $52,%xmm0,%xmm5
	VPSUBQ	%xmm1,%xmm5,%xmm1						# expdiff xmm4

	VMOVD %xmm1,%rax

	cmp .L_val_15(%rip),%eax
	JB exit_range_e5

	VMOVAPD %xmm2,%xmm5
	VMULSD	.L_piby2_2(%rip),%xmm4,%xmm3 			# rtail_1 xmm3
	VSUBSD	%xmm3,%xmm5,%xmm2						# rhead_1 xmm2
	VSUBSD	%xmm2,%xmm5,%xmm1
	VSUBSD	%xmm3,%xmm1,%xmm1
	VXORPD	.L_signbit(%rip),%xmm1,%xmm1
	VFMADDSD  %xmm1,.L_piby2_2tail(%rip),%xmm4,%xmm3	# rtail_1 xmm5

	cmp .L_val_48(%rip),%eax
	JB exit_range_e5

	VMOVAPD %xmm2,%xmm5
	VMULSD	.L_piby2_3(%rip),%xmm4,%xmm3 			# rtail_1 xmm3
	VSUBSD	%xmm3,%xmm5,%xmm2						# rhead_1 xmm2

	VSUBSD	%xmm2,%xmm5,%xmm1
	VSUBSD	%xmm3,%xmm1,%xmm1
	VXORPD	.L_signbit(%rip),%xmm1,%xmm1
	VFMADDSD  %xmm1,.L_piby2_3tail(%rip),%xmm4,%xmm3	# rtail_1 xmm5

exit_range_e5:
	VSUBSD %xmm3,%xmm2,%xmm0						# r_1  xmm10
	VSUBSD %xmm0,%xmm2,%xmm4
	VSUBSD %xmm3,%xmm4,%xmm1

	.endm



.data
.align 32
.L_Cos:
    .quad    0x3fa5555555555555                       # 0.0416667     c1
    .quad    0x3fa5555555555555
    .quad    0x3fa5555555555555                       # 0.0416667     c1
    .quad    0x3fa5555555555555

    .quad    0xbf56c16c16c16967                       # -0.00138889   c2
    .quad    0xbf56c16c16c16967
    .quad    0xbf56c16c16c16967                       # -0.00138889   c2
    .quad    0xbf56c16c16c16967

    .quad    0x3EFA01A019F4EC91                       # 2.48016e-005  c3
    .quad    0x3EFA01A019F4EC91
    .quad    0x3EFA01A019F4EC91                       # 2.48016e-005  c3
    .quad    0x3EFA01A019F4EC91


    .quad    0xbE927E4FA17F667B                       # -2.75573e-007 c4
    .quad    0xbE927E4FA17F667B
    .quad    0xbE927E4FA17F667B                       # -2.75573e-007 c4
    .quad    0xbE927E4FA17F667B

    .quad    0x3E21EEB690382EEC                       # 2.08761e-009  c5
    .quad    0x3E21EEB690382EEC
    .quad    0x3E21EEB690382EEC                       # 2.08761e-009  c5
    .quad    0x3E21EEB690382EEC

    .quad    0xbDA907DB47258AA7                       # -1.13826e-011 c6
    .quad    0xbDA907DB47258AA7
    .quad    0xbDA907DB47258AA7                       # -1.13826e-011 c6
    .quad    0xbDA907DB47258AA7


.align 32
.L_Sin:
    .quad    0xbfc5555555555555                       # -0.166667     s1
    .quad    0xbfc5555555555555
    .quad    0xbfc5555555555555                       # -0.166667     s1
    .quad    0xbfc5555555555555

    .quad    0x3f81111111110bb3                       # 0.00833333    s2
    .quad    0x3f81111111110bb3
    .quad    0x3f81111111110bb3                       # 0.00833333    s2
    .quad    0x3f81111111110bb3

    .quad    0xbf2a01a019e83e5c                       # -0.000198413  s3
    .quad    0xbf2a01a019e83e5c
    .quad    0xbf2a01a019e83e5c                       # -0.000198413  s3
    .quad    0xbf2a01a019e83e5c

    .quad    0x3ec71de3796cde01                       # 2.75573e-006  s4
    .quad    0x3ec71de3796cde01
    .quad    0x3ec71de3796cde01                       # 2.75573e-006  s4
    .quad    0x3ec71de3796cde01

    .quad    0xbe5ae600b42fdfa7                       # -2.50511e-008 s5
    .quad    0xbe5ae600b42fdfa7
    .quad    0xbe5ae600b42fdfa7                       # -2.50511e-008 s5
    .quad    0xbe5ae600b42fdfa7

    .quad    0x3de5e0b2f9a43bb8                       # 1.59181e-010  s6
    .quad    0x3de5e0b2f9a43bb8
    .quad    0x3de5e0b2f9a43bb8                       # 1.59181e-010  s6
    .quad    0x3de5e0b2f9a43bb8



.align 32
.L_Sincos:
    .quad    0x0bfc5555555555555                      # -0.16666666666666666    s1
    .quad    0x03fa5555555555555                      # 0.041666666666666664    c1
    .quad    0x03f81111111110bb3                      # 0.00833333333333095     s2
    .quad    0x0bf56c16c16c16967                      # -0.0013888888888887398  c2
    .quad    0x0bf2a01a019e83e5c                      # -0.00019841269836761127 s3
    .quad    0x03efa01a019f4ec90                      # 2.4801587298767041E-05  c3
    .quad    0x03ec71de3796cde01                      # 2.7557316103728802E-06  s4
    .quad    0x0be927e4fa17f65f6                      # -2.7557317272344188E-07 c4
    .quad    0x0be5ae600b42fdfa7                      # -2.5051132068021698E-08 s5
    .quad    0x03e21eeb69037ab78                      # 2.0876146382232963E-09  c6
    .quad    0x03de5e0b2f9a43bb8                      # 1.5918144304485914E-10  s6
    .quad    0x0bda907db46cc5e42                      # -1.1382639806794487E-11 c7

.align 32
.L_tanf:
	.quad 0x3FD8A8B0DA56CB17
	.quad 0x3FD8A8B0DA56CB17
	.quad 0x3FD8A8B0DA56CB17
	.quad 0x3FD8A8B0DA56CB17

	.quad 0x3F919DBA6EFD6AAD
	.quad 0x3F919DBA6EFD6AAD
	.quad 0x3F919DBA6EFD6AAD
	.quad 0x3F919DBA6EFD6AAD

	.quad 0x3FF27E84A3E73A2E
	.quad 0x3FF27E84A3E73A2E
	.quad 0x3FF27E84A3E73A2E
	.quad 0x3FF27E84A3E73A2E

	.quad 0xBFE07266D7B3511B
	.quad 0xBFE07266D7B3511B
	.quad 0xBFE07266D7B3511B
	.quad 0xBFE07266D7B3511B

	.quad 0x3F92E29003C692D9
	.quad 0x3F92E29003C692D9
	.quad 0x3F92E29003C692D9
	.quad 0x3F92E29003C692D9


.L_e_5:
		.quad 0x415312d000000000
		.quad 0x415312d000000000
		.quad 0x415312d000000000
		.quad 0x415312d000000000

.L_signbit:
			.quad 0x8000000000000000
			.quad 0x8000000000000000
			.quad 0x8000000000000000
			.quad 0x8000000000000000

.L_nan:
		.quad 0x7ff8000000000000
		.quad 0x7ff8000000000000
		.quad 0x7ff8000000000000
		.quad 0x7ff8000000000000


.L__sign_mask: .quad 0x7FFFFFFFFFFFFFFF
			   .quad 0x7FFFFFFFFFFFFFFF
			   .quad 0x7FFFFFFFFFFFFFFF
			   .quad 0x7FFFFFFFFFFFFFFF

.L__allone_mask: .quad 0xFFFFFFFFFFFFFFFF
			   .quad 0xFFFFFFFFFFFFFFFF
			   .quad 0xFFFFFFFFFFFFFFFF
			   .quad 0xFFFFFFFFFFFFFFFF


.L_one:
	.quad    0x3FF0000000000000
	.quad    0x3FF0000000000000
	.quad    0x3FF0000000000000
	.quad    0x3FF0000000000000



.L_int_one:
	.quad    0x0000000000000001
	.quad    0x0000000000000001
	.quad    0x0000000000000001
	.quad    0x0000000000000001




.L_int_three:
	.quad    0x0000000000000003
	.quad    0x0000000000000003
	.quad    0x0000000000000003
	.quad    0x0000000000000003


.L_int_four:
	.quad    0x0000000000000004
	.quad    0x0000000000000004
	.quad    0x0000000000000004
	.quad    0x0000000000000004


.L_point_five:
	 .quad   0x3FE0000000000000
	 .quad   0x3FE0000000000000
	 .quad   0x3FE0000000000000
	 .quad   0x3FE0000000000000


.L_twobypi:
			.quad 0x3FE45F306DC9C883
			.quad 0x3FE45F306DC9C883
			.quad 0x3FE45F306DC9C883
			.quad 0x3FE45F306DC9C883


.L_piby2_3tail:
			.quad 0x397B839A252049C1
			.quad 0x397B839A252049C1
			.quad 0x397B839A252049C1
			.quad 0x397B839A252049C1



.L_piby2_3:
			.quad 0x3BA3198A2E000000
			.quad 0x3BA3198A2E000000
			.quad 0x3BA3198A2E000000
			.quad 0x3BA3198A2E000000



.L_piby2_1:
			.quad 0x3FF921FB54400000
			.quad 0x3FF921FB54400000
			.quad 0x3FF921FB54400000
			.quad 0x3FF921FB54400000



.L_piby2_2:
			.quad 0x3DD0B4611A600000
			.quad 0x3DD0B4611A600000
			.quad 0x3DD0B4611A600000
			.quad 0x3DD0B4611A600000

.L_point_166:
			.quad 0x3FC5555555555555
			.quad 0x3FC5555555555555
			.quad 0x3FC5555555555555
			.quad 0x3FC5555555555555

.L_point_333:
			.quad 0x3FD5555555555555
			.quad 0x3FD5555555555555
			.quad 0x3FD5555555555555
			.quad 0x3FD5555555555555

.L_n_one:
		.quad 0xBFF0000000000000
		.quad 0xBFF0000000000000
		.quad 0xBFF0000000000000
		.quad 0xBFF0000000000000


.L_piby2_1tail:
			.quad 0x3DD0B4611A626331
			.quad 0x3DD0B4611A626331
			.quad 0x3DD0B4611A626331
			.quad 0x3DD0B4611A626331


.L_piby2_2tail:
			.quad 0x3BA3198A2E037073
			.quad 0x3BA3198A2E037073
			.quad 0x3BA3198A2E037073
			.quad 0x3BA3198A2E037073


.L_mask_5piby4:
				.quad 0x400f6a7a2955385e
				.quad 0x400f6a7a2955385e
				.quad 0x400f6a7a2955385e
				.quad 0x400f6a7a2955385e


.L_mask_3piby4:
				.quad 0x4002d97c7f3321d2
				.quad 0x4002d97c7f3321d2
				.quad 0x4002d97c7f3321d2
				.quad 0x4002d97c7f3321d2


.L_mask_9piby4:
				.quad 0x401c463abeccb2bb
				.quad 0x401c463abeccb2bb
				.quad 0x401c463abeccb2bb
				.quad 0x401c463abeccb2bb


.L_mask_7piby4:
				.quad 0x4015fdbbe9bba775
				.quad 0x4015fdbbe9bba775
				.quad 0x4015fdbbe9bba775
				.quad 0x4015fdbbe9bba775


.L__inf_mask_64: .quad 0x7FF0000000000000
				 .quad 0x7FF0000000000000
				 .quad 0x7FF0000000000000
				 .quad 0x7FF0000000000000

.L__inf_mask_32:  .long  0x7F800000
                  .long  0x7F800000
		  .quad 0x7F8000007F800000
		  .quad 0x7F8000007F800000
		  .quad 0x7F8000007F800000





.L_val_15:
				.quad 0xF
				.quad 0xF
				.quad 0xF
				.quad 0xF

.L_val_48:
				.quad 0x30
				.quad 0x30
				.quad 0x30
				.quad 0x30


.L_int_two:
	.quad    0x0000000000000002
	.quad    0x0000000000000002
	.quad    0x0000000000000002
	.quad    0x0000000000000002


.L_piby2_lead:
		.quad  0x3ff921fb54442d18
		.quad  0x3ff921fb54442d18
		.quad  0x3ff921fb54442d18
		.quad  0x3ff921fb54442d18




.align 16



.L_mask_3fe:
				.quad 0x3fe921fb54442d18
				.quad 0x3fe921fb54442d18


.L_mask_3f_9:
 				.quad 0x3fe921fb54442d19
 				.quad 0x3fe921fb54442d19


.L_mask_3e4:
				.quad 0x3e40000000000000
				.quad 0x3e40000000000000


.L_mask_3f2:
				.quad 0x3f20000000000000
				.quad 0x3f20000000000000


.L_mask_3f8: 	.quad 0x3f80000000000000
		.quad 0x3f80000000000000



.L_tan_c1:
			.quad 0x3FD7D50F6638564A
			.quad 0x3FD7D50F6638564A

.L_tan_c2:
			.quad 0xBF977C24C7569ABB
			.quad 0xBF977C24C7569ABB

.L_tan_c3:
			.quad 0x3F2D5DAF289C385A
			.quad 0x3F2D5DAF289C385A


.L_tan_c4:
			.quad 0x3FF1DFCB8CAA40B8
			.quad 0x3FF1DFCB8CAA40B8

.L_tan_c5:
			.quad 0xBFE08046499EB90F
			.quad 0xBFE08046499EB90F

.L_tan_c6:
			.quad 0x3F9AB0F4F80A0ACF
			.quad 0x3F9AB0F4F80A0ACF

.L_tan_c7:
			.quad 0x3F2E7517EF6D98F8
			.quad 0x3F2E7517EF6D98F8

.L_half_mask:
			.quad 0xffffffff00000000
			.quad 0xffffffff00000000


.L_point_68:
			.quad 0x3FE5C28F5C28F5C3
			.quad 0x3FE5C28F5C28F5C3

.L_n_point_68:
			.quad 0xBFE5C28F5C28F5C3
			.quad 0xBFE5C28F5C28F5C3



.L_piby4_lead:
			.quad 0x3FE921FB54442D18
			.quad 0x3FE921FB54442D18

.L_piby4_tail:
			.quad 0x3C81A62633145C06
			.quad 0x3C81A62633145C06


.L_two:
	.quad    0x4000000000000000
	.quad    0x4000000000000000


.L__mant_mask_64:
		.quad 0x000fffffffffffff
		.quad 0x000fffffffffffff

.L__imp_mask_64:
		.quad 0x0010000000000000
		.quad 0x0010000000000000


.L_157:
		.quad 0x9D
		.quad 0x9D

.L_8:
		.quad 0x8
		.quad 0x8

.L_10:
		.quad 0xA
		.quad 0xA
.L_959:

		.quad 0x3BF
		.quad 0x3BF

.L_116:
		.quad 0x74
		.quad 0x74

.L_fff800:
		.quad 0xfffffffff8000000
		.quad 0xfffffffff8000000

.L_piby2_part1:
		.quad 0x3ff921fb50000000
		.quad 0x3ff921fb50000000
.L_piby2_part2:
		 .quad 0x3e5110b460000000
		 .quad 0x3e5110b460000000

.L_piby2_part3:
		.quad 0x3c91a62633145c06
		.quad 0x3c91a62633145c06

.L_1023:
		.quad 0x3FF
		.quad 0x3FF


.L_23:
		.quad 0x17
		.quad 0x17

.L_53:
		.quad 0x35
		.quad 0x35

.L_64:
		.quad 0x40
		.quad 0x40


.type	.L__Pibits, @object
.size	.L__Pibits, 158
.L__Pibits:
    .byte 224,241,27,193,12,88,33,116,53,126,196,126,237,175,169,75,74,41,222,231,28,244,236,197,151,175,31,235,158,212,181,168,127,121,154,253,24,61,221,38,44,159,60,251,217,180,125,180,41,104,45,70,188,188,63,96,22,120,255,95,226,127,236,160,228,247,46,126,17,114,210,231,76,13,230,88,71,230,4,249,125,209,154,192,113,166,19,18,237,186,212,215,8,162,251,156,166,196,114,172,119,248,115,72,70,39,168,187,36,25,128,75,55,9,233,184,145,220,134,21,239,122,175,142,69,249,7,65,14,241,100,86,138,109,3,119,211,212,71,95,157,240,167,84,16,57,185,13,230,139,2,0,0,0,0,0,0,0




