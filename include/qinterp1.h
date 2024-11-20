/*!
 * @file qinterp1.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qLibs distribution.
 * @brief Class for a set of one-dimensional interpolators
 **/

#ifndef QCRC_H
#define QCRC_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>

    /** @addtogroup qinterp1 qInterp1
    * @brief One-dimensional interpolation class.
    *  @{
    */

    /**
    * @brief An enum with all the available interpolation methods.
    */
    typedef enum {
        QINTERP1_NEXT = 0,               /*!< Return the next neighbor.*/
        QINTERP1_PREVIOUS,               /*!< Return the previous neighbor.*/
        QINTERP1_NEAREST,                /*!< Return the nearest neighbor.*/
        QINTERP1_LINEAR,                 /*!< Linear interpolation from nearest neighbors.*/
        QINTERP1_SINE,                   /*!< Sine interpolation.*/
        QINTERP1_CUBIC,                  /*!< Cubic interpolation.*/
        QINTERP1_HERMITE,                /*!< Piecewise cubic Hermite interpolation.*/
        QINTERP1_SPLINE,                 /*!< Catmull spline interpolation.*/
        QINTERP1_CONSTRAINED_SPLINE,     /*!< A special kind of spline that doesn't overshoot.*/
        QINTERP1_MAX,
    } qInterp1Method_t;


    /**
    * @brief A 1D interpolation object.
    */
    typedef struct {
        float (*method)( const float x,
                         const float * const tx,
                         const float * const ty,
                         const size_t tableSize );
        const float *xData;
        const float *yData;
        size_t dataSize;
    } qInterp1_t;

    /**
    * @brief Setup and initialize the 1D interpolation instance.
    * @param[in] i A pointer to the interpolation instance.
    * @param[in] xTable An array of size @a sizeTable with the x points sorted in ascending order.
    * @param[in] yTable An array of size @a sizeTable with the y points.
    * @param[in] sizeTable The number of points in @a xTable @a yTable
    * @return 1 on success, otherwise return 0.
    */
    int qInterp1_Setup( qInterp1_t * const i,
                         const float * const xTable,
                         const float * const yTable,
                         const size_t sizeTable );

    /**
    * @brief Set the data table for the 1D interpolation instance.
    * @param[in] i A pointer to the interpolation instance.
    * @param[in] xTable An array of size @a sizeTable with the x points sorted in ascending order.
    * @param[in] yTable An array of size @a sizeTable with the y points.
    * @param[in] sizeTable The number of points in @a xTable @a yTable
    * @return 1 on success, otherwise return 0.
    */
    int qInterp1_SetData( qInterp1_t * const i,
                          const float * const xTable,
                          const float * const yTable,
                          const size_t sizeTable );

    /**
    * @brief Specify the interpolation method to use.
    * @param[in] i A pointer to the interpolation instance.
    * @param[in] m The interpolation method.
    * @return 1 on success, otherwise return 0.
    */
    int qInterp1_SetMethod( qInterp1_t * const i,
                            const qInterp1Method_t m );


    /**
    * @brief Interpolate input point @a x to determine the value of @a y
    * at the points @a xi using the current method. If value is beyond
    * the endpoints, extrapolation is performed using the current method.
    * @param[in] i A pointer to the interpolation instance.
    * @param[in] x The input point.
    * @return The interpolated-extrapolated @a y value.
    */
    float qInterp1_Get( qInterp1_t * const i,
                        const float x );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif