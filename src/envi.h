#include<time.h>
#include<stdio.h>
#include"fl0w3r.h"
#include<stdlib.h>
#ifdef __cplusplus
  #include<string>
#else
  #include<string.h>
#endif

#include"ansicolor.h"


void write_envi_hdr(const char * fn, int nrow, int ncol, int nband, int datatype){
  wprint(fn);
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  FILE * f = fopen(fn, "w");
  if(!f){
    #ifdef __cplusplus
      error((std::string("Error: could not open file: ") + std::string(fn)).c_str());
    #endif
  }

  fprintf(f, "ENVI\n");
  // fprintf(f, "description = {%s}\n", strip(string(asctime(timeinfo))).c_str());
  fprintf(f, "samples = %d\n", ncol);
  fprintf(f, "lines = %d\n", nrow);
  fprintf(f, "bands = %d\n", nband);
  fprintf(f, "header offset = 0\n");
  fprintf(f, "file type = ENVI Standard\n");
  fprintf(f, "data type = %d\n", datatype);
  fprintf(f, "interleave = bsq\n");
  fprintf(f, "byte order = 0\n");
  fclose(f);
}
#ifdef __cplusplus
/* wrap for default params */
void write_envi_hdr(const char * fn, int nrow, int ncol, int nband){
  write_envi_hdr(fn, nrow, ncol, nband, 4);
}

void write_envi_hdr(const char * fn, int nrow, int ncol){
  write_envi_hdr((const char *)fn, nrow, ncol, (int)1);
}
#endif


/* accept string filenames */
#ifdef __cplusplus
void write_envi_hdr(string fn, int nrow, int ncol, int nband, int datatype){
  write_envi_hdr(fn.c_str(), nrow, ncol, nband, datatype);
}
#endif

#ifdef __cplusplus
void write_envi_hdr(string fn, int nrow, int ncol, int nband){
  write_envi_hdr(fn.c_str(), nrow, ncol, nband);
}
#endif

#ifdef __cplusplus
void write_envi_hdr(string fn, int nrow, int ncol){
  write_envi_hdr(fn.c_str(), nrow, ncol);
}
#endif
