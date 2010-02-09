#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "tuple.h"
#include "expr.h"
#include "gfutil.h"

using namespace GF;
Scheme::Scheme() {}

void Scheme::addAttribute(const string &attr, Type type) {
  if (!this->isAttribute(attr)) {
    pos[attr] = sort.size();
    sort.push_back(make_pair(attr, type));
  }
}

void Scheme::removeAttribute(const string &attr) {
  vector< pair<string, Type> >::iterator i;
  for (i=sort.begin(); i!=sort.end(); i++) {
    if (i->first == attr) {
      sort.erase(i);
      // stop iterating, since we've mutated the vector!
      break;
    }
  }
  rebuildPositions();
}

void Scheme::rebuildPositions() {
  pos.clear();
  for (size_t i=0; i<sort.size(); i++) {
    pos[sort[i].first] = i;
  }
}

Scheme::Scheme(string scheme_string) {
  vector<string> attrs;
  vector<string> a;
  
  split(scheme_string, ";, ", attrs);

  vector<string>::iterator p;

  for (p=attrs.begin(); p!=attrs.end(); p++) { 
    split(*p, ":", a);
    if (a.size() == 1) {
//      cout << "SCHEME: " << a[0] << endl;
      this->addAttribute(a[0], FLOAT);
    } else if (a.size() == 2) {
      this->addAttribute(a[0], typeval(a[1]));
    } else {
      cout << "Parse error for scheme string: " << scheme_string << endl;
      exit(1);
    }
    a.clear();
  }
}

bool Scheme::operator==(const Scheme &s) {
  return (*this) >= s && (*this) <= s;
}
bool Scheme::operator>=(const Scheme &s) {
  return this->Subsumes(s);
}
bool Scheme::operator<=(const Scheme &s){
  return s.Subsumes((*this));
}

bool Scheme::Subsumes(const Scheme &sch) const {
  string attr;
  for (size_t i=0; i<sch.size(); i++) {
    attr = sch.getAttribute(i);
    if (this->isAttribute(attr)) {
      if (this->getType(attr) != sch.getType(attr)) {
      //  return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

bool Tuple::Covers(Scheme &sch) {
  return scheme->Subsumes(sch);
}

bool Tuple::CoveredBy(Scheme &sch) {
  return sch.Subsumes(*scheme);
}

string Scheme::asString() {
  stringstream retval;
  for (size_t i=0; i<this->size(); i++) {
    retval << this->getAttribute(i);
    retval << ":";
    retval << typeformat(this->getType(i));
    retval << ", ";
  }
  return retval.str();
}

size_t Scheme::byteposition(const string &attr) {
  int ord = this->getPosition(attr);
  if (ord == -1) {
    Fatal("Scheme::byteposition : Attribute '%s' not found.", attr.c_str() );
  }
  
  int bytes = 0;
  for (int i=0; i<ord; i++) {
    bytes += typesize(this->getType(i));
  }
  return bytes;
}

size_t Scheme::bytesize() {
  int s = this->size();
  int bytes = 0;
  for (int i=0; i<s; i++) {
    bytes += typesize(this->getType(i));
  }
  return bytes;
}

size_t Scheme::size() const { return sort.size(); }

Type Scheme::getType(const string &attr) const {
  return this->getType(this->getPosition(attr));
}

Type Scheme::getType(int i) const {
  assert(i>=0 && i<sort.size());
  return sort[i].second;
}

int Scheme::getPosition(const string &attr) const {
  map<string, int>::const_iterator i;
  i = pos.find(attr);
  if (i == pos.end()) {
    stringstream ss;
    Scheme *s = (Scheme *) this; 
    s->PrintTo(ss,0);
    Fatal("Attribute '%s' not found in scheme:\n %s", attr.c_str(), ss.str().c_str());
    return -1;
  } else {
    return i->second;
  }
}

string Scheme::formatOf(int position) {
  string fmt;
  fmt = typeformat(this->getType(position));
  return "%" + fmt;
}

bool Scheme::isAttribute(const string &nm) const {
  if (pos.find(nm) != pos.end()) 
    return true;
  return false;
}

string Scheme::getAttribute(int position) const {
  assert(position>=0  && position<sort.size());
  return sort[position].first;
}

void Scheme::print() { print(0); }
void Scheme::print(int indent) { PrintTo(cout,0); }

void Scheme::PrintTo(ostream &os, int indent) { 
  int i;
  vector<pair<string, Type> >::iterator pos;
  for (i=0; i<indent; i++) os << " ";
  os << "<";
  for (pos=sort.begin(); pos!=sort.end(); ++pos) {
    os << pos->first << " : " << pos->second << ", ";
  }
  if (pos==sort.begin()) {
    os << "<no attributes>";
  }
  os << ">" << endl;
}

Tuple::Tuple(Scheme *sch) {
  scheme = sch;
  tupledata.reserve(scheme->size());
  for (size_t i=0; i<scheme->size(); i++) {
    tupledata.push_back(NULL);
  }
}

void Tuple::Next() {
  vector<UnTypedPtr>::iterator it;
  Scheme::FieldIterator fi;
  fi=this->scheme->begin();  
  for (it=tupledata.begin(); it!=tupledata.end(); it++) {
    plusplus(&(*it), fi->second);
    fi++;
    //*it = (UnTypedPtr) (((int) (*it)) + sizeof(int));
    //((int *) (*it))++;// = (UnTypedPtr) (((int) (*it)) + sizeof(int));
  }
}

bool Tuple::isNull() {
  for (size_t i=0; i<scheme->size(); i++) {
    if (tupledata[i] != NULL) return false;
  }
  return true;
}

/*
Tuple::Tuple(const Tuple &tup) {
  scheme = tup.scheme;
  tupledata = tup.tupledata;
}
*/

UnTypedPtr Tuple::get(string attr) {
  int i = scheme->getPosition(attr);
  //cout << "got position: " << attr << ", " << i << endl;
  if (i == -1) Fatal("Tuple get: attribute %s not found.", attr.c_str());
  return tupledata[i];
}

void Tuple::set(string attr, UnTypedPtr val) {
  int i = scheme->getPosition(attr);
  //cout << ": " << *(float *) val << endl;
  tupledata[i] = val;
}

UnTypedPtr Tuple::operator[](size_t i) {
  if (i > tupledata.size()) {
    this->print();
    Fatal("Tuple: Index out of range %i", i);
  }    
  return tupledata[i];  
}

int Tuple::bytesize() {
  int s = this->scheme->size();
  int bytes = 0;
  for (int i=0; i<s; i++) {
    bytes += typesize(this->scheme->getType(i));
  }
  return bytes;
}

void Tuple::Read(ifstream &f) {
  // arbitrary limit...
  char textline[256];
  f.getline(textline, 256);
  this->Parse(textline);
}
 
void Tuple::Parse(char *text) {

  stringstream ss(text);
  int s = this->scheme->size();
  int *x = new int;
  float *y = new float;
  for (int j=0; j<s; j++) {
    switch (this->scheme->getType(j)) {
      case INT:
        ss >> *x;
        tupledata[j] = (UnTypedPtr) x;
        break;
      case FLOAT:
        ss >> *y;
        tupledata[j] = (UnTypedPtr) y;
        break;
      default:
         this->print();
         Fatal("Tuple::Parse: only ints an floats right now...");
    }
  }
}

void Tuple::copy(Tuple &t) {
    int n = scheme->size();
    for (int i=0; i<n; i++) {
      //int j=t.getPosition(this->getAttribute(i));
      // assume tuples have same scheme
      int j=i;
      switch (scheme->getType(i)) {
        case INT:
          *((int*) tupledata[i]) = *((int *) t.tupledata[j]);
          break;
        case FLOAT:
          *((float*) tupledata[i]) = *((float *) t.tupledata[j]);
          break;
        case OBJ:
          *((UnTypedPtr *) tupledata[i]) = *((UnTypedPtr *) t.tupledata[j]);
          break;
        case TUPLE:
          *((UnTypedPtr *) tupledata[i]) = *((UnTypedPtr *) t.tupledata[j]);
          break;
        case GRIDFIELD:
          *((UnTypedPtr *) tupledata[i]) = *((UnTypedPtr *) t.tupledata[j]);
          break;
      }
    }
}

string Tuple::asString(const string &delim) {
  stringstream os;
  if (scheme->size() != 0) { 
    printattr(os, 0);
    for (size_t i=1; i<scheme->size(); i++) {
      os << delim;
      printattr(os,i);
    }
  }
  return os.str();
}

void Tuple::assign(char *rawbytes) {
  char *p = rawbytes;
  for (size_t i=0; i<scheme->size(); i++) {
     this->set(scheme->getAttribute(i), p);
     p += typesize(scheme->getType(i));
  }
}

void Tuple::print() { print(0); }

void Tuple::print(int indent) {
  PrintTo(cout, indent);
}

void Tuple::PrintTo(ostream &os, int indent) {
  os << tab(indent) << scheme->size() << ", " << tupledata.size() << ": ";
  assert(scheme->size() == tupledata.size());
  if (scheme->size() == 0) { os << "(empty tuple)"; }
  
  os << "<" << scheme->getAttribute(0) << "=";
  os << tupledata[0] << "->";
  printattr(os, 0);    
  for (size_t i=1; i<scheme->size(); i++) {
    os << ", " << scheme->getAttribute(i) << "=";
    os << tupledata[i] << "->";
    printattr(os, i);
  }
  os << ">" << endl;
  
}

void Tuple::printattr(ostream &os, int i) {
  switch (scheme->getType(scheme->getAttribute(i))) {
    case INT:
      os <<  *((int*) tupledata[i]);
      break;
    case FLOAT:
      os <<  *((float*) tupledata[i]);
      break;
    case OBJ:
      os << "object(" << *(UnTypedPtr **) tupledata[i] << ")";
      break;
    case TUPLE:
      os << "object(" << *(UnTypedPtr **) tupledata[i] << ")";
      break;
    case GRIDFIELD:
      os << "object(" << *(UnTypedPtr **) tupledata[i] << ")";
      break;
  }
}
