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
  
  Implicit0Cells(unsigned int start, unsigned int stop, unsigned int step) 
            : start(start), stop(stop), step(step), materializednodes(NULL) 
  { this->ref(); }

  Implicit0Cells(unsigned int start, unsigned int stop)
            : start(start), stop(stop), step(1), materializednodes(NULL)
  { this->ref(); }

  Implicit0Cells(unsigned int size)
            : start(0), stop(size), step(1), materializednodes(NULL)
  { this->ref(); }

  ~Implicit0Cells() { 
     this->cleanup.clear(); 
     if (materializednodes != NULL) {
       delete [] materializednodes;
     }
  };

  inline idx getsize() { return (this->stop - this->start) / this->step; };

  Cell *getCell(idx i);
  Cell getCellCopy(idx i);
  Node *getCellNodes(idx i) { 
    // Calling this method effetcively destroys the benefits of the implicit array.
    // CellArray should inherit from Range0Cells, or this method should be removed
    if (materializednodes == NULL) {
      materializednodes = new Node[size];
      for (idx j=0; j<this->stop; j += ) {
        materializednodes[j] = this->start + j * this->step;
      }
    }
    return &materializednodes[i];
  }

  virtual int whoami() { return 0; };

  bool contains(const Node id) {
   // test that id == k * step + start for some k
   return ((id - this->start) % this->step == 0) && id < this->stop;
  };

  bool contains(const Cell &c) { 
   Node id = c.getnodes()[0];
   return this->contains(id);
  }; 

  idx getOrd(const Cell &c) { 
    Node &n = c.getnodes()[0]; 
    return this->getOrd(n);
  };

  idx getOrd(Node n) { 
    return this->contains(n) ? (id - this->start) % this->step : -1; 
  };

  void getIncidentCells(const Cell &c, set<CellId> &out); 
  void getIncidentCells(Node n, set<CellId> &out);
  void getAdjacentCells(CellId, vector<CellId> &) { };
  unsigned int getNodeCount() { return this->getsize(); };

  void mapNodes(NormNodeMap &h); 
  void mapNodes(UnaryNodeMap &h);
  
  void buildInvertedIndex() {};
  void buildAdjacentIndex() {};

  void print(size_t indent);
  void print();

  // This is not always true...how is this used?
  bool implicit() { return true; };
  // This does not seem correct...how is this used?
  int bytes() { return 2*this->getsize()*size*sizeof(int); };
  
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
  unsigned int start;
  unsigned int stop;
  unsigned int step;
  Node *materializednodes;
};

}
#endif /* _CELLARRAY_H */
