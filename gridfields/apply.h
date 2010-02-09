#ifndef _APPLY_H
#define _APPLY_H

#include <iostream>
#include "gridfield.h"
#include "gridfieldoperator.h"
#include <ext/functional>
#include <string>

namespace GF {
class ApplyOp : public UnaryGridFieldOperator {
 public:
//  ApplyOp(GridFieldOperator *op, string tupleexpr, Scheme *outscheme);
  ApplyOp(string tupleexpr, Dim_t k, GridFieldOperator *op);
  void Execute();
  static GridField *Apply(string tupleexpr, Dim_t k, GridField *Gg);

 void SetExpression(const string &expr) { 
    unparsedExpr = expr; Update(); 
 };
 void SetRank(Dim_t _k) { k=_k; Update();};
  
 private:
  Dim_t k;
  string unparsedExpr;

};
}
#endif /* APPLY_H */
