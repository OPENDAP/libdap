
#include "type.h"
#include "iostream"


char GF::typeformat(GF::Type t) {
  switch (t) {
    case FLOAT:
      return 'f';
    case INT:
      return 'i';
    case OBJ:
      return 'p';
  }
};

GF::Type GF::typeval(std::string typestring) {
  if (typestring == "f" || typestring == "F") {
    return FLOAT;
  }
  if (typestring == "i" || typestring == "I") {
    return INT;
  }
  if (typestring == "p" || typestring == "P") {
    return OBJ;
  }
  if (typestring == "o" || typestring == "O") {
    return OBJ;
  }
}
