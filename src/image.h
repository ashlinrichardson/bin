/* 20080627 mods 20080318, 20170610 */
#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include <math.h>
#include "file.h"

using std::ostream;
using namespace std;
using std::vector;

namespace _image{
  
  #define INPUT 0
  #define OUTPUT 1

  inline float * falloc(int size){
    float * p = (float *) malloc( size*sizeof(float));
    memset( p, '\0', size*sizeof(float));
    return p;
  }

  /* floating point generic binary image. */
  class Image{
    public:
    
    Image(){
    }

    ~Image(){
    }

    void setDimensions(int _NRow, int _NCol){
      NRow = _NRow; NCol=_NCol;
      pixels = NRow * NCol;
    }

    void alloc(){
      buf = (float*)malloc(NRow*NCol*sizeof(float) );
      if(!buf){
        printf("Malloc failed to allocate memory for a band\n");
        exit(1);
      }
      memset(buf, '\0', NRow*NCol*sizeof(float));
      printf("Finished alloc\n");
    }

    void initImage(int _type_input_or_output, char * _filename){
      type=_type_input_or_output; //Set type to INPUT or OUTPUT
      ENVItype=4; //This image is ENVI data type four (single precision floating point).
      filename = new char[2000]; //File name of this image.
      strcpy(filename, _filename);
      pixels_processed=0; //Clear the flag which counts the number of pixels processed so far.
      alloc();
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
        writeENVIHeader_(filename, ENVItype, NRow, NCol);
      }
      fclose(handle);
    }

    void destroy(){
      free(buf);
    }

    //Get a pixel from the image and store it into the pixel buffer.
    float getPixel(){
      tmp = buf[pixels_processed];
      ++pixels_processed;
      return tmp;
    }

    void rewind(){
      pixels_processed=0;
      close();
      open();
      pixels_processed=0;
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
  };
}
