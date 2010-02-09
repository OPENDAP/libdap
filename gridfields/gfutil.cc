#include <string>
#include <iostream>
#include <sstream>
#include "expr.h"
#include "gridfield.h"
#include "tuple.h"
#include "fparser.hh"
extern "C" {
  #include <stdarg.h>
}

using namespace std;

string tab(int indent) {
  string s;
  for (int i=0; i< indent; i++) {
    s += " ";
  }
  return s;
}

void split(const string &text, const string separators, vector<string> &words) {
  int n = text.length();
  int start, stop;
  
  start = text.find_first_not_of(separators);
  while ((start>=0) && (start<n)) {
    stop = text.find_first_of(separators, start);
    if ((stop<0) || (stop>n)) stop = n;
    words.push_back(text.substr(start, stop-start));
    start = text.find_first_not_of(separators, stop+1);
  }
}

void Fatal(const char *fmt, ...) {
    // vulnerable to buffer overruns!
    char foo[1000];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(foo, fmt, ap);
    va_end(ap);
    
    stringstream errmsg;
    errmsg << foo << endl;
    cout << "Fatal Error: " << errmsg.str();
    throw errmsg.str();
}
 
void Warning(const char *fmt, ...) {
    // vulnerable to buffer overruns!
    char foo[1000];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(foo, fmt, ap);
    va_end(ap);
    
    stringstream errmsg;
    errmsg << foo << endl;
    cout << "Warning: " << errmsg.str();
}

bool same(const string &r, const string &s) {
  return (remove_whitespace(r) == remove_whitespace(s));
}

string remove_whitespace(const string &text) {
  stringstream ss(text);
  string buf, out;
  while (ss >> buf) 
    out += buf;
  return out;
}

