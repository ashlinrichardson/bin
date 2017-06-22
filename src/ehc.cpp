#include "radlib.h"
#include <list>
using namespace std;
using namespace _bin;

inline long int bsq(long int row, long int col, long int band, long int nrow, long int ncol, long int nband){
  return band * (nrow * ncol) + row * (ncol) + col;
}

inline long int bip(long int row, long int col, long int band, long int nrow, long int ncol, long int nband){
  return row * (ncol * nband) + col * (nband) + band;
}

int NBand, ii;
float ret, val;

inline float dist(float * x, float * y){
  //int size = x.size();
  ret = 0;
  for(ii = 0; ii < NBand; ii++){
    val = x[ii] - y[ii];
    ret += val * val;
  }
  return ret; //sqrt(ret);
}

float * dat;

int main(int argc, char ** argv){

  if(argc < 5){
    printf("ec.cpp: Euclidean hierarchical clust. A. Richardson, Nov. 12, 2009 reimpl: 20170622\n");
    printf("data must be 32-bit floating point.\n");
    printf("Usage: ehc [nsamples] [nlines] [nbands] [data.bin]\n");
    printf("currently BIP interleave is supported? \n");
    exit(1);
  }

  int i, j, k;
  int NCol = atoi(argv[1]);
  int NRow = atoi(argv[2]);
  NBand = atoi(argv[3]);
  char * infilename = argv[4];
  int nfloats = NRow * NCol * NBand;
  int N = NRow * NCol;

  if(INT_MAX < nfloats){
    printf("Error: image dimensions are too big to be indexed by type int. Try using long int.\n");
    exit(1);
  }

  SA<float> data(nfloats);
  SA<float> _d(N);
  float * d = &_d[0];
  SA<int> _to(N);
  int * to = &_to[0];
  SA<int> Vunused(N);
  SA<int> _Vnew(N);
  int * Vnew = &_Vnew[0];

  int nVunused = N - 1;
  int nVnew = 1;
  Vnew[0] = 0;
  for(i = 0; i < N; i++){
    Vunused[i] = i;
    d[i] = -1.;
    to[i] = -1;
  }
  int nVold = nVunused;
  int * Vold = &Vunused[1];
  dat = &data[0];

  FILE * infile = fopen(infilename, "rb");
  if(!infile){
    free(dat);
    printf("Error: failed to open input file: %s\n", infilename);
    exit(1);
  }
  else{
  }

  printf("Reading file into RAM...\n");
  fread( dat, sizeof(float)*nfloats, 1, infile);
  printf("Finished reading file...\n");

  float x[NBand];
  float y[NBand];

  float D;
  int mini, minj, minVj, minVi;
  mini = minj = minVj = minVi = 0;

  float mind;
  // until all nodes are added to tree, add a node.
  while(nVnew < N){
    printf("nVnew %d nVold %d mini %d minj %d\n", nVnew, nVold, mini, minj);

    // find the nearest un-added node, to the tree.
    for(k = 0; k < NBand; k++){
      x[k] = dat[(Vnew[0]) * NBand + k];
      y[k] = dat[ (Vold[0]) * NBand + k];
    }
    mind = dist(x,y);
    mini = Vnew[0];
    minj = Vold[0];
    minVj = 0;
    minVi = 0;

    for(i = 0; i < nVnew; i++){
      for(k = 0; k < NBand; k++){
        x[k] = dat[ Vnew[i]*NBand + k];
      }
      for(j = 0; j < nVold; j++){
        for(k = 0; k < NBand; k++){
          y[k] = dat[ Vold[j]*NBand + k];
        }
        D = dist(x,y);
        if(D < mind){
          mind = D;
          mini = Vnew[i];
          minj = Vold[j];
          minVi = i;
          minVj = j;
        }
      }
    }

    Vnew[nVnew++] = minj; //add (vertex closest to tree), to the tree.
    to[minj] = mini;
    d[minj] = mind;

    //shift over the list of unused vertices (deleting the vertex added to the tree, from the list to be added).
    for(i = minVj; i < (nVold - 1); i++){
      Vold[i] = Vold[i + 1];
    }
    nVold--;
  }

  printf("nVnew %d nVold %d\n", nVnew, nVold);
  printf("\r \ndone\n");
  printf("Coords:\n");
  for(i = 0; i < N; i++){
    for(k = 0; k < NBand; k++){
      printf("%f,", dat[i * NBand + k]);
    }
    printf("\n");
  }

  printf("Segs:\n");
  for(i = 0; i < N; i++){
    if(to[i] >= 0){
      for(k = 0; k < NBand; k++){
        printf("%f,", dat[i* NBand + k]);
      }
      for(k = 0; k < NBand; k++){
        printf("%f,", dat[(to[i]) * NBand + k]);
      }
      printf("\n");
    }
  }

  fclose(infile);
}