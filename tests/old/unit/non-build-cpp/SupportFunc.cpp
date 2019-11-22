#include "SupportFunc.h"
#include <string.h>

int
LTrim(std::string &s) {
    if (s.empty())
    { return 0; }

    std::string::iterator p;
    int len = 0;

    for (p = s.begin(); p != s.end() && (*p == ' ' || *p == '\t' || *p == '\f' ||
                    *p == '\r'); p++) {
        len++;
    }

    s.erase(0, len);
    return len;
}


int
ValidateKey(const char *par_instr, std::string keys[], int len) {
    int loc_mid,
        loc_high,
        loc_low;
    loc_low = 0;
    loc_high = len - 1;

    /*
     *Binary search.
     */

    while (loc_low <= loc_high) {
        loc_mid = ((loc_low + loc_high) / 2);

        if (strcmp((const char *)keys[loc_mid].c_str(), (const char *)par_instr) < 0) {
            loc_low = (loc_mid + 1);
        } else if (strcmp((const char *)keys[loc_mid].c_str(),
                        (const char *)par_instr) > 0) {
            loc_high = (loc_mid - 1);
        } else {
            return loc_mid;
        }
    }

    /* Not a key word */
    return -1;
}

std::string
ReadLine(std::ifstream &infile) {
    std::string data;
    getline(infile, data);
    return data;
}


std::string
replacestr(const std::string &str, std::string find, std::string rep) {
    size_t pos;
    std::string st = str;
    pos = st.find(find);

    while (pos != std::string::npos) {
        st.replace(pos, find.size(), rep);
        pos = st.find(find, pos + 1);
    }

    return st;
}

std::string
replacestrExp(const std::string &str, std::string find, std::string rep) {
    size_t pos;
    std::string st = str;
    pos = st.find(find);

    while (pos != std::string::npos) {
        st.replace(pos, find.size(), rep);
        pos = st.find(find, pos + 2);
    }

    return st;
}

void
Trim(std::string &buffer) {
    if (buffer.length() == 0) { return; }

    std::string delimiters = " \r\n\t"; // delimiter symbol array
    const char desired_space = ' ';
    // trim leading white space
    std::string::size_type  lead_non_white = buffer.find_first_not_of(delimiters);

    if (lead_non_white == std::string::npos) {
        buffer.resize(0);
        buffer += desired_space;
        return;
    }

    buffer.erase(0, lead_non_white);
    // trim trailing white space
    std::string::size_type trail_non_white = buffer.find_last_not_of(delimiters);

    if (trail_non_white < (buffer.length() - 1))
    { buffer.erase(trail_non_white + 1); }

    std::string buffer_copy;

    while (1) {
        std::string::size_type  lead_white = buffer.find_first_of(delimiters);

        if (lead_white == std::string::npos) {
            buffer_copy += buffer;
            break;
        } else {
            buffer_copy += buffer.substr(0, lead_white);
            buffer_copy += desired_space;
            buffer.erase(0, lead_white);
            std::string::size_type  lead_non_white = buffer.find_first_not_of(delimiters);
            buffer.erase(0, lead_non_white);
        }
    }

    buffer = buffer_copy;
}



std::string
GetDeltaData(std::string &str) {
    size_t pos, end;
    std::string sep;
    sep = "delta";
    pos =  str.find(sep);
    std::string token;

    if (std::string::npos == pos) {
        token = "0";
    } else {
        sep = "=";
        pos =  str.find(sep, pos + 1);
        end = str.length();
        token = str.substr(pos + 1, end - pos);
    }

    Trim(token);
    return token;
}

std::string
GetExpData(std::string &str) {
    std::string token;
    size_t pos, end;
    std::string sep;
    sep = "out";
    pos =  str.find(sep);

    if (pos == std::string::npos) {
        token = "init";
        return token;
    }

    sep = "=";
    pos =  str.find(sep, pos + 1);
    sep = "delta";
    end =  str.find(sep, pos + 1);

    if (end == std::string::npos) {
        end = str.length();
    } else {
        end = end - 1;
    }

    token = str.substr(pos + 1, end - pos);
    Trim(token);
    return token;
}


std::string
GetInitData(std::string &str) {
    size_t pos, end;
    std::string sep;
    sep = "=";
    pos =  str.find(sep);
    sep = "out";
    end =  str.find(sep);
    std::string token;
    token = str.substr(pos + 1, end - 1 - pos);
    Trim(token);
    return token;
}

std::string
GetToken(const std::string &str) {
    size_t pos;
    std::string sep;
    sep = "*~\n\t\r ";
    pos =  str.find_first_of(sep);
    std::string token;

    if (pos != std::string::npos) {
        pos = pos == 0 ? 1 : pos;
        token = str.substr(0, pos);
    } else {
        token = str;
    }

    Trim(token);
    return token;
}




std::list<std::string *>   *
SplitString(const std::string &str, const char *sep, int include) {
    std::list<std::string *> *mylist = new std::list<std::string *>;
    size_t found = -1, strt = 0;
    size_t  len;

    do {
        std::string *st;
        found = str.find_first_of(sep, found + 1);

        if (std::string::npos == found) { // done last part of the string
            len = str.length() - strt;

            if (0 != len) {
                st = new std::string(str.substr(strt, len));
                Trim(*st);
                mylist->push_back(st);
            }

            break;
        }

        len = found - strt;

        if (0 == len) { // seperator found
            if (include) {
                st = new std::string(str.substr(strt, 1));
                Trim(*st);
                mylist->push_back(st);
            }

            strt++;
            continue;
        }

        st = new std::string(str.substr(strt, len));
        Trim(*st);
        mylist->push_back(st);

        if (include) {
            st = new std::string(str.substr(found, 1));
            Trim(*st);
            mylist->push_back(st);
        }

        strt = found + 1;
    } while (1);

    return mylist;
}

void
FreeLst(std::list<std::string *> *mylst) {
    std::list<std::string *>::iterator lit;

    for (lit = mylst->begin() ; lit != mylst->end(); lit++) {
        delete *lit;
    }

    mylst->clear();
    delete mylst;
}



