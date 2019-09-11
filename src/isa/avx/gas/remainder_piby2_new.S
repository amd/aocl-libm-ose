#define fname __amd_remainder_piby2

# xmm0 = x;
#  rdi = *r
#  rsi = *rr
#  rdx = *region

#ifdef __ELF__
.section .note.GNU-stack,"",@progbits
#endif

.text
.p2align 4
.globl fname
.type fname,@function
fname:
    #get the unbiased exponent and the mantissa part of x
    push %rdi   
    lea  .L__2_by_pi_bits(%rip),%r9
    
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
    mov  %rdx, %rdi      # save address of region since mul modifies rdx
    
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

.p2align 4 
.L__no_point_five:
    adc $0,   %rax
    and $3,   %rax
    mov %eax, (%rdi)     #store region to memory

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
    
    
.p2align 4  
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
    
.p2align 4 
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
   
    
.p2align 4
.L__form_numbers:
    add $1023, %r11
    btr $52,%r10     #remove the implied bit
    mov %r11,%rcx
    or  %rdx,%r10    #put the sign    
    shl $52, %rcx
    or  %rcx,%r10    #x is in r10
    
    movd %r10,%xmm0  #xmm0 = x
    movdqa %xmm0,%xmm1 #xmm1 = x
    psrlq $27,%xmm1
    psllq $27,%xmm1  #xmm1 = hx
    movdqa %xmm0,%xmm2 #xmm2 = x    
    subsd %xmm1,%xmm2 #xmm2 = tx
    movlhps %xmm0,%xmm0 #xmm0 = x,x
    movlhps %xmm1,%xmm2  #xmm2 = hx,tx

    movdqa .L__piby2_part3_piby2_lead(%rip),%xmm1   
    movdqa .L__piby2_part1(%rip),%xmm3 #xmm3 = piby2_part1, piby2_part1
    movdqa .L__piby2_part2(%rip),%xmm4 #xmm4 = piby2_part2, piby2_part2 

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
    movd %r9,%xmm5  #xmm5 = xx  
    
    mulpd %xmm1, %xmm0 #xmm0 = piby2_part3 * x, piby2_lead * x = c
    mulpd %xmm1, %xmm5 #xmm5 =                  piby2_lead * xx
    mulpd %xmm2, %xmm3 #xmm3 = piby2_part1 * hx, piby2_part1 * tx
    mulpd %xmm2, %xmm4 #xmm4 = piby2_part2 * hx, piby2_part2 * tx 
    
    pop %rdi
        
    #cc = (piby2_part1 * hx - c) +  (piby2_part1 * tx) +  (piby2_part2 * hx) +  (piby2_part2 * tx) + (piby2_lead * xx + piby2_part3 * x)
    movhlps %xmm3, %xmm1  #xmm1 = piby2_part1 * hx
    movhlps %xmm4,%xmm2   #xmm2 = piby2_part2 * hx        
    subsd %xmm0, %xmm1    #xmm1 = (piby2_part1 * hx - c)
    addsd %xmm3, %xmm1    #xmm1 = (piby2_part1 * hx - c) + (piby2_part1 * tx)
    movhlps %xmm0, %xmm3  #xmm3 = piby2_part3 * x
    addsd %xmm2, %xmm1    #xmm1 = (piby2_part1 * hx - c) +  (piby2_part1 * tx) +  (piby2_part2 * hx)
    addsd %xmm5, %xmm3    #xmm3 = (piby2_lead * xx + piby2_part3 * x)
    addsd %xmm4, %xmm1    #xmm1 = (piby2_part1 * hx - c) +  (piby2_part1 * tx) +  (piby2_part2 * hx) +  (piby2_part2 * tx)
    addsd %xmm3, %xmm1    #xmm1 = cc
    
    #r = c + cc
    #rr = (c - r) + cc
    movdqa %xmm0, %xmm2
    addsd %xmm1,%xmm2  #xmm2 = r
    subsd %xmm2,%xmm0
    addsd %xmm1,%xmm0  #xmm0 = rr
    movsd %xmm2,(%rdi)        
    movsd %xmm0,(%rsi)
    ret      
    
.section .rodata
.align 16
.type	.L__2_by_pi_bits, @object
.size	.L__2_by_pi_bits, 158
.L__2_by_pi_bits:
    .byte 224,241,27,193,12,88,33,116,53,126,196,126,237,175,169,75,74,41,222,231,28,244,236,197,151,175,31,235,158,212,181,168,127,121,154,253,24,61,221,38,44,159,60,251,217,180,125,180,41,104,45,70,188,188,63,96,22,120,255,95,226,127,236,160,228,247,46,126,17,114,210,231,76,13,230,88,71,230,4,249,125,209,154,192,113,166,19,18,237,186,212,215,8,162,251,156,166,196,114,172,119,248,115,72,70,39,168,187,36,25,128,75,55,9,233,184,145,220,134,21,239,122,175,142,69,249,7,65,14,241,100,86,138,109,3,119,211,212,71,95,157,240,167,84,16,57,185,13,230,139,2,0,0,0,0,0,0,0
.align 16
.L__piby2_part3_piby2_lead: .octa 0x3c91a62633145c063ff921fb54442d18
.L__piby2_part1:            .octa 0x3ff921fb500000003ff921fb50000000
.L__piby2_part2:            .octa 0x3e5110b4600000003e5110b460000000
