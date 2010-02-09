#ifndef _NORMNODEMAP_H
#define _NORMNODEMAP_H

#include "unarynodemap.h"
#include "cellarray.h"
#include "assert.h"

namespace GF {
class NormNodeMap : public UnaryNodeMap {

 public: 
  NormNodeMap(AbstractCellArray *zcs) : UnaryNodeMap() { 
    zerocells = zcs;
    Cell *c;
    int x;
    nodemap.resize(zerocells->getsize());
    for (unsigned int i=0; i<zerocells->getsize(); i++) {
      c = zerocells->getCell(i);
      x = c->getnodes()[0];
      nodemap[x] = i;
    }
  };
  virtual Node map(Node x) {
    return nodemap[x];
  };
  
  Node inv(Node o) { 
    Cell *c;
    c = zerocells->getCell(o);
    assert(c->getsize() == 1);
    return c->getnodes()[0];
  };
  
 private:
  hash_map<int, int> nodemap;
  //std::map<int, int> nodemap;
  AbstractCellArray *zerocells;
};
}
#endif /* _CROSSNODEMAP_H */
