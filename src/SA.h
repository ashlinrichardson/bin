/* SA.h: template array with bounds checking. Like Shane Ryan's SArray class, but with object (rather than object pointer) type, and some float operations. This is the July 2009 era version */

#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <float.h>

using namespace std;
using std::ostream;
using std::vector;

namespace _SA{

  template<class T> class SA{
    
    public:

    void init(unsigned int _sizei, unsigned int _sizej, unsigned int _sizek){
      sizei = _sizei;
      sizej = _sizej;
      sizek = _sizek;
      int size = sizei * ((sizej==0)?1:sizej) * ((sizek==0)?1:sizek);
      mySize = size;
      elements = NULL;

      if(size > 0){
        mySize = size;
        elements = NULL;
        elements = new T[size];
        if(!elements){
          printf("Error: SA.h: failed to allocate array.\n");
          exit(1);
        }
        memset(&(elements[0]), '\0', mySize*sizeof(T));
      }
    }

    SA(unsigned int size){
      init(size, 0, 0);
    }

    SA(unsigned int sizei, unsigned int sizej){
      init(sizei, sizej, 0);
    }

    SA(unsigned int sizei, unsigned int sizej, unsigned int sizek){
      init(sizei, sizej, sizek);
    }

    ~SA(){
      free(elements);
    }

    /* copy initializer */
    inline SA(SA<T> * other){
      if(!other){
        mySize = 0;
        elements = NULL;
      }
      mySize = other->size();
      if(mySize == 0){
        elements = NULL;
        return;
      }
      else{
        elements = new T[mySize];
        for(register unsigned int i = 0; i < mySize; i++)
        elements[i] = (*other)[i];
      }
      sizei = other->sizei;
      sizej = other->sizej;
      sizek = other->sizek;

    }

    inline unsigned int size(){
      return mySize;
    }

    inline unsigned int length(){
      return mySize;
    }

    inline T & at(unsigned int subscript){
      if(sizej + sizek > 0){
        printf("Error: must not interpret 2/3 dimensional array 1-dimensionally.\n");
        exit(1);
      }
      return elements[subscript];
    }

    inline T & at(unsigned int indi, unsigned int indj){
      if(sizek > 0 || sizej == 0){
        printf("Error: must use 2-d index for 2-d array\n");
        exit(1);
      }
      return elements[(indi * sizej) + indj];
    }

    inline T & at(unsigned int indi, unsigned int indj, unsigned int indk){
      if(sizei * sizej * sizek < 1){
        printf("Error: must only use 3-d index with 3d array.\n");
        exit(1);
      }
      return elements[(indi * sizej * sizek) + (indj * sizek) + indk];
    }

    inline void set(unsigned int subscript, T * e){
      elements[subscript] = *e;
    }

    /* array notation: equivalent to at() */
    inline T & operator[](unsigned int subscript){
      //if(sizej + sizek > 0){
        // printf("SA.h: Error: not allowed to use [] notation for 2/3 dimensional arrays.\n");
        // exit(1);
      //}
      if(mySize == 0){
        std::cerr << "\nError: SA.cpp: size()=0 Subscript " << subscript << " out of range" << std::endl;
        exit(1);
      }
      if(subscript > mySize){
        std::cerr << "\nError: SA.cpp: Subscript " << subscript << " out of range" << std::endl;
        exit( 1 );
      }
      return elements[subscript];
    }

    inline SA<T> & operator = (SA<T> &a){
      if(a.size() != mySize){
        printf("SA.h: error: tried to equate arrays of different lengths.\n");
        exit(1);
      }
      unsigned register int i = 0;
      for(i = 0; i < mySize; i++){
        elements[i] = a[i];
      }
      return a;
    }

    inline T total(){
      T a = 0;
      for(int i = 0; i < mySize; i++){
        a += elements[i];
      }
      return a;
    }

    inline void sortf(){
      /* only use if the data is float */
      float * adr = (float*) ((void*) (&elements[0]));
      sort(adr, adr+(mySize));
    }

    float percentBinBot(float _percent_){
      int ind = (int)(((float)(mySize-1))*(_percent_/100.));
      return (float)(elements[ind]);
    }

    float percentBinTop(float _percent_){
      int ind = (mySize-1) - (int)(((float)(mySize-1))*(_percent_/100.));
      return (float)(elements[ind]);
    }

    float max(){
      float m = FLT_MIN;
      for(register int i = 0; i < mySize; i++){
        if(!((isnan(elements[i])||(isinf(elements[i]))))){
          if(elements[i]>m){
            m = elements[i];
          }
        }
      }
      return m;
    }

    float max2(){
      return max();
    }

    float min(){
      float m = FLT_MAX;
      for(register int i = 0; i < mySize; i++){
        if(!((isnan(elements[i])||isinf(elements[i])))){
          if(elements[i] < m){
            m = elements[i];
          }
        }
      }
      return m;
    }

    public:
    unsigned int mySize, sizei, sizej, sizek;
    T* elements;

    public:
    inline void clear(void){
      memset(elements, '\0', mySize*sizeof(T));
    }
  };

  template <class T>
  inline ostream &operator<<(ostream &output, SA<T> &out){
    unsigned register int i=0;
    for(i = 0; i < (out.length()); i++){
      if(i != out.length() - 1){
        output << out[i]<<",";
      }
      else{
        output << out[i];
      }
    }
    return output;
  }

  template <typename T>
  static inline void XORSwap(T&a, T&b){
    /* XOR swap operation */
    if(&a != &b){
      a^=b;
      b^=a;
      a^=b;
    }
  }

  template <typename T>
  static inline T _abs(T a){
    return (a<0)?(-a):(a);
  }

};
