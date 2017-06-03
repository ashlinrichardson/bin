#include<time.h>
#include<stdio.h>
#include"fl0w3r.h"
#include<stdlib.h>
#include<string.h>
#include"ansicolor.h"

void write_envi_hdr(char * fn, int nrow, int ncol, int nband, int datatype){
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  FILE * f = fopen(fn, "w");
  if(!f){
    error((std::string("Error: could not open file: ") + std::string(fn)).c_str());
  }

  fprintf(f, "ENVI\n");
  //fprintf(f, "description = {%s}\n", strip(string(asctime(timeinfo))).c_str());
  fprintf(f, "samples = %d\n", ncol);
  fprintf(f, "lines   = %d\n", nrow);
  fprintf(f, "bands   = %d\n", nband);
  fprintf(f, "header offset = 0\n");
  fprintf(f, "file type = ENVI Standard\n");
  fprintf(f, "data type = %d\n", datatype);
  fprintf(f, "interleave = bsq\n");
  //fprintf(f, "sensor type = Unknown\n");  
  fprintf(f, "byte order = 0\n");
  //fprintf(f, "wavelength units = Unknown\n");
  fclose(f);
  wprint(fn);
}

void write_envi_hdr(char * fn, int nrow, int ncol, int nband){
  write_envi_hdr(fn, nrow, ncol, nband, 4);
}

void write_envi_hdr(char * fn, int nrow, int ncol){
  write_envi_hdr(fn, nrow, ncol, 1);
}

