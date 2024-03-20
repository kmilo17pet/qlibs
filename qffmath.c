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
static float lgamma_positive( float x );

/*============================================================================*/
float _qFFMath_GetAbnormal( const int i )
{
    static const uint32_t u_ab[ 2 ] = { 0x7F800000U, 0x7FBFFFFFU };
    static float f_ab[ 2 ] = { 0.0F, 0.0F };
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
    uint32_t u = 0U;
    int retVal;

    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( u, f, uint32_t );
    u &= 0x7FFFFFFFU;

    if ( 0U == u ) {
        retVal = QFFM_FP_ZERO;
    }
    else if ( u < 0x00800000U ) {
        retVal = QFFM_FP_SUBNORMAL;
    }
    else if ( u < 0x7F800000U ) {
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
bool qFFMath_IsAlmostEqual( const float a,
                            const float b,
                            const float tol )
{
    return ( qFFMath_Abs( a - b ) <= qFFMath_Abs( tol ) );
}
/*============================================================================*/
bool qFFMath_IsEqual( const float a,
                      const float b )
{
    return ( qFFMath_Abs( a - b ) <= FLT_MIN );
}
/*============================================================================*/
float qFFMath_Abs( float x )
{
    return ( x < 0.0F ) ? -x : x;
}
/*============================================================================*/
float qFFMath_Recip( float x )
{
    uint32_t y = 0U;
    float z = 0.0F;

    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( y, x, uint32_t );
    y = 0x7EF311C7U - y;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( z, y, float );

    return z*( 2.0F - ( x*z ) );
}
/*============================================================================*/
float qFFMath_Sqrt( float x )
{
    float retVal;

    if ( x < 0.0F ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = 0.0F;
    }
    else {
        uint32_t y = 0U;
        float z = 0.0F;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, x, uint32_t );
        y = ( ( y - 0x00800000U ) >> 1U ) + 0x20000000U;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( z, y, float );
        z = ( ( x/z ) + z ) * 0.5F;
        retVal = 0.5F*( ( x/z ) + z );
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_RSqrt( float x )
{
    float retVal;

    if ( x < 0.0F ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = QFFM_INFINITY;
    }
    else {
        uint32_t y = 0U;
        float z = 0.5F*x;
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( y, x, uint32_t );
        y = 0x5F375A86U - ( y >> 1U );
        /*cppcheck-suppress misra-c2012-21.15 */
        cast_reinterpret( x, y, float );
        retVal = x*( 1.5F - ( z*x*x ) );
    }

    return retVal;
}
/*============================================================================*/
static float qFFMath_CalcCbrt( float x , bool r )
{
    float retVal, y = 0.0F, c, d;
    const float k[ 3 ] = { 1.752319676F, 1.2509524245F, 0.5093818292F };
    uint32_t i = 0U;
    bool neg = false;

    if ( x < 0.0F ) {
        x = -x;
        neg = true;
    }
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( i, x, uint32_t );
    i = 0x548C2B4BU - ( i/3U );
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( y, i, float );
    c = x*y*y*y;
    y = y*( k[ 0 ] - ( c*( k[ 1 ] - ( k[ 2 ]*c ) ) ) );

    d = x*y*y;
    c = 1.0F - ( d*y );
    retVal = 1.0F + ( 0.333333333333F*c );
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
    x += 12582912.0F;
    x -= 12582912.0F;
    return x;
}
/*============================================================================*/
float qFFMath_Floor( float x )
{
    return qFFMath_Round( x - 0.5F );
}
/*============================================================================*/
float qFFMath_Ceil( float x )
{
    return qFFMath_Round( x + 0.5F );
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

    if ( qFFMath_Abs( x ) <= 0.0066F ) {
        y = x;
    }
    else {
        x *= -QFFM_1_PI;
        y = x + 25165824.0F;
        x -= y - 25165824.0F;
        x *= qFFMath_Abs( x ) - 1.0F;
        y = x*( ( 3.5841304553896F*qFFMath_Abs( x ) ) + 3.1039673861526F );
    }
    return y;
}
/*============================================================================*/
float qFFMath_Cos( float x )
{
    float y;
    const float abs_x = qFFMath_Abs( x );

    if ( qFFMath_IsEqual( abs_x, QFFM_PI_2 ) ) {
        y = 1.0e-12F;
    }
    else {
        y = qFFMath_Sin( x + QFFM_PI_2 );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Tan( float x )
{
    return qFFMath_Sin( x )/qFFMath_Cos( x );
}
/*============================================================================*/
float qFFMath_ASin( float x )
{
    x = qFFMath_Sqrt( 1.0F + x ) - qFFMath_Sqrt( 1.0F - x );
    return x*( ( 0.131754508171F*qFFMath_Abs( x ) ) + 0.924391722181F );
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

    x /= qFFMath_Abs( x ) + 1.0F;
    abs_x = qFFMath_Abs( x );

    return x*( ( abs_x*( ( -1.45667498914F*abs_x ) + 2.18501248371F ) ) + 0.842458832225F );
}
/*============================================================================*/
float qFFMath_ATan2( float y, float x )
{
    float t, f;

    t = QFFM_PI - ( ( y < 0.0F ) ? 6.283185307F : 0.0F );
    f = ( qFFMath_Abs( x ) <= FLT_MIN ) ? 1.0F : 0.0F;
    y = qFFMath_ATan( y/( x + f ) ) + ( ( x < 0.0F ) ? t : 0.0F );

    return y + ( f*( ( 0.5F*t ) - y ) );
}
/*============================================================================*/
float qFFMath_Exp2( float x )
{
    float retVal;

    if ( x <= -126.0F ) {
        retVal = 0.0F;
    }
    else if ( x > 128.0F ) {
        retVal = QFFM_INFINITY;
    }
    else {
        float ip, fp;
        float ep_f = 0.0F;
        int32_t ep_i;

        ip = qFFMath_Floor( x + 0.5F );
        fp = x - ip;
        /*cstat -MISRAC2012-Rule-10.1_R6 -CERT-FLP34-C*/
        ep_i = ( (int32_t)( ip ) + 127 ) << 23;
        /*cstat +MISRAC2012-Rule-10.1_R6 +CERT-FLP34-C*/
        x = 1.535336188319500e-4F;
        x = ( x*fp ) + 1.339887440266574e-3F;
        x = ( x*fp ) + 9.618437357674640e-3F;
        x = ( x*fp ) + 5.550332471162809e-2F;
        x = ( x*fp ) + 2.402264791363012e-1F;
        x = ( x*fp ) + 6.931472028550421e-1F;
        x = ( x*fp ) + 1.0F;
        cast_reinterpret( ep_f, ep_i, float );
        retVal = ep_f*x;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Log2( float x )
{
    float retVal;

    if ( x < 0.0F ) {
        retVal = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) {
        retVal = -QFFM_INFINITY;
    }
    else {
        float z, px;
        int32_t ip, fp;
        int32_t val_i = 0;

        cast_reinterpret( val_i, x, int32_t );
        /*cstat -MISRAC2012-Rule-10.1_R6*/
        fp = val_i & 8388607;
        ip = val_i & 2139095040;
        fp |= 1065353216;
        cast_reinterpret( x, fp, float );
        ip >>= 23;
        ip -= 127;
        /*cstat +MISRAC2012-Rule-10.1_R6*/
        if ( x > QFFM_SQRT2 ) {
            x *= 0.5F;
            ++ip;
        }
        x -= 1.0F;
        px = 7.0376836292e-2F;
        px = ( px*x ) - 1.1514610310e-1F;
        px = ( px*x ) + 1.1676998740e-1F;
        px = ( px*x ) - 1.2420140846e-1F;
        px = ( px*x ) + 1.4249322787e-1F;
        px = ( px*x ) - 1.6668057665e-1F;
        px = ( px*x ) + 2.0000714765e-1F;
        px = ( px*x ) - 2.4999993993e-1F;
        px = ( px*x ) + 3.3333331174e-1F;
        z = x*x;
        z = ( x*z*px ) - ( 0.5F*z ) + x;
        z *= QFFM_LOG2E;
        /*cstat -CERT-FLP36-C*/
        retVal = ( (float)ip ) + z;
        /*cstat +CERT-FLP36-C*/
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
    return qFFMath_Exp2( 3.32192809F*x );
}
/*============================================================================*/
float qFFMath_Log( float x )
{
    return QFFM_LN2*qFFMath_Log2(x);
}
/*============================================================================*/
float qFFMath_Log10( float x )
{
    return 0.301029996F*qFFMath_Log2(x);
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
    const float epx = qFFMath_Exp( x );
    const float enx = 1.0F/epx;
    return 0.5F*( epx - enx );
}
/*============================================================================*/
float qFFMath_Cosh( float x )
{
    const float epx = qFFMath_Exp( x );
    const float enx = 1.0F/epx;
    return 0.5F*( epx + enx );
}
/*============================================================================*/
float qFFMath_Tanh( float x )
{
    x = qFFMath_Exp( -2.0F*x );
    return ( 1.0F - x )/( 1.0F + x );
}
/*============================================================================*/
float qFFMath_ASinh( float x )
{
    return qFFMath_Log( x + qFFMath_Sqrt( ( x*x ) + 1.0F ) );
}
/*============================================================================*/
float qFFMath_ACosh( float x )
{
    return ( x < 1.0F ) ? QFFM_NAN
                        : qFFMath_Log( x + qFFMath_Sqrt( ( x*x ) - 1.0F ) );
}
/*============================================================================*/
float qFFMath_ATanh( float x )
{
    return qFFMath_Log( ( 1.0F + x )/( 1.0F - x ) )*0.5F;
}
/*============================================================================*/
float qFFMath_Erf( float x )
{
    float retVal;

    if ( x >= 6.912F ) {
        retVal = 1.0F;
    }
    else {
        x = qFFMath_Exp( 3.472034176F*x );
        retVal = ( x/( ( qFFMath_Abs( x ) + 1.0F )*2.0F ) ) - 1.0F;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Erfc( float x )
{
    return 1.0F - qFFMath_Erf( x );
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
    uint32_t lu = 0U, iu;
    int32_t i = 0;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( lu, x, uint32_t );
    iu  = ( lu >> 23U ) & 0x000000FFU;  /* Find the exponent (power of 2) */
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( i, iu, int32_t );
    i -= 0x7E;
    pw2[ 0 ] = (int)i;
    lu &= 0x807FFFFFU; /* strip all exponent bits */
    lu |= 0x3F000000U; /* mantissa between 0.5 and 1 */
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( x, lu, float );

    return x;
}
/*============================================================================*/
float qFFMath_LDExp( float x,
                     int32_t pw2 )
{
    uint32_t lu = 0U, eu;
    int32_t e = 0;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( lu, x, uint32_t );
    eu = ( lu >> 23U ) & 0x000000FFU;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( e, eu, int32_t );
    e += pw2;
    /*cppcheck-suppress misra-c2012-21.15 */
    cast_reinterpret( eu, e, uint32_t );
    lu = ( ( eu & 0xFFU ) << 23U ) | ( lu & 0x807FFFFFU );
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
    uint32_t ax, ay, uxi = 0U, uyi = 0U;
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
        ax = uxi & 0x7FFFFFFFU;
        ay = uyi & 0x7FFFFFFFU;
        if ( 0U == ax ) {
            uxi = ( 0U == ay ) ? uyi : ( ( uyi & 0x80000000U ) | 1U );
        }
        else if ( ( ax > ay ) || ( 0U != ( ( uxi^uyi ) & 0x80000000U ) ) ) {
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
float qFFMath_Midpoint( float a,
                        float b )
{
    float y;
    const float lo = 2.0F*FLT_MIN;
    const float hi = 0.5F*FLT_MAX;
    const float abs_a = qFFMath_Abs( a );
    const float abs_b = qFFMath_Abs( b );

    if ( ( abs_a <= hi ) && ( abs_b <= hi ) ) {
        y = 0.5F*( a + b );
    }
    else if ( abs_a < lo ) {
        y = a + ( 0.5F*b );
    }
    else if ( abs_b < lo) {
        y = ( 0.5F*a ) + b;
    }
    else {
        y = ( 0.5F*a ) + ( 0.5F*b );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Lerp( float a,
                    float b,
                    float t )
{
    float y;

    if ( ( ( a <= 0.0F ) && ( b >= 0.0F ) ) || ( ( a >= 0.0F ) && ( b <= 0.0F ) ) ) {
        y = ( t*b ) + ( a*( 1.0F - t ) );
    }
    else if ( qFFMath_IsEqual( t, 1.0F ) ) {
        y = b;
    }
    else {
        const float x = a + ( t*( b - a ) );
        y = ( ( t > 1.0F ) == ( b > a ) ) ? ( ( b < x ) ? x : b )
                                          : ( ( b > x ) ? x : b );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Normalize( const float x,
                         const float xMin,
                         const float xMax )
{
    return ( x - xMin )/( xMax - xMin );
}
/*============================================================================*/
float qFFMath_Map( const float x,
                   const float xMin,
                   const float xMax,
                   const float yMin,
                   const float yMax )
{
    return ( ( yMax - yMin )*qFFMath_Normalize( x, xMin, xMax ) ) + yMin;
}
/*============================================================================*/
bool qFFMath_InRangeCoerce( float * const x,
                            const float lowerL,
                            const float upperL )
{
    bool retVal = false;

    if ( qFFMath_IsNaN( x[ 0 ] ) ) {
        x[ 0 ] = lowerL;
    }
    else {
        if ( x[ 0 ] < lowerL ) {
            x[ 0 ] = lowerL;
        }
        else if ( x[ 0 ] > upperL ) {
            x[ 0 ] = upperL;
        }
        else {
            retVal = true;
        }
    }

    return retVal;
}
/*============================================================================*/
bool qFFMath_InPolygon( const float x,
                        const float y,
                        const float * const px,
                        const float * const py,
                        const size_t p )
{
    size_t i;
    bool retVal = false;
    float max_y = py[ 0 ], max_x = px[ 0 ], min_y = py[ 0 ], min_x = px[ 0 ];

    for ( i = 0U ; i < p ; ++i ) {
        max_y = ( py[ i ] > max_y ) ? py[ i ] : max_y;
        max_x = ( px[ i ] > max_x ) ? px[ i ] : max_x;
        min_y = ( py[ i ] < min_y ) ? py[ i ] : min_y;
        min_x = ( px[ i ] < min_x ) ? px[ i ] : min_x;
    }

    if ( ( y >= min_y ) && ( y <= max_y ) && ( x >= min_x ) && ( x <= max_x ) ) {
        size_t j = p - 1U;

        for ( i = 0U ; i < p ; ++i ) {
            if ( ( px[ i ] > x ) != ( px[ j ] > x ) ) {
                const float dx = px[ j ] - px[ i ];
                const float dy = py[ j ] - py[ i ];
                if ( y < ( ( dy*( x - px[ i ] ) )/( dx + py[ i ] ) ) ) {
                    retVal = !retVal;
                }
            }
            j = i;
        }
    }

    return retVal;
}
/*============================================================================*/
bool qFFMath_InCircle( const float x,
                       const float y,
                       const float cx,
                       const float cy,
                       const float r )
{
    const float d = ( ( x - cx )*( x - cx ) ) + ( ( y - cy )*( y - cy ) );
    return ( d <= ( r*r ) );
}
/*============================================================================*/
float qFFMath_TGamma( float x )
{
    float result;

    const int fClass = qFFMath_FPClassify( x );
    if ( QFFM_FP_NAN == fClass ) {
        result = QFFM_NAN;
    }
    else if ( QFFM_FP_ZERO == fClass ) {
        result = QFFM_INFINITY; /* a huge value */
    }
    else if ( QFFM_FP_INFINITE == fClass ) {
        result = ( x > 0.0F ) ? QFFM_INFINITY : QFFM_NAN;
    }
    else {
        bool parity = false;
        float fact = 1.0F;
        float y = x;
        float y1;

        if ( y <= 0.0F ) {
            float isItAnInt;

            y = -x;
            y1 = qFFMath_Trunc( y );
            isItAnInt = y - y1;
            if ( !qFFMath_IsEqual( 0.0F, isItAnInt ) ) {
                const float tmp = 2.0F*qFFMath_Trunc( y1*0.5F );

                if ( !qFFMath_IsEqual( y1, tmp ) ) {
                    parity = true;
                }
                fact = -QFFM_PI/qFFMath_Sin( QFFM_PI*isItAnInt );
                y += 1.0F;
            }
            else {
                result = QFFM_NAN;
                goto EXIT_TGAMMA;
            }
        }
        if ( y < FLT_EPSILON ) {
            if ( y >= FLT_MIN ) {
                result = 1.0F/y;
            }
            else {
                result = QFFM_INFINITY;
            }
        }
        else if ( y < 12.0F ) {
            float num = 0.0F, den = 1.0F, z;
            int n = 0;

            y1 = y;
            if ( y < 1.0F ) {
                z = y;
                y += 1.0F;
            }
            else {
                n = (int)y - 1;
                /*cstat -CERT-FLP36-C */
                y -= (float)n;
                /*cstat +CERT-FLP36-C */
                z = y - 1.0F;
            }

            num = z*( num + -1.71618513886549492533811e+0F );
            den = ( den*z ) -3.08402300119738975254353e+1F;
            num = z*( num + 2.47656508055759199108314e+1F );
            den = ( den*z ) + 3.15350626979604161529144e+2F;
            num = z*( num - 3.79804256470945635097577e+2F );
            den = ( den*z ) - 1.01515636749021914166146e+3F;
            num = z*( num + 6.29331155312818442661052e+2F );
            den = ( den*z ) - 3.10777167157231109440444e+3F;
            num = z*( num + 8.66966202790413211295064e+2F );
            den = ( den*z ) + 2.25381184209801510330112e+4F;
            num = z*( num - 3.14512729688483675254357e+4F );
            den = ( den*z ) + 4.75584627752788110767815e+3F;
            num = z*( num - 3.61444134186911729807069e+4F );
            den = ( den*z ) - 1.34659959864969306392456e+5F;
            num = z*( num + 6.64561438202405440627855e+4F );
            den = ( den*z ) - 1.15132259675553483497211e+5F;

            result = ( num/den ) + 1.0F;
            if ( y1 < y ) {
                  result /= y1;
            }
            else if ( y1 > y ) {
                for ( int i = 0; i < n ; ++i ) {
                    result *= y;
                    y += 1.0F;
                }
            }
        }
        else {
            if ( x <= 171.624F ) { /* x <= xBig */
                const float yy  = y*y;
                float sum = 5.7083835261e-03F;

                sum = ( sum/yy ) - 1.910444077728e-03F;
                sum = ( sum/yy ) + 8.4171387781295e-04F;
                sum = ( sum/yy ) - 5.952379913043012e-04F;
                sum = ( sum/yy ) + 7.93650793500350248e-04F;
                sum = ( sum/yy ) - 2.777777777777681622553e-03F;
                sum = ( sum/yy ) + 8.333333333333333331554247e-02F;

                sum = ( sum /y ) - y + QFFM_LN_SQRT_2PI;
                sum += ( y - 0.5F )*qFFMath_Log( y );
                result = qFFMath_Exp( sum );
            }
            else {
                result = QFFM_INFINITY;
            }
        }
        if ( parity ) {
            result = -result;
        }
        if ( !qFFMath_IsEqual( fact, 1.0F ) ) {
            result = fact/result;
        }
    }

    EXIT_TGAMMA:
    return result;
}
/*============================================================================*/
static float lgamma_positive( float x )
{
    const float d1 = -5.772156649015328605195174e-1F;
    const float d2 = 4.227843350984671393993777e-1F;
    const float d4 = 1.791759469228055000094023e+0F;
    const float pnt68 = 0.6796875F;
    float result;

    if ( x > 171.624F ) {
        result = QFFM_INFINITY;
    }
    else {
        float y, corrector, num, den;

        y = x;
        if ( y <= FLT_EPSILON ) {
            result = -qFFMath_Log( y );
        }
        else if ( y <= 1.5F ) {
            float xMinus;

            if ( y < pnt68 ) {
                corrector = -qFFMath_Log( y );
                xMinus = y;
            }
            else {
                corrector = 0.0F;
                xMinus = ( y - 0.5F ) - 0.5F;
            }
            if ( ( y <= 0.5F ) || ( y >= pnt68 ) ) {
                den = 1.0F;
                num = 0.0F;
                num = ( num*xMinus ) + 4.945235359296727046734888e+0F;
                num = ( num*xMinus ) + 2.018112620856775083915565e+2F;
                num = ( num*xMinus ) + 2.290838373831346393026739e+3F;
                num = ( num*xMinus ) + 1.131967205903380828685045e+4F;
                num = ( num*xMinus ) + 2.855724635671635335736389e+4F;
                num = ( num*xMinus ) + 3.848496228443793359990269e+4F;
                num = ( num*xMinus ) + 2.637748787624195437963534e+4F;
                num = ( num*xMinus ) + 7.225813979700288197698961e+3F;
                den = ( den*xMinus ) + 6.748212550303777196073036e+1F;
                den = ( den*xMinus ) + 1.113332393857199323513008e+3F;
                den = ( den*xMinus ) + 7.738757056935398733233834e+3F;
                den = ( den*xMinus ) + 2.763987074403340708898585e+4F;
                den = ( den*xMinus ) + 5.499310206226157329794414e+4F;
                den = ( den*xMinus ) + 6.161122180066002127833352e+4F;
                den = ( den*xMinus ) + 3.635127591501940507276287e+4F;
                den = ( den*xMinus ) + 8.785536302431013170870835e+3F;
                result = corrector + ( xMinus*( d1 + ( xMinus*( num/den ) ) ) );
            }
            else {
                xMinus = ( y - 0.5F ) - 0.5F;
                den = 1.0F;
                num = 0.0F;
                num = ( num*xMinus ) + 4.974607845568932035012064e+0F;
                num = ( num*xMinus ) + 5.424138599891070494101986e+2F;
                num = ( num*xMinus ) + 1.550693864978364947665077e+4F;
                num = ( num*xMinus ) + 1.847932904445632425417223e+5F;
                num = ( num*xMinus ) + 1.088204769468828767498470e+6F;
                num = ( num*xMinus ) + 3.338152967987029735917223e+6F;
                num = ( num*xMinus ) + 5.106661678927352456275255e+6F;
                num = ( num*xMinus ) + 3.074109054850539556250927e+6F;
                den = ( den*xMinus ) + 1.830328399370592604055942e+2F;
                den = ( den*xMinus ) + 7.765049321445005871323047e+3F;
                den = ( den*xMinus ) + 1.331903827966074194402448e+5F;
                den = ( den*xMinus ) + 1.136705821321969608938755e+6F;
                den = ( den*xMinus ) + 5.267964117437946917577538e+6F;
                den = ( den*xMinus ) + 1.346701454311101692290052e+7F;
                den = ( den*xMinus ) + 1.782736530353274213975932e+7F;
                den = ( den*xMinus ) + 9.533095591844353613395747e+6F;
                result = corrector + ( xMinus*( d2 + ( xMinus*( num/den ) ) ) );
            }
        }
        else if ( y <= 4.0F ) {
            const float xMinus = y - 2.0F;
            den = 1.0F;
            num = 0.0F;
            num = ( num*xMinus ) + 4.974607845568932035012064e+0F;
            num = ( num*xMinus ) + 5.424138599891070494101986e+2F;
            num = ( num*xMinus ) + 1.550693864978364947665077e+4F;
            num = ( num*xMinus ) + 1.847932904445632425417223e+5F;
            num = ( num*xMinus ) + 1.088204769468828767498470e+6F;
            num = ( num*xMinus ) + 3.338152967987029735917223e+6F;
            num = ( num*xMinus ) + 5.106661678927352456275255e+6F;
            num = ( num*xMinus ) + 3.074109054850539556250927e+6F;
            den = ( den*xMinus ) + 1.830328399370592604055942e+2F;
            den = ( den*xMinus ) + 7.765049321445005871323047e+3F;
            den = ( den*xMinus ) + 1.331903827966074194402448e+5F;
            den = ( den*xMinus ) + 1.136705821321969608938755e+6F;
            den = ( den*xMinus ) + 5.267964117437946917577538e+6F;
            den = ( den*xMinus ) + 1.346701454311101692290052e+7F;
            den = ( den*xMinus ) + 1.782736530353274213975932e+7F;
            den = ( den*xMinus ) + 9.533095591844353613395747e+6F;
            result = xMinus*( d2 + ( xMinus*( num/den ) ) );
        }
        else if ( y <= 12.0F ) {
            const float xMinus = y - 4.0F;
            den = -1.0F;
            num = 0.0F;
            num = ( num*xMinus ) + 1.474502166059939948905062e+04F;
            num = ( num*xMinus ) + 2.426813369486704502836312e+06F;
            num = ( num*xMinus ) + 1.214755574045093227939592e+08F;
            num = ( num*xMinus ) + 2.663432449630976949898078e+09F;
            num = ( num*xMinus ) + 2.940378956634553899906876e+10F;
            num = ( num*xMinus ) + 1.702665737765398868392998e+11F;
            num = ( num*xMinus ) + 4.926125793377430887588120e+11F;
            num = ( num*xMinus ) + 5.606251856223951465078242e+11F;
            den = ( den*xMinus ) + 2.690530175870899333379843e+03F;
            den = ( den*xMinus ) + 6.393885654300092398984238e+05F;
            den = ( den*xMinus ) + 4.135599930241388052042842e+07F;
            den = ( den*xMinus ) + 1.120872109616147941376570e+09F;
            den = ( den*xMinus ) + 1.488613728678813811542398e+10F;
            den = ( den*xMinus ) + 1.016803586272438228077304e+11F;
            den = ( den*xMinus ) + 3.417476345507377132798597e+11F;
            den = ( den*xMinus ) + 4.463158187419713286462081e+11F;
            result = d4 + ( xMinus*( num/den ) );
        }
        else {
            result = 0.0F;
            if ( y <= 4294967296.87842273712158203125F ) { /* y < xBig^(1/4)*/
                const float yy = y*y;
                result = 5.7083835261e-03F;
                result = ( result/yy ) - 1.910444077728e-03F;
                result = ( result/yy ) + 8.4171387781295e-04F;
                result = ( result/yy ) - 5.952379913043012e-04F;
                result = ( result/yy ) + 7.93650793500350248e-04F;
                result = ( result/yy ) - 2.777777777777681622553e-03F;
                result = ( result/yy ) + 8.333333333333333331554247e-02F;
            }
            result /= y;
            corrector = qFFMath_Log( y );
            result += QFFM_LN_SQRT_2PI - ( 0.5F*corrector );
            result += y*( corrector - 1.0F );
        }
    }

    return result;
}
/*============================================================================*/
float qFFMath_LGamma( float x )
{
    float result;

    const int fClass = qFFMath_FPClassify( x );
    if ( QFFM_FP_NAN == fClass ) {
        result = QFFM_NAN;
    }
    else if ( ( QFFM_FP_ZERO == fClass ) || ( QFFM_FP_INFINITE == fClass ) ) {
        result = QFFM_INFINITY;
    }
    else {
        if ( x < 0.0F ) {
            if ( x <= -4503599627370496.0F ) { /* x < 2^52 */
                result = QFFM_INFINITY;
            }
            else {
                float y, y1, isItAnInt;

                y = -x;
                y1 = qFFMath_Trunc( y );
                isItAnInt = y - y1;
                if ( qFFMath_IsEqual( 0.0F, isItAnInt ) ) {
                    result = QFFM_INFINITY;
                }
                else {
                    float a;

                    a = qFFMath_Sin( QFFM_PI*isItAnInt );
                    result = qFFMath_Log( QFFM_PI/qFFMath_Abs( a*x ) ) - lgamma_positive( -x );
                }
            }
        }
        else {
            result = lgamma_positive( x );
        }
    }

    return result;
}
/*============================================================================*/
float qFFMath_Factorial( float x )
{
    static const float ft[ 35 ] = { 1.0F, 1.0F, 2.0F, 6.0F, 24.0F, 120.0F, 720.0F,
                                    5040.0F, 40320.0F, 362880.0F, 3628800.0F,
                                    39916800.0F, 479001600.0F, 6227020800.0F,
                                    87178291200.0F, 1307674368000.0F,
                                    20922789888000.0F, 355687428096000.0F,
                                    6402373705728001.0F, 121645100408832000.0F,
                                    2432902008176640000.0F, 51090942171709440000.0F,
                                    1124000727777607680000.0F,
                                    25852016738884978212864.0F,
                                    620448401733239544217600.0F,
                                    15511210043330988202786816.0F,
                                    403291461126605719042260992.0F,
                                    10888869450418351940239884288.0F,
                                    304888344611713836734530715648.0F,
                                    8841761993739700772720181510144.0F,
                                    265252859812191104246398737973248.0F,
                                    8222838654177921277277005322125312.0F,
                                    263130836933693591553328612565319680.0F,
                                    8683317618811885938715673895318323200.0F,
                                    295232799039604119555149671006000381952.0F,
                                 };
    float y;

    if ( x > 34.0F ) {
        y = QFFM_INFINITY;
    }
    else if ( x >= 0.0F ) {
        y = ft[ (size_t)x ];
    }
    else {
        y = QFFM_NAN;
    }

    return y;
}
#endif /*#ifndef QLIBS_USE_STD_MATH*/