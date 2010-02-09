#ifndef _GRIDFIELDOPERATOR_H
#define _GRIDFIELDOPERATOR_H

#include "object.h"


namespace GF {
class GridField;

//class GridFieldOperator : public Object {
class GridFieldOperator {
public:
  GridFieldOperator();
  virtual ~GridFieldOperator();
  GridField *getResult();
  virtual void Execute() = 0;
  void Update();
  void clearResult();
  virtual bool Updated(float sincetime)=0;
  float getModTime() { return this->modtime; };
protected:
  //bool cleanup;
  virtual void PrepareForExecution() = 0;
  GridField *Result;
  bool updated;
  float modtime;
};

class ZeroaryGridFieldOperator : public GridFieldOperator {
  public:
    bool Updated(float sincetime);
  protected:
    ZeroaryGridFieldOperator();
    void PrepareForExecution();
};

class UnaryGridFieldOperator : public GridFieldOperator {
  public: 
    bool Updated(float sincetime);
    GridFieldOperator *GetPrevious() { return this->PreviousOp; this->Update(); };
    void SetPrevious(GridFieldOperator *Op) { this->PreviousOp = Op; this->Update();};
    
    
  protected:
    UnaryGridFieldOperator(GridFieldOperator *prev);
    void PrepareForExecution();
    GridFieldOperator *PreviousOp;
    GridField *GF;
};

class BinaryGridFieldOperator : public GridFieldOperator {
  public:
    bool Updated(float sincetime);
    GridFieldOperator *GetLeft() { return this->LeftOp; };
    void SetLeft(GridFieldOperator *L) { this->LeftOp = L; this->Update(); };
    GridFieldOperator *GetRight() { return this->RightOp; };
    void SetRight(GridFieldOperator *R) { this->RightOp = R; this->Update(); };

  protected:
    BinaryGridFieldOperator(GridFieldOperator *L, GridFieldOperator *R);
    BinaryGridFieldOperator() {};
    void PrepareForExecution();
    GridFieldOperator *LeftOp;
    GridFieldOperator *RightOp;
    GridField *A;
    GridField *B;
};

}
#endif
