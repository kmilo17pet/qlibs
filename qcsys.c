/*!
 * @file qcsys.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qcsys.h"

/*============================================================================*/
int qCSys_Setup( qCSys_t *sys, float *num, float *den, float *x, size_t n, float dt )
{
    int retVal = 0;
    if ( ( NULL != sys ) && ( NULL != num ) && ( NULL != den ) && ( NULL != x )  && ( n > 0u) && ( dt > 0.0f ) ) {
        size_t i;
        float a0;
        sys->b = num + 1;
        sys->a = den + 1;
        sys->x = x;
        sys->n = n - 1;
        sys->dt = dt;
        sys->min = -100.0f;
        sys->max =  100.0f;
        a0 = den[ 0 ];
        for ( i = 0; i < n ; ++i ) {
			num[ i ] /= a0;
			den[ i ] /= a0;
		}
        sys->b0 = num[ 0 ];
        sys->init = 1u;
    }
    return retVal;
}
/*============================================================================*/
int qCSys_IsInitialized( qCSys_t *sys )
{
    int retVal = 0;
    if ( NULL != sys ) {
        retVal = ( 1u == sys->init ) && ( NULL != sys->x );
    }
    return retVal;
}
/*============================================================================*/
int qCSys_SetSaturation( qCSys_t *sys, float min, float max )
{
    int retVal = 0;
    if ( ( NULL != sys ) && ( max > min ) && ( 0u == sys->init ) ) {
        sys->min = min;
        sys->max = max;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
float qCSys_Excite( qCSys_t *sys, float u )
{
    float y = 0.0f;
    if ( NULL != sys ) {
        float dx0 = 0.0f;

        if ( 1u == sys->n ) {
            sys->x[ 0 ] += ( u - sys->x[ 0 ]*sys->a[ 0 ] )*sys->dt; 
            y = ( sys->b[ 0 ] - sys->a[ 0 ]*sys->b0 )*sys->x[ 0 ];
        }
        else {    
            size_t i;
            for (  i = ( sys->n - 1u) ; i >= 1u ; --i ) {
                dx0 += sys->a[ i ]*sys->x[ i ]; 
                sys->x[ i ] += sys->x[ i - 1 ]*sys->dt;
                y += ( sys->b[ i ] - sys->a[ i ]*sys->b0 )*sys->x[ i ];
            }
            dx0 = u - ( dx0 + sys->a[ 0 ]*sys->x[ 0 ] );
            sys->x[ 0 ] += dx0*sys->dt;
            y+= ( sys->b[ 0 ] - sys->a[ 0 ]*sys->b0 )*sys->x[ 0 ];
        }

        if ( y < sys->min ) {
            y = sys->min;
        }
        if ( y > sys->max ) {
            y = sys->max;
        }
    }
    return y ;
}
/*============================================================================*/