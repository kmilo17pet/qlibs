/*!
 * @file qfmathex.h
 * @author J. Camilo Gomez C.
 * @version 1.03
 * @note This file is part of the qLibs distribution.
 * @brief Extra floating-point math and analysis functions
 **/

#ifndef QFMATHEX_H
#define QFMATHEX_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdbool.h>

    /** @addtogroup  qfmathex qFMathEx
    * @brief Extra floating-point math and analysis functions
    *  @{
    */

     /**
    * @brief Scales the given input @a x in value range given by  @a xMin and 
    * @a xMax to value range specified by the @a yMin and @a yMax.
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
    * @brief Normalize the given input @a x in value range given by @a xMin and 
    * @a xMax to value range between 0 and 1.
    * @param[in] x Input
    * @param[in] xMin Input minimum value for range
    * @param[in] xMax Input maximum  value for range
    * @return The scaled value in range [0 - 1].
    */
    float qFMathEx_Normalize( const float x,
                              const float xMin,
                              const float xMax );

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
    * @brief Determines if the parameters given as single-precision 
    * floating-point values are approximately equal.
    * @param[in] a Input to be compared.
    * @param[in] b Input to be compared.
    * @param[in] tol Tolerance
    * @return true when both values are approximately equal.
    */
    bool qFMathEx_AlmostEqual( const float a,
                               const float b,
                               const float tol );

    /**
    * @brief Determines if the parameters given as single-precision 
    * floating-point values are equal.
    * @param[in] a Input to be compared.
    * @param[in] b Input to be compared.
    * @return true when both values are equal.
    */
    bool qFMathEx_Equal( const float a,
                         const float b );

    /**
    * @brief Determines if the point at ( @a x, @a y ) is inside the polygon
    * given by the set of points on  @a px and @a py.
    * @param[in] x Point x-coordinate
    * @param[in] y Point y-coordinate 
    * @param[in] px x-coordinate points of the polygon 
    * @param[in] py y-coordinate points of the polygon 
    * @param[in] p Number of points that represent the polygon 
    * @return true when the given point is inside the polygon 
    */
    bool qFMathEx_InPolygon( const float x,
                             const float y,
                             const float * const px,
                             const float * const py,
                             const size_t p );

    /**
    * @brief Determines if the point at ( @a x, @a y) is inside the circle 
    * with radius @a r located at @a cx and @a cy.
    * @param[in] x Point x-coordinate
    * @param[in] y Point y-coordinate
    * @param[in] cx X-location of the circle
    * @param[in] cy Y-location of the circle
    * @param[in] r Radio of the circle 
    * @return true when the given point is inside the circle 
    */
    bool qFMathEx_isInsideCircle( const float x,
                                  const float y,
                                  const float cx,
                                  const float cy,
                                  const float r );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif