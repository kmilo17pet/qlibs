/*!
 * @file qnuma.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qnuma.h"

/*============================================================================*/
void qNumA_StateInit( qNumA_state_t x, const float x0, const float sn_1, const float sn_2 )
{
    x[ 0 ] = x0;
    x[ 1 ] = sn_1;
    x[ 2 ] = sn_2;
}
/*============================================================================*/
float qNumA_IntegralRe( qNumA_state_t x, const float s, const float dt )
{
    x[ 0 ] += s*dt;
    x[ 2 ] = x[ 1 ]; /*not used, but update anyway*/
    x[ 1 ] = s; /*not used, but update anyway*/
    return x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralTr( qNumA_state_t x, const float s, const float dt )
{
    x[ 0 ] += 0.5f*( s + x[ 1 ] )*dt;
    x[ 2 ] = x[ 1 ]; /*not used, but update anyway*/
    x[ 1 ] = s; 
    return x[ 0 ];
}
/*============================================================================*/
float qNumA_IntegralSi( qNumA_state_t x, const float s, const float dt )
{
    x[ 0 ] += ( 1.0f/6.0f )*( s + ( 4.0f*x[ 1 ] ) + x[ 2 ] )*dt;
    x[ 2 ] = x[ 1 ]; /*not used, but update anyway*/
    x[ 1 ] = s; 
    return x[ 0 ];
}
/*============================================================================*/
float qNumA_Derivative( qNumA_state_t x, const float s, const float dt )
{
    x[ 0 ] = ( s - x[ 1 ] )/dt;
    x[ 2 ] = x[ 1 ]; /*not used, but update anyway*/
    x[ 1 ] = s; 
    return x[ 0 ];
}
/*============================================================================*/