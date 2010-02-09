#include <iostream>
#include "array.h"
#include "access.h"
#include "gfutil.h"
#include "tuple.h"
#include <string>
#include <string.h>
using namespace std;
using namespace GF;

#define DEBUG 0
/*
Array::Array(string nm, Scheme *sch, int sz) {
  if (sch->size() == 1) {
    init(nm, sch->getType(0));
    switch (t) {
      case INT:
        this->ints = new int[sz];
        break;
      case FLOAT:
        this->floats = new float[sz];
        break;
      case OBJ:
        this->objs = new UnTypedPtr[sz];
        break;
    }
  } else {
    init(nm, OBJ);
    this->objs = new UnTypedPtr[sz];
  }
  _size = sz;
  _sch = sch;
  this->full = true;
}
*/

Array::Array(string nm, Type t, int sz) {
  init(nm.c_str(), t);
  switch (t) {
    case INT:
      this->ints = new int[sz];
      break;
    case FLOAT:
      this->floats = new float[sz];
      break;
    case OBJ:
      this->objs = new UnTypedPtr[sz];
      break;
  }
  _size = sz;
  this->full = true;
}

Array::Array(const char *nm, Type t) {
  init(nm, t);
}
Array::Array(string nm, Type t) {
  init(nm.c_str(), t);
}
void Array::fill(DatumIterator<int> &d) {
  assert(type==INT);
  d.Open();
  int i=0;
  while (i<this->size() && !d.Done()) {
    ints[i++] = d.Next();
    cout << i << ", " << ints[i-1] << endl;
  }
}
                                                                       
void Array::fill(DatumIterator<float> &d) {
  assert(type==FLOAT);
  d.Open();
  int i=0;
  while (i<size() && !d.Done()) {
    floats[i++] = d.Next();
    cout << i << ", " << floats[i-1] << endl;
  }
}


Array::Array(Array *a, string nm) {
  // If 'a' is a tuple-valued array, this method copies 
  // the given attribute out of the tuples and creates a new array
  Type t;
  if (a->type == OBJ) {
    Scheme *sch = a->getScheme();
    if (sch->isAttribute(nm)) {
      t = sch->getType(nm);
      init(nm.c_str(), t);
      this->_sch = sch;
      this->_size = a->_size;
//    } else {
//      Fatal("Array Copy: %s not a sub-attribute of this array");
    }
    //   copy the data to the new array
    Tuple *tup;
    if (t == INT) {
      this->ints = new int[_size];
      for (int i=0; i<this->_size; i++) {
        tup = (Tuple *) a->getValPtr(i);
        this->ints[i] = *(int *)tup->get(nm);
      }
    } else if (t == FLOAT) {
      this->floats = new float[_size];
      for (int i=0; i<this->_size; i++) {
        tup = (Tuple *) a->getValPtr(i);
        this->floats[i] = *(float *)tup->get(nm);
      }
    } else if (t == OBJ) {
      this->objs = new UnTypedPtr[_size];
      for (int i=0; i<this->_size; i++) {
        this->objs[i] = ((Tuple *) a->getValPtr(i))->get(nm);
      }
    } else {
        Fatal("array Copy: Unkown type");
    }
  } else {
    Fatal("Array Copy: Not a tuple typed array.");
  }
  this->full = 1;
  this->ref();
}

Array::Array(const char *nm, Scheme *sch) {
  if (sch->size() == 1)
    init(nm, sch->getType(0));
  else
    init(nm, OBJ);
  _sch = sch;
}
Array::Array(string nm, Scheme *sch) {
  if (sch->size() == 1)
    init(nm.c_str(), sch->getType(0));
  else
    init(nm.c_str(), OBJ);
  _sch = sch;
}

void Array::init(const char *nm, Type t) {
  _size = 0;
  int x = strlen(nm) + 1;
  name = new char[x];
  _sch = NULL;
  strcpy(name, nm);
  type = t;
  share = false;
  full = 0;
  //increment reference count
  this->ref();
}

void Array::getData(void **&out) {
  Type t = OBJ;
  if (type == t) {
    out = objs;
  } else {
    Warning("Array %s has type %i, not type %i", name, type, t);
    out =  NULL;
  }
}
void Array::getData(float *&out) {
  Type t = FLOAT;
  if (type == t) {
    out = floats;
  } else {
    Warning("Array %s has type %i, not type %i", name, type, t);
    out = NULL;
  }
}
void Array::getData(int *&out) {
  Type t = INT;
  if (type == t) {
    out = ints;
  } else {
    Warning("Array has type %i, not type %i", name, type, t);
    out = NULL;
  }
}

void Array::copyIntData(int *data, int s) {
  Array *arr = this;
  arr->clear();
  arr->setType(INT);
  arr->_size = s;  
  if (data == NULL) return; 
  arr->ints = new int[s];
  arr->share = false;
  memcpy(arr->ints, data, sizeof(int)*s);
  arr->full = 1;
}

void Array::shareIntData(int *data, int s) {
  Array *arr = this;
  arr->share = true;
  arr->clear();
  arr->setType(INT);
  arr->_size = s;  
  if (data == NULL) return; 
  arr->ints = data;
  arr->full = 1;
}

void Array::copyFloatData(float *data, int s) {
  Array *arr = this;
  arr->share = false;
  arr->clear();
  arr->setType(FLOAT);
  arr->_size = s;
  if (data == NULL) return; 
  arr->floats = new float[s];
  memcpy(arr->floats, data, sizeof(float)*s);
  arr->share = false;
  arr->full = 1;
}

void Array::shareFloatData(float *data, int s) {
  Array *arr = this;
  arr->share = true;
  arr->clear();
  arr->setType(FLOAT);
  arr->floats = data;
  arr->_size = s;
  arr->full = 1;
}

void Array::copyObjData(UnTypedPtr *data, int s) {
  Array *arr = this;
  arr->share = false;
  arr->clear();
  //cout << arr->name << endl;
  if (type == FLOAT) {
    arr->floats = new float[s];
    for (int i=0; i<s; i++) {
      arr->floats[i] = *(float *)data[i];
    }
  } else if (type == INT) {
    arr->ints = new int[s];
    arr->ints[0] = -1;
    for (int i=0; i<s; i++) {
      arr->ints[i] = *(int *)data[i];
    }    
  } else if (type == OBJ) {
    arr->objs = new UnTypedPtr[s];
    for (int i=0; i<s; i++) {
      arr->objs[i] = data[i];
    }    
  } 
  arr->_size = s;
  arr->share = false;
  if (data==NULL) return;
}

void Array::shareObjData(void **data, int _size) {
  Array *arr = this;
  arr->share = true;
  arr->clear();
  arr->setType(OBJ);
  arr->_size = _size;
  arr->objs = data;
  arr->full = 1;
}

Array *Array::copy() {
  Array *arr = this;
  int i;
  Array *newarr;
  UnTypedPtr vals = arr->getVals();

  //One of many places that makes bad assumptions about datum _sizes
  UnTypedPtr newvals = (UnTypedPtr) new UnTypedVal[_size];

  newarr = new Array(arr->name, arr->type);
  memcpy(newvals, vals, arr->_size*sizeof(UnTypedPtr));    
  newarr->setVals(newvals, _size);

  return newarr;
}

void Array::setVals(UnTypedPtr vals, int _size) {
  Array *arr = this;
  arr->_size = _size;

  arr->clear();
  switch (arr->type) {
  case INT:
    arr->ints = (int *) vals;
    break;
  case FLOAT:
    arr->floats = (float *) vals;
    break;
  case OBJ:
    arr->objs = (UnTypedPtr *) vals;
    break;
  default:
    Warning("Unknown type: %i", type);
    break;
  }
  this->full = 1;
}

Scheme *Array::getScheme() {
  if (_sch) return _sch;
  _sch = new Scheme();
  _sch->addAttribute(this->name, this->type);
  return _sch;
}

Array *Array::copyAndFilter(bool *filter) {
  Array *arr;
  arr = this;
  Array *newarr;
  UnTypedPtr newvals;
  UnTypedPtr vals;
  int i;
  int j = 0;;
  int new_size=0;

  if (filter == NULL) {
    newarr = arr->copy();
  } else {

    newarr = new Array(arr->name, arr->type);
    for (i=0; i<arr->_size; i++) {
      if (filter[i]) { new_size++; }
    }

    //    UnTypedPtr newvals = (UnTypedPtr) new UnTypedVal[_size];

    switch (arr->type) {
    case INT: {
      int *newvals = new int[new_size];
      int *vals = (int *) arr->getVals();
      for (i=0; i<arr->_size; i++) {
	if (filter[i]) {
	  newvals[j++] = vals[i];
	}
      }    
      newarr->setVals(newvals, new_size);
      break;
    }
    case FLOAT: {
      float *newvals = new float[new_size];
      float *vals = (float *) arr->getVals();
      for (i=0; i<arr->_size; i++) {
	if (filter[i]) {
	  newvals[j++] = vals[i];
	}
      }   
      newarr->setVals(newvals, new_size);
      break;
    }
    case OBJ: {
      UnTypedPtr *newvals = new UnTypedPtr[new_size];
      UnTypedPtr *vals = (UnTypedPtr *) arr->getVals();
      for (i=0; i<arr->_size; i++) {
	if (filter[i]) {
	  newvals[j++] = vals[i];
	}
      }   
      newarr->setVals(newvals, new_size);
      break;
    }
    default:
      Warning("unknown Type.");
    }
    
  }
  
  assert(newarr->share == false);
  return newarr;
};

void Array::UnSafeCast(Type t) {
  Array *arr = this;
  switch (arr->type) {
  case INT:
    if (t==INT) return;
    else if (t==OBJ) break;
    arr->floats = (float *)arr->ints;
    arr->type = t;
    return;
    break;
  case FLOAT:
    if (t==FLOAT) return;
    else if (t==OBJ) break;
    arr->ints = (int *)arr->floats;
    arr->type = t;
    return;
    break;
  default:
    break;
  }
  Warning("Can only cast ints and floats");

}

void Array::cast(Type t) {

  if (t != INT && t!= FLOAT) {
    Warning("Can only cast numeric types");
    return;
  }
  Array *arr = this;
  
  switch (arr->type) {
  case INT:
    if (t==INT) return;
    arr->floats = new float[arr->_size];
    for (int i=0; i<arr->_size; i++) {
      floats[i] = float(arr->ints[i]);
    }
    arr->type = t;
    if (arr->share) {
      ints = NULL;
    } else { 
      delete [] ints;
    }
    break;
  case FLOAT:
    if (t==FLOAT) return;
    arr->ints = new int[arr->_size];
    for (int i=0; i<arr->_size; i++) {
      ints[i] =  int(arr->floats[i]);
    }
    arr->type = t;
    if (arr->share) {
      floats = NULL;
    } else { 
      delete [] floats;
    }
    break;
  default:
    Warning("Can only cast ints and floats");
  }
}

void Array::clear() {
  Array *arr = this;
  if (!arr->full) return;
  if (arr->share) return;
  switch (arr->type) {
  case INT:
    if (arr->ints != NULL) delete [] arr->ints;
    break;
  case FLOAT:
    if (arr->floats != NULL) delete [] arr->floats;
    break;
  case OBJ:
      if (arr->objs != NULL) delete [] arr->objs;
      break;
  default:
    Warning("clear: unknown type");
  }
  arr->ints = NULL;
  arr->floats = NULL;
  arr->objs = NULL;
  arr->full = 0;
}

void Array::setType(Type type) {
  Array *arr = this;
  if (arr->type != type)
    Warning("Changing array type from %i to %i",arr->type, type);
  
  arr->type = type;
}

 
UnTypedPtr Array::getValPtr(int i) {
//  assert(i<_size);
  Array *arr;
  arr = this;

  switch (arr->type) {
  case INT:
    return (UnTypedPtr) &arr->ints[i];
    break;
  case FLOAT:
    return (UnTypedPtr) &arr->floats[i];
    break;
  case OBJ:
    return (UnTypedPtr) &arr->objs[i];
    break; 
  default:
    Warning("Unknown type: %i", arr->type);
    break;
  }

  return NULL;  
}

UnTypedPtr Array::getVals() {
  Array *arr;
  arr = this;
  switch (arr->type) {
  case INT:
    return (UnTypedPtr) arr->ints;
    break;
  case FLOAT:
    return (UnTypedPtr) arr->floats;
    break;
  case OBJ:
    return (UnTypedPtr) arr->objs;
    break;
  default:
    Warning("Unknown type: %i", arr->type);
    break;
  }

  return NULL;

}

Array *Array::expand(int n) {
  Array *out = new Array(name, type);
  int tsize = typesize(this->type);
  int arraysize = _size*tsize;
  char *newvals = new char[n*arraysize];
  char *oldvals = (char *) getVals();
  for (int j=0; j<_size; j++) {
    for (int i=0; i<n; i++) {
      for (int k=0; k<tsize; k++) { 
        char c = oldvals[j*tsize + k];
        newvals[i*tsize + j*n*tsize + k] = c;
      }
    }
  }
  out->setVals((UnTypedPtr *) newvals, n*_size);
  out->share = false;
  return out;
}

Array *Array::repeat(int n) {
  Array *out = new Array(name, type);
  int tsize = typesize(this->type);
  int arraysize = _size*tsize;
  char *newvals = new char[n*arraysize];
  char *oldvals = (char *) getVals();
  for (int i=0; i<n; i++) {
    for (int j=0; j<arraysize; j++) {
      newvals[i*arraysize + j] = oldvals[j];
    }
  }
  out->setVals((UnTypedPtr *) newvals, n*_size);
  out->share = false;
  return out;
}

void Array::print() {
  Array *arr;
  arr = this;
  int i;

  cout << "address: " << arr << "\n";
  cout << "name: " << arr->name << "\n";
  cout << "_size: " << arr->_size << "\n";
  cout << "type: " << arr->type << "\n";
  cout << "data: " << "\n";
  
  switch (arr->type) {
  case INT:
    for (i=0; i<MIN(_size,100); i++) {
      cout << " " << arr->ints[i];
      if (i%10 == 100) cout <<"\n";
    }
    break;
  case FLOAT:
    for (i=0; i<MIN(100,_size); i++) {
      cout << " " << arr->floats[i];
      if (i%100 == 100) cout <<"\n";
    }
    break;
  case OBJ:
    for (i=0; i<MIN(100,_size); i++) {
      cout << " " << arr->objs[i] ;
      if (i%100 == 100) cout <<"\n";
    }
    break;
  default:
    cout << "unknown type";
  }
  if (i < _size) cout << "...";
  cout << "\n";

}

void Array::ref() {
//  int old = refcount;
  //if (strcmp(name, "mag") == 0) cout << "increment " << name << endl;
  Object::ref();
  //cout << "array " << this->name << ", ref: " << old << " -> " << this->refcount << endl;
}

void Array::unref() {
  //  if (strcmp(name, "mag") == 0) cout << "decrement " << name << endl;
  //int old = refcount;
  Object::unref();
  //cout << "array " << this->name << ", unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {
    //cout << "deleting " << this->name << "..." << endl;
    delete this;
  }
}

Array::~Array() {
  //cout << "~Array: " << this << endl;
  Array *arr;
  arr = this;
  //if (strcmp(name, "mag") == 0) cout << *(int *)NULL << endl;
  delete [] arr->name;
  if (arr->share == false) {
    arr->clear();
    arr->name = NULL;
  }
}
namespace GF {
int *__dfi;
CArrayIterator<int> __di(__dfi, 0);
float *__dfp;
CArrayIterator<float> __df(__dfp, 0);
}
