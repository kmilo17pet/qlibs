/*!
 * @file qssmoother.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
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

static void qSSmoother_WindowSet( float *w,
                                  const size_t wsize,
                                  const float x );
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
static int qSSmoother_Setup_DESF( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize );
static int qSSmoother_Setup_ALNF( _qSSmoother_t * const f,
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
static float qSSmoother_Filter_DESF( _qSSmoother_t * const f,
                                     const float x );
static float qSSmoother_Filter_ALNF( _qSSmoother_t * const f,
                                     const float x );
/*============================================================================*/
int qSSmoother_Setup( qSSmootherPtr_t * const s,
                      const qSSmoother_Type_t type,
                      const float * const param,
                      float *window,
                      const size_t wsize )
{
    static struct qSmoother_Vtbl_s qSmoother_Vtbl[ 11 ] = {
        { &qSSmoother_Filter_LPF1, &qSSmoother_Setup_LPF1 },
        { &qSSmoother_Filter_LPF2, &qSSmoother_Setup_LPF2 },
        { &qSSmoother_Filter_MWM1, &qSSmoother_Setup_MWM1 },
        { &qSSmoother_Filter_MWM2, &qSSmoother_Setup_MWM2 },
        { &qSSmoother_Filter_MOR1, &qSSmoother_Setup_MOR1 },
        { &qSSmoother_Filter_MOR2, &qSSmoother_Setup_MOR2 },
        { &qSSmoother_Filter_GMWF, &qSSmoother_Setup_GMWF },
        { &qSSmoother_Filter_KLMN, &qSSmoother_Setup_KLMN },
        { &qSSmoother_Filter_EXPW, &qSSmoother_Setup_EXPW },
        { &qSSmoother_Filter_DESF, &qSSmoother_Setup_DESF },
        { &qSSmoother_Filter_ALNF, &qSSmoother_Setup_ALNF },
    };
    int retValue = 0;
    const size_t maxTypes = sizeof(qSmoother_Vtbl)/sizeof(qSmoother_Vtbl[ 0 ] );

    if ( ( s != NULL ) && ( (size_t)type < maxTypes ) ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
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

    if ( ( alpha > 0.0F ) && ( alpha < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
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

    if ( ( alpha > 0.0F ) && ( alpha < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_LPF2_t * const s = (qSSmoother_LPF2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        float aa, p1, r;
        aa = alpha*alpha;
        /*cstat -MISRAC2012-Dir-4.11_b*/
        p1 = QLIB_SQRT( 2.0F*alpha ); /*arg always positive*/
        /*cstat +MISRAC2012-Dir-4.11_b*/
        r = 1.0F + p1 + aa;
        s->k = aa/r;
        s->a1 = 2.0F*( aa - 1.0F )/r;
        s->a2 = ( 1.0F - p1 + aa )/r;
        s->b1 = 2.0F*s->k;
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

    if ( ( NULL != window ) && ( wsize > 0U ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
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

    if ( ( NULL != window ) && ( wsize > 0U ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_MWM2_t * const s = (qSSmoother_MWM2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        qTDL_Setup( &s->tdl, window, wsize, 0.0F );
        s->sum  = 0.0F;
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

    if ( ( NULL != window ) && ( wsize > 0U ) && ( a > 0.0F ) && ( a < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
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

    if ( ( NULL != window ) && ( wsize > 0U ) && ( q > 0.0F ) && ( q < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_MOR2_t * const s = (qSSmoother_MOR2_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->alpha  = q;
        qTDL_Setup( &s->tdl, window, wsize, 0.0F );
        s->sum = 0.0F;
        s->m = 0.0F;
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

    if ( ( NULL != window ) && ( wsize > 0U ) && ( c < ws ) && ( sg > 0.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_GMWF_t * const s = (qSSmoother_GMWF_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        float *kernel = &window[ ws ];
        float r, sum = 0.0F;
        size_t i;
        float l, center;
        /*cstat -CERT-FLP36-C -MISRAC2012-Rule-10.8*/
        /*cppcheck-suppress misra-c2012-10.8 */
        l = (float)( wsize - 1U )/2.0F;
        center = (float)c - l;
        r = 2.0F*sg*sg;
        for ( i = 0U ; i < ws ; ++i ) {
            float d = (float)i - l; /*symmetry*/
            d -= center;
            kernel[ i ] =  QLIB_EXP( -( d*d )/r );
            sum += kernel[ i ];
        }
        /*cstat +CERT-FLP36-C +MISRAC2012-Rule-10.8*/
        for ( i = 0U ; i < ws ; ++i ) {
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

    if ( ( p > 0.0F ) && ( q > 0.0F ) && ( r > 0.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_KLMN_t * const s = (qSSmoother_KLMN_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->p = p;
        s->q = q;
        s->r = r;
        s->A = 1.0F;
        s->H = 1.0F;
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

    if ( ( lambda > 0.0F ) && ( lambda < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_EXPW_t * const s = (qSSmoother_EXPW_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->lambda = lambda;
        s->m = 0.0F;
        s->w = 1.0F;
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_DESF( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float a = param[ 0 ];
    float b = param[ 1 ];
    float n = param[ 2 ];

    if ( ( n >= 0.0F ) && ( a > 0.0F ) && ( a < 1.0F ) && ( b > 0.0F ) && ( b < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_DESF_t * const s = (qSSmoother_DESF_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->alpha = a;
        s->beta = b;
        s->n = QLIB_ROUND( n );
        retValue = qSSmoother_Reset( s );
        (void)window;
        (void)wsize;
    }

    return retValue;
}
/*============================================================================*/
static int qSSmoother_Setup_ALNF( _qSSmoother_t * const f,
                                  const float * const param,
                                  float *window,
                                  const size_t wsize )
{
    int retValue = 0;
    float a = param[ 0 ];
    float m = param[ 1 ];

    if ( ( NULL != window ) && ( wsize > 0U ) && ( a > 0.0F ) && ( a < 1.0F ) && ( m > 0.0F ) && ( m < 1.0F ) ) {
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-EXP36-C_a*/
        /*cppcheck-suppress misra-c2012-11.3 */
        qSSmoother_ALNF_t * const s = (qSSmoother_ALNF_t* const)f;
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-EXP36-C_a*/
        s->alpha = a;
        s->mu = m;
        s->x = window;
        s->w = &window[ wsize ];
        s->w_1 = ( s->mu > 0.0F ) ? &window[ 2U*wsize ] : NULL;
        s->n = wsize;
        retValue = qSSmoother_Reset( s );
    }

    return retValue;
}
/*============================================================================*/
static void qSSmoother_WindowSet( float *w,
                                  const size_t wsize,
                                  const float x )
{
    size_t i;

    for ( i = 0U ; i < wsize ; ++i ) {
        w[ i ] = x;
    }
}
/*============================================================================*/
int qSSmoother_IsInitialized( const qSSmootherPtr_t * const s )
{
    int retValue = 0;

    if ( NULL != s ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        const _qSSmoother_t * const f = (const _qSSmoother_t* const)s;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-10.6 */
        retValue = ( NULL != f->vt ) ? 1 : 0;
    }

    return retValue;
}
/*============================================================================*/
int qSSmoother_Reset( qSSmootherPtr_t * const s )
{
    int retValue = 0;

    if ( NULL != s ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        _qSSmoother_t * const f = (_qSSmoother_t* const)s;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        f->init = 1U;
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
        /*cppcheck-suppress misra-c2012-11.5 */
        _qSSmoother_t * const f = (_qSSmoother_t* const)s;
        /*cppcheck-suppress misra-c2012-11.5 */
        struct qSmoother_Vtbl_s *vt = f->vt;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        if ( NULL != vt->perform ) {
            retValue = vt->perform( f, x );
        }
    }

    return retValue;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF1( _qSSmoother_t * const f,
                                     const float x )
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_LPF1_t * const s = (qSSmoother_LPF1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1U == f->init ) {
        s->y1 = x;
        f->init = 0U;
    }
    y = x + ( s->alpha*( s->y1 - x ) );
    s->y1 = y;

    return y;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF2( _qSSmoother_t * const f,
                                     const float x )
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_LPF2_t * const s = (qSSmoother_LPF2_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1U == f->init ) {
        s->y1 = x;
        s->y2 = x;
        s->x1 = x;
        s->x2 = x;
        f->init = 0U;
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
static float qSSmoother_Filter_MWM1( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_MWM1_t * const s = (qSSmoother_MWM1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1U == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0U;
    }
    /*cstat -CERT-FLP36-C*/
    return qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x )/(float)s->wsize;
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static float qSSmoother_Filter_MWM2( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-FLP36-C*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_MWM2_t * const s = (qSSmoother_MWM2_t* const)f;
    float wsize = (float)s->tdl.itemCount;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-FLP36-C*/
    if ( 1U == f->init ) {
        qTDL_Flush( &s->tdl, x );
        s->sum = x*wsize;
        f->init = 0U;
    }
    s->sum += x - qTDL_GetOldest( &s->tdl );
    qTDL_InsertSample( &s->tdl, x );

    return s->sum/wsize;
}
/*============================================================================*/
static float qSSmoother_Filter_MOR1( _qSSmoother_t * const f,
                                     const float x )
{
    float m;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_MOR1_t * const s = (qSSmoother_MOR1_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1U == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        s->m = x;
        f->init = 0U;
    }
    /*shift, sum and compensate*/
    m = qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x ) - x;
    /*is it an outlier?*/
    if ( QLIB_ABS( s->m - x ) > ( s->alpha*QLIB_ABS( s->m ) ) ) {
        s->w[ 0 ] = s->m; /*replace the outlier with the dynamic median*/
    }
    /*cstat -CERT-FLP36-C*/
    /*compute new mean for next iteration*/
    s->m = ( m + s->w[ 0 ] ) / (float)s->wsize;
    /*cstat +CERT-FLP36-C*/
    return s->w[ 0 ];
}
/*============================================================================*/
static float qSSmoother_Filter_MOR2( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d -CERT-FLP36-C*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_MOR2_t * const s = (qSSmoother_MOR2_t* const)f;
    float wsize = (float)s->tdl.itemCount;
    float xx = x;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d +CERT-FLP36-C*/
    if ( 1U == f->init ) {
        qTDL_Flush( &s->tdl, x );
        s->sum = wsize*x;
        s->m = x;
        f->init = 0U;
    }
    /*is it an outlier?*/
    if ( QLIB_ABS( s->m - x ) > ( s->alpha*QLIB_ABS( s->m ) ) ) {
        xx = s->m; /*replace the outlier with the dynamic median*/
    }
    s->sum += xx - qTDL_GetOldest( &s->tdl );
    s->m = s->sum/wsize;
    qTDL_InsertSample( &s->tdl, xx );

    return x;
}
/*============================================================================*/
static float qSSmoother_Filter_GMWF( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_GMWF_t * const s = (qSSmoother_GMWF_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/

    if ( 1U == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0U;
    }

    return qLTISys_DiscreteFIRUpdate( s->w, s->k, s->wsize, x );
}
/*============================================================================*/
static float qSSmoother_Filter_KLMN( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_KLMN_t * const s = (qSSmoother_KLMN_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float pH;
    if ( 1U == f->init ) {
        s->x = x;
        f->init = 0U;
    }
    /* Predict */
    s->x = s->A*s->x;
    s->p = ( s->A*s->A*s->p ) + s->q; /* p(n|n-1)=A^2*p(n-1|n-1)+q */
    /* Measurement */
    pH = s->p*s->H;
    s->gain =  pH/( s->r + ( s->H*pH ) );
    s->x += s->gain*( x - ( s->H*s->x ) );
    s->p = ( 1.0F - ( s->gain*s->H ) )*s->p; /*covariance update*/

    return s->x;
}
/*============================================================================*/
static float qSSmoother_Filter_EXPW( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_EXPW_t * const s = (qSSmoother_EXPW_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float iw;

    if ( 1U == f->init ) {
        s->m = x;
        s->w = 1.0F;
        f->init = 0U;
    }
    s->w = ( s->lambda*s->w ) + 1.0F;
    iw = 1.0F/s->w;
    s->m = ( s->m*( 1.0F - iw ) ) + ( iw*x );

    return s->m;
}
/*============================================================================*/
static float qSSmoother_Filter_DESF( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_DESF_t * const s = (qSSmoother_DESF_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float lt_1;

    if ( 1U == f->init ) {
        s->lt = x;
        s->bt = x;
        f->init = 0U;
    }
    lt_1 = s->lt;
    s->lt = ( ( 1.0F - s->alpha)*lt_1 ) + ( s->alpha*x ); /*level*/
    s->bt = ( ( 1.0F - s->beta )*s->bt ) + ( s->beta*( s->lt - lt_1 ) ); /*trend*/
    return s->lt + ( s->n*s->bt ); /*model/forecast*/
}
/*============================================================================*/
static float qSSmoother_Filter_ALNF( _qSSmoother_t * const f,
                                     const float x )
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    /*cppcheck-suppress misra-c2012-11.3 */
    qSSmoother_ALNF_t * const s = (qSSmoother_ALNF_t* const)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    float xe;
    size_t i;

    if ( 1U == f->init ) {
        /*cstat -CERT-FLP36-C*/
        float np = 1.0F/(float)s->n;
        /*cstat +CERT-FLP36-C*/
        qSSmoother_WindowSet( s->x, s->n, x );
        qSSmoother_WindowSet( s->w, s->n, np );
        if ( NULL != s->w_1 ) {
            qSSmoother_WindowSet( s->w_1, s->n, np );
        }
        f->init = 0U;
    }
    xe = qLTISys_DiscreteFIRUpdate( s->x, s->w, s->n, x );
    if ( NULL != s->w_1 ) {
        float *w_1 = &s->w[ s->n ];

        for ( i = 0U ; i < s->n ; ++i ) {
            float w = s->w[ i ],  w1 = w_1[ i ];

            s->w[ i ] += ( s->alpha*( x - xe )*s->x[ i ] ) + ( s->mu*( w - w1 ) );
            w_1[ i ] = w;
        }
    }
    else {
        for ( i = 0U ; i < s->n ; ++i ) {
            s->w[ i ] += s->alpha*( x - xe )*s->x[ i ];
        }
    }

    return xe;
}
/*============================================================================*/
