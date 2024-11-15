/*!
 * @file qrms.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qrms.h"
#include "qffmath.h"

/*============================================================================*/
int qRMS_Setup( qRMS_t * const q,
                float * const window,
                const size_t wsize )
{
    int retValue = 0;

    if ( ( NULL != q ) && ( NULL != window ) && ( wsize > 0U ) ) {
        float defaults[ 2 ] = { 0.99F, 0.75F }; /*default parameters*/
        (void)qSSmoother_Setup( &q->f1, QSSMOOTHER_TYPE_EXPW, &defaults[ 0 ], NULL, 0UL );
        (void)qSSmoother_Setup( &q->f2, QSSMOOTHER_TYPE_MWM2, NULL , window, wsize );
        (void)qSSmoother_Setup( &q->f3, QSSMOOTHER_TYPE_LPF1, &defaults[ 1 ], NULL, 0UL );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
float qRMS_Update( qRMS_t * const q,
                   const float x )
{
    float y = 0.0F;

    if ( NULL != q ) {
        y = QLIB_SQRT( qSSmoother_Perform( &q->f1, x*x ) );
        y = qSSmoother_Perform( &q->f2, y ); /*2nd stage moving-window overlap*/
        y = qSSmoother_Perform( &q->f3, y ); /*3rd stage low-pass filter*/
    }

    return y;
}
/*============================================================================*/
int qRMS_SetParams( qRMS_t * const q,
                    const float lambda,
                    const float alpha )
{
    int retValue = 0;

    if ( ( NULL != q ) && ( lambda > 0.0F ) && ( lambda <= 1.0F ) && ( alpha > 0.0F ) && ( alpha <= 1.0F ) ) {
        q->f1.lambda = lambda;
        q->f3.alpha = alpha;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/