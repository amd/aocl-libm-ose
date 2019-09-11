# logf_fma3.S
# An implementation of the libm logf function


#include "fn_macros.h"
#define fname FN_PROTOTYPE_FMA3(logf)

.text
..TXTST0:
.align    16,0x90
.globl fname
#logf(float)
fname:
# parameter 1: %xmm0                            
        xorl      %edx, %edx                                    
        vmovd     %xmm0, %esi                                   
        vmovss    %xmm0, -8(%rsp)                               
        cmpl      $8388608, %esi     #compare if number is zero or denormal by checking <0x00800000
        jge       .Number_is_normal    
                                
..B1.2:                       
                              
        testl     $2147483647, %esi                             #check if number is zero 
        je        .Number_is_zero                                      
        testl     %esi, %esi                                  
        js        .Number_is_negative       # jump if number is negative
                                
.Number_is_subnormal:                        
                                
        vmulss    .L_2il0floatpacket.1(%rip), %xmm0, %xmm0      #number is subnormal, so scale up by multiplying with 3.355443200e+07
        movl      $-25, %edx                                    #k = -24
        vmovss    %xmm0, -8(%rsp)                               
        vmovd     %xmm0, %esi                                 
                               
.Number_is_normal:                         
        cmpl      $2139095040, %esi    #check if infinity or NaN by checking if greater than 0x7F800000
        jge       .Number_infinity_or_NaN      

#checking if near one 
        vmovaps      .L_2il0floatpacket.10(%rip),%xmm10                                        
#        vsubss       %xmm10, %xmm0, %xmm4   # xmm4 = x - 1.0
#        vmovss       %xmm4,%xmm6,%xmm6
#        vandps       .L__real_notsign(%rip), %xmm4, %xmm4   # xmm4 = fabs (x - 1.0)
#        vcomiss      .L__real_threshold(%rip), %xmm4 # if (xmm4 < real_threshold) gp to near_one
#        jb           .L__near_one
..B1.6:                        
        movl      %esi, %eax                                    
        andl      $8388607, %esi                                #Extracting mantissa bits by ANDing with 0x007FFFFF 
        sarl      $23, %eax                                     #shift eax right by 23 bits to get exponent
        lea       4913952(%rsi), %ecx                           
        andl      $8388608, %ecx                               
        lea       -127(%rax,%rdx), %eax                         
        movl      %ecx, %edx                                    
        xorl      $1065353216, %edx                             
        orl       %edx, %esi                                    
        movl      %esi, -8(%rsp)                                
        addl      $32768, %esi                                  
        shrl      $23, %ecx                                     
        andl      $8388607, %esi                                
        vmovss    -8(%rsp), %xmm0                               
        addl      %ecx, %eax                                    
      
        vsubss  %xmm10, %xmm0, %xmm6 # f = 1-x
        cmpl      $49152, %esi                                 
        jge       ..B1.12      
                               
..B1.7:                        
                               
        vxorps    %xmm0, %xmm0, %xmm0                           #initialize xmm0 to 0
        vucomiss  %xmm0, %xmm6                                  #if( f == 0)
        jp        ..B1.8       
        je        .F_is_zero       
                               
..B1.8:                         
 
        vmovss    .L_2il0floatpacket.7(%rip), %xmm1            
        vfnmadd213ss .L_2il0floatpacket.8(%rip), %xmm6, %xmm1  
        vmulss    %xmm6, %xmm6, %xmm0                          
        vfnmadd213ss %xmm6, %xmm1, %xmm0                       
        testl     %eax, %eax                            
        je        ..B1.11                                 
                               
..B1.10:                       
                               
        vxorps    %xmm1, %xmm1, %xmm1                         
        vcvtsi2ss %eax, %xmm1, %xmm1  
        vmovss    .L_2il0floatpacket.9(%rip),%xmm10                
        vfmadd231ss %xmm10,%xmm1, %xmm0   
                                
..B1.11:                        
        ret

.L__near_one:
         # r = x - 1.0#
    # u = r / (2.0 + r)
    vaddss       %xmm10,%xmm0, %xmm2 # (r+2.0) or (x+1)
    vdivss       %xmm2, %xmm6, %xmm1 # u = r / (2.0 + r)

    # correction = r * u
    vmulss       %xmm1, %xmm6,%xmm4 # correction = u*r

    # u = u + u#
    vaddss       %xmm1, %xmm1, %xmm1 # u = u+u 
    vmulss       %xmm1, %xmm1, %xmm2  # v = u^2

    # r2 = (u * v * (ca_1 + v * ca_2) - correction)
    vmulss       %xmm2, %xmm1,%xmm3         # u^3
    vmovss       .L__real_ca2(%rip),%xmm11
    vfmadd213ss     .L__real_ca1(%rip),%xmm11,%xmm2
    vfmsub213ss     %xmm4, %xmm3, %xmm2 # r2 = (ca1 + ca2 * v) * u^3 - correction

    # r + r2
    vaddss       %xmm2, %xmm6, %xmm0 # return (r + r2)
    ret
                             
..B1.12:                     
                             
            
        vxorps    %xmm7, %xmm7, %xmm7                           
            
        vcvtsi2ss %eax, %xmm7, %xmm7                            
        vaddss    .L_2il0floatpacket.2(%rip), %xmm6, %xmm0      
        vdivss    %xmm0, %xmm6, %xmm0  
                         
        vmulss    %xmm0, %xmm0, %xmm1                           # s*s
        vmulss    %xmm1, %xmm1, %xmm5                           #z*z

        vmovss    .L_2il0floatpacket.5(%rip), %xmm2  
          vmovss    .L_2il0floatpacket.3(%rip), %xmm3 
        vfmadd213ss .L_2il0floatpacket.6(%rip), %xmm5, %xmm2  

 
     vfmadd213ss .L_2il0floatpacket.4(%rip), %xmm5, %xmm3    

        vmulss    %xmm2, %xmm1, %xmm4                    
        vfmadd213ss %xmm4, %xmm3, %xmm5                  
        jmp       ..B1.22       
                                
.Number_is_zero:                
                                
        vmovss    .L_2il0floatpacket.0(%rip), %xmm0      
        vxorps    %xmm1, %xmm1, %xmm1                    
        vdivss    %xmm1, %xmm0, %xmm0                    
        ret                                              
                                
.F_is_zero:                     
        testl     %eax, %eax    
        je        ..B1.21       
                                
..B1.20:                        
                                
        vxorps    %xmm0, %xmm0, %xmm0                          
        vcvtsi2ss %eax, %xmm0, %xmm0                           
        vmulss    .L_2il0floatpacket.9(%rip), %xmm0, %xmm0     
                               
..B1.21:                                                      
        ret                    

..B1.22:                       
                                
        vsubss    %xmm5, %xmm6, %xmm1                          
        vfnmadd213ss %xmm6, %xmm1, %xmm0                       
        testl     %eax, %eax                                   
        je        ..B1.25       
                               
..B1.24:                       
        vfmadd231ss .L_2il0floatpacket.9(%rip), %xmm7, %xmm0   
                              
..B1.25:                      
                              
        ret                   
.Number_is_negative:                        
                                
        vxorps    %xmm0, %xmm0, %xmm0                           
        vdivss    %xmm0, %xmm0, %xmm0                           
        ret                                                     
                                
.Number_infinity_or_NaN:                                  
        ret                    

                 
.align    16,0x90
                            
.data
.section .rodata
	.align 4
	.align 4
.L_2il0floatpacket.0:
	.long	0xcc000000


.align 16
.L__real_ca1:           .quad 0x03DAAAAAB3DAAAAAB   # 8.33333333333317923934e-02
                        .quad 0x03DAAAAAB3DAAAAAB
.L__real_ca2:           .quad 0x03C4CCCCD3C4CCCCD   # 1.25000000037717509602e-02
                        .quad 0x03C4CCCCD3C4CCCCD
.L__real_notsign:       .quad 0x07ffFFFFF7ffFFFFF   # ^sign bit
                        .quad 0x07ffFFFFF7ffFFFFF

.L__real_one:           .quad 0x03f8000003f800000   # 1.0
                        .quad 0x03f8000003f800000
.L__real_two:           .quad 0x04000000040000000   # 1.0
                        .quad 0x04000000040000000

.L__real_threshold: .long 0x3d800000
                    .long 0
                    .quad 0
.align 16
.L_2il0floatpacket.1:
	.long	0x4c000000
	
.align 4
.L_2il0floatpacket.2:
	.long	0x40000000
	
.align 4
.L_2il0floatpacket.3:
	.long	0x3e789e26

.align 4
.L_2il0floatpacket.4:
	.long	0x3eccce13


.p2align 4
.L_2il0floatpacket.5:
        .long   0x3e91e9ee
        .long   0x3e789e26
	.long	0x3f2aaaaa
	.long   0x3eccce13

.align 4
.L_2il0floatpacket.6:
	.long	0x3f2aaaaa

.align 4
.L_2il0floatpacket.7:
	.long	0x3eaaaaab

.align 4
.L_2il0floatpacket.8:
	.long	0x3f000000

.align 4
.L_2il0floatpacket.9:
	.long	0x3f317218

.p2align 4
.L_2il0floatpacket.10:
	.long	0x3f800000
        .long   0x3eaaaaab
        .long   0x3f000000
        .long   0x00000000	

.align 4
.globl zero

zero:
	.long	0x00000000
	.align 4
	.globl ln2_hi

ln2_hi:
	.long	0x3f317180
	.align 4
	.globl ln2_lo
ln2_lo:
	.long	0x3717f7d1
	.align 4
	.globl two25
two25:
	.long	0x4c000000
	.align 4
	.globl Lg1
Lg1:
	.long	0x3f2aaaaa
	.align 4
	.globl Lg2
Lg2:
	.long	0x3eccce13
	.align 4
	.globl Lg3
Lg3:
	.long	0x3e91e9ee
	.align 4
	.globl Lg4
Lg4:
	.long	0x3e789e26
	.data
	.section .note.GNU-stack, ""
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 8
# End
