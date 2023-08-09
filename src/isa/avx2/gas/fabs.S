#
# Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

# fabs.S
#
# An implementation of the fabs libm function.
#
# Prototype:
#
#     double fabs(double x);
#

#
#   Algorithm: AND the Most Significant Bit of the
#              double precision number with 0 to get the
#              floating point absolute.
#

#include "fn_macros.h"
#define fname ALM_PROTO_FMA3(fabs)
#define fname_special _fabs_special@PLT

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
ALM_FUNC_TYPE_ASM(f_name)
fname:
    #input is in %xmm0, which contains the final result also.
	sub $0x18,%rsp
	movdqa %xmm0,%xmm1
	andpd .L__fabs_and_mask(%rip),%xmm1
	movd %xmm1,%r8
	cmp .L__inf_mask(%rip),%r8
	jg  L__rais_exception
	movdqa %xmm1,%xmm0
	add     $0x18,%rsp
	ret
L__rais_exception:
	call fname_special
	add    $0x18,%rsp
	ret


.section .rodata

.align 16
.L__fabs_and_mask:          .quad 0x7FFFFFFFFFFFFFFF
                            .quad 0x0


.L__inf_mask:    .quad 0x7ff0000000000000
		.quad 0x7ff0000000000000


