/*!
 * @file qpid.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qpid.h"

static float qPID_Sat( float x, float min, float max );
static void qPID_AdaptGains( qPID_controller_t *c,  float e, float u );
static int qPID_ATCheck( float x );

/*============================================================================*/
int qPID_Setup( qPID_controller_t *c, float kc, float ki, float kd, float dt )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( dt > 0.0f ) ) {
        c->dt = dt;
        c->init = 1u;
        c->adapt = NULL;
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
        if ( NULL != c->adapt ) {

        }
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
        v  = ( c->kc*e ) + ( c->ki*c->ie ) + ( c->kd*c->D ); /*compute the PID action*/
        
        u = qPID_Sat( v, c->min, c->max );
        c->u1 = c->kw*( u - v ); /*anti-windup feedback*/
        if ( NULL != c->uEF ) { /*tracking mode*/
            c->u1 += c->kt*( c->uEF[ 0 ] - u );
        }
        c->e1 = e;
        if( NULL != c->adapt ) {
            qPID_AdaptGains( c, u, y );
        }
    }
    return u;
}
/*============================================================================*/
int qPID_BindAutoTunning( qPID_controller_t *c, qPID_AutoTunning_t *at )
{
    int retVal = 0;
    if ( ( NULL != c ) && ( NULL != at ) && ( 0u != c->init ) ) {
        c->adapt = at;
        at->l = 0.9898f;
        at->il = 1.0f/at->l;
        at->p00 = 1000.0f;
        at->p11 = 1000.0f;
        at->p01 = 0.0f;
        at->p10 = 0.0f;
        at->uk  = 0.0f;
        at->yk = 0.0f;
        at->k = 0.0f;
        at->T = 0.0f;
        at->it = 100uL;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qPID_EnableAutoTunning( qPID_controller_t *c, uint32_t tEnable )
{
    int retVal = 0;
    if ( NULL != c ) {
        if ( NULL != c->adapt ) {
            c->adapt->it = tEnable;
            retVal = 1;
        }
    }   
    return retVal;
}
/*============================================================================*/
static void qPID_AdaptGains( qPID_controller_t *c,  float u, float y )
{
    qPID_AutoTunning_t *s = c->adapt; 
    float error , r, l0, l1;
    float lp00, lp01, lp10, lp11;
    float l0uk1, l1yk1, k, T, a1;
    float p00uk, p11yk;
    p00uk = s->p00*s->uk;
    p11yk = s->p11*s->yk;
    r = s->l 
        + ( s->uk*( p00uk - ( s->p10*s->yk ) ) ) 
        - ( s->yk*( ( s->p01*s->uk ) - p11yk ) );
    /*compute corrections*/    
    l0 = ( p00uk - ( s->p01*s->yk ) )/r;
    l1 = ( ( s->p10*s->uk ) - p11yk )/r;
    error = y - ( ( s->b1*s->uk ) - ( s->a1*s->yk ) );
    /*fix estimations*/
    s->b1 += l0*error;
    s->a1 += l1*error;
    lp00 = s->il*s->p00;
    lp01 = s->il*s->p01;
    lp10 = s->il*s->p10;
    lp11 = s->il*s->p11;
    l0uk1 = ( l0*s->uk ) - 1.0f;
    l1yk1 = ( l1*s->yk ) + 1.0f;
    /*update covariances*/
    s->p00 = ( l0*lp10*s->yk ) - ( lp00*l0uk1 ) + 1e-10f;
    s->p01 = ( l0*lp11*s->yk ) - ( lp01*l0uk1 );
    s->p10 = ( lp10*l1yk1 ) - ( l1*lp00*s->uk );
    s->p11 = ( lp11*l1yk1 ) - ( l1*lp01*s->uk ) + 1e-10f;
    s->yk = y;
    s->uk = u;
    k = s->b1/( 1.0f + s->a1 );
    a1 = ( s->a1 < 0.0f )? -s->a1 : s->a1;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Rule-13.5*/
    T = -c->dt/logf( a1 ); /*ok, passing absolute value*/
    if ( ( 0 != qPID_ATCheck( T ) ) && ( 0 != qPID_ATCheck( k ) ) && ( s->it > 0uL ) ) {
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Rule-13.5*/
        float a, t; 
        s->k = k + ( 0.95f*( s->k - k ) );
        s->T = T + ( 0.95f*( s->T - T ) );    
        t = c->dt/s->T ;
        a = ( 1.35f + ( 0.25f*t ) );
        c->kc = 0.2f*a*( s->T/( s->k*c->dt ) );
        c->ki = ( ( 0.2f*c->kc )*( 0.54f + ( 0.33f*t ) ) )/( a*c->dt );
        c->kd = ( 0.1f*c->kc*c->dt )/a;
        if ( s->it < QPID_AUTOTUNNING_UNDEFINED) {
            s->it--;
        }
    }
}
/*============================================================================*/
static float qPID_Sat( float x, float min, float max )
{
    if ( x > max ) {
        x = max;
    }
    else if ( x < min ) {
        x = min;
    }
    else {
        x = x;
    }
    return x;
}
/*============================================================================*/
static int qPID_ATCheck( float x )
{
    /*cstat -MISRAC2012-Rule-13.5 -MISRAC2012-Rule-10.3*/
    return ( 0 == (int)isnan( x ) ) && ( x > 0.0f ) && ( 0 == (int)isinf( x ) ); 
    /*cstat +MISRAC2012-Rule-13.5 +MISRAC2012-Rule-10.3*/
}