#pragma once
#include <iostream>
#include <ostream>
#include <vector>
#include "bin.h"

/* define indexes for each channel of information from T4 matrix */
#define r11 0
#define r12 1
#define i12 2
#define r13 3
#define i13 4
#define r14 5
#define i14 6
#define r22 7
#define r23 8
#define i23 9
#define r24 10
#define i24	11
#define r33 12
#define r34	13
#define i34	14
#define r44 15

#define nT4Files 16

using std::ostream;
using namespace std;
using std::vector;
using namespace _bin;

namespace _T4{
	class T4{
    public:
			float pixel[nT4Files];

      /* constructor */
			T4(int _type, char * _dir, int nr, int nc){
				NRow = nr;  
				NCol = nc;
				initT4( _type, _dir);
			}

			T4( int _type, char * _dir){
				initT4( _type, _dir);
			}

			void initT4(int _type, char * _dir){

				T = new Image[nT4Files];

				for (i=0; i<nT4Files; i++){
          pixel[i] = 0.0;
        }

				char file_name[100];
	    	char * filenames[nT4Files] = { "T11.bin", "T12_real.bin", "T12_imag.bin", "T13_real.bin", "T13_imag.bin", "T14_real.bin", "T14_imag.bin", "T22.bin", "T23_real.bin", "T23_imag.bin", "T24_real.bin", "T24_imag.bin", "T33.bin", "T34_real.bin", "T34_imag.bin", "T44.bin"};

				dir = _dir;

				if(_type ==INPUT){
					getT3_Image_Dimensions(dir, NRow, NCol);
				}
				type=_type;
				printf("NRow %i NCol %i\n", NRow, NCol);

				for (i=0; i<nT4Files; i++){
					sprintf(file_name, "%s%s", dir, filenames[i]);
					
          /* call this before initImage */
          T[i].setDimensions(NRow, NCol);
					T[i].initImage(_type, file_name);
					file_name[0]='\n';
				}
				return;
			}

			void setPixel(){
				for(i=0; i<nT4Files; i++){
					T[i].setPixel(pixel[i]);
          }
			}

			void getPixel(){
				for(i = 0; i < nT4Files; i++){
					pixel[i] = T[i].getPixel();
				}
			}

			void writeHeaders(){
			}

			~T4(){
				close();
			}

			void close(){
				for(i=0; i<nT4Files; i++){
					T[i].close();
        }
			}

			void getDimensions(int & _NRow, int & _NCol){
				_NRow=NRow;
				_NCol=NCol;
			}

			inline cf operator[](unsigned int subscript){

				switch(subscript){
				  case 11:  return cf(pixel[r11], 0.);  break;
					case 22:  return cf(pixel[r22], 0.);  break;
					case 33:  return cf(pixel[r33], 0.);  break;
					case 44:  return cf(pixel[r44], 0.);  break;
					case 12:  return cf(pixel[r12], pixel[i12]);  break;
					case 13:  return cf(pixel[r13], pixel[i13]);  break;
					case 14:  return cf(pixel[r14], pixel[i14]);  break;
					case 21:  return cf(pixel[r12], -pixel[i12]); break;
					case 23: 	return cf(pixel[r23],  pixel[i23]); break;
					case 24: 	return cf(pixel[r24],  pixel[i24]);	break;
			  	case 31: 	return cf(pixel[r13], -pixel[i13]); break;
					case 32: 	return cf(pixel[r23], -pixel[i23]); break;
					case 34: 	return cf(pixel[r34],  pixel[i34]); break;
					case 41:	return cf(pixel[r14], -pixel[i14]); break;
					case 42: 	return cf(pixel[r24], -pixel[i24]); break;
					case 43: 	return cf(pixel[r34], -pixel[r34]); break;
					default:	printf("Error: invalid subscript on T4 matrix\n");  exit(1); break;
				}
			}

		protected:
		int type, NRow, NCol, i;
		char * dir;
		Image * T;
	};
};
