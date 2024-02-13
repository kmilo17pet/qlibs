/*!
 * @file qpid.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qpid.h"
#include "qffmath.h"
#include <float.h>

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
        c->init = 1U;
        c->adapt = NULL;
        c->integrate = &qNumA_IntegralTr; /*default integration method*/
        (void)qPID_SetDerivativeFilter( c, 0.98F );
        (void)qPID_SetEpsilon( c, FLT_MIN );
        (void)qPID_SetGains( c, kc, ki, kd );
        (void)qPID_SetSaturation( c , 0.0F, 100.0F );
        (void)qPID_SetMRAC( c, NULL, 0.5F );
        (void)qPID_SetMode( c, qPID_Automatic );
        (void)qPID_SetManualInput( c, 0.0F );
        (void)qPID_SetExtraGains( c, 1.0F, 1.0F );
        (void)qPID_SetDirection( c, qPID_Forward );
        (void)qPID_SetReferenceWeighting( c, 1.0F, 0.0F );
        retValue = qPID_Reset( c );
    }

    return retValue;

}
/*============================================================================*/
int qPID_SetDirection( qPID_controller_t * const c,
                       const qPID_Direction_t d )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        c->dir = d;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetParams( qPID_controller_t * const c,
                    const float kc,
                    const float ti,
                    const float td )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        c->kc = kc;
        c->ki = kc/ti;
        c->kd = kc*td;
        retValue = 1;
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

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        c->kc = kc;
        c->ki = ki;
        c->kd = kd;
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

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
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

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        qNumA_StateInit( &c->c_state, 0.0F, 0.0F, 0.0F );
        qNumA_StateInit( &c->m_state, 0.0F, 0.0F, 0.0F );
        qNumA_StateInit( &c->b_state, 0.0F, 0.0F, 0.0F );
        c->D = 0.0F;
        c->u1 = 0.0F;
        c->m = 0.0F;
        c->uSat = 0.0F;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetSaturation( qPID_controller_t * const c,
                        const float min,
                        const float max )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( max > min ) && ( 0U != c->init ) ) {
        c->min = min;
        c->max = max;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetSeries( qPID_controller_t * const c )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        float ti, td, tmp;

        ti = c->kc/c->ki;
        td = c->kd/c->kc;
        tmp = 1.0F + ( td/ti );
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

    if ( ( NULL != c ) && ( 0U != c->init) && ( eps > 0.0F ) ) {
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

    if ( ( NULL != c ) && ( 0U != c->init ) && ( beta > 0.0F ) && ( beta < 1.0F ) ) {
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

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        c->mode = m;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetReferenceWeighting( qPID_controller_t * const c,
                                const float gb,
                                const float gc )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        c->b = qPID_Sat( gb, 0.0F, 1.0F );
        c->c = qPID_Sat( gc, 0.0F, 1.0F );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_SetManualInput( qPID_controller_t * const c,
                         const float manualInput )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
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

    if ( ( NULL != c ) && ( gamma > 0.0F ) ) {
        qNumA_StateInit( &c->m_state, 0.0F, 0.0F, 0.0F );
        c->alfa = 0.01F;
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

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        float e, v, de, ie, bt, sw, kc, ki, kd;

        kc = c->kc;
        ki = c->ki;
        kd = c->kd;
        if ( qPID_Backward == c->dir ) {
            kc = ( kc > 0.0F ) ? -kc : kc;
            ki = ( ki > 0.0F ) ? -ki : ki;
            kd = ( kd > 0.0F ) ? -kd : kd;
        }

        e = w - y;
        if ( QLIB_ABS( e ) <= c->epsilon ) {
            e = 0.0f;
        }
        /*integral with anti-windup*/
        ie = c->integrate( &c->c_state,  e + c->u1, c->dt, false );
        de = qNumA_Derivative2p( &c->c_state, ( ( c->c*w ) - y ) , c->dt, true );
        c->D = de + ( c->beta*( c->D - de ) ); /*derivative filtering*/
        v  = ( kc*( ( c->b*w ) - y ) ) + ( ki*ie ) + ( kd*c->D ); /*compute PID action*/

        if ( NULL != c->yr ) {
            /*MRAC additive controller using the modified MIT rule*/
            float theta = 0.0F;
            if ( QLIB_ABS( c->u1 ) <= c->epsilon ) { /*additive anti-windup*/
                const float em = y - c->yr[ 0 ];
                const float delta = -c->gamma*em*c->yr[ 0 ]/
                                    ( c->alfa + ( c->yr[ 0 ]*c->yr[ 0 ] ) );
                theta = c->integrate( &c->m_state, delta /*+ c->u1*/, c->dt, true );
            }
            v += w*theta;
        }
        /*bumpless-transfer*/
        bt = ( c->kt*c->mInput ) + ( c->kw*( c->uSat - c->m ) );
        c->m = c->integrate( &c->b_state, bt ,c->dt, true );
        sw = ( qPID_Automatic == c->mode ) ? v : c->m;
        c->uSat = qPID_Sat( sw, c->min, c->max );
        u = c->uSat;
        /*anti-windup feedback*/
        c->u1 = c->kw*( u - v );
        if ( NULL != c->adapt ) {
            qPID_AdaptGains( c, u, y );
        }
    }

    return u;
}
/*============================================================================*/
int qPID_BindAutoTuning( qPID_controller_t * const c,
                         qPID_AutoTuning_t * const at )
{
    int retValue = 0;

    if ( ( NULL != c ) && ( 0U != c->init ) ) {
        if ( NULL != at ) {
            float k,T;

            c->adapt = at;
            at->l = 0.9898F;
            at->il = 1.0F/at->l;
            at->p00 = 1000.0F;
            at->p11 = 1000.0F;
            at->p01 = 0.0F;
            at->p10 = 0.0F;
            at->uk  = 0.0F;
            at->yk = 0.0F;
            at->k = 0.0F;
            at->tao = 0.0F;
            at->it = 100UL;
            at->mu = 0.95F;
            k = c->kc/0.9F;
            T = ( 0.27F*k )/c->ki;
            at->a1 = -QLIB_EXP( -c->dt/T );
            at->b1 = k*( 1.0F + at->a1 );
            at->speed = 0.25F;
            at->it = QPID_AUTOTUNING_UNDEFINED;
        }
        else {
            c->adapt = NULL;
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qPID_EnableAutoTuning( qPID_controller_t * const c,
                           const uint32_t tEnable )
{
    int retValue = 0;

    if ( NULL != c ) {
        if ( NULL != c->adapt ) {
            c->adapt->it = ( 0UL == tEnable )? QPID_AUTOTUNING_UNDEFINED : tEnable;
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
    qPID_AutoTuning_t *s = c->adapt;
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
    tmp1 = ( l0*s->uk ) - 1.0F;
    tmp2 = ( l1*s->yk ) + 1.0F;
    s->p00 = ( l0*lp10*s->yk ) - ( lp00*tmp1 ) + 1e-10F;
    s->p01 = ( l0*lp11*s->yk ) - ( lp01*tmp1 );
    s->p10 = ( lp10*tmp2 ) - ( l1*lp00*s->uk );
    s->p11 = ( lp11*tmp2 ) - ( l1*lp01*s->uk ) + 1e-10F;
    /*update I/O measurements*/
    s->yk = y;
    s->uk = u;
    k = s->b1/( 1.0F + s->a1 );
    tmp1 = ( s->a1 < 0.0F ) ? -s->a1 : s->a1;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Rule-13.5*/
    tao = -c->dt/QLIB_LOG( tmp1 ); /*ok, passing absolute value*/
    if ( ( 0 != qPID_ATCheck( tao ) ) && ( 0 != qPID_ATCheck( k ) ) && ( s->it > 0UL ) ) {
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Rule-13.5*/
        s->k = k + ( s->mu*( s->k - k ) );
        s->tao = tao + ( s->mu*( s->tao - tao ) );
        if ( ( 0UL == --s->it ) && ( s->it != QPID_AUTOTUNING_UNDEFINED ) ) {
            tmp1 = c->dt/s->tao;
            tmp2 = ( 1.35F + ( 0.25F*tmp1 ) );
            c->kc = ( s->speed*tmp2*s->tao )/( s->k*c->dt );
            c->ki = ( ( s->speed*c->kc )*( 0.54F + ( 0.33F*tmp1 ) ) )/( tmp2*c->dt );
            c->kd = ( 0.5F*s->speed*c->kc*c->dt )/tmp2;
        }
    }
}
/*============================================================================*/
int qPID_AutoTuningComplete( const qPID_controller_t * const c )
{
    int retVal = 0;

    if ( NULL != c ) {
        if ( NULL != c->adapt ) {
            /*cppcheck-suppress misra-c2012-10.6 */
            retVal = ( ( 0UL == c->adapt->it ) && ( c->adapt->it != QPID_AUTOTUNING_UNDEFINED ) ) ? 1 : 0;
        }
    }

    return retVal;
}
/*============================================================================*/
int qPID_AutoTuningSetParameters( qPID_controller_t * const c,
                                  const float mu,
                                  const float alfa,
                                  const float lambda )
{
    int retVal = 0;

    if ( ( NULL != c ) && ( mu > 0.0F ) && ( mu <= 1.0F ) &&
         ( alfa > 0.0F ) && ( alfa <= 1.0F ) &&
         ( lambda >= 0.8F ) && ( lambda <= 1.0F ) ) {
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
    return ( 0 == (int)QLIB_ISNAN( x ) ) && ( x > 0.0F ) && ( 0 == (int)QLIB_ISINF( x ) );
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
