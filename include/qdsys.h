/*!
 * @file qdsys.h
 * @author J. Camilo Gomez C.
 * @version 1.02
 * @note This file is part of the qTools distribution.
 * @brief API to simulate discrete LTI systems.
 **/

#ifndef QDSYS_H
#define QDSYS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <float.h>

    /** 
    * @brief A discrete-LTI system object
    * @details The instance should be initialized using the qDSys_Setup() API.
    * @note Do not access any member of this structure directly. 
    */
    typedef struct
    {
        /*! @cond  */
        uint8_t init;
        float *v, *b, *a;
        float b0, min, max;
        size_t n, na;
        /*! @endcond */
    } qDSys_t;

    /**
    * @brief Evaluate the FIR filter by updating the delay lines of @a x inside 
    * the window @a w of size @a wsize with the coefficients given in @a c. If
    * @a c it's not supplied, this function just perform the window update.
    * @param[in,out] w An array of @a wsize elements that holds the window with
    * the window with the delay lines of @a x. 
    * @param[in] c An array of @a wsize elements with the coefficients of the 
    * FIR filter. Coefficients should be given in descending powersof the 
    * nth-degree polynomial. To ignore pass NULL.
    * @param[wsize] The number of elements of @a w.
    * @return If @a c is provided, returns the evaluation of the FIR filter. 
    * otherwise return the sum of the updated window @a w.
    */  
    float qDSys_FIRUpdate( float *w, float *c, size_t wsize, float x );

    /**
    * @brief Setup and initialize an instance of the discrete LTI system.
    * @param[in] sys A pointer to the discrete LST system instance
    * @param[in,out] num An array of nb+1 elements with the numerator coefficients 
    * of the transfer function. Coefficients should be given in descending powers
    * of the nb-degree polynomial. Coefficients will be normalized internally.
    * @param[in,out] den An array of na+1 elements with the numerator coefficients 
    * of the transfer function. Coefficients should be given in descending powers
    * of the na-degree polynomial. Coefficients will be normalized internally.
    * @param[in,out] v An array of max(na,nb) elements to hold the discrete 
    * states of the system.
    * @param[in] nb The order of polynomial @a num.
    * @param[in] na The order of polynomial @a den.
    * @return 1 on success, otherwise return 0.
    */   
    int qDSys_Setup( qDSys_t *sys, float *num, float *den, float *v, size_t nb, size_t na );

    /**
    * @brief Check if the discrete LTI system is initialized.
    * @return 1 if the system has been initialized, otherwise return 0.
    */         
    int qDSys_IsInitialized( qDSys_t *sys );

    /**
    * @brief Setup the output saturation for the discrete LTI system.
    * @param[in] sys A pointer to the discrete LST system instance
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @return 1 on success, otherwise return 0.
    */  
    int qDSys_SetSaturation( qDSys_t *sys, float min, float max );

    /**
    * @brief Drives the system recursively using the input signal provided
    * @param[in] sys A pointer to the discrete LST system instance
    * @param[in] u The current sample of the input signal
    * @return The system response.
    */       
    float qDSys_Excite( qDSys_t *sys, float u );

#ifdef __cplusplus
}
#endif

#endif