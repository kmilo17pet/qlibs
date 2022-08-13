/*!
 * @file qnuma.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qnuma.h"

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
                        const float dt )
{
    x->x[ 0 ] += s*dt;
    qNumA_Update( x );
    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralTr( qNumA_state_t *x,
                        const float s,
                        const float dt )
{
    x->x[ 0 ] += 0.5f*( s + x->x[ 1 ] )*dt;
    qNumA_Update( x );
    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralSi( qNumA_state_t *x,
                        const float s,
                        const float dt )
{
    x->x[ 0 ] += ( 1.0f/6.0f )*( s + ( 4.0f*x->x[ 1 ] ) + x->x[ 2 ] )*dt;
    qNumA_Update( x );
    return x->x[ 0 ];
}
/*============================================================================*/
float qNumA_Derivative( qNumA_state_t *x,
                        const float s,
                        const float dt )
{
    x->x[ 0 ]= ( s - x->x[ 1 ] )/dt;
    qNumA_Update( x );
    return x->x[ 0 ];
}
/*============================================================================*/