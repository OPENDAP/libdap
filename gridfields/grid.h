#ifndef _GRID_H
#define _GRID_H

#include <vector>
#include <set>
#include <map>
#include <assert.h>
#include "abstractcellarray.h"
#include "cellarray.h"
#include "object.h"
#include "gfutil.h"
#include "ordmap.h"

namespace GF {
class UnaryNodeMap;
//class OrdMap;

string testString(const string &s);
class Grid : public Object {
//class Grid  {

 public:
  string name;

  Grid(string nm, Dim_t d);
  Grid(string nm);
  Grid(string nm, Dim_t d, OrdMap *om);
  void ref();
  void unref();
  ~Grid();

  std::string getName() { return this->name; }
  void init(string nm, Dim_t d, OrdMap *om);

  Dim_t getdim();
  unsigned int Size(Dim_t k) {
    assert(k<(Dim_t)cellsets.size()); 
    return cellsets[k]->getsize();
  };
  void setKCells(AbstractCellArray *cells, Dim_t k);
  AbstractCellArray *getKCells(Dim_t k);
  void setImplicit0Cells(int count);
  unsigned int countKCells(Dim_t k);

  bool empty();

  // Out grid references In's k-cells
  void shareCells(Grid *Out, Dim_t k);
  // Copy In's k-cells to Out, subject to the bitmap filter
  void copyCells(Grid *Out, bool *filter, Dim_t k);
  //void nodeFilter(Grid *Out, vector<Node> &badnodes);
  void nodeFilter(Grid *Out, bool *filter);
  bool checkWellFormed();
  int cellCount(int d);
/*
  void IncidentTo(CellId cid, Dim_t i, _OutputIterator<CellId> out, Dim_t j) {
    AbstractCellArray *ci = this->getKCells(i);
    AbstractCellArray *cj = this->getKCells(j);
    Cell *c = ci->getCell(cid);

    set<CellId> incis;
    cj->getIncidentCells(*c, incis);

    FOR (set<CellId>, x, incis) {
      *out = x;
      ++out;
    }
  }
*/
  void IncidentTo(CellId cid, Dim_t i, vector<CellId> &out, Dim_t j);

  void normalize();
  void mapNodes(UnaryNodeMap &h);
  void setReferent(OrdMap *om);
  
  Grid *Intersection(Grid *Other);
  Grid *Cross(Grid *Other);

  void print(int indent);
  void print();
  friend class GridField;
  OrdMap *ordmap;

 private:
  //int dim;
  vector<AbstractCellArray *> cellsets;

};

class UnitGrid : public Grid {

 public:

  UnitGrid() : Grid("unit", 0) {
    CellArray *nodes = new CellArray();
    Cell *c = new Cell(1);
    c->getnodes()[0] = 0;
    nodes->addCell(c);
    this->setKCells(nodes, 0);
    this->ref();
  };
  
};
/*
class ProductGrid : public Grid {
   string name;

  Grid(Grid *A, Grid *B);
  void ref();
  void unref();
  ~Grid();

  std::string getName() { return this->name; }
 
  Dim_t getdim() { return ;
  unsigned int Size(Dim_t k) {
    assert(k<cellsets.size());
  return cellsets[k]->getsize();
  };
  void setKCells(AbstractCellArray *cells, Dim_t k);
  AbstractCellArray *getKCells(Dim_t k);
  void setImplicit0Cells(int count);
  unsigned int countKCells(Dim_t k);
 
  bool Grid::empty();
 
  // Out grid references In's k-cells
  void shareCells(Grid *Out, Dim_t k);
  // Copy In's k-cells to Out, subject to the bitmap filter
  void copyCells(Grid *Out, bool *filter, Dim_t k);
  //void nodeFilter(Grid *Out, vector<Node> &badnodes);
  void nodeFilter(Grid *Out, bool *filter);
  bool checkWellFormed();
  int cellCount(int d);
 
  void IncidentTo(CellId cid, Dim_t i, vector<CellId> &out, Dim_t j) {
    AbstractCellArray *ci = this->getKCells(i);
    AbstractCellArray *cj = this->getKCells(j);
    Cell *c = ci->getCell(cid);
 
    set<CellId> incis;
    cj->getIncidentCells(*c, incis);
 
    COPY(vector<CellId>, incis, out, ii)
  }
 
  void normalize();
  void mapNodes(UnaryNodeMap &h);
  void setReferent(OrdMap *om);
   
  Grid *Intersection(Grid *Other);
  Grid *Cross(Grid *Other);
 
  void print(int indent);
  void print();
  friend class GridField;
  OrdMap *ordmap;
 
}
*/
}
#endif /* _GRID_H */

