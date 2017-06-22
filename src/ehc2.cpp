/* the requirement for the 3-decomp is the option of split left, split right, and nosplit? until resolved, use middle nbhd. */
#include"radlib.h"
#include<stack>
#include<list>

using namespace std;
using namespace _bin;

#define MAXBANDS 5
#define MAXLEVEL 15

struct nbhd{
  SA<int> * points;
  //SA<nbhd *> * children;
  nbhd * parent;
};

int nMin;
int p2[MAXLEVEL];
int pow(int x, int exp){
  if(exp == 0){
    return 1;
  }
  else{
    return x*pow(x,exp-1);
  }
}

nbhd * partition(nbhd nbd, int level);
list <nbhd*> fine_partitions;
stack<int> tmp;

int NBand, ii, n, li, nextlevel;
int id[MAXLEVEL][MAXBANDS];

float ret, val;
float X[MAXBANDS];

inline float dist(float * x, float * y){
  ret = 0;
  for(ii =0; ii< NBand; ii++){
    val = x[ii] - y[ii];
    /*if(isinf(val) || isnan(val)){*/
    ret += val * val;
  }
  return ret;
}

float * dat;

int main(int argc, char ** argv){

  for(li = 0; li < MAXLEVEL; li++){
    p2[li] = (int)pow((int)2, (int)li);
    printf("%d\n", p2[li]);
  }

  for(li = 0; li < 10; li++){
    printf("li p2[l1] %d\n", li & (p2[0]));
  }

  if(argc < 6){
    printf("ehc2.cpp: Euclidean hierarchical Spatial Partitioning. By A. Richardson, Nov. 12, 2009; reimpl. 20170622\n");
    printf("This module, along with ehc.cpp (Euclidean minimal spanning tree, could help solve a hierarchical classification problem for large data sets (millions of pixels).\n");
    printf("Warning: data must be type 32-bit floating point.\n");
    printf("Usage: ehc [nsamples] [nlines] [nbands] [data.bin] [NMIN - space partitioning stop threshold]\n");
    printf("currently only BIP interleave is supported?\n");
    exit(1);
  }

  int i,j,k;
  int NCol = atoi(argv[1]);
  int NRow = atoi(argv[2]);
  NBand = atoi(argv[3]);
  if(NBand > MAXBANDS){
    printf("Error: too many bands.\n");
    exit(1);
  }
  char * infilename = argv[4];
  nMin = atoi(argv[5]);

  int nFloats = NRow*NCol*NBand;
  int N = NRow*NCol;

  if(INT_MAX < nFloats){
    printf("Error: image dimensions are too big to be indexed by type int. Try using long int.\n");
    exit(1);
  }

  SA<float> data(nFloats);
  dat = &data[0];

  FILE * infile = fopen(infilename, "rb");
  if(!infile){
    printf("Error: failed to open input file: %s\n", infilename);
    exit(1);
  }

  printf("Reading file into RAM...\n");
  fread( dat, sizeof(float)*nFloats, 1, infile);
  printf("Finished reading file into RAM...\n");

  //hold current pixel coordinates (two points in n-dimensional space (where n is the number of bands).
  float x[NBand];
  float y[NBand];

  //in the hierarchical space partitioning, keep a link to the smallest neighborhood that contains a pixel.
  void * minNbd[NRow][NCol][NBand];

  // x[k] = dat[ (Vnew[0])*NBand + k];
  nbhd top; // = (nbhd *)malloc(sizeof(nbhd));
  top.points = new SA<int>(N);
  top.parent=NULL;
  for(i = 0; i < N; i++){
    top.points->at(i) = i;
  }
  //SA<int> topindex(NBand);
  for(i = 0; i < NBand; i++){
    id[0][i] = false; //topindex[i]=1;
  }
  partition(top, 0);
}

int nextperm(int cur, int * val){
  //if( val[cur] ==1)a
}

nbhd * partition(nbhd nbd, int level){
  n = nbd.points->size();
  //if we have reached a neighborhood containing the minimum number of points, don't partition further.
  if(n <= nMin){
    return NULL;
  }

  nextlevel = level + 1;
  int cur = 0;
  int mypow = p2[NBand];
  float h = 1. / ((float)p2[nextlevel]);
  int ti = 0;
  int pixelin = false;
  float XX;

  //for the 2^d permutations of left/right splits in each dimension:
  for(cur = 0; cur < mypow; cur++){
    for(li = 0; li < NBand; li++){
      id[nextlevel][li] = ((cur & ((p2[li])) == 0));
    }
    //go through all pixels in node
    if(tmp.size() > 0){
      printf("Error: this should not be able to happen.\n");
      exit(1);
    }
    for(li = 0; li < n; li++){
      //for each pixel in node
      pixelin = true;
      ti = nbd.points->at(li);
      for(int k = 0; k < NBand; k++){
        XX = dat[ti * NBand + k]; 
        //check that this pixel dimension is in range
      }
    }
  }
}
