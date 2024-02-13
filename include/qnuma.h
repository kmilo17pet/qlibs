/*!
 * @file qnuma.h
 * @author J. Camilo Gomez C.
 * @version 1.02
 * @note This file is part of the qLibs distribution.
 * @brief Numerical approximations for real-time signals.
 **/

#ifndef QMSTAT_H
#define QMSTAT_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdbool.h>

    typedef struct _qNumA_state_s
    {
        /*! @cond  */
        float x[ 3 ];
        /*! @endcond  */
    } qNumA_state_t;

    /*! @cond  */
    typedef float (*qNumA_IntegrationMethod_t)( qNumA_state_t *x,
                                                const float s,
                                                const float dt,
                                                const bool bUpdate );
     /*! @endcond  */

    /**
    * @brief Setup the state object
    * @param[in] x A pointer to the state instance
    * @param[in] sn_1 initial condition at time (t-1)
    * @param[in] sn_2 initial condition at time (t-2)
    * @return none
    */
    void qNumA_StateInit( qNumA_state_t *x,
                          const float x0,
                          const float sn_1,
                          const float sn_2 );

    /**
    * @brief Perform a numerical integration step by using the rectangular rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the integration step.
    */
    float qNumA_IntegralRe( qNumA_state_t *x,
                            const float s,
                            const float dt,
                            const bool bUpdate );

     /**
    * @brief Perform a numerical integration step by using the trapezoidal rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the integration step.
    */
    float qNumA_IntegralTr( qNumA_state_t *x,
                            const float s,
                            const float dt,
                            const bool bUpdate );

     /**
    * @brief Perform a numerical integration step by using the Simpson's rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the integration step.
    */
    float qNumA_IntegralSi( qNumA_state_t *x,
                            const float s,
                            const float dt,
                            const bool bUpdate);

     /**
    * @brief Perform a numerical derivation step by using the delta rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the derivation step.
    */
    float qNumA_Derivative2p( qNumA_state_t *x,
                              const float s,
                              const float dt,
                              const bool bUpdate );

     /**
    * @brief Perform a numerical derivation step by using the three-point
    * backward-difference.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the derivation step.
    */
    float qNumA_DerivativeBa( qNumA_state_t *x,
                              const float s,
                              const float dt,
                              const bool bUpdate );

     /**
    * @brief Perform a numerical derivation step by using the three-point
    * forward-difference.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the derivation step.
    */
    float qNumA_DerivativeFo( qNumA_state_t *x,
                              const float s,
                              const float dt,
                              const bool bUpdate );

#ifdef __cplusplus
}
#endif

#endif
