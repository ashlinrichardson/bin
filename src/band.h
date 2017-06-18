#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#define BIG_BUFFER (1024*1024)

template <class T>
/*! \brief Represents the data and functionality of one Band in a Band Sequential Image.
* Brief description continued.
*
* Detailed description goes here.
*/
class Band{

  public:
  /*! \brief A constructor
  * ...
  *
  * ...
  */
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

  /*! \brief Set the file handle from which BSQ data will be fetched.
  * ...
  *
  * ...
  */
  inline void setFileHandle(FILE * file){
    this->file=file;
    if(file){
      printf("Error: failed to open input file\n");
      exit(1);
    }
  }

  /*! \brief Set the number of Lines in the BSQ file.
  * ...
  *
  * ...
  */
  inline void setLines(long lines){
    this->lines=lines;
  }

  /*! \brief Set the number of Samples in the BSQ file.
  * ...
  *
  * ...
  */
  inline void setSamples(long samples){
    this->samples=samples;
  }

  /*! \brief Set the number of Bands in the BSQ file.
  * ...
  *
  * ...
  */
  inline void setBands(long bands){
    this->bands=bands;
  }
  inline void setHeaderOffset(long header_offset){
    this->header_offset=header_offset;
    //========================

    //========================
  }

  /*! \brief Set the type of file to be read.
  * ...
  *
  * Currently the only file type supported is "ENVI" file type.
  */
  inline void setFileType(char* file_type){
    this->file_type=file_type;
    if( (file_type[0]!='E')||(file_type[1]!='N')||(file_type[2]!='V')||(file_type[3]!='I')){
      printf("Error: only ENVI file type is currently supported\n");
    }
  }

  /*! \brief Set the type of data to be read.
  * ...
  *
  * The data types being implemented are float (4) and long (3).
  */
  inline void setDataType(int data_type){
    this->data_type=data_type;
    if( !((data_type==3)||(data_type==4))){
      printf("Error: only data types 3 and 4 are supported\n");
      exit(1);
    }
  }

  /*! \brief Set the type of interleave corresponding to the input file.
  * ...
  *
  * The only interleave type currently being implemented is "bsq".
  */
  inline void setInterleave(char * interleave){
    this->interleave = interleave;

    if((interleave[0]!='b')||(interleave[1]!='s')||(interleave[2]!='q')){
      printf("Error: interleave must be in bsq format\n");
      exit(1);
    }
  }

  /*! \brief Set the type of byte ordering corresponding to the input file.
  * ...
  *
  * Both big- and little- -endian formats are being implemented.
  */
  inline void setByteOrder(int byte_order){
    if(!((byte_order==0)||(byte_order==1))){
      printf("Error: byte order must be set to 0 or 1\n");
      exit(1);
    }
    this->byte_order = byte_order;
  }

  /*! \brief Read the block of data corresponding to a given, zero-indexed band.
  * ...
  *
  * The program is intended to read from an arbitrary band so that it is easy to implement a multi-threaded version of the app, with multiple Band objects declared, each representing the processing for a given band.
  */
  inline void readBand(int band_index){
    //========================

    //========================
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


  /*! \brief The array to hold the data from one band of the BSQ image.
  * ...
  *
  * Templates are used so that float, long, or other types may be easily used.
  */
  T ** data;

  /*! \brief The array to hold the data to write to one band of the output BSQ image.
  * ...
  *
  * ...
  */
  T ** out_data;

  /*! \brief An array to hold the data to write to an output file (of the same dimensions and data format) which is a mask representing which pixels have been changed by processing.
  * ...
  *
  * 0 means that the given pixel was untouched, whereas 1 means the given pixel was touched.
  */
  T ** mask;

  /*! \brief The file handle of the image to be read.
  * ...
  *
  *
  */
  FILE * file;

  /*! \brief The file handle of the output image.
  * ...
  *
  *
  */
  FILE * out_file;

};