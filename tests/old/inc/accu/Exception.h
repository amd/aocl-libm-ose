#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <iostream>
#include <cstdlib>

#define STRINGIZE(x) #x
#define MAKE_STRING(x) STRINGIZE(x)
#define ERROR_LOCATION " :" __FILE__ ":" MAKE_STRING(__LINE__)

class GeneralException
{
    const char *pMsg;
    const char *pLoc;

public:
    GeneralException(const char *v_pMsg="", const char *v_pLoc="") : pMsg(v_pMsg), pLoc(v_pLoc)
    {}

    void PrintMessageAndExit()
    {
        std::cout << pMsg << pLoc << std::endl;
        exit(-1);
    }
};

#endif // __EXCEPTION_H__

