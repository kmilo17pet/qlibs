/*!
 * @file qssmoother.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qssmoother.h"
#include "qltisys.h"
#include "qffmath.h"

/*! @cond  */
struct qSmoother_Vtbl_s
{
    float (*perform)( _qSSmoother_t * const f, const float x );
    int (*setup)( _qSSmoother_t * const f, const float * const param, float *window, const size_t wsize );
};
/*! @endcond  */

static float qSSmoother_Abs( float x );
static void qSSmoother_WindowSet( float *w,
                                  size_t wsize, float x );
static int qSSmoother_Setup_LPF1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_LPF2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_MWM1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_MWM2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_MOR1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_MOR2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_GMWF( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_KLMN( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_EXPW( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static float qSSmoother_Filter_LPF1( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_LPF2( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_MWM1( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_MWM2( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_MOR1( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_MOR2( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_GMWF( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_KLMN( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_EXPW( _qSSmoother_t * const f,
                                     const float x );

/*============================================================================*/
int qSSmoother_Setup( qSSmootherPtr_t * const s,
                      const qSSmoother_Type_t type,
                      const float * const param,
                      float *window,
                      const size_t wsize )
{
    static struct qSmoother_Vtbl_s qSmoother_Vtbl[ 9 ] = {
        { &qSSmoother_Filter_LPF1, &qSSmoother_Setup_LPF1 },
        { &qSSmoother_Filter_LPF2, &qSSmoother_Setup_LPF2 },
        { &qSSmoother_Filter_MWM1, &qSSmoother_Setup_MWM1 },
        { &qSSmoother_Filter_MWM2, &qSSmoother_Setup_MWM2 },
        { &qSSmoother_Filter_MOR1, &qSSmoother_Setup_MOR1 },
        { &qSSmoother_Filter_MOR2, &qSSmoother_Setup_MOR2 },
        { &qSSmoother_Filter_GMWF, &qSSmoother_Setup_GMWF },
        { &qSSmoother_Filter_KLMN, &qSSmoother_Setup_KLMN },
        { &qSSmoother_Filter_EXPW, &qSSmoother_Setup_EXPW },
    };
    int retValue = 0;
    const size_t maxTypes = sizeof(qSmoother_Vtbl)/sizeof(qSmoother_Vtbl[ 0 ] );

    if ( ( s != NULL ) && ( (size_t)type < maxTypes ) ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        _qSSmoother_t * const self = (_qSSmoother_t* const)s;
        self->vt = &qSmoother_Vtbl[ type ];
        retValue = qSmoother_Vtbl[ type ].setup( s, param, window, wsize );
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_LPF1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float alpha = param[ 0 ];

    if ( ( alpha > 0.0f ) && ( alpha < 1.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_LPF1_t * const s = (qSSmoother_LPF1_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->alpha = alpha;
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_LPF2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float alpha = param[ 0 ];

    if ( ( alpha > 0.0f ) && ( alpha < 1.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_LPF2_t * const s = (qSSmoother_LPF2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        float aa, p1, r;
        aa = alpha*alpha;
        /*cstat -MISRAC2012-Dir-4.11_b*/
        p1 = QLIB_SQRT( 2.0f*alpha ); /*arg always positive*/
        /*cstat +MISRAC2012-Dir-4.11_b*/
        r = 1.0f + p1 + aa;
        s->k = aa/r;
        s->a1 = 2.0f*( aa - 1.0f )/r;
        s->a2 = ( 1.0f - p1 + aa )/r;
        s->b1 = 2.0f*s->k;
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_MWM1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;

    if ( ( NULL != window ) && ( wsize > 0u ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_MWM1_t * const s = (qSSmoother_MWM1_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->w = window;
        s->wsize = wsize;
        retValue = qSSmoother_Reset( s );
        (void)param;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_MWM2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;

    if ( ( NULL != window ) && ( wsize > 0u ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_MWM2_t * const s = (qSSmoother_MWM2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        qTDL_Setup( &s->tdl, window, wsize, 0.0f );
        s->sum  = 0.0f;
        retValue = qSSmoother_Reset( s );
        (void)param;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_MOR1( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float a = param[ 0 ];

    if ( ( NULL != window ) && ( wsize > 0u ) && ( a > 0.0f ) && ( a < 1.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_MOR1_t * const s = (qSSmoother_MOR1_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->w = window;
        s->wsize = wsize;
        s->alpha  = a;
        retValue = qSSmoother_Reset( s );
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_MOR2( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float q = param[ 0 ];

    if ( ( NULL != window ) && ( wsize > 0u ) && ( q > 0.0f ) && ( q < 1.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_MOR2_t * const s = (qSSmoother_MOR2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->alpha  = q;
        qTDL_Setup( &s->tdl, window, wsize, 0.0f );
        s->sum = 0.0f;
        s->m = 0.0f;
        retValue = qSSmoother_Reset( s );
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_GMWF( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float sg = param[ 0 ]; /*gaussian sigma :  standard deviation*/
    /*cstat -CERT-FLP34-C*/
    const size_t c = (size_t)param[ 1 ];
    /*cstat +CERT-FLP34-C*/
    const size_t ws = wsize/2u;

    if ( ( NULL != window ) && ( wsize > 0u ) && ( c < ws ) && ( sg > 0.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_GMWF_t * const s = (qSSmoother_GMWF_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        float *kernel = &window[ ws ];
        float r, sum = 0.0f;
        size_t i;
        float l, center;
        /*cstat -CERT-FLP36-C -MISRAC2012-Rule-10.8*/
        l = (float)( wsize - 1u )/2.0f;
        center = (float)c - l;
        r = 2.0f*sg*sg;
        for ( i = 0u ; i < ws ; ++i ) {
            float d = (float)i - l; /*symmetry*/
            d -= center;
            kernel[ i ] =  QLIB_EXP( -( d*d )/r );
            sum += kernel[ i ];
        }
        /*cstat +CERT-FLP36-C +MISRAC2012-Rule-10.8*/
        for ( i = 0u ; i < ws ; ++i ) {
            kernel[ i ] /= sum;
        }
        s->w = window;
        s->k = kernel;
        s->wsize = ws;
        retValue = qSSmoother_Reset( s );
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_KLMN( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float p = param[ 0 ];
    float q = param[ 1 ];
    float r = param[ 2 ];

    if ( ( p > 0.0f ) && ( q > 0.0f ) && ( r > 0.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_KLMN_t * const s = (qSSmoother_KLMN_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->p = p;
        s->q = q;
        s->r = r;
        s->A = 1.0f;
        s->H = 1.0f;
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_EXPW( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float lambda = param[ 0 ];

    if ( ( lambda > 0.0f ) && ( lambda < 1.0f ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        qSSmoother_EXPW_t * const s = (qSSmoother_EXPW_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->lambda = lambda;
        s->m = 0.0f;
        s->w = 1.0f;
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static float qSSmoother_Abs( const float x )
{
    return ( x < 0.0f ) ? -x : x;
}
/*============================================================================*/
static void qSSmoother_WindowSet( float *w,
                                  const size_t wsize,
                                  const float x )
{
    size_t i;

    for ( i = 0 ; i < wsize ; ++i ) {
        w[ i ] = x;
    }
}
/*============================================================================*/
int qSSmoother_IsInitialized( const qSSmootherPtr_t * const s )
{
    int retValue = 0;

    if ( NULL != s ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        const _qSSmoother_t * const f = (const _qSSmoother_t* const)s;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        retValue = (int)( NULL != f->vt );
    }

    return retValue;
}
/*============================================================================*/
int qSSmoother_Reset( qSSmootherPtr_t * const s )
{
    int retValue = 0;

    if ( NULL != s ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        _qSSmoother_t * const f = (_qSSmoother_t* const)s;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        f->init = 1u;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
float qSSmoother_Perform( qSSmootherPtr_t * const s,
                          const float x )
{
    float retValue = x;

    if ( NULL != s ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        _qSSmoother_t * const f = (_qSSmoother_t* const)s;
        struct qSmoother_Vtbl_s *vt = f->vt;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        if ( NULL != vt->perform ) {
            retValue = vt->perform( f, x );
        }
    }

    return retValue;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF1( _qSSmoother_t *f,
                                     const float x )
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_LPF1_t * const s = (qSSmoother_LPF1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        s->y1 = x;
        f->init = 0u;
    }
    y = x + ( s->alpha*( s->y1 - x ) );
    s->y1 = y;

    return y;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF2( _qSSmoother_t *f,
                                     const float x )
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_LPF2_t * const s = (qSSmoother_LPF2_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        s->y1 = x;
        s->y2 = x;
        s->x1 = x;
        s->x2 = x;
        f->init = 0u;
    }
    y = ( s->k*x ) + ( s->b1*s->x1 ) + ( s->k*s->x2 )
        - ( s->a1*s->y1 ) - ( s->a2*s->y2 );
    s->x2 = s->x1;
    s->x1 = x;
    s->y2 = s->y1;
    s->y1 = y;

    return y;
}
/*============================================================================*/
static float qSSmoother_Filter_MWM1( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_MWM1_t * const s = (qSSmoother_MWM1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0u;
    }
    /*cstat -CERT-FLP36-C*/
    return qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x )/(float)s->wsize;
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static float qSSmoother_Filter_MWM2( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-FLP36-C*/
    qSSmoother_MWM2_t * const s = (qSSmoother_MWM2_t* const)f;
    float wsize = (float)s->tdl.itemCount;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-FLP36-C*/
    if ( 1u == f->init ) {
        qTDL_Flush( &s->tdl, x );
        s->sum = x*wsize;
        f->init = 0u;
    }
    s->sum += x - qTDL_GetOldest( &s->tdl );
    qTDL_InsertSample( &s->tdl, x );

    return s->sum/wsize;
}
/*============================================================================*/
static float qSSmoother_Filter_MOR1( _qSSmoother_t *f,
                                     const float x )
{
    float m;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_MOR1_t * const s = (qSSmoother_MOR1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        s->m = x;
        f->init = 0u;
    }
    /*shift, sum and compensate*/
    m = qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x ) - x;
    /*is it an outlier?*/
    if ( qSSmoother_Abs( s->m - x ) > ( s->alpha*qSSmoother_Abs( s->m ) ) ) {
        s->w[ 0 ] = s->m; /*replace the outlier with the dynamic median*/
    }
    /*cstat -CERT-FLP36-C*/
    /*compute new mean for next iteration*/
    s->m = ( m + s->w[ 0 ] ) / (float)s->wsize;
    /*cstat +CERT-FLP36-C*/
    return s->w[ 0 ];
}
/*============================================================================*/
static float qSSmoother_Filter_MOR2( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-FLP36-C*/
    qSSmoother_MOR2_t * const s = (qSSmoother_MOR2_t* const)f;
    float wsize = (float)s->tdl.itemCount;
    float xx = x;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-FLP36-C*/
    if ( 1u == f->init ) {
        qTDL_Flush( &s->tdl, x );
        s->sum = wsize*x;
        s->m = x;
        f->init = 0u;
    }
    /*is it an outlier?*/
    if ( qSSmoother_Abs( s->m - x ) > ( s->alpha*qSSmoother_Abs( s->m ) ) ) {
        xx = s->m; /*replace the outlier with the dynamic median*/
    }
    s->sum += xx - qTDL_GetOldest( &s->tdl );
    s->m = s->sum/wsize;
    qTDL_InsertSample( &s->tdl, xx );

    return x;
}
/*============================================================================*/
static float qSSmoother_Filter_GMWF( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_GMWF_t * const s = (qSSmoother_GMWF_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/

    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0u;
    }

    return qLTISys_DiscreteFIRUpdate( s->w, s->k, s->wsize, x );
}
/*============================================================================*/
static float qSSmoother_Filter_KLMN( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_KLMN_t * const s = (qSSmoother_KLMN_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float pH;
    if ( 1u == f->init ) {
        s->x = x;
        f->init = 0u;
    }
    /* Predict */
    s->x = s->A*s->x;
    s->p = ( s->A*s->A*s->p ) + s->q; /* p(n|n-1)=A^2*p(n-1|n-1)+q */
    /* Measurement */
    pH = s->p*s->H;
    s->gain =  pH/( s->r + ( s->H*pH ) );
    s->x += s->gain*( x - ( s->H*s->x ) );
    s->p = ( 1.0f - ( s->gain*s->H ) )*s->p; /*covariance update*/

    return s->x;
}
/*============================================================================*/
static float qSSmoother_Filter_EXPW( _qSSmoother_t *f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_EXPW_t * const s = (qSSmoother_EXPW_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float iw;

    if ( 1u == f->init ) {
        s->m = x;
        s->w = 1.0f;
        f->init = 0u;
    }
    s->w = ( s->lambda*s->w ) + 1.0f;
    iw = 1.0f/s->w;
    s->m = ( s->m*( 1.0f - iw ) ) + ( iw*x );

    return s->m;
}
/*============================================================================*/
