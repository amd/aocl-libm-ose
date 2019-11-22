
#if defined(WIN64) || defined(WINDOWS)
#define _WINSYS_
#endif

#if defined (__COMP_REF__)
#include "win_nag.h"
#endif

#if defined(_WINSYS_)
#include <windows.h>
#else

#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include <sys/ptrace.h>

#ifdef LNX_SLES10SP1
#define PTRACE_REQ (__ptrace_request)
#include <linux/ptrace.h>
#else
#define PTRACE_REQ
/*#include <linux/user.h> */ /* to make it work with 4.3.2 version */
#endif

#include <sys/user.h>
#include <sys/wait.h>
#include <fenv.h>

#endif

#include <cstdlib>


union Type_Conv
{
	unsigned int i[4];
	float f[4];
	unsigned long long int ll[2];
	double d[2];
	__m128i xi;
	__m128d xd;
	__m128  xf;
};





#if defined(_txt_)
#include "TestReport_csv.h"
#elif defined(_html_)
#include "TestReport_html.h"
#elif defined(_csv_)
#include "TestReport_csv.h"
#else
#define _txt_
#include "TestReport_csv.h"
#endif



namespace  TestReport
{
typedef unsigned int U32;
typedef unsigned long long int  U64;

	template <typename TI>
	inline bool isNaN(TI x)
	{
		std::cout << "Control in unexpected place: " << __FILE__ << " : " << __LINE__ << std::endl;
		return false;
	}

	template <>
	inline bool isNaN<U32>(U32 x)
	{
		U32 ax = x & 0x7fffffff;
		return ( (ax > 0x7f800000) ? true : false );
	}

	template <>
	inline bool isNaN<U64>(U64 x)
	{
		U64 ax = x & 0x7fffffffffffffffLL;
		return ( (ax > 0x7ff0000000000000LL) ? true : false );
	}

	template<typename TR>
	struct Genreport
	{
	};

	template<typename TR>
	struct GenreportBuffer
	{
	};

	template<>
	struct Genreport< int >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{

			std::string color;

#if defined(__REMQUO__)
			if((data->obtained%8) == (data->expected%8))
			{
				color =  PASS_COLOR;
			}
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
			}

#else
			if(data->obtained == data->expected)
			{
				color =  PASS_COLOR;
			}
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
			}

#endif

			OutData<int,0>(data->name,data->input,data->expected,data->obtained,color,0);

		}
	};


	template<>
	struct Genreport< long int >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;
			if(data->obtained == data->expected)
			{
				color =  PASS_COLOR;
			}
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
			}

			OutData<long int,0>(data->name,data->input,data->expected,data->obtained,color,0);

		}
	};


	template<>
	struct Genreport< long long int >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;
			if(data->obtained == data->expected)
			{
				color =  PASS_COLOR;
			}
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
			}

			OutData<long long int,0>(data->name,data->input,data->expected,data->obtained,color,0);

		}
	};


	template<>
	struct GenreportBuffer< int *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{

#if defined(__REMQUO__)
            for(int i =0 ; i <  data->size ; i++)
            {

			std::string color;
			testNum++;
			bool test_s = true;
			if((data->obtained[i]%8) == (data->expected[i]%8))
				color =  PASS_COLOR;
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
				test_s = false;
			}

#else
            for(int i =0 ; i <  data->size ; i++)
            {

			std::string color;
			testNum++;
			bool test_s = true;
			if(data->obtained[i] == data->expected[i])
				color =  PASS_COLOR;
			else
			{
				color =  FAIL_COLOR;
				teststatus = false;
				test_s = false;
			}

#endif

			#if defined(_FAIL_ONLY_)
			if(!test_s)
			{
				OutData<int ,1>(data->name,data->input[i],data->expected[i],data->obtained[i],color,i);
			}
			#endif
           }
		}
	};


	template<>
	struct GenreportBuffer< char *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{
           std::string color;
			color =  PASS_COLOR;
            int i;
            for(i =0 ; i <  data->size ; i++)
            {
			testNum++;
            if(data->obtained[i] != data->expected[i])
            {
				color =  FAIL_COLOR;
				teststatus = false;
			}

           }

			OutData<0>(data->name,data->input,data->expected,data->obtained,color,i);
		}
	};

	template<>
	struct Genreport< float >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;
            Type_Conv inp,exp,obt;
            inp.f[0] = data->input;
            exp.f[0] = data->expected;
            obt.f[0] = data->obtained;

			int diff = obt.i[0] - exp.i[0];

			if( isNaN<U32>(exp.i[0]) &&  isNaN<U32>(obt.i[0]))
			{
				color =  PASS_COLOR;
			}
			else
			{
				#if defined(__1BDIFF__)
				if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
				{
					color =  PASS_COLOR;
				}
				#else
				if(diff == 0x0)
				{
					color =  PASS_COLOR;
				}
				#endif
				else
				{
					color =  FAIL_COLOR;
					teststatus = false;
				}
			}

			OutData<float ,0>(data->name,inp.f[0],exp.f[0],obt.f[0],color,0);

		}
	};

	template<>
	struct GenreportBuffer< float *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{
            for(int i =0 ; i <  data->size ; i++)
            {
				std::string color;
				bool test_s = true;
				Type_Conv inp,exp,obt;
				inp.f[0] = data->input[i];
				exp.f[0] = data->expected[i];
				obt.f[0] = data->obtained[i];
				int diff = obt.i[0] - exp.i[0];
				testNum++;
				if( isNaN<U32>(exp.i[0]) &&  isNaN<U32>(obt.i[0]))
				{
					color =  PASS_COLOR;
				}
				else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						testFailed++;
						color =  FAIL_COLOR;
						test_s = false;
					}
				}


			#if defined(_FAIL_ONLY_)
			if(!test_s)
			{
				OutData<float ,1>(data->name,inp.f[0],exp.f[0],obt.f[0],color,i);
			}
			#else
				{
					OutData<float ,1>(data->name,inp.f[0],exp.f[0],obt.f[0],color,i);
				}
			#endif

            }
		}
	};


	template<>
	struct Genreport< double >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;
            Type_Conv inp,exp,obt;
            inp.d[0] = data->input;
            exp.d[0] = data->expected;
            obt.d[0] = data->obtained;

			long long int diff = obt.ll[0] - exp.ll[0];

			if( isNaN<U64>(exp.ll[0]) &&  isNaN<U64>(obt.ll[0]))
			{
				color =  PASS_COLOR;
			}
			else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						color =  FAIL_COLOR;
					}
				}

			OutData<double ,0>(data->name,inp.d[0],exp.d[0],obt.d[0],color,0);
		}
	};



	template<>
	struct Genreport< long double >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;
            Type_Conv inp,exp,obt;
            inp.d[0] = ( double) data->input;
            exp.d[0] = ( double) data->expected;
            obt.d[0] = ( double) data->obtained;

			long long int diff = obt.ll[0] - exp.ll[0];

			if( isNaN<U64>(exp.ll[0]) &&  isNaN<U64>(obt.ll[0]))
			{
				color =  PASS_COLOR;
			}
			else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						color =  FAIL_COLOR;
					}
				}

			OutData<double ,0>(data->name,inp.d[0],exp.d[0],obt.d[0],color,0);
		}
	};


	template<>
	struct GenreportBuffer< double *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{
            for(int i =0 ; i <  data->size ; i++)
            {
				std::string color;
				Type_Conv inp,exp,obt;
				inp.d[0] = data->input[i];
				exp.d[0] = data->expected[i];
				obt.d[0] = data->obtained[i];
				long long int diff = obt.ll[0] - exp.ll[0];
			testNum++;
			bool test_s=true;
			if( isNaN<U64>(exp.ll[0]) &&  isNaN<U64>(obt.ll[0]))
			{
				color =  PASS_COLOR;
			}
			else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						testFailed++;
						color =  FAIL_COLOR;
						test_s = false;
					}
				}

				#if defined(_FAIL_ONLY_)
				if(!test_s)
				{
					OutData<double ,1>(data->name,inp.d[0],exp.d[0],obt.d[0],color,i);
				}
				#else
				{
					OutData<double ,1>(data->name,inp.d[0],exp.d[0],obt.d[0],color,i);
				}
				#endif
            }
		}
	};

	template<>
	struct Genreport< __m128 >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{

            //data->input
            Type_Conv inp,exp,obt;
            inp.xf = data->input;
            exp.xf = data->expected;
            obt.xf = data->obtained;
            for(int i=0;i<4; i++)
            {
				std::string color;

				int diff = obt.i[i] - exp.i[i];

				if( isNaN<U32>(exp.i[i]) &&  isNaN<U32>(obt.i[i]))
				{
					color =  PASS_COLOR;
				}
				else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						color =  FAIL_COLOR;
					}
				}

				OutData<float ,1>(data->name,inp.f[i],exp.f[i],obt.f[i],color,i);
            }
		}
	};


	template<>
	struct Genreport< __m128i >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{

            //data->input
            Type_Conv inp,exp,obt;
            inp.xi = data->input;
            exp.xi = data->expected;
            obt.xi = data->obtained;
            for(int i=0;i<4; i++)
            {
				std::string color;
				if(obt.i[i] == exp.i[i])
						color =  PASS_COLOR;
					else
					{
						teststatus = false;
						color =  FAIL_COLOR;
					}
				OutData<int ,1>(data->name,inp.i[i],exp.i[i],obt.i[i],color,i);

            }
		}
	};

	template<>
	struct Genreport< __m128d >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{

            //data->input
			Type_Conv inp,exp,obt;
            inp.xd = data->input;
            exp.xd = data->expected;
            obt.xd = data->obtained;
            for(int i=0;i<2; i++)
            {
				std::string color;
				long long int diff = obt.ll[i] - exp.ll[i];
				if( isNaN<U64>(exp.ll[i]) &&  isNaN<U64>(obt.ll[i]))
				{
					color =  PASS_COLOR;
				}
				else
				{
					#if defined(__1BDIFF__)
					if((diff == 0x1) || (diff == (~0x0)) || (diff == 0x0) )
					{
						color =  PASS_COLOR;
					}
					#else
					if(diff == 0x0)
					{
						color =  PASS_COLOR;
					}
					#endif
					else
					{
						teststatus = false;
						color =  FAIL_COLOR;
					}
				}
				OutData<double ,1>(data->name,inp.d[i],exp.d[i],obt.d[i],color,i);

            }
		}
	};

	template<>
	struct Genreport< ExcFlags >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
			std::string color;

			if(data->obtained.us == data->expected.us && data->obtained.os ==  data->expected.os && data->obtained.zs ==  data->expected.zs && data->obtained.is ==  data->expected.is && data->obtained.other_s ==  data->expected.other_s   )
				color =  PASS_COLOR;
			else
			{
				teststatus = false;
				color =  FAIL_COLOR;
			}

			OutData<ExcFlags,0>(data->name,data->input,data->expected,data->obtained,color,0);

		}
	};


	template<>
	struct GenreportBuffer< ExcFlags *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{


            for(int i =0 ; i <  data->size ; i++)
            {
				std::string color;

				if(data->obtained[i].us == data->expected[i].us && data->obtained[i].os ==  data->expected[i].os && data->obtained[i].zs ==  data->expected[i].zs && data->obtained[i].is ==  data->expected[i].is && data->obtained[i].other_s ==  data->expected[i].other_s   )
					color =  PASS_COLOR;
				else
				{
					teststatus = false;
					color =  FAIL_COLOR;
				}

				OutData<ExcFlags,1>(data->name,data->input[i],data->expected[i],data->obtained[i],color,i);


			}
		}
	};


	template<typename DTP,typename TRD>
	void ReportUlpErrorBuff(const TRD *data)
	{
		std::string color;
		if(data->ulp[data->ulp_index] > 0.5)
		color  = FAIL_COLOR;
		else
		color  = PASS_COLOR;

		OutData<DTP,0>(data->name,data->input[data->ulp_index],data->expected[data->ulp_index],data->obtained[data->ulp_index],color,0);
		ulp_data<<out_data.str();
		out_data.str("");
		out_data.clear();

	}

	template<typename DTP,typename TRD>
	void ReportUlpError(const TRD *data)
	{
		std::string color;
		if(data->ulp[data->ulp_index] > 0.5)
		color  = FAIL_COLOR;
		else
		color  = PASS_COLOR;

		OutData<DTP,0>(data->name,data->input,data->expected,data->obtained,color,0);
		ulp_data<<out_data.str();
		out_data.str("");
		out_data.clear();

	}


}








template < class AT, class T>
struct ParameterObjBuffer
{
	char  name[50];
	T input;
	T obtained;
	T expected;
	int index;
	int size;
	bool align;
    double *ulp;
    double *relative_error;
	int ulp_index;

	AT min, max, inc;
	ParameterObjBuffer()
	{
		index = 0;
		size = 0;
		align = false;
		ulp_index = 0;
	}

	double MaxUlp()
	{
		double max_ulp =0;
		ulp_index = 0;
		for(int i=0;i<size;i++)
		{
			if(ulp[i] > max_ulp )
			{
				max_ulp = ulp[i];
				ulp_index = i;
			}
		}
		return max_ulp;
	}

	double MaxRelDiff()
	{
		double max_err =0;
		for(int i=0;i<size;i++)
		{
			if(relative_error[i] > max_err )
				max_err = relative_error[i];
		}
		return max_err;
	}



	void SetAlignMem(bool al)
	{
		align =	al;
	}

	void SetName(const char *nm)
	{
		int i =0 ;
		while(nm[i] != '\0')
		{
			name[i] = nm[i];
			i++;
		}
		name[i]= '\0';
	}

	void SetBufferData(T data,T exp, int num)
	{
		for(int i = 0 ; i < num ; i++)
		{
			input[i] = obtained[i] = data[i];
			expected[i] = exp[i];
			ulp[i]= 0.0;
			relative_error[i] = 0.0;

		}
        if(num == 0)
        {
            input = obtained = data;
            expected = exp;
        }
		index += num;

	}
	void SetDatatoBuffer(AT data, AT exp)
	{
		input[index] = obtained[index] = data;
		expected[index] = exp;
		index++;
	}

#if defined(_WINSYS_)
    void *AlignedMalloc ( size_t length )
    {
        return _aligned_malloc( length, 32 );
    }
    void AlignedFree ( void *ptr )
    {
        _aligned_free( ptr );
    }

#else

	void *AlignedMalloc ( size_t length )
	{
		void *memptr;
		posix_memalign ( &memptr, 32, length );
		return memptr;
	}
	void AlignedFree ( void *ptr )
	{
		free( ptr );
	}
#endif

	void AllocateMem (int len)
	{
		if(align)
		{
			input = (T) AlignedMalloc(sizeof(AT) * len);
			obtained = (T) AlignedMalloc(sizeof(AT) * len);
			expected = (T) AlignedMalloc(sizeof(AT) * len);
		}
		else
		{
			input =  new AT[len + 1] ;
			obtained =  new AT[len + 1];
			expected =  new AT[len + 1];

			input = (AT *)((char *) input + 1);
			obtained = (AT *)((char *) obtained + 1);
			expected = (AT *)((char *) expected + 1);

		}


		ulp = new double[len];
		relative_error = new double[len];
		size = len;
	}



	void DeAllocateMem()
	{
		if(size)
		{
			if(align)
			{
				AlignedFree(input);
				AlignedFree(obtained);
				AlignedFree(expected);
			}
			else
			{

				input = (AT *)((char *) input - 1);
				obtained = (AT *)((char *) obtained - 1);
				expected = (AT *)((char *) expected - 1);

				delete[] input;
				delete[] obtained;
				delete[] expected;

			}

		}
	}

	void Report()
	{
		TestReport::GenreportBuffer< T >::Report(this);
	}

	template<int val>
		void GRADIENT(int start, int end, int div)
	{
		if( val == 1)
		{
			int i = 0;
			AT inc;
			inc = (end - start)/div;

			AllocateMem(div);
			input[i] =  start;
			i++;
			while(i<div)
			{
				input[i] = input[i-1]+inc;
				obtained[i] = input[i];
				i++;
			}
		}
		if( val == 0)
		{
			int i = 0;
			AT inc;
			inc = (end - start)/div;

			expected[i] =  start;
			i++;
			while(i<div)
			{
				expected[i] = expected[i-1]+inc;
				i++;
			}
		}
	}

		void ReportUlpData()
		{
			TestReport::ReportUlpErrorBuff<AT>(this);
		}


		void SetDataPerf(double dmin, double dmax, double dntest)
		{
			min = (AT) dmin;
			max = (AT) dmax;
			inc  = ( max - min ) / ((AT) dntest);
		}

		void FillBuffer()
		{
			AT val = min;
			for(int i=0; i< size;i++)
			{
				input[i] = val;
			}
			min = min+inc;
		}

		AT GetValue()
		{
			return input[0];
		}

		void FillBuff()
		{
			AT val = min;
			*input = val;
			min = min+inc;
		}


	~ParameterObjBuffer()
	{
		DeAllocateMem();
	}
};

template <class T>
struct ParameterObj
{

	char  name[50];
	T input;
	T obtained;
	T expected;
    double ulp[4];
    double relative_error[4];
	int ulp_index;


	ParameterObj()
	{
		for(int i=0;i<4;i++)
			ulp[i] = relative_error[i] = 0.0;

		ulp_index = 0;
	}
	void AllocateMem (int len)
	{
    }

	void SetName(const char *nm)
	{
		int i =0 ;
		while(nm[i] != '\0')
		{
			name[i] = nm[i];
			i++;
		}
		name[i]= '\0';
	}

	double MaxUlp()
	{
		double max_ulp =0;
		ulp_index = 0;
		for(int i=0;i<4;i++)
		{
			if(ulp[i] > max_ulp )
			{
				max_ulp = ulp[i];
				ulp_index = i;
			}
		}
		return max_ulp;
	}

	double MaxRelDiff()
	{
		double max_err =0;
		for(int i=0;i<4;i++)
		{
			if(relative_error[i] > max_err )
				max_err = relative_error[i];
		}
		return max_err;
	}

	public :

		void SetData(T data, T exp)
		{
			input = obtained = data;
			expected = exp;
		}

        void SetDatatoBuffer(T data, T exp)
	    {
             SetData(data,exp);
	    }

		void Report()
		{
			TestReport::Genreport< T >::Report(this);
		}

		void ReportUlpData()
		{
			TestReport::ReportUlpError<T>(this);
		}


		void setVal(double val,__m128 &input)
		{
			input = _mm_set1_ps((float)val);
		}

		void setVal(double val,__m128d &input)
		{
			input = _mm_set1_pd(val);
		}

		void setVal(double val,int &input)
		{
			input = (int ) val;
		}

		void setVal(double val,float &input)
		{
			input = (float ) val;
		}

		void setVal(double val,double &input)
		{
			input = (double ) val;
		}

		void FillData(double val)
		{
			setVal(val,input);
		}

};

bool IsCloseTo( const double & a, const double & b, const double & percent )
{
	double top     = b * (1+percent);
	double bottom = b * (1-percent);

	return( a <= top &&
		b >= bottom );
}


#define MAX(a,b) a>b ? a : b
#define MIN(a,b) a<b ? a : b

class Stats
{
	double min_;
	double history[3];
	unsigned int  count;

	public:
		Stats()
		{

			count=0;
			min_ =  1.7E308;
			history[0] = history[1] = history[2] = 0;
		}

		bool IsStable() const
		{
			if( count < 3 )
				return false;

			double tmp = history[0] + history[1] + history[2];
			tmp /= 3;

			return IsCloseTo( tmp, history[0], .05 );
		}

		void Add( double & cycles )
		{
			history[count++%3] = cycles;

			if( cycles != cycles )
			{
				std::cout << "ERROR: cycles value(" << cycles << ")" << std::endl; std::cout.flush();
				cycles = 0;
			}

			min_ = MIN( min_, cycles );
		}

		double Time() const{ return min_; }
};

#if defined(_WINSYS_)

struct Timer
{
	LARGE_INTEGER start, stop, freq;
	Stats stats;

	public:
		Timer() { QueryPerformanceFrequency( &freq ); }

		void    Start  () { QueryPerformanceCounter(&start); }
		double Sample()
		{
			QueryPerformanceCounter  ( &stop );
			double time = (double)(stop.QuadPart-start.QuadPart) / freq.QuadPart;

			stats.Add( time );
			return time;
		}

		const Stats & GetStats(){ return stats; }

		bool IsStable() const { return stats.IsStable(); }

		int GetSeed()
		{
			LARGE_INTEGER seed;
			QueryPerformanceCounter  ( &seed );
			return seed.u.LowPart;
		}

};

#else

struct Timer
{
	timeval tval;
	double start, stop;
	Stats stats;

	public:
		Timer() { }

		void    Start  ()
		{
			gettimeofday(&tval, NULL);
			start = (double)(tval.tv_sec * 1E6 + tval.tv_usec);
		}
		double Sample()
		{
			gettimeofday(&tval, NULL);
			stop = (double)(tval.tv_sec * 1E6 + tval.tv_usec);
			double time = (stop - start) * 1E-6;

			stats.Add( time );
			return time;
		}

		const Stats & GetStats(){ return stats; }

		bool IsStable() const { return stats.IsStable(); }

		int GetSeed()
		{
			timeval tval1;
			gettimeofday(&tval1, NULL);
			return tval1.tv_usec;
		}
};
#endif

typedef union
{
	unsigned int i;
	float f;
}int_float;

typedef union
{
	unsigned long long int i;
	double d;
}int_double;


template <typename T1,typename T2>
T1 InputVal(double min)
{
	return 0;
}




template <typename T1,typename T2>
T1 randomNumber()
{
	return 0;
}

int SetSeed(int seed=0)
{
	Timer tm;
	int sd =seed;
	if(seed == 0)
	{
		sd = tm.GetSeed();
	}
    srand(sd);
	return sd;
}

int random_generator()
{
   return rand();
}

template <>
inline float randomNumber<float,float>()
{
	unsigned int m = random_generator();
	unsigned int e = random_generator()%256;
	unsigned int s = random_generator()%2;

	m = m & 0x007fffff;
	e = e << 23;
	s = s << 31;

	unsigned int num = 0;
	num = num | m;
	num = num | e;
	num = num | s;

	int_float temp;
	temp.i = num;
	return temp.f;
}


template <>
inline __m128 randomNumber<__m128,float>()
{
	return _mm_set_ps(randomNumber<float,float>(),randomNumber<float,float>(),randomNumber<float,float>(),randomNumber<float,float>());
}


template <>
inline double randomNumber<double,double>()
{
	unsigned long long int  ml = ( unsigned long long int)(random_generator());
	unsigned long long int mh = ( unsigned long long int)(random_generator());

	unsigned long long int m = ((mh << 32) | ml);
	unsigned long long int e = ( unsigned long long int)(random_generator()%2048);
	unsigned long long int s = ( unsigned long long int)(random_generator()%2);

	m = m & 0x000fffffffffffffLL;
	e = e << 52;
	s = s << 63;

	unsigned long long int num = 0;
	num = num | m;
	num = num | e;
	num = num | s;

	int_double temp;
	temp.i = num;
	return temp.d;
}


template <>
inline __m128d  randomNumber<__m128d,double>()
{
	return _mm_set_pd(randomNumber<double,double>(),randomNumber<double,double>());
}


template <>
inline __m128d  randomNumber<__m128d,__m128d>()
{
	__m128d temp;
	temp = _mm_set_pd(randomNumber<double,double>(),randomNumber<double,double>());
	return temp;
}


template <>
inline __m128  randomNumber<__m128,__m128>()
{
	__m128 temp;
	temp = _mm_set_ps(randomNumber<float,float>(),randomNumber<float,float>(),randomNumber<float,float>(),randomNumber<float,float>());
	return temp;
}


template <>
inline unsigned int  randomNumber<unsigned int,unsigned int >()
{
	return (unsigned int )random_generator();
}


template <>
inline unsigned long long int randomNumber<unsigned long long int,unsigned long long int>()
{
	unsigned long long int l = (unsigned long long int)(random_generator());
	unsigned long long int h = (unsigned long long int)(random_generator());
	unsigned long long int n = ((h << 32) | l);

	return n;
}


template <>
inline ExcFlags randomNumber<ExcFlags,ExcFlags>()
{
	ExcFlags temp;
	temp.us=0;
	temp.os=0;
	temp.zs=0;
	temp.is=0;
	temp.other_s=0;

	return temp;
}







template <>
inline float InputVal<float,float>(double min)
{
	return (float) min;
}


template <>
inline __m128 InputVal<__m128,float>(double min)
{
	return _mm_set_ps(InputVal<float,float>(min),InputVal<float,float>(min+0.1),InputVal<float,float>(min+0.2),InputVal<float,float>(min+0.3));
}


template <>
inline double InputVal<double,double>(double min)
{
   return min;
}


template <>
inline __m128d  InputVal<__m128d,double>(double min)
{
	return _mm_set_pd(InputVal<double,double>(min),InputVal<double,double>(min+0.1));
}


template <>
inline __m128d  InputVal<__m128d,__m128d>(double min)
{
	__m128d temp;
	temp = _mm_set_pd(InputVal<double,double>(min),InputVal<double,double>(min+0.0005));
	return temp;
}


template <>
inline __m128  InputVal<__m128,__m128>(double min)
{
	__m128 temp;
	temp = _mm_set_ps(InputVal<float,float>(min),InputVal<float,float>(min+.00025),InputVal<float,float>(min+.0005),InputVal<float,float>(min+.00075));
	return temp;
}


template <>
inline unsigned int  InputVal<unsigned int,unsigned int >(double min)
{
	return (unsigned int ) min;
}


template <>
inline unsigned long long int InputVal<unsigned long long int,unsigned long long int>(double min)
{
	return (unsigned int ) min;
}


template <>
inline ExcFlags InputVal<ExcFlags,ExcFlags>(double min)
{

	ExcFlags temp;
	temp.us=0;
	temp.os=0;
	temp.zs=0;
	temp.is=0;
	temp.other_s=0;
	return temp;
}

/*

int tesf(float a , int b , int * c)
{
	a;b;c;
 return 0;
}

class UnitTestData
{
public:
	ParameterObj<int> returnp;
	ParameterObj<float> param1;
	ParameterObj<int> param2;
	ParameterObjBuffer<int,int*> param3;

	void Run()
	{
			returnp.obtained = tesf(param1.input,param2.input,param3.input);
	}

	void Report()
	{
		TestReport<int,int>::TestHeader();
		returnp.Report();
		param1.Report();
		param2.Report();
		param3.Report();

	}

	void SetName()
	{
			returnp.SetName("returnp");
			param1.SetName("param1");
			param2.SetName("param2");
			param3.SetName("param3");
	}
};

void Test1()
{

UnitTestData testdata1[10];

for(int i=0; i< 10; i++)
{
	testdata1[i].returnp.SetData(1,2);
	testdata1[i].SetName();

	int a=10;
	testdata1[i].param3.AllocateMem(5);
	testdata1[i].param3.SetBufferData(&a,&a,1);
	testdata1[i].param3.SetBufferData(&a,&a,1);
	testdata1[i].param3.SetDatatoBuffer(a,a);
	testdata1[i].Run();
	testdata1[i].Report();
}

}
*/

#if defined(_WINSYS_)

#else


typedef struct FpCtrlStat_
{
	unsigned int um : 1;
	unsigned int om : 1;
	unsigned int zm : 1;
	unsigned int im : 1;

	unsigned int other_m : 4;

	unsigned int us : 1;
	unsigned int os : 1;
	unsigned int zs : 1;
	unsigned int is : 1;

	unsigned int other_s : 4;

	unsigned int pad : 16;
} FpCtrlStat;

#define BUILD64
#define AMD_X87_FP_P 0x0020
#define AMD_X87_FP_U 0x0010
#define AMD_X87_FP_O 0x0008
#define AMD_X87_FP_Z 0x0004
#define AMD_X87_FP_I 0x0001

#define AMD_SSE_FP_CL_P 0x00001000
#define AMD_SSE_FP_CL_U 0x00000800
#define AMD_SSE_FP_CL_O 0x00000400
#define AMD_SSE_FP_CL_Z 0x00000200
#define AMD_SSE_FP_CL_I 0x00000080

#define AMD_SSE_FP_ST_P 0x00000020
#define AMD_SSE_FP_ST_U 0x00000010
#define AMD_SSE_FP_ST_O 0x00000008
#define AMD_SSE_FP_ST_Z 0x00000004
#define AMD_SSE_FP_ST_I 0x00000001


static void assign_fp_x87(unsigned short cwd, unsigned short swd, FpCtrlStat *cs)
{
	cs->um = ( ((cwd & AMD_X87_FP_U) == AMD_X87_FP_U) ? 0 : 1 );
	cs->om = ( ((cwd & AMD_X87_FP_O) == AMD_X87_FP_O) ? 0 : 1 );
	cs->zm = ( ((cwd & AMD_X87_FP_Z) == AMD_X87_FP_Z) ? 0 : 1 );
	cs->im = ( ((cwd & AMD_X87_FP_I) == AMD_X87_FP_I) ? 0 : 1 );

	cs->us = ( ((swd & AMD_X87_FP_U) == AMD_X87_FP_U) ? 1 : 0 );
	cs->os = ( ((swd & AMD_X87_FP_O) == AMD_X87_FP_O) ? 1 : 0 );
	cs->zs = ( ((swd & AMD_X87_FP_Z) == AMD_X87_FP_Z) ? 1 : 0 );
	cs->is = ( ((swd & AMD_X87_FP_I) == AMD_X87_FP_I) ? 1 : 0 );
}

void SetupExcept()
{

			ptrace(PTRACE_REQ PTRACE_TRACEME, 0, 0, 0);

			feclearexcept(FE_ALL_EXCEPT);

			#ifdef BUILD64
			feenableexcept(FE_DIVBYZERO|FE_OVERFLOW|FE_UNDERFLOW|FE_INVALID);
			#elif BUILD32
			raise(SIGFPE);
			#else
			#error PROBLEM
			#endif
}

void ClearExcept()
{
			feclearexcept(FE_ALL_EXCEPT );
			exit(EXIT_SUCCESS);
}




static void assign_fp_sse(unsigned int mxcsr, FpCtrlStat *cs)
{
	cs->um = ( ((mxcsr & AMD_SSE_FP_CL_U) == AMD_SSE_FP_CL_U) ? 0 : 1 );
	cs->om = ( ((mxcsr & AMD_SSE_FP_CL_O) == AMD_SSE_FP_CL_O) ? 0 : 1 );
	cs->zm = ( ((mxcsr & AMD_SSE_FP_CL_Z) == AMD_SSE_FP_CL_Z) ? 0 : 1 );
	cs->im = ( ((mxcsr & AMD_SSE_FP_CL_I) == AMD_SSE_FP_CL_I) ? 0 : 1 );

	cs->us = ( ((mxcsr & AMD_SSE_FP_ST_U) == AMD_SSE_FP_ST_U) ? 1 : 0 );
	cs->os = ( ((mxcsr & AMD_SSE_FP_ST_O) == AMD_SSE_FP_ST_O) ? 1 : 0 );
	cs->zs = ( ((mxcsr & AMD_SSE_FP_ST_Z) == AMD_SSE_FP_ST_Z) ? 1 : 0 );
	cs->is = ( ((mxcsr & AMD_SSE_FP_ST_I) == AMD_SSE_FP_ST_I) ? 1 : 0 );
}


void trace_libm_fn(int pid, FpCtrlStat *pm, unsigned int *n)
{
	int status;
	*n = 0;
	pm->um = 0;
	pm->om = 0;
	pm->zm= 0;
	pm->im = 0;
	pm->other_m = 0;
	pm->us = 0;
	pm->os= 0;
	pm->zs= 0;
	pm->is= 0;
	pm->other_s= 0;
	pm->pad = 0;

	do
	{
		wait(&status);
		if(!WIFSTOPPED(status))
		{
			continue;
		}

		siginfo_t sig_data;
		#ifdef BUILD64
		/*       struct user_i387_struct fp_data; */
		struct user_fpregs_struct fp_data;
		#elif BUILD32
		/*        struct user_fxsr_struct fp_data; */
		struct user_fpxregs_struct fp_data;
		struct user_regs_struct urg;
		#else
		#error PROBLEM
		#endif

		ptrace(PTRACE_REQ PTRACE_GETSIGINFO, pid, 0, &sig_data);

		#ifdef BUILD64
		if(sig_data.si_signo == SIGFPE)
		{
			unsigned int tm;

			ptrace(PTRACE_REQ PTRACE_GETFPREGS, pid, 0, &fp_data);

			assign_fp_sse(fp_data.mxcsr, pm); pm++; (*n)++;
			tm = fp_data.mxcsr;
			tm &= 0xffffffc0;
			fp_data.mxcsr = tm | 0x00001f80;

		if((fp_data.swd & 0x001d) != 0)
		{
			assign_fp_x87(fp_data.cwd, fp_data.swd, pm); pm++; (*n)++;
			fp_data.swd &= 0xffc0;
		}

			ptrace(PTRACE_REQ PTRACE_SETFPREGS, pid, 0, &fp_data);

			ptrace(PTRACE_REQ PTRACE_SINGLESTEP, pid, 0, 0);

			wait(&status);
			if(!WIFSTOPPED(status))
			{
				printf("Control in unexpected place: %s : %d\n", __FILE__, __LINE__);
				continue;
			}

			ptrace(PTRACE_REQ PTRACE_GETFPREGS, pid, 0, &fp_data);
			fp_data.mxcsr = tm;
			ptrace(PTRACE_REQ PTRACE_SETFPREGS, pid, 0, &fp_data);

			ptrace(PTRACE_REQ PTRACE_CONT, pid, 0, 0);
		}
		else
		{
			printf("child stopped NOT because of SIGFPE: %s : %d\n", __FILE__, __LINE__);
			kill(pid, SIGKILL);
		}
		#elif BUILD32
		ptrace(PTRACE_REQ PTRACE_GETFPXREGS, pid, 0, &fp_data);
		ptrace(PTRACE_REQ PTRACE_GETREGS, pid, 0, &urg);

		if((fp_data.swd & 0x001d) != 0)
		{
			assign_fp_x87(fp_data.cwd, fp_data.swd, pm); pm++; (*n)++;
			fp_data.swd &= 0xffc0;
			ptrace(PTRACE_REQ PTRACE_SETFPXREGS, pid, 0, &fp_data);
		}

		ptrace(PTRACE_REQ PTRACE_SINGLESTEP, pid, 0, 0);
		#else
		#error PROBLEM
		#endif

	}while(!WIFEXITED(status));
}

void TraceExcept(int pid, ParameterObj<ExcFlags> &E)
{

			FpCtrlStat *cs = (FpCtrlStat *)calloc(100,sizeof(FpCtrlStat));
			unsigned int num = 0;
			trace_libm_fn(pid, cs, &num);

			E.obtained.us = 0;
			E.obtained.os = 0;
			E.obtained.zs = 0;
			E.obtained.is = 0;
			E.obtained.other_s = 0;

			for(unsigned int i=0; i< num ; i++)
			{

				E.obtained.us |= cs[i].us;
				E.obtained.os |= cs[i].os ;
				E.obtained.zs |= cs[i].zs ;
				E.obtained.is |= cs[i].is ;
				E.obtained.other_s |= cs[i].other_s;
			}

			free(cs);
}

void TraceExcept(int pid, ParameterObjBuffer<ExcFlags,ExcFlags *> &E)
{

			FpCtrlStat *cs = (FpCtrlStat *)calloc(100,sizeof(FpCtrlStat));
			unsigned int num = 0;
			trace_libm_fn(pid, cs, &num);

			for(int j=0; j<E.size;j++)
			{
				E.obtained[j].us = E.expected[j].us;
				E.obtained[j].os = E.expected[j].os;
				E.obtained[j].zs = E.expected[j].zs;
				E.obtained[j].is = E.expected[j].is;
				E.obtained[j].other_s = E.expected[j].other_s;
			}


			free(cs);
}

#endif




#ifdef _WINSYS_
#include <float.h>
#else
#include <fenv.h>
#endif


enum RndType
{
	RT_NEAR,
	RT_DOWN,
	RT_UP,
	RT_ZERO,
};



RndType GetRndType()
{
#ifdef _WINSYS_

    unsigned int cw;
	_controlfp_s(&cw,0,0);
    switch(cw & _MCW_RC)
    {
    case _RC_NEAR:  return RT_NEAR;
    case _RC_DOWN:  return RT_DOWN;
    case _RC_UP:    return RT_UP;
    case _RC_CHOP:  return RT_ZERO;
    default:        return RT_NEAR;
    }

#else

    int rt = fegetround();

    switch(rt)
    {
    case FE_TONEAREST:  return RT_NEAR;
    case FE_DOWNWARD:   return RT_DOWN;
    case FE_UPWARD:     return RT_UP;
    case FE_TOWARDZERO: return RT_ZERO;
    default:            return RT_NEAR;
    }

#endif
}

void SetRndType(RndType rt)
{
#ifdef _WINSYS_

   unsigned int cw;
   _controlfp_s(&cw, 0, 0);

    switch(rt)
    {
    case RT_NEAR:   _controlfp_s(&cw,_RC_NEAR, _MCW_RC); break;
    case RT_DOWN:   _controlfp_s(&cw,_RC_DOWN, _MCW_RC);break;
    case RT_UP:     _controlfp_s(&cw,_RC_UP, _MCW_RC);break;
    case RT_ZERO:   _controlfp_s(&cw,_RC_CHOP, _MCW_RC);break;
    default:        _controlfp_s(&cw,_RC_NEAR, _MCW_RC);
    }

#else

    switch(rt)
    {
    case RT_NEAR:   fesetround(FE_TONEAREST);break;
    case RT_DOWN:   fesetround(FE_DOWNWARD);break;
    case RT_UP:     fesetround(FE_UPWARD);break;
    case RT_ZERO:   fesetround(FE_TOWARDZERO);break;
    default:        fesetround(FE_TONEAREST);
    }

#endif
}


#ifdef _WINSYS_

void corrupt_reg(unsigned long long  val)
{

;
}

#else

void corrupt_reg(unsigned long long  val)
{

        asm("movd %rdi,%xmm0;\
        SHUFPD $0,%xmm0,%xmm0;\
        movapd %xmm0,%xmm1;\
        movapd  %xmm1,%xmm2;\
        movapd %xmm2,%xmm3;\
        movapd %xmm3,%xmm4;\
        movapd %xmm4,%xmm5;\
        movapd %xmm5,%xmm6;\
        movapd %xmm6,%xmm7;\
        movapd %xmm7,%xmm8;\
        movapd %xmm8,%xmm9;\
        movapd %xmm9,%xmm10;\
        movapd %xmm10,%xmm11;\
        movapd %xmm11,%xmm12;\
        movapd %xmm12,%xmm13;\
        movapd %xmm13,%xmm14;\
        movapd %xmm14,%xmm15;\
        movapd %xmm15,%xmm0");

}

#endif
