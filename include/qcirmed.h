/*!
 * @file qcirmed.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qTools distribution.
 * @brief API for the fast circular-buffered moving average with large windows.
 **/

#ifndef QCIRMED_H
#define QCIRMED_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdio.h>
    #include <stdlib.h>
    #include <float.h>

    /** 
    * @brief A circular-buffered moving average object
    * @details The instance should be initialized using the qCirMed_Setup() API.
    * @note Do not access any member of this structure directly. 
    */
    typedef struct
    {
        /*! @cond  */
        float *head, *tail, *reader, *writer;
        float sum, recent, oldest;
        size_t itemcount, itemswaiting;
        /*! @endcond  */
    } qCirMed_t;

    /**
    * @brief Setup and initialize an instance of the circular-buffered moving 
    * average object
    * @param[in] b A pointer to the circular-buffered moving average instance.
    * @param[in,out] dataarea An array of @a n elements for the moving window
    * @param[in] n The number of elements inside @a dataarea.
    * @param[in] initval The initial condition of the moving average.
    * @return 1 on success, otherwise return 0.
    */   
    int qCirMed_Setup( qCirMed_t *b, float *dataarea, size_t n, float initval );

    /**
    * @brief Resets the instance of the circular-buffered moving average object.
    * @param[in] b A pointer to the circular-buffered moving average instance.
    * @param[in] initval The initial condition of the moving average.
    * @return 1 on success, otherwise return 0.
    */     
    int qCirMed_Reset( qCirMed_t *b, float initval );

    /**
    * @brief Resets the instance of the circular-buffered moving average object.
    * @param[in] b A pointer to the circular-buffered moving average instance.
    * @param[in] initval The initial condition of the moving average.
    * @return 1 on success, otherwise return 0.
    */     
    int qCirMed_Insert( qCirMed_t *b, float xk );

    /**
    * @brief Computes the latest moving average.
    * @param[in] b A pointer to the circular-buffered moving average instance.
    * @return The moving-average value.
    */   
    float qCirMed_Get( qCirMed_t *b );  

#ifdef __cplusplus
}
#endif

#endif