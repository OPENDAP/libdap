#ifndef OBJECT_H
#define OBJECT_H

namespace GF {
// manual reference counting for shared objects
// We can replace this with a smart pointer implementation at some point.
class Object {
  public:
    Object() : refcount(0) {};
    /*
    void *operator new(size_t sz) { 

      cout << "obj::new" << endl;
      return malloc(sz); 
    };
    void operator delete(void *mem) { 
      cout << "obj::delete" << endl;
      Object *o = ((Object *) mem);
      o->unref(); 
      if (o->norefs()) free(mem);
    };
    */
    void ref() {
      refcount++;
    };
    
    bool norefs() { return (refcount == 0); }
    
    /*
    void unref() { 
      refcount--; 
      cout << "obj::unref" << endl; 
      if (refcount == 0) delete this;
    };
    */
    void unref() { 
      refcount--; 
    };
    
    //with a virtual destructor, I should be able
    //to use delete this in unref above and have
    //the correct destructor called.
    //SWIG, however, seems to want a destructor for Object
    
    //virtual ~Object()=0;

    int refcount;
  protected:
};

}
#endif
