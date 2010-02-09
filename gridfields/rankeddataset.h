#ifndef _RANKEDDATASET_H
#define _RANKEDDATASET_H

#include "dataset.h"
#include "type.h"
#include "object.h"
#include "cell.h"
#include <assert.h>
#include <iostream>
#include <string>

using namespace std;

namespace GF {
typedef vector<size_t> Shape;

class RankedDataset : public Object {

 public:
   RankedDataset(const Shape &s) { 
     this->ref(); 
     this->SetShape(s);
   };
   void SetShape(const Shape &s) {
     this->Clear();
     for (unsigned int i=0; i<s.size(); i++) {
       ranks.push_back(Dataset(s[i]));
     }
   }
  
 
   RankedDataset() { this->ref(); };
   Dim_t Dim() { return ranks.size()-1; };
 
   UnTypedPtr GetVoidPointer(const string &attr, Dim_t d) const;

   Dataset::IntIterator BeginInt(Dim_t d, const string &attr);
   Dataset::IntIterator EndInt(Dim_t d, const string &attr);
   Dataset::FloatIterator BeginFloat(Dim_t d, const string &attr);
   Dataset::FloatIterator EndFloat(Dim_t d, const string &attr);

   void Clear() { ranks.clear(); }

   void CoerceScheme(Dim_t k, Scheme *sch, size_t sz=0) { assert(k<(Dim_t)ranks.size()); return ranks[k].CoerceScheme(*sch, sz);};
  
   void AddAttribute(Dim_t k, Array *data) { 
     if (k>=(Dim_t)ranks.size()) {
       Fatal("Attempt to add an attribute at rank %i for rankeddataset of dim %i", k,this->Dim());
     }
     ranks[k].AddAttribute(data); 
   };
   void Bind(Dim_t k, Array *data) { this->AddAttribute(k, data); };
   void unBind(Dim_t k, const string &attr) { this->RemoveAttribute(k, attr); };

   void unref();
   Scheme GetScheme(Dim_t k) const { 
     if (k<(Dim_t)ranks.size()) return ranks[k].GetScheme(); 
     else return Scheme();
   };
   
   bool IsAttribute(Dim_t k, const string &attr) {
     if (k<(Dim_t)ranks.size()) return ranks[k].IsAttribute(attr); 
     else return false;
   };
   Array *GetAttribute(Dim_t k, const string &attr) { 
      if (k<(Dim_t)ranks.size()) return ranks[k].GetAttribute(attr); 
      else {
        Fatal("Request for an attribute at rank %i on a gridfield with max rank %i", k, ranks.size()-1);
        return NULL;
      }
   };
   void RemoveAttribute(Dim_t k, const string &attr) {
     if (k>=(Dim_t)ranks.size()) return;
     ranks[k].RemoveAttribute(GetAttribute(k, attr)); 
   };
   
   UnTypedPtr GetAttributeValue(Dim_t k, const string &attr, idx i) { 
     return ranks[k].GetAttributeVal(attr, i); 
   };
   float GetFloatAttributeValue(Dim_t k, const string &attr, idx i) { 
     return *(float *) ranks[k].GetAttributeVal(attr, i); 
   };
   float GetIntAttributeValue(Dim_t k, const string &attr, idx i) { 
     return *(int *) ranks[k].GetAttributeVal(attr, i); 
   };
  
   size_t Arity(Dim_t k) { if (k<(Dim_t)ranks.size()) return ranks[k].Arity(); else return 0; };
   size_t Size(Dim_t k) { if (k<(Dim_t)ranks.size()) return ranks[k].Size(); else return 0; };
   Dim_t MaxRank() const {  return ranks.size()-1; };

   void Apply(const string &expr, Dim_t d);
 
   void GetDataset(Dim_t d, const string &attrs, Dataset &Result);
   const Dataset &GetDataset(Dim_t d) const { if (d<(Dim_t)ranks.size()) return ranks[d]; else return empty; };
   void Zip(Dim_t d, const Dataset &D);

   void PrintTo(ostream &os, int indent) const;

 private:
   Dataset empty;
   vector<Dataset> ranks;
};
}
#endif /*_RANKEDDATASET_H */
