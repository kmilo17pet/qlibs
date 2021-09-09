/*!
 * @file qssmoother.h
 * @author J. Camilo Gomez C.
 * @version 1.02
 * @note This file is part of the qTools distribution.
 * @brief API to smooth noisy signals.
 **/

#ifndef QSIGNAL_SMOOTHER
#define QSIGNAL_SMOOTHER

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <float.h>
    #include <math.h>

    /*! @cond  */
    typedef void* qSSmootherPtr_t;
    /*! @endcond */
    

    typedef struct _qSSmoother_s 
    {
        /*! @cond  */
        float (*filtFcn)( struct _qSSmoother_s *f, float s ); 
        uint8_t init;
        /*! @endcond  */
    } _qSSmoother_t;

    typedef struct 
    {   
        /*! @cond  */
        _qSSmoother_t f;
        float alpha, y1;
        /*! @endcond  */
    } qSSmoother_LPF1_t;

    typedef struct 
    {
        /*! @cond  */
        _qSSmoother_t f;
        float alpha, y1, y2, x1, x2;
        float k, a1, a2, b1;
        /*! @endcond  */
    } qSSmoother_LPF2_t;

    typedef struct
    {
        /*! @cond  */
        _qSSmoother_t f;
        float *w;
        size_t wsize;
        /*! @endcond  */
    } qSSmoother_MWM_t;

    typedef struct
    {
        /*! @cond  */
        _qSSmoother_t f;
        float *w, m, alpha;
        size_t wsize;
        /*! @endcond  */
    } qSSmoother_MWOR_t;    

    typedef struct
    {
        /*! @cond  */
        _qSSmoother_t f;
        float *w, *k;
        size_t wsize;
        /*! @endcond  */
    } qSSmoother_GAUSSIAN_t;    

    /**
    * @brief Check if the smoother filter is initialized.
    * @param[in] s A pointer to the signal smoother instance.
    * @return 1 if the smoother has been initialized, otherwise return 0.
    */        
    int qSSmoother_IsInitialized( qSSmootherPtr_t s );

    /**
    * @brief Reset the the smoother filter.
    * @param[in] s A pointer to the signal smoother instance.
    * @return 1 on success, otherwise return 0.
    */     
    int qSSmoother_Reset( qSSmootherPtr_t s );

    /**
    * @brief Perform the smooth operation recursively for the input signal @a x.
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] x A sample of the input signal.
    * @return The smoothed output.
    */    
    float qSSmoother_Perform( qSSmootherPtr_t s, float x );

    /**
    * @brief Setup an initialize the order-1 Low-Pass filter for signal smoothing
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] alpha The smoother tune value. [ 0 < alpha < 1 ] 
    * @return 1 on success, otherwise return 0.
    */  
    int qSSmoother_Setup_LPF1( qSSmoother_LPF1_t *s, float alpha );

    /**
    * @brief Setup an initialize the order-2 Low-Pass filter for signal smoothing
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] alpha The smoother tune value. [ 0 < alpha < 1 ] 
    * @return 1 on success, otherwise return 0.
    */      
    int qSSmoother_Setup_LPF2( qSSmoother_LPF2_t *s, float alpha );

    /**
    * @brief Setup an initialize the moving average filter for signal smoothing
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] window An array of @a wsize elements to hold the moving window.
    * @param[in] wsize The number of elements in @a window.
    * @return 1 on success, otherwise return 0.
    */      
    int qSSmoother_Setup_MWM( qSSmoother_MWM_t *s, float *window, size_t wsize );

    /**
    * @brief Setup an initialize the outlier-removal filter by using a 
    * moving-average filter. 
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] window An array of @a wsize elements to hold the moving window.
    * @param[in] wsize The number of elements in @a window.
    * @param[in] alpha The smoother tune value. [ 0 < alpha < 1 ] 
    * @return 1 on success, otherwise return 0.
    */          
    int qSSmoother_Setup_MWOR( qSSmoother_MWOR_t *s, float *window, size_t wsize, float alpha );
    
    /**
    * @brief Setup an initialize the gaussian filter for signal smoothing
    * @param[in] s A pointer to the signal smoother instance.
    * @param[in] window An array of @a wsize elements to hold the moving window.
    * @param[in] kernel An array of @a wsize to hold the coefficients of the kernel.
    * @param[in] wsize The number of elements in @a window.
    * @param[in] sigma Standard deviation. The smoother tune value.  [ sigma > 0 ] 
    * @param[in] c The offset of the gaussian center. [ 0 < pos < (wsize-1) ] 
    * @return 1 on success, otherwise return 0.
    */     
    int qSSmoother_Setup_GAUSSIAN( qSSmoother_GAUSSIAN_t *s, float *window, float *kernel, size_t wsize, float sigma, size_t c );

#ifdef __cplusplus
}
#endif

#endif