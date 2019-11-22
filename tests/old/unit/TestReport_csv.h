#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>

#if defined( WIN64 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( _WIN32 )
#include <emmintrin.h>
#include <xmmintrin.h>
#else
#include <xmmintrin.h>
#endif



using namespace std;



#define PASS_COLOR  "#008000"
#define FAIL_COLOR "#FF0000"

namespace  TestReport
{

#if defined(_txt_)
#define testout cout
#else
static ofstream testout;
#endif
static int testNum = 1;
static int testFailed = 0;
static bool teststatus;
static std::ostringstream out_data,ulp_data;


    static void Header(const char *filename,int seed = 0)
    {
		#if defined(_csv_)
			testout.open(filename);
		#endif
        //testout<<"Testing " << fname << endl;
		testout<<"Unit Test Report"<<std::endl;
		if(seed !=0)
		{
			testout<<"Random test seed value: "<<seed<<std::endl;
		}
    }

    static void FuncHeader(const char *fname)
    {
        //testout.open(filename);
		testNum = 1;
		testout<<"Test Function :,"<< fname<<std::endl;
        testout.flush();

    }

    static void TestHeader()
    {

		out_data<<"Test Case No,"<<std::dec<<testNum<<std::endl;
		out_data<<"Input,Expected,Obtained,Status"<<std::endl;

        testNum++;
		teststatus=true;
    }

    static void TestFooter(double time)
    {
		out_data<<"Time taken to execute the test :  "<<time<<std::endl;

#if defined(_FAIL_ONLY_)
			if(!teststatus )
			{
				#if defined (__NUM_FAILED_TEST__)
				if(testFailed < __NUM_FAILED_TEST__)
				#endif
				testout<<out_data.str();
			}
#else
		testout<<out_data.str();
#endif

		out_data.str("");
		out_data.clear();

		testout.flush();
		if(!teststatus)
			testFailed++;
    }


    static void Footer(double ulp=0,double rel_diff=0)
    {

	if((testNum-1) != 0)
        {
            testout<<"\nTotal Test "<<std::dec<<--testNum<<std::endl;
		    testout<<"Test Failed "<<std::dec<<testFailed<<std::endl;
        }
    if(ulp!=0)
	{
	  testout<<"Max Ulp Error :"<<ulp<<std::endl;
	  testout<<"ULP Data : "<<std::endl;
	  testout<<ulp_data.str();
		ulp_data.str("");
		ulp_data.clear();

	}
        if(rel_diff!=0)
	{
	  testout<<"Max Relative diff :"<<rel_diff<<std::endl;
	}




        testout.flush();

		#if defined(_csv_)
        testout.close();
		#endif
    }

	void ReportPerf(double testtime)
	{
		testout<<"Time in Mega Exec/sec :  "<<testtime<<std::endl;
        testout.flush();

	}


	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name,DT inp, DT exp, DT obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<name<<std::dec<<"["<<std::dec<<index<<"],";
		}
		else
		{
			out_data<<name<<",";
		}

		out_data<<std::dec<<inp<<","<<exp<<","<<obt<<",";
		if(color.compare(PASS_COLOR) == 0)
		{
			out_data<<"PASS"<<std::endl;
		}
		else
		{
			out_data<<"FAIL"<<std::endl;
		}
	}

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, float inp,  float exp,  float obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<name<<"["<<std::dec<<index<<"],";
		}
		else
		{
			out_data<<name<<",";
		}

		Type_Conv typexp,typobt,typinp;
		typinp.f[0] = inp;
		typexp.f[0] = exp;
		typobt.f[0] = obt;

		out_data<<inp<<"  (0x"<<std::hex<<typinp.i[0]<<"),";
		out_data<<exp<<"  (0x"<<std::hex<<typexp.i[0]<<"),";
		out_data<<obt<<"  (0x"<<std::hex<<typobt.i[0]<<"),";

		if(color.compare(PASS_COLOR) == 0)
		{
			out_data<<"PASS"<<std::endl;
		}
		else
		{
			out_data<<"FAIL"<<std::endl;
		}
	}

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name,ExcFlags inp, ExcFlags exp, ExcFlags obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<name<<"["<<std::dec<<index<<"],";
		}
		else
		{
			out_data<<name<<",";
		}


		out_data<<"( U="<<inp.us<<" O="<<inp.os<<" Z="<<inp.zs<<" I="<<inp.is<<" T="<< inp.other_s<<" ),";
		out_data<<"( U="<<exp.us<<" O="<<exp.os<<" Z="<<exp.zs<<" I="<<exp.is<<" T="<< exp.other_s<<" ),";
		out_data<<"( U="<<obt.us<<" O="<<obt.os<<" Z="<<obt.zs<<" I="<<obt.is<<" T="<< obt.other_s<<" ), ";
	    
        if(color.compare(PASS_COLOR) == 0)
		{
			out_data<<"PASS"<<std::endl;
		}
		else
		{
			out_data<<"FAIL"<<std::endl;
		}
    
    }



	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, double inp,  double exp,  double obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<name<<"["<<std::dec<<index<<"],";
		}
		else
		{
			out_data<<name<<",";
		}

		Type_Conv typexp,typobt,typinp;
		typinp.d[0] = inp;
		typexp.d[0] = exp;
		typobt.d[0] = obt;

		out_data<<inp<<"  (0x"<<std::hex<<typinp.ll[0]<<"),";
		out_data<<exp<<"  (0x"<<std::hex<<typexp.ll[0]<<"),";
		out_data<<obt<<"  (0x"<<std::hex<<typobt.ll[0]<<"),";

		if(color.compare(PASS_COLOR) == 0)
		{
			out_data<<"PASS"<<std::endl;
		}
		else
		{
			out_data<<"FAIL"<<std::endl;
		}
	}
	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, __m128 inp,  __m128 exp,  __m128 obt,std::string &color, int index)
	{

		Type_Conv typexp,typobt,typinp;
		typinp.xf = inp;
		typexp.xf = exp;
		typobt.xf = obt;

		for(int i=0; i< 4; i++)
		{
			out_data<<name<<"["<<std::dec<<i<<"],";
			out_data<<typinp.f[i]<<" (0x"<<std::hex<<typinp.i[i]<<"),";
			out_data<<typexp.f[i]<<" (0x"<<std::hex<<typexp.i[i]<<"),";
			out_data<<typobt.f[i]<<" (0x"<<std::hex<<typobt.i[i]<<"),";

			if(color.compare(PASS_COLOR) == 0)
			{
				out_data<<"PASS"<<std::endl;
			}
			else
			{
				out_data<<"FAIL"<<std::endl;
			}
		}
	}

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, __m128d inp,  __m128d exp,  __m128d obt,std::string &color, int index)
	{

		Type_Conv typexp,typobt,typinp;
		typinp.xd = inp;
		typexp.xd = exp;
		typobt.xd = obt;

		for(int i=0; i< 2; i++)
		{
			out_data<<name<<"["<<std::dec<<i<<"],";
			out_data<<typinp.d[i]<<" (0x"<<std::hex<<typinp.ll[i]<<"),";
			out_data<<typexp.d[i]<<" (0x"<<std::hex<<typexp.ll[i]<<"),";
			out_data<<typobt.d[i]<<" (0x"<<std::hex<<typobt.ll[i]<<"),";

			if(color.compare(PASS_COLOR) == 0)
			{
				out_data<<"PASS"<<std::endl;
			}
			else
			{
				out_data<<"FAIL"<<std::endl;
			}
		}
	}

}

