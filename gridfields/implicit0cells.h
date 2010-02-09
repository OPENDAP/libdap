#ifndef _IMPLICIT0CELLS_H
#define _IMPLICIT0CELLS_H 

#include <vector>
#include <set>
#include <map>
#include <ext/hash_map>
#include <iostream>
#include "cellarray.h"
#include "cell.h"
#include "normnodemap.h"
#include "expr.h"

#define BLOCKSIZ 100
#define DIGITS 6

namespace GF {

class Implicit0Cells : public AbstractCellArray {
 public:
  
  Implicit0Cells(int s) : size(s), materializednodes(NULL) { this->ref(); }
  ~Implicit0Cells() { 
     this->cleanup.clear(); 
     if (materializednodes != NULL) {
       delete [] materializednodes;
     }
  };
  idx getsize() { return this->size; };

  Cell *getCell(idx i);
  Cell getCellCopy(idx i);
  Node *getCellNodes(idx i) { 
    // Calling this method effetcively destroys the benefits of the implicit array.
    // CellArray should inherit from Implicit0Cells, or this method should be removed  
    // altogether
    if (materializednodes == NULL) {
      materializednodes = new Node[this->size];
      for (idx j=0; j<this->size; j++) {
        materializednodes[j] = j;
      }
    }
    return &materializednodes[i];
  }

  virtual int whoami() { return 0; };

  bool contains(const Cell &c) { return (size > c.getnodes()[0]); }; 
  idx getOrd(const Cell &c) { Node &n = c.getnodes()[0]; return n < this->size ? n : -1; }
  idx getOrd(Node n) { return n < this->size ? n : -1; }

  void getIncidentCells(const Cell &c, set<CellId> &out); 
  void getIncidentCells(Node n, set<CellId> &out);
  void getAdjacentCells(CellId, vector<CellId> &) { };
  unsigned int getNodeCount() { return size; };

  void mapNodes(NormNodeMap &h); 
  void mapNodes(UnaryNodeMap &h);
  
  void buildInvertedIndex() {};
  void buildAdjacentIndex() {};

  void print(size_t indent);
  void print();
  bool implicit() { return true; };
  int bytes() { return 2*size*sizeof(int); };
  
  void unref();
  void ref();

  CellArray *asCellArray(); 
  vector<Cell> *getCellVector();
  void toNodeSet(set<Node> &outset);

  //CellArray *nodeFilter(vector<Node> nodes);
  CellArray *Intersection(AbstractCellArray *othercells);
  Implicit0Cells *Intersection(Implicit0Cells *othercells);
  AbstractCellArray *Cross(AbstractCellArray *othercells, CrossNodeMap &h);
  Implicit0Cells *Cross(Implicit0Cells *othercells, CrossNodeMap &h);

  CrossNodeMap makeCrossNodeMap(AbstractCellArray *other);
  CrossNodeMap makeCrossNodeMap(Implicit0Cells *other);


 private:
  vector<Cell> cleanup;
  set<CellId> incidences;
  unsigned int size;
  Node *materializednodes;
};

}
#endif /* _IMPLICIT0CELLS_H */
