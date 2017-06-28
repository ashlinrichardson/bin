#include <stdio.h>
#include <stdlib.h>
/* think this program draws out the edges of segments
(given an integer map of segments, e.g., as output by a 
clustering or segmentation program.

need to document what the input/output formats are */

int main(int argc, char ** argv){

  if(argc < 5){
    printf("segmentEdges.c [infile] [NRow] [NCol] [outfile]\n");
    exit(1);
  }

  FILE * in_file = fopen(argv[1], "rb");
  if(!in_file){
    printf("Error: could not open file %s\n", argv[1]);
    exit(1);
  }

  long int NRow = atol(argv[2]);
  long int NCol = atol(argv[3]);
  printf("NRow %ld NCol %ld Infile: %s Outfile %s\n", NRow, NCol, argv[1], argv[4]);

  FILE * out_file = fopen(argv[4], "wb");
  if(!out_file){
    printf("Error: could not open output file: %s\n", argv[4]);
    fclose( in_file);
    exit(1);
  }

  float * dat = malloc(sizeof(float) * NRow * NCol);
  if(!dat){
    printf("Error: could not allocate memory.\n");
    fclose(in_file);
    fclose(out_file);
    exit(1);
  }
  memset(dat, '\0', sizeof(float) * NRow * NCol);
  long int nr = fread(dat, sizeof(float), NRow * NCol, in_file);
  if(nr != NRow * NCol){
    printf("Error: did not read the correct number of records.\n"); exit(1);
  }

  float * out = malloc(sizeof(float)*NRow*NCol);
  if(!out){
    printf("Error: could not allocate memory.\n"); fclose( in_file); fclose(out_file); exit(1);
  }
  memset(out, '\0', sizeof(float) * NRow * NCol);

  long int ci, i, j, di, dj, ni, nj;

  for(i = 0; i < NRow; i++){
    for(j = 0; j < NCol; j++){
      ci = i * NCol + j;
      out[ci] = 0.;
      long int currentClass = (long int)floor(dat[ci]);
      printf("i, j, class, %ld,%ld,%ld\n", i, j, currentClass);
      for( di = -1; di <= 1; di++){
        ni = i + di;
        if(ni < 0 || ni >= NRow){
          continue;
        }
        else{
          for(dj = -1; dj <= 1; dj++){
            nj = j + nj;
            if(nj < 0 || nj >= NCol){
              continue;
            }
            else{
              if((long int)floor(dat[ni * NCol + nj]) != (long int)floor(dat[ci])){
                out[ci] = 1.;
              }
            }
          }
        }
      }
    }
  }
  fwrite(out, sizeof(float), NRow * NCol, out_file);
  fclose(out_file);
  fclose(in_file);
  return 0;
}
