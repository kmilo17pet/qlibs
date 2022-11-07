/*!
 * @file qvfloat.h
 * @author J. Camilo Gomez C.
 * @version 1.04
 * @note This file is part of the qLibs distribution.
 * @brief Floating-point Vector(1D-Array) operations
 **/

#ifndef QVFLOAT_H
#define QVFLOAT_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdbool.h>


    /** @addtogroup  qvfloat qFVector
    * @brief Floating-point vector(1D-Array) operations
    *  @{
    */

    /**
    * @brief Supported operators on vectors
    */
    typedef enum {
        VFLOAT_ADD = 0,     /*!< Addition operator*/
        VFLOAT_MUL,         /*!< Multiplication operator*/
        VFLOAT_DIV          /*!< Division operator*/
    } qVFloat_Operation_t;

    /**
    * @brief Metrics returned by qVFloat_Moment()
    */
    typedef struct {
        float mean;         /*!< Mean */
        float avgDev;       /*!< Average Deviation */
        float stdDev;       /*!< Standard Deviation */
        float var;          /*!< Variance */
        float skew;         /*!< Skewness */
        float curt;         /*!< Kurtosis */
    } qVFloat_Moment_t;

    /**
    * @brief Metrics returned by qVFloat_MinMax()
    */
    typedef struct {
        float min;          /*!< Minimal value on vector */
        float max;          /*!< Maximal value on vector */
        size_t index_min;   /*!< Index where minimum value is found*/
        size_t index_max;   /*!< Index where maximum value is found*/
    } qVFloat_MinMax_t;

    /**
    * @brief Computes one of the following 1D-vector operation :
    * - <tt>dst = a*x <o> b*y</tt> if both @a x and @a y are supplied.
    * - <tt>dst = a*x <o> b</tt> if the @a y argument is @c NULL.
    * 
    * Here, <tt><o></tt> corresponds to the operator that will be applied
    * @note If @a y or @a dst are used, they must have the same length as @a x.
    * @param[out] dst The pointer to the destination array where the result
    * will be stored. To ignore pass @c NULL as argument.
    * @param[in] o The desired operator, should be one of the following:
    * - ::VFLOAT_ADD : Addition
    * - ::VFLOAT_MUL : Multiplication
    * - ::VFLOAT_DIV : Division
    * @param[in] a Value to scale the vector @a x.
    * @param[in] x A vector as an 1D float array
    * @param[in] b Value to scale the vector @a y.
    * @param[in] y A vector as an 1D float array. To ignore pass @c NULL as
    * argument.
    * @param[in] n The number of elements of vector @a x and @a y
    * @return The sum of all elements on @a dst after the operation.
    */
    float qVFloat_Operate( float * const dst,
                           qVFloat_Operation_t o,
                           const float a,
                           const float * const x,
                           const float b,
                           const float * const y,
                           const size_t n );

    /**
    * @brief Apply one the supplied function ( @a fx1 or @a fx2 ) to the input 
    * vector(s).
    * - <tt>dst = a*fx1( x )</tt> if @a fx2 is @c NULL
    * - <tt>dst = a*fx2( x, y )</tt> if @a fx1 is @c NULL
    * - <tt>dst = a*fx2( x, b )</tt> if @a fx1 and @a y are @c NULL
    * @note If @a y or @a dst are used, they must have the same length as @a x. 
    * @remark If both function are supplied, only @a fx1 will be applied
    * @param[out] dst The pointer to the destination vector where the result
    * will be stored. To ignore pass @c NULL as argument.
    * @param[in] fx1 Function that will be applied to each element of @a x. This
    * function should take one parameter following this signature:
    * @code{.c}
    * float fx1( float p );
    * @endcode
    * To ignore pass @c NULL as argument.
    * @param[in] fx2 Function that will be applied to each element of @a x. This
    * function should take 2 parameters following this signature:
    * @code{.c}
    * float fx1( float p1, float p2 );
    * @endcode
    * To ignore pass @c NULL as argument.
    * @param[in] x A vector as an 1D float array
    * @param[in] y A vector as an 1D float array. To ignore pass @c NULL as
    * argument.
    * @param[in] a A value to scale the result of @a fx1 and @a fx2.
    * @param[in] b An scalar value used as second argument on @a fx2 if @a y
    * is ignored.
    * @param[in] n The number of elements of vectors @a x and @a y
    * @return The sum of all elements on @a dst after the operation.
    */
    float qVFloat_ApplyFx( float *dst,
                           float (*fx1)(float),
                           float (*fx2)(float, float),
                           float * const x,
                           float * const y,
                           const float a,
                           const float b,
                           const size_t n );

    /**
    * @brief Computes the moment of a distribution for a given set of data 
    * including : Mean, Variance, Average deviation, standard deviation, 
    * skewness and kurtosis.
    * @param[out] m A structure of type qVFloat_Moment_t where the metrics will
    * be stored.
    * @param[in] fx1 The function with one argument to be applied to every 
    * element of @a x. To ignore pass @c NULL as argument.
    * @param[in] fx2 The function with two arguments to be applied to every 
    * element of @a x or/and @a y. To ignore pass @c NULL as argument.
    * @param[in] x A vector as an 1D float array
    * @param[in] y A vector as an 1D float array. To ignore pass @c NULL as
    * argument.
    * @param[in] a A value to scale the result of @a fx1 and @a fx2.
    * @param[in] b An scalar value used as second argument on @a fx2 if @a y
    * is ignored.
    * @param[in] n The number of elements of vectors @a x and @a y
    * @return The sum of all elements on @a dst after the operation.
    */
    int qVFloat_Moment( qVFloat_Moment_t * const m,
                        const float * const x,
                        const size_t n );

    /**
    * @brief Set the value given on @a c to all the elements of the vector 
    * pointed by @a x 
    * @param[in] x A pointer to the input vector 
    * @param[in] c The value to set 
    * @param[in] n Number of elements of the vector 
    * @return A pointer to the vector, same as @a x on success.
    */
    float* qVFloat_Set( float * const x,
                        const float c,
                        const size_t n );

    /**
    * @brief Copies all the values on vector pointed by @a src to the vector 
    * pointed by @a dst.
    * @param[in] dst Pointer to the destination vector
    * @param[in] c Pointer to the source vector 
    * @param[in] n Number of elements of the vector 
    * @return A pointer to the vector, same as @dst.
    */
    float* qVFloat_Copy( float * const dst,
                         const float * const src,
                         const size_t n );

    /**
    * @brief Takes the input vector @a p as a polynomial p and evaluates it for 
    * @a x. The argument p is a vector of length @a n+1 whose 
    * elements are the coefficients (in descending powers) of an nth-degree 
    * polynomial.
    * @param[in] p The polynomial coefficients, specified as a vector.
    * @param[in] x Value to evaluate the polynomial
    * @param[in] n Number of coefficients on polynomial @a p
    * @return A pointer to @a dst.
    */
    float qVFloat_PolyVal( const float * const p,
                           const float x,
                           size_t n );

    /**
    * @brief Generate linearly spaced vector of @n points.  The spacing between 
    * the points is <tt> (x2-x1)/(n-1) </tt>
    * @param[out] dst The pointer to the destination vector where the result
    * will be stored.
    * @param[in] x1 Point interval
    * @param[in] x2 Point interval
    * @param[in] n Number of points
    * @return A pointer to @a dst.
    */
    float* qVFloat_LinSpace( float * const dst,
                             const float x1,
                             const float x2,
                             const size_t n );

    /**
    * @brief Returns the Euclidean distance between vectors @a x and @a 
    * @param[in] x Pointer to the input vector.
    * @param[in] y Pointer to the input vector 
    * @param[in] n Number of elements of the vector 
    * @return The Euclidean distance value.
    */
    float qVFloat_Distance( const float * const x,
                            const float * const y,
                            const size_t n );
    /**
    * @brief Reverse the given vector pointed by @a src. Operation takes place 
    * on the portion of the vector that starts at position @a init to position
    * @a end.
    * @remark If the @a dst argument is ignored, the operation will take place
    * over the @a src argument itself.
    * @param[out] dst The pointer to the destination vector where the result
    * will be stored. To ignore pass @c NULL as argument.
    * @param[in,out] src The input vector to reverse.
    * @param[in] init Position of the first element.
    * @param[in] end Position of the last element.
    * @return A pointer to the reversed vector.
    */
    float* qVFloat_Reverse( float * const dst,
                            float * const src,
                            const size_t low,
                            const size_t high );

    /**
    * @brief Rotates the elements of vector pointed by @a src the number of 
    * places and in the direction indicated by @a k.
    * @remark If the @a dst argument is ignored, the operation will take place
    * over the @a src argument itself.
    * @param[out] dst The pointer to the destination vector where the result
    * will be stored. To ignore pass @c NULL as argument.
    * @param[in,out] src The input vector to rotate.
    * @param[in] k Position of the first element.
    * @param[in] n Number of elements of the vector 
    * @return A pointer to the rotated vector.
    */
    float* qVFloat_Rotate( float * const dst,
                           float * const src,
                           const int k,
                           const size_t n );

    /**
    * @brief Find the smallest and largest elements of vector pointed by @a x. 
    * @param[out] o The pointer where the outcome of this function will be stored
    * @param[in] x The input vector to.
    * @param[in] n Number of places to rotate
    * @return 1 on success, otherwise return 0.
    */
    int qVFloat_MinMax( qVFloat_MinMax_t * const o,
                        const float * const x,
                        const size_t n );

    /**
    * @brief Returns a sorted version of unsorted vector pointed by @a src with 
    * the elements arranged in ascending or descending order according the value
    * or @a dir.
    * @note This function is a wrapper to qTypeGeneric_Sort()
    * @remark If the @a dst argument is ignored, the operation will take place
    * over the @a src argument itself.
    * @param[out] dst The pointer to the destination vector where the result
    * will be stored. To ignore pass @c NULL as argument.
    * @param[in,out] src The input vector to rotate.
    * @param[in] dir Pass false to sort in ascending order, otherwise pass true.
    * @param[in] n Number of elements of the vector 
    * @return A pointer to the sorted vector.
    */
    float* qVFloat_Sort( float * const dst,
                         float * const src,
                         const bool dir,
                         size_t n );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif