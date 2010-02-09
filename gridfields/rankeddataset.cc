#include "rankeddataset.h"
#include "dataset.h"
#include "array.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace GF;
UnTypedPtr RankedDataset::GetVoidPointer(const string &attr, Dim_t d) const { 
   if (d<=MaxRank()) {
     return ranks[d].GetVoidPointer(attr);
   } else {
     Fatal("dim %i requested on a rankeddataset of dim %i", d, MaxRank());
     return NULL;
   }
}
   
Dataset::IntIterator RankedDataset::BeginInt(Dim_t d, const string &attr) {
   if (d<=MaxRank()) {
     return ranks[d].BeginInt(attr);
   } else {
     Fatal("dim %i requested on a rankeddataset of dim %i", d, MaxRank());
     return NULL;
   }
}

Dataset::IntIterator RankedDataset::EndInt(Dim_t d, const string &attr) {
   if (d<=MaxRank()) {
     return ranks[d].EndInt(attr);
   } else {
     Fatal("dim %i requested on a rankeddataset of dim %i", d, MaxRank());
     return NULL;
   }
}

Dataset::FloatIterator RankedDataset::BeginFloat(Dim_t d, const string &attr) {
   if (d<=MaxRank()) {
     return ranks[d].BeginFloat(attr);
   } else {
     Fatal("dim %i requested on a rankeddataset of dim %i", d, MaxRank());
     return NULL;
   }
}

Dataset::FloatIterator RankedDataset::EndFloat(Dim_t d, const string &attr) {
   if (d<=MaxRank()) {
     return ranks[d].EndFloat(attr);
   } else {
     Fatal("dim %i requested on a rankeddataset of dim %i", d, MaxRank());
     return NULL;
   }
}

void RankedDataset::GetDataset(Dim_t d, const string &attrs, Dataset &ds) {
  vector<string> words;
  split(attrs, ",; :/", words);

  FOR(vector<string>, w, words) {
    Array *arr = this->GetAttribute(d, *w);
    ds.AddAttribute(arr);
    
    this->RemoveAttribute(d, *w);
  }
}

void RankedDataset::Zip(Dim_t d, const Dataset &ds) {
  /*
  if (Size(d) != ds.Size()) {
    std::stringstream ss;
    ss << "Cardinality of G_";
    ss << d << " (" << Size(d) << ") ";
    ss <<"does not match cardinality of dataset (";
    ss << ds.Size() << ")";
    Fatal(ss.str().c_str());
    //throw GFException(ss.str());
  }
  */
  ranks[d].Zip(ds);
}

void RankedDataset::Apply(const string &expr, Dim_t d) {
  if (d<=this->Dim()) {
    this->ranks[d].Apply(expr);
  }
}

void RankedDataset::PrintTo(ostream &os, int indent) const {
  os << "RankedDataset:" <<endl;
  vector<Dataset>::const_iterator di;
  
  for (di=this->ranks.begin(); di!=this->ranks.end(); di++) {
    di->PrintTo(os, indent+4);
  }
}

void RankedDataset::unref() {
  int old = refcount;
  Object::unref();
  DEBUG << "rankeddatset.unref " << this << ", " << "unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {

    DEBUG << "....deleting" << endl;
    delete this;
  }
}
