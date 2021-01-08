#
# Copyright (C) 2008-2021 Advanced Micro Devices, Inc. All rights reserved.
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

# fabsf.S
#
# An implementation of the fabsf libm function.
#
# Prototype:
#
#     float fabsf(float x);
#

#
#   Algorithm: AND the Most Significant Bit of the 
#              single precision number with 0 to get the 
#              floating point absolute.
#

#include "fn_macros.h"
#include "weak_macros.h"
#define fname FN_PROTOTYPE_BAS64(fabsf)
#define fname_special _fabsf_special@PLT




#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.align 16
.p2align 4,,15
.globl fname
.type fname,@function
fname:
    #input is in xmm0, which contains the final result also.

	sub $0x18,%rsp
	movdqa %xmm0,%xmm1
	andps .L__fabsf_and_mask(%rip),%xmm1
	movd %xmm1,%r8d
	cmp .L__inf_mask(%rip),%r8d
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
.L__fabsf_and_mask:          .long 0x7FFFFFFF
                             .long 0x0
                             .quad 0x0


.L__inf_mask:		     .long 0x7f800000
			      .long 0x0
                              .quad 0x0


