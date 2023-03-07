/*!
 * @file qffmath.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qffmath.h"

#ifndef QLIBS_USE_STD_MATH
#include <stdint.h>
#include <string.h>
#include <float.h>

/*cppcheck-suppress misra-c2012-20.7 */
#define cast_reinterpret( dst, src, dst_type )                              \
(void)memcpy( &dst, &src, sizeof(dst_type) )                                \

static float qFFMath_CalcCbrt( float x , bool r );
/*============================================================================*/
float _qFFMath_GetAbnormal( const int i )
{
    static const uint32_t u_ab[ 2 ] = { 0x7F800000u, 0x7FBFFFFFu };
    static float f_ab[ 2 ] = { 0.0f, 0.0f };
    static bool init = true;
    
    if ( init ) {
        /*cppcheck-suppress misra-c2012-21.15 */
        (void)memcpy( f_ab, u_ab, sizeof(f_ab) );
        init = false;
    }
    
    return f_ab[ i ]; 
}
/*============================================================================*/
int qFFMath_FPClassify( const float f ) 
{
    uint32_t u = 0u;
    int retVal;

    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( u, f, uint32_t );
    u &= 0x7FFFFFFFu;

    if ( 0u == u ) {
        retVal = QFFM_FP_ZERO;
    }
    else if ( u < 0x00800000u ) {
        retVal = QFFM_FP_SUBNORMAL;
    }
    else if ( u < 0x7F800000u ) {
        retVal = QFFM_FP_NORMAL;
    }
    else if ( 0x7F800000U == u ) {
        retVal = QFFM_FP_INFINITE;
    }
    else {
        retVal = QFFM_FP_NAN;
    }

    return retVal;
}
/*============================================================================*/
bool qFFMath_IsNaN( const float x )
{
    return ( QFFM_FP_NAN == qFFMath_FPClassify( x ) );
}
/*============================================================================*/
bool qFFMath_IsInf( const float x )
{
    return ( QFFM_FP_INFINITE == qFFMath_FPClassify( x ) );
}
/*============================================================================*/
bool qFFMath_IsFinite( const float x )
{
    return ( qFFMath_FPClassify( x ) < QFFM_FP_INFINITE );
}
/*============================================================================*/
bool qFFMath_IsNormal( const float x )
{
    return ( qFFMath_FPClassify( x ) == QFFM_FP_NORMAL );
}
/*============================================================================*/
float qFFMath_Abs( float x )
{
    return ( x < 0.0f ) ? -x : x;
}
/*============================================================================*/
float qFFMath_Recip( float x )
{
    uint32_t y = 0u;
    float z = 0.0f;

    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( y, x, uint32_t );
    y = 0x7EF311C7u - y;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( z, y, float );
    
    return z*( 2.0f - ( x*z ) );
}
/*============================================================================*/
float qFFMath_Sqrt( float x )
{
    float retVal;

    if ( x < 0.0f ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = 0.0f;
    }
    else {
        uint32_t y = 0u;
        float z = 0.0f;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, x, uint32_t );
        y = ( ( y - 0x00800000u ) >> 1u ) + 0x20000000u;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( z, y, float );
        retVal = ( ( x/z ) + z ) * 0.5f;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_RSqrt( float x )
{
    float retVal;

    if ( x < 0.0f ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = QFFM_INFINITY;
    }
    else {
        uint32_t y = 0u;
        float z = 0.5f*x;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, x, uint32_t );
        y = 0x5F375A86u - ( y >> 1u );
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( x, y, float );
        retVal = x*( 1.5f - ( z*x*x ) );
    }

    return retVal;
}
/*============================================================================*/
static float qFFMath_CalcCbrt( float x , bool r )
{
    float retVal, y = 0.0f, c, d;
    const float k[ 3 ] = { 1.752319676f, 1.2509524245f, 0.5093818292f };
    uint32_t i = 0u;
    bool neg = false;

    if ( x < 0.0f ) {
        x = -x;
        neg = true;
    }
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( i, x, uint32_t );
    i = 0x548C2B4Bu - ( i/3u );
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( y, i, float );
    c = x*y*y*y;
    y = y*( k[ 0 ] - ( c*( k[ 1 ] - ( k[ 2 ]*c ) ) ) );

    d = x*y*y;
    c = 1.0f - ( d*y );
    retVal = 1.0f + ( 0.333333333333f*c );
    retVal *= ( r ) ? y : d;
    return ( neg )? -retVal : retVal;
}
/*============================================================================*/
float qFFMath_Cbrt( float x )
{
    return qFFMath_CalcCbrt( x, false );
}
/*============================================================================*/
float qFFMath_RCbrt( float x )
{
    return ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) ? QFFM_INFINITY
                                                       : qFFMath_CalcCbrt( x, true );
}
/*============================================================================*/
float qFFMath_Round( float x )
{
    x += 12582912.0f;
    x -= 12582912.0f;
    return x;
}
/*============================================================================*/
float qFFMath_Floor( float x )
{
    return qFFMath_Round( x - 0.5f );
}
/*============================================================================*/
float qFFMath_Ceil( float x )
{
    return qFFMath_Round( x + 0.5f );
}
/*============================================================================*/
float qFFMath_Trunc( float x )
{
    /*cstat -CERT-FLP36-C -CERT-FLP34-C*/
    return (float)( (int32_t)x );
    /*cstat +CERT-FLP36-C +CERT-FLP34-C*/
}
/*============================================================================*/
float qFFMath_Frac( float x )
{
    return x - qFFMath_Trunc( x );
}
/*============================================================================*/
float qFFMath_Remainder( float x,
                         float y )
{
     return x - ( y*qFFMath_Floor( x/y ) );
}
/*============================================================================*/
float qFFMath_Mod( float x,
                   float y )
{
    return ( QFFM_FP_ZERO == qFFMath_FPClassify( x )) ? QFFM_NAN
                                                      : ( x - ( y*qFFMath_Trunc( x/y ) ) );
}
/*============================================================================*/
float qFFMath_Sin( float x )
{
    float y;

    x *= -QFFM_1_PI;
    y = x + 25165824.0f;
    x -= y - 25165824.0f;
    x *= qFFMath_Abs( x ) - 1.0f;

    return x*( ( 3.5841304553896f*qFFMath_Abs( x ) ) + 3.1039673861526f );
}
/*============================================================================*/
float qFFMath_Cos( float x )
{
    return qFFMath_Sin( x + QFFM_PI_2 );
}
/*============================================================================*/
float qFFMath_Tan( float x )
{
    return qFFMath_Sin( x )/qFFMath_Cos( x );
}
/*============================================================================*/
float qFFMath_ASin( float x )
{
    x = qFFMath_Sqrt( 1.0f + x ) - qFFMath_Sqrt( 1.0f - x );
    return x*( ( 0.131754508171f*qFFMath_Abs( x ) ) + 0.924391722181f );
}
/*============================================================================*/
float qFFMath_ACos( float x )
{
    return QFFM_PI_2 - qFFMath_ASin( x );
}
/*============================================================================*/
float qFFMath_ATan( float x )
{
    float abs_x;

    x /= qFFMath_Abs( x ) + 1.0f;
    abs_x = qFFMath_Abs( x );

    return x*( ( abs_x*( ( -1.45667498914f*abs_x ) + 2.18501248371f ) ) + 0.842458832225f );
}
/*============================================================================*/
float qFFMath_ATan2( float y, float x )
{
    float t, f;

    t = QFFM_PI - ( ( y < 0.0f ) ? 6.283185307f : 0.0f );
    f = ( qFFMath_Abs( x ) <= FLT_MIN ) ? 1.0f : 0.0f;
    y = qFFMath_ATan( y/( x + f ) ) + ( ( x < 0.0f ) ? t : 0.0f );

    return y + ( f*( ( 0.5f*t ) - y ) );
}
/*============================================================================*/
float qFFMath_Exp2( float x )
{
    float retVal;

    if ( x <= -126.0f ) {
        retVal = 0.0f;
    }
    else if ( x > 128.0f ) {
        retVal = QFFM_INFINITY;
    }
    else {
        float y = 0.0f;
        uint32_t exponent;
        /*cstat -CERT-FLP34-C -MISRAC2012-Rule-10.8*/
        exponent = (uint32_t)( x + 127.0f );
        /*cstat +CERT-FLP34-C +MISRAC2012-Rule-10.8 -CERT-FLP36-C*/
        x += 127.0f - (float)exponent;
        /*cstat +CERT-FLP36-C*/
        exponent <<= 23u;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, exponent, float );
        x *= ( x*0.339766027f ) + 0.660233972f;
        retVal = y*( x + 1.0f );
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Log2( float x )
{
    float retVal;

    if ( x < 0.0f ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = -QFFM_INFINITY;
    }
    else {
        uint32_t y = 0u, y2;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, x, uint32_t );
        y2 = y;
        y >>= 23u;
        /*cstat -CERT-FLP36-C*/
        retVal = (float)y;
        /*cstat +CERT-FLP36-C*/
        y = ( y2 & 0x007FFFFFu ) | 0x3F800000u;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( x, y, float );
        retVal += -128.0f + ( x*( ( -0.333333333f*x ) + 2.0f ) ) - 0.666666666f;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Exp( float x )
{
    return qFFMath_Exp2( QFFM_LOG2E*x );
}
/*============================================================================*/
float qFFMath_Exp10( float x )
{
    return qFFMath_Exp2( 3.32192809f*x );
}
/*============================================================================*/
float qFFMath_Log( float x )
{
    return QFFM_LN2*qFFMath_Log2(x);
}
/*============================================================================*/
float qFFMath_Log10( float x )
{
    return 0.301029996f*qFFMath_Log2(x);
}
/*============================================================================*/
float qFFMath_Pow( float b,
                   float e )
{
    return qFFMath_Exp2( e*qFFMath_Log2( b ) );
}
/*============================================================================*/
float qFFMath_Sinh( float x )
{
    x = qFFMath_Exp( x );
    return ( ( x - 1.0f )/x )*0.5f;
}
/*============================================================================*/
float qFFMath_Cosh( float x )
{
    x = qFFMath_Exp( x );
    return ( ( x + 1.0f )/x )*0.5f;
}
/*============================================================================*/
float qFFMath_Tanh( float x )
{
    x = qFFMath_Exp( -2.0f*x );
    return ( 1.0f - x )/( 1.0f + x );
}
/*============================================================================*/
float qFFMath_ASinh( float x )
{
    return qFFMath_Log( x + qFFMath_Sqrt( ( x*x ) + 1.0f ) );
}
/*============================================================================*/
float qFFMath_ACosh( float x )
{
    return ( x < 1.0f ) ? QFFM_NAN
                        : qFFMath_Log( x + qFFMath_Sqrt( ( x*x ) - 1.0f ) );
}
/*============================================================================*/
float qFFMath_ATanh( float x )
{
    return qFFMath_Log( ( 1.0f + x )/( 1.0f - x ) )*0.5f;
}
/*============================================================================*/
float qFFMath_Erf( float x )
{
    float retVal;

    if ( x >= 6.912f ) {
        retVal = 1.0f;
    }
    else {
        x = qFFMath_Exp( 3.472034176f*x );
        retVal = ( x/( ( qFFMath_Abs( x ) + 1.0f )*2.0f ) ) - 1.0f;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Erfc( float x )
{
    return 1.0f - qFFMath_Erf( x );
}
/*============================================================================*/
float qFFMath_Max( float x,
                   float y )
{
    return ( x > y ) ? x : y;
}
/*============================================================================*/
float qFFMath_Min( float x,
                   float y )
{
    return ( x < y ) ? x : y;
}
/*============================================================================*/
float qFFMath_RExp( float x,
                    int32_t *pw2 )
{
    uint32_t lu = 0u, iu;
    int32_t i = 0;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( lu, x, uint32_t );
    iu  = ( lu >> 23u ) & 0x000000FFu;  /* Find the exponent (power of 2) */
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( i, iu, int32_t );
    i -= 0x7E;
    pw2[ 0 ] = (int)i;
    lu &= 0x807FFFFFu; /* strip all exponent bits */
    lu |= 0x3F000000u; /* mantissa between 0.5 and 1 */
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( x, lu, float );

    return x;
}
/*============================================================================*/
float qFFMath_LDExp( float x,
                     int32_t pw2 )
{
    uint32_t lu = 0u, eu;
    int32_t e = 0;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( lu, x, uint32_t );
    eu = ( lu >> 23u ) & 0x000000FFu;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( e, eu, int32_t );
    e += pw2;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( eu, e, uint32_t );
    lu = ( ( eu & 0xFFu ) << 23u ) | ( lu & 0x807FFFFFu );
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( x, lu, float );

    return x;
}
/*============================================================================*/
float qFFMath_Hypot( float x,
                     float y )
{
    float retVal;
    /*cstat -MISRAC2012-Rule-13.5*/ 
    if ( qFFMath_IsFinite( x ) && qFFMath_IsFinite( y ) ) {
        float a, b, an, bn;;
        int32_t e = 0;

        if ( x >= y ) {
            a = x;
            b = y;
        }
        else {
            a = y;
            b = x;
        }
        /* Write a = an * 2^e, b = bn * 2^e with 0 <= bn <= an < 1.*/
        an = qFFMath_RExp( a, &e );
        bn = qFFMath_LDExp( b, -e );
        retVal = qFFMath_Sqrt( ( an*an ) + ( bn*bn ) );
        retVal = qFFMath_LDExp( retVal, e );
    }
    else {
        retVal = ( qFFMath_IsInf( x ) || qFFMath_IsInf( y ) ) ? QFFM_INFINITY
                                                              : QFFM_NAN;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return retVal;
}
/*============================================================================*/
float qFFMath_NextAfter( float x,
                         float y )
{
    float retVal;
    uint32_t ax, ay, uxi = 0u, uyi = 0u;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( uxi, x, uint32_t );
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( uyi, y, uint32_t );
    /*cstat -MISRAC2012-Rule-13.5*/ 
    if ( qFFMath_IsNaN( x ) || qFFMath_IsNaN( y ) ) {
    /*cstat +MISRAC2012-Rule-13.5*/ 
        retVal = QFFM_NAN;
    }
    else if ( uxi == uyi ) {
        retVal = y;
    }
    else {
        ax = uxi & 0x7FFFFFFFu;
        ay = uyi & 0x7FFFFFFFu;
        if ( 0u == ax ) {
            uxi = ( 0u == ay ) ? uyi : ( ( uyi & 0x80000000u ) | 1u );
        }
        else if ( ( ax > ay ) || ( 0u != ( ( uxi^uyi ) & 0x80000000u ) ) ) {
            uxi--;
        }
        else {
            uxi++;
        }
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( retVal, uxi, float );
    }

    return retVal;
}
/*============================================================================*/
#endif /*#ifndef QLIBS_USE_STD_MATH*/