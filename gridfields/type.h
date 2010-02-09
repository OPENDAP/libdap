#ifndef _TYPE_H
#define _TYPE_H
#include <string>
#include <stdio.h>

namespace GF {
enum e_Tag {VAR=1, VAL};
typedef enum e_Tag Tag;

enum e_Type {INT=1, FLOAT, OBJ, TUPLE, GRIDFIELD};
typedef enum e_Type Type;

typedef void *UnTypedPtr;
typedef float UnTypedVal;
typedef void *Datum;

char typeformat(Type t);

Type typeval(std::string typestring);

typedef unsigned int Node;

inline
int typesize(Type t) {
  switch (t) {
    case FLOAT:
      return sizeof(float);
    case INT:
      return sizeof(int);
    case OBJ:
      return sizeof(void *);
    case TUPLE:
      return sizeof(void *);
    case GRIDFIELD:
      return sizeof(void *);
    default: 
      return sizeof(int);
  }
};

inline
void plusplus(UnTypedPtr *p, Type type) {
    *p = (UnTypedPtr) ((char *) (*p) + typesize(type));
}; 
}
#endif
