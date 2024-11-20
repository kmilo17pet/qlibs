/*!
 * @file qinterp1.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qinterp1.h"
#include "qffmath.h"

typedef float (*qInterp1Fcn_t)( const float x,
                                const float * const tx,
                                const float * const ty,
                                const size_t tableSize );

static float qInterp1_next( const float x,
                            const float * const tx,
                            const float * const ty,
                            const size_t tableSize );
static float qInterp1_previous( const float x,
                                const float * const tx,
                                const float * const ty,
                                const size_t tableSize );
static float qInterp1_nearest( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize );
static float qInterp1_linear( const float x,
                              const float * const tx,
                              const float * const ty,
                              const size_t tableSize );
static float qInterp1_sine( const float x,
                            const float * const tx,
                            const float * const ty,
                            const size_t tableSize );
static float qInterp1_cubic( const float x,
                             const float * const tx,
                             const float * const ty,
                             const size_t tableSize );
static float qInterp1_hermite( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize );
static float qInterp1_spline( const float x,
                              const float * const tx,
                              const float * const ty,
                              const size_t tableSize );
static float qInterp1_cSpline( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize );

static float slope( const float * const tx,
                    const float * const ty,
                    const size_t i );
float firstDerivate( const float * const tx,
                     const float * const ty,
                     const size_t n,
                     const size_t i );
float leftSecondDerivate( const float * const tx,
                          const float * const ty,
                          const size_t n,
                          const size_t i );
float rightSecondDerivate( const float * const tx,
                           const float * const ty,
                           const size_t n,
                           const size_t i );


/*cstat -CERT-INT30-C_a*/

/*============================================================================*/
int qInterp1_Setup( qInterp1_t * const i,
                    const float * const xTable,
                    const float * const yTable,
                    const size_t sizeTable )
{
    int retVal = 0;

    if ( ( NULL != i ) && ( sizeTable > 2U ) && ( NULL != xTable ) && ( NULL != xTable ) ) {
        i->xData = xTable;
        i->yData = yTable;
        i->dataSize = sizeTable;
        i->method = &qInterp1_linear;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qInterp1_SetData( qInterp1_t * const i,
                      const float * const xTable,
                      const float * const yTable,
                      const size_t sizeTable )
{
    return qInterp1_Setup( i, xTable, yTable, sizeTable );
}
/*============================================================================*/
int qInterp1_SetMethod( qInterp1_t * const i,
                        const qInterp1Method_t m )
{
    int retVal = 0;
    static const qInterp1Fcn_t im[ QINTERP1_MAX ] = {
        &qInterp1_next,
        &qInterp1_previous,
        &qInterp1_nearest,
        &qInterp1_linear,
        &qInterp1_sine,
        &qInterp1_cubic,
        &qInterp1_hermite,
        &qInterp1_spline,
        &qInterp1_cSpline,
    };

    if ( ( NULL != i ) && ( m < QINTERP1_MAX ) ) {
        i->method = im[ m ];
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
float qInterp1_Get( qInterp1_t * const i,
                    const float x )
{
    return i->method( x, i->xData, i->yData, i->dataSize );
}
/*============================================================================*/
static float qInterp1_next( const float x,
                            const float * const tx,
                            const float * const ty,
                            const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        size_t nearestIndex = tableSize - 1U;

        for ( size_t i = 0U ; i < ( tableSize - 1U ) ; ++i ) {
            if ( x < tx[ i + 1U ] ) {
                nearestIndex = i;
                break;
            }
        }

        if ( x >= tx[ tableSize - 1U ] ) {
            y = ty[ tableSize - 1U ];
        }
        else {
            y = ty[ nearestIndex + 1U ];
        }
    }

    return y;
}
/*============================================================================*/
static float qInterp1_previous( const float x,
                                const float * const tx,
                                const float * const ty,
                                const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        if ( x <= tx[ 0 ] ) {
            y = ty[ 0 ];
        }
        else {
            size_t nearestIndex = 0U;

            for ( size_t i = 1U ; i < tableSize; ++i ) {
                if ( x < tx [ i ] ) {
                    break;
                }
                nearestIndex = i;
            }
            y = ty[ nearestIndex ];
        }
    }
    return y;
}
/*============================================================================*/
static float qInterp1_nearest( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        size_t nearestIndex = 0U;
        float minDistance = qFFMath_Abs( x - tx[ 0 ] );

        for ( size_t i = 1U ; i < tableSize; ++i ) {
            const float distance = qFFMath_Abs( x - tx[ i ] );

            if ( distance <= minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }
        y = ty[ nearestIndex ];
    }
    return y;
}
/*============================================================================*/
static float qInterp1_linear( const float x,
                              const float * const tx,
                              const float * const ty,
                              const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        if ( x < tx[ 0 ] ) {
            const float x0 = tx[ 0 ];
            const float x1 = tx[ 1 ];
            const float y0 = ty[ 0 ];
            const float y1 = ty[ 1 ];
            y = y0 + ( ( ( y1 - y0 )/( x1 - x0 ) )*( x - x0 ) );
        }
        else if ( x > tx[ tableSize - 1U ] ) {
            const float x0 = tx[ tableSize - 2U ];
            const float x1 = tx[ tableSize - 1U ];
            const float y0 = ty[ tableSize - 2U ];
            const float y1 = ty[ tableSize - 1U ];
            y = y1 + ( ( ( y0 - y1 )/( x0 - x1 ) )*( x - x1 ) );
        }
        else {
            const int maxIndex = (int)tableSize - 1;
            for ( int i = 0; i < maxIndex; ++i ) {
                if ( ( x >= tx[ i ] ) && ( x <= tx[ i + 1 ] ) ) {
                    const float x0 = tx[ i ];
                    const float x1 = tx[ i + 1 ];
                    const float y0 = ty[ i ];
                    const float y1 = ty[ i + 1 ];
                    y = y0 + ( ( ( y1 - y0 )/( x1 - x0 ) )*( x - x0 ) );
                    break;
                }
            }
        }
    }
    return y;
}
/*============================================================================*/
static float qInterp1_sine( const float x,
                            const float * const tx,
                            const float * const ty,
                            const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        if ( x < tx[ 0 ] ) {
            const float x0 = tx[ 0 ];
            const float x1 = tx[ 1 ];
            const float y0 = ty[ 0 ];
            const float y1 = ty[ 1 ];
            const float w = 0.5F - ( 0.5F*qFFMath_Cos( QFFM_PI*( x - x0 )/( x1 - x0 ) ) );
            y = y0 + ( w*( y1 - y0 ) );
        }
        else if ( x > tx[ tableSize - 1U ] ) {
            const float x0 = tx[ tableSize - 2U ];
            const float x1 = tx[ tableSize - 1U ];
            const float y0 = ty[ tableSize - 2U ];
            const float y1 = ty[ tableSize - 1U ];
            const float w = 0.5F - ( 0.5F*qFFMath_Cos( QFFM_PI*( x - x1 )/( x0 - x1 ) ) );
            y = y1 + ( w*( y0 - y1 ) );
        }
        else {
            for ( size_t i = 1; i < tableSize; ++i ) {
                if ( x <= tx[ i ] ) {
                    const float x0 = tx[ i - 1U ];
                    const float x1 = tx[ i ];
                    const float y0 = ty[ i - 1U ];
                    const float y1 = ty[ i];
                    const float w = 0.5F - ( 0.5F*qFFMath_Cos( QFFM_PI*( x - x0 )/( x1 - x0 ) ) );
                    y = y0 + ( w*( y1 - y0 ) );
                }
            }
        }
    }
    return y;
}
/*============================================================================*/
static float qInterp1_cubic( const float x,
                             const float * const tx,
                             const float * const ty,
                             const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 4U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        if ( x < tx[ 0 ] ) {
            const float x0 = tx[ 0 ];
            const float x1 = tx[ 1 ];
            const float y0 = ty[ 0 ];
            const float y1 = ty[ 1 ];
            const float h = x1 - x0;
            const float t = ( x - x0 )/h;
            const float t2 = t*t;
            const float t3 = t2*t;

            y = ( ( ( 2.0F*t3 ) - ( 3.0F*t2 ) + 1.0F )*y0 ) +
                ( ( t3 - ( 2.0F*t2 ) + t )*h*( y0 - y1 ) ) +
                ( ( -( 2.0F*t3 ) + ( 3.0F*t2 ) )*y1 ) +
                ( ( t3 - t2 )*h*( y1 - y0 ) );
        }
        else if ( x > tx[ tableSize - 1U ] ) {
            const float x0 = tx[ tableSize - 2U ];
            const float x1 = tx[ tableSize - 1U ];
            const float y0 = ty[ tableSize - 2U ];
            const float y1 = ty[ tableSize - 1U ];
            const float h = x1 - x0;
            const float t = ( x - x1 )/h;
            const float t2 = t*t;
            const float t3 = t2*t;

            y = ( ( ( 2.0F*t3 ) - ( 3.0F*t2 ) + 1.0F )*y1 ) +
                ( ( t3 - ( 2.0F*t2 ) + t )*h*( y0 - ty[ tableSize - 3U ] ) ) +
                ( ( -( 2.0F*t3 ) + ( 3.0F*t2 ) )*y0 ) +
                ( ( t3 - t2 )*h*( y1 - y0 ) );
        }
        else {
            for ( size_t i = 1U; i < tableSize; ++i ) {
                if ( x <= tx[ i ] ) {
                    const float x0 = tx[ i - 1U ];
                    const float x1 = tx[ i ];
                    const float y0 = ty[ i - 1U ];
                    const float y1 = ty[ i ];
                    const float h = x1 - x0;
                    const float t = ( x - x0 )/h;
                    const float t2 = t*t;
                    const float t3 = t2*t;
                    y = ( ( ( 2.0F*t3 ) - ( 3.0F*t2 ) + 1.0F )*y0 ) +
                        ( ( t3 - ( 2.0F*t2 ) + t )*h*( y0 - ty[ i - 2U ] ) ) +
                        ( ( -( 2.0F*t3 ) + ( 3.0F*t2 ) )*y1 ) +
                        ( ( t3 - t2 )*h*( y1 - y0 ) );
                    break;
                }
            }
        }
    }
    return y;
}
/*============================================================================*/
static float qInterp1_hermite( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 2U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        if ( x < tx[ 0 ] ) {
            const float x0 = tx[ 0 ];
            const float x1 = tx[ 1 ];
            const float y0 = ty[ 0 ];
            const float y1 = ty[ 1 ];
            y = y0 + ( ( ( y1 - y0 )/( x1 - x0 ) )*( x - x0 ) );
        }
        else if ( x > tx[ tableSize -1U ] ) {
            const float x0 = tx[ tableSize - 2U ];
            const float x1 = tx[ tableSize - 1U ];
            const float y0 = ty[ tableSize - 2U ];
            const float y1 = ty[ tableSize - 1U ];
            y = y1 + ( ( ( y0 - y1 )/( x0 - x1 ) )*( x - x1 ) );
        }
        else {
            y = 0.0F;
            for ( size_t i = 0U ; i < tableSize; ++i ) {
                float term = ty[ i ];

                for ( size_t j = 0U ; j < tableSize; ++j ) {
                    if ( i != j ) {
                        term *= ( x - tx[ j ] )/( tx[ i ] - tx[ j ] );
                    }
                }

                y += term;
            }
        }
    }
    return y;
}
/*============================================================================*/
static float qInterp1_spline( const float x,
                              const float * const tx,
                              const float * const ty,
                              const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 4U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        size_t i = 0U;
        /* Extrapolation for x beyond the range*/
        if ( x <= tx[ 0 ] ) {
            i = 0U;
        }
        else if ( x >= tx[ tableSize - 1U ] ) {
            i = tableSize - 2U; /* Use the last interval for extrapolation*/
        }
        else {
            while ( x >= tx[ i + 1U ] ) {
                i++;
            }
        }

        if ( qFFMath_IsEqual( x , tx[ i + 1U ] ) ) {
            y = ty[ i + 1U ];
        }
        else {
            const float t = ( x - tx[ i ] )/( tx[ i + 1U ] - tx[ i ] );
            const float t_2 = t*t;
            const float t_3 = t_2*t;
            const float tt_3 = 2.0F*t_3;
            const float tt_2 = 3.0F*t_2;
            const float h01 = tt_2 - tt_3;
            const float h00 = 1.0F - h01;
            const float h10 = t_3 - ( 2.0F*t_2 ) + t;
            const float h11 = t_3 - t_2;
            const float x1_x0 = tx[ i + 1U ] - tx[ i ];
            const float y0 = ty[ i ];
            const float y1 = ty[ i + 1U ];
            float m0;
            float m1;

            if ( 0U == i ) {
                m0 = ( ty[ 1 ] - ty[ 0 ] )/( tx[ 1 ] - tx[ 0 ] );
                m1 = ( ty[ 2 ] - ty[ 0 ] )/( tx[ 2 ] - tx[ 0 ] );
            }
            else if ( ( tableSize - 2U ) == i ) {
                m0 = ( ty[ tableSize - 1U ] - ty[ tableSize - 3U ] )/( tx[ tableSize - 1U ] - tx[ tableSize - 3U ] );
                m1 = ( ty[ tableSize - 1U ] - ty[ tableSize - 2U ] )/( tx[ tableSize - 1U ] - tx[ tableSize - 2U ] );
            }
            else {
                m0 = slope( tx, ty, i );
                m1 = slope( tx, ty, i + 1U );
            }
            y = ( h00*y0 ) + ( h01*y1 ) + ( h10*x1_x0*m0 ) + ( h11*x1_x0*m1 );
        }
    }

    return y;
}
/*============================================================================*/
static float qInterp1_cSpline( const float x,
                               const float * const tx,
                               const float * const ty,
                               const size_t tableSize )
{
    float y = QFFM_NAN;

    if ( ( tableSize >= 4U ) && ( NULL != tx ) && ( NULL != ty ) ) {
        size_t i = 0U;
        /* Extrapolation for x beyond the range*/
        if ( x <= tx[ 0 ] ) {
            i = 0U;
        }
        else if ( x >= tx[ tableSize - 1U ] ) {
            i = tableSize - 2U; /* Use the last interval for extrapolation*/
        }
        else {
            while ( x >= tx[ i + 1U ] ) {
                i++;
            }
        }

        if ( qFFMath_IsEqual( x , tx[ i + 1U ] ) ) {
            y = ty[ i + 1U ];
        }
        else {
            const float x0 = tx[ i ];
            const float x1 = tx[ i + 1U ];
            const float y0 = ty[ i ];
            const float y1 = ty[ i + 1U ];
            const float fd2i_xl1 = leftSecondDerivate( tx, ty, tableSize - 1U, i + 1U );
            const float fd2i_x = rightSecondDerivate( tx, ty, tableSize - 1U, i + 1U );
            const float x0_x1 = x0 - x1;
            const float x1_2 = x1*x1;
            const float x1_3 = x1_2*x1;
            const float x0_2 = x0*x0;
            const float x0_3 = x0_2*x0;
            const float inv_x0_x1 = 1.0F/x0_x1;
            const float d = ( fd2i_x - fd2i_xl1 )*( 0.166666667F*inv_x0_x1 );
            const float c = ( ( x0*fd2i_xl1 ) - ( x1*fd2i_x ) )*( 0.5F*inv_x0_x1 );
            const float b = ( y0 - y1 - ( c*( x0_2 - x1_2 ) ) - ( d*( x0_3 - x1_3 ) ) )*inv_x0_x1;
            const float a = y1 - ( b*x1 ) - ( c*x1_2 ) - ( d*x1_3 );
            y = a + ( x*( b + ( x*( c + ( x*d ) ) ) ) );
        }
    }

    return y;
}
/*============================================================================*/
static float slope( const float * const tx,
                    const float * const ty,
                    const size_t i )
{
    float m;

    if ( qFFMath_IsEqual( tx[ i + 1U ], tx[ i - 1U ] ) ) {
        m = 0.0F;
    }
    else {
        m = ( ty[ i + 1U ] - ty[ i - 1U ] )/( tx[ i + 1U ] - tx[ i - 1U ] );
    }

    return m;
}
/*============================================================================*/
float firstDerivate( const float * const tx,
                     const float * const ty,
                     const size_t n,
                     const size_t i )
{
    float fd1_x;

    if ( 0U == i ) {
        const float dx = tx[ 1 ] - tx[ 0 ];
        const float dy = ty[ 1 ] - ty[ 0 ];
        fd1_x = 1.5F*( dy/dx );
        fd1_x -= 1.0F/( ( ( tx[ 2 ] - tx[ 0 ] )/( ty[ 2 ] - ty[ 0 ] ) ) + ( dx/dy ) );
    }
    else if ( n == i ) {
        const float dx = tx[ n ] - tx[ n - 1U ];
        const float dy = ty[ n ] - ty[ n - 1U ];
        fd1_x = 1.5F*( dy/dx );
        fd1_x -= 1.0F/( ( ( tx[ n ] - tx[ n - 2U ] )/( ty[ n ] - ty[ n - 2U ] ) ) + ( dx/dy ) );
    }
    else {
        const float tmp1 = ( tx[ i + 1U ] - tx[ i ] )/( ty[ i + 1U ] - ty[ i ] );
        const float tmp2 = ( tx[ i ] - tx[ i - 1U ] )/( ty[ i ] - ty[ i - 1U ] );

        if ( ( tmp1*tmp2 ) < 0.0F ) {
            fd1_x = 0.0F;
        }
        else {
            fd1_x = 2.0F/( tmp1 + tmp2 );
        }
    }
    return fd1_x;
}
/*============================================================================*/
float leftSecondDerivate( const float * const tx,
                          const float * const ty,
                          const size_t n,
                          const size_t i )
{
    const float fdi_x = firstDerivate( tx, ty, n, i );
    const float fdi_xl1 = firstDerivate( tx, ty, n, i - 1U );
    const float xi_delta = tx[ i ] - tx[ i - 1U ];
    float fd2l_x = -2.0F*( fdi_x + ( 2.0F*fdi_xl1 ) )/xi_delta;

    fd2l_x += 6.0F*( ty[ i ] - ty[ i - 1U ] )/( xi_delta*xi_delta );
    return fd2l_x;
}
/*============================================================================*/
float rightSecondDerivate( const float * const tx,
                           const float * const ty,
                           const size_t n,
                           const size_t i )
{
    const float fdi_x = firstDerivate( tx, ty, n, i );
    const float fdi_xl1 = firstDerivate( tx, ty, n, i - 1U );
    const float xi_delta = tx[ i ] - tx[ i - 1U ];
    float fd2r_x = 2.0F*( ( 2.0F*fdi_x ) + fdi_xl1 )/xi_delta;

    fd2r_x -= 6.0F*( ty[ i ] - ty[ i - 1U ] )/( xi_delta*xi_delta );
    return fd2r_x;
}
/*============================================================================*/

/*cstat +CERT-INT30-C_a*/