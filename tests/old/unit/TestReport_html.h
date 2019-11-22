#include <iostream>
#include <list>
#include <string.h>
#include <fstream>
#include <sstream>

#if defined( WIN64 ) || defined( _WIN64 ) || defined( WIN32 ) || defined( _WIN32 )
#include <emmintrin.h>
#include <xmmintrin.h>
#else
#include <xmmintrin.h>
#endif




#define PASS_COLOR  "#008000"
#define FAIL_COLOR "#FF0000"

using namespace std;

typedef std::list<int> List;
namespace  TestReport
{

static ofstream testout;
static int testNum = 1;
static int testFailed = 0;
static bool teststatus;
static std::ostringstream out_data,ulp_data;
static List failedTestCases;

    static void Header(const char *filename,int seed =0)
    {
        testout.open(filename);
        //testout<<"Testing " << fname << endl;

		testout<<"<html><head><meta http-equiv=\"Content-Language\" content=\"en-us\"><meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">";
		testout<<"<title>Unit Test Report</title></head><body><p><font color=\"#008000\" size=\"5\"><b>Unit Test Report</b></font></p>";
		if(seed !=0)
		{
			testout<<"<p><b><font color=\"#008000\" size=\"4\">Random Test seed value :  "<< seed <<"</font></b></p>";
		}
		testout.flush();

    }

    static void FuncHeader(const char *fname)
    {
        //testout.open(filename);
		testNum = 1;
		testFailed = 0;

		testout<<"<p><b><font color=\"#008000\" size=\"4\">Test Function :  "<< fname <<"</font></b></p>";
		testout.flush();


    }

    static void TestHeader()
    {

		out_data<<"<p>&nbsp;</p>";
		out_data<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
		out_data<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Test Case No "<<std::dec<<testNum<<"</b></td>";
		out_data<<"</tr><tr><td width=\"20%\" bgcolor=\"#99CCFF\"><b>&nbsp;Parameters</b></td>";
		out_data<<"<td width=\"20%\" bgcolor=\"#99CCFF\"><b>&nbsp;Input</b></td>";
		out_data<<"<td width=\"20%\" bgcolor=\"#99CCFF\"><b>&nbsp;Expected</b></td>";
		out_data<<"<td width=\"20%\" bgcolor=\"#99CCFF\"><b>&nbsp;Obtained</b></td></tr>";

        testNum++;
		teststatus=true;
    }

    static void TestFooter(double time)
    {

		out_data<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Time taken to execute the test :  "<<time<<"</b></td></tr>";
		out_data<<"</table>";



#if defined(_FAIL_ONLY_)
			if(!teststatus)
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
		{
			testFailed++;
			failedTestCases.push_back(testNum-1);
		}


    }


    static void Footer(double ulp=0,double rel_diff=0)
    {

		testout<<"<p>&nbsp;</p>";

		if((testNum-1) != 0)
		{
			testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
			testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Total Test&nbsp;&nbsp;&nbsp;"<<std::dec<<--testNum<<"</b></td></tr>";
			testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Test Failed&nbsp;&nbsp;&nbsp;"<<std::dec<<testFailed<<"</b></td></tr></table>";
		}


     if(ulp !=0)
     {
		testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
		testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Maximum Ulp error&nbsp;&nbsp;&nbsp;"<<ulp<<"</b></td></tr></table>";

		testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
		testout<<ulp_data.str();
		testout<<"</table>";
		ulp_data.str("");
		ulp_data.clear();
    }

        if(rel_diff!=0)
	{
		testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
		testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Maximum Relative  diff&nbsp;&nbsp;&nbsp;"<<rel_diff<<"</b></td></tr></table>";
	}

                List::iterator itr = failedTestCases.begin();
                if( itr != failedTestCases.end())
                {
					testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
					testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Failed Test case numbers:&nbsp;&nbsp;&nbsp;";
					testout<<std::dec<< *itr;
					itr++;
						for(; itr != (failedTestCases.end()); itr++)
						testout<< ", " << *itr ;
					testout << "</b></td></tr></table>";
                }

		testout<<"</body></html>";
		testout.flush();
        testout.close();
    }

	void ReportPerf(double testtime)
	{
		testout<<"<p>&nbsp;</p>";
		testout<<"<table border=\"1\" cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\" width=\"100%\" id=\"AutoNumber1\">";
		testout<<"<tr> <td width=\"100%\" colspan=\"4\" bgcolor=\"#99CCFF\"><b>&nbsp;Time in Mega Exec/sec  :  "<<testtime<<"</b></td></tr>";
		testout<<"</table>";
		testout.flush();

	}


	template<int BUFFER_TYPE>
	void OutData(const char *name,char *inp, char *exp, char *obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<std::dec<<"["<<index<<"]"<<"</b></font></td>";
		}
		else
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"</b></font></td>";
		}


        if(inp == NULL)
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; NULL </b></font></td>";
        else
        out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<inp<<"</b></font></td>";

        if( exp == NULL)
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; NULL </b></font></td>";
        else
        out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<exp<<"</b></font></td>";

        if( obt == NULL)
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; NULL </b></font></td>";
        else
        out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<obt<<"</b></font></td>";
    }

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name,DT inp, DT exp, DT obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<index<<"]"<<"</b></font></td>";
		}
		else
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"</b></font></td>";
		}


		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<inp<<"</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<exp<<"</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<std::dec<<obt<<"</b></font></td></tr>";
    }

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name,ExcFlags inp, ExcFlags exp, ExcFlags obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<index<<"]"<<"</b></font></td>";
		}
		else
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"</b></font></td>";
		}


		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; "<<inp.us<<inp.os<<inp.zs<<inp.is<<inp.other_s<<" </b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; "<<exp.us<<exp.os<<exp.zs<<exp.is<<exp.other_s<<" </b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp; "<<obt.us<<obt.os<<obt.zs<<obt.is<<obt.other_s<<" </b></font></td></tr>";
	}



	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, float inp,  float exp,  float obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<std::dec<<index<<"]"<<"</b></font></td>";
		}
		else
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"</b></font></td>";
		}

		Type_Conv typexp,typobt,typinp;
		typinp.f[0] = inp;
		typexp.f[0] = exp;
		typobt.f[0] = obt;

		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<inp<<"&nbsp;&nbsp;(0x"<<std::hex<<typinp.i[0]<<")</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<exp<<"&nbsp;&nbsp;(0x"<<std::hex<<typexp.i[0]<<")</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<obt<<"&nbsp;&nbsp;(0x"<<std::hex<<typobt.i[0]<<")</b></font></td></tr>";
	}

	template<typename DT,int BUFFER_TYPE>
	void OutData(const char *name, double inp,  double exp,  double obt,std::string &color, int index)
	{

		if(BUFFER_TYPE)
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<index<<"]"<<"</b></font></td>";
		}
		else
		{
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"</b></font></td>";
		}

		Type_Conv typexp,typobt,typinp;
		typinp.d[0] = inp;
		typexp.d[0] = exp;
		typobt.d[0] = obt;

		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<inp<<"&nbsp;&nbsp;(0x"<<std::hex<<typinp.ll[0]<<")</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<exp<<"&nbsp;&nbsp;(0x"<<std::hex<<typexp.ll[0]<<")</b></font></td>";
		out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<obt<<"&nbsp;&nbsp;(0x"<<std::hex<<typobt.ll[0]<<")</b></font></td></tr>";
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
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<i<<"]"<<"</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typinp.d[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typinp.ll[i]<<")</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typexp.d[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typexp.ll[i]<<")</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typobt.d[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typobt.ll[i]<<")</b></font></td></tr>";
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
			out_data<<"<tr><td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<name<<"["<<std::dec<<i<<"]"<<"</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typinp.f[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typinp.i[i]<<")</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typexp.f[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typexp.i[i]<<")</b></font></td>";
			out_data<<"<td width=\"20%\"><font color=\""<<color.c_str()<<"\"><b>&nbsp;"<<typobt.f[i]<<"&nbsp;&nbsp;(0x"<<std::hex<<typobt.i[i]<<")</b></font></td></tr>";
		}
	}

}

