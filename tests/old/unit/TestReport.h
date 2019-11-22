#include <iostream>
#include <string.h>
#include <fstream>
#if defined( WIN64 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( _WIN32 )
#include <emmintrin.h>
#include <xmmintrin.h>
#else
#include <xmmintrin.h>
#endif



using namespace std;

namespace  TestReport
{

static ofstream testout;

static int testNum = 1;

    static void Header(const char *filename, const char *fname)
    {
        testout.open(filename);
        testout<<"Testing " << fname << endl;
    }

    static void TestHeader()
    {
        testout<<"Test No: "<<testNum<<endl;
        if(testNum == 0)
            testout<<"Parameter,Input,expected,obtained"<<endl;

        testNum++;

    }

    static void Footer()
    {
        testout.close();
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
            testout<<data->name<<","<<data->input<<","<<data->expected<<","<<data->obtained;
			if(data->obtained == data->expected)
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
		}
	};

	template<>
	struct GenreportBuffer< int *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{
            for(int i =0 ; i <  data->size ; i++)
            {
            testout<<data->name<<"["<<i<<"],"<<data->input[i]<<","<<data->expected[i]<<","<<data->obtained[i];
			if(data->obtained[i] == data->expected[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
            }
		}
	};


	template<>
	struct Genreport< float >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
            testout<<data->name<<","<<data->input<<","<<data->expected<<","<<data->obtained;
			if(data->obtained == data->expected)
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
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
            testout<<data->name<<"["<<i<<"],"<<data->input[i]<<","<<data->expected[i]<<","<<data->obtained[i];
			if(data->obtained[i] == data->expected[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
            }
		}
	};


	template<>
	struct Genreport< double >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
            testout<<data->name<<","<<data->input<<","<<data->expected<<","<<data->obtained;
			if(data->obtained == data->expected)
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
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
            testout<<data->name<<"["<<i<<"],"<<data->input[i]<<","<<data->expected[i]<<","<<data->obtained[i];
			if(data->obtained[i] == data->expected[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
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
            union typ
            {
                float f[4];
                unsigned int ui[4];
                __m128 t;
            };

            typ inp,exp,obt;
            inp.t = data->input;
            exp.t = data->expected;
            obt.t = data->obtained;
            for(int i=0;i<4; i++)
            {
                testout<<data->name<<"["<<i<<"]"<<","<<inp.f[i]<<","<<exp.f[i]<<","<<obt.f[i];
			if(obt.ui[i] == exp.ui[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
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
            union typ
            {
                int i[4];
                __m128i t;
            };

            typ inp,exp,obt;
            inp.t = data->input;
            exp.t = data->expected;
            obt.t = data->obtained;
            for(int i=0;i<4; i++)
            {
                testout<<data->name<<"["<<i<<"]"<<","<<inp.i[i]<<","<<exp.i[i]<<","<<obt.i[i];
			if(obt.i[i] == exp.i[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
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
            union typ
            {
                double d[2];
                unsigned long long ull[2];
                __m128d t;
            };

            typ inp,exp,obt;
            inp.t = data->input;
            exp.t = data->expected;
            obt.t = data->obtained;
            for(int i=0;i<2; i++)
            {
                testout<<data->name<<","<<inp.d[i]<<","<<exp.d[i]<<","<<obt.d[i];
			if(obt.ull[i] == exp.ull[i])
						testout<<","<<"PASS"<<endl;
			    else
						testout<<","<<"FAIL"<<endl;
            }
		}
	};


	template<>
	struct Genreport< ExcFlags >
	{
		template< typename TR1>
		static void Report(const TR1 *data)
		{
         /*   testout<<data->name<<","<<data->input<<","<<data->expected<<","<<data->obtained;
			if(data->obtained == data->expected)
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;*/
		}
	};


	template<>
	struct GenreportBuffer< ExcFlags *>
	{
		template< typename TR>
		static void Report(const TR *data)
		{
        /*    for(int i =0 ; i <  data->size ; i++)
            {
            testout<<data->name<<"["<<i<<"],"<<data->input[i]<<","<<data->expected[i]<<","<<data->obtained[i];
			if(data->obtained[i] == data->expected[i])
						testout<<","<<"PASS"<<endl;
			else
						testout<<","<<"FAIL"<<endl;
            }*/
		}
	};


}

