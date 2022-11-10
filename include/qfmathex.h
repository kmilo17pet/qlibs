/*!
 * @file qfloatextra.h
 * @author J. Camilo Gomez C.
 * @version 1.02
 * @note This file is part of the qLibs distribution.
 * @brief Extra floating-point math and analysis functions
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
    float qFMathEx_MapMinMax( const float x,
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
    bool qFMathEx_InRangeCoerce( float * const x,
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
    bool qFMathEx_Equal( const float a,
                         const float b );

    bool qFMathEx_InPolygon( const float x,
                             const float y,
                             const float * const px,
                             const float * const py,
                             const size_t p );

    bool qFMathEx_isInsideCircle( const float x,
                                  const float y,
                                  const float cx,
                                  const float cy,
                                  const float r );

#ifdef __cplusplus
}
#endif

#endif