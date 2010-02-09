#include "gridfield.h"
#include "array.h"
#include "gfutil.h"
#include "grid.h"
#include <string>

using namespace GF;

GridField::GridField(Grid *G) : grid(G) {
  this->init(G);
}


GridField::GridField(Grid *G, Dim_t k, Array *arr) {
  this->init(G);
  this->Bind(k, arr);
}


GridField::GridField(GridField *GF) {
  this->init(GF->grid);
  for(Dim_t k=0; k<=GF->Dim(); k++) {
    this->Zip(k, GF->GetDataset(k));
  }
}


void GridField::PrepareForExecution() { };
bool GridField::Updated(float sincetime) { return false; }
void GridField::Execute() { this->Result = this; };


void GridField::init(Grid *G) {

  //cleanup tells the base class to delete a gridfield;
  //we'll take care of this ourselves, though
  Dim_t d = G->getdim();
  Shape s(d+1,0);
  for (int i=0; i<d+1; i++) {
    s[i] = G->Size(i);
  }
  this->SetShape(s);
  //this->cleanup = false;
  
  this->updated = false;

  this->Result = this;

  this->grid = G;
  this->grid->ref();
}

void GridField::setGrid(Grid *G) {
  Grid *tmp = this->grid;  
  G->ref(); 
  this->grid = G; 
  tmp->unref();
}


/*
void GridField::flattenAttr(string attr) {
  Array *a = this->getAttribute(attr.c_str());
  Array *b;
  if (a->type == OBJ) {
    Scheme *sch = a->getScheme();
    for (int i=0; i<sch->size(); i++) {
      b = new Array(a, sch->getAttribute(i));
      this->Bind(b);
    }
    this->unBind(attr);
  }
}
*/

void GridField::RangeRestrict(Dim_t k, const GridField &G) {
  const Dataset &ds = G.GetDataset(k);
  Scheme s = ds.GetScheme();

  unsigned int ncells = this->Card(k);
  unsigned int ntups = ds.Size();
  
  // make sure there are attributes to restrict
  if (s.size() == 0) return;

//  this->AddAttribute(
  this->CoerceScheme(k, &s, ncells);
  const Dataset &newds = this->GetDataset(k);

  Tuple from(&s);
  Tuple to(&s);

  AbstractCellArray *kcells = grid->getKCells(k);
  AbstractCellArray *otherkcells = G.grid->getKCells(k);
  for (int i=0; i<ncells; i++) {
    Cell *c = kcells->getCell(i);
    int j = otherkcells->getOrd(*c);
    if (i>= ds.Size()) cout << "error? " << i << ", " << ds.Size() << endl;
    newds.FastBindTuple(i, to);
    ds.FastBindTuple(j, from);
    to.copy(from);
  }
  //this->Zip(k, newds);
}

void GridField::RestrictAll(const GridField &Base) {
  // (this) is a subgrid of Base
  // find data values of Base ssociated with the cells of (this)
  // create and bind the values as attributes
  for (Dim_t k=0; k<=Dim(); k++) {
    //Base.GetScheme(k).PrintTo(cout, 5);
    this->RangeRestrict(k, Base);
  }
}

/*
void GridField::subGridData(Grid *subgrid, vector<Array *> &outvec) {
  //provides dataset corresponding to the kcells in the subgrid
  Array *arr;
  Cell *c;
  int o;

  bool *filter = new bool[this->card()];
  for (int i=0; i<this->card(); i++) {
    filter[i] = false;
  }

  AbstractCellArray *kcells = grid->getKCells(k);
  AbstractCellArray *subkcells = subgrid->getKCells(k);
  int size = subkcells->getsize();

  for (int i=0; i<size; i++) {
    c = subkcells->getCell(i);
    o = kcells->getOrd((const Cell) *c);
    if (o == -1) {
      Fatal("Grid %s passed to subgGridData not a subgrid of %s", 
	    subgrid->name.c_str(), this->grid->name.c_str());
    }
    filter[o] = true;
  }
   

  for (int j=0; j<attributes.size(); j++) {
    arr = attributes[j]->copyAndFilter(filter);
//    arr->print();
    outvec.push_back(arr);
  }
}
*/
/*
void GridField::NextTuple(Tuple &t) {
  for (int i=0; i<t.size(); i++) {
    attr = t.getAttribute(i);
    t.set(attr, this->getAttributeVal(attr.c_str(), idx));
  }
}
*/
/* Coerces a gridfield to a new scheme, allocating
 * new arays as necessary
 */

int GridField::notValid() {
  GridField *GF = this;
  int i, size;
  Grid *G;
  G = GF->grid;

  for (Dim_t k=0; k<this->Dim(); k++) {
    // |k-cells| == |data values|
    if (G->cellsets[k]->getsize() != this->GetDataset(k).Size()) {
      return 6;
    }
  }

  return 0;

}


void GridField::lookupInt(string attr, Dim_t k, int p, vector<CellId> &out) {
  Array *arr = this->GetAttribute(k, attr);
  assert(arr->type == INT);
  int *data;

  arr->getData(data);
  
  size_t n = Size(k);
  for (int i=0; i<n; i++) {
    if (data[i] == p) { 
      out.push_back(i);
    }
  }
}

template<class T>
void nearestfunc(Array *arr, string attr, Dim_t k, UnTypedPtr p, vector<CellId> &out) {
  typedef T valtype;
  valtype near, v;
  valtype pf = *(valtype *)p;
  near = *(valtype *)arr->getValPtr(0);
  int nearest=0;
  
  for (int i=1; i<arr->size(); i++) {
    v = *(valtype *)arr->getValPtr(i);
    if (abs(pf-v) <= abs(pf-near)) {
      nearest = i;
      near = v;
    }
  }

  out.push_back(nearest);
}

void GridField::nearest(const string &attr, Dim_t k, UnTypedPtr p, vector<CellId> &out) {
  Array *arr = this->GetAttribute(k, attr);
  switch (arr->type) {
    case INT:
      nearestfunc<int>(arr, attr, k, p, out);
      break;
    case FLOAT:
      nearestfunc<float>(arr, attr, k, p, out);
      break;
    default:
      Fatal("nearest not defined on object types");
      break;
  }
}

void GridField::lookupFloat(string attr, Dim_t k, float p, vector<CellId> &out) {
  Array *arr = this->GetAttribute(k, attr);
  assert(arr->type == FLOAT);
  float *data;

  arr->getData(data);
  
  size_t n = Size(k);
  for (int i=0; i<n; i++) {
    if (data[i] == p) { 
      out.push_back(i);
    }
  }
}

void GridField::print(int a) {
  this->PrintTo(cout, a);
}
void GridField::print() {
  this->PrintTo(cout, 0);
}

void GridField::PrintTo(ostream &os, int indent) const {
  os << "GridField:" <<endl;
  grid->print(indent+2);

  for (int i=0; i<=Dim(); i++) {
    GetDataset(i).PrintTo(os, indent+4);
  }
}

void GridField::unref() {
  int old = refcount;
  Object::unref();
  DEBUG << "gridfield.unref " << this << ", " << this->grid->name << ", unref: " << old << " -> " << this->refcount << endl;
  if (this->norefs()) {

    DEBUG << "deleting gridfield" << this->grid->name << "..." << endl;
    this->Result = NULL;
    delete this;
  }

} 

GridField::~GridField() {
  DEBUG << "ref count at destruction: " << this->refcount << endl;
  DEBUG << "gridfield deleted: " << this->grid->name << ", " << this->grid->refcount << endl;
  if (grid != NULL) this->grid->unref();
}

UnitGridField::UnitGridField() : GridField(new UnitGrid()){
}

void UnitGridField::Bind(const string &name, int val) {
  Array *arr = new Array(name, INT, 1);
  int *aval;
  arr->getData(aval);
  *aval = val;
  GridField::Bind(0, arr);
}
void UnitGridField::Bind(const string &name, float val) {
  Array *arr = new Array(name, INT, 1);
  float *aval;
  arr->getData(aval);
  *aval = val;
  GridField::Bind(0, arr);
}
void UnitGridField::Bind(const string &name, UnTypedPtr val) {
  Array *arr = new Array(name, INT, 1);
  UnTypedPtr *aval;
  arr->getData(aval);
  *aval = val;
  GridField::Bind(0, arr);
}
