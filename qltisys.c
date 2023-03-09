/*!
 * @file qltisys.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qltisys.h"
#include <float.h>

static float qLTISys_DiscreteUpdate( qLTISys_t * const sys,
                                     const float u );
static float qLTISys_ContinuosUpdate( qLTISys_t * const sys,
                                      const float u );

/*============================================================================*/
static float qLTISys_DiscreteUpdate( qLTISys_t * const sys,
                                     const float u )
{
    float v = u;
    size_t i;

    /*using direct-form 2*/
    for ( i = 0 ; i < sys->na ; ++i ) {
        v -= sys->a[ i ]*sys->xd[ i ];
    }

    return qLTISys_DiscreteFIRUpdate( sys->xd, sys->b, sys->n, v );
}
/*============================================================================*/
static float qLTISys_ContinuosUpdate( qLTISys_t * const sys,
                                      const float u )
{
    float y = 0.0f;
    float dx0 = 0.0f;

    if ( 1u == sys->n ) {
        dx0 = ( u - ( sys->xc[ 0 ].x[ 0 ]*sys->a[ 0 ] ) );
        (void)sys->integrate( &sys->xc[ 0 ], dx0 , sys->dt );
        y = ( sys->b[ 0 ] - ( sys->a[ 0 ]*sys->b0 ) )*sys->xc[ 0 ].x[ 0 ];
    }
    else {
        size_t i;
        /*compute states of the system by using the controllable canonical form*/
        for ( i = ( sys->n - 1u ) ; i >= 1u ; --i ) {
            dx0 += sys->a[ i ]*sys->xc[ i ].x[ 0 ]; /*compute the first derivative*/
            /*integrate to obtain the remaining states*/
            (void)sys->integrate( &sys->xc[ i ], sys->xc[ i - 1u ].x[ 0 ], sys->dt );
            /*compute the first part of the output*/
            y += ( sys->b[ i ] - ( sys->a[ i ]*sys->b0 ) )*sys->xc[ i ].x[ 0 ];
        }
        /*compute remaining part of the output that depends of the first state*/
        dx0 = u - ( dx0 + ( sys->a[ 0 ]*sys->xc[ 0 ].x[ 0 ] ) );
        (void)sys->integrate( &sys->xc[ 0 ], dx0, sys->dt ); /*integrate to get the first state*/
        /*compute the remaining part of the output*/
        y += ( sys->b[ 0 ] - ( sys->a[ 0 ]*sys->b0 ) )*sys->xc[ 0 ].x[ 0 ];
    }

    return y;
}
/*============================================================================*/
float qLTISys_Excite( qLTISys_t * const sys,
                      float u )
{
    float y = 0.0f;

    if ( 1 == qLTISys_IsInitialized( sys ) ) {
        if ( NULL != sys->tDelay.head ) { /*check if has delay*/
            qTDL_InsertSample( &sys->tDelay, u ); /*delay the output*/
            /*excite the system with the most delayed input*/
            u = qTDL_GetOldest( &sys->tDelay );
        }
        y = sys->sysUpdate( sys, u ); /*evaluate the system*/
        /*saturate*/
        if ( y < sys->min ) {
            y = sys->min;
        }
        else if ( y > sys->max ) {
            y = sys->max;
        }
        else {
            /*do nothing*/
        }
    }

    return y;
}
/*============================================================================*/
int qLTISys_SetDelay( qLTISys_t * const sys,
                      float * const w,
                      const size_t n,
                      const float initVal )
{
    int retValue = 0;

    if ( 1 == qLTISys_IsInitialized( sys ) ) {
        qTDL_Setup( &sys->tDelay, w, n, initVal );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qLTISys_SetSaturation( qLTISys_t * const sys,
                           const float min,
                           const float max )
{
    int retValue = 0;

    if ( ( 1 == qLTISys_IsInitialized( sys ) ) && ( max > min ) ) {
        sys->min = min;
        sys->max = max;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qLTISys_IsInitialized( const qLTISys_t * const sys )
{
    int retValue = 0;

    if ( NULL != sys ) {
        /*cppcheck-suppress misra-c2012-10.6 */
        retValue = ( ( NULL != sys->sysUpdate ) && ( ( NULL != sys->xc ) || ( NULL != sys->xd  ) ) ) ? 1 : 0;
    }

    return retValue;
}
/*============================================================================*/
int qLTISys_SetInitStates( qLTISys_t * const sys, const float * const xi )
{
    int retValue = 0;

    if ( 1 == qLTISys_IsInitialized( sys ) ) {
        size_t i;

        for ( i = 0u; i < sys->n ; ++i ) {
            const float zero = 0.0f;
            const float *iv = ( NULL != xi ) ? &xi[ i ] : &zero;

            if ( sys->dt <= 0.0f ) {
                sys->xd[ i ] = iv[ 0 ];
            }
            else {
                qNumA_StateInit( &sys->xc[ i ], iv[ 0 ], iv[ 0 ], iv[ 0 ] );
            }
        }

        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qLTISys_Setup( qLTISys_t * const sys,
                   float *num,
                   float *den,
                   void *x,
                   const size_t nb,
                   const size_t na,
                   const float dt )
{
    int retValue = 0;

    if ( ( NULL != sys ) && ( NULL != num ) && ( NULL != den ) && ( NULL != x ) && ( na > 0u ) ) {
        float a0;
        size_t i;

        if ( dt <= 0.0f ) { /*discrete system*/
            sys->b = num;
            sys->na = na;
            sys->nb = nb;
            sys->n = ( na > nb ) ? na : nb;
            sys->sysUpdate = &qLTISys_DiscreteUpdate;
            /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
            /*cppcheck-suppress misra-c2012-11.5 */
            sys->xd = (float*)x;
            /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
            sys->xc = NULL;
        }
        else { /*continuos system*/
            sys->b = &num[ 1 ];
            sys->n = na - 1u;
            sys->nb = sys->n;
            sys->sysUpdate = &qLTISys_ContinuosUpdate;
            /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
            /*cppcheck-suppress misra-c2012-11.5 */
            sys->xc = (qNumA_state_t*)x;
            /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
            sys->xd = NULL;
        }
        sys->dt = dt;
        sys->integrate = &qNumA_IntegralTr; /*default integration method*/
        sys->a = &den[ 1 ];
        /*normalize the transfer function coefficients*/
        a0 = den[ 0 ];
        for ( i = 0 ; i < sys->nb ; ++i ) {
            num[ i ] /= a0;
        }
        for ( i = 0 ; i < sys->na ; ++i ) {
            den[ i ] /= a0;
        }
        sys->b0 = num[ 0 ];
        sys->tDelay.head = NULL;
        (void)qLTISys_SetInitStates( sys, NULL );
        retValue = qLTISys_SetSaturation( sys, -FLT_MAX, FLT_MAX );
    }

    return retValue;
}
/*============================================================================*/
float qLTISys_DiscreteFIRUpdate( float *w,
                                 const float * const c,
                                 const size_t wsize,
                                 const float x )
{
    size_t i;
    float y = 0.0f;

    if ( NULL != c ) {
        for ( i = ( wsize - 1u ) ; i >= 1u ; --i ) {
            w[ i ] = w[ i - 1u ];
            y += w[ i ]*c[ i ];
        }
        y += c[ 0 ]*x;
    }
    else {
        for ( i = ( wsize - 1u ) ; i >= 1u ; --i ) {
            w[ i ] = w[ i - 1u ];
            y += w[ i ];
        }
        y += x;
    }
    w[ 0 ] = x;

    return y;
}
/*============================================================================*/
int qLTISys_SetIntegrationMethod( qLTISys_t * const sys,
                                  qNumA_IntegrationMethod_t im )
{
    int retValue = 0;

    if ( ( NULL != sys ) && ( NULL != im ) ) {
        if ( ( im == &qNumA_IntegralRe ) || ( im == &qNumA_IntegralTr ) || ( im == &qNumA_IntegralSi ) ) {
            sys->integrate = im;
            retValue = 1;
        }
    }

    return retValue;
}
/*============================================================================*/
