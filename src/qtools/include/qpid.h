/*!
 * @file qpid.h
 * @author J. Camilo Gomez C.
 * @version 1.03
 * @note This file is part of the qTools distribution.
 * @brief API to control systems using the PID algorithm.
 **/

#ifndef QPID_H
#define QPID_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdio.h>
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
        float kc, ki, kd, dt, min, max, epsilon, kw, e1, ie, u1;
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
    * @brief Reset the internal PID controller calculations.
    * @param[in] c A pointer to the PID controller instance.
    * @return 1 on success, otherwise return 0.
    */      
    int qPID_Reset( qPID_controller_t *c );

    /**
    * @brief Setup the output saturation for the PID controller.
    * @param[in] c A pointer to the discrete LST system instance
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @param[in] kw Anti-windup feedback gain.
    * @return 1 on success, otherwise return 0.
    */     
    int qPID_SetSaturation( qPID_controller_t *c, float min, float max, float kw );

    /**
    * @brief Convert the controller gains to conform the parallel form.
    * @param[in] sys A pointer to the discrete LST system instance
    * @return 1 on success, otherwise return 0.
    */     
    int qPID_SetParallel( qPID_controller_t *c );

    /**
    * @brief Set the minimum value considered as error
    * @param[in] c A pointer to the discrete LST system instance
    * @param[in] epsilon The minimal error value.
    * @return 1 on success, otherwise return 0.
    */      
    int qPID_SetEpsilon( qPID_controller_t *c, float eps );

    /**
    * @brief Computes the control action for given PID controller instance.
    * @param[in] c A pointer to the discrete LST system instance
    * @param[in] w The reference value aka SetPoint.
    * @param[in] y The controlled variable aka Process-variable.
    * @return The control action.
    */       
    float qPID_Control( qPID_controller_t *c, float w, float y );    

#ifdef __cplusplus
}
#endif

#endif