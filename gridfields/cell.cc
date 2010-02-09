#include "timing.h"
#include <assert.h>
#include <iterator>
#include <iostream>
#include <ext/algorithm> 
#include "crossnodemap.h"
#include "normnodemap.h"


using namespace GF;
void Cell::print() const {
  print(0);
}

void Cell::setNode(unsigned int i, Node n){ 
  this->nodes[i] = n;
}

void Cell::print(int indent) const {
  const Cell *c = this;
  int i;
  for (i=0; i<indent; i++) printf(" ");
  cout << "<CELL>" << endl;
  for (i=0; i<indent; i++) printf(" ");
  cout << "size: " << c->size << endl;
  for (i=0; i<indent; i++) printf(" ");
  cout << "nodes: ";
  for (i=0; i<c->size; i++) {
    cout << c->nodes[i] << " ";
  }
  cout << endl;
}

Cell::Cell() {
  deletenodes = false;
  size = 0;
  nodes = NULL;
}
/*
Cell::Cell(bool delete) {
  deletenodes = delete;
  size = 0;
  nodes = NULL;
}
*/
Cell::Cell(int s) {
  deletenodes = true;
  size = s;
  nodes = new Node[s];
}

Cell::Cell(int s, Node *ns) {
  deletenodes = false;
  size = s;
  nodes = ns;
}

Cell::Cell(int *ns, int s) {
  deletenodes = true;
  size = s;
  nodes = new Node[s];
  for (int i=0; i<s; i++){
    nodes[i] = ns[i];
  }
}

Cell& Cell::operator=(const Cell& that) {
  Node *newnodes = 0;
  // attempt to allocate the node array
  try {
    newnodes = new Node[that.size];
  } 
  catch (...) {
    // if we fail, delete whatever we have
    delete newnodes;
  }
  // if we own our node array, delete it.
  if (deletenodes) {
    delete [] this->nodes;
  }
  // copy the rhs to lhs
  for (int i=0; i<that.size; i++) {
    newnodes[i] = that.nodes[i];
  } 

  // we own our new node array
  this->deletenodes = true;
  // set our node array to newnodes
  this->nodes = newnodes;
  this->size = that.size;
  // C++ convention is to retirn the lhs
  return *this;
}

Cell::Cell(const Cell &rhs) {
  // we own our nodes
  deletenodes = true;
  // Set the size
  this->size = rhs.size;
  this->nodes = NULL;
  if (size == 0) return;
  this->nodes = new Node[size];
  // copy the node data
  for (int i=0; i<size; i++) {
    this->nodes[i] = rhs.nodes[i];
  }     
}
void Cell::setnodes(Node *p) { 
  this->deletenodes = false; 
  nodes = p; 
}

Cell::~Cell() {
  if (deletenodes) {
    for (int i=0; i<size; i++) {
      //cout << nodes[i] << endl;
    }
    delete [] this->nodes;
  }
}

/*
void Cell::Guess2DEdges(CellArray *onecells) {
  int nodepair[2];
  for (int i=0; i<this->size-1; i++) {
    nodepair[0] = this->nodes[i]; 
    nodepair[1] = this->nodes[i+1]; 
    onecells->addCellNodes(nodepair); 
  }
}
*/


/*
void Cell::Cross(Cell &rhs, CrossNodeMap &h, Cell &out) {
  out.size = size* rhs.size; 
  for (int i=0; i<out.size; i++) {
    out.nodes[i] = h.map(nodes[i%size], rhs.nodes[i/size]);
  }
}
*/

Cell *Cell::Cross(Cell &rhs, CrossNodeMap &h) const {
  Cell *out = new Cell(size * rhs.size);
  for (int i=0; i<out->size; i++) {
    out->nodes[i] = h.map(nodes[i%size], rhs.nodes[i/size]);
//    out->nodes[i] = h.map(nodes[i/rhs.size], rhs.nodes[i%rhs.size]);
  }
  return out;
}
/*
void Cell::Cross21(Cell &rhs, CrossNodeMap &h, int &s, Node *n) const {
  s = size * rhs.size;
  
  for (int i=0; i<size; i++) {
    for (int j=0; j<rhs.size; j++) {
      n[i*rhs.size + j] 
        = h.map(nodes[i], rhs.nodes[j]);
    }
  }
}  
*/
void Cell::Cross2(Cell &rhs, CrossNodeMap &h, int &s, Node *n) const {
  s = size * rhs.size;
  
  if ((size == 2) & (rhs.size == 2)) {
    for (int i=0; i<size; i++) {
      for (int j=0; j<rhs.size; j++) {
        n[i*rhs.size + j] 
          = h.map(nodes[i], rhs.nodes[(1-i%2)*j + (i%2)*(rhs.size-j-1)]);
      }
    }
  } else if ((rhs.size > 2) & (size == 2)) {
    for (int j=0; j<rhs.size; j++) {
      for (int i=0; i<size; i++) {
        n[i*rhs.size + j] = h.map(nodes[i], rhs.nodes[j]);
      }
    }
  } else {
    for (int j=0; j<rhs.size; j++) {
      for (int i=0; i<size; i++) {
        n[j*size + i] = h.map(nodes[i], rhs.nodes[j]);
      }
    }
  }
 /* 
  for (int i=0; i<s; i++) {
//    n[i] = h.map(nodes[i%size], rhs.nodes[i/size]);
    n[i] = h.map(nodes[i/rhs.size], rhs.nodes[i%rhs.size]);
  }
  */
}

void Cell::mapNodes(UnaryNodeMap &h) {
  for (int i=0; i<size; i++) {
    nodes[i] = h.map(nodes[i]);
  }
}

/* Return True if the nodes of the rhs Cell are 
 * a subset of this cell's nodes.  This property is 
 * a proxy for true incidence in this node-oriented 
 * representation.  See dissertation.
 */

bool Cell::IncidentTo(const Cell &rhs) const {
  for (unsigned int i=0; i<rhs.getsize(); i++) {
    if (!this->hasNode(rhs.getnodes()[i])) return false;
  }
  return true;
}

bool Cell::operator==(const Cell &rhs) const {
  if (size != rhs.size) return false;
  for (int i=0; i<size; i++)
    if (nodes[i] != rhs.nodes[i]) return false;
  
  return true;
}

bool Cell::operator<(const Cell &rhs) const {
  if (size < rhs.size) return true;
  if (size > rhs.size) return false;
  //  cout << "cell:\n";
  for (int i=0; i<size; i++) {
    //cout << nodes[i] << ", " <<  rhs.nodes[i] << "\n";
    if (nodes[i] < rhs.nodes[i]) return true;
    if (rhs.nodes[i] < nodes[i]) return false;
  }
  return false;  
}

bool Cell::eq(Cell *c2) {
  for (int i=0; i<this->size; i++) {
    if (this->nodes[i] != c2->nodes[i]) {
      return false;
    }
  }
  return true;
}
