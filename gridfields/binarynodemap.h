#ifndef _BINARYNODEMAP_H
#define _BINARYNODEMAP_H

#include "nodemap.h"

namespace GF {
class BinaryNodeMap {

 public:

  virtual Node map(Node, Node)=0;
  //virtual ~BinaryNodeMap()=0; 
 private:
};
}
#endif /* _BINARYNODEMAP_H */
