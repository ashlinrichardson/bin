#pragma once
inline void kennaugh_huynen(complex<TYPE>  s11_, complex<TYPE>  s12_, complex<TYPE>  s22_,
                            TYPE & psi, TYPE & tau, TYPE & m, TYPE & gamma, TYPE & nu, TYPE & rho, TYPE & r);

inline void normalize_coneigenvector(const M2C &S, const M2C & G, V2C & e, cf_ & mu){
	e.unit();

  V2C prod (S*e);
  assert_0 ( norm(prod) - norm(mu) );
  V2C estar(conj(e));
  cf_ mu1_ = (prod.a) / (estar.a);
  cf_ mu2_ = (prod.b) / (estar.b);
  mu = mu1_;

  if( norm(estar.a) <TOL){
    mu = mu2_;
	}
  else if( norm(estar.b) <TOL){
	  mu = mu1_;
	}
  else if(  ( norm(estar.a) <TOL) && ( norm(estar.b) <TOL)){
    cout << "Error: zero eigenvector" <<endl;
		exit(1);
	}
  else{
    assert_0( norm( mu1_ - mu2_));
	}

  assert_0(norm( S*e - mu*conj(e)));

	return;
}

inline void condiag( const M2C &M, cf_ & mu1, cf_ & mu2, V2C & e1, V2C & e2){

	M2C  G( adjoint(M) * M);
	cf_ a(G.a);
	cf_ b(G.b);
	cf_ c(G.c);
	cf_ d(G.d);
	
	cf_ e = csqrt (   (a+d)*conj(a+d) - (4.*(a*d -b*c))  );
	cf_ L1 = 0.5*(a+d +e);
	cf_ L2 = 0.5*(a+d -e);

	if(norm(norm(L1)-norm(L2)) < TOL){
		cout << "Warning: degenerate system\n";
	}
	
	mu1 = csqrt(L1);  mu2 = csqrt(L2);

  if(norm(mu2) > norm(mu1)){
    cf_ tmp = mu2;
    mu2 = mu1;
    mu1 = tmp;
	}

	V2C e1m1(REAL(1.), c/(L1-d));// e1m1.unit();
	V2C e2m1(REAL(1.), c/(L2-d));// e2m1.unit();
	V2C e1m2(REAL(1.), (L1-a)/b);// e1m2.unit();
	V2C e2m2(REAL(1.), (L2-a)/b);// e2m2.unit();

//Check that the two methods are consistent    
  assert_0( norm(e1m1-e1m2));
  assert_0( norm(e2m1-e2m2));

	e1 = e1m1; 
	e2 = e2m1;
    
//Check that the two methods solve Graves' problem.
	assert_0( norm( (G*e1) - (L1*e1) ));
	assert_0( norm( (G*e2) - (L2*e2) ));

	normalize_coneigenvector(M, G, e1, mu1);
	normalize_coneigenvector(M, G, e2, mu2);
	
//Check again that the two methods solve Graves' problem

	assert_0( norm( (G*e1) - (L1*e1)  ));
	assert_0( norm( (G*e2) - (L2*e2)  ));

	M2C D( mu1, REAL(0), REAL(0), mu2);
	M2C V( e1, e2);

//check that condiagonalization succeeded.	
	M2C ck (((conj(V)) *( D * (adjoint(V)))));
	assert_0( norm(ck - M));
}

inline void correct_coneigenvectors(const M2C & S, cf_ & mu1, cf_ & mu2, V2C & e1, V2C & e2){
	cf_ a(e1.a);  cf_ b(e1.b);
	cf_ c(e2.a);  cf_ d(e2.b);

	TYPE theta = angle( a*a  + b*b );
	cf_ rot( exp ( - (I*(0.5)*theta) ));
	a = rot*a;  e1.a=a;
	b = rot*b;  e1.b=b;
	mu1 =  exp(-(I*theta)) *  mu1;
	assert_0(   norm ( (S*e1) - mu1*conj(e1) ) );

	theta = angle( d/conj(a));
	rot=(exp ( - (I * ( theta)) ));
	e2.a = c*rot;  c=e2.a;
	e2.b = d*rot;  d=e2.b;
	mu2 = mu2 * (exp ( - ((2.)*I*theta) ));
	assert_0(   norm ( (S*e2) - mu2*conj(e2) ) );

	assert_0( imag( square(a) + square(b) ) );
	assert_0( imag(  square(conj(d)) + square(conj(c)))) ;
	assert_0( norm( a*conj(c) + b*conj(d) ) );
}

inline void calculate_parameters(const M2C & S,  const cf_ & mu1, const cf_ & mu2, const V2C & e1, const V2C & e2, 
                                 TYPE & m, TYPE & psi, TYPE & tau, TYPE & gamma, TYPE & nu, TYPE & rho, TYPE & r){
	cf_ a(e1.a);  cf_ b(e1.b);
	cf_ c(e2.a);  cf_ d(e2.b);

	TYPE psi1 =  atan( -imag(a)/imag(b));
	TYPE psi2 =  atan(  real(b)/real(a));
	TYPE tau1 =  atan(  imag(b)/real(a));
	TYPE tau2 =  atan( -imag(a)/real(b));

	TYPE L1  = square(norm(mu1));
	TYPE L2  = square(norm(mu2));
	M2C  G( adjoint(S) * S);

	assert_0( norm( (G*e1) - (L1*e1)  ));
	assert_0( norm( (G*e2) - (L2*e2)  ));
	assert_0(norm( S*e1 - mu1*conj(e1)));
        assert_0(norm( S*e2 - mu2*conj(e2)));

	psi=psi1;
	if( norm(imag(e1.b)) < (TOL)){
		psi=psi2;
	}
	else if(norm(real(e1.a)) < (TOL)){
		psi=PI/2;
	}
	else{
		assert_0( norm(psi1-psi2));
	}
	
	tau=tau1;
	if( norm(imag(e1.a)) < (TOL)){
		tau=tau2;
	}
	else if ( norm(real(e1.b)) < (TOL)){
		tau=(PI/2.);
	}
	else{
		assert_0( norm(tau1-tau2));
	}
	
	TYPE a1_ = -angle(mu1);
 	TYPE a2_ = -angle(mu2);
 	gamma = atan(   sqrt (   abs(mu2)/abs(mu1)     )     );
 	rho = (a1_ + a2_)/2.;
 	nu = (a2_ - a1_)/2.;
	TYPE q1 = norm(mu1 - (m*exp(-I*(rho-nu))));
 	TYPE q2 = norm(mu2 - (m*(square(tan(gamma)))*exp(-I*(rho+nu))));
	assert_0(q1);
	assert_0(q2);
	r += q1;
  r += q2;
	
//at this point the parameters are not necessarily scaled properly.
// take care of this later....
//first, make sure the decomp. parameters satisfy the decomposition:

	M2C D( (m*exp(-I*(rho-nu))), REAL(0), REAL(0), (m*(square(tan(gamma)))*exp(-I*(rho+nu))));
	M2C V( e1, e2);
	
//check that condiagonalization succeeded.	
	M2C ck (((conj(V)) *( D * (adjoint(V)))));
	assert_0 ( norm (ck-S));
	r+= norm (ck-S);
}

inline void kennaugh_huynen(complex<TYPE> s11_, complex<TYPE> s12_, complex<TYPE> s22_,
				TYPE & psi, TYPE & tau, TYPE & m, TYPE & gamma, TYPE & nu, TYPE & rho, TYPE & r){

	psi  = tau = m = gamma = nu = rho = 0.;
	TYPE err =0.;
	M2C  M(s11_, s12_, s12_, s22_);
	M2C S(remove_absolute_phase(M));
	TYPE alpha  = angle ( M.a);
	M2C residue_alpha((exp( (cf_(0,alpha)))*S) - M);
	err += (TYPE)(norm( residue_alpha ));

	M = S;

	cf_ mu1;  zero(mu1);
	cf_ mu2;  zero(mu2);

	V2C e1;
	V2C e2;

	condiag(M, mu1, mu2, e1, e2);

	m = norm(mu1);
	correct_coneigenvectors( M, mu1, mu2, e1, e2);
	calculate_parameters( M, mu1, mu2, e1, e2, m, psi, tau, gamma, nu, rho, r);

	r = err;
	return;
}
