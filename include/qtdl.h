/*!
 * @file qtdl.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qTools distribution.
 * @brief Helper class that implements a Tapped Delay Line (TDL). This class 
 * runs in constant time O(1), so it becomes useful when you need to a moving window and the window is huge.
 **/

#ifndef QTDL_H
#define QTDL_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    
    /*! @cond  */
    typedef struct
    {
        float *head, *tail;
        float *rd, *wr;
        size_t itemcount;
    } qTDL_t;

    void qTDL_Setup( qTDL_t *q, float *area, size_t n, float initval );
    void qTDL_Flush( qTDL_t *q, float initval );
    float qTDL_GetOldest( qTDL_t *q );
    float qTDL_GetRecent( qTDL_t *q );
    void qTDL_InsertSample( qTDL_t *q, float data );
    float qTDL_GetAtIndex( qTDL_t *q, size_t i );
    /*! @endcond  */

#ifdef __cplusplus
}
#endif

#endif