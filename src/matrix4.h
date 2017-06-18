template<class T>
class herm4{
	public:
	inline herm4<T>(T A, T B, T C, T D, T E, T F, T G, T H, T I, T J) : a(A), b(B), c(C), d(D), e(E), f(F), g(G), h(H), i(I), j(J){
	}
	
	inline herm4<T>(const complex<T> &A): a(A.a), b(A.b), c(A.c), d(A.d), e(A.e), f(A.f), g(A.g), h(A.h), i(A.i), j(A.j){
	}
	
	inline herm4<T>(){
    zero();
  } 
	
	inline void initT4( float T11R, float T12I, float T12R, float T13I, float T13R, float T22R, float T23I, float T23R, float T33R){
		a.real((double)T11R);
		a.imag(0.);
		b.real((double)T12R);
		b.imag((double)T12I);
		c.real((double)T13R);
		c.imag((double)T13I);
		d.real((double)T22R);
		d.imag(0.);
		e.real((double)T23R);
		e.imag((double)T23I);
		f.real((double)T33R);
		f.imag(0.);
	}

	inline void zero(){
    a.real( 0.); a.imag( 0.);
    b.real( 0.); b.imag( 0.);
    c.real( 0.); c.imag( 0.);
    d.real( 0.); d.imag( 0.);
    e.real( 0.); e.imag( 0.);
    f.real( 0.); f.imag( 0.);
    g.real( 0.); g.imag( 0.);
    h.real( 0.); h.imag( 0.);
    i.real( 0.); i.imag( 0.);
		j.real( 0.); j.imag( 0.);
	}

	inline void nan(){
    a.real( NAN); a.imag( NAN);
    b.real( NAN); b.imag( NAN);
    c.real( NAN); c.imag( NAN);
    d.real( NAN); d.imag( NAN);
    e.real( NAN); e.imag( NAN);
    f.real( NAN); f.imag( NAN);
    g.real( NAN); g.imag( NAN);
    h.real( NAN); h.imag( NAN);
    i.real( NAN); i.imag( NAN);
	  j.real( NAN); j.imag( NAN);
	}

	inline void operator = (const herm4 &A){
		a=A.a; b=A.b; c=A.c;
		d=A.d; e=A.e; f=A.f;
		g=A.g; h=A.h; i=A.i; j=A.j; 
	}

	inline herm4<T> operator * (double s){
		return herm4<T>( s*a, s*b, s*c, s*d, s*e, s*f, s*g, s*h, s*i, s*j);
	}
	
	T a; T b; T c; T d; T e; T f; T g; T h; T i; T j;
};


template<class T>
	inline herm4<T> operator * (double s, const herm4<T> & A){
		 return herm4<T>( s*(A.a), s*(A.b), s*(A.c), s*(A.d), s*(A.e), s*(A.f), s*(A.g), s*(A.h), s*(A.i), s*(A.j));		
	}

	template<class T> 
  inline ostream & operator<<(ostream &output, const herm4<T> A){
    output << A.a       << " " << A.b       << " " << A.c       << " " << A.d << endl; 
		output << conj(A.b) << " " << A.e       << " " << A.f       << " " << A.g << endl;
		output << conj(A.c) << " " << conj(A.f) << " " << A.h       << " " << A.i << endl;
		output << conj(A.d) << " " << conj(A.g) << " " << conj(A.i) << " " << A.j << endl;
                return output;
  } 

	template<class T>
	inline herm4<T> operator + (const herm4<T> & A,  const herm4<T> & B){		
		return herm4<T>( 
			B.a+A.a, B.b+A.b, B.c+A.c,
      B.d+A.d, B.e+A.e, B.f+A.f,
      B.g+A.g, B.h+A.h, B.i+A.i, B.j+A.j);  
	}

	template<class T>
	inline herm4<T> operator-( const herm4<T> & A, const herm4<T> & B){
		return herm4<T>(A.a-B.a, A.b-B.b, A.c-B.c, A.d-B.d , A.e-B.e, A.f-B.f, A.g-B.g, A.h-B.h, A.i-B.i, A.j-B.j);
	}
	
	template<class T>
	inline herm4<T> operator-( const herm4<T> & B){
		return herm4<T>(-B.a, -B.b, -B.c, -B.d, -B.e, -B.f, -B.g, -B.h, -B.i, -B.j);
	}
