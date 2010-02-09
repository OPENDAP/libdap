#include <string>
#include <iostream>
#include <sstream>
#include "expr.h"
#include "gridfield.h"
#include "tuple.h"
#include "type.h"
#include "fparser.hh"
extern "C" {
  #include <stdarg.h>
}

using namespace std;
using namespace GF;


TupleFunction::TupleFunction() {
  SaveReservedWords();
}
  
void TupleFunction::deleteFunctions() {
  if (!functions.empty()) {
    delete [] bindings;
  }
  map<string, FunctionParser *>::iterator p;
  for (p=functions.begin(); p!=functions.end(); p++) {
    delete p->second;
  }
}

TupleFunction::~TupleFunction() {
  deleteFunctions();
}

void TupleFunction::Parse(string unparsedExpr) {

  // release memory if we've already parsed an expression.
  deleteFunctions();
   
  //extracts input and output types
  //and builds a function parser object
  vector<string> attribute_exprs;
  int pos;
  while (true) {
    pos = unparsedExpr.find("\n");
    if (pos == string::npos) break;
    unparsedExpr[pos] = ' ';
  } 
  split(unparsedExpr, ";", attribute_exprs);

  int n = attribute_exprs.size();
  if (n==0) Fatal("No attribute expressions found");
  
  vector<string> equation;
  set<string> input_attributes;
  vector<string> vars;
  
  
  map<string, string> expr;
  
  for (int i=0; i<n; i++) {
    string rawexp = attribute_exprs[i];
    int pos = rawexp.find("=");
    if (pos == -1) {
      Fatal("Malformed attribute expression: %s", rawexp.c_str());
    } else {
      equation.push_back(rawexp.substr(0, pos));
      equation.push_back(rawexp.substr(pos+1));
    }

    // parse expression for variable names
    getVars(equation[1], vars);
      
    //find the unique input attributes mentioned in any expression
    input_attributes.insert(vars.begin(), vars.end());
    
    string attr = remove_whitespace(equation[0]);
    expr[attr] = equation[1];
    vars.clear();
    equation.clear();
  }

  // build the input type, and the string list of variables for fparser.
  set<string>::iterator si;
  string varstr = "";  
  for (si=input_attributes.begin(); si!=input_attributes.end(); si++) {
    this->inScheme.addAttribute((*si), FLOAT);
    varstr = varstr + *si + ",";
  }

  //construct the function objects for evaluation,
  //parse the expressions, and build the output type.
  map<string, string>::iterator p;
  FunctionParser *fp;
    
  for (p=expr.begin(); p!=expr.end(); p++) {
    this->outScheme.addAttribute(p->first, FLOAT);
    fp = new FunctionParser();
    this->functions[p->first] = fp;
    
    if (fp->Parse(p->second, varstr) != -1)
      Fatal("Parse error: %s: %s", equation[1].c_str(), fp->ErrorMsg());
  }

  bindings = new double[inScheme.size()];
}

void TupleFunction::Eval(Tuple &intup, Tuple &outtup) {
  //both tuples should be of the correct type and have space reserved.
    
  double answer=0;
  float *valptr;
  
  map<string, FunctionParser *>::iterator p;
  BindVars(intup, bindings);
  assert(!intup.isNull());
  assert(!outtup.isNull());

  for (p=functions.begin(); p!=functions.end(); p++) { 
    answer = p->second->Eval(bindings);
    //cout << answer << endl;
    valptr = (float *) outtup.get(p->first);
    *valptr = (float) answer;
  }
}

void SpecializedTupleFunction::SpecializeFor(Scheme &out) {
  for (int i=0; i<inScheme.size(); i++) {
    string const &attr = inScheme.getAttribute(i);
    in_position_map[i] = make_pair(out.getPosition(attr), out.getType(attr));
  }
                                                                                 
  map<string,  FunctionParser *>::iterator p;
  for (p=functions.begin(); p!=functions.end(); p++) {
                                                                                 
    out_position_map[outScheme.getPosition(p->first)] =
                 make_pair(out.getPosition(p->first), p->second);
  }
                                                                                 
   in_tup_size = inScheme.size();
}


void SpecializedTupleFunction::Eval(Tuple &intup, Tuple &outtup) {
  //both tuples should be of the correct type and have space reserved.
    
  double answer=0;
  float *valptr;
    
  hash_map<int, pair<int, Type> >::iterator p;
  
   // cout << endl;
  for (int i=0; i<in_tup_size; i++) {
    p = in_position_map.find(i);
    if (p->second.second == FLOAT) {
      //cout << "i=" << p->second.first << ", ";
      //cout << intup.tupledata.size() << endl;
      int id = p->second.first;
      float foo = *(float *) intup.tupledata[id];
      //bindings[i] = *(float *) intup.tupledata[p->second.first];
      bindings[i] = foo;
      //cout << bindings[i] << endl;
    } else {
      bindings[i] = float(*(int *) intup.tupledata[p->second.first]);
    }
  }

  hash_map<int, pair<int, FunctionParser *> >::iterator q;
  for (q=out_position_map.begin(); q!=out_position_map.end(); q++) { 
    valptr = (float *) outtup.tupledata[q->second.first];
    answer = q->second.second->Eval(bindings);
    *valptr = (float) answer;
  }
}

void TupleFunction::BindVars(Tuple &intup, double *bindings) {
  string var;
  Scheme *real_inScheme = intup.getScheme();
  for (int i=0; i<inScheme.size(); i++) {
    var = inScheme.getAttribute(i);
    if (real_inScheme->getType(var) == FLOAT) {
      bindings[i] = *(float *)intup.get(var);
    } else {
      bindings[i] = float(*(int *)intup.get(var));
    }
  }
}


void TupleFunction::getVars(string expr, vector<string> &retval) {
    
  string var("");
  int i=0;

  while (i<expr.length()) {
    //variables begin with an alpha and are not reserved words
    if (isalpha(expr[i])) {
      while (isalpha(expr[i]) || expr[i] == '_' || isdigit(expr[i])) {
        var = var + expr[i++];
      }
        
      // if not a reserved word
      if (reservedWords.find(var) == reservedWords.end())
        // and if not already added (using a vector to maintain order)
        if (find(retval.begin(), retval.end(), var) == retval.end()) 
          // insert it
          retval.push_back(var);      

      var = "";
    }
    i++;
  }
}

void TupleFunction::SaveReservedWords() {

  set<string> &vars = reservedWords;
  vars.insert("sqrt");
  vars.insert("abs");
  vars.insert("acos");
  vars.insert("acosh");
  vars.insert("asin");
  vars.insert("asinh");
  vars.insert("atan");
  vars.insert("atan2");
  vars.insert("atanh");
  vars.insert("ceil");
  vars.insert("cos");
  vars.insert("cosh");
  vars.insert("cot");
  vars.insert("csc");
  vars.insert("eval");
  vars.insert("exp");
  vars.insert("floor");
  vars.insert("if");
  vars.insert("int");
  vars.insert("log");
  vars.insert("log10");
  vars.insert("max");
  vars.insert("min");
  vars.insert("sec");
  vars.insert("sin");
  vars.insert("sinh");
  vars.insert("sqrt");
  vars.insert("tan");
  vars.insert("tanh");

}

