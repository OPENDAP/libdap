#ifndef _ARRAY_H
#define _ARRAY_H

#include "access.h"
#include "type.h"
#include "object.h"
#include "expr.h"

#include <string>

namespace GF {
class Scheme;
template <class T> class DatumIterator;


class Array : public Object {

 public:
  Array(std::string nm, Type t);
  Array(std::string nm, Type t, int size);
  Array(Array *a, std::string attr);
  Array(const char *nm, Type t);
  Array(std::string nm, Scheme *sch);
  Array(const char *nm, Scheme *sch);
  void unref();
  void ref();
  virtual ~Array();

  virtual void fill(DatumIterator<int> &d);
  virtual void fill(DatumIterator<float> &d);

  virtual Array *copyAndFilter(bool *filter);
  virtual Array *copy();

  virtual void copyIntData(int *data, int s);
  virtual void shareIntData(int *data, int s);

  virtual void copyFloatData(float *data, int s);
  virtual void shareFloatData(float *data, int s);

  virtual void copyObjData(void **data, int s);
  virtual void shareObjData(void **data, int s);

  virtual void getData(int *&out);
  virtual void getData(float *&out);
  virtual void getData(void **&out);

  virtual void set(unsigned int i, int val) {this->ints[i] = val; };
  virtual void set(unsigned int i, float val) {this->floats[i] = val; };
  virtual void set(unsigned int i, UnTypedPtr val) {this->objs[i] = val; }

  int size() { return _size; }
  Type gettype() { return type; }
  virtual string sname() { return string(this->name); }
  
  virtual void setVals(UnTypedPtr vals, int s);
  virtual UnTypedPtr getVals();
  virtual UnTypedPtr getValPtr(int i);

  virtual inline void next(UnTypedPtr *p) { 
    plusplus(p, type);
  };

  virtual Array *expand(int n);
  virtual Array *repeat(int n);
  virtual void cast(Type t);
  virtual void UnSafeCast(Type t);

  virtual void print();
  virtual string getName() { return string(this->name); };
  virtual void deleteName() { delete [] name; name = NULL; };

  virtual void clear();

  Type type;

  virtual Scheme *getScheme();
 protected:
  int _size;
 private:
  char *name;
  int share;
  int full;
  int *ints;
  float *floats;
  short *bytes;
  void **objs;

  void setType(Type type);
  void init(const char *nm, Type t);

  Scheme *_sch;
};

/*



class ArrayOfVector : public Array, private vector<Tuple> {
 public:

   ArrayOfVector(Scheme *sch, int sz);

 private:
 
}
*/

}
#endif /* _ARRAY_H */
