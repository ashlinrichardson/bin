/* we get the matrix3.h and so on, errors for this one: need to work on this one more. 

compare the different versions of matrix3.h, matrix4.h, and so on..  */
#include "SA.h"
#include "T3.h"
#include <math.h>
using namespace _SA;
using namespace _T3;

#define T11R (T3_in.pixel[r11])
#define T12I (T3_in.pixel[i12])
#define T12R (T3_in.pixel[r12])
#define T13I (T3_in.pixel[i13])
#define T13R (T3_in.pixel[r13])
#define T22R (T3_in.pixel[r22])
#define T23I (T3_in.pixel[i23])
#define T23R (T3_in.pixel[r23])
#define T33R (T3_in.pixel[r33])

#define STR_MAX 2048
int main(int argc, char ** argv){
  if(argc < 3){
    printf("p3c.exe [t3 input dir] [t3 output dir]\n");
    exit(1);
  }

  char input_dir[STR_MAX];
  char output_dir[STR_MAX];
  strcpy(input_dir, argv[1]);
  strcpy(output_dir, argv[2]);

  int i,j, Row,Col;
  i = j = Row = Col = 0;
  T3 T3_in(INPUT, input_dir);
  T3 T3_out(OUTPUT, output_dir);

  int NRow, NCol;
  int NRow_o, NCol_o;

  T3_in.getDimensions(NRow, NCol);
  T3_out.getDimensions(NRow_o, NCol_o);

  if((NRow!=NRow_o)||(NCol!=NCol_o)){
    printf("Input and output image sizes don't match\n");
    exit(1);

  }

  float theta, s2t, c2t, s4t, c4t, s2t2, c2t2;
  float ct, st, ct_2, st_2;

  FILE * theta_file = fopen("theta","w");
  FILE * eta_file = fopen("eta","w");

  for(Row = 0; Row < NRow; Row++){
    for(i = 0; i < NCol; i++){

      T3_in.getPixel();
      theta = (atan2((T23R * (-4.0)), (2.0 * (T33R - T22R))) + M_PI) / 4.0;
      fwrite(&theta, sizeof(float), 1, eta_file);

      if(theta > (M_PI / 4.0)){
        theta -= (M_PI / 2.0);
      }

      fwrite(&theta, sizeof(float), 1, theta_file);

      s2t = sin(2 * theta);
      c2t = cos(2 * theta);
      s4t = sin(4 * theta);
      c4t = cos(4 * theta);
      s2t2 = s2t * s2t;
      c2t2 = c2t * c2t;

      ct = cos(theta);
      st = sin(theta);
      ct_2 = ct * ct;
      st_2 = st * st;

      T3_out.pixel[r11] = T11R;
      T3_out.pixel[r12] = c2t * T12R + T13R * s2t; //c2t* T12R + T13R*s2t;
      T3_out.pixel[r13] = c2t * T13R - T12R * s2t; //c2t* T13R - T12R*s2t;
      T3_out.pixel[r22] = pow(c2t, 2) * T22R + T33R * pow(s2t, 2) + T23R * s4t; //c2t2*T22R + 2*c2t*T23R*s2t + T33R*s2t2;
      T3_out.pixel[r23] = cos(4*theta)*T23R + (s4t*( T33R-T22R )) / 2.; //c2t2*T23R - c2t* T22R*s2t + c2t* T33R*s2t - T23R*s2t2;
      T3_out.pixel[r33] = pow(c2t, 2) * T33R + T22R * pow(s2t, 2) - T23R * s4t; //c2t2*T33R - 2*c2t* T23R*s2t + T22R*s2t2;
      T3_out.pixel[i12] = c2t * T12I + T13I * s2t; //c2t* T12I + T13I*s2t;
      T3_out.pixel[i13] = c2t * T13I - T12I * s2t; //c2t* T13I - T12I*s2t;
      T3_out.pixel[i23] = T23I; // - (s4t*(T22I - T33I))/2.;//T23I*(c2t2* + s2t2);
      T3_out.setPixel();

      /* Real:
      r11=T11R
      r12=c2t*T12R + T13R*s2t
      r13=c2t*T13R - T12R*s2t
      r21=c2t*T12R + T13R*s2t
      r22=Power(c2t,2)*T22R + T33R*Power(s2t,2) + T23R*s4t
      r23=cos(4*theta)*T23R + (s4t*( T33R-T22R ))/2.
      r31=c2t*T13R - T12R*s2t
      r32=cos(4*theta)*T23R + (s4t*( T33R-T22R ))/2.
      r33=Power(c2t,2)*T33R + T22R*Power(s2t,2) - T23R*s4t

      Imaginary:
      i11=T11I
      i12=c2t*T12I + T13I*s2t
      i13=c2t*T13I - T12I*s2t
      i21=-(c2t*T12I) - T13I*s2t
      i22=Power(c2t,2)*T22I + T33I*Power(s2t,2)
      i23=T23I - (s4t*(T22I - T33I))/2.
      i31=-(c2t*T13I) + T12I*s2t
      i32=-T23I + (s4t*(T33I - T22I))/2.
      i33=Power(c2t,2)*T33I + T22I*Power(s2t,2) */
    }
  }
  T3_in.close();
  T3_out.close();
  return 0;
}
