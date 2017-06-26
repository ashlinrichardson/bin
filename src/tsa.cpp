/* time series averaging. By Ash Richardson, 20140106 reimpl 20170626 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <algorithm>
using namespace std;
void quit(int n){
  exit(n);
}

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
      printf("Error: could not allocate %ld bytes.\n",S*((long int)(sizeof(T))) );
      exit(1);
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

void writeENVIHeader4(char * filename, int NRows, int NCols){
  int datatype = 4;
  char file_name[5000];
  sprintf(file_name, "%s%s", filename,".hdr");
  FILE * HdrFile = fopen(file_name, "w");
  fprintf(HdrFile, "ENVI\n");
  fprintf(HdrFile, "samples = %d\n", NCols);
  fprintf(HdrFile, "lines = %d\n", NRows);
  fprintf(HdrFile, "bands = 1\n");
  fprintf(HdrFile, "header offset = 0\n");
  fprintf(HdrFile, "file type = ENVI Standard\n");
  fprintf(HdrFile, "data type = %d\n",datatype);
  fprintf(HdrFile, "interleave = bsq\n");
  fprintf(HdrFile, "byte order = 0\n");
  fclose(HdrFile);
  printf("w %s\n",file_name);
}
void msg(){
  printf("Usage: tsa [nfiles] [nrow] [ncol] [file1] [file2] .. [file n] [minfile] [maxfile] [median file] [mad file] [avg. file] [stv. file]\n");
  exit(1);
}
float fabs(float x){
  if(x<0.) return -x;
  else return x;
}
int main(int argc, char ** argv){
  printf("argc %d\n", argc);
  if(argc < 4){
    msg();
  }
  int nFiles = atoi(argv[1]);
  long int nrow = atoi(argv[2]);
  long int ncol = atoi(argv[3]);
  long int nf = nrow*ncol;
  printf("nrow %ld ncol %ld nfloats %ld\n",nrow, ncol,nf);
  if( argc!= nFiles + 2 + 1 + 1 + 6){
    //me, nfiles,nrows+ncols, [nfiles files], [6 output files]
    msg();
  }
  SA<FILE *> fh(nFiles);
  long int i;
  for(i=0; i<nFiles; i++){
    fh[i] = fopen(argv[4 + i],"rb");
    if(!fh[i]){
      printf("Error: could not open file %s\n", argv[2 + i]);
      exit(1);
    }
  }
  SA< SA<float> * > d(nFiles);
  for(i=0; i<nFiles; i++){
    d[i] = new SA<float>(nrow*ncol);
    if(!d[i]){
      printf("Error: could not allocate memory.\n");
      exit(1);
    }
    long int br = fread( (d[i]->elements), sizeof(float), nrow*ncol, fh[i]);
    printf("read %ld from %s\n", br, argv[4+i]);
    if( br != (long int)(nrow*ncol)){
      printf("Error: read wrong number of records.\n"); exit(1);
    }
  }

  SA<float> min(nf);
  SA<float> max(nf);
  SA<float> med(nf);
  SA<float> mad(nf);
  SA<float> avg(nf);
  SA<float> std(nf);
  vector<float> WINDOW;
  long int j;
  SA<float > * dp;
  float minF, maxF, medF, madF, avgF, stdF, nS;
  float d1, d2, dv;
  printf("\n\r");
  for(j=0; j<nf; j++){
    if( j % (nf/100) ==10){
      printf("\r%ld\%", 1+ 100*(j+1)/nf);
    }
    minF = 0.; maxF = 0.; medF = 0.; madF = 0.; avgF = 0.; stdF = 0.; nS = 0.;
    WINDOW.clear();
    for(i=0; i<nFiles; i++){
      dp = d[i];
      dv = (*dp)[j];
      WINDOW.push_back(dv);
      nS+=1.;
      avgF +=dv;
      if(i==0){
        minF = dv; maxF = dv;
      }
      else{
        if(dv<minF) minF = dv;
        if(dv>maxF) maxF = dv;
      }
    }
    avgF = avgF/nS;
    medF = NAN;

    std::sort( WINDOW.begin(), WINDOW.end());
    int nItems = WINDOW.size();
    if( nItems >0){
      if(nItems % 2 ==0){
        //even
        if(nItems>=2){
          d1 = WINDOW[ (nItems/2)-1 ];
          d2 = WINDOW[ (nItems/2) ];
          medF = ((d1+d2)/2.);
        }
      }
      else{
        //odd
        medF = WINDOW[ (nItems-1)/2];
      }
    }
    min[j]= minF; max[j] =maxF; avg[j] = avgF; med[j] = medF;
  }

  //for each pixel
  //now, need mad and stdv.
  float stdev;
  float madev;
  for(j=0; j<nf; j++){
    if( j % (nf/100) ==10){
      printf("\r%ld\%", 1+ 100*(j+1)/nf);
    }
    stdF = 0.; madF = NAN; stdev =0; madev=0;
    avgF = avg[j]; medF = med[j];
    WINDOW.clear();

    for(i=0; i<nFiles; i++){
      dp = d[i];
      dv = (*dp)[j];
      stdev = (dv -avgF);
      stdF+=stdev*stdev;
      madev = fabs( dv - medF);
      WINDOW.push_back(madev);
    }
    stdF = stdF/((float)nFiles);
    int nItems = WINDOW.size();
    if(nItems >0){
      if(nItems % 2 ==0){
        if(nItems>=2){
          d1 = WINDOW[ (nItems/2)-1 ];
          d2 = WINDOW[ (nItems/2) ];
          madF = ((d1+d2)/2.);
        }
      }
      else{
        madF = WINDOW[ (nItems-1)/2];
      }
    }
    mad[j]=madF;
    std[j]=stdF;
  }
  printf("\n");

  SA< SA<float> *> outDat(6);
  outDat[0] = &min;
  outDat[1] = &max;
  outDat[2] = &med;
  outDat[3] = &mad;
  outDat[4] = &avg;
  outDat[5] = &std;
  for(i=0; i<6; i++){
    writeENVIHeader4(argv[4+nFiles+i], nrow, ncol);
    FILE * fh = fopen( argv[4+nFiles+i],"wb");
    if(!fh){
      printf("Error: could not open file %s\n", argv[4+nFiles+i]);
      exit(1);
    }
    long int br = fwrite( outDat[i]->elements, sizeof(float), nrow*ncol, fh);
    if(br!=nf){
      printf("Error: bytes written %ld was not %ld\n", br, nf);
      exit(1);
    }
    fclose(fh);
  }
  return(0);
}
