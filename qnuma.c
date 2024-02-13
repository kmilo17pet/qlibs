/*!
 * @file qnuma.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qnuma.h"

/*cppcheck-suppress misra-c2012-20.7 */
#define qNumA_Update( x )       (x)->x[ 2 ] = (x)->x[ 1 ];  \
                                (x)->x[ 1 ] = s             \

/*============================================================================*/
void qNumA_StateInit( qNumA_state_t *x,
                      const float x0,
                      const float sn_1,
                      const float sn_2 )
{
    x->x[ 0 ] = x0;
    x->x[ 1 ] = sn_1;
    x->x[ 2 ] = sn_2;
}
/*============================================================================*/
float qNumA_IntegralRe( qNumA_state_t *x,
                        const float s,
                        const float dt,
                        const bool bUpdate )
{
    x->x[ 0 ] += s*dt;
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralTr( qNumA_state_t *x,
                        const float s,
                        const float dt,
                        const bool bUpdate )
{
    x->x[ 0 ] += 0.5F*( s + x->x[ 1 ] )*dt;
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralSi( qNumA_state_t *x,
                        const float s,
                        const float dt,
                        const bool bUpdate )
{
    x->x[ 0 ] += ( 1.0F/6.0F )*( s + ( 4.0F*x->x[ 1 ] ) + x->x[ 2 ] )*dt;
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_Derivative2p( qNumA_state_t *x,
                          const float s,
                          const float dt,
                          const bool bUpdate )
{
    float ds;

    ds = ( s - x->x[ 1 ] )/dt;
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return ds;
}
/*============================================================================*/
float qNumA_DerivativeBa( qNumA_state_t *x,
                          const float s,
                          const float dt,
                          const bool bUpdate )
{
    float ds;

    ds = ( ( 3.0F*s ) - ( 4.0F*x->x[ 1 ] ) + x->x[ 2 ] )/( 2.0F*dt );
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return ds;
}
/*============================================================================*/
float qNumA_DerivativeFo( qNumA_state_t *x,
                          const float s,
                          const float dt,
                          const bool bUpdate )
{
    float ds;

    ds = ( ( 4.0F*x->x[ 1 ] ) - ( 3.0F*x->x[ 2 ] ) - s )/( 2.0F*dt );
    if ( bUpdate ) {
        qNumA_Update( x );
    }

    return ds;
}