/*!
 * @file qrms.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qTools distribution.
 * @brief Computes the RMS (Root Mean Square) of a signal using a 2-step 
 * recursive average specially designed for microcontrollers with FPU.
 **/

#ifndef QRMS_H
#define QRMS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    
    /*with this optimal parameters, estimator should converge at 50mS*/
    #define QRMS_DEFAULT_ALPHA      ( 0.95f )
    #define QRMS_DEFAULT_A1         ( -1.8f )   
    #define QRMS_DEFAULT_A2         ( 0.8181f )   

    typedef struct 
    {
        float m, alpha; 
        float a1, a2, b1;
        float f1, f2;
    } qRMS_t;

    #define QRMS_INITIALIZER        { 0.0f, 0.1f, QRMS_DEFAULT_ALPHA, QRMS_DEFAULT_A1, QRMS_DEFAULT_A2, ( 1.0f + QRMS_DEFAULT_A1 + QRMS_DEFAULT_A2 ), 0.0f, 0.0f }
    
    /**
    * @brief Initialize the RMS instance by setting the default optimal parameters.
    * @param[in] q A pointer to the RMS  instance.
    * @return 1 on success, otherwise returns 0.
    */  
    int qRMS_Init( qRMS_t *q );

    /**
    * @brief Computes the RMS value of the signal @a x by using a recursive 
    * 2-step average. 
    * @warning This function should be called at a rate of Tm = 1mS to be optimal
    * for the default parameters, the RMS value should converge to around 50mS.
    * If you are planning to use a different rate, you should recompute
    * the recursive parameters in order to fit the new sample time.
    * @param[in] q A pointer to the RMS  instance.
    * @param[in] x The raw signal.
    * @return A recursive estimation of the RMS for the incoming signal @a x.
    */      
    float qRMS_Update( qRMS_t *q, float x );

    /**
    * @brief Change the recursive parameters for the RMS estimator
    * @param[in] q A pointer to the RMS instance.
    * @param[in] alpha A parameter to tune the moving-mean window. Should be
    * a value between  [ 0 < alpha < 1 ]. A higher value will result in a 
    * smoother output but also increasing the convergence time.
    * @param[in] a1 First coefficient of the 2nd-order polynomial with the poles
    *  of the recurvise IIR filter
    * @param[in] a2 Second coefficient of the 2nd-order polynomial with the poles
    *  of the recurvise IIR filter
    * @return 1 on success, otherwise returns 0.
    */      
    int qRMS_SetParams( qRMS_t *q, float alpha, float a1, float a2 );

    /**
    * @briefReset the internal RMS estimator calculations.
    * @param[in] q A pointer to the RMS instance.
    * @return 1 on success, otherwise returns 0.
    */ 
    int qRMS_Reset( qRMS_t *q );

#ifdef __cplusplus
}
#endif

#endif