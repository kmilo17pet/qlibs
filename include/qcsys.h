/*!
 * @file qcsys.h
 * @author J. Camilo Gomez C.
 * @version 1.03
 * @note This file is part of the qTools distribution.
 * @brief API to simulate continuous LTI systems.
 **/

#ifndef QCSYS_H
#define QCSYS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <float.h>

    /** 
    * @brief A continuous-LTI system object
    * @details The instance should be initialized using the qCSys_Setup() API.
    * @note Do not access any member of this structure directly. 
    */
    typedef struct
    {
        /*! @cond  */
        uint8_t init;
        float *x, *b, *a, dt;
        float b0, min, max;
        size_t n;
        /*! @endcond  */
    } qCSys_t;

    /**
    * @brief Setup and initialize an instance of the continuous LTI system.
    * @param[in] sys A pointer to the continuous LST system instance
    * @param[in,out] num An array of n+1 elements with the numerator coefficients 
    * of the transfer function. Coefficients should be given in descending powers
    * of the nth-degree polynomial. Coefficients will be normalized.
    * @param[in,out] den An array of n+1 elements with the numerator coefficients 
    * of the transfer function. Coefficients should be given in descending powers
    * of the nth-degree polynomial. Coefficients will be normalized.
    * @param[in,out] x An array of n elements with the initial conditions of the 
    * system. This array will be update with the current states of the system.
    * @param[in] n The number of elements of @a num and @a den.
    * @param[in] dt The time-step of the system. User is responsible for ensuring this time
    * @return 1 on success, otherwise return 0.
    */   
    int qCSys_Setup( qCSys_t *sys, float *num, float *den, float *x, size_t n, float dt );

    /**
    * @brief Check if the continuous LTI system is initialized.
    * @return 1 if the system has been initialized, otherwise return 0.
    */     
    int qCSys_IsInitialized( qCSys_t *sys );

    /**
    * @brief Setup the output saturation for the continuous LTI system.
    * @param[in] sys A pointer to the continuous LST system instance
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @return 1 on success, otherwise return 0.
    */
    int qCSys_SetSaturation( qCSys_t *sys, float min, float max );

    /**
    * @brief Drives the system recursively using the input signal provided
    * @param[in] sys A pointer to the continuous LST system instance
    * @param[in] u The current sample of the input signal
    * @return The system response.
    */       
    float qCSys_Excite( qCSys_t *sys, float u );

#ifdef __cplusplus
}
#endif

#endif