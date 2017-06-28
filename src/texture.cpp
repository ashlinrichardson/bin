#include "envi.h"
#include <string>
#include <time.h>
#include <vector>
#include <math.h>
#include <stdio.h>
#include "radlib.h"
#include <stdlib.h>

int main(int argc, char ** argv){

  int NBINS;

  if(argc != 6){
    printf("texture.cpp: texture parameters from binary single-precision floating point file. By A. Richardson, 20100129 revised 20170628\n");
    printf("\n\tUsage: texture [input file] [path to directory containing config.txt file] [output file] [window size] [parameter]\n");
    printf("Parameter choices:\n0: median\n1: standard deviation\n");
    printf("2: sample kurtosis http://en.wikipedia.org/wiki/Kurtosis#Sample_kurtosis\n");
    printf("3: sample skewness http://en.wikipedia.org/wiki/Skewness#Sample_skewness\n");
    printf("4: geometric mean http://en.wikipedia.org/wiki/Geometric_mean\n");
    printf("5: geometric standard deviation http://en.wikipedia.org/wiki/Geometric_standard_deviation\n");
    printf("6: harmonic mean http://en.wikipedia.org/wiki/Weighted_harmonic_mean\n");
    printf("7: sample 2nd moment http://en.wikipedia.org/wiki/Moment_(mathematics)\n");
    printf("8: sample 3rd moment http://en.wikipedia.org/wiki/Moment_(mathematics)\n");
    printf("9: sample 4th moment http://en.wikipedia.org/wiki/Moment_(mathematics)\n");
    printf("10: entropy\n11: min\n12: max\n13: absentropy\n");
    exit(1);
  }

  FILE *infile;
  FILE * outfile;
  char * infilename = argv[1];
  char * configpath = argv[2];
  char * outfilename = argv[3];
  int windowsize = atoi( argv[4]);
  int parameter = atoi( argv[5]);

  int NRow, NCol;

  int i, j;
  if(!(infile = fopen(infilename, "rb"))){
    printf("Could not open input file: %s\n",argv[1]);
    exit(1);
  }
  if(!(outfile = fopen(outfilename, "wb"))){
    printf("Could not open output file: %s\n",argv[4]);
    exit(1);
  }

  getT3_image_Dimensions(configpath, NRow, NCol);

  if((windowsize - 1 < 2) || (((windowsize - 1) % 2) != 0)){
    printf("Error: incorrect window size. Must be an odd number.\n");
    exit(1);
  }

  int dw = (windowsize - 1) / 2;
  SA<float> * dat = new SA<float>(NRow * NCol);
  float fmax, fmin;
  register float d;
  fmax = 0.;
  fmin = 0.;

  int size = NRow * NCol;
  SA<int> HIST(NBINS);
  int IND =0;
  fread( &(*dat)[0], sizeof(float), size, infile);
  int totaln = 0;
  int row, col;
  int maxi = 0;
  float maxval = 0;
  int ind = 0;
  int k;
  float out;
  vector<float> WINDOW;
  double stddev, total;
  double fh = 0;

  for(row=0; row<NRow; row++){
    printf("\rProcessing row %d of %d", row+1, NRow);

    for(col=0; col<NCol; col++){
      totaln = 0;
      total = 0.;
      fmax = FLT_MIN;
      fmin = FLT_MAX;
      HIST.clear();
      WINDOW.clear();
      stddev = 0.;

      for(i = row - dw; i <= row + dw; i++){
        for(j = col - dw; j < col + dw; j++){
          if((i >= 0) && (j >= 0) && (i < NRow) && (j < NCol)){

            /* pixel in range */
            ind = i * NCol + j;
            d = (*dat)[ind];
            if(!(isinf(d) || isnan(d))){
              WINDOW.push_back(d);
            }
          }
        }
      }

      std::sort(WINDOW.begin(), WINDOW.end());
      double entropy = 0.;
      double meanabs = 0.;

      switch(parameter){

        case 0:
          out = WINDOW.at(max(0, (int)((WINDOW.size() / 2) - 1)));
          break;

        case 1:
          for(k = 0; k < WINDOW.size(); k++){
            total += WINDOW[k];
          }
          total = total / ((double)WINDOW.size());

          for(k = 0; k < WINDOW.size(); k++){
            stddev += (WINDOW[k] - total) * (WINDOW[k] - total);
          }
          out = sqrt(stddev);
          break;

        case 11:
          for(k = 0; k < WINDOW.size(); k++){
            if(WINDOW[k] < fmin)
            fmin = WINDOW[k];
          }
          out = fmin;
          break;

        case 12:
          for(k = 0; k < WINDOW.size(); k++){
            if(WINDOW[k] > fmax)
            fmax = WINDOW[k];
          }
          out = fmax;
          break;
        
        case 13:
          for(k = 0; k < WINDOW.size(); k++){
            meanabs += WINDOW[k] * WINDOW[k];
          }
          meanabs /= ((double)WINDOW.size());
          for(k = 0; k < WINDOW.size(); k++){
            entropy += (WINDOW[k] * WINDOW[k]) * log (WINDOW[k] * WINDOW[k] / meanabs);
          }
          out = entropy;
          break;

        default:
          printf("Error: parameter choice not defined\n");
          exit(1);
          break;
      }
      fwrite(&out, sizeof(float), 1, outfile);
    }
  }
  
  /* close file handles */
  fclose(infile);
  fclose(outfile);

  /* write header file */
  write_envi_hdr(outfilename, NRow, NCol);
  write_envi_hdr(infilename, NRow, NCol);

  return 0;
}
