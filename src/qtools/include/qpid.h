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

    int qPID_Setup( qPID_controller_t *c, float kc, float ki, float kd, float dt );
    int qPID_Reset( qPID_controller_t *c );
    int qPID_SetSaturation( qPID_controller_t *c, float min, float max, float kw );
    int qPID_SetParallel( qPID_controller_t *c );
    int qPID_SetEpsilon( qPID_controller_t *c, float eps );
    float qPID_Control( qPID_controller_t *c, float w, float y );    

#ifdef __cplusplus
}
#endif

#endif