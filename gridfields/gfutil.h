#ifndef _UTIL_H
#define _UTIL_H

#include <ext/hash_map>
#include <vector>
#include <string>
#include <stdio.h>

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>=0?(a):-(a))
#define SQR(x) ((x)*(x))
#define SIGN(x) ((x)==0?0:(x)<0?-1:1)
#define SIGN2(x,y) ((x)*SIGN(y))

#define FOR(type, x, xs) \
       type::iterator x;     \
       type::iterator end##x=xs.end(); \
       for (x=xs.begin(); x != end##x; x++) \

#define COPY(type, from, to, ii) \
        insert_iterator<type > ii(to, to.begin()); \
        copy(from.begin(), from.end(), ii);


struct nullstream:
std::ostream {
nullstream(): std::ios(0), std::ostream(0) {}
};

#define DEBUG nullstream()
//#define DEBUG cout

void split(const std::string &text, const std::string separators, std::vector<std::string> &words);
std::string remove_whitespace(const std::string &text);
bool same(const std::string &r, const std::string &s);

void Fatal(const char *fmt, ...); 
void Warning(const char *fmt, ...); 


std::string tab(int indent);
typedef size_t idx;
#endif /* _UTIL_H */
    

