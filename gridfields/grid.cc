#include <iostream>
#include <assert.h>
#include "grid.h"
#include "unarynodemap.h"
#include "crossnodemap.h"
#include "normnodemap.h"
#include "crossordmap.h"
#include "idordmap.h"
#include "timing.h"
#include "cellarray.h"
#include "implicit0cells.h"
#include "gfutil.h"


#include <string>

using namespace GF;
string testString(const string &s)  { return s; }
Grid::Grid(string nm, Dim_t d) {
  init(nm, d, new IdOrdMap(this));
}

Grid::Grid(string nm) {
  init(nm, -1, new IdOrdMap(this));
}

Grid::Grid(string nm, Dim_t d, OrdMap *om) {
  init(nm, d, om);
}

int g_instances=0;

void Grid::init(string nm, Dim_t d, OrdMap *om) {
// cout << g_instances++ << endl;
   
  //dim = d;
  name = nm;

  ordmap = om;

  for (int i=0; i<=d; i++) {
    cellsets.push_back(new CellArray());
  }

  //increment refcount
  this->ref();
}
Dim_t Grid::getdim() {
  int i;
  for (i=cellsets.size()-1; i>=0; i--) {
    if (cellsets[i]->getsize() > 0) break;
  }
  return i;
}

bool Grid::empty() {
  for (int i=0; i<=this->getdim(); i++) {
    if (cellsets[i]->getsize() > 0) {
      return false;
    }
  }
  return true;
}

unsigned int Grid::countKCells(Dim_t k) {
  if (k>getdim()) return 0;
  return cellsets[k]->getsize();
}

AbstractCellArray *Grid::getKCells(Dim_t k) {
  if (k >= (Dim_t) cellsets.size()) {
    for (int i=cellsets.size(); i<=k; i++) {
      cellsets.push_back(new CellArray());
    }
  }
  return cellsets[k];
}

void Grid::setKCells(AbstractCellArray *cells, Dim_t d) {

  int s = cellsets.size();
  if (d >= s) {
    cellsets.resize(d+1);
    for (int i=s; i<d; i++) {
      cellsets[i] = new CellArray(); 
    }
  } else {
    cellsets[d]->unref();
  }
  //cout << cellsets.size() << endl;
  cellsets[d] = cells; 
}

Grid *Grid::Intersection(Grid *Other){
  string outname = "(" + name + "-i-" + Other->name + ")";
  
  Grid *Out = new Grid(outname);
  AbstractCellArray *caOut, *ca1, *ca2;
  Dim_t d = MIN(Other->getdim(), getdim());

  for (int i=d; i>=0; i--){
    ca1 = this->getKCells(i);
    ca2 = Other->getKCells(i);
    caOut = ca2->Intersection(ca1);

    if (caOut->getsize() != 0)
      Out->setKCells(caOut, i);
  }
  return Out;
}

Grid *Grid::Cross(Grid *Other) {
  string outname = "(" + name + "-c-" + Other->name + ")";
  //string outname = "cross";
  Grid *Out = new Grid(outname);
  AbstractCellArray *ca1, *ca2;
  float tpost;

  Dim_t d = getdim() + Other->getdim();

  AbstractCellArray *myNodes = this->getKCells(0);
  AbstractCellArray *yourNodes = Other->getKCells(0);
  CrossNodeMap h = myNodes->makeCrossNodeMap(yourNodes);

  tpost = gettime();
  AbstractCellArray *newnodes;
  ca1 = this->getKCells(0);
  ca2 = Other->getKCells(0);

  Dim_t SIFTDIM = d;
  
  //cout << "crossing 0-cells" << endl;
  newnodes = ca1->Cross(ca2, h); 
  //    cout << gettime() - tpost << " ( Cross(0-cells) ) " << endl;

  //tpost = gettime();
  //    cout << gettime() - tpost << " ( Append(0-cells) ) " << endl;

//    tpost = gettime();
//    caOut->buildInvertedIndex();
//    cout << gettime() - tpost << " ( BuildIndex(0-cells) ) " << endl;

  Out->setKCells( newnodes, 0 );

  //CellArray *kcellsout = new CellArray();

  //tpost = gettime();
  //ca1 = this->getKCells(getdim());
  //ca2 = Other->getKCells(Other->getdim());
  //cout << "crossing d-cells" << endl;
  //c = ca1->Cross( ca2, h ); 
  //cout << this->name << ": " << this->getdim() << ", " << Other->name << ", " << Other->getdim() << endl;
  //if (Other->getdim() == 0) {
    //Other->print();
  //}
    
    //    cout << gettime() - tpost << " ( Cross(d-cells) ) " << endl;
    
  tpost = gettime();
  //kcellsout->Append( c );     
    //    cout << gettime() - tpost << " ( Append(d-cells) ) " << endl;
  
  //tpost = gettime();
    //kcellsout->buildInvertedIndex();
//   cout << gettime() - tpost << " ( BuildIndex(d-cells) ) " << endl;
  

   //Out->setKCells( kcellsout, d );
   //Out->setKCells( c, d );
 
   
  AbstractCellArray *foo;
  CellArray *caOut;
  for ( int k=1; k<=d; k++ ) {
    caOut = new CellArray();
    //cout << "k: " << k << endl;
    if (k!=SIFTDIM) continue;
    for ( int j=0; j<=k; j++ ) {
    //cout << "j: " << j << endl;
      //cout << j << " cross " << k-j << endl;
      ca1 = this->getKCells( j );
      ca2 = Other->getKCells( k - j );
      //      cout << "ca1:" << j << "\n";
      //      ca1->print(10);
      //      cout << "ca2: " << k-j << "\n";
      //      ca2->print(11);
//      cout << "Cross..." << flush;
      tpost = gettime();
      foo = ca1->Cross( ca2, h ); 
      
      //cout << (gettime() - tpost) << "\n";
      //cout << "Append..." << flush;
      tpost = gettime();
      caOut->Append( foo );
      foo->unref();
      //cout << (gettime() - tpost) << "\n"; 
    }  
    
//    cout << "buildInvertedIndex...." << flush;

//    tpost = gettime();
//    caOut->buildInvertedIndex();
//    cout << (gettime() - tpost) << "\n"; 
    Out->setKCells( caOut, k );
  }
   
  return Out;
}

void Grid::setImplicit0Cells(int count) {
  Implicit0Cells *cells = new Implicit0Cells(count);
  assert(count > 0);
  Grid *G = this;
  /*
  CellArray *cells = new CellArray();
  
  for (int i=0; i<count; i++)
    cells->addCellNodes(&i, 1);
  */
  G->setKCells(cells, 0);
}

void Grid::shareCells(Grid *Out, Dim_t d) {
  Grid *In = this;
  
  In->cellsets[d]->ref();
  Out->setKCells(In->cellsets[d], d);
}

void Grid::copyCells(Grid *Out, bool *filter, Dim_t i) {
  Grid *In = this;
  //filter must be the correct size!

  AbstractCellArray *kcells;
  CellArray *newkcells = new CellArray();
  unsigned int j=0;
  
  kcells = In->cellsets[i];
  for (j=0; j<kcells->getsize(); j++) {
    if (filter[j]) {
      Cell c(kcells->getCellCopy(j));
      newkcells->addCell(&c);
    }
  } //cell loop#include "crossnodemap.h"
  Out->setKCells(newkcells, i);
}

void Grid::IncidentTo(CellId cid, Dim_t i, vector<CellId> &out, Dim_t j) {
    AbstractCellArray *ci = this->getKCells(i);
    AbstractCellArray *cj = this->getKCells(j);
    Cell *c = ci->getCell(cid);

    set<CellId> incis;
    cj->getIncidentCells(*c, incis);

    COPY(vector<CellId>, incis, out, ii)
}


void Grid::nodeFilter(Grid *Out, bool *filter) {
  Grid *In = this;
  
  AbstractCellArray *kcells;
  AbstractCellArray *zerocells;
  CellArray *newkcells;
  Node *nodes;

  unsigned int i,j,k;
  Dim_t d = In->getdim();
  bool copy = true;

  if (In->empty()) {
    Warning("The grid to filter is empty.");
    return;
  }

  zerocells = In->cellsets[0];
  hash_map<int, int> nodemap(zerocells->getsize());
  for (unsigned int n=0; n<zerocells->getsize(); n++) {
    Cell c(zerocells->getCellCopy(n));
    Node x = c.getnodes()[0];
    nodemap[x] = n;
  }
 
  for (k=0; k<=d; k++) {
    newkcells = (CellArray *) Out->getKCells(k);
    kcells = In->cellsets[k];
    
    for (j=0; j<kcells->getsize(); j++) {
      Cell c(kcells->getCellCopy(j));
      nodes = c.getnodes();
      i=0;
      while (i<c.getsize() && copy) {
	if (!filter[nodemap[nodes[i++]]]) {
	  copy = false;
	  break;
	}
      }
 
      if (copy) { newkcells->addCell(&c); }
      copy = true;
    } //cells
  }//dim
}

int Grid::cellCount(int d) {
  return cellsets[d]->getsize();
}
/*
unsigned int Grid::Size(Dim_t k) { 
  //cout << "Size: " << k << ", " << cellsets.size() << endl;
  assert(k<cellsets.size()); 
  return cellsets[k]->getsize();
};
*/

void Grid::normalize() {
  AbstractCellArray *ca = this->getKCells(0);
  NormNodeMap h(ca);
  //cout << " normalizing nodes..." << endl;
  for (int i=0; i<=getdim(); i++) {
    getKCells(i)->mapNodes(h);
  }
  //this->setKCells(new Implicit0Cells(ca->getsize()), 0);
  //ca->unref();
}

void Grid::mapNodes(UnaryNodeMap &h) {
  for (int i=0; i<=getdim(); i++) {
    getKCells(i)->mapNodes(h);
  }
}

void Grid::setReferent(OrdMap *om) {
  this->ordmap = om;
}

bool Grid::checkWellFormed() {
  Grid *G = this;
  int i, d,s; 
  set<Node> A;
  set<Node> B;
  set<Node>::iterator iter;
  bool x=true;
  AbstractCellArray *ca;

  d = G->getdim();
  G->cellsets[0]->toNodeSet(A);

  for (i=1; i<=d; i++) {
    ca = G->cellsets[i];
    if (ca->getsize() == 0) continue;
    ca->toNodeSet(B);
    iter = B.begin();

    //check subset relationship
    for (; iter!=B.end(); iter++) {
      s = A.size();
      A.insert(*iter);
      x = (A.size() == s);
      if (!x) break;
    }
  }
  return x;
}

void Grid::print() { print(0); }

void Grid::print(int indent) {
  Grid *G = this;
  int i,j;
  for (i=0; i<indent; i++) cout << " ";
  cout << "<GRID> " << "\n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "name: " << G->name << "\n";
  for (i=0; i<indent; i++) cout << " ";
  cout << "dim: " << G->getdim() << "\n";
  for (j=0; j<=G->getdim(); j++) {
    for (i=0; i<indent; i++) printf(" ");
    cout << j << "-cells: " << G->cellsets[j] << "\n";
    if (G->cellsets[j] == NULL) {
      for (i=0; i<indent; i++) cout << " ";
      cout << "--none--\n";
    } else {
      G->cellsets[j]->print(indent+2);
    }
  }
}

void Grid::ref() {
  int old = refcount;
  Object::ref(); 
  DEBUG << "grid " << this << ", " << this->name << ", ref: " << old << " -> " << this->refcount << endl;
}

void Grid::unref() {
  int old = refcount;
  Object::unref();
  DEBUG << "grid " << this << ", " << this->name << ", unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {

    DEBUG << "deleting " << this->name << "..." << endl;
    delete this;
  }
}

Grid::~Grid() {
  DEBUG << "~grid: " << this << endl;
  g_instances--;

  int i;
  for (i=0; i<cellsets.size(); i++) {
    AbstractCellArray *ca = this->cellsets[i];
    DEBUG << " unreffing cellarray " << i << flush; 
    ca->unref();
    DEBUG << "...done" << endl; 
  }


  delete ordmap;
}
