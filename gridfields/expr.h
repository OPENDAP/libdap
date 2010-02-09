#ifndef _EXPR_H
#define _EXPR_H

#include "type.h"
#include "gridfield.h"
#include "fparser.hh"
#include <ext/hash_map>

namespace GF {
class TupleFunction {
  public:
    TupleFunction();
    ~TupleFunction();
    void Parse(string tupleexpr);
    Scheme *ReturnType() { return &outScheme; };
    Scheme *InputType() { return &inScheme; };
    void Eval(Tuple &input, Tuple &output);
  private:
    void SaveReservedWords();
    void BindVars(Tuple &intup, double *bindings);
    void getVars(string expr, vector<string> &returnval);
    void deleteFunctions();
    set<string> reservedWords;
  protected:
    map<string, FunctionParser *> functions;
    double *bindings;
    Scheme inScheme;
    Scheme outScheme;
};

class SpecializedTupleFunction : public TupleFunction {
  public:
    void SpecializeFor(Scheme &out);
    void Eval(Tuple &in, Tuple &out);
  private:
    int in_tup_size;
    hash_map<int, pair<int, Type> > in_position_map;
    hash_map<int, pair<int, FunctionParser *> > out_position_map;
};
}
#endif /* _EXPR_H */
    

