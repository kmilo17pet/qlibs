/*!
 * @file qpid.h
 * @author J. Camilo Gomez C.
 * @version 1.07
 * @note This file is part of the qLibs distribution.
 * @brief API to control systems using the PID algorithm. This controller
 * features anti-windup, tracking mode, and derivative filter.
 **/

#ifndef QPID_H
#define QPID_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <float.h>
    #include <stdint.h>
    #include <math.h>
    #include "qnuma.h"

    typedef struct
    {
        float p00, p01, p10, p11;   /*covariance values*/
        float b1, a1;               /*estimation  values*/
        float uk, yk;               /*process I/O measurements*/
        float l, il;                /*memory factor [ 0.9 < l < 1 ]*/
        float k, tao;               /*process metrics*/
        float mu, speed;            /*fine adjustments  [ 0 < mu < speed ] [ 0 < speed < 1 ]*/
        uint32_t it;                /*enable time*/
    } qPID_AutoTunning_t;

    #define QPID_AUTOTUNNING_UNDEFINED      ( 0xFFFFFFFEuL )


    /**
    * @brief A PID controller object
    * @details The instance should be initialized using the qPID_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        float kc, ki, kd, dt, min, max, epsilon, kw, kt, D, u1, beta;
        float *uEF; /*external feedback for tracking mode*/
        const float *yr;
        float alfa, gamma; /*mrac additive controller parameters*/
        qNumA_state_t c_state; /*controller integral & derivative state*/
        qNumA_state_t m_state; /*mrac additive controller state*/
        qPID_AutoTunning_t *adapt;
        float (*integrate)( qNumA_state_t x, const float s, const float dt );
        uint8_t init;
        /*! @endcond  */
    } qPID_controller_t;

    /**
    * @brief Setup and initialize the PID controller instance.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kc Proportional Gain.
    * @param[in] ki Integral Gain.
    * @param[in] kd Derivative Gain.
    * @param[in] dt Time step in seconds.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_Setup( qPID_controller_t * const c,
                    const float kc,
                    const float ki,
                    const float kd,
                    const float dt );

    /**
    * @brief Set/Change the PID controller gains.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kc Proportional Gain.
    * @param[in] ki Integral Gain.
    * @param[in] kd Derivative Gain.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetGains( qPID_controller_t * const c,
                       const float kc,
                       const float ki,
                       const float kd );

    /**
    * @brief Reset the internal PID controller calculations.
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_Reset( qPID_controller_t * const c );

    /**
    * @brief Setup the output saturation for the PID controller.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @param[in] kw Anti-windup feedback gain. A zero value disables the
    * anti-windup feature.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetSaturation( qPID_controller_t * const c,
                            const float min,
                            const float max,
                            const float kw );

    /**
    * @brief Convert the controller gains to conform the series or interacting
    * form.
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetSeries( qPID_controller_t * const c );

    /**
    * @brief Set the minimum value considered as error.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] eps The minimal error value.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetEpsilon( qPID_controller_t * const c,
                         const float eps );

    /**
    * @brief Set the tunning parameter for the derivative filter.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] beta The tunning parameter. [ 0 < beta < 1 ]
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetDerivativeFilter( qPID_controller_t * const c,
                                  const float beta );

    /**
    * @brief Set the PID tracking mode. This allows the PID controller to adjust
    * its internal state by changing its integrator output so that the output
    * tracks a prescribed signal feeding this extra input. Can be used to
    * achieve bumpless control transfer.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] var A pointer to the external feedback variable.
    * @param[in] kt Tracking gain.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetTrackingMode( qPID_controller_t * const c,
                              float *var,
                              const float kt );

    /**
    * @brief Enable the additive MRAC(Model Reference Adaptive Control) feature.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] modelref A pointer to the output of the model reference.
    * @param[in] gamma Adjustable parameter to indicate the adaptation speed.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetMRAC( qPID_controller_t * const c,
                    const float *modelref,
                    const float gamma );

    /**
    * @brief Computes the control action for given PID controller instance.
    * @note The user must ensure that this function is executed in the time
    * specified in @a dt either by using a HW or SW timer, a real time task,
    * or a timing service.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] w The reference value aka SetPoint.
    * @param[in] y The controlled variable aka Process-variable.
    * @return The control action.
    */
    float qPID_Control( qPID_controller_t * const c,
                        const float w,
                        const float y );

    /**
    * @brief Binds the specified instance to enable the PID controller auto
    * tuning algorithm.
    * @note To unbind the auto tunning algorithm, pass NULL as argument.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] at A pointer to the PID auto tunning instance.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_BindAutoTunning( qPID_controller_t * const c,
                              qPID_AutoTunning_t * const at );

    /**
    * @brief Set the number of time steps where the auto tuner algorithm will
    *  modify the controller gains.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] tEnable The number of time steps. To keep the auto tuner
    * enabled indefinitely pass #QPID_AUTOTUNNING_UNDEFINED as argument.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_EnableAutoTunning( qPID_controller_t * const c,
                                const uint32_t tEnable );


#ifdef __cplusplus
}
#endif

#endif
