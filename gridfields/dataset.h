#ifndef _DATASET_H
#define _DATASET_H

#include <vector>
#include "tuple.h"
#include "gfutil.h"

namespace GF {
class Array;

using namespace std;

class RankedDataset;

class Dataset {

 public:
  Dataset(const Scheme &s, size_t N);
  Dataset(unsigned int N) : _size(N) {};
  Dataset(const Dataset &ds) { 
    _size = ds.Size();
    COPY(vector<Array *>, ds.attributes, attributes, ii);
  };
  ~Dataset();
  
  typedef int* IntIterator;
  typedef float* FloatIterator;
 
  void AddAttribute(Array *arr);
  void RemoveAttribute(Array *arr);
  void Zip(const Dataset &arr);
  void Clear();
  void CoerceScheme(Scheme sch, unsigned int sz=0);
  void Apply(const string &unparsedExpr);
  
  void recordOrdinals(string ordname);
 
  void FilterBy(const string &mask, Dataset &Result);
 
  IntIterator BeginInt(const string &attr);
  IntIterator EndInt(const string &attr);
  FloatIterator BeginFloat(const string &attr);
  FloatIterator EndFloat(const string &attr);

  bool IsEmpty() const { return this->attributes.empty(); };

  int IsAttribute(const string &attr) const;

  Array *GetAttribute(const string &attr) const;
  UnTypedPtr GetVoidPointer(const string &attr) const;
  UnTypedPtr GetAttributeVal(const string &attr, idx i) const;

//  void flattenAttr(string attr);
  
  void FastBindTuple(int idx, Tuple &t) const;
  void BindTuple(int idx, Tuple &t) const;
  //void BindTuples(vector<CellId> &cs, vector<Tuple> &ts);

  size_t Size() const;
  int Arity() const;

  Scheme GetScheme() const;

  void PrintTo(ostream &os, int indent=0, int limit=100) const;
  void print(int indent=0) const;

  void nearest(const string &attr, UnTypedPtr p, vector<idx> &out);
  void lookupFloat(const string &attr, float p, vector<idx> &out);
  void lookupInt(const string &attr, int p, vector<idx> &out);
 friend class RankedDataset;
 private:
  Dataset() : _size(0) {};
  size_t _size;
  vector<Array *> attributes;
};

}
#endif /*_DATASET_H */
