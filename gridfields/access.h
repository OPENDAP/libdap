#ifndef _CATALOG_H
#define _CATALOG_H

#include "object.h"
#include "gfutil.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

namespace GF {
typedef char Byte;

template<class T>
class DatumIterator {
 public:
  typedef T Datum_t;
  virtual void Open()=0;
  virtual Datum_t Next()=0;
  virtual bool Done()=0;
  //virtual ~DatumIterator()=0;
};

template<class T>
class CArrayIterator : public DatumIterator<T> {
  public:
    CArrayIterator(T *s, size_t sz) : source(s), size(sz), i(0) {};
    void Open() {};
    T Next() { return source[i++]; }
    bool Done() { return (i>=size); }
    
  private:
    T *source;
    size_t size;
    idx i;
};

template<class T>
class SliceIterator : public DatumIterator<T> {
  public:
    SliceIterator(DatumIterator<T> &b, idx start, idx stop, idx step=1) 
       : _start(start), _step(step), _stop(stop), i(0), base(b) {};
    
    virtual void Open() {
      base.Open();
      for (int j=_start; j>0; j--) base.Next();
    };
     
    virtual T Next() { 
      T d = base.Next();
      
      // advance to just before first element
      idx next = MIN(_start+i*_step, _stop);
      for (int j=_step-1; j>0; j--) base.Next();
      i++; 
      return d;
    };

    virtual bool Done() {
      return (_start + i*_step >= _stop) || (base.Done());
    };

  private:
    idx _start, _step, _stop;
    idx i;
    DatumIterator<T> &base;
};

template<class T>
class PrimitiveIterator : public DatumIterator<T> {
  public:
    PrimitiveIterator(DatumIterator<Byte> &b) : base(b) {};
  
    void Open() { base.Open(); }

    T Next() { 
      for (int i=0; i<sizeof(T); i++) {
        buf[i] = base.Next();
      }
      return *(T *) buf;
    };
    
    bool Done() { return base.Done(); };
    
  private:
    Byte buf[sizeof(T)]; 
    DatumIterator<Byte> &base;
};
/*
template
class ProjectIterator : public DatumIterator<Byte *> {
  public:
    ProjectIterator(DatumIterator
}
*/

class MMapIterator : public DatumIterator<Byte> {
  public:
    MMapIterator(const std::string &fname, int off, int len) 
        : i(off), end(len), filename(fname) {};
        
    MMapIterator(const std::string &fname, int off=0) 
        : i(off), end(-1), filename(fname) {}

    void Open() { 

      struct stat sbuf;
      if ((fd = open(filename.c_str(), O_RDONLY)) == -1) {
        perror("Open");
        exit(1);
      }

      if (stat(filename.c_str(), &sbuf) == -1) {
        perror("Stat");
        exit(1);
      }
      
      if (end<0) end = sbuf.st_size;

      if ((data = (char *) mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, 
                     fd, 0)) == (caddr_t)(-1)) {
         perror("mmap");
         exit(1);
      }
      
    }
    
    void Advance(idx n) { i+=n; }
    
    Byte Next() { return data[i++]; }

    bool Done() { return (int) i >=end; }
    
  private:
    idx i;
    int end;
    const std::string filename;
    int fd;
    Byte *data;
};
/*
template<class T>
class ElcircSurfIterator : public DatumIterator<T> {
  public:
    ElcircSurfIterator(MMapIterator mi, ElcircHeader &h) : hdr(h), rdr(mi) {};
    void Open() { 
      offset = this->h.hsize + timestep*this->h.ssize + sizeof(int) + sizeof(float);
      end = 
      rdr.Advance(offset);
      // pi = new PrimitiveIterator<T>(rdr)
    }

    T Next() { return rdr.Next(); };
    bool Done() { return (i>=end); }

  private:
    idx end,offset;  
    ElcircHeader &hdr;
    MMapIterator &rdr;
}
*/
}
#endif
