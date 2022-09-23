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

    typedef struct _qNumA_state_s
    {
        /*! @cond  */
        float x[ 3 ];
        /*! @endcond  */
    } qNumA_state_t;


    typedef float (*qNumA_IntegrationMethod_t)( qNumA_state_t *x,
                                                const float s,
                                                const float dt );

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
                            const float dt );

     /**
    * @brief Perform a numerical integration step by using the trapezoidal rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the integration step.
    */
    float qNumA_IntegralTr( qNumA_state_t *x,
                            const float s,
                            const float dt );

     /**
    * @brief Perform a numerical integration step by using the Simpson's rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the integration step.
    */
    float qNumA_IntegralSi( qNumA_state_t *x,
                            const float s,
                            const float dt );

     /**
    * @brief Perform a numerical derivation step by using the delta rule.
    * @param[in] x A pointer to the numerical state instance
    * @param[in] s The input signal
    * @param[in] dt The time-step given in seconds.
    * @return The current value of the derivation step.
    */
    float qNumA_Derivative( qNumA_state_t *x,
                            const float s,
                            const float dt );


#ifdef __cplusplus
}
#endif

#endif
