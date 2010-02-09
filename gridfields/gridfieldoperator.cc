
#include "timing.h"
#include <iostream>
#include "gridfieldoperator.h"
#include "gridfield.h"
#include "expr.h"
using namespace std;
using namespace GF;

ZeroaryGridFieldOperator::ZeroaryGridFieldOperator() {};

UnaryGridFieldOperator::UnaryGridFieldOperator(GridFieldOperator *prev) 
        : PreviousOp(prev), GF(NULL) {};

BinaryGridFieldOperator::BinaryGridFieldOperator(GridFieldOperator *L, 
                                                 GridFieldOperator *R)
        : LeftOp(L), RightOp(R), A(NULL), B(NULL) {};

GridFieldOperator::GridFieldOperator() : 
         Result(NULL), updated(true), modtime(0) { //cleanup(true), { 
}

GridField *GridFieldOperator::getResult() { 
  if (this->Updated(this->modtime)) {
    //if (this->Result) { this->Result->getScheme()->print();}
    GridField *tmp = this->Result;
    this->modtime = gettime();
    this->updated = false;
    DEBUG << "Executing....";
    this->Execute();
    DEBUG << "Finished.";
    if (tmp) {
      DEBUG << "UNREFFING the result"<< endl;
      tmp->unref();
    }
  }
  DEBUG << "refcount: " << this->Result->refcount << endl;
  return this->Result; 
};

void ZeroaryGridFieldOperator::PrepareForExecution() {} 

void UnaryGridFieldOperator::PrepareForExecution() {
  if (this->PreviousOp == NULL) {
     if (this->GF == NULL) {
       Fatal("No gridfield or previous operator provided as input to Operator.");
     }
  } else {
    if (this->GF == NULL) {
      //Warning("Overwriting GridField pointer with previous op's output.");
    }
    this->GF = this->PreviousOp->getResult();
  }
  
  if (this->GF == NULL) {
    Fatal("No gridfield provided as input to Operator.");
  }
  //std::cout << "UnOp: " << gettime() - start << std::endl; 
}

void BinaryGridFieldOperator::PrepareForExecution() {
  char arg = '0';
  if (this->LeftOp == NULL && this->A == NULL) {
    arg = 'A';
  }
  if (this->RightOp == NULL && this->B == NULL) {
    arg = 'B';
  }
  if (arg != '0') {
    Fatal("No %c argument or previous operator provided as input to Operator.", arg);
  }
  
  //cout << this->LeftOp << ", " << this->A << endl;
  //cout << this->LeftOp->Updated() << endl;
  if (this->LeftOp != NULL && this->A != NULL) {
    arg = 'A';
  }
  if (this->RightOp != NULL && this->B != NULL) {
    arg = 'B';
  }
  if (arg != '0') {
  //  Warning("Overwriting pointer to argument %c with previous op's output.", arg);
  }
  
  this->A = this->LeftOp->getResult();
  this->B = this->RightOp->getResult();
  
  if (this->A == NULL || this->B == NULL) {
    Fatal("No gridfield available as input to binary operator.");
  }
  //std::cout << "BinOp: " << gettime() - start << std::endl; 
}

GridFieldOperator::~GridFieldOperator() {
  // if we're a trivial gridfield operator, do nothing
  // otherwise dispose of our result
//  cout << this << ", " << this->Result << ", " << (this->Result == this) << endl;
  //if (this->cleanup) {
    
 // DEBUG << "~gridfieldop: " << this << ", " << this->Result << ", " << this->Result->GetGrid() << " -> " << endl;
    
    //cout << "deleting result gridfield" << endl;
    //this->Result->unref();
  //clearResult();
 // }
  //DEBUG << "Delete operator"  << endl;
}

inline bool ZeroaryGridFieldOperator::Updated(float sincetime) {
  return this->updated || this->modtime > sincetime;
}

bool UnaryGridFieldOperator::Updated(float sincetime) {
  bool result = this->updated || this->modtime > sincetime || this->PreviousOp->Updated(sincetime);
  return result;
}

bool BinaryGridFieldOperator::Updated(float sincetime) {
  bool result =  this->updated || this->modtime > sincetime 
	      || this->LeftOp->Updated(sincetime) || this->RightOp->Updated(sincetime);
  return result;
}

void GridFieldOperator::Update() {
  this->updated = true;
}

void GridFieldOperator::clearResult() {
  DEBUG << "Clear operator"  << endl;
  if (this->Result) { // && this->cleanup) {
    //delete this->Result;
    this->Result->unref();
  }
}
