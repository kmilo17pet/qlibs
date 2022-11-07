/*!
 * @file qfloatextra.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qLibs distribution.
 * @brief Extra utility for for floats
 **/

#ifndef QFLOATEXTRA_H
#define QFLOATEXTRA_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdbool.h>

     /**
    * @brief Scales the given input @a x in value range given by  @a xMin and 
    * @a xMax in value range specified by the @a yMin and @a yMax.
    * @param[in] x Input
    * @param[in] xMin Input minimum value for range
    * @param[in] xMax Input maximum  value for range
    * @param[in] yMin Output minimum value for range
    * @param[in] yMax Output maximum value for range
    * @return The scaled value in range @a yMin and @a yMax.
    */
    float qFloat_MapMinMax( const float x,
                            const float xMin,
                            const float xMax,
                            const float yMin,
                            const float yMax );
    /**
    * @brief Determines if the value pointed by @a x falls within a range 
    * specified by the upper limit and lower limit inputs and coerces the value
    * to fall within the range
    * @param[in,out] x Input
    * @param[in] lowerL Lower limit.
    * @param[in] upperL Upper limit.
    * @return true when the value falls within the specified range, otherwise 
    * false
    */
    bool qFloat_InRangeCoerce( float * const x,
                               const float lowerL,
                               const float upperL );

    /**
    * @brief Determines if the value pointed by @a x falls within a range 
    * specified by the upper limit and lower limit inputs and coerces the value
    * to fall within the range.
    * @param[in,out] x Input
    * @param[in] lowerL Lower limit.
    * @param[in] upperL Upper limit.
    * @return true when the value falls within the specified range, otherwise 
    * false
    */
    bool qFloat_Equal( const float a,
                       const float b );


#ifdef __cplusplus
}
#endif

#endif