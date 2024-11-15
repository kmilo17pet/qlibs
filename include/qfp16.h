/*!
 * @file qfp16.h
 * @author J. Camilo Gomez C.
 * @version 1.08
 * @note This file is part of the qLibs distribution.
 * @brief Fixed-Point math Q16.16 with rounding and saturated arithmetic.
 **/

#ifndef QFP16_H
#define QFP16_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdlib.h>

    /** @addtogroup qfp16 Q16.16 Fixed-point math
    * @brief API for the qFP16 Fixed-point math library
    *  @{
    */

    /**  @brief Fixed-point Q16.16 type with width of exactly 32 bits.*/
    typedef int32_t qFP16_t;

    /*! @cond  */
    struct _qFP16_const_s //skipcq: CXX-E2000
    {
        const qFP16_t
        f_e,            /* [ e ] The base of natural logarithms, e.*/
        f_log2e,        /* [log2(e)] The base 2 logarithm of e.*/
        f_log10e,       /* [log10(e)] The base 10 logarithm of e*/
        f_ln2,          /* [ln(2)] The natural logarithm of 2*/
        f_ln10,         /* [ln(10)] The natural logarithm of 10*/
        f_pi,           /* [pi] The circumference of a circle with diameter 1*/
        f_pi_2,         /* [pi/2] Half of pi */
        f_2pi,          /* [ 2*pi ] Twice pì*/
        f_pi_4,         /* [pi/4] A quarter of pi */
        f_1_pi,         /* [1/pi] The inverse of pi.*/
        f_2_pi,         /* [2/pi] Twice the inverse of pi*/
        f_2_sqrtpi,     /* [2/sqrt(pi)] The inverse of the square root of π.*/
        f_sqrt2,        /* [sqrt(2)] The square root of 2*/
        f_sqrt1_2,      /* [1/sqrt(2)] The inverse of the square root of 2*/
        epsilon,
        max,
        overflow,
        one,            /* [ 1 ] */
        one_half,       /* [ 1/2 ] */
        f_180_pi,       /* [ 180/pi ] */
        f_pi_180,       /* [ pi/180 ] */
        f_180,          /* [ 180 ] */
        f_360;          /* [ 360 ] */
    };
    /*! @endcond  */

    /**
    * @brief Fixed-point Q16.16 constants
    */
    extern const struct _qFP16_const_s qFP16; //skipcq: CXX-E2000

    /**
    * @brief A macro for defining a fixed-point constant value.
    * @note You should only use this for constant values. For runtime-conversions
    * use qFP16_IntToFP(), qFP16_FloatToFP() or qFP16_DoubleToFP() instead.
    * @param[in] x A literal value
    * @return The literal argument @a x converted to fixed-point(qFP16_t).
    */
    #define qFP16_Constant(x)                                               \
    ( (qFP16_t)( ( (x) >= 0 ) ? ( (x) * 65536.0F + 0.5F )                   \
                              : ( (x) * 65536.0F - 0.5F ) ) )               \

    /**
    * @brief A Q16.16 fixed-point settings object
    * @details The instance should be accessed using the qFP16_SettingsSet() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        qFP16_t min, max;
        uint8_t rounding, saturate;
        /*! @endcond  */
    } qFP16_Settings_t;

    /**
    * @brief Set settings for the fixed-point API
    * @param[in] instance A pointer to the fixed-point settings instance
    * @param[in] min The minimal value allowed for the output.
    * @param[in] max The maximal value allowed for the output.
    * @param[in] rounding Enable rounding mode.
    * @param[in] saturate Enable saturation mode.
    * @return 1 on success, otherwise return 0.
    */
    int qFP16_SettingsSet( qFP16_Settings_t * const instance,
                           qFP16_t min,
                           qFP16_t max,
                           uint8_t rounding,
                           uint8_t saturate );

    /**
    * @brief Select the provided setting instance to perform fixed-point
    * operations.
    * @param[in] instance A pointer to the fixed-point settings instance. Pass
    * @c NULL to use the default settings.
    * @return none.
    */
    void qFP16_SettingsSelect( qFP16_Settings_t * const instance );

    /**
    * @brief Returns the fixed-point value @a x converted to int.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns @a x converted to int.
    */
    int qFP16_FPToInt( const qFP16_t x );

    /**
    * @brief Returns the int value @a x converted to fixed-point q16.16.
    * @param[in] x The integer value.
    * @return This function returns @a x converted to fixed-point q16.16.
    */
    qFP16_t qFP16_IntToFP( const int x );

    /**
    * @brief Returns the float value @a x converted to fixed-point q16.16.
    * @param[in] x The floating-point value.
    * @return This function returns @a x converted to fixed-point q16.16.
    */
    qFP16_t qFP16_FloatToFP( const float x );

     /**
    * @brief Returns the fixed-point value @a x converted to float.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns @a x converted to float.
    */
    float qFP16_FPToFloat( const qFP16_t x );

    /**
    * @brief Returns the double value @a x converted to fixed-point q16.16.
    * @param[in] x The double precision floating-point value.
    * @return This function returns @a x converted to fixed-point q16.16.
    */
    qFP16_t qFP16_DoubleToFP( const double x );

    /**
    * @brief Returns the fixed-point value @a x converted to double.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns @a x converted to double.
    */
    double qFP16_FPToDouble( const qFP16_t x );

    /**
    * @brief Returns the absolute value of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the absolute value of x.
    */
    qFP16_t qFP16_Abs( const qFP16_t x );

    /**
    * @brief Returns the largest integer value less than or equal to @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the largest integral value not greater
    * than @a x.
    */
    qFP16_t qFP16_Floor( const qFP16_t x );

    /**
    * @brief Returns the smallest integer value greater than or equal to @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the smallest integral value not less than @a x.
    */
    qFP16_t qFP16_Ceil( const qFP16_t x );

    /**
    * @brief Returns the nearest integer value of the fixed-point argument @a x
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the nearest integral value of @a x.
    */
    qFP16_t qFP16_Round( const qFP16_t x );

    /**
    * @brief Returns the fixed-point addition @a x + @a y.
    * @param[in] X The fixed-point(q16.16) value.
    * @param[in] Y The fixed-point(q16.16) value.
    * @return This function returns the addition operation x+y. @c qFP16.overflow
    * when an operation overflow is detected.
    */
    qFP16_t qFP16_Add( const qFP16_t X,
                       const qFP16_t Y );

    /**
    * @brief Returns the fixed-point subtraction  @a x - @a y.
    * @param[in] X The fixed-point(q16.16) value.
    * @param[in] Y The fixed-point(q16.16) value.
    * @return This function returns the subtraction operation x-y. @c qFP16.overflow
    * when an operation overflow is detected.
    */
    qFP16_t qFP16_Sub( const qFP16_t X,
                       const qFP16_t Y );

    /**
    * @brief Returns the fixed-point product operation  @a x * @a y.
    * @param[in] x The fixed-point(q16.16) value.
    * @param[in] y The fixed-point(q16.16) value.
    * @return This function returns the product  operation x*y. @c qFP16.overflow
    * when an operation overflow is detected.
    */
    qFP16_t qFP16_Mul( const qFP16_t x,
                       const qFP16_t y );

    /**
    * @brief Returns the fixed-point division operation  @a x / @a y.
    * @param[in] x The fixed-point(q16.16) value.
    * @param[in] y The fixed-point(q16.16) value.
    * @return This function returns the product operation x/y. @c qFP16.overflow
    * when an operation overflow is detected.
    */
    qFP16_t qFP16_Div( const qFP16_t x,
                       const qFP16_t y );

    /**
    * @brief Returns the fixed-point modulo operation  @a x % @a y.
    * @param[in] x The fixed-point(q16.16) value.
    * @param[in] y The fixed-point(q16.16) value.
    * @return This function returns the modulo operation x%y.
    */
    qFP16_t qFP16_Mod( const qFP16_t x,
                       const qFP16_t y );

    /**
    * @brief Returns the fixed-point square root of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the square root of @a x. For negative
    * numbers, returns @c qFP16.overflow .
    */
    qFP16_t qFP16_Sqrt( qFP16_t x );

    /**
    * @brief Returns the fixed-point value of e raised to the xth power.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns the exponential value of x. @c qFP16.overflow
    * when an operation overflow is detected.
    */
    qFP16_t qFP16_Exp( qFP16_t x );

    /**
    * @brief Returns the fixed-point natural logarithm (base-e logarithm) of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns natural logarithm of @a x. For negative
    * values returns@c qFP16.overflow
    */
    qFP16_t qFP16_Log( qFP16_t x );

    /**
    * @brief Returns the fixed-point log base 2 of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns log base 2 of @a x. For negative values
    * returns @c qFP16.overflow
    */
    qFP16_t qFP16_Log2( const qFP16_t x );

    /**
    * @brief Converts angle units from radians to degrees.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns the angle converted in degrees.
    */
    qFP16_t qFP16_RadToDeg( const qFP16_t x );

    /**
    * @brief Converts angle units from degrees to radians.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in degrees.
    * @return This function returns the angle converted in radians.
    */
    qFP16_t qFP16_DegToRad( const qFP16_t x );

    /**
    * @brief Wrap the fixed-point angle in radians to [−pi pi]
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns the wrapped angle in the range [−pi, pi]
    * of @a x.
    */
    qFP16_t qFP16_WrapToPi( qFP16_t x );

    /**
    * @brief Wrap the fixed-point angle in degrees  to [−180 180]
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in degrees.
    * @return This function returns the wrapped angle in the range [−180, 180]
    * of @a x.
    */
    qFP16_t qFP16_WrapTo180( qFP16_t x );

    /**
    * @brief Computes the fixed-point sine of the radian angle @a x.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns sine of @a x.
    */
    qFP16_t qFP16_Sin( qFP16_t x );

    /**
    * @brief Computes the fixed-point cosine of the radian angle @a x.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns cosine of @a x.
    */
    qFP16_t qFP16_Cos( qFP16_t x );

    /**
    * @brief Computes the fixed-point tangent  of the radian angle @a x.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns tangent of @a x.
    */
    qFP16_t qFP16_Tan( qFP16_t x );

    /**
    * @brief Computes the fixed-point arc tangent in radians of @a y / @a x
    * based on the signs of both values to determine the correct quadrant.
    * @param[in] y The fixed-point(q16.16) value representing an x-coordinate.
    * @param[in] x The fixed-point(q16.16) value representing an y-coordinate.
    * @return This function returns the principal arc tangent of y/x, in the
    * interval [-pi,+pi] radians.
    */
    qFP16_t qFP16_Atan2( const qFP16_t y,
                         const qFP16_t x );

    /**
    * @brief Computes the fixed-point arc tangent of @a x in radians.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns arc tangent of @a x.
    */
    qFP16_t qFP16_Atan( qFP16_t x );

    /**
    * @brief Computes the fixed-point arc sine of @a x in radians.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns arc sine of @a x.
    */
    qFP16_t qFP16_Asin( qFP16_t x );

    /**
    * @brief Computes the fixed-point arc cosine of @a x in radians.
    * @param[in] x The fixed-point(q16.16) value representing an angle expressed
    * in radians.
    * @return This function returns arc cosine of @a x.
    */
    qFP16_t qFP16_Acos( qFP16_t x );

    /**
    * @brief Computes the fixed-point hyperbolic cosine of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns hyperbolic cosine of @a x. If overflow
    * detected returns @c qFP16.overflow. If the function saturates, returns
    * QFP16_EXP_MAX or QFP16_EXP_MIN.
    */
    qFP16_t qFP16_Cosh( qFP16_t x );

    /**
    * @brief Computes the fixed-point hyperbolic sine of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns hyperbolic sine of @a x. If overflow
    * detected returns @c qFP16.overflow. If the function saturates, returns
    * QFP16_EXP_MAX or QFP16_EXP_MIN.
    */
    qFP16_t qFP16_Sinh( qFP16_t x );

    /**
    * @brief Computes the fixed-point hyperbolic tangent  of @a x.
    * @param[in] x The fixed-point(q16.16) value.
    * @return This function returns hyperbolic tangent of @a x. If overflow
    * detected returns @c qFP16.overflow. If the function saturates, returns
    * QFP16_EXP_MAX or QFP16_EXP_MIN.
    */
    qFP16_t qFP16_Tanh( qFP16_t x );

    /**
    * @brief Evaluates the fixed-point polynomial @a p at the point @a x. The
    * argument @a p is a vector of length n+1 whose elements are the coefficients
    * (in descending powers) of an nth-degree polynomial.
    * @param[in] p Polynomial coefficients, specified as a fixed-point(q16.16)
    * array.
    * @param[in] n The number of elements of the fixed-point array @a p.
    * @param[in] x The fixed-point(q16.16) value to evaluate the polynomial.
    * @return This function returns the polynomial evaluation p(x). If overflow
    * detected returns @c qFP16.overflow.
    */
    qFP16_t qFP16_Polyval( const qFP16_t * const p,
                           const size_t n,
                           const qFP16_t x );

    /**
    * @brief Returns @a x raised to the power of the integer part of @a y. (x^y)
    * @param[in] x The fixed-point(q16.16) base value.
    * @param[in] y The fixed-point(q16.16) power value. Only the integer part is
    * taken
    * @return This function returns the result of raising @a x to the power @a y.
    * @c qFP16.overflow when an operation overflow is detected.
    */
    qFP16_t qFP16_IPow( const qFP16_t x,
                        const qFP16_t y );

    /**
    * @brief Returns @a x raised to the power of @a y. (x^y)
    * @param[in] x The fixed-point(q16.16) base value.
    * @param[in] y The fixed-point(q16.16) power value.
    * @return This function returns the result of raising @a x to the power @a y.
    * @c qFP16.overflow when an operation overflow is detected.
    */
    qFP16_t qFP16_Pow( const qFP16_t x,
                       const qFP16_t y );

    /**
    * @brief Converts the fixed-point value to a formatted string.
    * @param[in] num The fixed-point(q16.16) value to be converted to string.
    * @param[in] str Array in memory where to store the resulting null-terminated
    * string.
    * @param[in] decimals Number of decimals to show in the string representation.
    * @note: Max decimal allowed = 5
    * @return A pointer to the resulting null-terminated string, same as
    * parameter @a str
    */
    char* qFP16_FPToA( const qFP16_t num,
                       char *str,
                       int decimals );

    /**
    * @brief Parses the C string @a s, interpreting its content as a fixed-point
    * (q16.16) number and returns its value as a qFP16_t. The function first
    * discards as many whitespace characters (as in isspace) as necessary until
    * the first non-whitespace character is found. Then, starting from this
    * character, takes as many characters as possible that are valid following a
    * syntax resembling that of floating point literals, and interprets them as
    * a fixed-point numerical value. The rest of the string after the last valid
    * character is ignored and has no effect on the behavior of this function.
    * @param[in] s The string beginning with the representation of a
    * floating-point number.
    * @return On success, the function returns the converted floating point
    * number as a fixed-point value. If no valid conversion could be performed,
    * the function returns zero (0.0) or @c qFP16.overflow. If the converted value
    * would be out of the range of representable values by a fixed-point Q16.16,
    * the function returns @c qFP16.overflow .
    */
    qFP16_t qFP16_AToFP( const char *s );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif
