/*!
 * @file qdsys.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qdsys.h"

/*============================================================================*/
float qDSys_FIRUpdate( float *w, float *c, size_t wsize, float x )
{
    size_t i;
    float y = 0.0f;
    if ( NULL != c ) {
        for ( i = ( wsize - 1u ); i >= 1u ; --i ) { 
            w[ i ] = w[ i - 1u ];
            y += w[ i ]*c[ i ];
        }
        y += c[ 0 ]*x;
    }
    else {
        for ( i = ( wsize - 1u ); i >= 1u ; --i ) { 
            w[ i ] = w[ i - 1u ];
            y += w[ i ];
        }
        y += x;
    }
    w[ 0 ] = x;
    return y;
}
/*============================================================================*/
int qDSys_Setup( qDSys_t *sys, float *num, float *den, float *v, size_t nb, size_t na )
{
    int retVal = 0;
    if ( ( NULL != sys ) && ( NULL != num ) && ( NULL != den ) && ( NULL != v ) && ( na > 0u) && ( nb > 0u ) ) {
        size_t i;
        float a0;
        sys->b = num;
        sys->a = &den[ 1 ];
        sys->v = v;
        sys->na = na;
        sys->n = ( na > nb ) ? na : nb;
        sys->min = -100.0f;
        sys->max =  100.0f;
        a0 = den[ 0 ];
        for ( i = 0; i < nb ; ++i ) {
			num[ i ] /= a0;
		}
        for ( i = 0; i < na ; ++i ) {
            den[ i ] /= a0;
        }
        sys->b0 = num[ 0 ];
        sys->init = 1u;
    }
    return retVal;
}
/*============================================================================*/
int qDSys_SetInputDelay( qDSys_t *sys, float *delaywindow, size_t n, float initval )
{
    int retVal = 0;
    if ( NULL != sys ) {
        qTDL_Setup( &sys->tDelay, delaywindow, n, initval );
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qDSys_IsInitialized( qDSys_t *sys )
{
    int retVal = 0;
    if ( NULL != sys ) {
        retVal = (int)( ( 1u == sys->init ) && ( NULL != sys->v ) );
    }
    return retVal;
}
/*============================================================================*/
int qDSys_SetSaturation( qDSys_t *sys, float min, float max )
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
float qDSys_Excite( qDSys_t *sys, float u )
{
    float y = 0.0f;
    if ( NULL != sys ) {
        size_t i;
        float v = u;
        
        /*using direct-form 2*/   
        for ( i = 0; i < sys->na ; ++i ) {
            v -= sys->a[ i ]*sys->v[ i ];
        }
        y = qDSys_FIRUpdate( sys->v, sys->b, sys->n, v );  

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