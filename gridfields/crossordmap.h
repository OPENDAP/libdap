#ifndef _CROSSORDMAP_H
#define _CROSSORDMAP_H

#include "ordmap.h"
#include "cellarray.h"
#include "gridfield.h"

namespace GF {
class CrossOrdMap : public OrdMap {

 public: 
  CrossOrdMap(Grid *ga, Grid *gb, GridField *gf) { 
    A = ga;
    B = gb;
    G = gf->GetGrid();
    GF = gf;
  };
  virtual int getBaseSize(int d) {
    int n,m,ord=0;
    for (int k=0; k<d; k++) {
      n = A->cellCount(k);
      m = B->cellCount(d-k);
      ord += n*m;
    }
    return ord;
  }
  
  virtual int getBaseOrd(Cell *c, int d) { 
    AbstractCellArray *Acells, *Bcells, *Gcells;
    int n,m,ai,bi,bsize,pos,ord=0;
    Cell *a, *b;

    Gcells = G->getKCells(d);
    pos = Gcells->getOrd((const Cell) *c);

    // cout << "\npos, d: " << pos << ", " << d << endl;

    for (int k=0; k<=d; k++) {
      Acells = A->getKCells( k );
      Bcells = B->getKCells( d - k );
      n = Acells->getsize();
      m = Bcells->getsize();
      ord += n * m;
      //      cout << "ord: " << ord << endl;
      if (pos < ord) {
        a = Acells->getCell(pos / m);
        b = Bcells->getCell(pos % m);
	//cout << "a, b: " << endl;
	//a->print(1);
	//b->print(1);
        ai = A->ordmap->getBaseOrd( a, k );
        bi = B->ordmap->getBaseOrd( b, d - k );
	bsize = B->ordmap->getBaseSize( d - k );
	//	addr = *(float *) GF->getAttributeVal("addr", pos);
	//	zpos = *(float *) GF->getAttributeVal("zpos", pos);
	//	bot = *(float *) GF->getAttributeVal("b", pos);
	//cout << "  addr, zpos, pos: " << addr << ", " << zpos << ", " << pos << endl;
	//	return (int) addr + (zpos - bot);
	//	cout << "addr: " << addr << endl;
	//	getchar();
        return ai*bsize + bi;
	
      }
    }
  };
  
 private:
  // G = A x B
  Grid *A;
  Grid *B;
  Grid *G;
  GridField *GF;
};
}
#endif /* _CROSSORDMAP_H */
