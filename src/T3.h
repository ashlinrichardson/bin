#pragma once
#include <iostream>
#include <ostream>
#include <vector>
#include "bin.h"

/* define indexes for each channel of information from T3 matrix */
#define  r11  0
#define  i12  1
#define  r12  2
#define  i13  3
#define  r13  4
#define  r22  5
#define  i23  6
#define  r23  7
#define  r33  8

using std::ostream;
using namespace std;
using std::vector;
using namespace _bin;

namespace _T3{
	class T3	{
	  public:

			float pixel[nT3Files];

			T3(int _type, char * _dir){
				T = new Image[nT3Files];
				for (i=0; i<nT3Files; i++) pixel[i]=0.0;

				char file_name[100];
			  char * filenames[nT3Files] = {"T11.bin", "T12_imag.bin", "T12_real.bin", "T13_imag.bin", "T13_real.bin", "T22.bin", "T23_imag.bin", "T23_real.bin", "T33.bin"};

				NRow = NCol = 0;
				dir = _dir;
				getT3_Image_Dimensions(dir, NRow, NCol);
				type=_type;
				printf("NRow %i NCol %i\n", NRow, NCol);

				for (i=0; i<nT3Files; i++){
					sprintf(file_name, "%s%s", dir, filenames[i]);
					T[i].setDimensions(NRow, NCol);  //must call this before initImage
					T[i].initImage(_type, file_name);
					file_name[0]='\n';
				}
				return;
			}

			void setPixel(){
				for(i=0; i<nT3Files; i++){
					T[i].setPixel(pixel[i]);
          }
			}

			void getPixel(){
				for(i=0; i<nT3Files; i++){
					pixel[i]=T[i].getPixel();
				}
			}

			void writeHeaders(){
			}

			~T3(){
			}

			void close(){
				for(i=0; i<nT3Files; i++){
					T[i].close();
					T[i].destroy();
				}
			}

			void getDimensions(int & _NRow, int & _NCol){
				_NRow=NRow;
				_NCol=NCol;

			}

			void rewind(){
				int NR, NC;
				for (i=0; i<nT3Files; i++){
					T[i].rewind();
				}
			}

		protected:
		char * dir;
		Image * T;
		int NRow, NCol, type, i;
	};
};

