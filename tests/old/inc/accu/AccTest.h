#ifndef __ACC_TEST_H__
#define __ACC_TEST_H__

#include <cassert>
#include <string>
#include <sstream>

#include "DataTypes.h"
#include "Buffer.h"

template <typename TI>
TI random_fnum()
{
    return 0;
}

template <>
U32 random_fnum<U32>()
{
    U32 m = rand();
    U32 e = rand()%256;
    U32 s = rand()%2;

    m = m & 0x007fffff;
    e = e << 23;
    s = s << 31;

    U32 num = 0;
    num = num | m;
    num = num | e;
    num = num | s;

    return num;
}

template <>
U64 random_fnum<U64>()
{
    U64 ml = (U64)(rand());
    U64 mh = (U64)(rand());

    U64 m = ((mh << 32) | ml);
    U64 e = (U64)(rand()%2048);
    U64 s = (U64)(rand()%2);

    m = m & 0x000fffffffffffff;
    e = e << 52;
    s = s << 63;

    U64 num = 0;
    num = num | m;
    num = num | e;
    num = num | s;

    return num;
}


template <typename T, typename TI>
class AccTest
{
protected:
    union TU
    {
        TI ti;
        T t;
    };

    TU out_test, out_ref;

    inline void CheckAndReport(const char *inStr)
    {
        if(out_test.ti != out_ref.ti)
        {
            //std::cout << "No Exact match";

            TI diff = out_test.ti - out_ref.ti;
            
            if((diff != 0x1) && (diff != (~0x0)))
            {
                std::cout << ", Failure:: In: " << inStr << 
                    " Expected: " << std::hex << out_ref.ti <<
                    " Actual: " << out_test.ti << std::endl;
            }
            else
            {
                //std::cout << std::endl;
            }
        }
    }
};

template <typename T, typename TI, T (*libmFnTest)(T), T (*libmFnRef)(T)>
class AccTestParam1 : public AccTest<T, TI>
{
public:
    void Run(size_t length)
    {
        for(size_t i=0; i<length; i++)
        {
            TU in;
            in.ti = random_fnum<TI>();

            out_test.t = libmFnTest(in.t);
            out_ref.t = libmFnRef(in.t);

            std::ostringstream oss;
            oss << std::hex << in.ti;
            CheckAndReport(oss.str().c_str());
        }
    }
};

template <typename T, typename TI, T (*libmFnTest)(T, T), T (*libmFnRef)(T, T)>
class AccTestParam2 : public AccTest<T, TI>
{
public:
    void Run(size_t length)
    {
        for(size_t i=0; i<length; i++)
        {
            TU in1, in2;
            in1.ti = random_fnum<TI>();
            in2.ti = random_fnum<TI>();

            out_test.t = libmFnTest(in1.t, in2.t);
            out_ref.t = libmFnRef(in1.t, in2.t);

            std::ostringstream oss;
            oss << std::hex << "(" << in1.ti << ", " << in2.ti << ")";
            CheckAndReport(oss.str().c_str());
        }
    }
};

void ref_sincosf(float input, float *out_sin, float *out_cos)
{
	*out_sin = sinf(input);
	*out_cos = cosf(input);
	return;
}

void ref_sincos(double input, double *out_sin, double *out_cos)
{
	*out_sin = sin(input);
	*out_cos = cos(input);
	return;
}

template <typename T, typename TI>
class AccTest2
{
protected:
    union TU
    {
        TI ti;
        T t;
    };

    TU out_one, out_two, ref_one, ref_two;

    inline void CheckAndReport2para(const char *inStr)
    {			
		if (ref_one.ti != out_one.ti || ref_two.ti != out_two.ti)
		{
			//std::cout << "No Exact match";

			TI diff1 = ref_one.ti - out_one.ti;
			TI diff2 = ref_two.ti - out_two.ti;
            
			if (((diff1 != 0x1) && (diff1 != (~0x0))) &&
				((diff2 != 0x1) && (diff2 != (~0x0))))
			{
				std::cout << ", Failure:: In: " << inStr <<
					" Expected: " << std::hex << ref_one.ti << ", " << ref_two.ti <<
					" Actual: " << out_one.ti << ", " << out_two.ti << std::endl;
			}
			else
			{
				//std::cout << std::endl;
			}
		}
	}

    inline void CheckAndReport2ulps(const char *inStr)
    {			
		if (ref_one.ti != out_one.ti || ref_two.ti != out_two.ti)
		{
			//std::cout << "No Exact match";

			TI diff1 = ref_one.ti - out_one.ti;
			TI diff2 = ref_two.ti - out_two.ti;
            
			if (((diff1 != 0x1) && (diff1 != (~0x0)) && (diff1 != 0x2) && (diff1 != (~0x1))) &&
				((diff2 != 0x1) && (diff2 != (~0x0)) && (diff2 != 0x2) && (diff2 != (~0x1))))
			{
				std::cout << ", Failure:: In: " << inStr <<
					" Expected: " << std::hex << ref_one.ti << ", " << ref_two.ti <<
					" Actual: " << out_one.ti << ", " << out_two.ti << std::endl;
			}
			else
			{
				//std::cout << std::endl;
			}
		}
	}
};

template <typename T, typename TI, void (*libmFnTest)(T, T *, T *), void (*libmFnRef)(T, T *, T *)>
class AccTestParam1Out2 : public AccTest2<T, TI>
{
public:
    void Run(size_t length)
    {
		for(size_t i=0; i<length; i++)
		{
			TU in;
			in.ti = random_fnum<TI>();
			libmFnTest(in.t, &(out_one.t), &(out_two.t));
			libmFnRef(in.t, &(ref_one.t), &(ref_two.t));

            std::ostringstream oss;
            oss << std::hex << "(" << in.ti  << ")";
            CheckAndReport2para(oss.str().c_str());
		}
	}
};


_float_complex ref_cexpf(_float_complex x)
{
	_float_complex result;
	Hold32 tmpexp, tmpcos, tmpsin;

	tmpexp.f32 = expf(x.real());//expf(x._Val[0]);
	tmpcos.f32 = cosf(x.imag());//cosf(x._Val[1]);
	tmpsin.f32 = sinf(x.imag());//sinf(x._Val[1]);

	
	//if (tmpexp.u32 >= 0x7f800000) 
	//{
	//	result._Val[0]=tmpexp.f32;
	//	result._Val[1]=tmpexp.f32;
	//}
	//else 
	if (tmpexp.u32 == 0x0 && ((tmpcos.u32 & 0x7fffffff) >= 0x7f800000)) 
	{
		result.real(0);
		result.imag(0);
	}
	else 
	{
		//preserve the sign for negative zeroes.
		U32 sign_cos, sign_sin;
		sign_cos = (tmpcos.u32 & 0x80000000);
		tmpcos.f32 *= tmpexp.f32;
		tmpcos.u32 |= sign_cos; //tmpexp always positive
		sign_sin = (tmpsin.u32 & 0x80000000);
		tmpsin.f32 *= tmpexp.f32;
		tmpsin.u32 |= sign_sin; //tmpexp always positive
		result.real(tmpcos.f32);//result._Val[0]=tmpexp.f32 * tmpcos.f32;
		result.imag(tmpsin.f32);//result._Val[1]=tmpexp.f32 * tmpsin.f32;
	}

	return result;
}

_double_complex ref_cexp(_double_complex x)
{
	_double_complex result;
	Hold64 tmpexp, tmpcos, tmpsin;

	tmpexp.f64 = exp(x.real());//exp(x._Val[0]);
	tmpcos.f64 = cos(x.imag());//cos(x._Val[1]);
	tmpsin.f64 = sin(x.imag());//sin(x._Val[1]);

	//if (tmpexp.u64 >= 0x7ff0000000000000) 
	//{
	//	result._Val[0]=tmpexp.f64;
	//	result._Val[1]=tmpexp.f64;
	//}
	//else 
	if (tmpexp.u64 == 0x0 && ((tmpcos.u64& 0x7fffffffffffffff) >= 0x7ff0000000000000)) 
	{
		result.real(0);
		result.imag(0);
	}
	else 
	{
		//preserve the sign for negative zeroes.
		U64 sign_cos, sign_sin;
		sign_cos = (tmpcos.u64 & 0x80000000);
		tmpcos.f64 *= tmpexp.f64;
		tmpcos.u64 |= sign_cos; //tmpexp always positive
		sign_sin = (tmpsin.u64 & 0x80000000);
		tmpsin.f64 *= tmpexp.f64;
		tmpsin.u64 |= sign_sin; //tmpexp always positive
		result.real(tmpcos.f64);//result._Val[0]=tmpexp.f64 * tmpcos.f64;
		result.imag(tmpsin.f64);//result._Val[1]=tmpexp.f64 * tmpsin.f64;
	}

	return result;
}

template <typename T, typename TI, typename TC,
		  TC (*libmFnTest)(TC),  TC (*libmFnRef)(TC)>
class AccTestComplexParam : public AccTest2<T, TI>
{
public:
    void Run(size_t length)
    {
		for(size_t i=0; i<length; i++)
		{
			TU in1, in2;
			TC out, input, ref;
			in1.ti = random_fnum<TI>();
			if (in1.t < 0) //restrict the exp power to increase meaningful test
			{
				while (in1.ti > (TI)((unsigned int)0xc4800000)) in1.ti -= (TI)((unsigned int)0x44800000);
			}
			else
			{
				while (in1.ti > (TI)((unsigned int)0x44800000)) in1.ti -= (TI)((unsigned int)0x44800000);
			}
			in2.ti = random_fnum<TI>();
			input.real(in1.t);//input._Val[0]=in1.t;
			input.imag(in2.t);//input._Val[1]=in2.t;
			out = libmFnTest(input);
			out_one.t = out.real();//out._Val[0];
			out_two.t = out.imag();//out._Val[1];
			ref = libmFnRef(input);
			ref_one.t = ref.real();//ref._Val[0];
			ref_two.t = ref.imag();//ref._Val[1];

            std::ostringstream oss;
            oss << std::hex << "(" << in1.ti  << ", " << in2.ti << ")";
            CheckAndReport2ulps(oss.str().c_str());
		}
	}
};

#endif // __ACC_TEST_H__
