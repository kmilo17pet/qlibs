#include "qffmath.h"
#include <stdint.h>
#include <string.h>
#include <float.h>

#define cast_float_to_uint32( i, f )    (void)memcpy( &i, &f, sizeof(float) )
#define cast_uint32_to_float( f, i )    (void)memcpy( &f, &i, sizeof(float) )

/*============================================================================*/
float _qFFMath_GetAbnormal( const int i )
{
    static const uint32_t u_ab[ 2 ] = { 0x7F800000u, 0x7FBFFFFFu };
    static float f_ab[ 2 ] = { 0.0f, 0.0f };
    static bool init = true;
    
    if ( init ) {
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

    cast_float_to_uint32( u, f );
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

    cast_float_to_uint32( y, x );
    y = 0x7EF311C7u - y;
    cast_uint32_to_float( z, y );
    
    return z*( 2.0f - ( x*z ) );
}
/*============================================================================*/
float qFFMath_Sqrt( float x )
{
    float retVal;

    if ( x < 0.0f ) {
        retVal = QFFM_NAN;
    }
    else {
        uint32_t y = 0u;
        float z = 0.0f;

        cast_float_to_uint32( y, x );
        y = ( ( y - 0x00800000u ) >> 1u ) + 0x20000000u;
        cast_uint32_to_float( z, y );
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

        cast_float_to_uint32( y, x );
        y = 0x5F375A86u - ( y >> 1u );
        cast_uint32_to_float( x, y );
        retVal = x*( 1.5f - ( z*x*x ) );
    }

    return retVal;
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
    x *= -0.318309886f;
    y = x + 25165824.0f;
    x -= y - 25165824.0f;
    x *= qFFMath_Abs( x ) - 1.0f;
    return x*( ( 3.5841304553896f*qFFMath_Abs( x ) ) + 3.1039673861526f );
}
/*============================================================================*/
float qFFMath_Cos( float x )
{
    return qFFMath_Sin( x + 1.570796327f );
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
    return 1.570796327f - qFFMath_ASin( x );
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
    float t;
    float f;
    t = 3.141592653f - ( ( y < 0.0f ) ? 6.283185307f : 0.0f );
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
        cast_float_to_uint32( y, exponent );
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

        cast_float_to_uint32( y, x );
        y2 = y;
        y >>= 23u;
        /*cstat -CERT-FLP36-C*/
        retVal = (float)y;
        /*cstat +CERT-FLP36-C*/
        y = ( y2 & 0x007FFFFFu ) | 0x3F800000u;
        cast_uint32_to_float( x, y );
        retVal += -128.0f + ( x*( ( -0.333333333f*x ) + 2.0f ) ) - 0.666666666f;
    }

    return retVal;
}
/*============================================================================*/
float qFFMath_Exp( float x )
{
    return qFFMath_Exp2( 1.442695041f*x );
}
/*============================================================================*/
float qFFMath_Exp10( float x )
{
    return qFFMath_Exp2( 3.32192809f*x );
}
/*============================================================================*/
float qFFMath_Log( float x )
{
    return 0.693147181f*qFFMath_Log2(x);
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
