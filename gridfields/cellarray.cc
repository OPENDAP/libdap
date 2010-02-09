#include "timing.h"
#include "expr.h"
#include <assert.h>
#include <iterator>
#include <ext/algorithm> 
#include "cellarray.h"
#include "implicitcrossnodemap.h"
#include "crossnodemap.h"
#include "normnodemap.h"


using namespace GF;
CellArray::CellArray(Node *celldata, int cellcount) :
            cells(cellcount, Cell(0)),
			      cleanup_node_array(true), 
            nodecount(0),
			      node_array(NULL) {

  this->node_array = celldata;
  Node *ptr = celldata;                            
  for(int i=0; i<cellcount; i++) {
    Cell &c = cells[i];
    c.setsize(*ptr);
    this->nodecount += *ptr;
    ++ptr;
    c.setnodes(ptr);
    ptr += c.getsize();
  }
  this->ref();
}
  
CellArray::CellArray(Node *celldata, int cellcount, int nodespercell) :
                              cells(cellcount, Cell(0)), 
			      cleanup_node_array(true), 
            nodecount(0),
			      node_array(NULL) {
                  
  this->node_array = celldata;
  Node *ptr = celldata;                            
  for(int i=0; i<cellcount; i++) {
    Cell &c = cells[i];
    c.setsize(nodespercell);
    c.setnodes(ptr);
    ptr += nodespercell;
  }
  this->nodecount = cellcount*nodespercell;
  this->ref();
}

CellArray::~CellArray() {
  if (this->cleanup_node_array) {
    DEBUG << "DELETING node array" << endl;
    delete [] node_array;
  }
}

idx CellArray::getsize()  {
  return this->cells.size();
}

void CellArray::addCell(Cell &c) {
  cells.push_back(c);
  nodecount += c.getsize();
}

void CellArray::addCell(Cell *c) {
  //inverted_cells[*c] = cells.size();
  cells.push_back(*c);
  nodecount += c->getsize();
}

void CellArray::setNodeArray(Node *na, unsigned int ns) {
  this->node_array = na;
  this->nodecount = ns;
  this->cleanup_node_array = true;
}

Cell *CellArray::addCellNodes(Node *ns, int s) {
  //Cell *c = new Cell(s);
  Cell c(s);
  for (int i=0; i<s; i++) {
   c.getnodes()[i] = ns[i];
  }
  addCell(&c);
  return &cells.back();
}

void CellArray::buildInvertedIndex(){
  inverted_cells.clear();
  //inverted_cells.resize(cells.size());
      
  for (size_t i=0; i< cells.size(); i++) {
    inverted_cells[cells[i]] = i;
  }
  //cout << "SIZE: " << inverted_cells.size() << ", BUCKETS: " << inverted_cells.bucket_count() << endl;
}

void CellArray::buildAdjacencyIndex() {
  // build adjacency index
  unsigned int n = this->getsize();

  adj.clear();
  adj.resize(n);

  for (size_t i=0; i<n; i++) {
    this->getAdjacentCells(i, adj[i]);
  }
  this->UseAdjacencyIndex = true;
}



void CellArray::getAdjacentCells(CellId cid, vector<CellId> &out) {

  if (this->UseAdjacencyIndex) {
    COPY(vector<CellId>, adj[cid], out, ii);
  }
  
  AbstractCellArray *ca = this;
  set<CellId> cellset;
  Cell &c = *(this->getCell(cid));
  /*
  if (d == 0) {
    for (int i=0; i<c.getsize(); i++) {
      //cout << "get Incident cells using " << this->getdim() << endl;
      ca->getIncidentCells(c.getnodes()[i], cellset);
      for (p=cellset.begin(); p!=cellset.end(); p++) {
        Cell *ic = ca->getCell(*p);
        for (int j=0; j<ic->getsize(); j++) {
          out.insert(ic->getnodes()[j]); 
        }
      }
    }    
    out.erase(cid);
    //cout << "Node: " << c.getnodes()[0] << " <| " << out.size() << endl;
  }
  else {
    */
    for (int i=0; i<c.getsize(); i++) {
      ca->getIncidentCells(c.getnodes()[i], cellset);
    }
    cellset.erase(cid);
    out.insert(out.end(), cellset.begin(), cellset.end());
  //}
}
void CellArray::getIncidentCells(const Cell &c, set<CellId> &out) {
  if (incidence.empty()) buildIncidenceIndex();
/*
  c.print();
  cout 
  << c.getnodes()[0]
  << ": "
  << incidence[c.getnodes()[0]].size()
  << "->"
  << *(incidence[c.getnodes()[0]].begin())
  << ":"
  << *(incidence[c.getnodes()[0]].end())
  << endl;
*/
  for (int i=0; i<c.getsize(); i++) {
    out.insert(incidence[c.getnodes()[i]].begin(), incidence[c.getnodes()[i]].end());
  }
  FOR (set<CellId>, x, out) { 
    if (!c.IncidentTo(*this->getCell(*x))) {
      out.erase(*x);
    }
  }
}

void CellArray::getIncidentCells(Node n, set<CellId> &out) {
  if (incidence.empty()) buildIncidenceIndex();
  out.insert(incidence[n].begin(), incidence[n].end());
}

void CellArray::buildIncidenceIndex(){
  vector<Cell>::iterator p;
  set<Node> nodeset; 
  toNodeSet(nodeset);
  incidence.clear();
  incidence.resize(nodeset.size());
  int i = 0;
  for (p=cells.begin(); p!=cells.end(); ++p) {
    for (int j=0; j<(*p).getsize(); j++) {
      incidence[(*p).getnodes()[j]].insert(i);
    }
    i++;
  }
}

size_t CellArray::getOrd(Node n) {
  //convenient specialization for 0-cells
  const Cell c(1); 
  c.getnodes()[0] = n;
  return getOrd(c);
}

inline bool CellArray::contains(const Cell &c) {
  return (inverted_cells.find(c) != inverted_cells.end());
}
/*
inline int CellArray::fastOrd(const Cell &c) {
  //assumes inverted index is fresh
  InvertedCellIndex::iterator pos;
  pos = inverted_cells.find(c);
  if (pos == inverted_cells.end()) {
    return -1;
  }
  return pos->second; 
}
*/
size_t CellArray::getOrd(const Cell &c) {
  InvertedCellIndex::iterator pos;
  if (inverted_cells.empty()) {
    this->buildInvertedIndex();
  }
  pos = inverted_cells.find(c);
  
  if (pos == inverted_cells.end()) {
    //rebuild the index and check again
    this->buildInvertedIndex(); 
    pos = inverted_cells.find(c);
    if (pos == inverted_cells.end()) {
      return -1;
    }
  }
  return (*pos).second;
}

Cell CellArray::getCellCopy(idx i) {
  assert(0 <= i && i < this->cells.size());
  return this->cells[i];
}

Cell *CellArray::getCell(idx i) {
  assert(0 <= i && i < this->cells.size());
  return &this->cells[i];
}

Node *CellArray::getCellNodes(idx i) {
  assert(0 <= i && i < this->cells.size());
  return this->cells[i].getnodes();
}

void CellArray::print() {
  this->print(0);
}

int CellArray::bytes() {
  int b = 0;
  for (size_t i=0; i< this->getsize(); i++) {
    b += 1 + this->getCell(i)->getsize();
  }
  return b*sizeof(int);
}

void CellArray::print(size_t indent) {
  CellArray *ca = this;
  size_t i;

  for (i=0; i<indent; i++) cout << " ";
  cout << "<CELLARRAY>: \n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "size: " << ca->cells.size() << "\n";
  cout << "nodecount: " << ca->getNodeCount() << "\n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "cells: \n";
  for (i=0; i<ca->cells.size(); i++) {
    ca->cells[i].print(indent+2);
  }
}
/*
void CellArray::Edges(CellArray *onecells) {
  int nodepair[2];
  
  for (int i=0; i<this->getsize(); i++) {
    Cell *c = this->getCell(i);
    c->Guess2DEdges(onecells);
  }  
}
*/
CellArray *CellArray::Intersection(AbstractCellArray *othercells) {
  typedef SortedCellIndex::iterator Ptr;
//  typedef InvertedCellIndex::iterator Ptr;
  Ptr me, mylast;
  Ptr you, yourlast;
  CellArray *out = new CellArray();
  vector<Cell> *ours = out->getCellVector();

  SortedCellIndex sortedcells;
  SortedCellIndex othersortedcells;
  
  for (size_t i=0; i< cells.size(); i++) {
    sortedcells[cells[i]] = i;
  }
  for (size_t i=0; i< othercells->getsize(); i++) {
    othersortedcells[*(othercells->getCell(i))] = i;
  }

  me = sortedcells.begin();
  mylast = sortedcells.end();
  you = othersortedcells.begin();
  yourlast = othersortedcells.end();

  while (me != mylast && you != yourlast) {
    
     //((Cell)(*me).first).print();
    //  ((Cell)(*you).first).print();
    if ((*me).first < (*you).first) { 
      ++me;
      continue;
    }
    if ((*you).first < (*me).first) { 
      ++you;
      continue;
    }
    if ((*me).first == (*you).first) {
      ours->push_back((*me).first);
      //cout << "match!" << endl;
      ++me;
      ++you;
    } else { 
      ((Cell)(*me).first).print();
      ((Cell)(*you).first).print();
      
      Fatal("Error computing intersection."); 
    }
  }
    
  //out->buildInvertedIndex();

  return out;
}

CellArray *CellArray::Cross(AbstractCellArray *othercells, CrossNodeMap &h) {
  CellArray *out = new CellArray();

  int mysize = this->getsize();
  int yoursize = othercells->getsize();
  

  int s=0;
  //cout << this->getNodeCount()  << ", " <<  othercells->getNodeCount() << ": ";
  unsigned int outnodes = this->getNodeCount() * othercells->getNodeCount();
  Node *raw = new Node[outnodes];

  out->setNodeArray(raw, outnodes);
  
  int i, j;
  int nextnode = 0;
  int nextsize = 0;

  float tpost = gettime();
  vector<Cell> &outies = *out->getCellVector();
  //outies.resize(mysize * yoursize);
  //outies.clear();
  outies.reserve(mysize*yoursize);
//  Cell c;
//  Cell *cs = (Cell *) malloc(mysize * yoursize * sizeof(Cell));
//  uninitialized_fill(outies.insert(outies.end(), mysize * yoursize, Cell());
  outies.insert(outies.end(), mysize * yoursize, Cell());
  //cout << "resize: " << gettime() - tpost << "\n";

  vector<Cell> &yours = *othercells->getCellVector();
  Cell x;
  tpost = gettime();
  
  for (i=0; i<mysize; i++) {  
    for (j=0; j<yoursize; j++) {
      //      c1 = &cells[i];
      //      c2 = &yours[j];
      nextsize = i*(yoursize) + j;
      
      this->cells[i].Cross2(yours[j], h, s, &raw[nextnode]);
      
      Cell &x = outies[nextsize];
      x.setsize(s);
      x.setnodes((&(raw[nextnode])));
      //outies[nextsize].nodes = &raw[nextnode];
      //outies[nextsize].deletenodes = false;

//      outies[nextsize].print();
      //outies.push_back(x);
      nextnode += s;
    }
  }
  

  /*
  vector<Cell> &o  = out->cells;
  vector<Cell>::iterator p;
  int ci=0;
  for (p=o.begin(); p!=o.end(); ++p) {
    (*p) = outies[ci++];
  }
  */
  //  cout << "making cells...\n";
  /*
  nextnode = 0;
  for (int i=0; i<mysize * yoursize; i++) {
    out->cells[i].size = s[i];
    //    cout << "s[i] = " << s[i] << "\n";
    out->cells[i].setnodes(&raw[nextnode]);
    //    cout << "nextnode = " << nextnode << "\n";
    //    cout << "raw[nextnode] = " << raw[nextnode] << "\n";
    nextnode += s[i];
  }
  */
  return out;
}

/*
CellArray *CellArray::nodeFilter(vector<Node> nodes) {
  if (this->incidence.empty()) this->buildIncidenceIndex();

  vector<Node>::iterator i;
  IncidenceIndex::iterator cs;
  set<Cell> *cellsAsSet = new set<Cell>;
  set<Cell> *subtrahend;
  set<Cell> *temp = new set<Cell>;

  copy(cells.begin(), cells.end(), inserter(*cellsAsSet, (*cellsAsSet).begin()));
//  cellsAsSet.insert(cells.begin(), cells.end());
  
  
  for (i=nodes.begin(); i!=nodes.end(); i++) {
    cs = incidence.find((*i));
    subtrahend = &((*cs).second);
    set_difference(cellsAsSet->begin(), cellsAsSet->end(),
                   subtrahend->begin(), subtrahend->end(),
                   inserter(*temp,(*temp).begin()));
    
    subtrahend->clear();
    subtrahend = cellsAsSet;
    cellsAsSet = temp;
    temp = subtrahend;
    temp->clear();
    
//  cellsAsSet.erase((*cs).second.begin(), (*cs).second.end());
  }
  
  CellArray *out =  new CellArray();
  insert_iterator<vector<Cell> > ii(out->cells, out->cells.begin());
  copy(cellsAsSet->begin(), cellsAsSet->end(), ii);
  
}
*/
void CellArray::Append(AbstractCellArray *othercells) {
  vector<Cell> *others = othercells->getCellVector();
  cells.reserve(others->size());
  cells.insert(cells.end(), others->begin(), others->end());
  this->nodecount += othercells->getNodeCount();
}

void CellArray::mapNodes(UnaryNodeMap &h) {
  //if (inverted_cells.empty()) this->buildInvertedIndex();
  for (size_t i=0; i<getsize(); i++) {
    getCell(i)->mapNodes(h);
  }
}

CrossNodeMap CellArray::makeCrossNodeMap(AbstractCellArray *other) {
  return CrossNodeMap(this, other);
}

void CellArray::toNodeSet(set<Node> &outset) {
  
  Cell *c;
  Node *cn;

  int n = cells.size();

  for (int i=0; i<n; i++) {
    c = getCell(i);  
    cn = getCellNodes(i);
    for (int j=0; j<c->getsize(); j++)
      outset.insert(cn[j]);
  }
}

int ca_instances=0;

void CellArray::ref() {
  //cout << "--" << ca_instances++ << endl;
//  cout << "cellarray ref" << endl;
  Object::ref();
}

void CellArray::unref() {
  ca_instances--;
  int old = refcount;
  Object::unref();
  DEBUG << "cellarray " << this << ", unref: " << old << " -> " << this->refcount << endl;
  if (old == 0) cout << "bad refcount" << endl;
  //cout << "cellarray " << this << ", unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {
    //cout << "deleting cellarray" << endl;
    delete this;
  }
}
