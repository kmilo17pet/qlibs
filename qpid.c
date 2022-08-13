/*!
 * @file qpid.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qpid.h"

static float qPID_Sat( float x,
                       const float min,
                       const float max );
static void qPID_AdaptGains( qPID_controller_t * const c,
                             const float u,
                             const float y );
static int qPID_ATCheck( const float x );

/*============================================================================*/
int qPID_Setup( qPID_controller_t * const c,
                const float kc,
                const float ki,
                const float kd,
                const float dt )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( dt > 0.0f ) ) {
        c->dt = dt;
        c->init = 1u;
        c->adapt = NULL;
        c->integrate = &qNumA_IntegralTr; /*default integration method*/
        (void)qPID_SetDerivativeFilter( c, 0.98f );
        (void)qPID_SetEpsilon( c, FLT_MIN );
        (void)qPID_SetGains( c, kc, ki, kd );
        (void)qPID_SetSaturation( c , 0.0f, 100.0f, 1.0f );
        (void)qPID_SetTrackingMode( c, NULL, 1.0f );
        (void)qPID_SetMRAC( c, NULL, 0.5f );
        retValue = qPID_Reset( c );
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetGains( qPID_controller_t * const c,
                   const float kc,
                   const float ki,
                   const float kd )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->kc = kc;
        c->ki = ki;
        c->kd = kd;
        if ( NULL != c->adapt ) {
            /*to be defined*/
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_Reset( qPID_controller_t * const c )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        qNumA_StateInit( &c->c_state, 0.0f, 0.0f, 0.0f );
        c->D = 0.0f;
        c->u1 = 0.0f;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetSaturation( qPID_controller_t * const c,
                        const float min,
                        const float max,
                        const float kw )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( max > min ) && ( 0u != c->init ) && ( kw >= 0.0f ) ) {
        c->min = min;
        c->max = max;
        c->kw = kw;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetSeries( qPID_controller_t * const c )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        float ti, td, tmp;

        ti = c->kc/c->ki;
        td = c->kd/c->kc;
        tmp = 1.0f + ( td/ti );
        c->kc = c->kc*tmp;
        c->ki = c->kc/( ti*tmp );
        c->kd = c->kc*( td/tmp );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetEpsilon( qPID_controller_t * const c,
                     const float eps )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init) && ( eps > 0.0f ) ) {
        c->epsilon = eps;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetDerivativeFilter( qPID_controller_t * const c,
                              const float beta )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) && ( beta > 0.0f ) && ( beta < 1.0f ) ) {
        c->beta = beta;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetTrackingMode( qPID_controller_t * const c,
                          float *var,
                          const float kt )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( kt > 0.0f ) && ( 0u != c->init ) ) {
        c->kt = kt;
        c->uEF = var;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetMRAC( qPID_controller_t * const c,
                  const float *modelref,
                  const float gamma )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( gamma > 0.0f ) ) {
        qNumA_StateInit( &c->m_state, 0.0f, 0.0f, 0.0f );
        c->alfa = 0.01f;
        c->gamma = gamma;
        c->yr = modelref;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
float qPID_Control( qPID_controller_t * const c,
                    const float w,
                    const float y )
{
    float u = w;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        float e, v, de, ie;

        e = w - y;
        if ( fabs( e ) <= ( c->epsilon ) ) {
            e = 0.0f;
        }
        /*integral with anti-windup*/
        ie = c->integrate( &c->c_state,  e + c->u1, c->dt );
        de = qNumA_Derivative( &c->c_state, e, c->dt  );
        c->D = de + ( c->beta*( c->D - de ) ); /*derivative filtering*/
        v  = ( c->kc*e ) + ( c->ki*ie ) + ( c->kd*c->D ); /*compute PID action*/

        if ( NULL != c->yr ) {
            /*MRAC additive controller using the modified MIT rule*/
            float theta = 0.0f;
            if ( ( c->u1*c->u1 ) <= c->epsilon ) { /*additive anti-windup*/
                const float em = y - c->yr[ 0 ];
                const float delta = -c->gamma*em*c->yr[ 0 ]/
                                    ( c->alfa + ( c->yr[ 0 ]*c->yr[ 0 ] ) );
                theta = c->integrate( &c->m_state, delta, c->dt );
            }
            v += w*theta;
        }

        u = qPID_Sat( v, c->min, c->max );
        c->u1 = c->kw*( u - v ); /*anti-windup feedback*/
        if ( NULL != c->uEF ) { /*tracking mode*/
            c->u1 += c->kt*( c->uEF[ 0 ] - u );
        }

        if ( NULL != c->adapt ) {
            qPID_AdaptGains( c, u, y );
        }
    }
    return u;
}
/*============================================================================*/
int qPID_BindAutoTunning( qPID_controller_t * const c,
                          qPID_AutoTunning_t * const at )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        if ( NULL != at ) {
            float k,T;

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
            at->tao = 0.0f;
            at->it = 100uL;
            at->mu = 0.95f;
            k = c->kc/0.9f;
            T = ( 0.27f*k )/c->ki;
            at->a1 = -expf( -c->dt/T );
            at->b1 = k*( 1.0f + at->a1 );
            at->speed = 0.25f;
        }
        else {
            c->adapt = NULL;
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_EnableAutoTunning( qPID_controller_t * const c,
                            const uint32_t tEnable )
{
    int retValue = 0;

    if ( NULL != c ) {
        if ( NULL != c->adapt ) {
            c->adapt->it = tEnable;
            retValue = 1;
        }
    }

    return retValue;
}
/*============================================================================*/
static void qPID_AdaptGains( qPID_controller_t *c,
                             const float u,
                             const float y )
{
    qPID_AutoTunning_t *s = c->adapt;
    float error , r, l0, l1;
    float lp00, lp01, lp10, lp11;
    float k, tao, tmp1, tmp2;

    tmp1 = s->p00*s->uk;
    tmp2 = s->p11*s->yk;
    r = s->l +( s->uk*( tmp1 - ( s->p10*s->yk ) ) ) - ( s->yk*( ( s->p01*s->uk ) - tmp2 ) );
    /*compute corrections*/
    l0 = ( tmp1 - ( s->p01*s->yk ) )/r;
    l1 = ( ( s->p10*s->uk ) - tmp2 )/r;
    error = y - ( ( s->b1*s->uk ) - ( s->a1*s->yk ) );
    /*fix estimations*/
    s->b1 += l0*error;
    s->a1 += l1*error;
    /*update covariances*/
    lp00 = s->il*s->p00;
    lp01 = s->il*s->p01;
    lp10 = s->il*s->p10;
    lp11 = s->il*s->p11;
    tmp1 = ( l0*s->uk ) - 1.0f;
    tmp2 = ( l1*s->yk ) + 1.0f;
    s->p00 = ( l0*lp10*s->yk ) - ( lp00*tmp1 ) + 1e-10f;
    s->p01 = ( l0*lp11*s->yk ) - ( lp01*tmp1 );
    s->p10 = ( lp10*tmp2 ) - ( l1*lp00*s->uk );
    s->p11 = ( lp11*tmp2 ) - ( l1*lp01*s->uk ) + 1e-10f;
    /*update I/O measurements*/
    s->yk = y;
    s->uk = u;
    k = s->b1/( 1.0f + s->a1 );
    tmp1 = ( s->a1 < 0.0f )? -s->a1 : s->a1;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Rule-13.5*/
    tao = -c->dt/logf( tmp1 ); /*ok, passing absolute value*/
    if ( ( 0 != qPID_ATCheck( tao ) ) && ( 0 != qPID_ATCheck( k ) ) && ( s->it > 0uL ) ) {
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Rule-13.5*/
        s->k = k + ( s->mu*( s->k - k ) );
        s->tao = tao + ( s->mu*( s->tao - tao ) );
        tmp1 = c->dt/s->tao;
        tmp2 = ( 1.35f + ( 0.25f*tmp1 ) );
        c->kc = ( s->speed*tmp2*s->tao )/( s->k*c->dt );
        c->ki = ( ( s->speed*c->kc )*( 0.54f + ( 0.33f*tmp1 ) ) )/( tmp2*c->dt );
        c->kd = ( 0.5f*s->speed*c->kc*c->dt )/tmp2;
        if ( s->it < QPID_AUTOTUNNING_UNDEFINED ) {
            s->it--;
        }
    }
}
/*============================================================================*/
static float qPID_Sat( float x,
                       const float min,
                       const float max )
{
    if ( x > max ) {
        x = max;
    }
    else if ( x < min ) {
        x = min;
    }
    else {
        /*nothing to do*/
    }

    return x;
}
/*============================================================================*/
static int qPID_ATCheck( const float x )
{
    /*cstat -MISRAC2012-Rule-13.5 -MISRAC2012-Rule-10.3*/
    return ( 0 == (int)isnan( x ) ) && ( x > 0.0f ) && ( 0 == (int)isinf( x ) );
    /*cstat +MISRAC2012-Rule-13.5 +MISRAC2012-Rule-10.3*/
}
/*============================================================================*/
