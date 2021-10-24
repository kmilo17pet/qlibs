/*!
 * @file qrms.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qrms.h"


static float qRMS_NewtonsFastSqrt( const float x );

/*============================================================================*/
int qRMS_Setup( qRMS_t * const q, float *window, const size_t wsize ) 
{
    int retValue = 0;
    if ( ( NULL != q ) && ( NULL != window ) && ( wsize > 0u ) ) {
        float defaults[ 2 ] = { 0.99f, 0.75f }; /*default parameters*/
        (void)qSSmoother_Setup( &q->f1, QSSMOOTHER_TYPE_EXPW, &defaults[0], NULL, 0uL );
        (void)qSSmoother_Setup( &q->f2, QSSMOOTHER_TYPE_MWM2, NULL , window, wsize );
        (void)qSSmoother_Setup( &q->f3, QSSMOOTHER_TYPE_LPF1, &defaults[1], NULL, 0uL );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
float qRMS_Update( qRMS_t * const q, const float x )
{
    float y = 0.0f;
    if ( NULL != q ) {
        y = qRMS_NewtonsFastSqrt( qSSmoother_Perform( &q->f1, x*x ) );
        y = qSSmoother_Perform( &q->f2, y ); /*2nd stage moving-window overlap*/
        y = qSSmoother_Perform( &q->f3, y ); /*3rd stage low-pass filter*/
    } 
    return y;
}
/*============================================================================*/
int qRMS_SetParams( qRMS_t * const q, const float lambda, const float alpha ) 
{
    int retValue = 0;
    if ( ( NULL != q ) && ( lambda > 0.0f ) && ( lambda <= 1.0f ) && ( alpha > 0.0f ) && ( alpha <= 1.0f ) ) {
        q->f1.lambda = lambda;
        q->f3.alpha = alpha;
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
static float qRMS_NewtonsFastSqrt( const float x )
{
    uint32_t i = 0uL;
    float xHalf = 0.5f*x , y = x;
    const uint32_t SQRT_2_RAISED_127 = 0x5F3759DFuL; /* sqrt(2^127) */

    (void)memcpy( &i, &y, sizeof(uint32_t) );   /* allowed type-punning*/
    i  = SQRT_2_RAISED_127 - ( i >> 1 );        /* Quake3 hack to get the best initial guess*/
    (void)memcpy( &y, &i, sizeof(uint32_t) );   /* allowed type-punning*/
    y  = y * ( 1.5f - ( xHalf * y * y ) );      /* 1st iteration*/
    return y*x;                                 /* multiply the result by x to get sqrt(x)*/
}
/*============================================================================*/