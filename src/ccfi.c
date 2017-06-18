/* forestry calculation

gcc ccfi.c  -Wall -lgsl -lblas -lgslcblas 

using code from:

 http://www.gnu.org/software/gsl/manual/html_node/Root-Finding-Examples.html  
 http://www.gnu.org/software/gsl/manual/html_node/Interpolation-Example-programs.html 

*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

#define MAXN 1000

struct inter_params{
	gsl_interp_accel *acc_lhq;
	gsl_spline *spline_lhq;
	double ival;
};


double inter (double x, void *params){
	struct inter_params *p  = (struct inter_params *) params;
  gsl_interp_accel *acc_lhq = p->acc_lhq;
	gsl_spline *spline_lhq = p->spline_lhq;
	double ival = p->ival;
	return  gsl_spline_eval(spline_lhq, x, acc_lhq)-(ival);
}


int main(int argc, char * argv[]){
  if(argc!=4){
	  printf("ccfi.c: invert interpolated LHQ to solve for percentile height, which is used to evaluate interpolated CCF.  By Ash Richardson, 20090616 with revisions 20170510.\nRequires GSL (GNU Scientific Library).\n");
	  printf("usage: ccfi [name of ascii file] [number of data records] [input lhq value (float)]\n");
	  printf("Skips the first four lines of the file.\n");
	exit(1);
  }

	int nlines = atoi(argv[2]);
	float band[nlines];
	float ccf[nlines];
	float lhq[nlines];
	double dband[nlines];
	double dccf[nlines];
	double dlhq[nlines];
	memset(&band[0], '\0',  sizeof(float)*nlines);
	memset(&ccf[0], '\0',  sizeof(float)*nlines);
	memset(&lhq[0], '\0',  sizeof(float)*nlines);
 
	memset(&dband[0], '\0',  sizeof(double)*nlines);
  	memset(&dccf[0], '\0',  sizeof(double)*nlines);
  	memset(&dlhq[0], '\0',  sizeof(double)*nlines);

	char chars[MAXN];
	char * fn = argv[1];
	float l = atof(argv[3]);
	double lhqin = (double)(l);

	FILE * f = fopen( fn, "rb");

	int i=0;
	for(i=0; i<4; i++){
		fgets( &chars[0], MAXN, f);
	}
	int a=0;
	i=0;

	while( (a=fscanf(f, "%f %f %f", &(band[i]), &(ccf[i]), &(lhq[i]))) & (a!=-1) ){
		dband[i] = (double)(band[i]);
		dccf[i] = (double)(ccf[i]);
		dlhq[i] = (double)(lhq[i]);
		i++;
	}
	
	for(i=0; i<(nlines-1); i++){
		if( (dband[i+1]-dband[i]) < 0.){
			printf( "%i of %i Warning: %f %f %f\n", i, nlines, dband[i+1], dband[i], dband[i+1]- dband[i]);
			printf("ABORTED.\n");
			exit(1);
		}
	}
	gsl_interp_accel * acc_ccf = gsl_interp_accel_alloc();
  gsl_spline * spline_ccf = gsl_spline_alloc(gsl_interp_cspline, nlines);
  gsl_spline_init (spline_ccf, dband, dccf, nlines);

	gsl_interp_accel * acc_lhq = gsl_interp_accel_alloc ();
  gsl_spline * spline_lhq = gsl_spline_alloc(gsl_interp_cspline, nlines);
  gsl_spline_init(spline_lhq, dband, dlhq, nlines);


	int status;
        int iter = 0, max_iter = 100;
        const gsl_root_fsolver_type *T;
        gsl_root_fsolver *s;
        double r = 0, r_expected = sqrt(lhqin);
        double x_lo = dband[0], x_hi = dband[nlines-1];
        gsl_function F;

	struct inter_params params = {acc_lhq, spline_lhq,lhqin};


	F.function = &inter;
	F.params = &params;

	T = gsl_root_fsolver_brent;
  s = gsl_root_fsolver_alloc (T);
  gsl_root_fsolver_set (s, &F, x_lo, x_hi);
  
  printf ("using %s method\n", gsl_root_fsolver_name (s));
  printf ("%5s [%9s, %9s] %9s %10s %9s\n", "iter", "lower", "upper", "root", "err", "err(est)");

	do{
    iter++;
    status = gsl_root_fsolver_iterate(s);
    r = gsl_root_fsolver_root(s);
    x_lo = gsl_root_fsolver_x_lower(s);
    x_hi = gsl_root_fsolver_x_upper(s);
    status = gsl_root_test_interval(x_lo, x_hi, 0, 0.001);
     
    if (status == GSL_SUCCESS) printf ("Converged:\n");
     
    printf ("%5d [%.7f, %.7f] %.7f %+.7f %.7f\n", iter, x_lo, x_hi, r, r - r_expected, x_hi - x_lo);
  }
  while(status == GSL_CONTINUE && iter < max_iter);

	double iv = gsl_spline_eval(spline_lhq, r, acc_lhq);    
  if(abs(iv  - lhqin) > 0.0001){
		printf("Error: interpolated LHQ did not match input LHQ.\n");
		printf("ABORTED.\n");
		exit(1);
	}
	printf("Interpolated value of LHQ for percentile height %f matched input LHQ: %f\n", r, iv );
	
	double CCF = gsl_spline_eval(spline_ccf, r, acc_ccf);
	printf("Interpolated value of CCF for percentile height: %f\n", CCF );
	printf("PERCENTILE HEIGHT LHQ CCF\n");
	printf("%f %f %f\n", r, iv, CCF);

	gsl_root_fsolver_free(s);
	gsl_spline_free(spline_ccf);
  gsl_interp_accel_free(acc_ccf);
  gsl_spline_free (spline_lhq);
  gsl_interp_accel_free(acc_lhq);

	fclose(f);
  return 0;
}
