/*!
 * @file qrms.h
 * @author J. Camilo Gomez C.
 * @version 1.03
 * @note This file is part of the qLibs distribution.
 * @brief Computes the RMS (Root Mean Square) of a signal using a 2-step
 * recursive average specially designed for micro-controllers with FPU.
 **/

#ifndef QRMS_H
#define QRMS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include "qssmoother.h"

    typedef struct
    {
        /*! @cond  */
        qSSmoother_EXPW_t f1;
        qSSmoother_MWM2_t f2;
        qSSmoother_LPF1_t f3;
        /*! @endcond  */
    } qRMS_t;

    /**
    * @brief Initialize the RMS instance by setting the default optimal parameters.
    * @param[in] q A pointer to the RMS instance.
    * @return 1 on success, otherwise returns 0.
    */
    int qRMS_Setup( qRMS_t * const q,
                    float * const window,
                    const size_t wsize );

    /**
    * @brief Computes the moving root mean square (RMS) of the input signal.
    * The object uses both the exponential weighting method and the sliding
    * window method to compute the moving RMS.
    * @note For a 50-60Hz sine waveform leave default parameters, use a window
    * of 8 samples and make sure you call the qRMS_Update() at a rate of 1mS.
    * In this configuration you will obtain an optimal estimate that converges
    * in a cycle of the wave
    * @param[in] q A pointer to the RMS instance.
    * @param[in] x The raw signal.
    * @return A recursive estimation of the RMS for the incoming signal @a x.
    */
    float qRMS_Update( qRMS_t * const q,
                       const float x );

    /**
    * @brief Change the recursive parameters for the moving RMS estimator.
    * @param[in] q A pointer to the RMS instance.
    * @param[in] lambda Exponential weighting factor, specified as a positive
    * real scalar in the range [0,1]. A forgetting factor of 0.9 gives more
    * weight to the older data than does a forgetting factor of 0.1. A forgetting
    * factor of 1.0 indicates infinite memory. All the past samples are given an
    * equal weight.
    * @param[in] alpha A parameter to tune the 2nd stage filter. Should be
    * a value between  [ 0 < alpha < 1 ]. A higher value will result in a
    * smoother output but also increasing the convergence time.
    * @return 1 on success, otherwise returns 0.
    */
    int qRMS_SetParams( qRMS_t * const q,
                        const float lambda,
                        const float alpha );


#ifdef __cplusplus
}
#endif

#endif
