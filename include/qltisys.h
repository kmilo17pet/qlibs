/*!
 * @file qltisys.h
 * @author J. Camilo Gomez C.
 * @version 1.07
 * @note This file is part of the qTools distribution.
 * @brief API to simulate continuous and discrete LTI systems.
 **/

#ifndef QLTISYS_H
#define QLTISYS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <float.h>
    #include "qtdl.h"
    #include "qnuma.h"

    /** @addtogroup  qltisys Recursive LTI systems evaluation by transfer functions
    * @brief API for the qLTISys library for recursive evaluation of LTI systems
    * defined by transfer functions
    *  @{
    */

    /**
    * @brief Macro to specify that the system is time-discrete
    */
    #define QLTISYS_DISCRETE        ( -1.0f )

    /**
    * @brief Type to specify continuos states 
    */
    typedef qNumA_state_t qLTISys_ContinuosX_t;

    /**
    * @brief Type to specify continuos states 
    */
    typedef float qLTISys_DiscreteX_t;

    /**
    * @brief A LTI system object
    * @details The instance should be initialized using the qLTISys_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct _qLTISys_s
    {
        /*! @cond  */
        float (*sysUpdate)( struct _qLTISys_s *sys, float u );
        float *b, *a;
        float *xd;
        qNumA_state_t *xc;
        qTDL_t tDelay;
        float dt, b0, min, max;
        size_t n, na, nb;
        qNumA_IntegrationMethod_t integrate;
        /*! @endcond  */
    } qLTISys_t;

    /**
    * @brief Drives the LTI system recursively using the input signal provided
    * @pre Instance must be previously initialized by qLTISys_Setup()
    * @note The user must ensure that this function is executed in the time
    * specified in @a dt either by using a HW or SW timer, a real time task,
    * or a timing service.
    * @param[in] sys A pointer to the LTI system instance
    * @param[in] u A sample of the input signal that excites the system
    * @return The system response.
    */
    float qLTISys_Excite( qLTISys_t * const sys,
                          float u );

    /**
    * @brief Set the input delay for LTI system.
    * @param[in] sys A pointer to the LTI system instance
    * @param[in] w A array of n elements with the delay window for the
    *  input channel.
    * @param[in] n The number of elements of @a w.
    * @param[in] initVal The initial value of the input channel.
    * @return 1 if the system has been initialized, otherwise return 0.
    */
    int qLTISys_SetDelay( qLTISys_t * const sys,
                          float * const w,
                          const size_t n,
                          const float initVal );

    /**
    * @brief Setup the output saturation for the LTI system.
    * @param[in] sys A pointer to the LTI system instance
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @return 1 on success, otherwise return 0.
    */
    int qLTISys_SetSaturation( qLTISys_t * const sys,
                               const float min,
                               const float max );

    /**
    * @brief Check if the LTI system is initialized.
    * @return 1 if the system has been initialized, otherwise return 0.
    */
    int qLTISys_IsInitialized( const qLTISys_t * const sys );

    /**
    * @brief Setup and initialize an instance of a LTI system.
    * @param[in] sys A pointer to the continuous LTI system instance
    * @param[in,out] num : An array of n+1(if continuous) or nb+1(if discrete)
    * elements with the numerator coefficients of the transfer function.
    * Coefficients should be given in descending powers of the n or nb-degree
    * polynomial. Coefficients will be normalized internally.
    * @param[in,out] den An array of n+1(if continuous) or na+1(if discrete)
    * elements with the denominator coefficients of the transfer function.
    * Coefficients should be given in descending powers of the n or nb-degree
    * polynomial. Coefficients will be normalized internally.
    * @param[in,out] x Initial conditions of the system. For a continuos system,
    * an array of type qNumA_state_t with n elements.
    * For a discrete system, an array of type float with max(na,nb) elements
    * For both cases, the supplied array will be updated on every invocation of
    * qLTISys_Excite().
    * @param[in] nb The order of polynomial @a num + 1 (Only for discrete systems).
    * example: \f$ b_{0}+b_{1}z^{-1}+b_{2}z^{-2}+b_{3}z^{-3}, nb = 4 \f$
    * @note If the system is continuous, pass 0 as argument.
    * @param[in] na The order of polynomial @a den. (if system is discrete). For
    * continuous system the number of elements of @a num and @a den.
    * 
    * example 1: \f$ a_{0}+a_{1}z^{-1}+a_{2}z^{-2}+a_{3}z^{-3}, na = 3 \f$
    * 
    * example 2: \f$ \frac{ b_{0}s^{2}+b_{1}s+b_{2} }{ a_{0}s^{2} + a_{1}s + a_{2} }, na = 3 \f$
    * @note For continuous systems, size of @a num and @a den should be equal.
    * @param[in] dt The time-step of the continuos system. For discrete systems
    * pass #QLTISYS_DISCRETE as argument
    * @return 1 on success, otherwise return 0.
    */
    int qLTISys_Setup( qLTISys_t * const sys,
                       float *num,
                       float *den,
                       void *x,
                       const size_t nb,
                       const size_t na,
                       const float dt );

    /**
    * @brief Evaluate the discrete FIR filter by updating the delay lines of
    * @a x inside the window @a w of size @a wsize with the coefficients given
    * in @a c. If @a c it's not supplied, this function just perform the window
    * update.
    * @param[in,out] w An array of @a wsize elements that holds the window with
    * the delay lines of @a x.
    * @param[in] c An array of @a wsize elements with the coefficients of the
    * FIR filter. Coefficients should be given in descending powers of the
    * nth-degree polynomial. To ignore pass @c NULL.
    * @param[in] wsize The number of elements of @a w.
    * @param[in] x A sample of the input signal.
    * @return If @a c is provided, returns the evaluation of the FIR filter.
    * otherwise return the sum of the updated window @a w.
    */
    float qLTISys_DiscreteFIRUpdate( float *w,
                                     const float * const c,
                                     const size_t wsize,
                                     const float x );

    /**
    * @brief Set integration method for continuos systems.
    * @param[in] sys A pointer to the continuous LTI system instance
    * @param[in] im The desired integration method. Use one of the following:
    *
    * @c qNumA_IntegralRe : Integrate using the Rectangular rule.
    *
    * @c qNumA_IntegralTr : (default) Integrate using the Trapezoidal rule.
    *
    * @c qNumA_IntegralSi : Integrate using the Simpson's 1/3 rule.
    *
    * @return 1 on success, otherwise return 0.
    */
    int qLTISys_SetIntegrationMethod( qLTISys_t * const sys,
                                      qNumA_IntegrationMethod_t im );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif
