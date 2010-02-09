#ifndef _CELLARRAY_H
#define _CELLARRAY_H 

#include <vector>
#include <set>
#include <map>
#include <ext/hash_map>
#include <iostream>

#include "cell.h"
#include "abstractcellarray.h"


namespace GF {
class CellArray : public AbstractCellArray {
 public:
  typedef map<Cell, int, ltCell> SortedCellIndex;
  typedef hash_map<Cell, int, SimpleCellHash> InvertedCellIndex;
  //typedef map<Cell, int> InvertedCellIndex;
  typedef hash_map<Node, set<CellId> > IncidenceIndex;
  
  CellArray() : cleanup_node_array(false), 
                nodecount(0),
                node_array(NULL), 
                UseAdjacencyIndex(false)  
  { this->ref(); };
  
  CellArray(std::vector<Cell> vec) : cleanup_node_array(false), 
                                     cells(vec),
                                     node_array(NULL), 
                                     UseAdjacencyIndex(false)
  { this->ref(); };
  
  virtual int whoami() { return 1; };
  CellArray(Node *cells, int cellcount, int nodespercell);
  CellArray(Node *cells, int cellcount);
  ~CellArray();
  idx getsize();
  void addCell(Cell &c);
  void addCell(Cell *c);
  Cell *addCellNodes(Node *nodes, int size);

  Cell *getCell(idx i);
  Cell getCellCopy(idx i);
  Node *getCellNodes(idx i);

  //void Edges(CellArray *onecells);

  bool contains(const Cell &c); 
  idx getOrd(const Cell &c);
  idx getOrd(Node n);
  int bytes();
  void ref();
  void unref();

  void setNodeArray(Node *na, unsigned int ns);
  void getIncidentCells(Node n, set<CellId> &out);
  void getIncidentCells(const Cell &c, set<CellId> &out);
  void getAdjacentCells(CellId c, vector<CellId> &out);
  unsigned int getNodeCount() { return this->nodecount; };
  
  void print(size_t indent);
  void print();

  void toNodeSet(set<Node> &outset);

  //CellArray *nodeFilter(vector<Node> nodes);
  CellArray *Intersection(AbstractCellArray *othercells);
  CellArray *Cross(AbstractCellArray *othercells, CrossNodeMap &h);
  void Append(AbstractCellArray *othercells);

  void mapNodes(UnaryNodeMap &h);
  CrossNodeMap makeCrossNodeMap(AbstractCellArray *other);

  void buildInvertedIndex();
  void buildIncidenceIndex();
  void buildAdjacencyIndex();

  vector<Cell> *getCellVector() { return &cells; }
  
  bool cleanup_node_array;
 private:
  vector<Cell> cells;
  unsigned int nodecount;
  Node *node_array;
  //hash_map<Cell, int, CellHash, eqCell>  hashed_cells;
  InvertedCellIndex inverted_cells;
  IncidenceIndex incidence;
  vector<vector<CellId> > adj;
  bool UseAdjacencyIndex;
};
}
#endif /* _CELLARRAY_H */
