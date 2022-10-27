/*!
 * @file qpid.h
 * @author J. Camilo Gomez C.
 * @version 1.14
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

    /** @addtogroup  qpid Closed Loop PID Controller library
    * @brief API for the qPID Controller library
    *  @{
    */

    /**
    * @brief Enumeration with the operational modes for the PID controller
    */
    typedef enum {
        qPID_Automatic, /*!< Fully operational closed-loop PID controller */
        qPID_Manual     /*!< Open-loop with manual input*/
    } qPID_Mode_t;


    /**
    * @brief Direction modes of the PID controller
    */
    typedef enum {
        qPID_Forward,   /*!< Forward control action */
        qPID_Backward   /*!< Reverse the control action*/
    } qPID_Direction_t;

    /**
    * @brief A PID Auto-tunning object
    * @details The instance should be bound to a configured PID controller by
    * using the qPID_BindAutoTunning() API
    */
    typedef struct
    {
        /*! @cond  */
        float p00, p01, p10, p11;   /*covariance values*/
        float b1, a1;               /*estimation  values*/
        float uk, yk;               /*process I/O measurements*/
        float l, il;                /*memory factor [ 0.9 < l < 1 ]*/
        float k, tao;               /*process metrics*/
        float mu, speed;            /*fine adjustments  [ 0 < mu < speed ] [ 0 < speed < 1 ]*/
        uint32_t it;                /*enable time*/
        /*! @endcond  */
    } qPID_AutoTunning_t;

    /**
    * @brief Macro to keep the auto-tuner enabled indefinitely
    */
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
        float m, mInput;
        const float *yr;
        float alfa, gamma; /*MRAC additive controller parameters*/
        qNumA_state_t c_state; /*controller integral & derivative state*/
        qNumA_state_t m_state; /*MRAC additive controller state*/
        qNumA_state_t b_state; /*Bumples-transfer state*/
        qPID_AutoTunning_t *adapt;
        qNumA_IntegrationMethod_t integrate;
        qPID_Mode_t mode;
        qPID_Direction_t dir;
        uint8_t init, dKick;
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
    * @brief Set the PID control action direction.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] d Desired output direction.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetDirection( qPID_controller_t * const c,
                        const qPID_Direction_t d );

    /**
    * @brief Set/Change the PID controller gains by using the [Kc, Ti Td ] 
    * triplet.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kc Proportional Gain.
    * @param[in] ti Integral time.
    * @param[in] td Derivative time.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetParams( qPID_controller_t * const c,
                        const float kc,
                        const float ti,
                        const float td );

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
    * @brief Set/Change extra PID controller gains.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kw Saturation feedback gain. Used for antiWindup and bumpless
    * transfer. A zero value disables these features.
    * @param[in] kt Manual input gain.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetExtraGains( qPID_controller_t * const c,
                            const float kw,
                            const float kt );

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
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetSaturation( qPID_controller_t * const c,
                            const float min,
                            const float max );

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
    * @brief Set the PID manual input mode. This value will be used
    * as the manual input when the controller it set into the ::qPID_Manual
    * mode. Bumpless-transfer is guaranteed.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] manualInput The value of the manual input.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetManualInput( qPID_controller_t * const c,
                             const float manualInput );

    /**
    * @brief Change the controller operational mode.
    * In ::qPID_Automatic mode, the computed output of the PID controller
    * will be used as the control signal for the process. In
    * ::qPID_Manual mode, the manual input will be used as the control
    * signal for the process and the PID controller loop will continue
    * operating to guarantee the bumpless-transfer when a switch to 
    * the ::qPID_Automatic mode its performed;
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] m The desired operational mode.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetMode( qPID_controller_t * const c,
                      const qPID_Mode_t m );

    /**
    * @brief Enable the additive MRAC(Model Reference Adaptive Control) feature.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] modelRef A pointer to the output of the model reference.
    * @param[in] gamma Adjustable parameter to indicate the adaptation speed.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_SetMRAC( qPID_controller_t * const c,
                    const float *modelRef,
                    const float gamma );

    /**
    * @brief Computes the control action for given PID controller instance.
    * @pre Instance must be previously initialized by qPID_Setup()
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
    * @note To unbind the auto-tunning algorithm, pass @c NULL as argument.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] at A pointer to the PID auto tunning instance.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_BindAutoTunning( qPID_controller_t * const c,
                              qPID_AutoTunning_t * const at );

    /**
    * @brief Feature to eliminate the phenomena called Derivative Kick
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] tEnable The value to enable(1u) or disable(0u) this feature.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_RemoveDerivativeKick( qPID_controller_t * const c,
                                   const uint32_t tEnable );

    /**
    * @brief Set the number of time steps where the auto tuner algorithm will
    * modify the controller gains.
    * @pre Controller must have an qPID_AutoTunning_t object already binded wih
    * qPID_BindAutoTunning()
    * @note To disable auto-tunning pass a 0uL value to the @a tEnable argument.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] tEnable The number of time steps. To keep the auto tuner
    * enabled indefinitely pass #QPID_AUTOTUNNING_UNDEFINED as argument.
    * @return 1 on success, otherwise return 0.
    */
    int qPID_EnableAutoTunning( qPID_controller_t * const c,
                                const uint32_t tEnable );

    /**
    * @brief Verifies that the auto tuning process has finished with new gains 
    * set on the controller
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 if auto-tunning its complete, otherwise return 0.
    */
    int qPID_AutoTunningComplete( const qPID_controller_t * const c );

    /**
    * @brief Change parameters of the auto-tunning algorithm.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] mu Algorithm momentum. [ 0 <= alfa <= 1 ].
    * @param[in] alfa Final controller speed adjustment. [ 0 < alfa <= 1 ].
    * @param[in] lambda Algorithm forgetting factor [ 0.8 <= lambda <= 1 ].
    * @return 1 on success, 0 on failure.
    */
    int qPID_AutoTunningSetParameters( qPID_controller_t * const c,
                                       const float mu,
                                       const float alfa,
                                       const float lambda );
    /**
    * @brief Set integration method for the PID controller.
    * @param[in] c A pointer to the PID controller instance.
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
    int qPID_SetIntegrationMethod( qPID_controller_t * const c,
                                   qNumA_IntegrationMethod_t im );


     /** @}*/

#ifdef __cplusplus
}
#endif

#endif
