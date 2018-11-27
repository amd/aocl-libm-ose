/*

   vrd2_exp_fma3
   =================
   Returns the value of e^x for two 64-bit floating point numbers packed in an xmm register

   Method:
   =======

   We use the identity,
   e^x = 2 ^ (x/ln(2)) = 2^(x*(64/ln(2))/64)        --- (1)

   Choose 'n' and 'f' such that
   x*(64/ln(2)) = n + f        | n is an integer    --- (2)
                               | |f| <= 0.5

   Choose 'm' such that
   n = 64 * m + j              | 0 <= j <64         --- (3)

   From (1), (2) and (3),
   e^x = 2^((64*m + j + f)/64)
       = (2^m) * (2^(j/64)) * 2^(f/64)
       = (2^m) * (2^(j/64)) * e^(f*(ln(2)/64))

   From (2),
   f = x*(64/ln(2)) - n

   Let r  = f*(ln(2)/64) = x - n*(ln(2)/64)

   Thus,
   e^x = (2^m) * (2^(j/64)) * e^r

   Values of (2^(j/64)) are precomputed

   e^r is calculated using the Taylor series expansion,
   e^r = 1 + r + (r^2)/2! + (r^3)/3! + (r^4)/4! + (r^5)/5! + (r^6)/6! + (r^7)/7!

*/
#include "fn_macros.h"
#include "exp_tables.h"

#define fname FN_PROTOTYPE_FMA3(vrd2_exp)
#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    
    vmovdqa      %xmm0, %xmm10   # save for later use

    # x * (64/ln(2))
    vmulpd       .L__real_64_by_log2(%rip), %xmm0,%xmm3

    # n = int( x * (64/ln(2)) )
    vcvttpd2dq  %xmm3, %xmm4   #xmm4 = 0,0,(int)n1,(int)n0
    vcvtdq2pd   %xmm4, %xmm3   #xmm3 = (double)n1,n0    
    vmovd       %xmm4, %rcx    #rcx = (int)n1,n0
    
    # r1 = x - n * ln(2)/64 head    
    # r2 = - n * ln(2)/64 tail    
    # r  = r1+r2 
    vmovdqa %xmm3, %xmm2
    vfmadd132pd .L__log2_by_64_mhead(%rip),%xmm0,%xmm2
    vfmadd231pd .L__log2_by_64_mtail(%rip),%xmm3,%xmm2  #xmm2 = r_1,r_0
       
    #j = n & 0x3f    
    mov         $0x0000003f0000003f, %rax
    and         %rcx, %rax     #rax = j1,j0
    mov         %eax, %ecx     #rcx = j0
    shr         $32, %rax      #rax = j1        
    # m = (n - j) / 64      
    vpsrad         $6, %xmm4,%xmm5      #xmm5 = 0,0,m1,m0

    # Computing q=r + r^2*1/2 + r^3*1/6 + r^4 *1/24 + r^5*1/120 + r^6*1/720 + r^7/5040
    # Using Estrin's method
    # Upto 7th degree
    vmovapd .L__real_1_by_6(%rip),%xmm0                 # xmm0 = 1/6
    vfmadd213pd .L__real_1_by_2(%rip),%xmm2,%xmm0       # xmm0 = A0 = 1/2+(1/6)*r
    vmovapd .L__real_1_by_120(%rip),%xmm6               # xmm6 = 1/120
    vfmadd213pd .L__real_1_by_24(%rip),%xmm2,%xmm6      # xmm6 = A1 = 1/24+(1/120)*r
    vmovapd .L__real_1_by_5040(%rip),%xmm7              # xmm7 = 1/5040
    vfmadd213pd .L__real_1_by_720(%rip),%xmm2,%xmm7     # xmm7 = A2 = 1/720+(1/5040)*r
    vmulpd %xmm2,%xmm2,%xmm8                            # xmm8 = r^2
    vfmadd213pd %xmm2,%xmm8,%xmm0                       # xmm0 = B0 = r + A0 * r^2
    vfmadd213pd %xmm6,%xmm8,%xmm7                       # xmm7 = B1 = A1 + A2 * r^2
    vmulpd %xmm8,%xmm8,%xmm8                            # xmm8 = r^4
    vfmadd231pd %xmm8,%xmm7,%xmm0                       # xmm0 = B0 + B1 * r^4

    # (f)*(q) + f2 + f1
    lea         .L__two_to_jby64_table(%rip), %rdx        
    lea         .L__two_to_jby64_tail_table(%rip), %r11       
    lea         .L__two_to_jby64_head_table(%rip), %r10    
    vmovsd      (%rdx,%rcx,8), %xmm2
    vmovhpd     (%rdx,%rax,8),%xmm2,%xmm1  #xmm1 = f_1,f_0    
    vmovsd      (%r11,%rcx,8), %xmm2    
    vmovhpd     (%r11,%rax,8), %xmm2,%xmm3 #xmm3 = f2_1,f2_0
    vmovsd      (%r10,%rcx,8), %xmm4
    vmovhpd     (%r10,%rax,8), %xmm4,%xmm2 #xmm2 = f1_1,f1_0           
    vfmadd231pd    %xmm1,%xmm0,%xmm3    #xmm3 = f*q + f2
    vaddpd      %xmm3,%xmm2,%xmm0   
    
    # normal results
    vpmovsxdq   %xmm5,%xmm4       #xmm4 = m1,m0
    vpsllq      $52,%xmm4,%xmm5   #xmm5 = 2^m1,2^m0
    vpaddq      %xmm5,%xmm0,%xmm1 #xmm1 = normal results  
   
    #check and process denormals    
    vcmppd      $1,.L__real_one(%rip),%xmm0,%xmm3 #check e^r < 1
    vpcmpeqq    .L__m1022(%rip),%xmm5,%xmm2   #check m == -1022
    vpand       %xmm2,%xmm3,%xmm3  #xmm3 = ( e^r < 1 and m = -1022 )     
	
   # vpcomltq    .L__m1022(%rip),%xmm5,%xmm2   #check m < -1022    
     vmovdqa  .L__m1022(%rip),%xmm2
     vpcmpgtq %xmm5,%xmm2,%xmm2  #check m < -1022
   
    vpor        %xmm3,%xmm2,%xmm2   #xmm2 = mask for inputs requiring denormal processing
    vpmovmskb   %xmm2,%eax
    cmp         $0,%eax
    je          .L__check_min   #jmp to avoid any accidental intermediate denormal results
    vpaddq      .L__ulong_1074(%rip),%xmm4,%xmm3 #xmm3 = 1074 + m     
	
	vunpckhpd %xmm3,%xmm3,%xmm9 #store high to xmm9
	vmovdqa .L__ulong_1(%rip),%xmm5
	vpsllq %xmm3,%xmm5,%xmm4
	vpsllq %xmm9,%xmm5,%xmm9
	vunpcklpd %xmm9,%xmm4,%xmm4
	vmulpd %xmm4,%xmm0,%xmm3 #xmm3 = results for denormal numbers
	vandnpd %xmm1,%xmm2,%xmm5
	vandpd  %xmm2,%xmm3,%xmm1
	vorpd   %xmm5,%xmm1,%xmm1

  
.L__check_min:  
    ##special case for any x < min_exp_arg,remove this code if the above code takes care of this
    vcmppd       $2,.L__min_exp_arg(%rip),%xmm10,%xmm2 #cmp x<= min_exp_arg
    vpxor        %xmm3,%xmm3,%xmm4  #make zeros to put in place of x<= min_exp_arg

    VANDNPD %xmm1, %xmm2, %xmm9
    VANDPD %xmm2, %xmm4, %xmm1
    VORPD %xmm9, %xmm1, %xmm1



    vcmppd       $2,.L__denormal_tiny_threshold(%rip),%xmm10,%xmm3
    vpandn       %xmm3,%xmm2,%xmm4
    vmovdqa      .L__real_smallest_denormal(%rip), %xmm2
    #vpcmov       %xmm4,%xmm1,%xmm2,%xmm1 #xmm1 = normal results

    VANDNPD %xmm1, %xmm4, %xmm9
    VANDPD %xmm4, %xmm2, %xmm1
    VORPD %xmm9, %xmm1, %xmm1



        
    ##special case for any x > max_exp_arg,remove this code if the above code takes care of this
    vcmppd       $1,.L__max_exp_arg(%rip),%xmm10,%xmm2 #cmp x < max_exp_arg
    vmovdqa     .L__real_inf(%rip), %xmm3  #inf to put in place of any x >= max_exp_arg
    #vpcmov       %xmm2,%xmm3,%xmm1,%xmm1 #xmm1 = normal results
    VANDNPD %xmm3, %xmm2, %xmm9
    VANDPD %xmm2, %xmm1, %xmm1
    VORPD %xmm9, %xmm1, %xmm1
    
    ##special case for any x = nan,remove this code if the above code takes care of this
    vcmppd       $0x0,%xmm10,%xmm10,%xmm2    
    vaddpd       %xmm10, %xmm10,%xmm3  #make qnan to put in place of any x =nan
    #vpcmov       %xmm2,%xmm3,%xmm1,%xmm0 #xmm0 = normal results
    VANDNPD %xmm3, %xmm2, %xmm9
    VANDPD %xmm2, %xmm1, %xmm0
    VORPD %xmm9, %xmm0, %xmm0    

    ret
   
.section .rodata
.align 16
.L__max_exp_arg:            .octa 0x40862e42fefa39f040862e42fefa39f0  #40862e42fefa39ef40862e42fefa39ef
.L__denormal_tiny_threshold:.octa 0xc0874046dfefd9d0c0874046dfefd9d0
.L__min_exp_arg:            .octa 0xc0874910d52d3051c0874910d52d3051
.L__real_64_by_log2:        .octa 0x40571547652b82fe40571547652b82fe    # 64/ln(2)                         
.L__log2_by_64_mhead:       .octa 0xbf862e42fefa0000bf862e42fefa0000
.L__log2_by_64_mtail:       .octa 0xbd1cf79abc9e3b39bd1cf79abc9e3b39
.L__real_one:                     .octa 0x3ff00000000000003ff0000000000000
.L__real_smallest_denormal:       .octa 0x00000000000000010000000000000001
.L__real_inf:                     .octa 0x7ff00000000000007ff0000000000000
.L__m1022:                        .octa 0xc020000000000000c020000000000000
.L__ulong_1074:                   .octa 0x00000000000004320000000000000432
.L__ulong_1:                      .octa 0x00000000000000010000000000000001
.L__real_1_by_5040:               .octa 0x3f2a01a01a01a01a3f2a01a01a01a01a    # 1/5040 (1/7!)
