/*!
 * @file qfp16.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qfp16.h"

#define QFP16_6_5               ( 425984 )              /*6.5*/
#define QFP16_1DIVUNITY_FLOAT   ( 0.0000152587890625f ) /*1/65536*/
#define QFP16_1DIVUNITY_DOUBLE  ( 0.0000152587890625  )

static qFP16_Settings_t fp_default = { QFP16_MIN, QFP16_MAX, 1u, 0u };
static qFP16_Settings_t *fp = &fp_default;

static uint32_t qFP16_OverflowCheck( uint32_t res, uint32_t x, uint32_t y );
static qFP16_t qFP16_rs( qFP16_t x );
static qFP16_t qFP16_log2i( qFP16_t x );
static char *qFP16_itoa( char *buf, uint32_t scale, uint32_t value, uint8_t skip );
static qFP16_t qFP16_Saturate( qFP16_t nsInput, qFP16_t x, qFP16_t y );

/*cstat -MISRAC2012-Rule-10.8 -CERT-FLP34-C -MISRAC2012-Rule-1.3_n -MISRAC2012-Rule-10.1_R6 -ATH-shift-neg -CERT-INT34-C_c*/

/*============================================================================*/
int qFP16_SettingsSet( qFP16_Settings_t *instance, qFP16_t min, qFP16_t max, uint8_t rounding, uint8_t saturate )
{
    int retVal = 0;
    if ( ( NULL != instance ) && ( max > min) && ( rounding <= 1u ) && ( saturate <= 1u ) ) {
        instance->min = min;
        instance->max = max;
        instance->rounding = rounding;
        instance->saturate = saturate;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
void qFP16_SettingsSelect( qFP16_Settings_t *instance )
{
    if ( NULL == instance ) {
        fp = &fp_default;
    }
    else {
        fp = instance;
    }
}
/*============================================================================*/
int qFP16_FPToInt( qFP16_t x )
{
    int RetValue;
    if ( 1u == fp->rounding ) {
        if ( x >= 0 ) {
            RetValue =  ( x + ( QFP16_1 >> 1 ) ) / QFP16_1;
        }
        else {
            RetValue =  ( x - ( QFP16_1 >> 1 ) ) / QFP16_1;
        }	        
    }
    else {
        RetValue = (int)( (uint32_t)x >> 16 );
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_IntToFP( int x )
{
    return (qFP16_t)( (uint32_t)x << 16 ); 
}
/*============================================================================*/
qFP16_t qFP16_FloatToFP( float x )
{
    float RetValue;
    /*cstat -CERT-FLP36-C*/
    RetValue = x * (float)QFP16_1;
    /*cstat +CERT-FLP36-C*/
    if ( 1u == fp->rounding ) {
        RetValue += (RetValue >= 0.0f) ? 0.5f : -0.5f;
    }    
    return (qFP16_t)RetValue;
}
/*============================================================================*/
float qFP16_FPToFloat( qFP16_t x )
{
    /*cstat -CERT-FLP36-C*/
    return (float)x * QFP16_1DIVUNITY_FLOAT; 
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
qFP16_t qFP16_DoubleToFP( double x )
{
    double RetValue;
    /*cstat -CERT-FLP36-C*/
    RetValue = x * (double)QFP16_1;
    /*cstat +CERT-FLP36-C*/
    if( 1u == fp->rounding ){
        RetValue += ( RetValue >= 0.0 )? 0.5 : -0.5;
    }    
    return (qFP16_t)RetValue;
}
/*============================================================================*/
double qFP16_FPToDouble( qFP16_t x )
{
    /*cstat -CERT-FLP36-C*/
    return (double)x * QFP16_1DIVUNITY_DOUBLE; 
    /*cstat +CERT-FLP36-C*/
}     
/*============================================================================*/
qFP16_t qFP16_Abs( qFP16_t x )
{   
    qFP16_t RetValue;
    if ( x == fp->min ) {
        RetValue = QFP16_OVERFLOW;
    }
    else{
        RetValue = ( x >= 0 ) ? x : -x;
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Floor( qFP16_t x )
{
    return (qFP16_t)( (uint32_t)x & 0xFFFF0000uL ); 
}
/*============================================================================*/
qFP16_t qFP16_Ceil( qFP16_t x )
{
    /*cstat -MISRAC2012-Rule-10.1_R2*/
    return ( x & (qFP16_t)0xFFFF0000uL ) + ( ( x & (qFP16_t)0x0000FFFFuL ) ? QFP16_1 : 0 ); 
    /*cstat +MISRAC2012-Rule-10.1_R2*/
}
/*============================================================================*/
qFP16_t qFP16_Round( qFP16_t x )
{
    return qFP16_Floor( x + QFP16_1_DIV_2 );
}
/*============================================================================*/
qFP16_t qFP16_Add( qFP16_t X, qFP16_t Y )
{
    uint32_t x = (uint32_t)X, y = (uint32_t)Y;
    uint32_t RetValue;
    RetValue =  x + y;
    RetValue = qFP16_OverflowCheck( RetValue, x, y );
    return qFP16_Saturate( (qFP16_t)RetValue, X, X );    
}
/*============================================================================*/
qFP16_t qFP16_Sub( qFP16_t X, qFP16_t Y )
{
    uint32_t x = (uint32_t)X, y = (uint32_t)Y; 
    uint32_t RetValue;
    RetValue =  x - y;
    RetValue = qFP16_OverflowCheck( RetValue, x, y );
    return qFP16_Saturate( (qFP16_t)RetValue, X, X ); 
}
/*============================================================================*/
qFP16_t qFP16_Mul( qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
    int32_t a, c, ac, adcb, mulH;
    uint32_t b, d, bd, tmp, mulL;
    /*cstat -MISRAC2012-Rule-10.3*/
    a = ( x >> 16 );
    c = ( y >> 16 );
    b = ( x & 0xFFFF );
    d = ( y & 0xFFFF );
    /*cstat +MISRAC2012-Rule-10.3*/
    ac = a*c;
    adcb = (int32_t)( ( (uint32_t)a*d ) + ( (uint32_t)c*b ) );
    bd = b*d;    
    mulH = ac + ( adcb >> 16 );
    tmp = (uint32_t)adcb << 16;
    mulL = bd + tmp;
    if ( mulL < bd ) {
        mulH++;
    }
    if ( ( mulH >> 31 ) == ( mulH >> 15 ) ) {
        if ( 1u == fp->rounding ) {
            uint32_t tmp2;
            tmp2 = mulL;
            mulL -= (uint32_t)QFP16_1_DIV_2;
            mulL -= (uint32_t)mulH >> 31;
            if ( mulL > tmp2 ) {
                mulH--;
            }
            RetValue = (qFP16_t)( mulH << 16 ) | (qFP16_t)( mulL >> 16 );
            RetValue += 1;  
        }
        else{
            RetValue = (qFP16_t)( mulH << 16 ) | (qFP16_t)( mulL >> 16 );
        }		
    }
    return qFP16_Saturate( RetValue, x, y );
}
/*============================================================================*/
qFP16_t qFP16_Div( qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue = fp->min;
    if ( 0 != y ) {
        uint32_t xRem, xDiv, bit = 0x10000uL;
        xRem = (uint32_t)( ( x >= 0 ) ? x : -x );
        xDiv = (uint32_t)( ( y >= 0 ) ? y : -y );

        while ( xDiv < xRem ) {
            xDiv <<= 1;
            bit <<= 1;
        }
        RetValue = QFP16_OVERFLOW;
        /*cstat -MISRAC2012-Rule-14.3_a*/
        if ( 0uL != bit ) { /*MISRAC2012-Rule-14.3_a false positive*/
        /*cstat +MISRAC2012-Rule-14.3_a*/  
            uint32_t quotient = 0uL;
            if ( 0uL != ( xDiv & 0x80000000uL ) ) {
                if ( xRem >= xDiv ) {
                    quotient |= bit;
                    xRem -= xDiv;
                }
                xDiv >>= 1;
                bit >>= 1;
            }

            while ( ( 0uL != bit ) && ( 0uL != xRem ) ) {
                if ( xRem >= xDiv ) {
                    quotient |= bit;
                    xRem -= xDiv;
                }
                xRem <<= 1;
                bit >>= 1;
            }	 
            if ( 1u == fp->rounding ) {
                if ( xRem >= xDiv ) {
                    quotient++;
                }
            }

            RetValue = (qFP16_t)quotient;

            if ( 0uL != ( (uint32_t)( x ^ y ) & 0x80000000uL ) ) {
                if ( quotient == (uint32_t)fp->min ) {
                    RetValue = QFP16_OVERFLOW;
                }
                else{
                    RetValue = -RetValue;
                }
            }
        }
    }
    return qFP16_Saturate( RetValue, x, y );    
}
/*============================================================================*/
qFP16_t qFP16_Mod( qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue = 0;
    if ( 0 != y ) {
      RetValue = x % y;
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Sqrt( qFP16_t x )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
   
    if( x > 0 ) {
        uint32_t bit;
        uint8_t  n;

        RetValue = 0;
        bit = ( 0 != ( x & (qFP16_t)0xFFF00000uL ) )? (uint32_t)( 1 << 30 ) : (uint32_t)( 1 << 18 );
        while ( bit > (uint32_t)x ) {
            bit >>= 2;
        }
            
       	for ( n = 0u ; n < 2u ; ++n ) {
            while ( 0u != bit ) {
                if ( x >= (qFP16_t)( (uint32_t)RetValue + bit ) ) {
                    x -= (qFP16_t)( (uint32_t)RetValue + bit );
                    RetValue = (qFP16_t)( ( (uint32_t)RetValue >> 1uL ) + bit );
                }
                else {
                    RetValue = ( RetValue >> 1 );
                }
                bit >>= 2;
            }
		
            if ( 0u == n ) {
                if ( x > 65535 ) {
                    x -= RetValue;
                    x = ( x << 16 ) - QFP16_1_DIV_2;
                    RetValue = ( RetValue << 16 ) + QFP16_1_DIV_2;
                }
                else {
                    x <<= 16;
                    RetValue <<= 16;
                }
                bit = 1 << 14;
            }
        }  
    }
    if( ( 1u == fp->rounding ) && ( x > RetValue ) ) {
            RetValue++;    
    }
    return (qFP16_t)RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Exp( qFP16_t x )
{
    qFP16_t RetValue, term;
    uint8_t isNegative;
    int i;
    
    if ( 0 == x ) {
        RetValue = QFP16_1;
    }
    else if ( x == QFP16_1 ) {
        RetValue = QFP16_E;
    }
    else if ( x >= QFP16_EXP_MAX ) {
        RetValue =  fp->max;
    }  
    else if ( x <= QFP16_EXP_MIN ) {
        RetValue = 0;
    }
    else {
        isNegative = (uint8_t)( x < 0 );
        if ( 1u == isNegative ) {
            x = -x;
        }
        
        RetValue = x + QFP16_1;
        term = x;
       
        for ( i = 2; i < 30; ++i ) {
            term = qFP16_Mul( term, qFP16_Div( x, qFP16_IntToFP( i ) ) );
            RetValue += term;

            if ( ( term < 500 ) && ( (i > 15) || ( term < 20 ) ) ) {
                break;
            }	
        }

        if ( 1u == isNegative ){
            RetValue = qFP16_Div( QFP16_1, RetValue );  
        }
    }
    return RetValue; 
}
/*============================================================================*/
qFP16_t qFP16_Log( qFP16_t x )
{
    qFP16_t RetValue = (qFP16_t)QFP16_OVERFLOW;
    
    if ( x > 0 ) {
        qFP16_t guess = QFP16_2, delta, e;
        int scaling = 0, count = 0;
        
        while ( x > QFP16_100 ){
            x = qFP16_Div( x, QFP16_E4 );
            scaling += 4;
        }

        while ( x < QFP16_1 ) {
            x = qFP16_Mul(x, QFP16_E4 );
            scaling -= 4;
        }

        do {
            e = qFP16_Exp( guess );
            delta = qFP16_Div( x - e , e );

            if ( delta > QFP16_3 ) {
                delta = QFP16_3;
            }
            guess += delta;
        } while ( ( count++ < 10 ) && ( ( delta > 1 ) || ( delta < -1 ) ) );

        RetValue = guess + qFP16_IntToFP( scaling );
    } 
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Log2( qFP16_t x )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
    qFP16_t inv;
    if ( x > 0 ) {
        if ( x < QFP16_1 ) {
            if ( 1 == x ) {
                RetValue = QFP16_N16;
            }
            else {
                inv = qFP16_Div( QFP16_1, x );
                RetValue = -qFP16_log2i( inv );   
            }
        }        
        else {
            RetValue = qFP16_log2i(x);
        }
    }
    if ( 1u == fp->saturate ) {
        if ( QFP16_OVERFLOW == RetValue ) {
            RetValue = fp->min;
        }
    }
    
    return RetValue;
}
/*============================================================================*/
qFP16_t fp16_RadToDeg( qFP16_t x )
{
    return qFP16_Mul( qFP16_WrapToPi( x ), QFP16_180_DIV_PI );
}
/*============================================================================*/
qFP16_t fp16_DegToRad( qFP16_t x )
{
    return qFP16_Mul( qFP16_WrapTo180(x), QFP16_PI_DIV_180 );
}
/*============================================================================*/
qFP16_t qFP16_WrapToPi( qFP16_t x )
{
    if ( ( x < -QFP16_PI ) || ( x > QFP16_PI ) ) {
        while ( x > QFP16_PI ) {
            x -= QFP16_2PI;
        }
        while ( x <= -QFP16_PI ) {
            x += QFP16_2PI;
        }
    }
    return x;
}
/*============================================================================*/
qFP16_t qFP16_WrapTo180( qFP16_t x )
{
    if ( ( x < -QFP16_180 ) || ( x > QFP16_180 ) ) {
        while ( x > QFP16_180 ) {
            x -= QFP16_360;
        }
        while ( x <= -QFP16_PI ) {
            x += QFP16_360;
        }
    }
    return x;
}
/*============================================================================*/
qFP16_t qFP16_Sin( qFP16_t x )
{
    qFP16_t RetValue, x2;
    
    x = qFP16_WrapToPi( x );
    x2 = qFP16_Mul( x ,x );

    RetValue = x;
    x = qFP16_Mul( x, x2 );
    RetValue -= ( x / 6 );
    x = qFP16_Mul( x, x2 );
    RetValue += ( x / 120 );
    x = qFP16_Mul( x, x2 );
    RetValue -= ( x / 5040 );
    x = qFP16_Mul( x, x2 );
    RetValue += ( x / 362880 );
    x = qFP16_Mul( x, x2);
    RetValue -= ( x / 39916800 );

    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Cos( qFP16_t x )
{
    return qFP16_Sin( x + QFP16_PI_DIV_2 );
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
qFP16_t qFP16_Atan2( qFP16_t y , qFP16_t x )
{
    qFP16_t absY, mask, angle, r, r_3;

    mask = ( y >> ( sizeof(qFP16_t)*7u ) );
    absY = ( y + mask ) ^ mask;
    if ( x >= 0 ) {
        r = qFP16_Div( ( x - absY ), ( x + absY ) );
        angle = QFP16_PI_DIV_4;
    } 
    else {
        r = qFP16_Div( ( x + absY ), ( absY - x ) );
        angle = QFP16_3PI_DIV_4;
    }
    r_3 = qFP16_Mul( qFP16_Mul( r, r ), r );
    angle += qFP16_Mul( 0x00003240, r_3 ) - qFP16_Mul( 0x0000FB50, r );
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
    return qFP16_Atan2( x, QFP16_1 );
}
/*============================================================================*/
qFP16_t qFP16_Asin( qFP16_t x )
{
    qFP16_t RetValue = 0;
    if ( ( x <= QFP16_1 ) && ( x >= -QFP16_1 ) ) {
        RetValue = QFP16_1 - qFP16_Mul( x, x );
        RetValue = qFP16_Div( x, qFP16_Sqrt( RetValue ) );
        RetValue = qFP16_Atan( RetValue ); 
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Acos( qFP16_t x )
{
    return ( QFP16_PI_DIV_2 - qFP16_Asin( x ) );
}
/*============================================================================*/
qFP16_t qFP16_Cosh( qFP16_t x )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
    qFP16_t epx, enx;
    
    if ( 0 == x ) {
        RetValue = QFP16_1;
    }
    else if ( ( x >= QFP16_EXP_MAX ) || ( x <= QFP16_EXP_MIN ) ) {
        RetValue = fp->max;
    }  
    else {
        epx = qFP16_Exp( x );
        enx = qFP16_Exp( -x );
        if( ( QFP16_OVERFLOW != epx ) && ( QFP16_OVERFLOW != enx ) ) {
            RetValue = epx + enx;
            RetValue = ( RetValue >> 1 );
        }        
    }

    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Sinh( qFP16_t x )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
    qFP16_t epx, enx;
    
    if ( 0 == x ) {
        RetValue = QFP16_1;
    }
    else if ( x >= QFP16_EXP_MAX ) {
        RetValue = fp->max;
    }  
    else if ( x <= QFP16_EXP_MIN ) {
        RetValue = -fp->max;
    }
    else {
        epx = qFP16_Exp( x );
        enx = qFP16_Exp( -x );
        if ( ( QFP16_OVERFLOW != epx ) && ( QFP16_OVERFLOW != enx ) ) {
            RetValue = epx - enx;
            RetValue = ( RetValue >> 1 );
        }        
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Tanh( qFP16_t x )
{
    qFP16_t RetValue, epx, enx;
    
    if( 0 == x ) {
        RetValue = 0;
    }
    else if ( x >  QFP16_6_5 ) { /* tanh for any x>6.5 ~= 1*/
        RetValue = QFP16_1;
    }
    else if ( x < -QFP16_6_5 ) { /* tanh for any x<6.5 ~= -1*/
        RetValue = -QFP16_1;
    }
    else {
        RetValue = qFP16_Abs( x );
        epx = qFP16_Exp( RetValue ); 
        enx = qFP16_Exp( -RetValue );    
        RetValue = qFP16_Div( epx - enx, epx + enx );
        RetValue = ( x > 0 ) ? RetValue : -RetValue;
    }
    
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Polyval( qFP16_t *p, size_t n, qFP16_t x )
{
    qFP16_t fx, tmp;
    int32_t i;
    /*polynomial evaluation using Horner's method*/
    fx = p[ 0 ];
    for ( i = 1 ; i < (int32_t)n ; ++i ) {
        tmp = qFP16_Mul( fx, x ); 
        if ( QFP16_OVERFLOW == tmp ) {
            fx = QFP16_OVERFLOW;
            break;
        }
        fx =  qFP16_Add( tmp , p[ i ] );
    }   
    return fx;  
}
/*============================================================================*/
qFP16_t qFP16_IPow( qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue;
    qFP16_t n;
    int32_t i;
    
    RetValue = QFP16_1;
    n = y >> 16;
    if ( 0 == n ) {
        RetValue = QFP16_1;
    }
    else if ( QFP16_1 == n ) {
        RetValue = x;
    }
    else {
        for ( i = 0; i < n; ++i ) {
            RetValue = qFP16_Mul( x, RetValue );
            if ( QFP16_OVERFLOW == RetValue ) {
                break;
            }
        }
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_Pow( qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue = QFP16_OVERFLOW;
    qFP16_t tmp;
    
    if ( ( 0uL == ( (uint32_t)y & 0x0000FFFFuL ) ) && ( y > 0 ) ) { /*handle integer exponent explicitly*/
        RetValue = qFP16_IPow( x, y );
    }
    else{
        tmp = qFP16_Mul( y, qFP16_Log( qFP16_Abs( x ) ) );
        if ( QFP16_OVERFLOW != tmp ) {
            RetValue = qFP16_Exp( tmp );
            if ( x < 0 ) {
                RetValue = -RetValue;
            }
        }        
    }        

    return RetValue;
}
/*============================================================================*/
char* qFP16_FPToA( qFP16_t num, char *str, int decimals )
{
    char *RetValue = str;
    
    if( QFP16_OVERFLOW == num ) {
        str[0] = 'o';
        str[1] = 'v';
        str[2] = 'e';
        str[3] = 'r';
        str[4] = 'f';
        str[5] = 'l';
        str[6] = 'o';
        str[7] = 'w';
        str[8] = '\0';
    }
    else {
        const uint32_t itoa_scales[ 6 ] = { 1uL, 10uL, 100uL, 1000uL, 10000uL, 100000uL };
        uint32_t uValue, fPart, scale;
        int32_t iPart;
        
        uValue = (uint32_t)( ( num >= 0 ) ? num : -num );
        if ( num < 0 ) {
            *str++ = '-';
        }

        iPart = (int32_t)( uValue >> 16 );
        fPart = uValue & 0xFFFFuL;
        if ( decimals > 5 ) {
            decimals = 5;
        }
        if ( decimals < 0 ) {
            decimals = 0;
        }
        scale = itoa_scales[ decimals ];
        fPart = (uint32_t)qFP16_Mul( (qFP16_t)fPart, (qFP16_t)scale );

        if ( fPart >= scale ) {
            iPart++;
            fPart -= scale;    
        }
        str = qFP16_itoa( str, 10000, (uint32_t)iPart, 1u );

        if ( 1u != scale ) {
            *str++ = '.';
            str = qFP16_itoa( str, scale / 10U, fPart, 0u );
        }
        *str = '\0';        
    }
    return RetValue;
}
/*============================================================================*/
qFP16_t qFP16_AToFP(char *s)
{
    uint8_t negative;
    uint32_t iPart = 0uL, fPart = 0uL, scale = 1uL, digit;
    int32_t count = 0;
    qFP16_t RetValue = QFP16_OVERFLOW;
    int point_seen = 0, overflow = 0;
    char c;
    
    /*cstat -MISRAC2012-Dir-4.11_h*/
    while ( 0 != isspace( (int)*s) ) {
        s++; /*discard whitespaces*/
    }
    
    negative = (uint8_t)( '-' == *s );
    if ( ( '+' == *s )  || ( '-' == *s ) ) {
        s++; /*move to the next sign*/
    }

    for ( c = s[0] ; '\0' != c ;  c = s[0] ) {
        if ( '.' == c ) {
            point_seen = 1;
        }
        else if ( 0 != isdigit( (int)c ) ) {
            digit = (uint32_t)c - (uint32_t)'0';
            if ( 1 == point_seen ) { /* Decode the fractional part */
                scale *= 10u;
                fPart *= 10u;
                fPart += digit;
            }
            else { /* Decode the decimal part */
                iPart *= 10u;
                iPart += digit;
                count++;        
                overflow = (int)( ( 0 == count ) || ( count > 5 ) || ( iPart > 32768uL ) || ( ( 0u == negative ) && ( iPart  > 32767uL ) ) );
                if ( 1 == overflow ){
                    break;
                }
            }
        }
        else{
            break;
        }
        s++;
    }
    if( 0 == overflow ){
        RetValue = (qFP16_t)iPart << 16;
        RetValue += qFP16_Div( (qFP16_t)fPart, (qFP16_t)scale );
        RetValue = ( 1u == negative )? -RetValue : RetValue;
    }   
    /*cstat +MISRAC2012-Dir-4.11_h*/
    return RetValue;
}
/*============================================================================*/
static uint32_t qFP16_OverflowCheck( uint32_t res, uint32_t x, uint32_t y )
{
    if ( ( 0uL == ( ( x ^ y ) & 0x80000000uL ) ) && ( 0uL != ( ( x ^ res ) & 0x80000000uL ) ) ) {
        res =  (uint32_t)QFP16_OVERFLOW;
    }    
    return res;
}
/*============================================================================*/
static qFP16_t qFP16_rs( qFP16_t x )
{
    qFP16_t RetValue;
    if ( 1u == fp->rounding ) {
        RetValue = ( x >> 1u ) + ( x & 1 );
    }
    else {
        RetValue = x >> 1;
    }
    return RetValue;
}
/*============================================================================*/
static qFP16_t qFP16_log2i( qFP16_t x )
{
    qFP16_t RetValue = 0;

    while ( x >= QFP16_2 ) {
        RetValue++;
        x = qFP16_rs( x );
    }

    if ( 0 == x ) {
        RetValue = RetValue << 16;
    }
    else {
        int i;
        for ( i = 16 ; i > 0 ; --i ) {
            x = qFP16_Mul( x, x );
            RetValue <<= 1;
            if ( x >= QFP16_2 ) {
                RetValue |= 1;
                x = qFP16_rs( x );
            }
        }
        if ( 1u == fp->rounding ) {
            x = qFP16_Mul( x, x );
            if ( x >= QFP16_2 ) {
                RetValue++;
            }
        }        
    }

    return RetValue;   
}
/*============================================================================*/
static char *qFP16_itoa( char *buf, uint32_t scale, uint32_t value, uint8_t skip )
{
    uint32_t digit;  
    while ( 0u != scale ) {
        digit = ( value / scale );
        if ( ( 0u == skip ) || ( 0u != digit ) || ( 1u == scale ) ) {
            skip = 0u;
            /*cstat -MISRAC2012-Rule-10.2 -MISRAC2012-Rule-10.3*/
            *buf++ = (char)'0' + (char)digit;
            /*cstat +MISRAC2012-Rule-10.2 +MISRAC2012-Rule-10.3*/
            value %= scale;
        }
        scale /= 10u;
    }
    return buf;
}
/*============================================================================*/
static qFP16_t qFP16_Saturate( qFP16_t nsInput, qFP16_t x, qFP16_t y )
{
    qFP16_t RetValue = nsInput;
    if ( 1u == fp->saturate ) {
        if ( QFP16_OVERFLOW == nsInput ) {
            RetValue = ( ( x >= 0 ) == ( y >= 0 ) )? fp->max : fp->min;
        }
    }      
    return RetValue;
}
/*============================================================================*/
/*cstat +MISRAC2012-Rule-10.8 +CERT-FLP34-C +MISRAC2012-Rule-1.3_n +MISRAC2012-Rule-10.1_R6 +ATH-shift-neg +CERT-INT34-C_c*/