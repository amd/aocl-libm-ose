;
; fmaf_bdozr.S
;
; A bulldozer implementation of fmaf libm function.
;
; Prototype:
;
;     float fmaf(float x, float y, float z);
;

;
;   Algorithm:
;
;
;
include fm.inc

FN_PROTOTYPE_FMA3 fmaf



; local variable storage offsets

text SEGMENT EXECUTE

PUBLIC fname
fname PROC FRAME
    .ENDPROLOG 

  ; vfmaddss     xmm0,xmm1,xmm0,xmm2 
   vfmadd213ss     xmm0,xmm1,xmm2
   ret
    
fname        endp
TEXT    ENDS   
END 

