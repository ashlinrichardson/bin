#define FLOATING_POINT_TOLERANCE 0.000005f
#include <math.h>

/*! \brief Use bubble sort to sort an array of arbitrary type into Ascending order.
 *         ...
 *
 *  ...
 */
template <class T>
inline void BubbleSortAscending( int length, T array[]){
 	register int i,j;
 	T tmp;
 	for(i=0; i<length; i++)
 	for(j=1; j<length; j++)
 	{
		if(array[j-1]>array[j]){
			tmp=array[j-1];
			array[j-1]=array[j];
			array[j]=tmp;
		}
	}
}

/*! \brief Calculate the mean and standard deviation of an array of arbitrary type.
 *         ...
 *
 *  It's only recommended that int, float, or long types be used in this routine.
 */
template <class T>
inline double StdDev(int length, T *array, double * mean_){
	register int i=0;
	float total=0.;
	float mean=0;
	for(i=0; i<length; i++)total+=(float)array[i];
	mean = total/((float)length);
	*mean_=mean;

	total=0;
	float tmp;
	for(i=0; i<length; i++){
		tmp=mean-array[i];
		total+=(tmp*tmp);
	}
	total/=(float(length-1));
	return sqrt(total);
}

/*! \brief Double absolute value (probably the same as in math.h)
 *         ...
 *
 *
 */
inline double _fabs(double a){
	if(a<0) return (-a);
	else return a;
}

/*! \brief Floating point equality (As floating point representations are not exact).
 *         ...
 *
 *
 */

inline int _feq(float a, float b){
	return ((a-b) < FLOATING_POINT_TOLERANCE)?1:0;
}


