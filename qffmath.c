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

static float poly_laguerre_recursion( size_t n,
                                      float alpha,
                                      float x );
static float poly_laguerre_large_n( size_t n,
                                    float alpha,
                                    float x );
static float poly_laguerre_hyperg( size_t n,
                                   float alpha,
                                   float x );
static float poly_legendre_p( size_t l,
                              float x );
static float ellint_rf( float x,
                        float y,
                        float z );
static float ellint_rd( float x,
                        float y,
                        float z );
static float ellint_rc( float x,
                        float y );
static float ellint_rj( float x,
                        float y,
                        float z,
                        float p );
static float expint_E1_series( float x );
static float expint_E1_asymp( float x );
static float expint_Ei_asymp( float x );
static float expint_E1( float x );
static float expint_En_cont_frac( size_t n,
                                  float x );
static float expint_Ei_series( float x );
static float expint_Ei( float x );
static float riemann_zeta_glob( float s );
static float riemann_zeta_product( float s );
static void gamma_temme( float mu,
                         float* gam1,
                         float* gam2,
                         float* gam_pl,
                         float* gam_mi);
static void bessel_jn( float nu,
                       float x,
                       float* j_nu,
                       float* n_nu,
                       float* j_pnu,
                       float* n_pnu );
static void sph_bessel_jn( size_t n,
                           float x,
                           float* j_n,
                           float* n_n,
                           float* jp_n,
                           float* np_n );
static void bessel_ik( float nu,
                       float x,
                       float* i_nu,
                       float* k_nu,
                       float* i_pnu,
                       float* k_pnu );
static float cyl_bessel_ij_series( float nu,
                                   float x,
                                   float sgn,
                                   size_t max_iter );

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
    int32_t i0 = 0, j0;
    float ret = x;

    cast_reinterpret( i0, x, int32_t );
    /*cstat -MISRAC2012-Rule-10.1_R7 -MISRAC2012-Rule-10.3 -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.4_a*/
    j0 = ( ( i0 >> 23 ) & 0xFF ) - 0x7F;
    if ( j0 < 23 ) {
        if ( j0 < 0 ) {
            i0 &= (int32_t)0x80000000;
            if ( -1 == j0 ) {
                i0 |= 0x3F800000;
            }
            cast_reinterpret( ret, i0, float );
        }
        else {
            const int32_t i = 0x007FFFFF >> j0;
            if ( 0 != ( i0 & i ) ) {
                i0 += 0x00400000 >> j0;
                i0 &= ~i;
                cast_reinterpret( ret, i0, float );
            }
        }
    }
    /*cstat +MISRAC2012-Rule-10.1_R7 +MISRAC2012-Rule-10.3 +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.4_a*/
    return ret;
}
/*============================================================================*/
float qFFMath_Floor( float x )
{
    int32_t i0 = 0, j0;
    float ret = x;

    cast_reinterpret( i0, x, int32_t );
    /*cstat -MISRAC2012-Rule-10.1_R7 -MISRAC2012-Rule-10.3 -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.4_a*/
    j0 = ( ( i0 >> 23 ) & 0xFF ) - 0x7F;
    if ( j0 < 23 ) {
        if ( j0 < 0 ) {
            if ( i0 >= 0 ) {
                i0 = 0;
            }
            else if ( 0 != ( i0 & 0x7FFFFFFF ) ) {
                i0 = (int32_t)0xBF800000;
            }
            else {
                /*nothing to do here*/
            }
            cast_reinterpret( ret, i0, float);
        }
        else {
            const int32_t i = ( 0x007FFFFF ) >> j0;
            if ( 0 != ( i0 & i ) ) {
                if ( i0 < 0 ) {
                    i0 += 0x00800000 >> j0;
                }
                i0 &= (~i);
                cast_reinterpret( ret, i0, float);
            }
        }
    }
    /*cstat +MISRAC2012-Rule-10.1_R7 +MISRAC2012-Rule-10.3 +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.4_a*/
    return ret;
}
/*============================================================================*/
float qFFMath_Ceil( float x )
{
    int32_t i0 = 0, j0;
    float ret = x;

    cast_reinterpret( i0, x, int32_t );
    /*cstat -MISRAC2012-Rule-10.1_R7 -MISRAC2012-Rule-10.3 -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.4_a*/
    j0 = ( ( i0 >> 23 ) & 0xFF ) - 0x7F;
    if ( j0 < 23 ) {
        if ( j0 < 0 ) {
            if ( i0 < 0 ) {
                i0 = (int32_t)0x80000000;
            }
            else if ( 0 != i0 ) {
                i0 = (int32_t)0x3F800000;
            }
            else {
                /*nothing to do here*/
            }
            cast_reinterpret( ret, i0, float);
        }
        else {
            const int32_t i = ( 0x007FFFFF ) >> j0;
            if ( 0 != ( i0 & i ) ) {
                if ( i0 > 0 ) {
                    i0 += 0x00800000 >> j0;
                }
                i0 &= (~i);
                cast_reinterpret( ret, i0, float);
            }
        }
    }
    /*cstat +MISRAC2012-Rule-10.1_R7 +MISRAC2012-Rule-10.3 +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.4_a*/
    return ret;
}
/*============================================================================*/
float qFFMath_Trunc( float x )
{
    int32_t i0 = 0, sx, j0;
    float ret = x;

    cast_reinterpret( i0, x, int32_t );
    /*cstat -MISRAC2012-Rule-10.1_R7 -MISRAC2012-Rule-10.3 -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.4_a*/
    sx = i0 & (int32_t)0x80000000;
    j0 = ( ( i0 >> 23 ) & 0xFF ) - 0x7F;
    if ( j0 < 23 ) {
        const int32_t tmp = ( j0 < 0 ) ? sx : ( sx | ( i0 & ~( 0x007FFFFF >> j0 ) ) );
        cast_reinterpret( ret, tmp, float );
    }
    /*cstat +MISRAC2012-Rule-10.1_R7 +MISRAC2012-Rule-10.3 +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.4_a*/
    return ret;
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
    return ( QFFM_FP_ZERO == qFFMath_FPClassify( x ) ) ? QFFM_NAN
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
float qFFMath_WrapToPi( float x )
{
     return qFFMath_Mod( x + QFFM_PI, QFFM_2PI ) - QFFM_PI;
}
/*============================================================================*/
float qFFMath_WrapTo2Pi( float x )
{
    return qFFMath_Mod( x, QFFM_2PI );
}
/*============================================================================*/
float qFFMath_WrapTo180( float x )
{
    return qFFMath_Mod( x + 180.0F, 360.0F ) - 180.0F;
}
/*============================================================================*/
float qFFMath_WrapTo360( float x )
{
    return qFFMath_Mod( x, 360.0F );
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
/*============================================================================*/
static float poly_laguerre_recursion( size_t n,
                                      float alpha,
                                      float x )
{
    const float l0 = 1.0F;
    float y;

    if ( 0U == n ) {
        y = l0;
    }
    else {
        const float l1 = -x + 1.0F + alpha;
        if ( 1U == n ) {
            y = l1;
        }
        else {
            float ln2 = l0;
            float ln1 = l1;
            float ln = 0.0F;
            for ( size_t i = 2U ; i <= n ; ++i ) {
                /*cstat -CERT-FLP36-C*/
                const float nn = (float)i;
                /*cstat +CERT-FLP36-C*/
                ln = ( ( ( ( 2.0F*nn ) - 1.0F ) + alpha - x )*( ln1/nn ) )
                     - ( ( ( nn - 1.0F ) + alpha )*( ln2/nn ) );
                ln2 = ln1;
                ln1 = ln;
            }
            y = ln;
        }
    }

    return y;
}
/*============================================================================*/
static float poly_laguerre_large_n( size_t n,
                                    float alpha,
                                    float x )
{
    const float PI_2_SQ = 2.467401100272339498076235031476244330406188964F;
    /*cstat -CERT-FLP36-C*/
    const float m = (float)n;
    /*cstat +CERT-FLP36-C*/
    const float a = -m;
    const float b = alpha + 1.0F;
    const float eta = ( 2.0F*b ) - ( 4.0F*a );
    const float cos2th = x/eta;
    const float sin2th = 1.0F - cos2th;
    const float th = qFFMath_ACos( qFFMath_Sqrt( cos2th ) );
    const float pre_h = PI_2_SQ*eta*eta*cos2th*sin2th;
    const float lg_b = qFFMath_LGamma( b + m );
    const float ln_fact = qFFMath_LGamma( m + 1.0F );

    const float preTerm1 = 0.5F*( 1.0F - b )*qFFMath_Log( 0.25F*x*eta );
    const float preTerm2 = 0.25F*qFFMath_Log( pre_h );
    const float lnPre = lg_b - ln_fact + ( 0.5F*x ) + preTerm1 - preTerm2;
    const float serTerm1 = qFFMath_Sin( QFFM_PI*a );
    const float th2 = 2.0F*th;
    const float serTerm2 = qFFMath_Sin( ( ( 0.25F*eta*( ( th2 ) - qFFMath_Sin( th2 ) ) ) + QFFM_PI_4 ) );

    return qFFMath_Exp( lnPre )*( serTerm1 + serTerm2 );
}
/*============================================================================*/
static float poly_laguerre_hyperg( size_t n,
                                   float alpha,
                                   float x )
{
    const float b = alpha + 1.0F;
    const float mx = -x;
    const float tc_sgn = ( x < 0.0F ) ? 1.0F : ( ( 1U == ( n % 2U ) ) ? -1.0F : 1.0F );
    const float ax = qFFMath_Abs( x );
    float tc = 1.0F;

    for ( size_t i = 1U ; i <= n ; ++i ) {
        /*cstat -CERT-FLP36-C*/
        const float k = (float)i;
        /*cstat +CERT-FLP36-C*/
        tc *= ax/k;
    }

    float term = tc*tc_sgn;
    float sum = term;
    const int N = (int)n;
    for ( int i = ( N - 1 ) ; i >= 0; --i ) {
        /*cstat -CERT-FLP36-C -MISRAC++2008-5-0-7*/
        const float k = (float)i;
        term *= ( b + k )/((float)( N - i ))*( k + 1.0F )/mx;
        /*cstat +CERT-FLP36-C +MISRAC++2008-5-0-7*/
        sum += term;
    }

    return sum;
}
/*============================================================================*/
float qFFMath_Assoc_laguerre( size_t n,
                              size_t m,
                              float x )
{
    float y;
    /*cstat -CERT-FLP36-C*/
    const float alpha = (float)m;
    const float N = (float)n;
    /*cstat +CERT-FLP36-C -MISRAC2012-Rule-13.5*/
    if ( ( x < 0.0F ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( 0U == n ) {
        y = 1.0F;
    }
    else if ( 1U == n ) {
        y = 1.0F + alpha - x;
    }
    else if ( qFFMath_IsEqual( 0.0F, x ) ) {
        float prod = alpha + 1.0F;
        for ( size_t i = 2U ; i < n ; ++i ) {
            /*cstat -CERT-FLP36-C*/
            const float k = (float)i;
            /*cstat +CERT-FLP36-C*/
            prod *= ( alpha + k )/k;
        }
        y = prod;
    }
    else if ( ( alpha > -1.0F ) && ( x < ( ( 2.0F*( alpha + 1.0F ) ) + ( 4.0F*N ) ) ) ) {
        y = poly_laguerre_large_n( n, alpha, x );
    }
    else if ( ( ( x > 0.0F ) && ( alpha < -( N + 1.0F ) ) ) ) {
        y = poly_laguerre_recursion( n, alpha, x );
    }
    else {
        y = poly_laguerre_hyperg( n, alpha, x );
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
static float poly_legendre_p( size_t l,
                              float x )
{
    float y;

    if ( qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( -1.0F, x ) ) {
        y = ( 1U == ( l % 2U ) ) ? -1.0F : 1.0F;
    }
    else {
        float p_lm2 = 1.0F;

        if ( 0U == l ) {
            y = p_lm2;
        }
        else {
            float p_lm1 = x;

            if ( 1U == l ) {
                y = p_lm1;
            }
            else {
                float p_l = 0.0F;

                for ( size_t i = 2U ; i <= l ; ++i ) {
                    /*cstat -CERT-FLP36-C*/
                    const float ll = (float)i;
                    /*cstat +CERT-FLP36-C*/
                    p_l = ( 2.0F*x* p_lm1 ) - p_lm2 - ( ( ( x*p_lm1 ) - p_lm2)/ll );
                    p_lm2 = p_lm1;
                    p_lm1 = p_l;
                }
                y = p_l;
            }
        }
    }

    return y;
}
/*============================================================================*/
float qFFMath_Assoc_legendre( size_t n,
                              size_t m,
                              float x )
{
    float y;
    const float phase = 1.0F;

    if ( m > n ) {
        y = 0.0F;
    }
    else if ( qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( 0U == m ) {
        y = poly_legendre_p( n, x );
    }
    else {
        float p_mm = 1.0F;
        const float root = qFFMath_Sqrt( 1.0F - x )*qFFMath_Sqrt( 1.0F + x );
        float fact = 1.0F;

        for ( size_t i = 1U ; i <= m ; ++i ) {
            p_mm *= phase*fact*root;
            fact += 2.0F;
        }

        if ( n == m ) {
            y = p_mm;
        }
        else {
            /*cstat -CERT-FLP36-C*/
            const float p_mp1m = ( ( 2.0F*( (float)m ) ) + 1.0F )*x*p_mm;
            /*cstat +CERT-FLP36-C*/
            if ( n == ( m + 1U ) ) {
                y = p_mp1m;
            }
            else {
                float p_lm2m = p_mm;
                float p_lm1m = p_mp1m;
                float p_lm = 0.0F;
                /*cstat -CERT-FLP36-C*/
                const float M = (float)m;
                for ( size_t i = ( m + 2U ) ; i <= n ; ++i ) {
                    const float j = (float)i;
                    /*cstat +CERT-FLP36-C*/
                    p_lm = ( ( ( 2.0F*j ) - 1.0F )*x*p_lm1m ) - ( ( j + M - 1.0F )*p_lm2m/( j - M ) );
                    p_lm2m = p_lm1m;
                    p_lm1m = p_lm;
                }
                y = p_lm;
            }
        }
    }

    return y;
}
/*============================================================================*/
float qFFMath_Beta( float x,
                    float y )
{
    float result;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( qFFMath_IsNaN( x ) || qFFMath_IsNaN( y ) ) { //no side effects here
        result = QFFM_NAN;
    }
    else {
        const float bet = qFFMath_LGamma( x ) + qFFMath_LGamma( y ) - qFFMath_LGamma( x + y );
        result = qFFMath_Exp( bet );
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return result;
}
/*============================================================================*/
static float ellint_rf( float x,
                        float y,
                        float z )
{
    const float loLim = 5.0F*FLT_MIN;
    float result;


    if ( ( x < 0.0F ) || ( y < 0.0F ) || ( z < 0.0F ) ||
        ( ( x + y ) < loLim ) || ( ( x + z ) < loLim ) || ( ( y + z) < loLim )
       ) {
        result = QFFM_NAN;
    }
    else {
        const float c0 = 1.0F/4.0F;
        const float c1 = 1.0F/24.0F;
        const float c2 = 1.0F/10.0F;
        const float c3 = 3.0F/44.0F;
        const float c4 = 1.0F/14.0F;
        const float errTol = 0.0024607833005759250678823324F;
        const float c13 = 1.0F/3.0F;

        float xn = x;
        float yn = y;
        float zn = z;
        float mu, xnDev, ynDev, znDev;
        const size_t maxIter = 100U;
        float e2, e3, s;

        for ( size_t iter = 0U ; iter < maxIter ; ++iter ) {
            float abs_xnDev, abs_ynDev, abs_znDev, lambda, epsilon;
            float xRoot, yRoot, zRoot;

            mu = ( xn + yn + zn )*c13;
            xnDev = 2.0F - ( ( mu + xn )/mu );
            ynDev = 2.0F - ( ( mu + yn )/mu );
            znDev = 2.0F - ( ( mu + zn )/mu );
            abs_xnDev = qFFMath_Abs( xnDev );
            abs_ynDev = qFFMath_Abs( ynDev );
            abs_znDev = qFFMath_Abs( znDev );
            epsilon = ( abs_xnDev > abs_ynDev ) ? abs_xnDev : abs_ynDev;
            epsilon = ( abs_znDev > epsilon ) ? abs_znDev : epsilon;
            if ( epsilon < errTol ) {
                break;
            }
            xRoot = qFFMath_Sqrt( xn );
            yRoot = qFFMath_Sqrt( yn );
            zRoot = qFFMath_Sqrt( zn );
            lambda = ( xRoot*( yRoot + zRoot ) ) + ( yRoot*zRoot );
            xn = c0*( xn + lambda );
            yn = c0*( yn + lambda );
            zn = c0*( zn + lambda );
        }
        e2 = xnDev*ynDev;
        e3 = ( e2*znDev );
        e2 = e2 - ( znDev*znDev );
        s  = 1.0F + ( ( ( c1*e2 ) - c2 - ( c3*e3 ) )*e2 ) + ( c4*e3 );
        result = s/qFFMath_Sqrt( mu );
    }

    return result;
}
/*============================================================================*/
static float ellint_rd( float x,
                        float y,
                        float z )
{
    const float errTol = 0.0017400365588678507952624663346341549F;
    const float loLim = 4.103335708781587555782386855921935e-26F;
    float result;

    if ( ( x < 0.0F ) || ( y < 0.0F ) || ( ( x + y ) < loLim ) || ( z < loLim ) ) {
        result = QFFM_NAN;
    }
    else {
        const float c0 = 1.0F/4.0F;
        const float c1 = 3.0F/14.0F;
        const float c2 = 1.0F/6.0F;
        const float c3 = 9.0F/22.0F;
        const float c4 = 3.0F/26.0F;
        float xn = x;
        float yn = y;
        float zn = z;
        float sigma = 0.0F;
        float power4 = 1.0F;
        float mu, xnDev, ynDev, znDev;
        float ea, eb, ec, ed, ef, s1, s2;
        const size_t maxIter = 100U;

        for ( size_t iter = 0U ; iter < maxIter ; ++iter ) {
            float abs_xnDev, abs_ynDev, abs_znDev, lambda, epsilon;
            float xRoot, yRoot, zRoot;

            mu = ( xn + yn + ( 3.0F*zn ) )*0.2F;
            xnDev = ( mu - xn )/mu;
            ynDev = ( mu - yn )/mu;
            znDev = ( mu - zn )/mu;
            abs_xnDev = qFFMath_Abs( xnDev );
            abs_ynDev = qFFMath_Abs( ynDev );
            abs_znDev = qFFMath_Abs( znDev );
            epsilon = ( abs_xnDev > abs_ynDev ) ? abs_xnDev : abs_ynDev;
            epsilon = ( abs_znDev > epsilon ) ? abs_znDev : epsilon;
            if ( epsilon < errTol ) {
                break;
            }
            xRoot = qFFMath_Sqrt( xn );
            yRoot = qFFMath_Sqrt( yn );
            zRoot = qFFMath_Sqrt( zn );
            lambda = ( xRoot*( yRoot + zRoot ) ) + ( yRoot*zRoot );
            sigma += power4/( zRoot*( zn + lambda ) );
            power4 *= c0;
            xn = c0*( xn + lambda );
            yn = c0*( yn + lambda );
            zn = c0*( zn + lambda );
        }
        ea = xnDev*ynDev;
        eb = znDev*znDev;
        ec = ea - eb;
        ed = ea - ( 6.0F*eb );
        ef = ed + ec + ec;
        s1 = ed*( -c1 + ( c3*ed/3.0F ) - ( 1.5F*c4*znDev*ef ) );
        s2 = znDev*( ( c2*ef ) + ( znDev*( -( c3*ec ) - ( znDev*c4 ) - ea ) ) );
        result = ( 3.0F*sigma ) + ( power4*qFFMath_RSqrt( mu )*( 1.0F + s1 + s2)/mu );
    }

    return result;
}
/*============================================================================*/
static float ellint_rc( float x,
                        float y )
{
    float result;
    const float loLim = 5.8774717550000002558112628881984982848919e-38F;
    const float errTol = 0.049606282877419791144113503378321F;

    if ( ( x < 0.0F ) || ( y < 0.0F ) || ( y < loLim ) ) {
        result = QFFM_NAN;
    }
    else {
        const float c0 = 1.0F/4.0F;
        const float c1 = 1.0F/7.0F;
        const float c2 = 9.0F/22.0F;
        const float c3 = 3.0F/10.0F;
        const float c4 = 3.0F/8.0F;
        const float c13 = 1.0F/3.0F;
        float xn = x;
        float yn = y;
        const size_t maxIter = 100;
        float mu, s, sn;

        for ( size_t iter = 0U ; iter < maxIter ; ++iter ) {
            float lambda;

            mu = ( xn + ( 2.0F*yn ) )*c13;
            sn = ( ( yn + mu )/mu ) - 2.0F;
            if ( qFFMath_Abs( sn ) < errTol ){
                break;
            }
            lambda = ( 2.0F*qFFMath_Sqrt( xn )*qFFMath_Sqrt( yn ) ) + yn;
            xn = c0*( xn + lambda );
            yn = c0*( yn + lambda );
        }
        s = sn*sn*( c3 + ( sn*( c1 + ( sn*( c4 + ( sn*c2 ) ) ) ) ) );
        result = ( 1.0F + s )*qFFMath_RSqrt( mu );
    }

    return result;
}
/*============================================================================*/
static float ellint_rj( float x,
                        float y,
                        float z,
                        float p )
{
    float result;
    const float loLim = 4.103335708781587555782386855921935e-26F;
    const float errTol = 0.049606282877419791144113503378321F;

    if ( ( x < 0.0F ) || ( y < 0.0F ) || ( z < 0.0F ) || ( ( x + y ) < loLim ) ||
         ( ( x + z ) < loLim ) || ( ( y + z) < loLim )|| ( p < loLim ) ) {
        result = QFFM_NAN;
    }
    else {
        const float c0 = 1.0F/4.0F;
        const float c1 = 3.0F/14.0F;
        const float c2 = 1.0F/3.0F;
        const float c3 = 3.0F/22.0F;
        const float c4 = 3.0F/26.0F;
        float xn = x;
        float yn = y;
        float zn = z;
        float pn = p;
        float sigma = 0.0F;
        float power4 = 1.0F;
        float mu, xnDev, ynDev, znDev, pnDev;
        float ea, eb, ec, e2, e3, s1, s2, s3;
        const size_t maxIter = 100;

        for ( size_t iter = 0U ; iter < maxIter ; ++iter ) {
            float abs_xnDev, abs_ynDev, abs_znDev, abs_pnDev, lambda, alpha1;
            float alpha2, beta, epsilon;
            float xRoot, yRoot, zRoot;
            mu = 0.2F*( xn + yn + zn + ( 2.0F*pn ) );
            xnDev = ( mu - xn )/mu;
            ynDev = ( mu - yn )/mu;
            znDev = ( mu - zn )/mu;
            pnDev = ( mu - pn )/mu;
            abs_xnDev = qFFMath_Abs( xnDev );
            abs_ynDev = qFFMath_Abs( ynDev );
            abs_znDev = qFFMath_Abs( znDev );
            abs_pnDev = qFFMath_Abs( pnDev );
            epsilon = ( abs_xnDev > abs_ynDev ) ? abs_xnDev : abs_ynDev;
            epsilon = ( abs_znDev > epsilon ) ? abs_znDev : epsilon;
            epsilon = ( abs_pnDev > epsilon ) ? abs_pnDev : epsilon;
            if ( epsilon < errTol ) {
                break;
            }
            xRoot = qFFMath_Sqrt( xn );
            yRoot = qFFMath_Sqrt( yn );
            zRoot = qFFMath_Sqrt( zn );
            lambda = ( xRoot*( yRoot + zRoot ) ) + ( yRoot*zRoot );
            alpha1 = ( pn*( xRoot + yRoot + zRoot ) ) + ( xRoot*yRoot*zRoot );
            alpha2 = alpha1*alpha1;
            beta = pn*( pn + lambda )*( pn + lambda );
            sigma += power4*ellint_rc( alpha2, beta );
            power4 *= c0;
            xn = c0*( xn + lambda );
            yn = c0*( yn + lambda );
            zn = c0*( zn + lambda );
            pn = c0*( pn + lambda );
        }
        ea = ( xnDev*( ynDev + znDev ) ) + ( ynDev*znDev );
        eb = xnDev*ynDev*znDev;
        ec = pnDev*pnDev;
        e2 = ea - ( 3.0F*ec );
        e3 = eb + ( 2.0F*pnDev*( ea - ec ) );
        s1 = 1.0F + ( e2*( -c1 + ( 0.75F*c3*e2 ) - ( 1.5F*c4*e3 ) ) );
        s2 = eb*( ( 0.5F*c2 ) + ( pnDev*( -c3 - c3 + ( pnDev*c4 ) ) ) );
        s3 = ( pnDev*ea*( c2 - ( pnDev*c3 ) ) ) - ( c2*pnDev*ec );
        result = ( 3.0F*sigma ) + ( power4*( s1 + s2 + s3)/( mu * qFFMath_Sqrt( mu ) ) );
    }

    return result;
}
/*============================================================================*/
float qFFMath_Comp_ellint_1( float k )
{
    float y;

    if ( qFFMath_IsNaN( k ) || ( qFFMath_Abs( k ) >= 1.0F ) ) {
        y = QFFM_NAN;
    }
    else {
        y = ellint_rf( 0.0F, 1.0F - ( k*k ), 1.0F );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Comp_ellint_2( float k )
{
    float y;
    const float abs_k = qFFMath_Abs( k );

    if ( qFFMath_IsNaN( k ) || ( abs_k > 1.0F ) ) {
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 1.0F, abs_k ) ) {
        y = 1.0F;
    }
    else {
        const float kk = k*k;
        const float one_m_kk = 1.0F - kk;
        const float c13 = 1.0F/3.0F;

        y = ellint_rf( 0.0F, one_m_kk, 1.0F ) - ( c13*kk*ellint_rd( 0.0F, one_m_kk, 1.0F ) );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Comp_ellint_3( float k,
                             float nu )
{
    float y;
    const float abs_k = qFFMath_Abs( k );
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( qFFMath_IsNaN( k ) || qFFMath_IsNaN( nu ) || ( abs_k > 1.0F ) ) { //no side effects here
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 1.0F, nu ) ) {
        y = QFFM_INFINITY;
    }
    else {
        const float kk = k*k;
        const float one_m_kk = 1.0F - kk;
        const float c13 = 1.0F/3.0F;

        y = ellint_rf( 0.0F, one_m_kk, 1.0F ) + ( c13*nu*ellint_rj( 0.0F, one_m_kk, 1.0F, 1.0F - nu ) );
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Ellint_1( float k,
                        float phi )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( qFFMath_IsNaN( k ) || qFFMath_IsNaN( phi ) || ( qFFMath_Abs(k) > 1.0F ) ) { //no side effects here
        y = QFFM_NAN;
    }
    else {
        const float n = qFFMath_Floor( ( phi/QFFM_PI ) + 0.5F );
        const float phi_red = phi - ( n*QFFM_PI );
        const float s = qFFMath_Sin( phi_red );
        const float c = qFFMath_Cos( phi_red );
        const float f = s*ellint_rf( c*c, 1.0F - ( k*k*s*s ), 1.0F );
        if ( QFFM_FP_ZERO == qFFMath_FPClassify( n ) ) {
            y = f;
        }
        else {
            y = f + ( 2.0F*n*qFFMath_Comp_ellint_1( k ) );
        }
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Ellint_2( float k,
                        float phi )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( qFFMath_IsNaN( k ) || qFFMath_IsNaN( phi ) || ( qFFMath_Abs( k ) > 1.0F ) ) { //no side effects here
        y = QFFM_NAN;
    }
    else {
        const float c13 = 1.0F/3.0F;
        const float n = qFFMath_Floor( ( phi/QFFM_PI ) + 0.5F );
        const float phi_red = phi - ( n*QFFM_PI );
        const float kk = k*k;
        const float s = qFFMath_Sin( phi_red );
        const float ss = s*s;
        const float sss = ss*s;
        const float c = qFFMath_Cos( phi_red );
        const float cc = c*c;
        const float tmp = 1.0F - ( kk*ss );
        const float e = ( s*ellint_rf( cc, tmp, 1.0F ) ) -
                        ( c13*kk*sss*ellint_rd( cc, tmp, 1.0F ) );

        if ( QFFM_FP_ZERO == qFFMath_FPClassify( n ) ) {
            y = e;
        }
        else {
            y = e + ( 2.0F*n*qFFMath_Comp_ellint_2( k ) );
        }
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Ellint_3( float k,
                        float nu,
                        float phi )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( qFFMath_IsNaN( k ) || qFFMath_IsNaN( nu ) || qFFMath_IsNaN( phi ) || ( qFFMath_Abs( k ) > 1.0F ) ) {
        y = QFFM_NAN;
    }
    else {
        const float n = qFFMath_Floor( ( phi/QFFM_PI ) + 0.5F );
        const float phi_red = phi - ( n*QFFM_PI );
        const float kk = k*k;
        const float s = qFFMath_Sin( phi_red );
        const float ss = s*s;
        const float sss = ss*s;
        const float c = qFFMath_Cos( phi_red );
        const float cc = c*c;
        const float tmp = 1.0F - ( kk*ss );
        const float c13 = 1.0F/3.0F;
        const float pi = ( s*ellint_rf( cc, tmp, 1.0F ) ) +
                         ( c13*nu*sss*ellint_rj( cc, tmp, 1.0F, 1.0F - ( nu*ss ) ) ) ;

        if ( QFFM_FP_ZERO == qFFMath_FPClassify( n ) ) {
            y = pi;
        }
        else {
            y = pi + ( 2.0F*n*qFFMath_Comp_ellint_3( k, nu ) );
        }
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
static float expint_E1_series( float x )
{
    float term = 1.0F;
    float eSum = 0.0F;
    float oSum = 0.0F;
    const size_t maxIter = 1000U;

    for ( size_t i = 1U; i < maxIter; ++i ) {
        /*cstat -CERT-FLP36-C*/
        const float j = (float)i;
        /*cstat +CERT-FLP36-C*/
        term *= -x/j;
        if ( qFFMath_Abs( term ) < FLT_EPSILON ) {
            break;
        }
        if ( term >= 0.0F ) {
            eSum += term/j;
        }
        else {
            oSum += term/j;
        }
    }
    return - eSum - oSum - QFFM_GAMMA_E - qFFMath_Log( x );
}
/*============================================================================*/
static float expint_E1_asymp( float x )
{
    float term = 1.0F;
    float eSum = 1.0F;
    float oSum = 0.0F;
    const size_t maxIter = 1000U;

    for ( size_t i = 1U; i < maxIter; ++i ) {
        const float prev = term;
        /*cstat -CERT-FLP36-C*/
        term *= -( (float)i )/x;
        /*cstat +CERT-FLP36-C*/
        if ( qFFMath_Abs( term ) > qFFMath_Abs( prev ) ) {
            break;
        }
        if ( term >= 0.0F ) {
            eSum += term;
        }
        else {
            oSum += term;
        }
    }
    return qFFMath_Exp( -x )*( eSum + oSum )/x;
}
/*============================================================================*/
static float expint_Ei_asymp( float x )
{
    float term = 1.0F;
    float sum = 1.0F;
    const size_t maxIter = 1000U;

    for ( size_t i = 1U; i < maxIter; ++i ) {
        const float prev = term;
        /*cstat -CERT-FLP36-C*/
        term *= -( (float)i )/x;
        /*cstat +CERT-FLP36-C*/
        if ( ( term < FLT_EPSILON ) || ( term >= prev ) ) {
            break;
        }
        sum += term;
    }

    return qFFMath_Exp( x )*sum/x;
}
/*============================================================================*/
static float expint_E1( float x )
{
    float y;

    if ( x < 0.0F ) {
        y = -expint_Ei( -x );
    }
    else if ( x < 1.0F ) {
        y = expint_E1_series( x );
    }
    else if ( x < 100.F ) {
        y = expint_En_cont_frac( 1, x );
    }
    else {
        y = expint_E1_asymp( x );
    }

    return y;
}
/*============================================================================*/
static float expint_En_cont_frac( size_t n,
                                  float x )
{
    float y = QFFM_NAN;
    const int maxIter = 1000;
    const int nm1 = (int)n - 1;
    /*cstat -CERT-FLP36-C*/
    float b = x + (float)n;
    /*cstat +CERT-FLP36-C*/
    float c = 1.0F/FLT_MIN;
    float d = 1.0F/b;
    float h = d;

    for ( int i = 1; i <= maxIter; ++i ) {
        /*cstat -MISRAC++2008-5-0-7 -CERT-FLP36-C*/
        const float a = -( (float)( i*( nm1 + i ) ) );
        /*cstat MISRAC++2008-5-0-7 +CERT-FLP36-C*/
        b += 2.0F;
        d = 1.0F/( ( a*d ) + b );
        c = b + ( a/c );
        const float del = c*d;
        h *= del;
        if ( qFFMath_Abs( del - 1.0F ) < FLT_EPSILON ) {
            y = h*qFFMath_Exp( -x );
            break;
        }
    }

    return y;
}
/*============================================================================*/
static float expint_Ei_series( float x )
{
    float term = 1.0F;
    float sum = 0.0F;
    const size_t maxIter = 1000U;

    for ( size_t i = 1U; i < maxIter; ++i ) {
        /*cstat -CERT-FLP36-C*/
        const float j = (float)i;
        /*cstat +CERT-FLP36-C*/
        term *= x/j;
        sum += term/j;
        if ( term < ( FLT_EPSILON*sum ) ) {
            break;
        }
      }
    return QFFM_GAMMA_E + sum + qFFMath_Log( x );
}
/*============================================================================*/
static float expint_Ei( float x )
{
    const float logEps = 36.044F;
    float y;

    if ( x < 0.0F ) {
        y = -expint_E1( -x );
    }
    else if ( x < logEps ) {
        y = expint_Ei_series( x );
    }
    else {
        y = expint_Ei_asymp( x );
    }

    return y;
}
/*============================================================================*/
float qFFMath_Expint( float num )
{
    return ( qFFMath_IsNaN( num ) ) ? QFFM_NAN : expint_Ei( num );
}
/*============================================================================*/
float qFFMath_Hermite( size_t n,
                       float x )
{
    float y = 0.0F;

    if ( qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else {
        const float H_0 = 1.0F;

        if ( 0U == n ) {
            y= H_0;
        }
        else {
            const float H_1 = 2.0F*x;
            if ( 1U == n ) {
                y = H_1;
            }
            else {
                float H_nm1, H_nm2;

                H_nm2 = H_0;
                H_nm1 = H_1;
                for ( size_t i = 2U; i <= n; ++i ) {
                    /*cstat -CERT-FLP36-C*/
                    const float j = (float)( i - 1U );
                    /*cstat +CERT-FLP36-C*/
                    y = 2.0F*( ( x*H_nm1 ) - ( j*H_nm2 ) );
                    H_nm2 = H_nm1;
                    H_nm1 = y;
                }
            }
        }
    }

    return y;
}
/*============================================================================*/
float qFFMath_Laguerre( size_t n,
                        float x )
{
    return qFFMath_Assoc_laguerre( n, 0, x );
}
/*============================================================================*/
float qFFMath_Legendre( size_t n,
                        float x )
{
    float y = 0.0F;

    if ( qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 1.0F, x ) ) {
        y = 1.0F;
    }
    else if ( qFFMath_IsEqual( -1.0F, x ) ) {
        y =  ( 1U == ( n % 2U ) ) ? -1.0F : 1.0F;
    }
    else {
        float p_lm2 = 1.0F;

        if ( 0U == n ) {
            y = p_lm2;
        }
        else {
            float p_lm1 = x;

            if ( 1U == n ) {
                y = p_lm1;
            }
            else {
                for ( size_t ll = 2U ; ll <= n; ++ll ) {
                    /*cstat -CERT-FLP36-C*/
                    const float ll_f = (float)ll;
                    /*cstat +CERT-FLP36-C*/
                    y = ( 2.0F*x*p_lm1 ) - p_lm2 - ( ( ( x*p_lm1 ) - p_lm2)/ll_f );
                    p_lm2 = p_lm1;
                    p_lm1 = y;
                }
            }
        }
    }

    return y;
}
/*============================================================================*/
static float riemann_zeta_glob( float s )
{
    const float maxBinCoeff = 86.4982335337F;
    float zeta = 0.0F;
    const float ss = s;
    bool neg = false;

    if ( s < 0.0F ) {
        s = 1.0F - s;
        neg = true;
    }
    float num = 0.5F;
    const size_t maxIt = 10000U;
    /*cstat -MISRAC++2008-6-6-4*/
    for ( size_t i = 0U ; i < maxIt; ++i ) {
        bool punt = false;
        float sgn = 1.0F;
        float term = 0.0F;
        for ( size_t j = 0U ; j <= i ; ++j ) {
            /*cstat -CERT-FLP36-C*/
            const float ii = (float)i;
            const float jj = (float)j;
            /*cstat +CERT-FLP36-C*/
            float bin_coeff = qFFMath_LGamma( 1.0F + ii ) -
                              qFFMath_LGamma( 1.0F + jj ) -
                              qFFMath_LGamma( 1.0F + ii - jj );

            if ( bin_coeff > maxBinCoeff ) {
                punt = true;
                break;
            }
            bin_coeff = qFFMath_Exp( bin_coeff );
            term += sgn*bin_coeff*qFFMath_Pow( 1.0F + jj, -s );
            sgn *= -1.0F;
        }
        if ( punt ) {
            break;
        }
        term *= num;
        zeta += term;
        if ( qFFMath_Abs( term/zeta ) < FLT_EPSILON ) {
            break;
        }
        num *= 0.5F;
    }
    /*cstat +MISRAC++2008-6-6-4*/
    zeta /= 1.0F - qFFMath_Pow( 2.0F, 1.0F - s );

    if ( neg ) {
        zeta *= qFFMath_Pow( 2.0F*QFFM_PI, ss )*
                qFFMath_Sin( QFFM_PI_2*ss )*
                qFFMath_Exp( qFFMath_LGamma( s ) )/QFFM_PI;
    }
    return zeta;
}
/*============================================================================*/
static float riemann_zeta_product( float s )
{
    static const uint8_t prime[ 29 ] = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67,
        71, 73, 79, 83, 89, 97, 101, 103, 107, 109
    };
    float zeta = 1.0F;

    for ( size_t i = 0U ; i < sizeof(prime) ; ++i ) {
        const float f = 1.0F - qFFMath_Pow( (float)prime[ i ], -s );

        zeta *= f;
        if ( ( 1.0F - f ) < FLT_EPSILON ) {
            break;
        }
    }
    zeta = 1.0F/zeta;

    return zeta;
}
/*============================================================================*/
float qFFMath_Riemann_zeta( float s )
{
    float z;

    if ( qFFMath_IsNaN( s ) ) {
        z = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 1.0F, s ) ) {
        z = QFFM_INFINITY;
    }
    else if ( s < -19.0F ) {
        z = riemann_zeta_product( 1.0F - s );
        z *= qFFMath_Pow( 2.0F*QFFM_PI, s )*
             qFFMath_Sin( QFFM_PI_2*s )*
             qFFMath_Exp( qFFMath_LGamma( 1.0F - s ) )/
             QFFM_PI;
    }
    else if ( s < 20.0F ) {
        z = riemann_zeta_glob( s );
    }
    else {
        z = riemann_zeta_product( s );
    }

    return z;
}
/*============================================================================*/
static void gamma_temme( float mu,
                         float* gam1,
                         float* gam2,
                         float* gam_pl,
                         float* gam_mi)
{

    gam_pl[ 0 ] = 1.0F/qFFMath_TGamma( 1.0F + mu );
    gam_mi[ 0 ] = 1.0F/qFFMath_TGamma( 1.0F - mu );

    if ( qFFMath_Abs( mu ) < FLT_EPSILON ) {
        gam1[ 0 ] = -QFFM_GAMMA_E;
    }
    else {
        gam1[ 0 ] = ( gam_mi[ 0 ] - gam_pl[ 0 ] )/( 2.0F*mu );
    }
    gam2[ 0 ] = 0.5F*( gam_mi[ 0 ] + gam_pl[ 0 ] );
}
/*============================================================================*/
static void bessel_jn( float nu,
                       float x,
                       float* j_nu,
                       float* n_nu,
                       float* j_pnu,
                       float* n_pnu )
{
    if ( qFFMath_IsEqual( 0.0F, x ) ) {
        if ( qFFMath_IsEqual( 0.0F, nu ) ) {
            j_nu[ 0 ] = 1.0F;
            j_pnu[ 0 ] = 0.0F;
        }
        else if ( qFFMath_IsEqual( 1.0F, nu )  ) {
            j_nu[ 0 ] = 0.0F;
            j_pnu[ 0 ] = 0.5F;
        }
        else {
            j_nu[ 0 ] = 0.0F;
            j_pnu[ 0 ] = 0.0F;
        }
        n_nu[ 0 ] = -QFFM_INFINITY;
        n_pnu[ 0 ] = QFFM_INFINITY;
    }
    else {
        const float eps = FLT_EPSILON;
        const float fp_min = 1.08420217256745973463717809E-19F;
        const int max_iter = 15000;
        const float x_min = 2.0F;
        /*cstat -CERT-FLP34-C*/
        const int tmp_nl = (int)( nu - x + 1.5F );
        const int nl = ( x < x_min ) ? (int)( nu + 0.5F )
                                     : ( ( tmp_nl > 0 )? tmp_nl : 0 );
        /*cstat +CERT-FLP34-C -CERT-FLP36-C*/
        const float mu = nu - (float)nl;
        /*cstat +CERT-FLP36-C*/
        const float mu2 = mu*mu;
        const float xi = 1.0F/x;
        const float xi2 = 2.0F*xi;
        const float w = xi2/QFFM_PI;
        float iSign = 1.0F;
        float h = nu*xi;
        if ( h < fp_min ) {
            h = fp_min;
        }
        float b = xi2*nu;
        float d = 0.0F;
        float c = h;
        for ( int i = 1; i <= max_iter; ++i ) {
            b += xi2;
            d = b - d;
            if ( qFFMath_Abs( d ) < fp_min ) {
                d = fp_min;
            }
            c = b - ( 1.0F/c );
            if ( qFFMath_Abs( c ) < fp_min ) {
                c = fp_min;
            }
            d = 1.0F / d;

            const float del = c * d;
            h *= del;
            if ( d < 0.0F ) {
                iSign = -iSign;
            }
            if ( qFFMath_Abs( del - 1.0F ) < eps ) {
                break;
            }
        }
        float j_nul = iSign*fp_min;
        float j_pnu_l = h*j_nul;
        const float j_nul1 = j_nul;
        const float j_pnu1 = j_pnu_l;
        float fact = nu*xi;

        for ( int l = nl; l >= 1; --l ) {
            const float j_nu_temp = ( fact*j_nul ) + j_pnu_l;

            fact -= xi;
            j_pnu_l = ( fact*j_nu_temp ) - j_nul;
            j_nul = j_nu_temp;
        }
        if ( qFFMath_IsEqual( 0.0F, j_nul ) ) {
            j_nul = eps;
        }
        const float f = j_pnu_l/j_nul;
        float n_mu, n_nu1, n_pmu, Jmu;
        if ( x < x_min ) {
            const float x2 = 0.5F*x;
            const float pi_mu = QFFM_PI*mu;
            const float fact_l = ( qFFMath_Abs( pi_mu ) < eps ) ? 1.0F : ( pi_mu/qFFMath_Sin( pi_mu ) );
            d = -qFFMath_Log( x2 );
            float e = mu*d;
            const float fact2 = ( qFFMath_Abs( e ) < eps ) ? 1.0F : ( qFFMath_Sinh(e)/e );
            float gam1 = 0.0F, gam2 = 0.0F, gam_pl = 0.0F, gam_mi = 0.0F;

            gamma_temme( mu, &gam1, &gam2, &gam_pl, &gam_mi );
            float ff = ( 2.0F/QFFM_PI )*fact_l*( ( gam1*qFFMath_Cosh( e ) ) + ( gam2*fact2*d ) );
            e = qFFMath_Exp( e );
            float p = e/( QFFM_PI*gam_pl );
            float q = 1.0F/( e*QFFM_PI*gam_mi );
            const float pi_mu2 = pi_mu / 2.0F;
            const float fact3 = ( qFFMath_Abs( pi_mu2 ) < eps ) ? 1.0F : ( qFFMath_Sin( pi_mu2 )/pi_mu2 );
            const float r = QFFM_PI*pi_mu2*fact3*fact3;
            float sum = ff + ( r*q );
            float sum1 = p;
            c = 1.0F;
            d = -x2*x2;
            for ( int i = 1; i <= max_iter ; ++i ) {
                /*cstat -CERT-FLP36-C*/
                const float j = (float)i;
                /*cstat +CERT-FLP36-C*/
                ff = ( ( j*ff ) + p + q )/( ( j*j ) - mu2 );
                c *= d/j;
                p /= j - mu;
                q /= j + mu;
                const float del = c*( ff + ( r*q ) );
                sum += del;
                const float del1 = ( c*p ) - ( j*del );
                sum1 += del1;
                if ( qFFMath_Abs( del ) < ( eps*( 1.0F + qFFMath_Abs( sum ) ) ) ) {
                    break;
                }
            }

            n_mu = -sum;
            n_nu1 = -sum1*xi2;
            n_pmu = ( mu*xi*n_mu ) - n_nu1;
            Jmu = w/( n_pmu - ( f*n_mu ) );
        }
        else {
            float a = 0.25F - mu2;
            float q = 1.0F;
            float p = -xi*0.5F;
            const float br = 2.0F*x;
            float bi = 2.0F;
            float fact_g = a*xi/( ( p*p ) + ( q*q ) );
            float cr = br + ( q*fact_g );
            float ci = bi + ( p*fact_g );
            float den = ( br*br ) + ( bi*bi );
            float dr = br/den;
            float di = -bi/den;
            float dlr = ( cr*dr ) - ( ci*di );
            float dli = ( cr*di ) + ( ci*dr );
            float temp = ( p*dlr ) - ( q*dli );
            q = ( p*dli ) + ( q * dlr );
            p = temp;

            for ( int i = 2; i <= max_iter; ++i ) {
                a += (float)( 2*( i - 1 ) );
                bi += 2.0F;
                dr = ( a*dr ) + br;
                di = ( a*di ) + bi;
                if ( ( qFFMath_Abs( dr ) + qFFMath_Abs( di ) ) < fp_min ) {
                    dr = fp_min;
                }
                fact_g = a/( ( cr*cr ) + ( ci*ci ) );
                cr = br + ( cr*fact_g );
                ci = bi - ( ci*fact_g );
                if ( ( qFFMath_Abs( cr ) + qFFMath_Abs( ci ) ) < fp_min ) {
                    cr = fp_min;
                }
                den = ( dr*dr ) + ( di*di );
                dr /= den;
                di /= -den;
                dlr = ( cr*dr ) - ( ci*di );
                dli = ( cr*di ) + ( ci*dr );
                temp = ( p*dlr ) - ( q*dli );
                q = ( p*dli ) + ( q*dlr );
                p = temp;
                if ( ( qFFMath_Abs( dlr - 1.0F ) + qFFMath_Abs( dli ) ) < eps ) {
                    break;
                }
            }
            const float gam = ( p - f )/q;

            Jmu = qFFMath_Sqrt( w/( ( ( p - f )*gam ) + q ) );
            if ( ( Jmu*j_nul ) < 0.0F) {
                Jmu = -Jmu;
            }
            n_mu = gam*Jmu;
            n_pmu = ( p + ( q/gam ) )*n_mu;
            n_nu1 = ( mu*xi*n_mu ) - n_pmu;
        }
        fact = Jmu/j_nul;
        j_nu[ 0 ] = fact*j_nul1;
        j_pnu[ 0 ] = fact*j_pnu1;
        for ( int i = 1; i <= nl; ++i ) {
            /*cstat -CERT-FLP36-C*/
            const float n_nu_temp = ( ( mu + (float)i )*xi2*n_nu1 ) - n_mu;
            /*cstat +CERT-FLP36-C*/
            n_mu = n_nu1;
            n_nu1 = n_nu_temp;
        }
        n_nu[ 0 ] = n_mu;
        n_pnu[ 0 ] = ( nu*xi*n_mu ) - n_nu1;
    }
}
/*============================================================================*/
static void sph_bessel_jn( size_t n,
                           float x,
                           float* j_n,
                           float* n_n,
                           float* jp_n,
                           float* np_n )
{
    /*cstat -CERT-FLP36-C*/
    const float nu = (float)n + 0.5F;
    /*cstat +CERT-FLP36-C*/
    const float sqrtpi2 = 1.25331413731550012080617761967005208134651184F;
    float j_nu = 0.0F, n_nu = 0.0F, jp_nu = 0.0F, np_nu = 0.0F;
    const float factor = sqrtpi2*qFFMath_RSqrt( x );
    const float inv_2x = 1.0F/( 2.0F*x );

    bessel_jn( nu, x, &j_nu, &n_nu, &jp_nu, &np_nu );
    j_n[ 0 ] = factor*j_nu;
    n_n[ 0 ] = factor*n_nu;
    jp_n[ 0 ] = ( factor*jp_nu ) - ( j_n[ 0 ]*inv_2x );
    np_n[ 0 ] = ( factor*np_nu ) - ( n_n[ 0 ]*inv_2x );
}
/*============================================================================*/
float qFFMath_Sph_bessel( size_t n,
                          float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( x < 0.0F ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 0.0F, x ) ) {
        y = ( 0U == n ) ? 1.0F : 0.0F;
    }
    else {
        float j_n = 0.0F, n_n = 0.0F, jp_n = 0.0F, np_n = 0.0F;

        sph_bessel_jn( n, x, &j_n, &n_n, &jp_n, &np_n );
        y = j_n;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Sph_neumann( size_t n,
                           float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( x < 0.0F ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( qFFMath_IsEqual( 0.0F, x ) ) {
        y = -QFFM_INFINITY;
    }
    else {
        float j_n = 0.0F, n_n = 0.0F, jp_n = 0.0F, np_n = 0.0F;

        sph_bessel_jn( n, x, &j_n, &n_n, &jp_n, &np_n );
        y = n_n;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
static void bessel_ik( float nu,
                       float x,
                       float* i_nu,
                       float* k_nu,
                       float* i_pnu,
                       float* k_pnu )
{
    if ( qFFMath_IsEqual( 0.0F, x ) ) {
        if ( qFFMath_IsEqual( 0.0F, nu ) ) {
            i_nu[ 0 ] = 1.0F;
            i_pnu[ 0 ] = 0.0F;
        }
        else if ( qFFMath_IsEqual( 1.0F, x ) ) {
            i_nu[ 0 ] = 0.0F;
            i_pnu[ 0 ] = 0.5F;
        }
        else {
            i_nu[ 0 ] = 0.0F;
            i_pnu[ 0 ] = 0.0F;
        }
        k_nu[ 0 ] = QFFM_INFINITY;
        k_pnu[ 0 ] = -QFFM_INFINITY;
    }
    else {
        const float eps = FLT_EPSILON;
        const float fp_min = 10.0F*FLT_EPSILON;
        const int max_iter = 15000;
        const float x_min = 2.0F;
        /*cstat -CERT-FLP34-C -CERT-FLP36-C*/
        const int nl = (int)( nu + 0.5F );
        const float mu = nu - (float)nl;
        /*cstat +CERT-FLP34-C +CERT-FLP36-C*/
        const float mu2 = mu*mu;
        const float xi = 1.0F/x;
        const float xi2 = 2.0F*xi;
        float h = nu*xi;

        if ( h < fp_min ) {
            h = fp_min;
        }
        float b = xi2*nu;
        float d = 0.0F;
        float c = h;

        for ( int i = 1; i <= max_iter; ++i ) {
            b += xi2;
            d = 1.0F/( b + d );
            c = b + ( 1.0F/c );
            const float del = c*d;
            h *= del;
            if ( qFFMath_Abs( del - 1.0F ) < eps ) {
                break;
            }
        }

        float i_nul = fp_min;
        float i_pnu_l = h*i_nul;
        const float i_nul1 = i_nul;
        const float i_pnu_1 = i_pnu_l;
        float fact_m = nu*xi;

        for ( int l = nl ; l >= 1 ; --l ) {
            const float i_nu_temp = ( fact_m*i_nul )  + i_pnu_l;

            fact_m -= xi;
            i_pnu_l = ( fact_m*i_nu_temp ) + i_nul;
            i_nul = i_nu_temp;
        }
        const float f = i_pnu_l/i_nul;
        float Kmu, k_nu1;

        if ( x < x_min ) {
            const float x2 = 0.5F*x;
            const float pi_mu = QFFM_PI*mu;
            const float fact = ( qFFMath_Abs( pi_mu ) < eps ) ? 1.0F : ( pi_mu/qFFMath_Sin( pi_mu ) );
            d = -qFFMath_Log( x2 );
            float e = mu*d;
            const float fact2 = ( qFFMath_Abs( e ) < eps ) ? 1.0F : ( qFFMath_Sinh( e )/e );
            float gam1 = 0.0F, gam2 = 0.0F, gam_pl = 0.0F, gam_mi = 0.0F;
            gamma_temme(mu, &gam1, &gam2, &gam_pl, &gam_mi);
            float ff = fact*( ( gam1*qFFMath_Cosh( e ) ) + ( gam2*fact2*d ) );
            float sum = ff;
            e = qFFMath_Exp( e );
            float p = e/( 2.0F*gam_pl );
            float q = 1.0F/( 2.0F*e*gam_mi );
            float sum1 = p;
            c = 1.0F;
            d = x2*x2;

            for ( int i = 1; i <= max_iter; ++i ) {
                const float j = (float)i;
                ff = ( ( j*ff ) + p + q )/( ( j*j ) - mu2 );
                c *= d/j;
                p /= j - mu;
                q /= j + mu;
                const float del = c*ff;
                sum += del;
                sum1 += c*( p - ( j*ff ) );
                if ( qFFMath_Abs( del ) < ( eps*qFFMath_Abs( sum ) ) ) {
                    break;
                }
            }

            Kmu = sum;
            k_nu1 = sum1*xi2;
        }
        else {
            float del_h = d;
            float q1 = 0.0F;
            float q2 = 1.0F;
            const float a1 = 0.25F - mu2;
            float q = a1;
            float a = -a1;
            float s = 1.0F + ( q*del_h );

            b = 2.0F*( 1.0F + x );
            d = 1.0F/b;
            h = d;
            c = a1;
            for ( int i = 2 ; i <= max_iter; ++i) {
                a -= (float)( 2*( i - 1 ) );
                c = -a*c/( (float)i );
                const float q_new = ( q1 - ( b*q2 ) )/a;
                q1 = q2;
                q2 = q_new;
                q += c*q_new;
                b += 2.0F;
                d = 1.0F/( b + ( a*d ) );
                del_h = ( ( b*d ) - 1.0F )*del_h;
                h += del_h;
                const float del_s = q*del_h;
                s += del_s;
                if ( qFFMath_Abs( del_s/s ) < eps ) {
                    break;
                }
            }
            h = a1*h;
            Kmu = qFFMath_Sqrt( QFFM_PI/( 2.0F*x ) )*qFFMath_Exp(-x)/s;
            k_nu1 = Kmu*( mu + x + 0.5F - h )*xi;
        }
        const float k_pmu = ( mu*xi*Kmu ) - k_nu1;
        const float i_num_u = xi/( ( f*Kmu ) - k_pmu );

        i_nu[ 0 ] = i_num_u*i_nul1/i_nul;
        i_pnu[ 0 ] = i_num_u*i_pnu_1/i_nul;
        for ( int i = 1 ; i <= nl ; ++i ) {
            /*cstat -CERT-FLP36-C*/
            const float k_nu_temp = ( ( mu + (float)i )*xi2*k_nu1 ) + Kmu;
            /*cstat +CERT-FLP36-C*/
            Kmu = k_nu1;
            k_nu1 = k_nu_temp;
        }
        k_nu[ 0 ] = Kmu;
        k_pnu[ 0 ] = ( nu*xi*Kmu ) - k_nu1;
    }
}
/*============================================================================*/
static float cyl_bessel_ij_series( float nu,
                                   float x,
                                   float sgn,
                                   size_t max_iter )
{
    float y;

    if ( qFFMath_IsEqual( 0.0F, x ) ) {
        y = ( qFFMath_IsEqual( 0.0F, nu ) ) ? 1.0F : 0.0F;
    }
    else {
        const float x2 = 0.5F*x;
        float fact = nu*qFFMath_Log( x2 );
        float Jn = 1.0F;
        float term = 1.0F;
        const float xx4 = sgn*x2*x2;

        fact -= qFFMath_LGamma( nu + 1.0F );
        fact = qFFMath_Exp( fact );
        for ( size_t i = 1U ; i < max_iter; ++i ) {
            /*cstat -CERT-FLP36-C*/
            const float j = (float)i;
            /*cstat +CERT-FLP36-C*/
            term *= xx4/( j*( nu + j ) );
            Jn += term;
            if ( qFFMath_Abs( term/Jn ) < FLT_EPSILON ) {
                break;
            }
        }
        y = fact*Jn;
    }

    return y;
}
/*============================================================================*/
float qFFMath_Cyl_bessel_i( float nu,
                            float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( nu < 0.0F ) || ( x < 0.0F ) || qFFMath_IsNaN( nu ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( ( x*x ) < ( 10.0F*( nu + 1.0F ) ) ) {
        y = cyl_bessel_ij_series( nu, x, 1.0F, 200U );
    }
    else {
        float I_nu = 0.0F, K_nu = 0.0F, Ip_nu = 0.0F, Kp_nu = 0.0F;

        bessel_ik( nu, x, &I_nu, &K_nu, &Ip_nu, &Kp_nu );
        y = I_nu;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
static void cyl_bessel_jn_asymp( float nu,
                                 float x,
                                 float * Jnu,
                                 float * Nnu)
{
    const float mu = 4.0F*nu*nu;
    const float x8 = 8.0F*x;
    float P = 0.0F;
    float Q = 0.0F;
    float term = 1.0F;
    const float eps = FLT_EPSILON;
    size_t i = 0U;

    do {
        bool epsP, epsQ;
        float k2_1;
        /*cstat -MISRAC++2008-0-1-2_b*/
        float k = (float)i;
        /*cstat +MISRAC++2008-0-1-2_b*/
        k2_1 = ( 2.0F*k ) - 1.0F;
        term *= ( i == 0U ) ? 1.0F : ( -( mu - ( k2_1*k2_1 ) )/( k*x8 ) );
        epsP = qFFMath_Abs( term ) < ( eps*qFFMath_Abs( P ) );
        P += term;
        ++i;
        k = (float)i;
        k2_1 = ( 2.0F*k ) - 1.0F;
        term *= ( mu - ( k2_1*k2_1 ) )/( k*x8 );
        epsQ = qFFMath_Abs( term ) < ( eps*qFFMath_Abs( Q ) );
        Q += term;
        if ( epsP && epsQ && ( k > ( 0.5F*nu ) ) ) {
            break;
        }
        ++i;
    } while ( i < 1000U );
    const float chi = x - ( ( nu + 0.5F )*QFFM_PI_2 );
    const float c = qFFMath_Cos( chi );
    const float s = qFFMath_Sin( chi );
    const float coeff = qFFMath_Sqrt( 2.0F/( QFFM_PI*x ) );
    Jnu[ 0 ] = coeff*( ( c*P ) - ( s*Q ) );
    Nnu[ 0 ] = coeff*( ( s*P ) + ( c*Q ) );;
}
/*============================================================================*/
float qFFMath_Cyl_bessel_j( float nu,
                            float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( nu < 0.0F ) || ( x < 0.0F ) || qFFMath_IsNaN( nu ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( ( x*x ) < ( 10.0F*( nu + 1.0F ) ) ) {
        y = cyl_bessel_ij_series( nu, x, -1.0F, 200U );
    }
    else if ( x > 1000.0F ) {
        float j_nu = 0.0F, n_nu = 0.0F;

        cyl_bessel_jn_asymp( nu, x, &j_nu, &n_nu );
        y = j_nu;
    }
    else {
        float J_nu = 0.0F, N_nu = 0.0F, Jp_nu = 0.0F, Np_nu = 0.0F;

        bessel_jn( nu, x, &J_nu, &N_nu, &Jp_nu, &Np_nu );
        y = J_nu;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Cyl_bessel_k( float nu,
                            float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( nu < 0.0F ) || ( x < 0.0F ) || qFFMath_IsNaN( nu ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else {
        float I_nu = 0.0F, K_nu = 0.0F, Ip_nu = 0.0F, Kp_nu = 0.0F;

        bessel_ik( nu, x, &I_nu, &K_nu, &Ip_nu, &Kp_nu );
        y = K_nu;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Cyl_neumann( float nu,
                           float x )
{
    float y;
    /*cstat -MISRAC2012-Rule-13.5*/
    if ( ( nu < 0.0F ) || ( x < 0.0F ) || qFFMath_IsNaN( nu ) || qFFMath_IsNaN( x ) ) {
        y = QFFM_NAN;
    }
    else if ( x > 1000.0F ) {
        float J_nu = 0.0F, N_nu = 0.0F;
        cyl_bessel_jn_asymp( nu, x, &J_nu, &N_nu );
        y = N_nu;
    }
    else {
        float J_nu = 0.0F, N_nu = 0.0F, Jp_nu = 0.0F, Np_nu = 0.0F;

        bessel_jn( nu, x, &J_nu, &N_nu, &Jp_nu, &Np_nu );
        y = N_nu;
    }
    /*cstat +MISRAC2012-Rule-13.5*/
    return y;
}
/*============================================================================*/
float qFFMath_Sph_legendre( size_t l,
                            size_t m,
                            float theta )
{
    float y;

    if ( qFFMath_IsNaN( theta ) ) {
        y = QFFM_NAN;
    }
    else {
        const float x = qFFMath_Cos( theta );
        const float pi4 = 4.0F*QFFM_PI;

        if ( m > l ) { //skipcq : CXX-W2041
            y = 0.0F;
        }
        else if ( 0U == m ) {
            float P = qFFMath_Legendre( l, x );
            /*cstat -CERT-FLP36-C*/
            const float fact = qFFMath_Sqrt( (float)( ( 2U*l ) + 1U )/pi4 );
            /*cstat +CERT-FLP36-C*/
            P *= fact;
            y = P;
        }
        else if ( qFFMath_IsEqual( 1.0F, x ) || qFFMath_IsEqual( -1.0F, x ) ) {
            y = 0.0F;
        }
        else {
            /*cstat -CERT-FLP36-C*/
            const float mf = (float)m;
            const float y_mp1m_factor = x*qFFMath_Sqrt( (float)( ( 2U*m ) + 3U ) );
            /*cstat +CERT-FLP36-C*/
            const float sgn = ( 1U == ( m % 2U ) ) ? -1.0F : 1.0F;
            const float ln_circ = qFFMath_Log( 1.0F - ( x*x ) );
            const float ln_poc_h = qFFMath_LGamma( mf + 0.5F ) - qFFMath_LGamma( mf );
            const float ln_pre_val = ( -0.25F*QFFM_LN_PI ) + ( 0.5F*( ln_poc_h + ( mf*ln_circ ) ) );
            const float sr = qFFMath_Sqrt( ( 2.0F + ( 1.0F/mf ) )/pi4);
            float y_mm = sgn*sr*qFFMath_Exp( ln_pre_val );
            float y_mp1m = y_mp1m_factor*y_mm;

            if ( l == m ) {
                y = y_mm;
            }
            else if ( l == ( m + 1U ) ) {
                y = y_mp1m;
            }
            else {
                float y_lm = 0.0F;

                for ( size_t ll = ( m + 2U ) ; ll <= l; ++ll ) {
                    /*cstat -CERT-FLP36-C*/
                    const float ll_m_m = (float)( ll - m );
                    const float ll_p_m = (float)( ll + m );
                    const float ll2_p_1 = (float)( ( 2U*ll ) + 1U );
                    const float ll2_m_1 = (float)( ( 2U*ll ) - 1U );
                    const float ll_pm_m1 = (float)( ll + m - 1U );
                    const float ll_mm_m1 = (float)( ll - m - 1U );
                    /*cstat +CERT-FLP36-C*/
                    const float rat1 = ll_m_m/ll_p_m;
                    const float fact1 = qFFMath_Sqrt( rat1*ll2_p_1*ll2_m_1 );
                    /*cstat -CERT-FLP36-C*/
                    const float fact2 = qFFMath_Sqrt( rat1*( ll_mm_m1/ll_pm_m1 )*ll2_p_1/(float)( ( 2U*ll ) - 3U ) );
                    /*cstat -CERT-FLP36-C*/
                    y_lm = ( ( x*y_mp1m*fact1 ) - ( ll_pm_m1*y_mm*fact2 ) )/ll_m_m;
                    y_mm = y_mp1m;
                    y_mp1m = y_lm;
                }
                y = y_lm;
            }
        }
    }
    return y;
}
#endif /*#ifndef QLIBS_USE_STD_MATH*/