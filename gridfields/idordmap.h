#ifndef _IDORDMAP_H
#define _IDORDMAP_H

#include "ordmap.h"

namespace GF {
class IdOrdMap : public OrdMap {

 public: 
  IdOrdMap(Grid *g) { G = g; };
  virtual ~IdOrdMap() {};
  virtual int getBaseOrd(Cell *c, int d) { 
    AbstractCellArray *cells = G->getKCells(d);
    int pos = cells->getOrd(*c);
    return pos; 
  };
  virtual int getBaseSize(int d) { return G->cellCount(d); };
  
 private:
  Grid *G;
};
}
#endif /* _IDORDMAP_H */
