/*!
 * @file qfmathex.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qfmathex.h"
#include "qffmath.h"
#include <float.h>
#include <limits.h>
#include <string.h>

/*============================================================================*/
float qFMathEx_Normalize( const float x,
                          const float xMin,
                          const float xMax )
{
    return ( x - xMin )/( xMax - xMin );
}
/*============================================================================*/
float qFMathEx_MapMinMax( const float x,
                          const float xMin,
                          const float xMax,
                          const float yMin,
                          const float yMax )
{
    return ( ( yMax - yMin )*qFMathEx_Normalize( x, xMin, xMax ) ) + yMin;
}
/*============================================================================*/
bool qFMathEx_InRangeCoerce( float * const x,
                             const float lowerL,
                             const float upperL )
{
    bool retVal = false;

    if ( 1 == (int)QLIB_ISNAN( x[ 0 ] ) ) {
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
bool qFMathEx_AlmostEqual( const float a,
                           const float b,
                           const float tol )
{
    return ( QLIB_ABS( a - b ) <= QLIB_ABS( tol ) );
}
/*============================================================================*/
bool qFMathEx_Equal( const float a,
                     const float b )
{
    return qFMathEx_AlmostEqual( a, b, FLT_MIN );
}
/*============================================================================*/
bool qFMathEx_InPolygon( const float x,
                         const float y,
                         const float * const px,
                         const float * const py,
                         const size_t p )
{
    size_t i;
    bool retVal = false;
    float max_y = py[ 0 ], max_x = px[ 0 ], min_y = py[ 0 ], min_x = px[ 0 ];

    for ( i = 0u ; i < p ; ++i ) {
        max_y = QLIB_MAX( py[ i ], max_y );
        max_x = QLIB_MAX( px[ i ], max_x );
        min_y = QLIB_MIN( py[ i ], min_y );
        min_x = QLIB_MIN( px[ i ], min_x );
    }

    if ( ( y >= min_y ) && ( y <= max_y ) && ( x >= min_x ) && ( x <= max_x ) ) {
        size_t j = p - 1u;

        for ( i = 0u ; i < p ; ++i ) {
            if ( ( px[ i ] > x ) != ( px[ j ] > x ) ) {
                const float dx = px[ j ] - px[ i ];
                const float dy = py[ j ] - py[ i ];
                if ( y < ( ( dy*( x - px[ i ] ) )/( dx + py[ i ] ) ) ) {
                    retVal = !retVal;
                }
            }
            j = i;
        }
    }

    return retVal;
}
/*============================================================================*/
bool qFMathEx_isInsideCircle( const float x,
                              const float y,
                              const float cx,
                              const float cy,
                              const float r )
{
    const float d = ( ( x - cx )*( x - cx ) ) + ( ( y - cy )*( y - cy ) );
    return ( d <= (r*r) );
}
/*============================================================================*/
