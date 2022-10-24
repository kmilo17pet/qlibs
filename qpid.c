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
        (void)qPID_SetMRAC( c, NULL, 0.5f );
        (void)qPID_SetMode( c, qPID_Automatic );
        (void)qPID_SetManualInput( c, 0.0f );
        (void)qPID_SetExtraGains( c, 1.0f, 1.0f );
        qPID_RemoveDerivativeKick( c, 1u );
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
int qPID_SetExtraGains( qPID_controller_t * const c,
                        const float kw,
                        const float kt )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->kw = kw;
        c->kt = kt;
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
int qPID_SetMode( qPID_controller_t * const c,
                  const qPID_Mode_t m )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->mode = m;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetManualInput( qPID_controller_t * const c,
                         float manualInput )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0u != c->init ) ) {
        c->mInput = manualInput;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetMRAC( qPID_controller_t * const c,
                  const float *modelRef,
                  const float gamma )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( gamma > 0.0f ) ) {
        qNumA_StateInit( &c->m_state, 0.0f, 0.0f, 0.0f );
        c->alfa = 0.01f;
        c->gamma = gamma;
        c->yr = modelRef;
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
        float e, v, de, ie, m, bt, sw;

        e = w - y;
        if ( fabs( e ) <= ( c->epsilon ) ) {
            e = 0.0f;
        }
        /*integral with anti-windup*/
        ie = c->integrate( &c->c_state,  e + c->u1, c->dt );
        de = qNumA_Derivative( &c->c_state, ( 1u == c->dKick ) ? -y : e, c->dt  );
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
        /*bumpless-transfer*/
        bt = c->kt*c->mInput + c->kw*( u - m );
        m = c->integrate( &c->b_state, bt ,c->dt );
        sw = ( qPID_Automatic == c->mode ) ? v : m; 
        u = qPID_Sat( sw, c->min, c->max );
        /*anti-windup feedback*/
        c->u1 = c->kw*( u - v );
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
            at->it = QPID_AUTOTUNNING_UNDEFINED;
        }
        else {
            c->adapt = NULL;
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_RemoveDerivativeKick( qPID_controller_t * const c,
                               const uint32_t tEnable )
{
    int retValue = 0;

    if ( NULL != c ) {
        c->dKick = ( 0uL != tEnable )? 1u : 0u;
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
            c->adapt->it = ( 0uL == tEnable )? QPID_AUTOTUNNING_UNDEFINED : tEnable;
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
    tmp1 = ( s->a1 < 0.0f ) ? -s->a1 : s->a1;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Rule-13.5*/
    tao = -c->dt/logf( tmp1 ); /*ok, passing absolute value*/
    if ( ( 0 != qPID_ATCheck( tao ) ) && ( 0 != qPID_ATCheck( k ) ) && ( s->it > 0uL ) ) {
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Rule-13.5*/
        s->k = k + ( s->mu*( s->k - k ) );
        s->tao = tao + ( s->mu*( s->tao - tao ) );
        if ( ( 0uL == --s->it ) && ( s->it != QPID_AUTOTUNNING_UNDEFINED ) ) {
            tmp1 = c->dt/s->tao;
            tmp2 = ( 1.35f + ( 0.25f*tmp1 ) );
            c->kc = ( s->speed*tmp2*s->tao )/( s->k*c->dt );
            c->ki = ( ( s->speed*c->kc )*( 0.54f + ( 0.33f*tmp1 ) ) )/( tmp2*c->dt );
            c->kd = ( 0.5f*s->speed*c->kc*c->dt )/tmp2;
        }
    }
}
/*============================================================================*/
int qPID_AutoTunningComplete( const qPID_controller_t * const c )
{
    int retVal = 0;

    if ( NULL != c ) {
        if ( NULL != c->adapt ) {
            retVal = ( ( 0uL == c->adapt->it ) && 
                       ( c->adapt->it != QPID_AUTOTUNNING_UNDEFINED )
                     ) ? 1 : 0;
        }
    }

    return retVal;
}
/*============================================================================*/
int qPID_AutoTunningSetParameters( qPID_controller_t * const c,
                                   const float mu,
                                   const float alfa,
                                   const float lambda )
{
    int retVal = 0;

    if ( ( NULL != c ) && ( mu > 0.0f ) && ( mu <= 1.0f ) &&
         ( alfa > 0.0f ) && ( alfa <= 1.0f ) &&
         ( lambda >= 0.8f ) && ( lambda <= 1.0f ) ) {
        if ( NULL != c->adapt ) {
            c->adapt->l = lambda;
            c->adapt->mu = mu;
            c->adapt->speed = alfa;
            retVal = 1;
        }
    }

    return retVal;
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
int qPID_SetIntegrationMethod( qPID_controller_t * const c,
                               qNumA_IntegrationMethod_t im )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( NULL != im ) ) {
        if ( ( im == &qNumA_IntegralRe ) || ( im == &qNumA_IntegralTr ) || ( im == &qNumA_IntegralSi ) ) {
            c->integrate = im;
            retValue = 1;
        }
    }

    return retValue;
}
/*============================================================================*/
