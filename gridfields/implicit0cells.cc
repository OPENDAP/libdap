#include "implicit0cells.h"
#include "implicitcrossnodemap.h"
#include "crossnodemap.h"
#include "normnodemap.h"
#include "timing.h"
#include "gfutil.h"
#include <assert.h>
#include <iterator>
#include <ext/algorithm> 

using namespace GF;
void Implicit0Cells::print() {
  this->print(0);
}

void Implicit0Cells::print(size_t indent) {
  Implicit0Cells *ca = this;
  size_t i;

  for (i=0; i<indent; i++) cout << " ";
  cout << "<CELLARRAY>: \n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "size: " << ca->size << "\n";
  cout << "nodecount: " << ca->getNodeCount() << "\n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "cells: \n";
    cout << "(implicit)" << endl;
}

void Implicit0Cells::toNodeSet(set<Node> &outset) {
  for (size_t i=0; i<this->size; i++) {
    outset.insert(i);
  }
}

Cell Implicit0Cells::getCellCopy(idx i) {
  assert(0 <= i && i < this->getsize());
  Cell c(1);
  c.getnodes()[0] = i;
  return c;
}

Cell *Implicit0Cells::getCell(idx i) { 
    assert(0 <= i && i < this->getsize());
    if (i+1 > this->cleanup.size()) {
      Cell c(1);
      this->cleanup.resize(i+1, c);
    }
    Cell *r = &this->cleanup[i];
    Node *n = r->getnodes();
    n[0] = Node(i);
    return r;
}

void Implicit0Cells::getIncidentCells(const Cell &c, set<CellId> &out) {
  for (int i=0; i<c.getsize(); i++) {
    out.insert(c.getnodes()[i]);
  }
}

void Implicit0Cells::getIncidentCells(Node n, set<CellId> &out) {
    out.insert(n);
}


void Implicit0Cells::mapNodes(NormNodeMap &h) {  }; 
void Implicit0Cells::mapNodes(UnaryNodeMap &h) { 
  //if (h.whoami() != NORMNODEMAP) {
  //  Fatal("Can't map implicit nodes"); 
  //}
}; 
  
vector<Cell> *Implicit0Cells::getCellVector() {
  Node *nodes = new Node[this->size];
  vector<Cell> *cells = new vector<Cell>(this->size, Cell(1));
  vector<Cell>::iterator p;
  
  for (size_t i=0; i<this->size; ++i) {
    cells->operator[](i).setnodes(&nodes[i]);
    nodes[i] = i;
  }
  return cells;
}

CellArray *Implicit0Cells::asCellArray() {
  return new CellArray(*this->getCellVector());
}

CrossNodeMap Implicit0Cells::makeCrossNodeMap(Implicit0Cells *other) {
    return ImplicitCrossNodeMap(this, other);
}

CrossNodeMap Implicit0Cells::makeCrossNodeMap(AbstractCellArray *other) {
  if (other->whoami()) {
    return CrossNodeMap(this, other);
  } else {
    return CrossNodeMap(this, other);
    //return ImplicitCrossNodeMap(this, (Implicit0Cells *) other);
  }
}

AbstractCellArray *Implicit0Cells::Cross(AbstractCellArray *othercells, CrossNodeMap &h) {
  if (othercells->whoami()) {
    Node nodes[this->size];
  
    for (size_t i=0; i<this->size; ++i) {
      nodes[i] = i;
    }
  
    CellArray ca(nodes, this->size, 1);
    CellArray *out = ca.Cross(othercells, h);
    //cout <<"slow implicit cross" << endl;
    return out;
  } else {
    //cout <<"fast implicit cross" << endl;
    return new Implicit0Cells(this->size * ((Implicit0Cells *)othercells)->size);  
  }
}

Implicit0Cells *Implicit0Cells::Cross(Implicit0Cells *othercells, CrossNodeMap &h) {
  return new Implicit0Cells(this->size * othercells->size);  
}


CellArray *Implicit0Cells::Intersection(AbstractCellArray *othercells) {
  CellArray *out = new CellArray();

  Cell *c;
  int max = this->size;
  size_t count = othercells->getsize();
  int node=-1;
  for (size_t i=0; i<count; ++i) {
    c = othercells->getCell(i);
    
    if (c->getsize() != 1) {
      Fatal("Error: Attempt to intersect 0-cells with k-cells, k>0");
    }
    node = c->getnodes()[0];
    if ((node>=0) && (node<max)) {
      out->addCell(c);
    }

  }
  return out;
}

Implicit0Cells *Implicit0Cells::Intersection(Implicit0Cells *others) {
  if (this->size >= others->size) {
    return others;
  } else {
    return this;
  }
}

void Implicit0Cells::unref() {
  int old = this->refcount;
  if (old == 0) cout << "bad refcount" << endl;
  Object::unref();
  //cout << "implicit0cells " << this << ", unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {
    //cout << "deleting implicit0cells" << endl;
    delete this;
  }
}

void Implicit0Cells::ref() {
  //cout << "cellarray ref" << endl;
  Object::ref();
}


