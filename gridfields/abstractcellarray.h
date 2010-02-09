#ifndef _ABSTRACTCELLARRAY_H
#define _ABSTRACTCELLARRAY_H 

#include <vector>
#include <set>
#include "cell.h"
#include "unarynodemap.h"
#include "gfutil.h"
#include "object.h"


using namespace __gnu_cxx;
using namespace std;

namespace GF {
class UnaryNodeMap;
class CrossNodeMap;

class AbstractCellArray : public Object {
 public:
  
  virtual idx getsize()=0;
  virtual ~AbstractCellArray() {};

  virtual int whoami()=0;

  virtual Cell *getCell(idx i)=0;
  virtual Cell getCellCopy(idx i)=0;
  virtual Node *getCellNodes(idx i)=0;

  virtual bool contains(const Cell &c)=0; 
  virtual idx getOrd(const Cell &c)=0;
  virtual idx getOrd(Node n)=0;
  virtual unsigned int getNodeCount()=0;

  virtual void getIncidentCells(Node n, set<CellId> &out)=0;
  virtual void getIncidentCells(const Cell &c, set<CellId> &out)=0;
  virtual void getAdjacentCells(CellId cid, vector<CellId> &out)=0;
  
  virtual void print(size_t indent)=0;
  virtual void print()=0;

  virtual void toNodeSet(set<Node> &outset)=0;
  virtual CrossNodeMap makeCrossNodeMap(AbstractCellArray *other)=0;

  virtual void mapNodes(UnaryNodeMap &h)=0;
  bool implicit() { return false; };
  virtual int bytes()=0;
  
  virtual AbstractCellArray *Intersection(AbstractCellArray *othercells)=0;
  virtual AbstractCellArray *Cross(AbstractCellArray *othercells, CrossNodeMap &h)=0;

  virtual void ref() { };
  virtual void unref() { };

  virtual void buildInvertedIndex()=0;
  virtual vector<Cell> *getCellVector()=0;
};
}
#endif 
