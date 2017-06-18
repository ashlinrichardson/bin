#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#define BIG_BUFFER (1024*1024)

/*! \brief Represents the data and functionality of one Band in a Band Sequential Image.
* Brief description continued.
*
* Detailed description goes here.
*/
template <class T> class Band{
  public:

  Band(FILE* file, long samples, long lines, long bands, long header_offset, int data_type, int byte_order ){
    setFileHandle(file);
    setSamples(samples);
    setLines(lines);
    setBands(bands);
    setHeaderOffset(header_offset);
    setDataType(data_type);
    setByteOrder(byte_order);
  }

  ~Band();

  inline void setFileHandle(FILE * file){
    /* set file handle to fetch BSQ data from */
    this->file=file;
    if(!file){
      printf("Error: bad file handle\n");
      exit(1);
    }
  }

  inline void setLines(long lines){
    /* number of lines in BSQ file */
    this->lines=lines;
  }

  inline void setSamples(long samples){
    /* number of samples in BSQ file */
    this->samples=samples;
  }

  inline void setBands(long bands){
    /* number of bands in BSQ file */
    this->bands=bands;
  }

  inline void setHeaderOffset(long header_offset){
    this->header_offset=header_offset;
  }

  inline void setFileType(char* file_type){
    /* set filetype (currently only "ENVI" supported) */
    this->file_type=file_type;
    if( (file_type[0]!='E')||(file_type[1]!='N')||(file_type[2]!='V')||(file_type[3]!='I')){
      printf("Error: only ENVI file type is currently supported\n");
    }
  }

  inline void setDataType(int data_type){
    /* set data type */
    this->data_type=data_type;
    if( !((data_type==3)||(data_type==4))){
      printf("Error: only data types 3 and 4 are supported\n");
      exit(1);
    }
  }

  inline void setInterleave(char * interleave){
    /* set interleave (BSQ supported) */
    this->interleave = interleave;

    if((interleave[0]!='b')||(interleave[1]!='s')||(interleave[2]!='q')){
      printf("Error: interleave must be in bsq format\n");
      exit(1);
    }
  }

  inline void setByteOrder(int byte_order){
    /* set byte order */
    if(!((byte_order==0)||(byte_order==1))){
      printf("Error: byte order must be set to 0 or 1\n");
      exit(1);
    }
    this->byte_order = byte_order;
  }

  inline void readBand(int band_index){
    /* read a band */
  }

  private:
  long lines;
  long samples;
  long bands;
  long header_offset;
  char* file_type;
  int data_type;
  char* interleave;
  int byte_order;

  /* array to hold a band */
  T ** data;

  /* array to hold an output band */
  T ** out_data;

  /* array to hold the data to write to an output file (of the same dimensions and data format) which is a mask representing which pixels have been changed by processing. 0 means that the given pixel was untouched, whereas 1 means the given pixel was touched. */
  T ** mask;

  /* The file handle of the image to be read. */
  FILE * file;

  /* The file handle of the output image. */
  FILE * out_file;
};