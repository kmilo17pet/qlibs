/*!
 * @file qfp16.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qfp16.h"
#include <ctype.h>
#include <stdbool.h>

/*used only for internal operations*/
/*! @cond  */
struct _qFP16_intern_s //skipcq: CXX-E2000
{
    const qFP16_t
    exp_max,
    f_2,                    /* [ 2 ] */
    f_3,                    /* [ 3 ] */
    f_16,                   /* [ -16 ] */
    f_100,                  /* [ 100 ] */
    f_6_5;                  /* [ 6.5 ] */
    const float one_fp16_f; /* [ 1/65536 ] */
    const double one_fp16_d;/* [ 1/65536 ] */
    const uint32_t
    overflow_mask,
    fraction_mask,
    integer_mask;
};

static const struct _qFP16_intern_s intern = { //skipcq: CXX-E2000
    /*exp_max*/         681391,
    /*f_2*/             131072,
    /*f_3*/             196608,
    /*f_16*/            1048576,
    /*f_100*/           6553600,
    /*f_6_5*/           425984,
    /*one_fp16_f*/      0.0000152587890625F,
    /*one_fp16_d*/      0.0000152587890625,
    /*overflow_mask*/   0x80000000U,
    /*fraction_mask*/   0x0000FFFFU,
    /*integer_mask*/    0xFFFF0000U
};

const struct _qFP16_const_s qFP16 = { //skipcq: CXX-E2000
    /*f_e*/             178145,
    /*f_log2e*/         94548,
    /*f_log10e*/        28462,
    /*f_ln2*/           45426,
    /*f_ln10*/          150902,
    /*f_pi*/            205887,
    /*f_pi_2*/          102944,
    /*f_2pi*/           411775,
    /*f_pi_4*/          51471,
    /*f_1_pi*/          20861,
    /*f_2_pi*/          41722,
    /*f_2_sqrtpi*/      73949,
    /*f_sqrt2*/         92682,
    /*f_sqrt1_2*/       46341,
    /*epsilon*/         1,
    /*max*/             2147483647,
    /*overflow*/        -2147483648,
    /*one*/             65536,
    /*one_half*/        32768,
    /*f_180_pi*/        3754936,
    /*f_pi_180*/        1144,
    /*f_180*/           11796480,
    /*f_360*/           23592960,
};

static qFP16_Settings_t fp_default = { -2147483647, 2147483647, 1U, 0U }; //skipcq: CXX-W2009
static qFP16_Settings_t *fp = &fp_default; //skipcq: CXX-W2009

/*! @endcond  */

static qFP16_t qFP16_rs( const qFP16_t x );
static qFP16_t qFP16_log2i( qFP16_t x );
static char *qFP16_itoa( char *buf,
                         uint32_t scale,
                         uint32_t value,
                         uint8_t skip );
static qFP16_t qFP16_Saturate( const qFP16_t nsInput,
                               const qFP16_t x,
                               const qFP16_t y );

/*cstat -MISRAC2012-Rule-10.8 -CERT-FLP34-C -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c*/

/*============================================================================*/
int qFP16_SettingsSet( qFP16_Settings_t * const instance,
                       qFP16_t min,
                       qFP16_t max,
                       uint8_t rounding,
                       uint8_t saturate )
{
    int retValue = 0;

    if ( ( NULL != instance ) && ( max > min ) && ( rounding <= 1U ) && ( saturate <= 1U ) ) {
        instance->min = min;
        instance->max = max;
        instance->rounding = rounding;
        instance->saturate = saturate;
        retValue = 1;
    }
    else {
        (void)fp_default;
    }

    return retValue;
}
/*============================================================================*/
void qFP16_SettingsSelect( qFP16_Settings_t * const instance )
{
    if ( NULL == instance ) {
        fp = &fp_default;
    }
    else {
        fp = instance;
    }
}
/*============================================================================*/
int qFP16_FPToInt( const qFP16_t x )
{
    int retValue;

    if ( 1U == fp->rounding ) {
        if ( x >= 0 ) {
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = ( x + ( qFP16.one >> 1 ) ) / qFP16.one;
        }
        else {
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = ( x - ( qFP16.one >> 1 ) ) / qFP16.one;
        }
    }
    else {
        retValue = (int)( (uint32_t)x >> 16 );
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_IntToFP( const int x )
{
    return (qFP16_t)( (uint32_t)x << 16 );
}
/*============================================================================*/
qFP16_t qFP16_FloatToFP( const float x )
{
    float retValue;
    /*cstat -CERT-FLP36-C*/
    retValue = x * (float)qFP16.one;
    /*cstat +CERT-FLP36-C*/
    if ( 1U == fp->rounding ) {
        retValue += ( retValue >= 0.0F ) ? 0.5F : -0.5F;
    }

    return (qFP16_t)retValue;
}
/*============================================================================*/
float qFP16_FPToFloat( const qFP16_t x )
{
    /*cstat -CERT-FLP36-C*/
    return (float)x * intern.one_fp16_f;
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
qFP16_t qFP16_DoubleToFP( const double x )
{
    double retValue;
    /*cstat -CERT-FLP36-C*/
    retValue = x * (double)qFP16.one;
    /*cstat +CERT-FLP36-C*/
    if ( 1U == fp->rounding ) {
        retValue += ( retValue >= 0.0 ) ? 0.5 : -0.5;
    }

    return (qFP16_t)retValue;
}
/*============================================================================*/
double qFP16_FPToDouble( const qFP16_t x )
{
    /*cstat -CERT-FLP36-C*/
    return (double)x * intern.one_fp16_d;
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
qFP16_t qFP16_Abs( const qFP16_t x )
{
    qFP16_t retValue;

    if ( x == fp->min ) {
        retValue = qFP16.overflow;
    }
    else {
        retValue = ( x >= 0 ) ? x : -x;
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Floor( const qFP16_t x )
{
    /*cppcheck-suppress misra-c2012-10.8 */
    return (qFP16_t)( (uint32_t)x & intern.integer_mask );
}
/*============================================================================*/
qFP16_t qFP16_Ceil( const qFP16_t x )
{
    /*cstat -MISRAC2012-Rule-10.1_R2*/
    return ( x & (qFP16_t)intern.integer_mask ) +
           ( ( x & (qFP16_t)intern.fraction_mask ) ? qFP16.one : 0 );
    /*cstat +MISRAC2012-Rule-10.1_R2*/
}
/*============================================================================*/
qFP16_t qFP16_Round( const qFP16_t x )
{
    return qFP16_Floor( x + qFP16.one_half );
}
/*============================================================================*/
qFP16_t qFP16_Add( const qFP16_t X,
                   const qFP16_t Y )
{
    uint32_t x = (uint32_t)X, y = (uint32_t)Y;
    uint32_t retValue;

    retValue =  x + y;
    if ( ( 0U == ( ( x ^ y ) & intern.overflow_mask ) ) && ( 0U != ( ( x ^ retValue ) & intern.overflow_mask ) ) ) {
        retValue = (uint32_t)qFP16.overflow;
    }

    return qFP16_Saturate( (qFP16_t)retValue, X, X );
}
/*============================================================================*/
qFP16_t qFP16_Sub( const qFP16_t X,
                   const qFP16_t Y )
{
    uint32_t x = (uint32_t)X, y = (uint32_t)Y;
    uint32_t retValue;

    retValue =  x - y;
    if ( ( 0U != ( ( x ^ y ) & intern.overflow_mask ) ) && ( 0U != ( ( x ^ retValue ) & intern.overflow_mask ) ) ) {
        retValue = (uint32_t)qFP16.overflow;
    }

    return qFP16_Saturate( (qFP16_t)retValue, X, X );
}
/*============================================================================*/
qFP16_t qFP16_Mul( const qFP16_t x,
                   const qFP16_t y )
{
    qFP16_t retValue = qFP16.overflow;
    int32_t a, c, ac, adcb, mulH;
    uint32_t b, d, bd, tmp, mulL;
    /*cstat -MISRAC2012-Rule-10.3*/
    /*cppcheck-suppress misra-c2012-10.1 */
    a = ( x >> 16 );
    /*cppcheck-suppress misra-c2012-10.1 */
    c = ( y >> 16 );
    b = ( x & 0xFFFF );
    d = ( y & 0xFFFF );
    /*cstat +MISRAC2012-Rule-10.3*/
    ac = a*c;
    /*cppcheck-suppress misra-c2012-10.8 */
    adcb = (int32_t)( ( (uint32_t)a*d ) + ( (uint32_t)c*b ) );
    bd = b*d;
    /*cppcheck-suppress misra-c2012-10.1 */
    mulH = ac + ( adcb >> 16 );
    tmp = (uint32_t)adcb << 16;
    mulL = bd + tmp;
    if ( mulL < bd ) {
        ++mulH;
    }
    /*cppcheck-suppress misra-c2012-10.6 */
    a = ( mulH < 0 ) ? -1 : 0;
    /*cppcheck-suppress misra-c2012-10.1 */
    if ( a == ( mulH >> 15 ) ) {
        if ( 1U == fp->rounding ) {
            uint32_t tmp2;

            tmp2 = mulL;
            mulL -= (uint32_t)qFP16.one_half;
            mulL -= (uint32_t)mulH >> 31;
            if ( mulL > tmp2 ) {
                --mulH;
            }
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = (qFP16_t)( mulH << 16 ) | (qFP16_t)( mulL >> 16 );
            retValue += 1;
        }
        else {
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = (qFP16_t)( mulH << 16 ) | (qFP16_t)( mulL >> 16 );
        }
    }

    return qFP16_Saturate( retValue, x, y );
}
/*============================================================================*/
qFP16_t qFP16_Div( const qFP16_t x,
                   const qFP16_t y )
{
    qFP16_t retValue = fp->min;

    if ( 0 != y ) {
        uint32_t xRem, xDiv, bit = 0x10000U;

        xRem = (uint32_t)( ( x >= 0 ) ? x : -x );
        xDiv = (uint32_t)( ( y >= 0 ) ? y : -y );

        while ( xDiv < xRem ) {
            xDiv <<= 1;
            bit <<= 1;
        }
        retValue = qFP16.overflow;
        /*cstat -MISRAC2012-Rule-14.3_a*/
        if ( 0U != bit ) { /*MISRAC2012-Rule-14.3_a false positive*/
        /*cstat +MISRAC2012-Rule-14.3_a*/
            uint32_t quotient = 0U;

            if ( 0U != ( xDiv & 0x80000000U ) ) {
                if ( xRem >= xDiv ) {
                    quotient |= bit;
                    xRem -= xDiv;
                }
                xDiv >>= 1;
                bit >>= 1;
            }

            while ( ( 0U != bit ) && ( 0U != xRem ) ) {
                if ( xRem >= xDiv ) {
                    quotient |= bit;
                    xRem -= xDiv;
                }
                xRem <<= 1;
                bit >>= 1;
            }
            if ( 1U == fp->rounding ) {
                if ( xRem >= xDiv ) {
                    ++quotient;
                }
            }

            retValue = (qFP16_t)quotient;
            /*cppcheck-suppress misra-c2012-10.8 */
            if ( 0U != ( (uint32_t)( x ^ y ) & intern.overflow_mask ) ) {
                if ( quotient == (uint32_t)fp->min ) {
                    retValue = qFP16.overflow;
                }
                else {
                    retValue = -retValue;
                }
            }
        }
    }

    return qFP16_Saturate( retValue, x, y );
}
/*============================================================================*/
qFP16_t qFP16_Mod( const qFP16_t x,
                   const qFP16_t y )
{
    qFP16_t retValue = 0;

    if ( 0 != y ) {
      retValue = x % y;
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Sqrt( qFP16_t x )
{
    qFP16_t retValue = qFP16.overflow;

    if ( x > 0 ) {
        uint32_t bit;
        uint8_t n;

        retValue = 0;
        /*cppcheck-suppress [ cert-INT31-c, misra-c2012-12.2, misra-c2012-12.1  ]*/
        bit = ( 0 != ( x & (qFP16_t)4293918720 ) ) ? ( 1U << 30U ) : ( 1U << 18U );
        while ( bit > (uint32_t)x ) {
            bit >>= 2U;
        }

        for ( n = 0U ; n < 2U ; ++n ) {
            while ( 0U != bit ) {
                /*cppcheck-suppress misra-c2012-10.8 */
                if ( x >= (qFP16_t)( (uint32_t)retValue + bit ) ) {
                    /*cppcheck-suppress misra-c2012-10.8 */
                    x -= (qFP16_t)( (uint32_t)retValue + bit );
                    /*cppcheck-suppress misra-c2012-10.8 */
                    retValue = (qFP16_t)( ( (uint32_t)retValue >> 1U ) + bit );
                }
                else {
                    /*cppcheck-suppress misra-c2012-10.1 */
                    retValue = ( retValue >> 1 );
                }
                bit >>= 2U;
            }

            if ( 0U == n ) {
                if ( x > 65535 ) {
                    x -= retValue;
                    /*cppcheck-suppress misra-c2012-10.1 */
                    x = ( x << 16 ) - qFP16.one_half;
                    /*cppcheck-suppress misra-c2012-10.1 */
                    retValue = ( retValue << 16 ) + qFP16.one_half;
                }
                else {
                    x <<= 16;
                    retValue <<= 16;
                }
                /*cppcheck-suppress [ misra-c2012-10.6, misra-c2012-12.2 ] */
                bit = 1U << 14U;
            }
        }
    }
    if ( ( 1U == fp->rounding ) && ( x > retValue ) ) {
        ++retValue;
    }

    return (qFP16_t)retValue;
}
/*============================================================================*/
qFP16_t qFP16_Exp( qFP16_t x )
{
    qFP16_t retValue, term;
    bool isNegative;
    int i;

    if ( 0 == x ) {
        retValue = qFP16.one;
    }
    else if ( x == qFP16.one ) {
        retValue = qFP16.f_e;
    }
    else if ( x >= intern.exp_max ) {
        retValue = fp->max;
    }
    else if ( x <= -intern.exp_max ) {
        retValue = 0;
    }
    else {
        isNegative = ( x < 0 );
        if ( isNegative ) {
            x = -x;
        }

        retValue = x + qFP16.one;
        term = x;

        for ( i = 2 ; i < 30 ; ++i ) {
            term = qFP16_Mul( term, qFP16_Div( x, qFP16_IntToFP( i ) ) );
            retValue += term;

            if ( ( term < 500 ) && ( ( i > 15 ) || ( term < 20 ) ) ) {
                break;
            }
        }

        if ( isNegative ) {
            retValue = qFP16_Div( qFP16.one, retValue );
        }
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Log( qFP16_t x )
{
    qFP16_t retValue = qFP16.overflow;
    static const qFP16_t e4 = 3578144; /*e^4*/

    if ( x > 0 ) {
        qFP16_t guess = intern.f_2, delta;
        int scaling = 0, count = 0;

        while ( x > intern.f_100 ) {
            x = qFP16_Div( x, e4 );
            scaling += 4;
        }

        while ( x < qFP16.one ) {
            x = qFP16_Mul( x, e4 );
            scaling -= 4;
        }

        do {
            qFP16_t e = qFP16_Exp( guess );

            delta = qFP16_Div( x - e , e );

            if ( delta > intern.f_3 ) {
                delta = intern.f_3;
            }
            guess += delta;
        } while ( ( count++ < 10 ) && ( ( delta > 1 ) || ( delta < -1 ) ) );

        retValue = guess + qFP16_IntToFP( scaling );
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Log2( const qFP16_t x )
{
    qFP16_t retValue = qFP16.overflow;

    if ( x > 0 ) {
        if ( x < qFP16.one ) {
            if ( 1 == x ) {
                retValue = -intern.f_16;
            }
            else {
                qFP16_t inv;
                inv = qFP16_Div( qFP16.one, x );
                retValue = -qFP16_log2i( inv );
            }
        }
        else {
            retValue = qFP16_log2i( x );
        }
    }
    if ( 1U == fp->saturate ) {
        if ( qFP16.overflow == retValue ) {
            retValue = fp->min;
        }
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_RadToDeg( const qFP16_t x )
{
    return qFP16_Mul( qFP16_WrapToPi( x ), qFP16.f_180_pi );
}
/*============================================================================*/
qFP16_t qFP16_DegToRad( qFP16_t x )
{
    return qFP16_Mul( qFP16_WrapTo180( x ), qFP16.f_pi_180 );
}
/*============================================================================*/
qFP16_t qFP16_WrapToPi( qFP16_t x )
{
    if ( ( x < -qFP16.f_pi ) || ( x > qFP16.f_pi ) ) {
        while ( x > qFP16.f_pi ) {
            x -= qFP16.f_2pi;
        }
        while ( x <= -qFP16.f_pi ) {
            x += qFP16.f_2pi;
        }
    }

    return x;
}
/*============================================================================*/
qFP16_t qFP16_WrapTo180( qFP16_t x )
{
    if ( ( x < -qFP16.f_180 ) || ( x > qFP16.f_180 ) ) {
        while ( x > qFP16.f_180 ) {
            x -= qFP16.f_360;
        }
        while ( x <= -qFP16.f_180 ) {
            x += qFP16.f_360;
        }
    }

    return x;
}
/*============================================================================*/
qFP16_t qFP16_Sin( qFP16_t x )
{
    qFP16_t retValue, x2;

    x = qFP16_WrapToPi( x );
    x2 = qFP16_Mul( x ,x );
    retValue = x;
    x = qFP16_Mul( x, x2 );
    retValue -= ( x / 6 ); /*x^3/3!*/
    x = qFP16_Mul( x, x2 );
    retValue += ( x / 120 ); /*x^5/5!*/
    x = qFP16_Mul( x, x2 );
    retValue -= ( x / 5040 ); /*x^7/7!*/
    x = qFP16_Mul( x, x2 );
    retValue += ( x / 362880 ); /*x^9/9!*/
    x = qFP16_Mul( x, x2);
    retValue -= ( x / 39916800 ); /*x^11/11!*/

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Cos( qFP16_t x )
{
    return qFP16_Sin( x + qFP16.f_pi_2 );
}
/*============================================================================*/
qFP16_t qFP16_Tan( qFP16_t x )
{
    qFP16_t a ,b;

    a = qFP16_Sin( x );
    b = qFP16_Cos( x );

    return qFP16_Div( a, b );
}
/*============================================================================*/
qFP16_t qFP16_Atan2( const qFP16_t y,
                     const qFP16_t x )
{
    qFP16_t absY, mask, angle, r, r_3;
    const qFP16_t QFP16_0_981689 = 0x0000FB50;
    const qFP16_t QFP16_0_196289 = 0x00003240;
    static const qFP16_t f_3pi_div_4 = 154415; /*3*pi/4*/
    /*cppcheck-suppress misra-c2012-10.1 */
    mask = ( y >> ( sizeof(qFP16_t)*7U ) );
    absY = ( y + mask ) ^ mask;
    if ( x >= 0 ) {
        r = qFP16_Div( ( x - absY ), ( x + absY ) );
        angle = qFP16.f_pi_4;
    }
    else {
        r = qFP16_Div( ( x + absY ), ( absY - x ) );
        angle = f_3pi_div_4;
    }
    r_3 = qFP16_Mul( qFP16_Mul( r, r ), r );
    /* 3rd order polynomial approximation*/
    angle += qFP16_Mul( QFP16_0_196289, r_3 ) - qFP16_Mul( QFP16_0_981689, r );
    /*cstat -ATH-neg-check-nonneg*/
    if ( y < 0 ) { /*ATH-neg-check-nonneg false positive*/
        angle = -angle;
    }
    /*cstat +ATH-neg-check-nonneg*/
    return angle;
}
/*============================================================================*/
qFP16_t qFP16_Atan( qFP16_t x )
{
    return qFP16_Atan2( x, qFP16.one );
}
/*============================================================================*/
qFP16_t qFP16_Asin( qFP16_t x )
{
    qFP16_t retValue = 0;

    if ( ( x <= qFP16.one ) && ( x >= -qFP16.one ) ) {
        retValue = qFP16.one - qFP16_Mul( x, x );
        retValue = qFP16_Div( x, qFP16_Sqrt( retValue ) );
        retValue = qFP16_Atan( retValue );
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Acos( qFP16_t x )
{
    return ( qFP16.f_pi_2 - qFP16_Asin( x ) );
}
/*============================================================================*/
qFP16_t qFP16_Cosh( qFP16_t x )
{
    qFP16_t retValue = qFP16.overflow;
    qFP16_t epx, enx;

    if ( 0 == x ) {
        retValue = qFP16.one;
    }
    else if ( ( x >= intern.exp_max ) || ( x <= -intern.exp_max ) ) {
        retValue = fp->max;
    }
    else {
        epx = qFP16_Exp( x );
        enx = qFP16_Exp( -x );
        if ( ( qFP16.overflow != epx ) && ( qFP16.overflow != enx ) ) {
            retValue = epx + enx;
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = ( retValue >> 1 );
        }
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Sinh( qFP16_t x )
{
    qFP16_t retValue = qFP16.overflow;
    qFP16_t epx, enx;

    if ( 0 == x ) {
        retValue = qFP16.one;
    }
    else if ( x >= intern.exp_max ) {
        retValue = fp->max;
    }
    else if ( x <= -intern.exp_max ) {
        retValue = -fp->max;
    }
    else {
        epx = qFP16_Exp( x );
        enx = qFP16_Exp( -x );
        if ( ( qFP16.overflow != epx ) && ( qFP16.overflow != enx ) ) {
            retValue = epx - enx;
            /*cppcheck-suppress misra-c2012-10.1 */
            retValue = ( retValue >> 1 );
        }
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Tanh( qFP16_t x )
{
    qFP16_t retValue, epx, enx;

    if ( 0 == x ) {
        retValue = 0;
    }
    else if ( x >  intern.f_6_5 ) { /* tanh for any x>6.5 ~= 1*/
        retValue = qFP16.one;
    }
    else if ( x < -intern.f_6_5 ) { /* tanh for any x<6.5 ~= -1*/
        retValue = -qFP16.one;
    }
    else {
        retValue = qFP16_Abs( x );
        epx = qFP16_Exp( retValue );
        enx = qFP16_Exp( -retValue );
        retValue = qFP16_Div( epx - enx, epx + enx );
        retValue = ( x > 0 ) ? retValue : -retValue;
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Polyval( const qFP16_t * const p,
                       const size_t n,
                       const qFP16_t x )
{
    qFP16_t fx;
    size_t i;
    /*polynomial evaluation using Horner's method*/
    fx = p[ 0 ];
    for ( i = 1U ; i < n ; ++i ) {
        qFP16_t tmp = qFP16_Mul( fx, x );

        if ( qFP16.overflow == tmp ) {
            fx = qFP16.overflow;
            break;
        }
        fx =  qFP16_Add( tmp , p[ i ] );
    }

    return fx;
}
/*============================================================================*/
qFP16_t qFP16_IPow( const qFP16_t x,
                    const qFP16_t y )
{
    qFP16_t retValue;
    qFP16_t n;
    int32_t i;

    retValue = qFP16.one;
    /*cppcheck-suppress misra-c2012-10.1 */
    n = y >> 16;
    if ( 0 == n ) {
        retValue = qFP16.one;
    }
    else if ( qFP16.one == n ) {
        retValue = x;
    }
    else {
        for ( i = 0 ; i < n ; ++i ) {
            retValue = qFP16_Mul( x, retValue );
            if ( qFP16.overflow == retValue ) {
                break;
            }
        }
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_Pow( const qFP16_t x,
                   const qFP16_t y )
{
    qFP16_t retValue = qFP16.overflow;

    if ( ( 0U == ( (uint32_t)y & intern.fraction_mask ) ) && ( y > 0 ) ) {
        /*handle integer exponent explicitly*/
        retValue = qFP16_IPow( x, y );
    }
    else {
        qFP16_t tmp;
        tmp = qFP16_Mul( y, qFP16_Log( qFP16_Abs( x ) ) );
        if ( qFP16.overflow != tmp ) {
            retValue = qFP16_Exp( tmp );
            if ( x < 0 ) {
                retValue = -retValue;
            }
        }
    }

    return retValue;
}
/*============================================================================*/
char* qFP16_FPToA( const qFP16_t num,
                   char *str,
                   int decimals )
{
    char *retValue = str;

    if ( qFP16.overflow == num ) {
        str[ 0 ] = 'o';
        str[ 1 ] = 'v';
        str[ 2 ] = 'e';
        str[ 3 ] = 'r';
        str[ 4 ] = 'f';
        str[ 5 ] = 'l';
        str[ 6 ] = 'o';
        str[ 7 ] = 'w';
        str[ 8 ] = '\0';
    }
    else {
        const uint32_t iScales[ 6 ] = { 1U, 10U, 100U, 1000U, 10000U, 100000U };
        uint32_t uValue, fPart, scale;
        int32_t iPart;

        uValue = (uint32_t)( ( num >= 0 ) ? num : -num );
        if ( num < 0 ) {
            *str++ = '-';
        }

        iPart = (int32_t)( uValue >> 16 );
        fPart = uValue & intern.fraction_mask;
        if ( decimals > 5 ) {
            decimals = 5;
        }
        if ( decimals < 0 ) {
            decimals = 0;
        }
        scale = iScales[ decimals ];
        fPart = (uint32_t)qFP16_Mul( (qFP16_t)fPart, (qFP16_t)scale );

        if ( fPart >= scale ) {
            iPart++;
            fPart -= scale;
        }
        str = qFP16_itoa( str, 10000, (uint32_t)iPart, 1U );

        if ( 1U != scale ) {
            *str++ = '.';
            str = qFP16_itoa( str, scale/10U, fPart, 0U );
        }
        *str = '\0';
    }

    return retValue;
}
/*============================================================================*/
qFP16_t qFP16_AToFP( const char *s )
{
    uint8_t neg;
    uint32_t iPart = 0U, fPart = 0U, scale = 1U, digit;
    int32_t count = 0;
    qFP16_t retValue = qFP16.overflow;
    bool point_seen = false, overflow = false, badchr = false;
    char c;

    /*cstat -MISRAC2012-Dir-4.11_h*/
    while ( 0 != isspace( (int)*s ) ) {
        s++; /*discard whitespaces*/
    }

    neg = ( '-' == *s ) ? 1U : 0U;
    if ( ( '+' == *s ) || ( '-' == *s ) ) {
        s++; /*move to the next sign*/
    }

    for ( c = s[ 0 ] ; '\0' != c ; c = s[ 0 ] ) {
        if ( '.' == c ) {
            point_seen = true;
        }
        else if ( 0 != isdigit( (int)c ) ) {
            digit = (uint32_t)c - (uint32_t)'0';
            if ( point_seen ) { /* Decode the fractional part */
                scale *= 10U;
                fPart *= 10U;
                fPart += digit;
            }
            else { /* Decode the decimal part */
                iPart *= 10U;
                iPart += digit;
                ++count;
                overflow = ( ( 0 == count ) || ( count > 5 ) ||
                             ( iPart > 32768U ) ||
                             ( ( 0U == neg ) && ( iPart > 32767U ) ) );
            }
        }
        else {
            badchr = true;
        }
        if ( overflow || badchr ) {
            break;
        }
        s++;
    }
    if ( false == overflow ) {
        /*cppcheck-suppress misra-c2012-10.1 */
        retValue = (qFP16_t)iPart << 16;
        retValue += qFP16_Div( (qFP16_t)fPart, (qFP16_t)scale );
        retValue = ( 1U == neg ) ? -retValue : retValue;
    }
    /*cstat +MISRAC2012-Dir-4.11_h*/
    return retValue;
}
/*============================================================================*/
static qFP16_t qFP16_rs( const qFP16_t x )
{
    qFP16_t retValue;

    if ( 1U == fp->rounding ) {
        /*cppcheck-suppress misra-c2012-10.1 */
        retValue = ( x >> 1U ) + ( x & 1 );
    }
    else {
        /*cppcheck-suppress misra-c2012-10.1 */
        retValue = x >> 1;
    }

    return retValue;
}
/*============================================================================*/
static qFP16_t qFP16_log2i( qFP16_t x )
{
    qFP16_t retValue = 0;

    while ( x >= intern.f_2 ) {
        ++retValue;
        x = qFP16_rs( x );
    }

    if ( 0 == x ) {
        /*cppcheck-suppress misra-c2012-10.1 */
        retValue = retValue << 16;
    }
    else {
        int i;
        for ( i = 16 ; i > 0 ; --i ) {
            x = qFP16_Mul( x, x );
            retValue <<= 1;
            if ( x >= intern.f_2 ) {
                retValue |= 1;
                x = qFP16_rs( x );
            }
        }
        if ( 1U == fp->rounding ) {
            x = qFP16_Mul( x, x );
            if ( x >= intern.f_2 ) {
                ++retValue;
            }
        }
    }

    return retValue;
}
/*============================================================================*/
static char *qFP16_itoa( char *buf,
                         uint32_t scale,
                         uint32_t value,
                         uint8_t skip )
{
    while ( 0U != scale ) {
        uint32_t digit = ( value / scale );
        if ( ( 0U == skip ) || ( 0U != digit ) || ( 1U == scale ) ) {
            skip = 0U;
            /*cstat -MISRAC2012-Rule-10.2 -MISRAC2012-Rule-10.3*/
            *buf++ = (char)'0' + (char)digit;
            /*cstat +MISRAC2012-Rule-10.2 +MISRAC2012-Rule-10.3*/
            value %= scale;
        }
        scale /= 10U;
    }

    return buf;
}
/*============================================================================*/
static qFP16_t qFP16_Saturate( const qFP16_t nsInput,
                               const qFP16_t x,
                               const qFP16_t y )
{
    qFP16_t retValue = nsInput;

    if ( 1U == fp->saturate ) {
        if ( qFP16.overflow == nsInput ) {
            retValue = ( ( x >= 0 ) == ( y >= 0 ) ) ? fp->max : fp->min;
        }
    }

    return retValue;
}
/*============================================================================*/
/*cstat +MISRAC2012-Rule-10.8 +CERT-FLP34-C +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c*/
