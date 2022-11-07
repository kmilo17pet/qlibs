#include "qfloatextra.h"
#include <float.h>
#include <math.h>
#include <limits.h>
#include <string.h>

/*============================================================================*/
float qFloat_MapMinMax( const float x,
                        const float xMin,
                        const float xMax,
                        const float yMin,
                        const float yMax )
{
    return ( ( ( ( yMax - yMin )*( x - xMin ) )/( xMax - xMin ) ) + yMin );
}
/*============================================================================*/
bool qFloat_InRangeCoerce( float * const x,
                           const float lowerL,
                           const float upperL )
{
    bool retVal = false;
    if ( 1 == (int)isnan( x[ 0 ] ) ) {
        x[ 0 ] = lowerL;
    }
    else {
        if ( x[ 0 ] < lowerL ) {
            x[ 0 ] = lowerL;
        }
        else if ( x[ 0 ] > upperL ) {
            x[ 0 ] = upperL;
        }
        else {
            retVal = true;
        }
    }

    return retVal;
}
/*============================================================================*/
bool qFloat_Equal( const float a,
                   const float b )
{
    return ( fabsf( a - b ) <= FLT_MIN );
}
/*============================================================================*/