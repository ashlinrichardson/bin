#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "radlib.h"

//the filter width field is only for optimizing speedup.
void kernelSmooth( SA<double> * data, double sigma, int filter_w, SA<double> * dataf){
  double smoothfact = sigma;
  int size = data->size();
  float interp;
  int ind1, ind2;
  float ef, eft;
  for(ind1=0; ind1<size; ind1++){
    interp=0.;
    ef=0.; eft=0.;
    for(ind2=(ind1-filter_w); ind2<(ind1+filter_w); ind2++){
      if( (ind2 >=0) && (ind2<size)){
        ef = ((float)ind1)- ((float)ind2);
        ef = exp(-smoothfact*ef*ef);
        eft +=ef;
        interp += (data->at(ind2))*ef;
      }
    }
    interp/=eft;
    (dataf->at(ind1)) = interp;
  }
}

void countNonBoundaryExtremePoints( int & nMin, int & nMax, SA<double> & HIST){
  nMin = 0;
  nMax = 0;
  int NBINS = HIST.size();
  register int i;
  for(i=1; i< NBINS-1; i++){

    if ( (HIST[i-1] < HIST[i]) && (HIST[i] > HIST[i+1])){
      if( (i > 0) && (i<(NBINS-1))){
        printf("hist Max: i=%d #=%d\n", i, HIST[i]);
        nMax++;
      }
    }
    if ( (HIST[i-1] > HIST[i]) && (HIST[i] < HIST[i+1])){
      if( (i > 0) && (i<(NBINS-1))){
        printf("hist Min: i=%d #=%d\n", i, HIST[i]);
        nMin++;
      }
    }

  }
}


void findNonBoundaryExtremePoints(SA<double> &HIST, SA<double> & MIN, SA<double> &MAX, SA<int> &mini, SA<int> &maxi ){
  int nMin = 0;
  int nMax = 0;
  int NBINS = HIST.size();
  register int i;
  for(i=1; i< NBINS-1; i++){

    if ( (HIST[i-1] < HIST[i]) && (HIST[i] > HIST[i+1])){
      if( (i > 0) && (i<(NBINS-1))){
        //printf("hist Max: i=%d #=%d\n", i, HIST[i]);
        maxi[nMax]=i;
        MAX[nMax++]=HIST[i];

      }
    }
    if ( (HIST[i-1] > HIST[i]) && (HIST[i] < HIST[i+1])){
      if( (i > 0) && (i<(NBINS-1))){
        //printf("hist Min: i=%d #=%d\n", i, HIST[i]);
        mini[nMin]=i;
        MIN[nMin++]=HIST[i];
      }
    }

  }
}



int main(int argc, char ** argv){

  if(argc < 6){
    printf("histf.cpp: Calculate histogram of binary file, filter or classify by discretizing data based on maxima/minima of histograms. By Ash Richardson, August 25, 2009; reimplemented 20170625\n");
    printf("\n\tUsage: histf [input file] [path to directory containing config.txt file] [Number of Histogram bins] [filter_factor (float)] [output file] [split_min 0/1] [split_max 0/1]\n");
    printf("\nNote: config.txt file must be present\n");
    exit(1);

  }

  int NBINS = atoi( argv[3] );
  double filter_factor = (double)atof( argv[4]);

  FILE *infile;
  FILE * outfile;

  int splitmin, splitmax;
  splitmin = splitmax = 0;

  if(argc >= 7){
    splitmin = atoi(argv[6]);
  }

  if(argc >= 8){    
    splitmax = atoi(argv[7]);
  }

  if((splitmin!=1) && (splitmax!=1)){
    printf("Defaulting to functionality: split on minimum.\n");
  }

  int NRow, NCol;
  register int i;

  if(!(infile = fopen(argv[1], "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }

  if(!(outfile = fopen(argv[5], "wb"))){
    printf("Could not open output file: %s\n",argv[1]);
    exit(1);
  }

  char * fn = argv[5];
  getT3_image_Dimensions(argv[2], NRow, NCol);
  SA<float> * dat = new SA<float>(NRow*NCol);
  float fmax, fmin;
  register float d;
  fmax = 0.;
  fmin = 0.;
  double total=0.;
  double total_squared_dif = 0.0;
  double dif=0.;

  int size = NRow * NCol;
  SA<int> HIST(NBINS);
  int IND = 0;
  int fr = 0;

  fr = fread( &((*dat)[0]), sizeof(float), size, infile);
  printf("Read %d bytes.\n", fr);

  fmax = FLT_MIN;
  fmin = FLT_MAX;
  int totaln = 0;

  printf("First pass... \n");
  for(i=0; i<size;i++){
    d = (*dat)[i];
    if(!(isinf(d) || isnan(d))){
      totaln++;
      total+=d;
      if(d<fmin) fmin=d;
      if(d>fmax) fmax=d;
    }
  }
  double avg = total / ((double)totaln); // ((double)(NRow*NCol));
  printf("Max %e\nMin %e\nMean %e\n", fmax, fmin, avg);
  printf("Second pass... \n");
  for(i=0; i<size; i++){
    d = (*dat)[i];
    if( isinf(d) || isnan(d)){
      //pass on nan or infinity.
    }
    else{

      dif = d-avg;
      total_squared_dif+= (dif*dif);
      IND = (int)(floor(((d - fmin)/abs(fmax-fmin)) * ((float)NBINS)));
      if(IND == NBINS){
        IND = IND-1;
      }
      //if( IND < HIST.size()){
        HIST[IND]++;
      //}
    }
  }
  printf("\r ");
  double stdev = total_squared_dif / ((double)totaln); //((double)(NRow*NCol));
  stdev = sqrt(stdev);

  printf("NBINS %d\n", NBINS);
  printf("Max %e\nMin %e\nMean %e\nStdv %e\n", fmax, fmin, avg, stdev);

  SA<double> HISTd(NBINS);
  SA<double> HISTdf(NBINS);
  for(i=0; i<NBINS; i++){
    HISTd[i] = (double)HIST[i];
  }

  int nMin_nofilter, nMax_nofilter;
  int nMin_filter, nMax_filter;

  countNonBoundaryExtremePoints( nMin_nofilter, nMax_nofilter, HISTd);
  kernelSmooth( &HISTd, filter_factor, (NBINS/5), &HISTdf);
  countNonBoundaryExtremePoints( nMin_filter, nMax_filter, HISTdf);

  printf("no filter: nMin %d nMax %d\n", nMin_nofilter, nMax_nofilter);
  printf("w./filter: nMin %d nMax %d\n", nMin_filter , nMax_filter );

  SA<double> dMin(nMin_filter);
  SA<double> dMax(nMax_filter);
  SA<int> iMin(nMin_filter);
  SA<int> iMax(nMax_filter);

  findNonBoundaryExtremePoints( HISTdf, dMin, dMax, iMin, iMax);

  printf("\n\n\n");

  double xval =0;

  for(i=0; i<nMin_filter; i++){
    xval = ((double)(iMin[i]) / ((double)NBINS)) * abs(fmax-fmin) + fmin;
    printf("Min bin: %d hval: %f (%e) xval: %f (%e)\n", iMin[i], (float)dMin[i], dMin[i], (float)xval, xval);
  }
  for(i=0; i<nMax_filter; i++){
    xval = ((double)(iMax[i]) / ((double)NBINS)) * abs(fmax-fmin) + fmin;
    printf("Max bin: %d hval: %f (%e) xval: %f (%e)\n", iMax[i], (float)dMax[i], dMax[i], (float)xval, xval);
  }

  int j = 0;

  int foundmax_below = 0;
  int foundmax_above = 0;

  SA<int> isplit(nMin_filter);
  int nsplit=0;

  for(i=0; i<nMin_filter; i++){
    foundmax_below =0;
    foundmax_above =0;

    for(j=0; j<nMax_filter; j++){
      if( iMax[j] < iMin[i] ){
        foundmax_below=1;
      }
      if( iMax[j] > iMin[i]){
        foundmax_above=1;
      }
    }
    if((foundmax_above==1) && (foundmax_below==1)){
      isplit[nsplit++]=i;
    }
  }

  printf("\n\n\nMinima to split upon (%d):\n", nsplit);

  SA<double> sv(nsplit);

  for(j=0; j<nsplit; j++){
    i=isplit[j];
    xval = ((((double)(iMin[i]))+0.5) / ((double)NBINS)) * abs(fmax-fmin) + fmin;
    sv[j] = xval;
    printf("Min bin: %d hval: %f (%e) xval: %f (%e)\n", iMin[i], (float)dMin[i], dMin[i], (float)xval, xval);
    //sv[j] = xval;
  }
  for(j=0; j<nsplit; j++){
    printf("Splitval %e\n", sv[j]);// = xval;
  }

  double splitval =0;
  double lastsplitval=0;

  int w=0;
  float wr = 0.;
  int wassplit = 0;

  printf("\nSplitting image...\n");
  for(i=0; i<size;i++) {
    d = (*dat)[i];
    if( isinf(d) || isnan(d)){
      //pass on nan or infinity.
    }
    else{
      lastsplitval=0.;
      IND = 0;
      wassplit = 0;

      for(j=0; j<nsplit; j++){
        splitval = sv[j];
        if( ( splitval > d ) && ( d >= lastsplitval) ){
          wr = (float)IND;
          wassplit = 1;
          break;
        }
        IND++;
        lastsplitval = splitval;
      }
      if(wassplit==0){
        wr = (float)nsplit;
      }

    }
    w+=fwrite( &wr, sizeof(float), 1, outfile);
  }

  printf("Bytes written: %d, should have been: %d\n", w, NRow*NCol);
  if( w==(NRow*NCol)){
    printf("Wrote the correct number of bytes.\n");
  }
  else{
    printf("Error: wrong number of bytes written.\n");

  }

  fclose(infile);
  fclose(outfile);
  writeENVIHeader(fn, NRow, NCol, 4);
  return 0;
}
