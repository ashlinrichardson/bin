//This version modified July 7 2009.  Added sorting functionality.

#pragma once
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <float.h>
using namespace std;
using std::ostream;
using std::vector;

namespace _SA{
	template<class T>
	class SA{
	/*! \class
	*  \brief A template array with bounds checking\n
	*	Similar to Shane's class SArray but built more with floating point array calcluations in mind as it uses template type of "Object" rather than "Object Pointer"
	*/
	public:


	//! Start a template array of a certain size - start the elements off at NULL(0)
	/*!
	*
	*/
		void init(unsigned int _sizei, unsigned int _sizej, unsigned int _sizek){
			sizei = _sizei;
			sizej = _sizej;
			sizek = _sizek;
			int size = sizei* ((sizej==0)?1:sizej) *  ((sizek==0)?1:sizek);
			mySize = size;
			elements=NULL;
			if(size > 0)
			{
				mySize=size;
				elements = NULL;
				elements= new T[size];
				if(!elements){
					printf("Error: SA.h: failed to allocate array.\n");
					exit(1);
				}
				memset( &(elements[0]), '\0', mySize*sizeof(T));
			}
		}
		SA(unsigned int size)
		{
			init( size, 0, 0);
		}
	
		SA(unsigned int sizei, unsigned int sizej){
			init( sizei, sizej, 0);
		}
		SA(unsigned int sizei, unsigned int sizej, unsigned int sizek){
			init( sizei, sizej, sizek);
		}


		~SA(){
			free( elements );
		}
		
	//! Initialize a template array from another, copying the elements
	/*!
	*
	*/
		inline SA(SA<T> * other)
		{
			if(!other){
				mySize=0;
				elements=NULL;
			}
			mySize = other->size();
			if(mySize==0)
			{
				elements=NULL;
				return;
			}
			else
			{
				elements= new T[mySize];
				for(register unsigned int i=0; i<mySize; i++)
					elements[i]=(*other)[i];
			}
			sizei = other->sizei;
			sizej = other->sizej;
			sizek = other->sizek;
			
		}


//! Return the number of elements
	/*!
	*
	*/
		inline unsigned int size()
		{
			return mySize;
		}
	//! Return the number of elements
	/*!
	*
	*/
		inline unsigned int length()
		{
			return mySize;
		}
	//! Equality operator - return true if the lengths and elements of both arrays are all the same
	/*!
	*
	*/
		inline T & at(unsigned int subscript)
		{
			if( sizej + sizek >0){
				printf("Error: must not interpret 2/3 dimensional array 1-dimensionally.\n");
				exit(1);
			}
			return elements[subscript];
		}

		inline T & at(unsigned int indi, unsigned int indj)
		{
			if(  (sizek >0)  || (sizej==0) ){
				printf("Error: must use 2-d index for 2-d array\n");
				exit(1);
			}
			return elements[ (indi * sizej) + indj];
		}

		inline T & at(unsigned int indi, unsigned int indj, unsigned int indk)
		{
			if( sizei * sizej * sizek <1){
				printf("Error: must only use 3-d index with 3d array.\n");
				exit(1);
			}
			return elements[ (indi * sizej *sizek) + (indj*sizek) + indk];
		}
	//! Set the element at index "unsigned int subscript"
	/*!
	*
	*/
		inline void set(unsigned int subscript, T * e)
		{
			elements[subscript]=*e  ;
		}
	//! Array notation access operator - equivalent to at()
	/*!
	*
	*/
	inline T & operator[]( unsigned int subscript )
	{
		if( sizej + sizek >0 ){
			printf("SA.h: Error: not allowed to use [] notation for 2/3 dimensional arrays.\n");
			exit(1);
		}
		if(mySize==0)
		{
			std::cerr << "\nError: SA.cpp: size()=0 Subscript " << subscript  << " out of range" << std::endl;
				exit( 1 );

		}
		if ( subscript > mySize )
		{

			std::cerr << "\nError: SA.cpp: Subscript " << subscript  << " out of range" << std::endl;
			exit( 1 );
		}
		return elements[ subscript ]; // reference return

	}

	inline SA<T> & operator = (SA<T> &a )
	{
		if( a.size() != mySize){
			printf("SA.h: error: tried to equate arrays of different lengths.\n");
			exit(1);
		}
		unsigned register int i=0;
		for(i=0; i<mySize; i++)
		{
			elements[i]=a[i];
		}
		return a;
	}
	
	inline T total(){
		T a = 0;
		for(int i=0; i<mySize; i++){
			a += elements[i];
		}
		return a;
		
	}


//ONLY USE IF T is float....
	inline void sortf(){   
		float * adr =(float*) ((void*) (&elements[0]));
		sort( adr, adr+(mySize)); ///sizeof(float)));
	}
	float 	percentBinBot(float _percent_){
		int ind = (int)(((float)(mySize-1))*(_percent_/100.));
		return (float)(elements[ind]);		
	}
	float 	percentBinTop(float _percent_)
	{
		int ind = (mySize-1) - (int)(((float)(mySize-1))*(_percent_/100.));
		return (float)(elements[ind]);		
	}
	float max(){
		float m = FLT_MIN;// elements[0];
		for(register int i=0; i<mySize; i++){
			if(!((isnan(elements[i])||(isinf(elements[i])))))
				if(elements[i]>m)
					m=elements[i];	
		}return m;
	}
	float max2(){
		return max();

	}
	float min(){
		float m = FLT_MAX; //elements[0];
		for(register int i=0; i<mySize; i++){
			if(!((isnan(elements[i])||isinf(elements[i]))))
				if(elements[i]<m)
					m=elements[i];	
		}return m;
	}
	

	public:
	//! Size of the template array
	/*!
	*
	*/
		unsigned int mySize;
		unsigned int sizei;
		unsigned int sizej;
		unsigned int sizek;

	//! The array of elements itself
	/*!
	*
	*/
		T* elements;
		//int * indices;
	public:
	//! Clear sets each element to NULL (0)
	/*!
	*
	*/
		inline void clear(void){
			memset( elements, '\0', mySize*sizeof(T));
		}
	};

		//! Return a representation of the class SA instance as a std::vector.
	/*!
	*  It is desirable to switch between these representations depending on whether one would rather have an O(1) insertion operation, or an O(1) array operation.
	*/


	//! Stream output operator (for string representations)
	/*!
	*
	*/

	template <class T>
	inline ostream &operator<<( ostream &output, SA<T> &out )
	{
		unsigned register int i=0;
		for(i=0; i<(out.length()); i++)
		{
			if(i!=(out.length()-1))
				output << out[i]<<",";
			else
				output<<out[i];
		}
		return output;

	}

	template <typename T>
	//! XOR Swap operation
	/*!
	*
	*/
	static inline void XORSwap(T&a, T&b)
	{
		if(&a != &b)
		{
			a^=b;
			b^=a;
			a^=b;
		}

	}

	template <typename T>
	//! Absolute value function for int's or floats
	/*!
	*
	*/
	static inline T _abs(T a)
	{
		return (a<0)?(-a):(a);

	}

};

