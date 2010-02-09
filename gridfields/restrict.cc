#include "gridfield.h"
#include "array.h"
#include "restrict.h"
#include "apply.h"
#include "expr.h"
#include "timing.h"


using namespace GF;
RestrictOp::RestrictOp(string expr, Dim_t k, GridFieldOperator *op) 
 : UnaryGridFieldOperator(op), k(k), expr(expr) 
{
}

void RestrictOp::Execute() {
  this->PrepareForExecution();
  this->Result = Restrict(this->expr, this->k, this->GF);
  /*
  if (Result->isAttribute("salt") && Result->card() > 0)  {
    cout << "size: " << Result->card() << endl;
    //getchar();
  }
  */
}

GridField *RestrictOp::Restrict(string expr, Dim_t k, GridField *GF) {

  GridField *Out;
  Grid *G = GF->GetGrid();
  string name = "r" + G->name;
  Grid *outgrid;

  if (k>GF->Dim()) {
    // if k>dim, then the result is the empty grid
    outgrid = new Grid(name, -1);
  } else {
  
    GF->Apply("mask="+expr, k);
  

    int n = GF->Size(k);

    bool kcellFilter[n];
 
    outgrid = new Grid(name, G->getdim());
  
    // build the subgrid
    Dataset::FloatIterator fi;
    Dataset::FloatIterator vstop = GF->EndFloat(k, "mask");
    int i=0;
    for (fi=GF->BeginFloat(k, "mask"); fi!=vstop; ++fi) {
      kcellFilter[i++] = (bool) (*fi);
    }
    if (k==0) {
      GF->grid->nodeFilter(outgrid, kcellFilter);
    } else {
      for (i=0; i <= GF->grid->getdim(); i++) {
        if (i==k){
          GF->grid->copyCells(outgrid, kcellFilter, i);
        } else {
          GF->grid->shareCells(outgrid, i);
        }
      }
    }
  }
  
  //now filter the data
  Out = new GridField(outgrid);  
  outgrid->unref();
  GF->RemoveAttribute(k, "mask");
  Out->RestrictAll(*GF);

  DEBUG << "Restrict(" << expr << ")" << endl;
  return Out;
}

