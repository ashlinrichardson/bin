/* dmat/dmat.h: distance matrix calculation for
k-nearest neighbors graph clustering algorithm

july 17, 2013 with modifications 20170618 */
#include<fstream>
#include<iostream>
#include<memory.h>
#include<stdlib.h>
#include<sstream>
#include<stdio.h>
#include<vector>
#include<math.h>
#include<time.h>
#include<limits>
#include<cfloat>
#include<queue>

#ifndef __DIRECT_H_INCLUDED
#define _getcwd getcwd
#include<unistd.h>
#else
#include<direct.h>
#endif

#define for0(j,n) for(j=0; j<(n); j++)

using namespace std;

/* exit with memory cleanup*/
void quit(int retcode){
  exit(retcode);
}

class mytimer{
  public:
  time_t tstart, tend, tlast;
  struct tm * tstart_info;
  struct tm * tend_info;
  struct tm * tlast_info;
  clock_t clast;
  mytimer(){
    time(&tstart);
    tlast = tstart;
    clast = clock();
    tstart_info = localtime(&tstart);
  }
  void stop(){
  }
  double delta(){
    time_t now;
    time(&now);
    clock_t cnow = clock();
    //double seconds = difftime(now, tlast);
    double seconds = cnow - clast;
    seconds = seconds / ((double)CLOCKS_PER_SEC);
    tlast = now;
    clast = cnow;
    return(seconds);
  }
};

template <class T> class SA{
  /*"Safe Array" with bounds checking and "Safe Allocation"*/
  public: long int s; T * elements;
  SA(){
    s=0; elements = NULL;
  }
  void init(long int S){
    s=S;
    elements= (T*) malloc(S*sizeof(T));
    if(!elements){
      printf("Error: could not allocate %ld bytes.\n",S*((long int)(sizeof(T))) ); exit(1);
    }
  }
  SA(long int S){
    init(S);
  }
  long int size(){
    return s;
  }
  inline T & operator[]( long int i){
    if(i>=s){
      printf("Array index (%ld) out of bounds.\n", i);
      quit(1);
    }
    return elements[i];
  }
  private:
};

struct dS{
  long int j,k;
  double d;
};

/*overload the > operator*/
bool operator > (const dS& dS1, const dS& dS2){
  return dS1.d < dS2.d; //dS1.d < dS2.d is a MAX-HEAP implementation.
}
typedef vector<dS> dSVect;
typedef greater<dS> dSCompare; //priority_queue<dS, dSVect, dSCompare> pq;

void s01( float *d, long int n, double * sd){
  double fmin = (double)(std::numeric_limits<float>::max());
  double fmax = (double)(std::numeric_limits<float>::min());
  long int i;
  double t,r;
  for0(i,n){
    t = (double)(d[i]);
    if( (!isinf(t))&&(!isnan(t))){
      if(t<fmin) fmin=t; if(t>fmax) fmax=t;
    }
  }
  r = fmax-fmin;
  if( isinf(fmax)|| isnan(fmax) || isinf(fmin) || isnan(fmin) || (r<0.) || isinf(r) || isnan(r)){
    cout <<"Error: scaling band failed. Could not calculate maximum or minimum\n"; exit(1);
  }
  for0(i,n){
    t = (double)(d[i]);
    sd[i] = (t-fmin)/r;
  }
}

/*read file, checking number of bytes*/
int sread( int n, ifstream * i,std::ostringstream * fn, void * f, int halt){
  i->open((fn->str()).c_str(), ios::binary); /*open file in binary mode*/
  if( !(i->is_open())){
    return (-1);
  }
  i->seekg(0,ios::end); /*check number of bytes in file*/
  int len = i->tellg();
  i->seekg(0, ios::beg);
  if((len!=n) && (halt)){
    /*check if the number of bytes in the file was correct*/
    cout<< "Error: file \""<< fn <<"\" had "<<len<<" bytes; expected "<< n <<" bytes.";
    quit(1);
  }
  if(len!=n){
    return(len);
  }
  i->read( (char * ) f,len);
  return(len);
}

/*memory alloc. fail message*/
void nomem(){
  cout<<"Error: failed to allocate memory.\n"; quit(1);
}

/*safe memory alloc.*/
void * fmalloc( long int n){
  void * f = (void *) malloc(n);
  memset(f,'\0',n);
  if(!f) nomem();
  return(f);
}
/*allocate array of floats*/
float * newf32(long int n){
  float * f = (float *) fmalloc(n*sizeof(float));
  return(f);
}

/*allocate n-dim'l float array*/
float ** newf32p(long int n){
  float ** f = (float **) fmalloc(n*sizeof(float *));
  return(f);
}

/*allocate array of floats*/
double * newd32(long int n){
  double * f = (double *) fmalloc(n*sizeof(double));
  return(f);
}

/*allocate n-dim'l float array*/
double ** newd32p(long n){
  double ** f = (double **) fmalloc(n*sizeof(double *));
  return(f);
}