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

    if ( ( NULL != q ) && ( NULL != window ) && ( wsize > 0u ) ) {
        float defaults[ 2 ] = { 0.99f, 0.75f }; /*default parameters*/
        (void)qSSmoother_Setup( &q->f1, QSSMOOTHER_TYPE_EXPW, &defaults[ 0 ], NULL, 0uL );
        (void)qSSmoother_Setup( &q->f2, QSSMOOTHER_TYPE_MWM2, NULL , window, wsize );
        (void)qSSmoother_Setup( &q->f3, QSSMOOTHER_TYPE_LPF1, &defaults[ 1 ], NULL, 0uL );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
float qRMS_Update( qRMS_t * const q,
                   const float x )
{
    float y = 0.0f;

    if ( NULL != q ) {
        y = qFFMath_Sqrt( qSSmoother_Perform( &q->f1, x*x ) );
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

    if ( ( NULL != q ) && ( lambda > 0.0f ) && ( lambda <= 1.0f ) && ( alpha > 0.0f ) && ( alpha <= 1.0f ) ) {
        q->f1.lambda = lambda;
        q->f3.alpha = alpha;
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/