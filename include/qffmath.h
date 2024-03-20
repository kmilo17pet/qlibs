/*!
 * @file qffmath.h
 * @author J. Camilo Gomez C.
 * @version 1.06
 * @note This file is part of the qLibs distribution.
 * @brief Fast floating-point math library for applications where speed is more
 * important than accuracy
 **/

#ifndef QFFMATH_H
#define QFFMATH_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdlib.h>
    #include <stdbool.h>

#ifdef QLIBS_USE_STD_MATH
    /*! @cond  */
    #include <math.h>
    #define QLIB_ABS        fabsf
    #define QLIB_COS        cosf
    #define QLIB_POW        powf
    #define QLIB_EXP        expf
    #define QLIB_LOG        logf
    #define QLIB_SQRT       sqrtf
    #define QLIB_ISNAN      isnan
    #define QLIB_ISINF      isinf
    #define QLIB_MAX        fmaxf
    #define QLIB_MIN        fminf
    #define QLIB_NAN        NAN
    #define QLIB_MOD        fmodf
    #define QLIB_ROUND      roundf
    /*! @endcond  */
#else
    /*! @cond  */
    #define QLIB_ABS        qFFMath_Abs
    #define QLIB_COS        qFFMath_Cos
    #define QLIB_POW        qFFMath_Pow
    #define QLIB_EXP        qFFMath_Exp
    #define QLIB_LOG        qFFMath_Log
    #define QLIB_SQRT       qFFMath_Sqrt
    #define QLIB_ISNAN      qFFMath_IsNaN
    #define QLIB_ISINF      qFFMath_IsInf
    #define QLIB_MAX        qFFMath_Max
    #define QLIB_MIN        qFFMath_Min
    #define QLIB_NAN        QFFM_NAN
    #define QLIB_FMOD       qFFMath_Mod
    #define QLIB_ROUND      qFFMath_Round
    /*! @endcond  */

    /** @addtogroup  qffmath qFFMath
    * @brief Fast floating-point math library for applications where speed is
    * more important than accuracy
    *  @{
    */

    /*! @cond  */
    float _qFFMath_GetAbnormal( const int i ); //skipcq: CXX-E2000
    /*! @endcond  */

    /** @brief The base of natural logarithms ( e ) given as a single-precision floating-point number*/
    #define QFFM_E          ( 2.7182818284590452354F )
    /** @brief The base 2 logarithm of e ( log_2 e ) given as a single-precision floating-point number */
    #define QFFM_LOG2E      ( 1.4426950408889634074F )
    /** @brief The base 10 logarithm of e ( log_10 e ) given as a single-precision floating-point number */
    #define QFFM_LOG10E     ( 0.43429448190325182765F )
    /** @brief The natural logarithm of 2 ( ln 2 ) given as a single-precision floating-point number */
    #define QFFM_LN2        ( 0.69314718055994530942F )
    /** @brief The natural logarithm of 10 ( ln 10 ) given as a single-precision floating-point number */
    #define QFFM_LN10       ( 2.30258509299404568402F )
    /** @brief The circumference of a circle with diameter 1, ( π ) given as a single-precision floating-point number */
    #define QFFM_PI         ( 3.14159265358979323846F )
    /** @brief Half of π ( π/2 ) given as a single-precision floating-point number */
    #define QFFM_PI_2       ( 1.57079632679489661923F )
    /** @brief A quarter of π ( π/4 ) given as a single-precision floating-point number */
    #define QFFM_PI_4       ( 0.78539816339744830962F )
    /** @brief The inverse of π  ( 1/π ) given as a single-precision floating-point number */
    #define QFFM_1_PI       ( 0.31830988618379067154F )
    /** @brief Twice the inverse of π  (  2/π ) given as a single-precision floating-point number */
    #define QFFM_2_PI       ( 0.63661977236758134308F )
    /** @brief The inverse of the square root of π ( 2/√π ) given as a single-precision floating-point number */
    #define QFFM_2_SQRTPI   ( 1.12837916709551257390F )
    /** @brief The square root of 2 ( √2 ) given as a single-precision floating-point number */
    #define QFFM_SQRT2      ( 1.41421356237309504880F )
    /** @brief The inverse of square root of 2 ( 1/√2 ) given as a single-precision floating-point number */
    #define QFFM_SQRT1_2    ( 0.70710678118654752440F )
    /** @brief The natural logarithm of the square root of 2π given as a single-precision floating-point number */
    #define QFFM_LN_SQRT_2PI ( 0.918938533204672669540968854562379419F )

    /** @brief The maximum value of a non-infinite single-precision floating-point number */
    #define QFFM_MAXFLOAT   ( 3.40282347e+38F )
    /** @brief Positive infinity given as a single-precision floating-point number */
    #define QFFM_INFINITY   _qFFMath_GetAbnormal( 0 )
    /** @brief Not a Number (NaN) given as a single-precision floating-point number */
    #define QFFM_NAN        _qFFMath_GetAbnormal( 1 )

    /** @brief Indicates that the value is positive or negative zero */
    #define QFFM_FP_ZERO        ( 0 )

    /** @brief Indicates that the value is subnormal */
    #define QFFM_FP_SUBNORMAL   ( 1 )

    /** @brief Indicates that the value is normal, i.e. not an infinity,
     * subnormal, not-a-number or zero*/
    #define QFFM_FP_NORMAL      ( 2 )

    /** @brief Indicates that the value is not representable by the underlying
     * type, positive or negative infinity*/
    #define QFFM_FP_INFINITE    ( 3 )

    /** @brief Indicates that the value is not-a-number NaN */
    #define QFFM_FP_NAN         ( 4 )

    /**
    * @brief Categorizes the floating-point number @a x. This function
    * determines whether its argument is a normal floating-point number, or one
    * of several special categories of values, including NaN (not a number),
    * infinity, subnormal floating-point values or zero. To determine what
    * category the argument belongs to, compare the return value with the
    * any of the following number classification macros:
    * - #QFFM_FP_ZERO
    * - #QFFM_FP_SUBNORMAL
    * - #QFFM_FP_NORMAL
    * - #QFFM_FP_INFINITE
    * - #QFFM_FP_NAN
    * @param[in] f The number you want to test.
    * @return One of #QFFM_FP_INFINITE, #QFFM_FP_NAN, #QFFM_FP_NORMAL,
    * #QFFM_FP_SUBNORMAL, or #QFFM_FP_ZERO, specifying the category of @a x.
    */
    int qFFMath_FPClassify( const float f );

    /**
    * @brief Determine if @a x is Not-A-Number (NaN) aka #QFFM_NAN.
    * @param[in] x The number you want to test.
    * @return true if the value of @a x is (NaN) aka #QFFM_NAN, otherwise
    * returns false.
    */
    bool qFFMath_IsNaN( const float x );

    /**
    * @brief Determine if @a x is Infinity.
    * @param[in] x The number you want to test.
    * @return true if the value of @a x is ±Infinity, otherwise returns false.
    */
    bool qFFMath_IsInf( const float x );

    /**
    * @brief Determines if the given floating point number @a x has finite
    * value i.e. it is normal, subnormal or zero, but not infinite or NaN.
    * @param[in] x The number you want to test.
    * @return true if @a x has a finite value, false otherwise
    */
    bool qFFMath_IsFinite( const float x );

    /**
    * @brief Determines if the given floating point number @a x is normal, i.e.
    * is neither zero, subnormal, infinite, or NaN.
    * @param[in] x The number you want to test.
    * @return true if @a x has a normal value, false otherwise
    */
    bool qFFMath_IsNormal( const float x );

    /*! @cond  */
    bool qFFMath_IsEqual( const float a,
                          const float b );

    bool qFFMath_IsAlmostEqual( const float a,
                                const float b,
                                const float tol );
    /*! @endcond  */

    /**
    * @brief Computes the absolute value of a floating point value @a x.
    * @param[in] x The floating point value
    * @return  The absolute value of @a x
    */
    float qFFMath_Abs( float x );

    /**
    * @brief Computes the multiplicative inverse or reciprocal for the value
    * @a x, denoted by @c 1/x or <tt>x^(−1)</tt>
    * @param[in] x The floating point value
    * @return The reciprocal value of @a x
    */
    float qFFMath_Recip( float x );

    /**
    * @brief Computes the square-root of @a x
    * @param[in] x The floating point value
    * @return If no errors occur, square root of @a x, is returned. If a domain
    * error occurs #QFFM_NAN is returned
    */
    float qFFMath_Sqrt( float x );

    /**
    * @brief Computes the reciprocal square-root of @a x denoted as
    * <tt>1/sqrt(x)</tt>
    * @param[in] x The floating point value
    * @return If no errors occur, the reciprocal square root of @a x, is
    * returned. If a domain error occurs #QFFM_NAN is returned
    */
    float qFFMath_RSqrt( float x );

    /**
    * @brief Computes the cubic-root of @a x
    * @param[in] x The floating point value
    * @return If no errors occur, cubic root of @a x, is returned. If a domain
    * error occurs #QFFM_NAN is returned
    */
    float qFFMath_Cbrt( float x );

    /**
    * @brief Computes the reciprocal cubic-root of @a x denoted as
    * <tt>1/cbrt(x)</tt>
    * @param[in] x The floating point value
    * @return If no errors occur, the reciprocal cubic root of @a x, is
    * returned. If a domain error occurs #QFFM_NAN is returned
    */
    float qFFMath_RCbrt( float x );

    /**
    * @brief Computes the nearest integer value to @a x (in floating-point
    * format), rounding halfway cases away from zero.
    * @param[in] x The floating point value
    * @return The nearest integer value to @a x, rounding halfway cases away
    * from zero
    */
    float qFFMath_Round( float x );

    /**
    * @brief Computes the largest integer value not greater than @a x.
    * @param[in] x The floating point value
    * @return The largest integer value not greater than @a x
    */
    float qFFMath_Floor( float x );

    /**
    * @brief Computes the smallest integer value not less than @a x.
    * @param[in] x The floating point value
    * @return The smallest integer value not less than @a x
    */
    float qFFMath_Ceil( float x );

    /**
    * @brief  Computes the nearest integer not greater in magnitude than @a x.
    * @param[in] x The floating point value
    * @return The nearest integer value not greater in magnitude than @a x
    * (in other words, @a x rounded towards zero)
    */
    float qFFMath_Trunc( float x );

    /**
    * @brief Obtain the fractional part of @a x.
    * @param[in] x The floating point value
    * @return The fractional part of @a x
    */
    float qFFMath_Frac( float x );

    /**
    * @brief Computes the IEEE remainder of the floating point division
    * operation @c x/y
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If successful, returns the IEEE floating-point remainder of the
    * division @c x/y. If a domain error occurs, a #QFFM_NAN value is returned.
    */
    float qFFMath_Remainder( float x,
                             float y );

    /**
    * @brief Computes the floating-point remainder of the division operation
    * @c x/y
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If successful, returns the floating-point remainder of the
    * division @c x/y. If a domain error occurs, a #QFFM_NAN value is returned.
    */
    float qFFMath_Mod( float x,
                       float y );

    /**
    * @brief Computes the sine of @a x (measured in radians).
    * @param[in] x The floating point value
    * @return If no errors occur, the sine of @a x @c sin(x) in the range
    * [-1 ; +1], is returned. If a domain error occurs, a #QFFM_NAN value is
    * returned.
    */
    float qFFMath_Sin( float x );

    /**
    * @brief Computes the cosine of @a x (measured in radians).
    * @param[in] x The floating point value
    * @return If no errors occur, the cosine of @a x @c cos(x) in the range
    * [-1 ; +1], is returned. If a domain error occurs, a #QFFM_NAN value is
    * returned.
    */
    float qFFMath_Cos( float x );

    /**
    * @brief Computes the tangent of @a x (measured in radians).
    * @param[in] x The floating point value
    * @return If no errors occur, the tangent of @a x @c tan(x) is returned. If
    * a domain error occurs, a #QFFM_NAN value is returned.
    */
    float qFFMath_Tan( float x );

    /**
    * @brief Computes the principal value of the arc sine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the arc sine of @a x @c arcsin(x) in the range
    * [-pi/2 ; pi/2]. is returned.If a domain error occurs, a #QFFM_NAN value is
    * returned.
    */
    float qFFMath_ASin( float x );

    /**
    * @brief Computes the principal value of the arc cosine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the arc cosine of @a x @c arccos(x) in the
    * range [0 ; pi]. is returned.If a domain error occurs, a #QFFM_NAN value is
    * returned.
    */
    float qFFMath_ACos( float x );

    /**
    * @brief Computes the principal value of the arc tangent of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the arc sine of @a x @c arctan(x) in the range
    * [-pi/2 ; pi/2]. is returned.If a domain error occurs, a #QFFM_NAN value is
    * returned.
    */
    float qFFMath_ATan( float x );

    /**
    * @brief Computes the arc tangent of y/x using the signs of arguments to
    * determine the correct quadrant.
    * @param[in] y The floating point value
    * @param[in] x The floating point value
    * @return If no errors occur, the arc tangent of @c y/x <tt>arctan(y/x)</tt>
    * in the range [-pi ; +pi] radians, is returned. If a domain error occurs,
    * a #QFFM_NAN value is returned.
    */
    float qFFMath_ATan2( float y,
                         float x );

    /**
    * @brief Computes 2 raised to the given power @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the base-2 exponential of @a x <tt>2^x</tt> is
    * returned. If a range error due to overflow occurs, #QFFM_INFINITY is
    * returned.
    */
    float qFFMath_Exp2( float x );

    /**
    * @brief Computes the base 2 logarithm of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the base-2 logarithm of @a x @c log_2(x) is
    * returned. If a domain error occurs, a #QFFM_NAN value is returned. If a
    * pole error occurs, -#QFFM_INFINITY is returned.
    */
    float qFFMath_Log2( float x );

    /**
    * @brief Computes the e (Euler's number, 2.7182818) raised to the given
    * power @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the base-e exponential of @a x <tt>e^(x)</tt>
    * is returned. If a range error due to overflow occurs, +#QFFM_INFINITY is
    * returned.
    */
    float qFFMath_Exp( float x );

    /**
    * @brief Computes the value of 10 raised to the power of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the base-e exponential of @a x <tt>10^(x)</tt>
    * is returned. If a range error due to overflow occurs, ± #QFFM_INFINITY or
    * NAN is returned.
    */
    float qFFMath_Exp10( float x );

    /**
    * @brief Computes the natural (base e) logarithm of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the natural (base-e) logarithm of @a x
    * @c ln(x) is returned. If a domain error occurs, a #QFFM_NAN value is
    * returned. If a pole error occurs, -#QFFM_INFINITY is returned.
    */
    float qFFMath_Log( float x );

    /**
    * @brief Computes the common (base-10) logarithm of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the common (base-10) logarithm of @a x
    * @c log_10(x) is returned. If a domain error occurs, a #QFFM_NAN value is
    * returned. If a pole error occurs, -#QFFM_INFINITY is returned.
    */
    float qFFMath_Log10( float x );

    /**
    * @brief Computes the value of @a b raised to the power @a e.
    * @param[in] b Base as floating point value
    * @param[in] e Exponent as floating point value
    * @return If no errors occur, @a b raised to the power of @a e @c b^e is
    * returned. If a domain error occurs, a #QFFM_NAN value is returned. If a
    * pole error or a range error due to overflow occurs, ± #QFFM_INFINITY is
    * returned.
    */
    float qFFMath_Pow( float b,
                       float e );

    /**
    * @brief Computes hyperbolic sine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the hyperbolic sine of @a x @c sinh(x) is
    * returned. If a range error occurs, a ± #QFFM_INFINITY is value is
    * returned.
    */
    float qFFMath_Sinh( float x );

    /**
    * @brief Computes hyperbolic cosine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the hyperbolic cosine of @a x @c cosh(x) is
    * returned. If a range error occurs, a #QFFM_INFINITY value is returned.
    */
    float qFFMath_Cosh( float x );

    /**
    * @brief Computes hyperbolic tangent of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the hyperbolic tangent of @a x @c tanh(x) is
    * returned.
    */
    float qFFMath_Tanh( float x );

    /**
    * @brief Computes the inverse hyperbolic sine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the inverse hyperbolic sine of @a x
    * <tt>sinh^-1(x)</tt> is returned.
    */
    float qFFMath_ASinh( float x );

    /**
    * @brief Computes the inverse hyperbolic cosine of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the inverse hyperbolic cosine of @a x
    * <tt>cosh^-1(x)</tt> is returned.
    */
    float qFFMath_ACosh( float x );

    /**
    * @brief Computes the inverse hyperbolic tangent of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, the inverse hyperbolic tangent of @a x
    * <tt>tanh^-1(x)</tt> is returned. If a domain error occurs, a #QFFM_NAN
    * value is returned. If a pole error occurs, ± #QFFM_INFINITY is returned.
    */
    float qFFMath_ATanh( float x );

    /**
    * @brief Computes the error function of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, value the error function is returned.
    */
    float qFFMath_Erf( float x );

    /**
    * @brief Computes the complementary error function of @a x.
    * @param[in] x The floating point value
    * @return If no errors occur, value the complementary error function is
    * returned.
    */
    float qFFMath_Erfc( float x );

    /**
    * @brief  Returns the larger of two floating point arguments.
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If successful, returns the larger of two floating point values
    */
    float qFFMath_Max( float x,
                       float y );

    /**
    * @brief  Returns the smaller of two floating point arguments.
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If successful, returns the smaller of two floating point values
    */
    float qFFMath_Min( float x,
                       float y );

    /**
    * @brief Decomposes given floating point value @a x into a normalized
    * fraction and an integral power of two.
    * @param[in] x The floating point value
    * @param[in] pw2 Pointer to integer value to store the exponent to
    * @return If @a x is zero, returns zero and stores zero in @a pw2. Otherwise
    * (if @a x is not zero), if no errors occur, returns the value @a y in the
    * range (-1;-0.5], [0.5; 1) and stores an integer value in @a pw2 such that
    * <tt>y×2^(pw2) = x</tt> . If the value to be stored in @a pw2 is outside
    * the range of an @c int, the behavior is unspecified. If @a x is not a
    * floating-point number, the behavior is unspecified.
    */
    float qFFMath_RExp( float x,
                        int32_t *pw2 );

    /**
    * @brief Multiplies a floating point value @a x by the number 2 raised to
    * the @a pw2 power.
    * @param[in] x The floating point value
    * @param[in] pw2 Integer value
    * @return If no errors occur, @a x multiplied by 2 to the power of @a pw2
    * <tt>x×2^pwd</tt> is returned. If a range error due to overflow occurs,
    * ± #QFFM_INFINITY is returned. If a range error due to underflow occurs,
    * the correct result (after rounding) is returned.
    */
    float qFFMath_LDExp( float x,
                         int32_t pw2 );

    /**
    * @brief Computes the square root of the sum of the squares of @a x and @a y,
    * without undue overflow or underflow at intermediate stages of the
    * computation.
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If no errors occur, the hypotenuse of a right-angled triangle,
    * <tt>sqrt(x^2 + y^2)</tt>, is returned. If a range error due to overflow
    * occurs, +#QFFM_INFINITY is returned. If a range error due to underflow
    * occurs, the correct result (after rounding) is returned.
    */
    float qFFMath_Hypot( float x,
                         float y );

    /**
    * @brief Returns the next representable value of @a x in the direction of
    * @a y. If @a x equals to @a y, @a y is returned.
    * @param[in] x The floating point value
    * @param[in] y The floating point value
    * @return If no errors occur, the next representable value of @a x in the
    * direction of @a y is returned. If @a x equals @a y, then @a yis returned.
    * If a range error due to overflow occurs, ± #QFFM_INFINITY is returned
    * (with the same sign as @a x). If a range error occurs due to underflow,
    * the correct result is returned.
    */
    float qFFMath_NextAfter( float x,
                             float y );

    /**
    * @brief Computes the midpoint of the floating-points @a  a and @a b.
    * @param a A floating point value
    * @param b A floating point value
    * @return Half the sum of @a a and @a b. No overflow occurs. A at most
    * one inexact operation occurs.
    */
    float qFFMath_Midpoint( float a,
                            float b );

    /**
    * @brief  Computes the linear interpolation between @a a and @a b, if
    * the parameter t is inside [0, 1] (the linear extrapolation otherwise),
    * i.e. the result of <tt> a+t*(b-a) </tt> with accounting for floating-point
    * calculation imprecision.
    * @param a A floating point value
    * @param b A floating point value
    * @param t A floating point value
    * @return Return <tt> a+t*(b-a) </tt>. When both @a a and @a b are finite, the
    * following properties are guaranteed:
    * If @c t==0, the result is equal to @a a.
    * If @c t==1, the result is equal to @a b.
    * If @c t>=0 and @c t<=1, the result is finite.
    * If @a t is finite and @c a==b, the result is equal to @a a.
    * If @a t is finite or <tt> (a-b)!=0</tt> with @a t infinity, the result
    * is not @c nan.
    * Let @c CMP(x,y) be @c 1 if @c x>y, @c -1 if @c x<y, and @c 0 otherwise.
    * For any @c t1 and @c t2, the product of: <tt> CMP(lerp(a,b,t2)</tt>,
    * <tt>lerp(a,b,t1)</tt>, <tt> CMP(t2,t1)</tt> , and @c CMP(b,a) is non-negative.
    * (That is, lerp() is monotonic.).
    */
    float qFFMath_Lerp( float a,
                        float b,
                        float t );

    /**
    * @brief Normalize the given input @a x in value range given by @a xMin and
    * @a xMax to value range between 0 and 1.
    * @param[in] x Input
    * @param[in] xMin Input minimum value for range
    * @param[in] xMax Input maximum  value for range
    * @return The scaled value in range [0 - 1].
    */
    float qFFMath_Normalize( const float x,
                             const float xMin,
                             const float xMax );

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
    float qFFMath_Map( const float x,
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
    * @return @c true when the value falls within the specified range, otherwise
    * false
    */
    bool qFFMath_InRangeCoerce( float * const x,
                                const float lowerL,
                                const float upperL );

    /**
    * @brief Determines if the point at ( @a x, @a y ) is inside the polygon
    * given by the set of points on  @a px and @a py.
    * @param[in] x Point x-coordinate
    * @param[in] y Point y-coordinate
    * @param[in] px x-coordinate points of the polygon
    * @param[in] py y-coordinate points of the polygon
    * @param[in] p Number of points that represent the polygon
    * @return @c true when the given point is inside the polygon
    */
    bool qFFMath_InPolygon( const float x,
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
    * @return @c true when the given point is inside the circle
    */
    bool qFFMath_InCircle( const float x,
                           const float y,
                           const float cx,
                           const float cy,
                           const float r );

    /**
    * @brief Computes the gamma function of @a x
    * @param[in] x The floating point value
    * @return Upon successful completion, this function shall return @c Gamma(x).
    * If @a x is a negative integer, a @c inf value shall be returned. If the
    * correct value would cause overflow, qFFMath_TGamma() shall return @c ±Inf,
    * with the same sign as the correct value of the function.
    * If @a x is @c nan, a @c nan shall be returned.
    * If @a x is @c +inf, @a x shall be returned.
    * If @a x is @c ±0, tgamma() shall return @c ±Inf.
    * If @a x is @c -inf, a @c nan  value shall be returned.
    * For IEEE Std 754-1985 float, overflow happens when <tt> 0 < x < 1/FLT_MAX</tt>,
    * and <tt>171.7 < x</tt>.
    */
    float qFFMath_TGamma( float x );

    /**
    * @brief Computes the natural logarithm of the absolute value of the
    * gamma function of @a x
    * @note The argument @a x need not be a non-positive integer ( is defined
    * over the reals, except the non-positive integers).
    * @param[in] x The floating point value
    * @return Upon successful completion, this function shall return the
    * logarithmic gamma of @a x. If @a x is a non-positive integer, qFFMath_TGamma() shall
    * return @c +inf. If the correct value would cause overflow, qFFMath_TGamma() shall
    * return @c ±inf having the same sign as the correct value.
    * If @a x is @c nan, a @c nan shall be returned.
    * If @a x is @c 1 or @c 2, @c +0 shall be returned.
    * If @a x is @c ±inf, @c +inf shall be returned.
    */
    float qFFMath_LGamma( float x );

    /**
    * @brief Return the factorial of the integer part of @a x.
    * @note The argument @a x needs to be positive
    * @warning For @a x values greater than @c 14, result is imprecise because of
    * the limited precision of the 32-bit floating point data-type.
    * With @a x values greater than @c 35, this function overflows.
    * @param[in] x The floating point value
    * @return Upon successful completion, this function shall return the
    * factorial of the integer part of @a x. If @a x is non-positive, qFFMath_Factorial() shall
    * return @c nan. If the correct value would cause overflow, qFFMath_Factorial() shall
    * return @c +inf.
    */
    float qFFMath_Factorial( float x );

#endif /*#ifdef QLIBS_USE_STD_MATH*/

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif
