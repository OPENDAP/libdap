#include <string>
#include "gridfield.h"
#include "gridfieldoperator.h"

namespace GF {
class SiftOp : public UnaryGridFieldOperator {
 public:
  SiftOp(Dim_t k, GridFieldOperator *prev);
  void Execute(); 
  static GridField *Sift(Dim_t k, GridField *Gf);
 private:
  Dim_t _k;
  static string newName(string gfname);

};
}
