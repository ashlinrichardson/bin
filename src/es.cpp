#include "radlib.h"
inline TYPE frand(){
  return ((TYPE) ((double)rand() / ((double)RAND_MAX)));
}

inline TYPE arand(){
	return frand()-frand();
}

inline cf poly(cf x, cf  a, cf  b, cf c, cf d){
	return a*cpow(x,3.)+b*cpow(x,2.)+c*x+d;
}

inline TYPE residual(vec3<cf> & x, cf a, cf b, cf c, cf d){
	return abs( poly(x.a, a,b,c,d)) + abs(poly(x.b, a,b,c,d))+abs(poly(x.c,a,b,c,d));
}

inline cf rc(){
	return cf(arand(),arand());
}

int main(){
  printf("Eigendiagonalization benchmark\n");

  long rtime1, rtime2;
	rtime1 = clock();

	srand ( time(NULL) );

	TYPE R = 0.;
	TYPE maxr = 0.;
	
	int MAX = 10000;

	int i=0;
	
  for(i = 0; i < MAX ; i++){
	
		cf a(rc());
		cf b(rc());
		cf c(rc());
		cf d(rc());	
		cf e(rc());
		cf f(rc());	

		herm3<cf> A( a, b, c, d, e, f);
	
		vec3<cf> E1;
		vec3<cf> E2;
		vec3<cf> E3;
		vec3<cf> L;

		R = eig(A , L, E1, E2, E3);

		if (!((abs(L.a) > abs(L.b) ) && (abs(L.b) > abs(L.c)))){
			cout <<"Error:eigenvectors were not correctly sorted\n";
			exit(1);
		}

		if(R > maxr) {
			maxr = R ;
		}
	}	

	rtime2 = clock();
	float ttime = ((float)((double)(rtime2-rtime1))/((double)(CLOCKS_PER_SEC)));

 	cout << "maxr of " << MAX << " runs " << maxr <<endl;
	cout << "last R "<< R <<endl;
	cout << "total time: " << ttime << endl;
	cout << "Eigendiagonalizations per second: "<< ((double)MAX) / ttime <<endl;

	return 0;

}

/*	cp (a);  cp (b);
	cp (c);  cp (d);

	TYPE r=0.;	
	_matrix3::vec3<cf> x(     solve_cubic(a,b,c,d)             );

	
	r = residual(x, a, b, c, d);

	cf x1, x2, x3;	
	x1 = x.a; x2 = x.b; x3=x.c;

	printf("x1 "); cp(x1);
	printf("x2 "); cp(x2);
	printf("x3 "); cp(x3);
	printf( "\nresidual: %e\n",r);
*/
