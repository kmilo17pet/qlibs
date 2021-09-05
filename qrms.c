/*!
 * @file qrms.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qrms.h"

#define QRMS_SQRT_2_RAISED_127      ( 0x5F3759DFuL )    /* sqrt(2^127) */
static float qRMS_NewtonsFastSqrt( float x );

/*============================================================================*/
int qRMS_Init( qRMS_t *q ) 
{
    int RetValue = 0;
    if ( NULL != q ) {
        (void)qRMS_Reset( q );
        RetValue = qRMS_SetParams( q, QRMS_DEFAULT_ALPHA, QRMS_DEFAULT_A1, QRMS_DEFAULT_A2 );
    }
    return RetValue;
}
/*============================================================================*/
float qRMS_Update( qRMS_t *q, float x )
{
    float y = 0.0f;
    if ( NULL != q ){
        x *= x;     /* compute x^2 */
        q->m = x + ( q->alpha*(q->m - x) ) ; /*compute the recursive mean*/
        y = ( q->b1*qRMS_NewtonsFastSqrt( q->m ) ) - ( q->a1*q->f1 ) - ( q->a2*q->f2 ); /*compute the recursive filter to improve the output*/
        q->f2 = q->f1;  /*update 2nd-stage filter*/
        q->f1 = y;
    }
    return y;
}
/*============================================================================*/
int qRMS_SetParams( qRMS_t *q, float alpha, float a1, float a2 ) 
{
    int RetValue = 0;
    if ( ( NULL != q ) && ( alpha > 0.0f ) && ( alpha < 1.0f) ) {
        q->alpha = alpha;
        q->a1 = a1;
        q->a2 = a2;
        q->b1 = 1.0f + q->a1 + q->a2;   /*gain of the output filter should be unary*/
        RetValue = 1;
    }
    return RetValue;
}
/*============================================================================*/
int qRMS_Reset( qRMS_t *q ) 
{
    int RetValue = 0;
    if ( NULL != q ) {
        q->m = 0.0f;    /* initialize the recursive mean*/
        q->f1 = 0.0f;   /* f(t-1) for the output filter */
        q->f2 = 0.0f;   /* f(t-2) for the output filter */  
        RetValue = 1; 
    }
    return RetValue;
}
/*============================================================================*/
static float qRMS_NewtonsFastSqrt( float x )
{
    uint32_t i = 0uL;
    float xHalf = 0.5f*x , y = x;

    (void)memcpy( &i, &y, sizeof(uint32_t) );   /* allowed type-punning*/
    i  = QRMS_SQRT_2_RAISED_127 - ( i >> 1 );   /* Quake3 hack to get the best initial guess*/
    (void)memcpy( &y, &i, sizeof(uint32_t) );   /* allowed type-punning*/
    y  = y * ( 1.5f - ( xHalf * y * y ) );      /* 1st iteration*/
    return y*x;                                 /* multiply the result by x to get sqrt(x)*/
}
/*============================================================================*/