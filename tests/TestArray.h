#ifndef _testarray_h
#define _testarray_h 1

#ifndef __POWERPC__
#ifdef __GNUG__
#pragma interface
#endif
#endif

#include "Array.h"

class TestArray: public Array {
public:
    TestArray(const string &n = "", BaseType *v = 0);
    virtual ~TestArray();

    virtual BaseType *ptr_duplicate();

    virtual bool read(const string &dataset);
};

#endif // _testarray_h

