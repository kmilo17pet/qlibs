/*!
 * @file qvfloat.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qvfloat.h"
#include "qtypegeneric.h"
#include "qfmathex.h"
#include "qffmath.h"
#include <string.h>

typedef float (*qVFloat_VVFcn_t)( float **dst,
                                  float **pOut,
                                  const float a,
                                  const float * const x,
                                  const float b,
                                  const float * const y,
                                  size_t n );
typedef float (*qVFloat_kVFcn_t)( float **dst,
                                  float **pOut,
                                  const float a,
                                  const float * const x,
                                  const float b,
                                  size_t n );

static float qVFloat_VV_Add( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n );
static float qVFloat_VV_Mul( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n );
static float qVFloat_VV_Div( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n );
static float qVFloat_kV_Add( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n );
static float qVFloat_kV_Mul( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n );
static float qVFloat_kV_Div( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n );
static int qVFloat_CmpAsc( const void *a,
                           const void *b,
                           void *arg );
static int qVFloat_CmpDes( const void *a,
                           const void *b,
                           void *arg );

/*============================================================================*/
static float qVFloat_VV_Add( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n )
{
    size_t i;
    float s = 0.0f;

    for ( i = 0u; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] = ( a*x[ i ] ) + ( b*y[ i ] );
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
static float qVFloat_kV_Add( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n )
{
    size_t i;
    float s = 0.0f;

    for ( i = 0u ; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] = ( a*x[ i ] ) + b;
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
static float qVFloat_VV_Mul( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n )
{
    size_t i;
    float s = 0.0f;
    const float k = a*b;

    for ( i = 0u ; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] = k*x[ i ]*y[ i ];
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
static float qVFloat_kV_Mul( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n )
{
    size_t i;
    float s = 0.0f;
    const float k = a*b;

    for ( i = 0u ; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] = k*x[ i ];
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
static float qVFloat_VV_Div( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             const float * const y,
                             size_t n )
{
    size_t i;
    float s = 0.0f;
    const float k = a/b;

    for ( i = 0u ; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] = ( k*x[ i ] )/y[ i ];
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
static float qVFloat_kV_Div( float **dst,
                             float **pOut,
                             const float a,
                             const float * const x,
                             const float b,
                             size_t n )
{
    size_t i;
    float s = 0.0f;
    const float k = a/b;

    for ( i = 0u ; i < n ; ++i ) {
        pOut[ 0 ][ 0 ] =  k*x[ i ];
        s += pOut[ 0 ][ 0 ];
        dst[ 0 ]++;
    }

    return s;
}
/*============================================================================*/
float qVFloat_Operate( float * dst,
                       qVFloat_Operation_t o,
                       const float a,
                       const float * const x,
                       const float b,
                       const float * const y,
                       const size_t n )
{
    static qVFloat_VVFcn_t vv_fcn[ 3 ] = {
                                         &qVFloat_VV_Add, &qVFloat_VV_Mul, &qVFloat_VV_Div
                                         };
    static qVFloat_kVFcn_t kv_fcn[ 3 ] = {
                                         &qVFloat_kV_Add, &qVFloat_kV_Mul, &qVFloat_kV_Div
                                         };
    float retVal = 0.0f;

    if ( ( NULL != x ) && ( n > 0u ) ) {
        float t, *pt = &t;
        float **ppt = ( NULL == dst ) ? &pt : &dst;
        float **pdst = &dst;

        retVal = ( NULL != y ) ? vv_fcn[ o ]( pdst, ppt, a, x, b, y, n )
                               : kv_fcn[ o ]( pdst, ppt, a, x, b, n );
    }

    return retVal;
}
/*============================================================================*/
float qVFloat_ApplyFx( float *dst,
                       float (*fx1)(float),
                       float (*fx2)(float, float),
                       float * const x,
                       float * const y,
                       const float a,
                       const float b,
                       const size_t n )
{
    size_t i;
    float s = 0.0f;
    float t, *pt = &t;
    float **ppt = ( NULL == dst ) ? &pt : &dst;

    if ( NULL != fx1 ) {
        for ( i = 0u ; i < n ; ++i ) {
            ppt[ 0 ][ 0 ] = a*fx1( x[ i ] );
            s += ppt[ 0 ][ 0 ];
            dst++;
        }
    }
    else if ( NULL != fx2 ) {
        if ( NULL != y ) {
            for ( i = 0u ; i < n ; ++i ) {
                ppt[ 0 ][ 0 ]= a*fx2( x[ i ], y[ i ] );
                s += ppt[ 0 ][ 0 ];
                dst++;
            }
        }
        else {
            for ( i = 0u ; i < n ; ++i ) {
                ppt[ 0 ][ 0 ] = a*fx2( x[ i ], b );
                s += ppt[ 0 ][ 0 ];
                dst++;
            }
        }
    }
    else {
        /*nothing to do here*/
    }

    return s;
}
/*============================================================================*/
int qVFloat_Moment( qVFloat_Moment_t * const m,
                    const float * const x,
                    const size_t n )
{
    int retVal = 0;

    if ( n > 1u ) {
        size_t j;
        float ep = 0.0f, s = 0.0f;
        /*cstat -CERT-FLP36-C*/
        const float l = (float)n;
        /*cstat +CERT-FLP36-C*/
        (void)memset( m, 0, sizeof(qVFloat_Moment_t) );
        for ( j = 1u ; j <= n ; ++j ) {
            s += x[ j ];
        }
        m->mean = s/l;
        for ( j = 1u ; j <= n ; ++j ) {
            float p;

            s = x[ j ] - m->mean;
            ep += s;
            m->avgDev += QLIB_ABS( s );
            p = s*s;
            m->var += p;
            p *= s;
            m->skew += p;
            p *= s;
            m->curt += p;
        }
        m->avgDev /= l;
        m->var = ( m->var - ( ( ep*ep )/l ) )/( l - 1.0f );
        /*cstat -MISRAC2012-Dir-4.11_b -CERT-FLP34-C*/
        m->stdDev =  ( m->var >= 0.0f ) ? QLIB_SQRT( m->var ) : QLIB_NAN;
        /*cstat +MISRAC2012-Dir-4.11_b*/
        if ( false == qFMathEx_Equal( 0.0f, m->var ) ) {
            m->skew /= ( ( m->curt )/( l*m->var*m->var ) ) - 3.0f;
        }
        else {
            m->skew = QLIB_NAN;
        }
        /*cstat +CERT-FLP34-C*/
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
float* qVFloat_Set( float * const x,
                    const float c,
                    const size_t n )
{
    size_t i;

    for ( i = 0u ; i < n ; ++i ) {
        x[ i ] = c;
    }

    return x;
}
/*============================================================================*/
float* qVFloat_Copy( float * const dst,
                     const float * const src,
                     const size_t n )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
    /*cppcheck-suppress misra-c2012-11.5 */
    return (float*)memcpy( dst, src, n*sizeof(float) );
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
}
/*============================================================================*/
float qVFloat_PolyVal( const float * const p,
                       const float x,
                       size_t n )
{
    size_t i;
    float fx = 0.0f;

    for ( i = 0u ; i < n ; ++i ) {
        /*cstat -CERT-FLP36-C*/
        fx += p[ n - i - 1u ]*QLIB_POW( x, (float)i );
        /*cstat +CERT-FLP36-C*/
    }

    return fx;
}
/*============================================================================*/
float* qVFloat_LinSpace( float * const dst,
                         const float x1,
                         const float x2,
                         const size_t n )
{
    /*cstat -CERT-FLP36-C -MISRAC2012-Rule-10.8*/
    /*cppcheck-suppress misra-c2012-10.8 */
    float step = ( x2 - x1 )/(float)( n - 1u );
    size_t i;

    for ( i = 0u ; i < n ; ++i ) {
        dst[ i ] = x1 + ( (float)i*step );
    }
    /*cstat +CERT-FLP36-C +MISRAC2012-Rule-10.8*/
    return dst;
}
/*============================================================================*/
float qVFloat_Distance( const float * const x,
                        const float * const y,
                        const size_t n )
{
    float s = 0.0f;

    if ( ( NULL != x ) && ( NULL != y ) ) {
        size_t i;

        for ( i = 0u ; i < n ; ++i ) {
            float tmp = x[ i ] - y[ i ];

            s += tmp*tmp;
        }
        /*cstat -MISRAC2012-Dir-4.11_b*/
        s = QLIB_SQRT( s ); /*always positive*/
        /*cstat +MISRAC2012-Dir-4.11_b*/
    }
    
    return s; 
}
/*============================================================================*/
float* qVFloat_Reverse( float * const dst,
                        float * const src,
                        const size_t init,
                        const size_t end )
{
    float *v = NULL;

    if ( ( NULL != src ) && ( end > init ) ) {
        v = ( NULL != dst ) ? qVFloat_Copy( dst, src, end - init ) : src;
        qTypeGeneric_Reverse( v, sizeof(float), init, end );
    }

    return v;
}
/*============================================================================*/
float* qVFloat_Rotate( float * const dst,
                       float * const src,
                       const int k,
                       const size_t n )
{
    float *v = NULL;

    if ( ( NULL != src ) && ( 0 != k ) && ( n > 0u ) ) {
        v = ( NULL != dst ) ? qVFloat_Copy( dst, src, n ) : src;
        qTypeGeneric_Rotate( v, sizeof(float), n, k );
    }

    return v;
}
/*============================================================================*/
int qVFloat_MinMax( qVFloat_MinMax_t * const o,
                    const float * const x,
                    const size_t n )
{
    int retVal = 0;

    if ( ( NULL != x ) && ( NULL != o ) && ( n > 0u ) ) {
        if ( 1u == n ) {
            o->min = x[ 0 ];
            o->max = x[ 0 ];
            o->index_min = 0u;
            o->index_max = 0u;
        }
        else {
            size_t i;

            if ( x[ 0 ] > x[ 1 ] ) {
                o->max = x[ 0 ];
                o->min = x[ 1 ];
                o->index_min = 1u;
                o->index_max = 0u;
            } 
            else {
                o->max = x[ 1 ];
                o->min = x[ 0 ];
                o->index_min = 0u;
                o->index_max = 1u;
            }

            for ( i = 2u ; i < n ; ++i ) {
                if ( x[ i ] >  o->max ) {
                    o->max = x[ i ];
                    o->index_max = i;
                }
                else if ( x[ i ] < o->min ) {
                    o->min = x[ i ];
                    o->index_min = i;
                }
                else {
                    /*nothing to do here*/
                }
            }
        }
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
static int qVFloat_CmpAsc( const void *a,
                           const void *b,
                           void *arg )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b -MISRAC2012-Rule-10.1_R3*/
    /*cppcheck-suppress misra-c2012-11.5 */
    const float *fa = (const float*)a;
    /*cppcheck-suppress misra-c2012-11.5 */
    const float *fb = (const float*)b;
    (void)arg;
    /*cppcheck-suppress misra-c2012-10.5 */
    return ( (int)( fa[ 0 ] > fb[ 0 ] ) ) - ( (int)( fa[ 0 ] < fb[ 0 ] ) );
}
/*============================================================================*/
static int qVFloat_CmpDes( const void *a,
                           const void *b,
                           void *arg )
{
    /*cppcheck-suppress misra-c2012-11.5 */
    const float *fa = (const float*)a;
    /*cppcheck-suppress misra-c2012-11.5 */
    const float *fb = (const float*)b;
    (void)arg;
    /*cppcheck-suppress misra-c2012-10.5 */
    return (int)( fa[ 0 ] < fb[ 0 ] ) - (int)( fa[ 0 ] > fb[ 0 ] );
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b +MISRAC2012-Rule-10.1_R3*/
}
/*============================================================================*/
float* qVFloat_Sort( float * const dst,
                     float * const src,
                     const bool dir,
                     size_t n )
{
    float *v = NULL;

    if ( ( src != NULL ) && ( n > 0u ) ) {
        v = ( NULL != dst ) ? qVFloat_Copy( dst, src, n ) : src;
        qTypeGeneric_Sort( v, n, sizeof(float), ( dir ) ? &qVFloat_CmpAsc : &qVFloat_CmpDes, NULL );
    }

    return v;
}
/*============================================================================*/