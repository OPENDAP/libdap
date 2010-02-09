#ifndef _CELL_H
#define _CELL_H 

//#define __USE_MALLOC
#include "type.h"
#include <cmath>
#include <vector>
//using namespace __gnu_cxx;
using namespace std;

namespace GF {
class UnaryNodeMap;
class CrossNodeMap;

typedef unsigned long CellId;
typedef std::vector<CellId> CellVector;
typedef short Dim_t;

class Cell {
 public:
  Cell();
  //Cell(bool delete) ;
  Cell(int s);
  Cell(int s, Node *ns);
  Cell(int *nodes, int s);
  Cell(const Cell &rhs);
  ~Cell();

  Cell *Cross(Cell &rhs, CrossNodeMap &h) const;
  void Cross2(Cell &rhs, CrossNodeMap &h, int &s, Node *n) const;
  void mapNodes(UnaryNodeMap &h);

  void setNode(unsigned int i, Node n);

  bool eq(Cell *c2);
  bool operator==(const Cell &rhs) const;
  bool operator<(const Cell &rhs) const;
  bool operator[](int &i) const { return nodes[i]; };
  Cell& operator=(const Cell& that);

  void print(int indent) const;
  void print() const;

  bool hasNode(Node n) const {
    for (int j=0; j<size; j++) {
      if (nodes[j] == n) return true;
    }
    return false;
  }

  /* Return True if the nodes of the rhs Cell are 
   * a subset of this cell's nodes.  This property is 
   * a proxy for true incidence in this node-oriented 
   * representation.  See dissertation.
   */
  bool IncidentTo(const Cell &rhs) const;

  void setsize(int sz) { size = sz; }
  void setnodes(Node *p);
  
  int getsize() const {return size; };
  Node *getnodes() const { return nodes; };
  Node getnode(unsigned int i) const { return nodes[i]; };
 
  //void Guess2DEdges(CellArray *onecells); 
 private:
  Node *nodes;
  bool deletenodes;
  int size;
};

struct eqCell {
    bool operator()(const Cell &c1, const Cell &c2) const {
      if (c1.getsize() != c2.getsize()) return false;
      for (int i=0; i<c1.getsize(); i++)
	if (c1.getnodes()[i] != c2.getnodes()[i]) return false;
      
      return true;
    }
};

struct ltCell { 
    bool operator()(const Cell &c1, const Cell &c2) const {
      return (c1 < c2);
    }
};

class SimpleCellHash {
  public:
  int operator()(const Cell &c) const {
    return c.getnodes()[0];
  }
};

class CellHash {
 public:
  int operator()(const Cell &c) const {
    int base = 0;
    for (int i=c.getsize(); i>0; i--) {
      base += c.getnodes()[i-1] * (int) pow(10.0,i-1);
    }
    return base;
  }
};
}
#endif 
