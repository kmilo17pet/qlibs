/*!
 * @file qtdl.h
 * @author J. Camilo Gomez C.
 * @version 1.02
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

    void qTDL_Setup( qTDL_t * const q, 
                     float * const area, 
                     const size_t n, 
                     const float initval );
    void qTDL_Flush( qTDL_t * const q, 
                     const float initval );
    float qTDL_GetOldest( const qTDL_t * const q );
    float qTDL_GetAtIndex( const qTDL_t * const q, 
                           const size_t i );
    float qTDL_GetRecent( const qTDL_t * const q );
    void qTDL_InsertSample( qTDL_t * const q, 
                            const float sample );
    
    /*! @endcond  */

#ifdef __cplusplus
}
#endif

#endif