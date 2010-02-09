#ifndef _TUPLE_H
#define _TUPLE_H

#include "type.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

namespace GF {
class Scheme {
 public:
  Scheme();
  Scheme(string scheme_string);

  typedef vector<pair<string, Type> >::iterator FieldIterator;
  void addAttribute(const string &attr, Type t);
  void removeAttribute(const string &attr);

  void clear() { sort.clear(); rebuildPositions(); }
  bool Subsumes(const Scheme &sch) const;
  FieldIterator begin() { return sort.begin(); }
  FieldIterator end() { return sort.end(); }
  
  size_t size() const;
  size_t bytesize() ;
  size_t byteposition(const string &attr);

  Type getType(const string &attr) const;
  Type getType(int i) const;
  int getPosition(const string &attr) const;
  string getAttribute(int position) const ;
  bool isAttribute(const string &nm) const ;
  
  bool operator==(const Scheme &s);
  bool operator>=(const Scheme &s);
  bool operator<=(const Scheme &s);
  string formatOf(int position);

  string asString() ;

  void PrintTo(ostream &os, int indent=0) ;
  void print() ;
  void print(int indent) ;

 private:
  void rebuildPositions();
  //pair<int, Type> getValue(string attr);
  vector<pair<string, Type> > sort;
  map<string, int> pos;
};

class Tuple {
typedef vector<UnTypedPtr> TupleData;
 public:
  Tuple(Scheme *);

  Scheme *getScheme() { return scheme; };

  UnTypedPtr get(int pos) { return tupledata[pos]; };
  void set(int pos, UnTypedPtr val) { tupledata[pos] = val; };
  
  UnTypedPtr get(string attr);
  void set(string attr, UnTypedPtr val);

  string getAttribute(int i) { return scheme->getAttribute(i); };
  int size() { return scheme->size(); };

  string asString(const string &delim=", ");
  void print();
  void print(int indent);
  void PrintTo(ostream &os, int indent=0) ;

  int bytesize();
  void Next();
  char *Allocate() { 
    char *data = new char[this->bytesize()]; 
    this->assign(data); 
    return data; 
  };
  void assign(char *rawbytes);
  void copy(Tuple &t);
  bool isNull();
  void Read(ifstream &f);
  void Parse(char *text);
  bool Covers(Scheme &sch);
  bool CoveredBy(Scheme &sch);
  
  UnTypedPtr operator[](size_t i);
  
  friend class Dataset;
  friend class TupleFunction;
  friend class SpecializedTupleFunction;
 private:
  void printattr(ostream &os, int i);
  Scheme *scheme;
  TupleData tupledata;
};

}
#endif /*_TUPLE_H */
