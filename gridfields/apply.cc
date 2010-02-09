#include "apply.h"
#include "timing.h"
#include "gridfield.h"
#include "array.h"
#include "tuple.h"
#include "expr.h"
#include <sstream>
#include <math.h>

namespace GF {
/*
ApplyOp::ApplyOp(GridFieldOperator *op, string tupleexpr, Scheme *outscheme) 
   : UnaryGridFieldOperator(op), unparsedExpr(tupleexpr), _sch(outscheme)
{
  this->SaveReservedWords();
  this->cleanup = false;
}
*/

ApplyOp::ApplyOp(string tupleexpr, Dim_t k, GridFieldOperator *op) 
   : UnaryGridFieldOperator(op), k(k), unparsedExpr(tupleexpr)
{
  //this->cleanup = false;
}

void ApplyOp::Execute() {
  
  this->PrepareForExecution();

  //float start = gettime();
  DEBUG << "ApplyOperator" << endl;
  this->Result =  Apply(this->unparsedExpr, this->k,
                        this->GF);
}

GridField *ApplyOp::Apply(string expr, Dim_t k, GridField *Gg) {
  GridField *Out = new GridField(Gg);
  if (Gg->Card(k) == 0) return Out;
  Out->Apply(expr, k); 
  DEBUG << "Apply(" << k << ", " << expr << ")" << endl;
  //Gg->ref();
  return Out;
}
}
