/* boxcar.cpp: revisioned 20170609. need to add header reading/writing to this one */
#include <stdlib.h>
#include <memory.h>
#include "fl0w3r.h"
#include <stdio.h>

int main(int argc, char ** argv){
  if(argc < 6){
    error("usage: boxcar [infile] [nrow] [ncol] [nwind] [outfile]\n");
  }
  
  int samples, lines;
  char * infile_name = argv[1];
  samples = atoi(argv[3]);
  lines = atoi(argv[2]);
  char * outfile_name = argv[5];
  int window_size = atoi(argv[4]);
  
  printf("Window size: %i\n", window_size);
  if((window_size % 2) != 1){
    error("window size must be an odd number.\n");
  }
  
  float * dat = f32(lines * samples);
  float * out = f32(lines * samples);
  
  FILE * infile = open(infile_name);
  FILE * outfile = wopen(outfile_name);
  
  int x, y;
  register int dx, dy;
  register int ind, wind;
  
  int mi = samples * lines;
  int ix = window_size / 2;
  float npix;
  fread(dat, lines * samples, sizeof(float), infile);
  for0(x, lines){
    for0(y, samples){
      ind = (x * samples) + y ; //calculate index of current pixel
      out[ind] = 0.; //clear the output value for the current pixel (x,y).
      npix = 0;
      
      /* for each pixel within the filter window around (x,y): */
      for(dx = (x - ix); dx <= (x +ix); dx++){
        for(dy = (y - ix); dy <= (y + ix); dy++){
          wind = (dx * samples) + dy;
          
          /* Make sure the pixel is not masked, and then check if the pixel is in bounds */
          if((dx >= 0) && (dy >= 0) && (dx < lines) && (dy < samples)){
            
            /* pixel under consideration is "good" and can be used for averaging*/
            npix++;
            
            /* add the value of the pixel to the running total */
            out[ind] = out[ind] + dat[ wind];
          }
        }
      }
      if(npix > 0.){
        out[ind]= out[ind] / ((float)npix);
      }
      else{
        /* if no unmasked filter window pixels, leave the pixel unchanged */
        out[ind] = dat[ind];
      }
    }
  }
  fwrite(out, lines * samples, sizeof(float), outfile);
  fclose(infile);
  fclose(outfile);
  return 0;
}
