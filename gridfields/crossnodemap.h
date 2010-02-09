#ifndef _CROSSNODEMAP_H
#define _CROSSNODEMAP_H

#include "binarynodemap.h"
#include "abstractcellarray.h"

namespace GF {
class CrossNodeMap : public BinaryNodeMap {

 public: 
  CrossNodeMap() {};
  CrossNodeMap(AbstractCellArray *An, AbstractCellArray *Bn) : BinaryNodeMap() { 
    Anodes = An;
    Bnodes = Bn;
  };
  inline virtual Node map(Node a, Node b) { 
    return Anodes->getOrd(a) * Bnodes->getsize() + Bnodes->getOrd(b) ;
    //return a * Bnodes->getsize() + b ;
  };
 
  
  void setInputs(AbstractCellArray *An, AbstractCellArray *Bn) {
    Anodes = An;
    Bnodes = Bn;
  }

  Node inv_b(Node o, Node a) { 
    Cell *c;
    int i = o % Bnodes->getsize();
    c = Bnodes->getCell(i);
    return c->getnodes()[0];
  };
  
  Node inv_a(Node o, Node b) { 
    Cell *c;
    int i = o / Bnodes->getsize();
    c = Bnodes->getCell(i);
    return c->getnodes()[0];
  };
  
 private:
  AbstractCellArray *Anodes;
  AbstractCellArray *Bnodes;
};
}
#endif /* _CROSSNODEMAP_H */
