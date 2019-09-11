;
; fma_bdozr.S
;
; A bulldozer implementation of fmaf libm function.
;
; Prototype:
;
;     double fma(double x, double y, double z);
;

;
;   Algorithm:
;
;
;
include fm.inc

FN_PROTOTYPE_FMA3 fma

; local variable storage offsets

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    .ENDPROLOG 

   ;vfmaddsd     xmm0,xmm1,xmm0,xmm2 
   vfmadd213sd     xmm0,xmm1,xmm2
   ret
    


fname        endp
TEXT    ENDS   
END 
