/*!
 * @file qpid.h
 * @author J. Camilo Gomez C.
 * @version 1.03
 * @note This file is part of the qTools distribution.
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

    /** 
    * @brief A PID controller object
    * @details The instance should be initialized using the qPID_Setup() API.
    * @note Do not access any member of this structure directly. 
    */
    typedef struct 
    {
        /*! @cond  */
        uint8_t init;    
        float kc, ki, kd, dt, min, max, epsilon, kw, kt, e1, ie, D, u1, beta;
        float *uEF; /*external feedback for tracking mode*/
        /*! @endcond  */
    } qPID_controller_t;

    /**
    * @brief Setup and initialize the PID controller instance.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kc Proportional Gain.
    * @param[in] ki Integral Gain
    * @param[in] kd Derivative Gain
    * @param[in] dt Time step in seconds.
    * @return 1 on success, otherwise return 0.
    */  
    int qPID_Setup( qPID_controller_t *c, float kc, float ki, float kd, float dt );

    /**
    * @brief Set/Change the PIC controller gains.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] kc Proportional Gain.
    * @param[in] ki Integral Gain
    * @param[in] kd Derivative Gain
    * @return 1 on success, otherwise return 0.
    */  
    int qPID_SetGains( qPID_controller_t *c, float kc, float ki, float kd );

    /**
    * @brief Reset the internal PID controller calculations.
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 on success, otherwise return 0.
    */      
    int qPID_Reset( qPID_controller_t *c );

    /**
    * @brief Setup the output saturation for the PID controller.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @param[in] kw Anti-windup feedback gain. A zero value disables the 
    * anti-windup feature.
    * @return 1 on success, otherwise return 0.
    */     
    int qPID_SetSaturation( qPID_controller_t *c, float min, float max, float kw );

    /**
    * @brief Convert the controller gains to conform the series or interacting
    * form. 
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 on success, otherwise return 0.
    */     
    int qPID_SetSeries( qPID_controller_t *c );

    /**
    * @brief Set the minimum value considered as error
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] eps The minimal error value.
    * @return 1 on success, otherwise return 0.
    */      
    int qPID_SetEpsilon( qPID_controller_t *c, float eps );

    /**
    * @brief Set the tunning parameter for the derivative filter.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] beta The tunning parameter. [ 0 < beta < 1 ]
    * @return 1 on success, otherwise return 0.
    */  
    int qPID_SetDerivativeFilter( qPID_controller_t *c, float beta );

    /**
    * @brief Set the PID tracking mode. This allows the PID controller to adjust
    * its internal state by changing its integrator output so that the output 
    * tracks a prescribed signal feeding this extra input. Can be used to achieve bumpless control transfer.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] var A pointer to the external feedback variable.
    * @param[in] kt Tracking gain.
    * @return 1 on success, otherwise return 0.
    */     
    int qPID_SetTrackingMode( qPID_controller_t *c, float *var, float kt ); 

    /**
    * @brief Computes the control action for given PID controller instance.
    * @param[in] c A pointer to the PID controller instance.
    * @param[in] w The reference value aka SetPoint.
    * @param[in] y The controlled variable aka Process-variable.
    * @return The control action.
    */       
    float qPID_Control( qPID_controller_t *c, float w, float y ); 

 

#ifdef __cplusplus
}
#endif

#endif