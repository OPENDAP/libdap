#ifndef _RESTRICTOP_H
#define _RESTRICTOP_H

#include "expr.h"
#include "gridfieldoperator.h"

namespace GF {
class RestrictOp : public UnaryGridFieldOperator {
public:
  RestrictOp(string expr, Dim_t k, GridFieldOperator *op);
  
  void Execute();
  
  static GridField *Restrict(string expr, Dim_t k, GridField *GF);
protected:
  Dim_t k;
  string expr;
private:
 
};
}
#endif
