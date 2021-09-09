/*!
 * @file qpid.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qpid.h"

/*============================================================================*/
int qPID_Setup( qPID_controller_t *c, float kc, float ki, float kd, float dt )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( dt > 0.0f ) ) {
        c->dt = dt;
        c->init = 1u;
        (void)qPID_SetDerivativeFilter( c, 0.98f );
        (void)qPID_SetEpsilon( c, FLT_MIN );
        (void)qPID_SetGains( c, kc, ki, kd );
        (void)qPID_SetSaturation( c , 0.0f, 100.0f, 1.0f );
        (void)qPID_SetTrackingMode( c, NULL, 1.0f );
        retVal = qPID_Reset( c );
        
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetGains( qPID_controller_t *c, float kc, float ki, float kd )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->kc = kc;
        c->ki = ki;
        c->kd = kd;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_Reset( qPID_controller_t *c )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->e1 = 0.0f;
        c->ie = 0.0f;
        c->D = 0.0f;
        c->u1 = 0.0f;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetSaturation( qPID_controller_t *c, float min, float max, float kw )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( max > min ) && ( 0u != c->init ) && ( kw >= 0.0f ) ) {
        c->min = min;
        c->max = max;
        c->kw = kw;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetSeries( qPID_controller_t *c )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( 0u != c->init) ) {
        float ti, td, tmp;
        ti = c->kc/c->ki;
        td = c->kd/c->kc;
        tmp = 1.0f + ( td/ti );
        c->kc = c->kc*tmp;
        c->ki = c->kc/( ti*tmp );
        c->kd = c->kc*( td/tmp );
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetEpsilon( qPID_controller_t *c, float eps )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( 0u != c->init) && ( eps > 0.0f) ) {
        c->epsilon = eps;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetDerivativeFilter( qPID_controller_t *c, float beta )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( 0u != c->init ) && ( beta > 0.0f) && ( beta < 1.0f) ) {
        c->beta = beta;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_SetTrackingMode( qPID_controller_t *c, float *var, float kt )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( kt > 0.0f) && ( 0u != c->init ) ) {
        c->kt = kt;
        c->uEF = var;
        retVal = 1;
    }
    return retVal;    
}
/*============================================================================*/
float qPID_Control( qPID_controller_t *c, float w, float y )
{
    float u = w;
    if ( ( NULL != c ) && ( 0u != c->init) ) {
        float e, v, de;
        e = w - y;
        if ( fabs( e ) <= ( c->epsilon ) ) {
            e = 0.0f;
        }
        c->ie += ( ( 0.5f*(e + c->e1) ) + c->u1 )*( c->dt ); /*integral with anti-windup*/
        de = ( e - c->e1 )/c->dt;   /*compute the derivative component*/
        c->D = de + ( c->beta*( c->D - de ) ); /*derivative filtering*/
        v  = ( c->kc*e ) + ( c->ki*c->ie ) + ( c->kd*c->D );
        
        u = v;
        if ( u > c->max ) {
            u = c->max;
        }
        if ( u < c->min ) {
            u = c->min;
        }
        c->u1 = c->kw*( u - v ); /*anti-windup feedback*/
        if ( NULL != c->uEF ) { /*tracking mode*/
            c->u1 += c->kt*( c->uEF[ 0 ] - u );
        }
        c->e1 = e;
    }
    return u;
}
/*============================================================================*/