/* March 18, 2008.. Mods 20170510 */
#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include "bin.h"

/* define indexes for each channel of information from S2 matrix */
#define s11 0
#define s12 1
#define s21 2
#define s22 3
#define nS2Files 4

using std::ostream;
using namespace std;
using std::vector;
using namespace _bin;

/* reading and writing for S2 data sets */
namespace _S2{

  class S2{

    public:

    int type; //Type is INPUT or OUTPUT
    char * dir; //Directory the data set resides within.
    Image_Complex * S; //Pointer to an array of channels (S11 ,S12, S21, S22)
    int NRow, NCol; //Dimensions of this image
    int dimensions_set; //Have the dimensions of this image been set yet?a
    int output, i;
    complex<float> pixel[nS2Files]; //Buffer pixel variable.
    char * filenames[nS2Files];

    S2(int _type, char * _dir){
      char * fn[nS2Files] = {
        "s11.bin",
        "s12.bin",
        "s21.bin",
        "s22.bin"
      };
      init_S2(_type, _dir, fn);
    }

    S2(int _type, char * _dir, char * fn[]){
      init_S2(_type, _dir, fn);
    }

    void init_S2(int _type, char * _dir, char * fn[]){
      setFilenames(fn);
      output = 1;
      int dimensions_set = 0; //note whether the dimensions of the image have been set or not.
      S = new Image_Complex[nS2Files]; //initialize a complex image object for each S2 channel.

      for(i = 0; i < nS2Files; i++){
        /* set to zero */
        (pixel[i]).zero();
      }
      //File names for the S2 data channels.
      char file_name[1000];
      NRow = NCol = 0; //Default dimensions are 0x0.
      dir = _dir; //new char[1000]; //Hold the directory in which the S2 files are situated.
      //If it is an INPUT data set (as opposed to an OUTPUT data set) fetch the image's dimensions.
      type = _type;

      if(type == INPUT){
        getT3_Image_Dimensions(dir, NRow, NCol);
        setDimensions(NRow, NCol);
      }

      for(i=0; i<nS2Files; i++){
        //Open the corresponding files in the input directory.
        sprintf(file_name, "%s%s", dir, filenames[i]);
        S[i].initImage(_type, file_name);
        //For each channel, set the dimensions.
        if(type == INPUT){
          S[i].setDimensions(NRow, NCol);
        }
        file_name[0]='\n';
        S[i].open(); //Open the file corresponding to that channel.
      }
      return;
    }

    void setFilenames(char * fn[]){
      int i;
      for(i=0; i<4; i++) filenames[i] = fn[i];
    }

    void setPixel(){
      //Store the buffer pixel to file.
      for(i = 0; i < nS2Files; i++){
        S[i].setPixel(pixel[i]);
      }
    }

    void setPixel(complex<float> _S11_, complex<float> _S12_, complex<float> _S21_, complex<float> _S22_){
      S[0].setPixel(_S11_);
      S[1].setPixel(_S12_);
      S[2].setPixel(_S21_);
      S[3].setPixel(_S22_);
    }

    void getPixel(){
      //Fetch a pixel from the file, into the buffer pixel.
      if(!dimensions_set){
        printf("S2::getPixel(): error: dimensions not yet set\n");
        exit(1);
      }
      for(i=0; i<nS2Files; i++){
        S[i].getPixel(pixel[i]);
      }
    }

    void writeHeaders(){
      //Headers are currently written on file close. See close() method below.
    }

    void supressOutput(){
      output=0;
    }

    ~S2(){
    }

    void close(){
      //Close the S2 file object by closing the individual channels (complex images)
      for(i = 0; i < nS2Files; i++){
        S[i].close();
      }
    }

    //Get the dimensions of an S2 image (from the variables which record dimension.) For an input data set, these were set when the object was initialized.
    void getDimensions(int & _NRow, int & _NCol){
      if(!dimensions_set){
        printf("S2::getDimensions: error: dimensions not yet set\n");
        exit(1);
      }
      _NRow=NRow;
      _NCol=NCol;
    }

    //Initialize the dimensions of the S2 image represented by this object.
    void setDimensions(int _NRow, int _NCol){
      dimensions_set=1;
      NRow=_NRow;
      NCol=_NCol;
      //Initialize the dimensions for each constituent channel.
      for(i=0; i<nS2Files; i++){
        S[i].setDimensions(NRow, NCol);
      }
    }
  };
};
