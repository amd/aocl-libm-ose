#ifndef __SUPPORTFUNC_H__
#define __SUPPORTFUNC_H__

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <string>


int LTrim(std::string& s);
int ValidateKey(const char *par_instr,std::string keys[],int len);
std::string ReadLine(std::ifstream &infile);
std::string  replacestr(const std::string &str, std::string find, std::string rep);
void Trim(std::string &buffer);
std::string GetDeltaData(std::string &str);
std::string GetExpData(std::string &str);
std::string GetInitData(std::string &str);
std::string GetToken(const std::string &str);
std::list<std::string *>  * SplitString(const std::string &str, const char *sep, int include=1);
void FreeLst(std::list<std::string *> *mylst);
std::string  replacestrExp(const std::string &str, std::string find, std::string rep);

#endif //__SUPPORTFUNC_H__
