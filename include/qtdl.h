/*!
 * @file qtdl.h
 * @author J. Camilo Gomez C.
 * @version 1.04
 * @note This file is part of the qLibs distribution.
 * @brief Helper class that implements a Tapped Delay Line (TDL). A TDL is a
 * delay line that provides access to its contents at arbitrary intermediate
 * delay length values.
 * This class runs in constant time O(1), so it becomes useful when you need to
 * work with long delayed lines.
 **/

#ifndef QTDL_H
#define QTDL_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>


    /** @addtogroup  qtdl qTDL
    * @brief An implementation of the Tapped Delay Line (TDL) structure in O(1) 
    *  @{
    */

    /**
    * @brief A Tapped Delay Line (TDL) object
    * @details The instance should be initialized using the qTDL_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        float *head, *tail;
        float *rd, *wr;
        size_t itemCount;
        /*! @endcond  */
    } qTDL_t;

    /**
    * @brief Setup and initialize a Tapped Delay Line (TDL) instance by setting
    * the default optimal parameters.
    * @param[in] q A pointer to the TDL instance.
    * @param[in] area An array of size @a n where delays will be stored
    * @param[in] n The number of elements on @a area.
    * @param[in] initVal The value with which all TDL delays will be initialized
    * @return none
    */
    void qTDL_Setup( qTDL_t * const q,
                     float * const area,
                     const size_t n,
                     const float initVal );

    /**
    * @brief Clears all delays from the TDL and sets them to the specified value
    * @param[in] q A pointer to the TDL instance.
    * @param[in] initVal The value with which all TDL delays will be initialized
    * @return none
    */
    void qTDL_Flush( qTDL_t * const q,
                     const float initVal );

    /**
    * @brief Get the oldest sample from the TDL x(k-n)
    * @param[in] q A pointer to the TDL instance.
    * @return The requested value from the TDL
    */
    float qTDL_GetOldest( const qTDL_t * const q );

    /**
    * @brief Get the specified delayed sample from the TDL x(k-i)
    * @param[in] q A pointer to the TDL instance.
    * @param[in] i The requested delay index
    * @return The requested value from the TDL
    */
    float qTDL_GetAtIndex( const qTDL_t * const q,
                           const size_t i );

    /**
    * @brief Get the most recent sample from the TDL x(k)
    * @param[in] q A pointer to the TDL instance.
    * @return The requested value from the TDL
    */
    float qTDL_GetRecent( const qTDL_t * const q );

    /**
    * @brief Insert a new sample to the TDL removing the oldest sample
    * @param[in] q A pointer to the TDL instance.
    * @return none
    */
    void qTDL_InsertSample( qTDL_t * const q,
                            const float sample );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif