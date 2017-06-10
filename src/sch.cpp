/* mapping from T4 matrix to physically meaningful parameters, to HSV encoded result

Includes some code from PolSARPro */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "envi.h"
#include <list>
#define ENVI_FLOAT_TYPE 4
#define PI 3.141592653589793
#define eps 1.E-15

#define Npolar_in   16		/* number of input/output files */
#define Npolar_out  13

/* T matrix */
#define T11     0
#define T12_re  1
#define T12_im  2
#define T13_re  3
#define T13_im  4
#define T14_re  5
#define T14_im  6
#define T22     7
#define T23_re  8
#define T23_im  9
#define T24_re  10
#define T24_im  11
#define T33     12
#define T34_re  13
#define T34_im  14
#define T44     15

#define oL1 0
#define oL2 1
#define oL3 2
#define oL4 3
#define oH  4
#define oS  5
#define oV  6
#define oR  7
#define oG  8
#define oB  9
#define oHs  10
#define oSs  11
#define oVs  12

/*******************************************************************************
Routine  : diagonalisation
Authors  : Eric POTTIER, Laurent FERRO-FAMIL
Creation : 01/2002
Update   :
*-------------------------------------------------------------------------------
Description :  Computes the eigenvectors and eigenvalues of a N*N hermitian
matrix (with N < 10)
*-------------------------------------------------------------------------------
Inputs arguments :
MatrixDim : Dimension of the Hermitian Matrix (N)
HermitianMatrix : N*N*2 complex hermitian matrix
Returned values  :
EigenVect : N*N*2 complex eigenvector matrix
EigenVal  : N elements eigenvalue real vector
*******************************************************************************/
void edit_error(char *s1, char *s2){
    printf("\n A processing error occured ! \n %s%s\n", s1, s2);
    exit(1);
}
void Diagonalisation(int MatrixDim, float ***HM, float ***EigenVect, float *EigenVal){

    double a[10][10][2], v[10][10][2], d[10], b[10], z[10];
    double w[2], s[2], c[2], titi[2], gc[2], hc[2];
    double sm, tresh, x, toto, e, f, g, h, r, d1, d2;
    int n, p, q, ii, i, j, k;

    n = MatrixDim;

    for (i = 1; i < n + 1; i++) {
		for (j = 1; j < n + 1; j++) {
			a[i][j][0] = HM[i - 1][j - 1][0];
			a[i][j][1] = HM[i - 1][j - 1][1];
			v[i][j][0] = 0.;
			v[i][j][1] = 0.;
		}
		v[i][i][0] = 1.;
		v[i][i][1] = 0.;
	}

    for (p = 1; p < n + 1; p++) {
		d[p] = a[p][p][0];
		b[p] = d[p];
		z[p] = 0.;
	}

    for (ii = 1; ii < 1000 * n * n; ii++) {
		sm = 0.;
		for (p = 1; p < n; p++) {
			for (q = p + 1; q < n + 1; q++) {
				sm = sm + 2. * sqrt(a[p][q][0] * a[p][q][0] + a[p][q][1] * a[p][q][1]);
			}
		}
		sm = sm / (n * (n - 1));
		if (sm < 1.E-16) goto Sortie;
		tresh = 1.E-17;
		if (ii < 4) tresh = (long) 0.2 *sm / (n * n);
		x = -1.E-15;
		for (i = 1; i < n; i++) {
			for (j = i + 1; j < n + 1; j++) {
				toto = sqrt(a[i][j][0] * a[i][j][0] + a[i][j][1] * a[i][j][1]);
				if (x < toto) {
					x = toto;
					p = i;
					q = j;
				}
			}
		}
		toto = sqrt(a[p][q][0] * a[p][q][0] + a[p][q][1] * a[p][q][1]);
		if (toto > tresh) {
			e = d[p] - d[q];
			w[0] = a[p][q][0];
			w[1] = a[p][q][1];
			g = sqrt(w[0] * w[0] + w[1] * w[1]);
			g = g * g;
			f = sqrt(e * e + 4. * g);
			d1 = e + f;
			d2 = e - f;
			if (fabs(d2) > fabs(d1)) d1 = d2;
			r = fabs(d1) / sqrt(d1 * d1 + 4. * g);
			s[0] = r;
			s[1] = 0.;
			titi[0] = 2. * r / d1;
			titi[1] = 0.;
			c[0] = titi[0] * w[0] - titi[1] * w[1];
			c[1] = titi[0] * w[1] + titi[1] * w[0];
			r = sqrt(s[0] * s[0] + s[1] * s[1]);
			r = r * r;
			h = (d1 / 2. + 2. * g / d1) * r;
			d[p] = d[p] - h;
			z[p] = z[p] - h;
			d[q] = d[q] + h;
			z[q] = z[q] + h;
			a[p][q][0] = 0.;
			a[p][q][1] = 0.;

			for (j = 1; j < p; j++) {
				gc[0] = a[j][p][0];
				gc[1] = a[j][p][1];
				hc[0] = a[j][q][0];
				hc[1] = a[j][q][1];
				a[j][p][0] = c[0] * gc[0] - c[1] * gc[1] - s[0] * hc[0] - s[1] * hc[1];
				a[j][p][1] = c[0] * gc[1] + c[1] * gc[0] - s[0] * hc[1] + s[1] * hc[0];
				a[j][q][0] = s[0] * gc[0] - s[1] * gc[1] + c[0] * hc[0] + c[1] * hc[1];
				a[j][q][1] = s[0] * gc[1] + s[1] * gc[0] + c[0] * hc[1] - c[1] * hc[0];
			}
			for (j = p + 1; j < q; j++) {
				gc[0] = a[p][j][0];
				gc[1] = a[p][j][1];
				hc[0] = a[j][q][0];
				hc[1] = a[j][q][1];
				a[p][j][0] = c[0] * gc[0] + c[1] * gc[1] - s[0] * hc[0] - s[1] * hc[1];
				a[p][j][1] = c[0] * gc[1] - c[1] * gc[0] + s[0] * hc[1] - s[1] * hc[0];
				a[j][q][0] = s[0] * gc[0] + s[1] * gc[1] + c[0] * hc[0] + c[1] * hc[1];
				a[j][q][1] = -s[0] * gc[1] + s[1] * gc[0] + c[0] * hc[1] - c[1] * hc[0];
			}
			for (j = q + 1; j < n + 1; j++) {
				gc[0] = a[p][j][0];
				gc[1] = a[p][j][1];
				hc[0] = a[q][j][0];
				hc[1] = a[q][j][1];
				a[p][j][0] = c[0] * gc[0] + c[1] * gc[1] - s[0] * hc[0] + s[1] * hc[1];
				a[p][j][1] = c[0] * gc[1] - c[1] * gc[0] - s[0] * hc[1] - s[1] * hc[0];
				a[q][j][0] = s[0] * gc[0] + s[1] * gc[1] + c[0] * hc[0] - c[1] * hc[1];
				a[q][j][1] = s[0] * gc[1] - s[1] * gc[0] + c[0] * hc[1] + c[1] * hc[0];
			}
			for (j = 1; j < n + 1; j++) {
				gc[0] = v[j][p][0];
				gc[1] = v[j][p][1];
				hc[0] = v[j][q][0];
				hc[1] = v[j][q][1];
				v[j][p][0] = c[0] * gc[0] - c[1] * gc[1] - s[0] * hc[0] - s[1] * hc[1];
				v[j][p][1] = c[0] * gc[1] + c[1] * gc[0] - s[0] * hc[1] + s[1] * hc[0];
				v[j][q][0] = s[0] * gc[0] - s[1] * gc[1] + c[0] * hc[0] + c[1] * hc[1];
				v[j][q][1] = s[0] * gc[1] + s[1] * gc[0] + c[0] * hc[1] - c[1] * hc[0];
			}
		}
	}

	Sortie:

    for (k = 1; k < n + 1; k++) {
		d[k] = 0;
		for (i = 1; i < n + 1; i++) {
			for (j = 1; j < n + 1; j++) {
				d[k] = d[k] + v[i][k][0] * (HM[i - 1][j - 1][0] * v[j][k][0] - HM[i - 1][j - 1][1] * v[j][k][1]);
				d[k] = d[k] + v[i][k][1] * (HM[i - 1][j - 1][0] * v[j][k][1] + HM[i - 1][j - 1][1] * v[j][k][0]);
			}
		}
	}

    for (i = 1; i < n + 1; i++) {
		for (j = i + 1; j < n + 1; j++) {
			if (d[j] > d[i]) {
				x = d[i];
				d[i] = d[j];
				d[j] = x;
				for (k = 1; k < n + 1; k++) {
					c[0] = v[k][i][0];
					c[1] = v[k][i][1];
					v[k][i][0] = v[k][j][0];
					v[k][i][1] = v[k][j][1];
					v[k][j][0] = c[0];
					v[k][j][1] = c[1];
				}
			}
		}
	}

    for (i = 0; i < n; i++) {
		EigenVal[i] = d[i + 1];
		for (j = 0; j < n; j++) {
			EigenVect[i][j][0] = v[i + 1][j + 1][0];
			EigenVect[i][j][1] = v[i + 1][j + 1][1];
		}
	}

}

/* this method should go in a utility file*/
int HSVtoRGB( float *r, float *g, float *b, float h, float s, float v){
	if( (h>360.)||(h<0.))
	{
		printf("H: HSV out of range %f %f %f %d %d\n", h, s, v);
		return(1);
	}
	if((s<0.)||(s>1.))
	{
		printf("S: HSV out of range %f %f %f %d %d\n", h, s, v);
		return(1);
	}
	if((v<0.)||(v>1.))
	{
		printf("V: HSV out of range %f %f %f %d %d\n", h, s, v);
		return(1);
	}
	if (h==360.){
		h=0;
	}
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return 0;
	}
	float H,S,V;
	H=h; V=v; S=s;
	h /= 60.;			// sector 0 to 5
	i = (int)floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1. - s );
	q = v * ( 1. - s * f );
	t = v * ( 1. - s * ( 1 - f ) );
	switch( i ) {
		case 0: *r = v; *g = t; *b = p; break;
		case 1: *r = q; *g = v; *b = p; break;
		case 2: *r = p; *g = v; *b = t; break;
		case 3: *r = p; *g = q; *b = v; break;
		case 4: *r = t; *g = p; *b = v; break;
		case 5: *r = v; *g = p; *b = q; break;
		default: printf("\nERROR HSV to RGB"); printf("i=%d hsv= %f %f %f\n", i, H, S, V);
		//exit(1);
	}
	return 0;
}

float *vector_float(int nrh)
{
    int ii;
    float *m;

    m = (float *) malloc((unsigned) (nrh + 1) * sizeof(float));
    if (!m)
	edit_error("allocation failure 1 in vector_float()", "");

    for (ii = 0; ii < nrh; ii++)
	m[ii] = 0;
    return m;
}
float ***matrix3d_float(int nz, int nrh, int nch)
{
    int ii, jj, dd;
    float ***m;


    m = (float ***) malloc((unsigned) (nz + 1) * sizeof(float **));
    if (m == NULL)
	edit_error("D'ALLOCATION No.1 DANS MATRIX()", "");
    for (jj = 0; jj < nz; jj++) {
	m[jj] = (float **) malloc((unsigned) (nrh + 1) * sizeof(float *));
	if (m[jj] == NULL)
	    edit_error("D'ALLOCATION No.2 DANS MATRIX()", "");
	for (ii = 0; ii < nrh; ii++) {
	    m[jj][ii] =
		(float *) malloc((unsigned) (nch + 1) * sizeof(float));
	    if (m[jj][ii] == NULL)
		edit_error("D'ALLOCATION No.3 DANS MATRIX()", "");
	}
    }
    for (dd = 0; dd < nz; dd++)
	for (jj = 0; jj < nrh; jj++)
    for (ii = 0; ii < nch; ii++)
		m[dd][jj][ii] = (float) (0.);
    return m;
}
void zero(float * a, int l)
{
	for(int i=0; i<l; i++) a[i]=0;
}
void read_config(char *dir, int *Nlig, int *Ncol, char *PolarCase,
	    char *PolarType)
{
    char file_name[128];
    char Tmp[128];
    FILE *file;

    sprintf(file_name, "%sconfig.txt", dir);
    if ((file = fopen(file_name, "r")) == NULL)
	edit_error("Could not open configuration file : ", file_name);


    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%i\n", &*Nlig);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%i\n", &*Ncol);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", PolarCase);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", Tmp);
    fscanf(file, "%s\n", PolarType);

    fclose(file);
}
int main(int argc, char *argv[]){
	if(argc<4){
	  printf("cloude_v3_change_percent.cpp: Shane Cloude Algorithm Implemented by Ash Richardson March 2008 with arbitrary histogram scaling revisioned 20170608 \nUsage: sch [in_dir] [out_dir] [value cutoff 0-100] [alpha=1/2/3] [hue cutoff 0-100] \n");
	  exit(1);
	}
    register int lig, col;
    FILE *in_file[Npolar_in], *out_file[Npolar_out];
    char file_name[128], in_dir[128], out_dir[128];
    char *file_name_in[Npolar_in] =
	{ "T11.bin", "T12_real.bin", "T12_imag.bin",
	"T13_real.bin", "T13_imag.bin",
	"T14_real.bin", "T14_imag.bin",
	"T22.bin", "T23_real.bin", "T23_imag.bin",
	"T24_real.bin", "T24_imag.bin",
	"T33.bin", "T34_real.bin", "T34_imag.bin",
	"T44.bin"
    };
    char *file_name_out[Npolar_out] = {
	"L1.bin", "L2.bin", "L3.bin","L4.bin",
	"H.bin","S.bin","V.bin",
	"R.bin","G.bin","B.bin",
	"Hscaled.bin","Sscaled.bin","Vscaled.bin"
	};
	strcpy(in_dir, argv[1]);
	strcpy(out_dir, argv[2]);
	char PolarCase[20];
	char PolarType[20];
	int Nlig, Ncol, Np;
	float pp = atof(argv[3]);
	int skipHIST =0;

	float huepercent =0.;
	register int alphaselect = 2;
        if(argc >4){
                alphaselect = atoi(argv[4]);
                //alphaselect--;

                if( (alphaselect<1) || (alphaselect>3)){
                        printf("ERROR: alpha parameter must be set to 1,2, or 3.");
                        exit(1);
                }
		if(argc>5){
			huepercent = atof(argv[5]);
			if(argc>6){
				skipHIST = atoi(argv[6]);
				if( (skipHIST ==1) || (skipHIST==0)){
				}
				else{
					printf("Error: SKIPHIST parameter must be set to 0 or 1.\n"); 
					exit(1);
				} 
			}
		}
        }
        alphaselect = alphaselect-1;   //zero index the alpha.


	read_config(in_dir, &Nlig, &Ncol, PolarCase, PolarType);

	for (Np = 0; Np < Npolar_in; Np++) {
		sprintf(file_name, "%s%s", in_dir, file_name_in[Np]);
		if ((in_file[Np] = fopen(file_name, "rb")) == NULL)
		edit_error("Could not open input file : ", file_name);
	}


	for (Np = 0; Np < Npolar_out; Np++) {
	//	if (Flag[Np] == 1) {
		sprintf(file_name, "%s%s", out_dir, file_name_out[Np]);
		write_envi_hdr(file_name, Nlig, Ncol); //writeENVIHeader(file_name, ENVI_FLOAT_TYPE , Nlig,Ncol);
		if ((out_file[Np] = fopen(file_name, "wb")) == NULL)
			edit_error("Could not open output file : ", file_name);
	//	}

	}


	float minL, maxL;
	minL=0;
	maxL=0;
	float minH, maxH;
	minH=0;
	maxH=0;
	float minS, maxS;
	minS=0;
	maxS=0;
	int k;
	float m[Npolar_in];
	float o[Npolar_out];

	float *** T = matrix3d_float(4, 4, 2);  //input matrix
    	float *** _V = matrix3d_float(4, 4, 2);  //eigenvector matrix
	float * lambda = vector_float(4);
	float p[4], alpha[4], alpha2, H;
	zero(p,4);
	zero(alpha,4);
	alpha2=0;
	int count=0;


	std::list<float> L2pL3;
	L2pL3.clear();

	std::list<float> Huelist;
  Huelist.clear();

	for(lig=0; lig<Nlig; lig++){
		printf("\rprocessing row %d of %d", lig, Nlig);
		for(col=0; col<Ncol; col++){
			count ++;

			for(Np=0; Np<Npolar_in; Np++){
				fread( &m[Np], sizeof(float), 1, in_file[Np]);
			}
			/* Average complex coherency matrix determination*/
			T[0][0][0] = eps + m[T11];
			T[0][0][1] = 0.;
			T[0][1][0] = eps + m[T12_re];
			T[0][1][1] = eps + m[T12_im];
			T[0][2][0] = eps + m[T13_re];
			T[0][2][1] = eps + m[T13_im];
			T[0][3][0] = eps + m[T14_re];
			T[0][3][1] = eps + m[T14_im];

			T[1][0][0] = eps + m[T12_re];
			T[1][0][1] = eps - m[T12_im];
			T[1][1][0] = eps + m[T22];
			T[1][1][1] = 0.;
			T[1][2][0] = eps + m[T23_re];
			T[1][2][1] = eps + m[T23_im];
			T[1][3][0] = eps + m[T24_re];
			T[1][3][1] = eps + m[T24_im];

			T[2][0][0] = eps + m[T13_re];
			T[2][0][1] = eps - m[T13_im];
			T[2][1][0] = eps + m[T23_re];
			T[2][1][1] = eps - m[T23_im];
			T[2][2][0] = eps + m[T33];
			T[2][2][1] = 0.;
			T[2][3][0] = eps + m[T34_re];
			T[2][3][1] = eps + m[T34_im];

			T[3][0][0] = eps + m[T14_re];
			T[3][0][1] = eps - m[T14_im];
			T[3][1][0] = eps + m[T24_re];
			T[3][1][1] = eps - m[T24_im];
			T[3][2][0] = eps + m[T34_re];
			T[3][2][1] = eps - m[T34_im];
			T[3][3][0] = eps + m[T44];
			T[3][3][1] = 0.;


			Diagonalisation(4, T, _V, lambda);

			for (k = 0; k < 4; k++)
				if (lambda[k] < 0.) lambda[k] = 0.;

			for (k = 0; k < 4; k++) {
				/* Unitary eigenvectors */
				alpha[k] = acos(sqrt(_V[0][k][0] * _V[0][k][0] + _V[0][k][1] * _V[0][k][1]));
				p[k] = lambda[k] / (eps + lambda[0] + lambda[1] + lambda[2] + lambda[3]);
				if (p[k] < 0.) p[k] = 0.;
				if (p[k] > 1.) p[k] = 1.;
			}

			H=0;

			for (k = 0; k < 4; k++) {
				if(p[k]>0) H=H-(p[k]*log(p[k])/log(4));
				//H =-(p[1]*log(p[1])+p[2]*log(p[2])+p[3]*log(p[3])+p[4]*log(p[4]))/log(4)  ;
			}

			if(isnan(H)){
				fprintf(stderr, "NAN error: lig %d col %d p=[%f %f %f %f]\n",lig, col, p[0], p[1],p[2],p[3]);
				exit(1);
			}
			o[oL1]=lambda[0];
			o[oL2]=lambda[1];
			o[oL3]=lambda[2];
			o[oL4]=lambda[3];


			o[oH]=alpha[alphaselect];//*180.0/PI;  //'quadalpha'
			o[oS]=H;
			o[oV]=lambda[1]+lambda[2];



			L2pL3.push_back(o[oV]);

			Huelist.push_back(o[oH]);

			if( (lig==0) && (col==0)) minL = o[oV];
			if(minL > o[oV]) minL = o[oV];
			if(maxL < o[oV]) maxL = o[oV];

			if( (lig==0) && (col==0)) minH = o[oH];
			if(minH > o[oH]) minH = o[oH];
			if(maxH < o[oH]) maxH = o[oH];

			if( (lig==0) && (col==0)) minS = o[oS];
			if(minS > o[oS]) minS = o[oS];
			if(maxS < o[oS]) maxS = o[oS];

			//HSVtoRGB( &o[oR], &o[oG], &o[oB], o[oH], o[oS], o[oV] );

			for(Np=0; Np<oR; Np++)
			{
				fwrite( &o[Np], sizeof(float), 1, out_file[Np]);
				//fread(&M_in[0][0][0], sizeof(float), Ncol, in_file[Np]);
			}
		}
	}

	if(skipHIST==1){
		for(Np=oH; Np<oV; Np++) fclose(out_file[Np]);
        	for(Np=oR; Np<oB; Np++) fclose(out_file[Np]);
        	for(Np=oHs; Np<oVs; Np++) fclose(out_file[Np]);

		exit(0);
	}


	for(Np=0; Np<Npolar_out; Np++) fclose(out_file[Np]);
	for(Np=0; Np<Npolar_in; Np++) fclose(in_file[Np]);

//sort the list of all eigenvalue channel entries.
L2pL3.sort();
Huelist.sort();
printf("\n");

printf("Histogram binning Value...\n");


//THIS LINE WAS MODIFIED FOR ARBITRARY PERCENTAGE INDEX
//int _2percent_index = (int)(((float)( L2pL3.size()))*((float)0.02));
//==============Value scaling==========================================================
int _2percent_index = (int)(((float)( L2pL3.size()))*((float)pp/((float)100.0)));
//printf("\n 2_percent_ind %d of %d\n",_2percent_index, L2pL3.size());
register int q=0;

float _2percent_cutoff;

float list_first=(L2pL3.front());
float list_last=(L2pL3.back());

for(q=0; q<_2percent_index; q++){
	L2pL3.pop_back();
}
	_2percent_cutoff =(L2pL3.back());

//==============Hue scaling==========================================================
printf("Histogram binning Hue...\n");

int hueindex = (int)(((float)( Huelist.size()))*((float)huepercent/((float)100.0)));
//printf("\n 2_percent_ind %d of %d\n",_2percent_index, L2pL3.size());
q=0;

float huecutoff;

list_first=(Huelist.front());
list_last=(Huelist.back());

for(q=0; q<hueindex; q++)
{
        Huelist.pop_front();
}
huecutoff =(Huelist.front());  //back());

printf("Applying histogram derived cutoffs to Hue and Value...\n");


	sprintf(file_name, "%s%s", out_dir, file_name_out[oH]);
	if ((out_file[oH] = fopen(file_name, "rb")) == NULL)
		edit_error("Could not open output file for reading: ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oS]);
	if ((out_file[oS] = fopen(file_name, "rb")) == NULL)
		edit_error("Could not open output file for reading: ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oV]);
	if ((out_file[oV] = fopen(file_name, "rb")) == NULL)
		edit_error("Could not open output file for reading: ", file_name);


	sprintf(file_name, "%s%s", out_dir, file_name_out[oR]);
	if ((out_file[oR] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oG]);
	if ((out_file[oG] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oB]);
	if ((out_file[oB] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);

	sprintf(file_name, "%s%s", out_dir, file_name_out[oHs]);
	if ((out_file[oHs] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oSs]);
	if ((out_file[oSs] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);
	sprintf(file_name, "%s%s", out_dir, file_name_out[oVs]);
	if ((out_file[oVs] = fopen(file_name, "wb")) == NULL)
		edit_error("Could not open output file : ", file_name);


	float __H, __S, __V;
	float __R, __G, __B;

	float vv=0;

	for(lig=0; lig<Nlig; lig++){
		printf("\rprocessing row %d of %d", lig, Nlig);
		for(col=0; col<Ncol; col++){
			fread( &__H, sizeof(float), 1, out_file[oH]);
			fread( &__S, sizeof(float), 1, out_file[oS]);
			fread( &__V, sizeof(float), 1, out_file[oV]);

			//__H = ((__H - minH)/(maxH-minH))*360.;   //was 90 degrees.
			if(__H < huecutoff){
        __H = 0.;
      }
      else{
        __H = (__H - huecutoff)/(maxH-huecutoff);
      }
			__H *= 90.;

			if(__H<0) __H=eps;

			__S = ((__S - minS)/(maxS-minS));
			if(__S<0) __S=eps;
			vv=__V;

			if(__V > _2percent_cutoff){
				__V = 1.0;
			}
			else
			{
				__V = (__V - minL)/(_2percent_cutoff-minL);
 			}

			if(__V<0) __V=eps;
			HSVtoRGB( &__R, &__G, &__B, __H, __S, __V);

			fwrite( &__R, sizeof(float), 1, out_file[oR]);
			fwrite( &__G, sizeof(float), 1, out_file[oG]);
			fwrite( &__B, sizeof(float), 1, out_file[oB]);

			fwrite( &__H, sizeof(float), 1, out_file[oHs]);
			fwrite( &__S, sizeof(float), 1, out_file[oSs]);
			fwrite( &__V, sizeof(float), 1, out_file[oVs]);

		}
	}
	for(Np=oH; Np<oV; Np++) fclose(out_file[Np]);
	for(Np=oR; Np<oB; Np++) fclose(out_file[Np]);
	for(Np=oHs; Np<oVs; Np++) fclose(out_file[Np]);
	printf("\r                         ");
	printf("\rminH %f maxH %f\nminS %f maxS %f\nminL %f maxL %f\n", minH, maxH, minS, maxS, minL, maxL );
	printf("L cutoff %f \n",_2percent_cutoff);
  return 0;
}
