/*!
 * @file qssmother.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qssmoother.h"
#include "qltisys.h"
    
static float qSSmoother_Filter_LPF1( _qSSmoother_t *f, float x );
static float qSSmoother_Filter_LPF2( _qSSmoother_t *f, float x );
static float qSSmoother_Filter_MWM( _qSSmoother_t *f, float x );
static float qSSmoother_Filter_MWOR( _qSSmoother_t *f, float x );
static float qSSmoother_Filter_GAUSSIAN( _qSSmoother_t *f, float x ) ;
static void qSSmoother_WindowSet( float *w, size_t wsize, float x );

/*============================================================================*/
static float qSSmoother_Abs( float x ){
    return ( x < 0.0f )? -x : x; 
}
/*============================================================================*/
static void qSSmoother_WindowSet( float *w, size_t wsize, float x )
{
    size_t i;
    for ( i = 0 ; i < wsize ; ++i ) {
        w[ i ] = x;
    }    
}
/*============================================================================*/
int qSSmoother_IsInitialized( qSSmootherPtr_t s )
{
    int retVal = 0;
    if ( NULL != s ) {
        _qSSmoother_t *f = (_qSSmoother_t*)s;
        retVal = (int)( NULL != f->filtFcn );
    }    
    return retVal;
} 
/*============================================================================*/
int qSSmoother_Reset( qSSmootherPtr_t s ) 
{
    int retVal = 0;
    if ( NULL != s ) {
        _qSSmoother_t *f = (_qSSmoother_t*)s;
        f->init = 1u;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
float qSSmoother_Perform( qSSmootherPtr_t s, float x )
{
    float out = x;
    if ( NULL != s ) {
        _qSSmoother_t *f = (_qSSmoother_t*)s;
        if ( NULL != f->filtFcn ) {
            out = f->filtFcn( f, x );
        }
    }
    return out;
}
/*============================================================================*/
int qSSmoother_Setup_LPF1( qSSmoother_LPF1_t *s, float alpha )
{
    int retVal = 0;
    if ( ( NULL != s ) && ( alpha > 0.0f ) && ( alpha < 1.0f ) ) {
        s->alpha = alpha;
        s->f.filtFcn = &qSSmoother_Filter_LPF1;
        retVal = qSSmoother_Reset( s );
    }
    return retVal;
}
/*============================================================================*/
int qSSmoother_Setup_LPF2( qSSmoother_LPF2_t *s, float alpha )
{
    int retVal = 0;
    if ( ( NULL != s ) && ( alpha > 0.0f ) && ( alpha < 1.0f ) ) {
        float aa, p1, r;
        s->alpha = alpha;
        aa = s->alpha*s->alpha;
        /*cstat -MISRAC2012-Dir-4.11_b*/
        p1 = sqrtf( 2.0f*s->alpha ); /*arg always positive*/
        /*cstat +MISRAC2012-Dir-4.11_b*/
        r = 1.0f + p1 + aa;
        s->k = aa/r;
        s->a1 = 2.0f*( aa - 1.0f )/r;
        s->a2 = ( 1.0f - p1 + aa )/r;
        s->b1 = 2.0f*s->k;        
        s->f.filtFcn = &qSSmoother_Filter_LPF2;
        retVal = qSSmoother_Reset( s );
    }
    return retVal;
}
/*============================================================================*/
int qSSmoother_Setup_MWM( qSSmoother_MWM_t *s, float *window, size_t wsize )
{
    int retVal = 0;
    if ( ( NULL != s ) && ( NULL != window ) && ( wsize > 0u ) ) {
        s->w = window;
        s->wsize = wsize;
        s->f.filtFcn = &qSSmoother_Filter_MWM;
        retVal = qSSmoother_Reset( s );
    }
    return retVal;
}
/*============================================================================*/
int qSSmoother_Setup_MWOR( qSSmoother_MWOR_t *s, float *window, size_t wsize, float alpha )
{
    int retVal = 0;
    if ( ( NULL != s ) && ( NULL != window ) && ( wsize > 0u ) && ( alpha > 0.0f ) && ( alpha < 1.0f ) ) {
        s->w = window;
        s->wsize = wsize;
        s->alpha  = alpha;
        s->f.filtFcn = &qSSmoother_Filter_MWOR;
        retVal = qSSmoother_Reset( s ); ;
    }
    return retVal;
}
/*============================================================================*/
int qSSmoother_Setup_GAUSSIAN( qSSmoother_GAUSSIAN_t *s, float *window, float *kernel, size_t wsize, float sigma, size_t c )
{
    int retVal = 0;

    if ( ( NULL != s ) && ( NULL != window ) && ( NULL != kernel ) && ( wsize > 0u ) && ( c < wsize ) && ( sigma > 0.0f ) ) {
        float r, sum = 0.0f;
        size_t i;
        float L, center, d; 
        /*cstat -CERT-FLP36-C -MISRAC2012-Rule-10.8*/
        L = (float)(wsize - 1u)/2.0f;
        center = (float)c - L;
        r =  2.0f*sigma*sigma ;
        for ( i = 0u ; i < wsize ; ++i ) {
            d = (float)i - L;  /*symetry*/     
            d -= center;     
            kernel[ i ] =  expf( -(d*d)/r );            
            sum += kernel[ i ];
        }        
        /*cstat +CERT-FLP36-C +MISRAC2012-Rule-10.8*/
        for ( i = 0u ; i < wsize ; ++i ) {
            kernel[ i ] /= sum;
        }
        
        s->w = window;
        s->k = kernel;
        s->wsize = wsize;
        s->f.filtFcn = &qSSmoother_Filter_GAUSSIAN;
        retVal = qSSmoother_Reset( s );
    }
    return retVal;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF1( _qSSmoother_t *f, float x ) 
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_LPF1_t *s = (qSSmoother_LPF1_t*)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        s->y1 = x;
        f->init = 0u;
    }
    y = x + ( s->alpha*( s->y1 - x ) ) ;
    s->y1 = y;    
    
    return y;
}
/*============================================================================*/
static float qSSmoother_Filter_LPF2( _qSSmoother_t *f, float x ) 
{
    float y;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_LPF2_t *s = (qSSmoother_LPF2_t*)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        s->y1 = x;
        s->y2 = x;
        s->x1 = x;
        s->x2 = x;
        f->init = 0u;
    }     
    y = ( s->k*x  ) + ( s->b1*s->x1 ) + ( s->k*s->x2 ) - ( s->a1*s->y1 ) - ( s->a2*s->y2 );
    s->x2 = s->x1;
    s->x1 = x;
    s->y2 = s->y1;
    s->y1 = y;    
    
    return y;
}
/*============================================================================*/
static float qSSmoother_Filter_MWM( _qSSmoother_t *f, float x ) 
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_MWM_t *s = (qSSmoother_MWM_t*)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0u;
    }        
    /*cstat -CERT-FLP36-C*/
    return qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x ) / (float)s->wsize; 
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static float qSSmoother_Filter_MWOR( _qSSmoother_t *f, float x ) 
{
    float m;
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_MWOR_t *s = (qSSmoother_MWOR_t*)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        s->m = x; 
        f->init = 0u;
    }        

    m = qLTISys_DiscreteFIRUpdate( s->w, NULL, s->wsize, x ) - x; /*shift, sum and compensate*/
    if ( qSSmoother_Abs( s->m - x )  > ( s->alpha*qSSmoother_Abs( s->m ) ) ) { /*is it an outlier?*/
        s->w[ 0 ] = s->m; /*replace the outlier with the dynamic median*/
    }
    /*cstat -CERT-FLP36-C*/
    s->m = ( m + s->w[ 0 ] ) / (float)s->wsize;  /*compute new mean for next iteration*/   
    /*cstat +CERT-FLP36-C*/
    return s->w[ 0 ];
}
/*============================================================================*/
static float qSSmoother_Filter_GAUSSIAN( _qSSmoother_t *f, float x ) 
{
    /*cstat -CERT-EXP36-C_a -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
    qSSmoother_GAUSSIAN_t *s = (qSSmoother_GAUSSIAN_t*)f;
    /*cstat +CERT-EXP36-C_a +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    
    if ( 1u == f->init ) {
        qSSmoother_WindowSet( s->w, s->wsize, x );
        f->init = 0u;
    }   
    return qLTISys_DiscreteFIRUpdate( s->w, s->k, s->wsize, x );
}
/*============================================================================*/