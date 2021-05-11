
Hyperbolic Cosine 
=================

The hyperbolic function cosh is defined as
    
        cosh(x)=(e^x+e^(-x))/2                  (1)

This function exists for all real arguments and vary smoothly over the range =
[1, INFINITY) =. The cosh function is an even function. The rule for negative
values of x is given by:
    
    cosh(-x) = +cosh(x)                         (2)
      
Thus, the plot of cosh is symmetric along y-axis and we can calculate the
negative values by computing the cosh function only for the absolute value of
the argument. The Taylor series of the hyperbolic function cosh for x ~= 0 is:

                   1         1          1                1
    cosh(x) = 1 + ----x^2 + ---- x^4 + ---- x^6 + ... + ---- x^2n         (3) 
                   2!        4!         6!              (2n)!
              
NOTE: that polynomial will have even powers of x. 

As the cosh function is dependent on exponential function and as the input real
argument is always positive, the exponential function for negative input
values satisfies

    e^(-x) = 1/e^x                          (4) 


Single equation to calculate cosh(x),

                       +-
                       | v    ,--                 1                   --.
         coshf(x)   =  |--- * |e^(x - ln(v)) +  ------  * e^(-(x-ln(v)) |
                       | 2    |                  (v*v)                  |
                       +-     `--                                     --'

However to calcualte e^x, we rely on library function exp(x), which has an upper
limit, but cosh(x) is defined beyond that. The input argument at which cosh(x)
overflows is larger than that for exp(x).
    
Thus, it is impossible to compute the cosh(x) function by direct application of
their definitions for input arguments in the region where the hyperbolic
functions remain finite, but exp(x) overflows. The above problem is solved by
dividing the input argument interval [0, FLT_MAX) into these consecutive regions.


 1. (0, 2^-11]
 2. (2^-11, ln(2)/2]    ; 1/2 * ln(2) or 0.5 or 1
 3. (ln(2)/2, 8.5]
 4. (8.5, EXPMAX]      ; max value that exp() can take
 5. (EXPMAX, COSHMAX]  ; cosh() can take values beyond what exp() can
 6. (COSHMAX, FLT_MAX] ; returns INF

 1. [0, EPS]: In the range first two terms of the Taylor series is used.

   cosh(x) = 1 + 1/2 * x^2                   (5) 
   
   EPS value can be calculated by 
   
   EPS=2*2^(t/2)                             (6)
   
   Where t is number of bits in the significand. NOTE: The mathematical
   definition of cosh(x) also can be used here in this range. The costly
   inverse/ division (1/e^x) can be avoided by using this.
      
 2. (EPS, XC]: A polynomial approximation can be used here. XC can be any value
   of our interest. It can be ½*ln(2) or 0.5 or 1. Depending on the range of
   interval the degree of polynomial increase. We can pick a polynomial of
   degree 6, 8, 10 respectively for good accuracy. NOTE: The mathematical
   definition of cosh(x) also can be used here in this range. The costly
   inverse/ division (1/e^x) can be avoided by using the polynomial in this
   range.

 3. (XC, 8.5]: In this interval the mathematical definition of cosh can be used. 

    Z = e^x                               (7) 
    
    cosh(x) = 1/2*(Z + 1/Z)               (8)
      
 4. (8.5, EXPMAX]: In this interval, exp(-x) takes very small value and can be
      neglected. The function cosh can be simply evaluated using: 
      
      Z = e^x                              (9)
      
      cosh(x) = 1/2 * Z                    (10) 
      
      The value of EXPMAX is given by 
      
      EXPMAX = ln(FLT_MAX)                 (11)
      
      EXPMAX is the largest floating-point number acceptable by exp function and
      realmax is the largest floating-point number possible in the machine.
      NOTE: The mathematical definition of cosh(x) also can be used here in this
      range. The costly inverse/ division (1/e^x) can be avoided by using
      this in the interval.
      
 5. (EXPMAX, COSHMAX]: In this range e^x takes INF and expf() cannot be used as it is.
    Thus, in this range the direct mathematical definition of cosh cannot be used.
    This problem can be avoided by translating the input argument using the relation

    e^x/2 = e^x*e^(-ln(2)) = e^((x-ln(2)))            (12) 
    
    But ln(2) is transcendental number and cannot be represented exactly, it
    has to be approximated to represent and thus it carries some error. The
    error-magnification factor of e^x and thus also of cosh(x), is proportional
    to x; when x is large, that translation introduces large errors in the function
    due ln(2) and the results will be unsatisfactory. To avoid the above
    uncertainty, instead of ln(2), a number log(v) is chosen such that the number
    is slightly larger than ln(2) and the number is exactly represented in the
    machine with the last few digits of its significand zero. The translation of
    argument is 
    
    e^x/2 = v/v * e^x/2 = v/2 * e^((x-log(v)))            (13) 
    
    The subtraction now is exact and now the argument for the exponential
    function is error-free whenever x is, and the transmitted error depends only on
    the error in input argument x. The values required v/2 and log(v) are stored in
    the code and accessed when needed. The value used in this interval range is

    log(v) = 45427/65536                                  (14)

 6. (COSHMAX, FLT_MAX): In this range the function takes infinity as the value. 
        / cosh(x) = INF /. 


