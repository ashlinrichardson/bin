//;***********************************************************************************************************
//;*
//;* bin.h
//;* \\squall\palsar\pos\RADLIB\bin.h
//;*
//;***********************************************************************************************************
//;*
//;* This header performs library type functions for generic binary file type data. See:
//;* http://squall/wiki/index.php/RADLIB
//;*
//;* This header supports generic float binary files and generic complex binary files.
//;*
//;* Status: R&D
//;*
//;* Last modification:June 27, 2008.
//;*
//;* Developer: Ashlin Richardson <ashricha@nrcan.gc.ca> <ashlin.richardson@gmail.com>
//;*
//;* Modified by A. Richardson on March 18th, 2008 for commenting. with revisions 20170510
//;*
//;*
//;***********************************************************************************************************

#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include <math.h>
//#include <complex>
#include "./file.h"
#include "matrix2.h"
using namespace _matrix2;

using std::ostream;
using namespace std;
using std::vector;

namespace _bin{

  #define INPUT 0
  #define OUTPUT 1
  #define nT3Files 9

  #define BSQ 0
  #define BIP 1
  #define BIL 2

  void writeENVIHeader_(char * filename, int datatype, int NRows, int NCols, char * INTERLEAVE, int nbands);
  void writeENVIHeader_(char * filename, int datatype, int NRows, int NCols);

  void displayParams(int nparams, char ** params, char * description ){
    printf("%s\n", description);
    printf("Usage:");
    int i;
    for(i=0; i<nparams; i++){
      printf("%s",params[i]);
    }
    printf("\n");

  }

  void writeENVIHeader(char * filename, int datatype, int NRows, int NCols, int INTERLEAVE, int nbands){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char bsq[5]="bsq\n";
    char bip[5]="bip\n";
    char bil[5]="bil\n";

    char * intl = NULL;
    switch(INTERLEAVE){
      case BSQ: intl = &bsq[0];
      break;
      case BIP: intl = &bip[0];
      break;
      case BIL: intl = &bil[0];
      break;

    }

    char file_name[1000];
    sprintf(file_name, "%s%s", filename,".hdr");
    FILE * HdrFile = fopen(file_name, "w");

    fprintf(HdrFile, "ENVI\n");
    fprintf(HdrFile, "description = {\n");
    fprintf(HdrFile, "\t%s\n",asctime (timeinfo));
    fprintf(HdrFile, "\n}\n");
    fprintf(HdrFile, "samples = %d\n", NCols);
    fprintf(HdrFile, "lines = %d\n", NRows);
    fprintf(HdrFile, "bands = %d\n", nbands);
    fprintf(HdrFile, "header offset = 0\n");
    fprintf(HdrFile, "file type = ENVI Standard\n");
    fprintf(HdrFile, "data type = %d\n",datatype);


    fprintf(HdrFile, "interleave = %s\n", intl);
    fprintf(HdrFile, "sensor type = Unknown\n");
    fprintf(HdrFile, "byte order = 0\n");
    fprintf(HdrFile, "wavelength units = Unknown\n");
    fclose(HdrFile);
    printf("w %s\n",file_name);
  }

  void writeENVIHeader(char * filename, int datatype, int NRows, int NCols){
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    char file_name[1000];
    sprintf(file_name, "%s%s", filename,".hdr");
    FILE * HdrFile = fopen(file_name, "w");

    fprintf(HdrFile, "ENVI\n");
    fprintf(HdrFile, "description = {\n");
    //fprintf(HdrFile, "\t%s\textract_channel_T3.c\n\tBefore: %s\n\tAfter: %s }\n",asctime (timeinfo), before, after);
    fprintf(HdrFile, "\t%s\n",asctime (timeinfo));
    fprintf(HdrFile, "\n}\n",asctime (timeinfo));
    fprintf(HdrFile, "samples = %d\n", NCols);
    fprintf(HdrFile, "lines = %d\n", NRows);
    fprintf(HdrFile, "bands = 1\n");
    fprintf(HdrFile, "header offset = 0\n");
    fprintf(HdrFile, "file type = ENVI Standard\n");
    fprintf(HdrFile, "data type = %d\n",datatype);
    fprintf(HdrFile, "interleave = bsq\n");
    fprintf(HdrFile, "sensor type = Unknown\n");
    fprintf(HdrFile, "byte order = 0\n");
    fprintf(HdrFile, "wavelength units = Unknown\n");
    fclose(HdrFile);
    printf("w %s\n",file_name);
  }

  inline float * falloc( int size ){

    float * p =(float *) malloc( size*sizeof(float));
    memset( p, '\0', size*sizeof(float));
    return p;

  }

  //This class represents a floating point generic binary image.
  class Image{

    public:
    Image(){
      return;
    }

    ~Image(){
      return;
    }

    //Set the image's dimensions
    void setDimensions(int _NRow, int _NCol){
      NRow = _NRow; NCol=_NCol;
      pixels=NRow*NCol;
    }

    void initImage(int _type_input_or_output, char * _filename){
      type=_type_input_or_output; //Set type to INPUT or OUTPUT
      ENVItype=4; //This image is ENVI data type four (single precision floating point).
      filename = new char[2000]; //File name of this image.
      strcpy(filename, _filename);
      printf(type?"w %s\n":"r %s\n",filename); //Indicate whether the file is being written or read.
      //NRow=0; NCol=0; //Set the dimensions of this image to 0x0.
      pixels_processed=0;
      //Clear the flag which counts the number of pixels processed so far.
      //printf("T init %s",filename);
      buf = NULL;
      buf = (float*)malloc( NRow*NCol*sizeof(float) );
      if(!buf){
        printf("Malloc failed to allocate memory for a band\n");
        exit(1);
      }

      memset( buf, '\0', NRow*NCol*sizeof(float)); //void * memset ( void * ptr, int value, size_t num );

      open();

      if(type==INPUT){
        fread(&buf[0], NRow*NCol*sizeof(float), 1, handle);
      }
    }

    //Get the image's dimensions
    void getDimensions(int & _NRow, int & _NCol){
      _NRow=NRow;
      _NCol=NCol;
    }

    //Open the file corresponding to this image.
    void open(){
      if(!(handle = fopen(filename, type?"wb":"rb"))){
        printf("Could not open file: %s\n",filename);
        exit(1);
      }
    }

    //Close the file corresponding to this image, writing an ENVI header.
    void close(){
      if(type==OUTPUT){
        fwrite(buf, NRow*NCol*sizeof(float), 1, handle);
        writeENVIHeader(filename, ENVItype, NRow, NCol);
      }

      fclose(handle);
    }

    void destroy(){
      free(buf);
    }

    void rewind(){
      close();
      open();
      pixels_processed=0;
    }

    //Get a pixel from the image and store it into the pixel buffer.
    float getPixel(){
      tmp = buf[pixels_processed];
      ++pixels_processed;
      return tmp;
    }

    // Take the value stored in the pixel buffer and write it to file.
    void setPixel(float data){
      buf[pixels_processed]=data ;
      ++pixels_processed;
      return;
    }

    void writeHeaders(){
      //headers are written when the file is closed.
    }

    protected:
    int type; //type is INPUT or OUTPUT.
    int ENVItype; //ENVI data type (4 for this object)
    char * filename; //Name of the image file.
    int NRow, NCol; //Dimensions of the image.
    int pixels_processed; //Count how many pixels have been processed so far.
    FILE * handle; //File handle for the image file.
    float tmp;
    int pixels;
    float * buf;
    private:
  };

  //This is the analogous class for complex valued binary files. See the above methods for comments as the functions are analogous (only difference is reading and writing two floats at once for the real and imaginary components).
  class Image_Complex{

    public:
    Image_Complex(){
      output=1;
    }

    void initImage(int _type_input_or_output, char * _filename){
      type=_type_input_or_output;
      ENVItype=6;
      filename = new char[1000];
      strcpy(filename, _filename);
      printf(type?"w %s\n":"r %s\n",filename);
      NRow=0; NCol=0;
      pixels_processed=0;
    }

    void supressOutput(){
      output = 0;
    }

    ~Image_Complex(){
    }

    void setDimensions(int _NRow, int _NCol){
      NRow = _NRow; NCol=_NCol;
      pixels=NRow*NCol;
    }

    void getDimensions(int & _NRow, int & _NCol){
      _NRow=NRow;
      _NCol=NCol;
    }

    void open(){
      if(!(handle = fopen(filename, type?"wb":"rb"))){
        printf(type?"Could not open output file: %s\n":"Could not open input file: %s\n",filename);
        exit(1);
      }
    }

    void close(){
      writeENVIHeader(filename, ENVItype, NRow, NCol);
      fclose(handle);
    }

    void getPixel(complex<float> & pix){
      if(type!=INPUT){
        printf("Error: not an output file %s\n",filename);
        exit(1);
      }
      if(pixels_processed <= pixels){
        if(!fread(&t[0], sizeof(float), 1, handle)) {
          printf("read failed\n");
          exit(1);

        }
        if(!fread(&t[1], sizeof(float), 1, handle)) {
          printf("read failed\n");
          exit(1);

        }
        pix.real(t[0]);
        pix.imag(t[1]);
        pixels_processed++;
        return;
      }
      printf("Error: tried to read too many pixels from file %s %d %d\n",filename,pixels_processed,pixels);
    }

    void setPixel(complex<float> & data){
      if(type!=OUTPUT){
        printf("Error: not an output file %s\n",filename);
        exit(1);
      }
      if(pixels_processed <= pixels){
        t[0]=real(data);//.real();
        t[1]=imag(data);//.imag();
        if(!fwrite(&t[0], sizeof(float), 2, handle)) {
          printf("read failed\n");
          exit(1);
        }
        pixels_processed++;
        return;
      }
      printf("Error: tried to write too many pixels from file %s %d %d\n",filename,pixels_processed,pixels);

    }

    void writeHeaders(){
    }

    protected:
    int type;
    int ENVItype;
    char * filename;
    int NRow, NCol;
    int pixels_processed;
    FILE * handle;
    float tmp, tmp2;
    float t[2];
    int pixels;
    int output;
  };
}