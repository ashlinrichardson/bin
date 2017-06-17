#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "envi.h"

using namespace std;

int main(int argc, char * argv[]) {
  if(argc!=9){
    printf("af4.cpp: Contextual filter for hyperspectral data: (box filtering without sampling from masked pixels).\n");
    printf("Author: A. Richardson (ashy@uvic.ca)\tDate: 200806, 200905, 201202, 201706.\n\n");
    printf("Choose (according to option [output for masked pixels (0/1)] whether to:\n");
    printf("\t0: leave the masked pixels unchanged or...\n");
    printf("\t1: assign a result to masked pixels (only unmasked pixels are sampled).\n\n");
    printf("- a log file that includes the summation of each band is printed.\n");
    printf("\nUsage: af4 [input file name] [samples] [lines] [bands] [mask file name] [output file name] [window size] [output for masked pixels 0/1]\n");
    printf("\nProgram inputs:\n");
    printf("\t\t1/ Input ENVI image (Must have: no header offset, data type 4, bsq interleave, byte order 0).\n");
    printf("\t\t2. Input Mask image (Must have: no header offset, data type 1, bsq interleave, byte order 0).\n");
    printf("\nProgram output:\n");
    printf("\t\t1. ENVI format output file (same format as input image)\n");
    
    printf("\n\n*** Note: need to test this program!!!!!!\n");
    exit(1);
  }
  
  if(!argv[1]){
    printf("No input file specified\n");
    exit(1);
  }
  printf("-------------------------------------------\n");
  printf("Input file: %s\n", argv[1]);
  printf("Output file: %s\n", argv[6]);
  printf("Mask file: %s\n", argv[5]);
  
  int samples, lines, bands;
  char * infile_name = argv[1];
  
  samples = atoi(argv[2]);
  lines = atoi(argv[3]);
  bands = atoi(argv[4]);
  
  char * maskfile_name = argv[5];
  char * outfile_name = argv[6];
  std::string outfile_file_names(outfile_name);
  
  int window_size = atoi(argv[7]);
  int assign_output_masked = atoi(argv[8]);
  if(!((assign_output_masked == 1) || (assign_output_masked == 0))){
    printf("Error: [output for masked pixels (0/1)] must be assigned a value 0 or 1.\n");
    exit(1);
  }
  
  printf("Window size: %i\n", window_size);
  
  if((window_size % 2) != 1){
    printf("Error: window size must be an odd number.\n");
    printf("-------------------------------------------\n");
    exit(1);
  }
  printf("-------------------------------------------\n");
  
  /* buffer for 1 band of input image */
  float * dat = f32(lines*samples);
  
  /* output buffer */
  float * out = f32(lines * samples);
  
  /* input mask */
  char * mask = c8(lines*samples);
  
  /* number of pixels within the window (on a per pixel basis) within the band */
  short int * npix = (short int * )malloc(lines * samples * sizeof(short int));
  memset(npix,'\0', lines*samples*sizeof(short int));
  if(!npix){
    printf("Error: Failed to allocate 1-band memory buffer #3\n");
  }
  
  /* Open the data, output, and possibly the mask file */
  FILE * infile = open(infile_name);
  FILE * maskfile = open(maskfile_name);
  FILE * npixl_file = wopen("npixl.bin");
  
  ofstream ofs;
  ofs.open(outfile_name, ios::binary | ios::trunc);
  
  /* read mask band */
  fread(mask, lines * samples * sizeof(char), 1, maskfile);
  fclose(maskfile);
  
  int band, x, y;
  register int dx, dy;
  register int ind, wind;
  
  int mi = samples * lines;
  int ix = window_size / 2;
  long int outbcount = 0;
  int pixbcount = 0;
  
  for0(band, bands){
    double total_in = 0.;
    double total_out = 0.;
    printf("processing band %d of %d\n", band + 1, bands);
    fread(dat, (size_t)sizeof(float), (size_t)(lines * samples), infile);
    
    /*for each pixel of coordinates (x,y) within the current band: */
    for0(x, lines){
      for0(y, samples){
        
        /* calculate index of current pixel */
        ind = (x * samples) + y ;
        total_in += (double)dat[ind];
        if((mask[ind] == '\0') && (!assign_output_masked)){
          
          /* don't filter pixels under the mask */
          out[ind] = dat[ind];
        }
        else{
          
          /* keep track of how many pixels within the filter window have been totaled*/
          npix[ind] = 0;
          
          /* clear output value for current pixel */
          out[ind] = 0.;
          
          /* for each pixel within the filter window around (x,y):*/
          for(dx = (x - ix); dx <= (x + ix); dx++){
            for(dy = (y - ix); dy <= (y + ix); dy++){
              
              /* index of current pixel within window around (x,y) */
              wind = (dx * samples) + dy;
              
              /* Make sure pixel not masked, then check if pixel in bounds.*/
              if((dx >= 0) && (dy >= 0) && (dx < lines) && (dy < samples) && (mask[wind] != '\0')){
                
                /*pixel under consideration is "good" and can be used for averaging.*/
                npix[ind]++;
                
                /* add value of pixel to running total */
                out[ind] = out[ind] + dat[wind];
              }
            }
          }
          if(npix[ind] > 0.){
            
            /* set output pixel to average of samples available within the window */
            out[ind] = out[ind] / ((float)npix[ind]);
          }
          else{
            
            /* if no unmasked filter window pixels, leave pixel unchanged */
            out[ind] = dat[ind];
          }
        }
        total_out += out[ind];
      }
    }
    ofs.write((char *)(void *)out, sizeof(float) * lines * samples);
  }
  pixbcount += fwrite(npix, (size_t)sizeof(short int), (size_t)(lines * samples), npixl_file);
  
  fclose(infile); ofs.close(); fclose(npixl_file);
  
  printf("\nCheck that the output file is %i bytes\n", (long)bands * (long)samples * (long)lines * ((long)sizeof(float)));
  
  return 0;
}