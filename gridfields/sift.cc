#include "sift.h"
#include "timing.h"


using namespace GF;
SiftOp::SiftOp(Dim_t k, GridFieldOperator *prev) 
  : UnaryGridFieldOperator(prev), _k(k)
{
  // sift does not create a new gridfield, so 
  // do not delete anything on destruction
  //this->cleanup = false;
}
    
void SiftOp::Execute() {
  this->PrepareForExecution();
  Result = this->Sift(this->_k, this->GF);
}

string SiftOp::newName(string gfname) {

  string gname = "sift(" + gfname + ")";
  return gname;
}

GridField *SiftOp::Sift(Dim_t k, GridField *GF) {

  Grid *G = new Grid(GF->GetGrid()->name);
  AbstractCellArray *kcells = GF->GetGrid()->getKCells(k);
  G->setKCells(kcells, k);
  kcells->ref();

  if (k!=0) {

    CellArray *newnodes = new CellArray();

    set<Node> uniquenodes;

    Cell *c;
    for (size_t i=0; i<kcells->getsize(); i++) {
      c = kcells->getCell(i);
      const int n = c->getsize();
      const Node *ns = c->getnodes();
      for (int j=0; j<n; j++) {
        uniquenodes.insert(ns[j]);
      }
    }
    FOR(set<Node>, x, uniquenodes) {
      Node c = *x;
      newnodes->addCellNodes(&c, 1);
    }
    G->setKCells(newnodes, 0);
  }

  GridField *Out = new GridField(G);
  Out->RestrictAll(GF);
  return Out;
}

