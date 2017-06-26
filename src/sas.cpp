#pragma once
#include "SA.h"
using namespace _SA;
using namespace std;

template <class T>
struct Pair{
  int index;
  T value;
};

template <class T>
inline ostream &operator<<( ostream &output, Pair<T> &out){
  output << "[" << out.index <<","<< out.value<<"]";
  return output;
}

template<class T>
inline bool operator < (Pair<T> P1, Pair<T> P2){
  return P1.value < P2.value;
}

template<class T>
class SAS : public SA< Pair<T> >{
  public:

  SAS( unsigned int mysize): SA< Pair<T> >( mysize ){
    int i;
    for(i=0; i< (this->size()); i++){
      (this->at(i)).index = i;
    }
  }

  inline int & index( int i){
    return (this->at(i)).index;
  }

  inline float & f( int i){
    return (this->at(i)).value;
  }

  void Sort(){
    Pair<T> * adr =&(this->at(0));
    sort(adr, adr+((this->size())));
    int i;
    for(i=1; i<(this->size()); i++){
      T v1 = (this->at(i-1)).value;
      T v2 = (this->at(i )).value;
      if( v1 > v2){
        printf("Error: sas.cpp sort failed.\n");
        printf("Sort failed at position %d\n", i);
        exit(1);
      }
    }
  }

  void reset(){
    int i;
    for(i=0; i< (this->size()); i++){
      (this->at(i)).index = i;
    }

  }
};

