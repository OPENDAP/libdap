#ifndef _GRIDFIELD_H
#define _GRIDFIELD_H

#include <iostream>
#include <vector>
#include "grid.h"
#include "gridfieldoperator.h"
#include "cellarray.h"
#include "rankeddataset.h"
#include "tuple.h"

namespace GF {
class Array;

class GridField : public RankedDataset, public GridFieldOperator {

 public:
  GridField(Grid *G);
  GridField(Grid *G, Dim_t k, Array *arr);
  GridField(GridField *G);
  ~GridField();

  //void Bind(vector<Tuple *> &data);

  void setGrid(Grid *G); 

  //void subGridData(Grid *A, vector<Array *> &outvec); 
  void nearest(const string &attr, Dim_t k, UnTypedPtr p, vector<CellId> &out);

  void lookupInt(string attr, Dim_t k, int p, vector<CellId> &out);
  void lookupFloat(string attr, Dim_t k, float p, vector<CellId> &out);

  void Clear() { RankedDataset::Clear(); grid->unref(); }

  Cell *getKCell(Dim_t k, CellId c) {
    return this->grid->getKCells(k)->getCell(c);
  };

  void unref();
  int notValid();
  void RestrictAll(const GridField &Base);
  void RangeRestrict(Dim_t k, const GridField &Base);
  void print();
  void print(int a);
  void PrintTo(ostream &os, int indent) const; 

  // gridfields are a trivial subclass of gridfieldoperator
  void PrepareForExecution();
  bool Updated(float sincetime);
  void Execute();

  Dim_t Dim() const { return this->grid->getdim(); };
  size_t Card(Dim_t k) const { return this->grid->Size(k); };
  
  Grid *GetGrid() { return grid; };
  friend class RestrictOp;

  void ref() {
    //cout << "ref: " << this << "(" << old << ")" << endl;
    Object::ref();
  }

 private:
  Grid *grid;
  void init(Grid *G);

};


class UnitGridField : public GridField {
  public: 
    UnitGridField();
    void Bind(const string &name, int val);
    void Bind(const string &name, float val);
    void Bind(const string &name, UnTypedPtr val);
};
}
#endif /*_GRIDFIELD_H */
