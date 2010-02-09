#include "dataset.h"
#include "array.h"
#include "gfutil.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace GF;

Dataset::Dataset(const Scheme &s, size_t N) : _size(N) {
  if (s.size() > 0) {
    Array *arr = new Array(s.getAttribute(0), s.getType(0), N);
    this->AddAttribute(arr);
    this->CoerceScheme(s);
  }
}

Dataset::~Dataset() {
  for (int i=0; i<this->Arity(); i++) {
    attributes[i]->unref();
  }
}

void Dataset::RemoveAttribute(Array *arr) {
  vector<Array *>::iterator p;
  for (p=this->attributes.begin(); p!=this->attributes.end(); p++) {
    if (*p == arr) {
      (*p)->unref();
      this->attributes.erase(p);
      return;
    }
  }
}

void Dataset::recordOrdinals(string ordname) {
  Array *arr = new Array(ordname, INT, this->Size());
  int *ords;
  arr->getData(ords);
  idx n=this->Size();
  for (int i=0; i<n; i++) ords[i] = i;
  this->AddAttribute(arr);
}

void Dataset::Clear() {
  vector<Array *>::iterator p;
  for (p=this->attributes.begin(); p!=this->attributes.end(); p++) {
    (*p)->unref();
  }
  this->attributes.clear();
}

Dataset::IntIterator Dataset::BeginInt(const string &attr) {
  // iterator starting at the first element of the attribute.
  Array *a = this->GetAttribute(attr);
  int *ii;
  
  if (a->type != INT) {
    Fatal("Type mismatch: IntIterator requested for attribute of type %s.", typeformat(a->type));
  } else {
    ii = (int *) a->getValPtr(0);
    return IntIterator(ii);
  }
}

Dataset::IntIterator Dataset::EndInt(const string &attr) {
  // iterator just past the end of the attribute.
  Array *a = this->GetAttribute(attr);
  int *ii;
  
  if (a->type != INT) {
    Fatal("Type mismatch: IntIterator requested for attribute of type %s.", typeformat(a->type));
  } else {
    ii = (int *) a->getValPtr(0);
    return IntIterator(ii + a->size());
  }
}

Dataset::FloatIterator Dataset::BeginFloat(const string &attr) {
  Array *a = this->GetAttribute(attr);
  float *fi;

  if (a->type != FLOAT) {
    Fatal("Type mismatch: FloatIterator requested for attribute of type %s.", typeformat(a->type));
  } else {
    fi = (float *) a->getValPtr(0);
    return FloatIterator(fi);
  }
}

Dataset::FloatIterator Dataset::EndFloat(const string &attr) { 
  // iterator just past the end of the attribute.
  Array *a = this->GetAttribute(attr);
  float *fi;

  if (a->type != FLOAT) {
    Fatal("Type mismatch: FloatIterator requested for attribute of type %s.", typeformat(a->type));
  } else {
    fi = (float *) a->getValPtr(0);
    return FloatIterator(fi + a->size());
  }
} 

UnTypedPtr Dataset::GetAttributeVal(const string &attr, idx i) const { 
  return this->GetAttribute(attr)->getValPtr(i);
};

void Dataset::AddAttribute(Array *arr) {

  if (!arr) {
    Fatal("AddAttribute: array is NULL");
  }

  if (arr->size() != this->Size() && !this->IsEmpty()) {
    Fatal("Cardinality of array (%i) does not match cardinality of dataset (%i)", arr->size(), this->Size());
  }
  
  if (IsAttribute(string(arr->getName()))) {
    Array *a = this->GetAttribute(arr->getName());
    if (a != arr) {
      Fatal("Dataset already contains a different array named %s", arr->getName().c_str());
    } else {
      // we already have this array as an attribute
      return;
    }
  }
  
  attributes.push_back(arr);
  //increment ref count
  arr->ref();
}

void Dataset::print(int indent) const { PrintTo(cout, 5); };
 
void Dataset::Zip(const Dataset &d) {
  vector<Array *>::const_iterator p;
  if (this->IsEmpty()) { this->_size = d.Size(); }
  for (p=d.attributes.begin(); p!=d.attributes.end(); p++) {
    this->AddAttribute((*p));
  }
}

UnTypedPtr Dataset::GetVoidPointer(const string &attr) const {
  return GetAttribute(attr)->getVals();
}

Array *Dataset::GetAttribute(const string &attr) const {
  int i;
  if (i = IsAttribute(attr)) {
    return attributes[i-1];
  } else {
    Fatal("%s is not an attribute of this gridfield", attr.c_str());
    return NULL;
  }
}

void Dataset::CoerceScheme(Scheme sch, unsigned int sz) {
  // Coerces the dataset to subsume the input scheme
  // new attributes are generated with default values
  // existing attributes are reordered to match the input scheme
  vector<Array *> copy;
   
  // put the input attributes in front
  unsigned int n = sz == 0 ? this->Size() : sz;
  for (int i=0; i<sch.size(); i++) {
    string a = sch.getAttribute(i);
    Type t = sch.getType(i);
    if (this->IsAttribute(a)) {
      Array *keeper = this->GetAttribute(a);
      copy.push_back(keeper);
      keeper->ref();
      this->RemoveAttribute(keeper);
    } else {
      copy.push_back(new Array(a, t, n));
    }
  }
 
  // add the remainder to the end
  vector<Array *>::iterator p;
  for (p=this->attributes.begin(); p!=this->attributes.end(); p++) {
    copy.push_back(*p);
    (*p)->ref();
  }                  
  this->Clear();
  this->attributes.swap(copy);
}

void Dataset::FilterBy(const string &mask, Dataset &Result) {
  if (!IsAttribute(mask)) {
    Fatal("Attempt to filter with mask '%s', but '%s' is not an attribute.", mask.c_str(), mask.c_str());
  }

  Array *m = this->GetAttribute(mask);
  m->cast(INT);
  int *filter;
  m->getData(filter);
  
  vector<Array *>::iterator ai;
  vector<Array *>::iterator stop = attributes.end();
  
  for (ai=attributes.begin(); ai!=stop; ai++) {
    Array *newarr = (*ai)->copyAndFilter((bool *) filter);
    Result.AddAttribute(newarr);
  }
}

void Dataset::Apply(const string &unparsedExpr) {
  Dataset *Gg = this;

  SpecializedTupleFunction tf;

  tf.Parse(unparsedExpr);

  Scheme *func_insch = tf.InputType();
  Scheme *func_outsch = tf.ReturnType();

  Scheme real_insch = Gg->GetScheme();

  // check that all needed attributes appear in the gridfield
  if (!real_insch.Subsumes(*func_insch)) {
    stringstream ss;
    ss << "'"<< unparsedExpr << "'" << endl;
    ss << "required type: ";
    func_insch->PrintTo(ss);
    ss << "actual type: ";
    real_insch.PrintTo(ss);
    Fatal("Type Error in Apply Operator: %s", ss.str().c_str());
  }

  //allocate new attributes as necessary to match required output type
  // and change types if necessary
  
  Gg->CoerceScheme(*func_outsch);
  Scheme real_outsch = Gg->GetScheme();

  tf.SpecializeFor(real_outsch);

  // get a tuple of the new coerced scheme
  Tuple tup(&real_outsch);

  int n = Gg->Size();

  float bt=0;
  float et=0;
  float t;
  Gg->BindTuple(0, tup);

  vector<pair<Array*, UnTypedPtr*> > fast_loop;
  Array *arr;
  string attr;
  for (int i=0; i<tup.size(); i++) {
    attr = tup.getAttribute(i);
    arr = Gg->GetAttribute(attr);
    fast_loop.push_back(make_pair(arr, &tup.tupledata[i]));
  }

  for (int i=0; i<Gg->Size(); i++) {
    vector<pair<Array*, UnTypedPtr*> >::iterator p;
    tf.Eval(tup, tup);
    for (p=fast_loop.begin(); p!=fast_loop.end(); p++) {
      p->first->next(p->second);
    }
  } 
  for (int i=0; i<tup.size(); i++) {
    attr = tup.getAttribute(i);
    arr = Gg->GetAttribute(attr);
    //arr->UnSafeCast(FLOAT);
  }
}

int Dataset::IsAttribute(const string &attr) const {
  int j=1;
  vector<Array *>::const_iterator p;
  for (p=this->attributes.begin(); p!=this->attributes.end(); p++) {
    if (attr == (*p)->getName()) {
      return j;
    }
    j++;
  }
  return 0;
}

int Dataset::Arity() const { return this->attributes.size(); };

Scheme Dataset::GetScheme() const {
  Scheme s;
  string n;
  vector<Array *>::const_iterator p;
  for (p=this->attributes.begin(); p!=this->attributes.end(); p++) {
    n.assign((*p)->getName());
    s.addAttribute(n, (*p)->type);   
  }
  return s;
}

void Dataset::FastBindTuple(int idx, Tuple &t) const {
  /* random access to all attributes of a dataset
     assumes the scheme of the tuple matches the 
     scheme of the dataset
  */
  Array *arr;
  UnTypedPtr ptr;
  assert(idx < this->Size());
  for (int i=0; i<t.size(); i++) {
    //this->attributes[i]->print();
    //cout << this->attributes[i]->getValPtr(idx) << endl;
    t.set(i, this->attributes[i]->getValPtr(idx));
  }
}


void Dataset::BindTuple(int idx, Tuple &t) const {
  /* random access to all attributes of a dataset
  */
/* if the tuple has attributes that do not appear in the gridfield,
 * they will be NULL (as in NULL pointers, not pointers to NULL)
 */
  string arr;
  UnTypedPtr ptr;

  //cout << idx << ", " << this->Size() << endl;
  assert(idx < this->Size());
  for (int i=0; i<t.size(); i++) {
    if (int j = this->IsAttribute(t.getAttribute(i))) {
      Array *arr = this->attributes[j-1];
      t.set(i, arr->getValPtr(idx));
    } else {
      stringstream ss;
      this->GetScheme().PrintTo(ss);
      Fatal("BindTuple: attribute %s not in scheme %s", t.getAttribute(i).c_str(), ss.str().c_str());
    }
  }
}

size_t Dataset::Size() const {
  return _size;
  if (this->IsEmpty()) {
    return 0;
  } else {
    return attributes[0]->size();
  }
}

void Dataset::PrintTo(ostream &os, int indent, int limit) const { 
  int i;
  Scheme s = this->GetScheme();
  Tuple t(&s);
  os << tab(indent) << "dataset: " << endl;
  if (this->IsEmpty()) return;
  for (i=0; i<this->Size(); i++) {
    this->BindTuple(i, t);
    t.PrintTo(os, indent+4);
    if (limit>0 && i>=limit) break;
  }
}

void Dataset::nearest(const string &attr, UnTypedPtr p, vector<idx> &out) {
  typedef int valtype;
  Array *arr = this->GetAttribute(attr);
  valtype near, v;
  valtype pf = *(valtype *)p;
  near = *(valtype *)arr->getValPtr(0);
  int nearest=0;

  for (int i=1; i<arr->size(); i++) {
    v = *(valtype *)arr->getValPtr(i);
    if (abs(pf-v) <= abs(pf-near)) {
      nearest = i;
      near = v;
    }
  }
  out.push_back(nearest);
}

void Dataset::lookupFloat(const string &attr, float p, vector<idx> &out) {

  Array *arr = this->GetAttribute(attr);
  int x = int(p);
  switch (arr->type) {
    case FLOAT: 
  
      for (int i=0; i<arr->size(); i++) {
       //cout << p << ", " <<  *(float*)arr->getValPtr(i) << endl;
       if (p == *(float*)arr->getValPtr(i)) {
         out.push_back(i);
        }
      }
      break;
    case INT:
      
      for (int i=0; i<arr->size(); i++) {
       //cout << x << ", " <<  *(int*)arr->getValPtr(i) << endl;
       if (x == *(int*)arr->getValPtr(i)) {
         out.push_back(i);
        }
      }
      break;
    case OBJ:
      Fatal("Array is not of type float.");
      break;
  }
}

void Dataset::lookupInt(const string &attr, int p, vector<idx> &out) {

  Array *arr = this->GetAttribute(attr);
  float x = float(p);
  switch (arr->type) {
    case FLOAT: 
  
      for (int i=0; i<arr->size(); i++) {
       //cout << x << ", " <<  *(float*)arr->getValPtr(i) << endl;
       if (x == *(float*)arr->getValPtr(i)) {
         out.push_back(i);
        }
      }
      break;
    case INT:
      for (int i=0; i<arr->size(); i++) {
       //cout << p << ", " <<  *(int*)arr->getValPtr(i) << endl;
       if (p == *(int*)arr->getValPtr(i)) {
         out.push_back(i);
        }
      }
      break;
    case OBJ:
      for (int i=0; i<arr->size(); i++) {
       if (p == *(int*)arr->getValPtr(i)) {
         out.push_back(i);
        }
      }
  }
}

/*
void Dataset::BindTuples(vector<CellId> &cs, vector<Tuple> &ts) {
  // binds n tuples corresponding to n cells
  assert(cs.size() == ts.size());
  
  vector<Cell>::iterator cptr;
  for (int i=0; i<cs.size(); i++) {
    BindTuple(cs[i], ts[i]);
  }
}
*/

