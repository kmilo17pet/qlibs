/*!
 * @file qfp16.h
 * @author J. Camilo Gomez C.
 * @version 1.04
 * @note This file is part of the qTools distribution.
 * @brief Fixed-Point math Q16.16 with rounding and saturated arithmetic .
 **/

#ifndef QFP16_H
#define QFP16_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <stdlib.h>
    #include <ctype.h>

    typedef int32_t qFP16_t;

    #define QFP16_EPSILON             (  1 ) 
    #define QFP16_MIN                 ( -2147483647 ) /* -32767.99998 */
    #define QFP16_MAX                 (  2147483647 ) /* +32767.99998 */
    #define QFP16_OVERFLOW            ( (qFP16_t)( 0x80000000 ) ) /* overflow */
    
    #define QFP16_PI                  (  205887  )    /* pi */
    #define QFP16_2PI                 (  411775 )     /* 2*pi */ 
    #define QFP16_NPI                 ( -205887 )     /* -pi */
    #define QFP16_1_DIV_PI            (  20861 )      /* 1/pi */
    #define QFP16_E                   (  178145 )     /* e */
    #define QFP16_E4                  (  3578144 )    /* e^4 */
    #define QFP16_P4_DIV_PI           (  83443 )      /* 4/pi */
    #define QFP16_N4_DIV_PI           ( -83443 )      /* -4/pi */
    #define QFP16_PI_DIV_2            (  102944 )     /* pi/2 */
    #define QFP16_PI_DIV_4            (  51471 )      /* pi/4 */
    #define QFP16_3PI_DIV_4           (  154415 )     /* 3*pi/4 */      
    #define QFP16_1_DIV_2             (  32768 )      /* 1/2 */
    #define QFP16_LN2                 (  45426 )      /* log(2) */
    #define QFP16_LN10                (  150902 )     /* log(10) */
    #define QFP16_SQRT2               (  92682 )      /* sqrt(2) */
    #define QFP16_180_DIV_PI          (  3754936 )    /* 180/pi */
    #define QFP16_PI_DIV_180          (  1144 )       /* pi/180 */
    #define QFP16_180                 (  11796480 )   /* 180 */
    #define QFP16_360                 (  23592960 )   /* 360 */
    #define QFP16_EXP_MAX             (  681391 )     /* max value for qFP16_Exp function*/
    #define QFP16_EXP_MIN             ( -681391 )     /* min value for qFP16_Exp function*/
    /*used only for internal operations*/
    #define QFP16_1                   (  65536 )      /* 1 */
    #define QFP16_2                   (  131072 )     /* 2 */
    #define QFP16_3                   (  196608 )     /* 3 */
    #define QFP16_N16                 ( -1048576 )    /* -16 */
    #define QFP16_100                 (  6553600 )    /* 100 */

    #define qFP16_Constant(x)      ( (fp_t)(((x) >= 0) ? ((x) * 65536.0 + 0.5) : ((x) * 65536.0 - 0.5)) )


    typedef struct
    {
        qFP16_t min, max;
        uint8_t rounding, saturate;
    }qFP16_Settings_t;

    int qFP16_SettingsSet( qFP16_Settings_t *instance, qFP16_t min, qFP16_t max, uint8_t rounding, uint8_t saturate );
    void qFP16_SettingsSelect( qFP16_Settings_t *instance );
    
    int qFP16_FP2Int( qFP16_t x );
    qFP16_t qFP16_Int2FP( int x );
    qFP16_t qFP16_Float2FP( float x );
    float qFP16_FP2Float( qFP16_t x );
    
    qFP16_t qFP16_Double2FP( double x );
    double qFP16_FP2Double( qFP16_t x );    
    
    qFP16_t qFP16_Abs( qFP16_t x );
    qFP16_t qFP16_Floor( qFP16_t x );
    qFP16_t qFP16_Ceil( qFP16_t x );
    qFP16_t qFP16_Round( qFP16_t x );
    
    qFP16_t qFP16_Add( qFP16_t X, qFP16_t Y );
    qFP16_t qFP16_Sub( qFP16_t X, qFP16_t Y );
    qFP16_t qFP16_Mul( qFP16_t x, qFP16_t y );
    qFP16_t qFP16_Div( qFP16_t x, qFP16_t y );
    qFP16_t qFP16_Mod( qFP16_t x, qFP16_t y );
    
    qFP16_t qFP16_Sqrt( qFP16_t x );
    qFP16_t qFP16_Exp( qFP16_t x );
    qFP16_t qFP16_Log( qFP16_t x );
    qFP16_t qFP16_Log2( qFP16_t x );
    
    qFP16_t fp16_Rad2Deg( qFP16_t x );
    qFP16_t fp16_Deg2Rad( qFP16_t x );
    
    qFP16_t qFP16_WrapToPi( qFP16_t x );
    qFP16_t qFP16_WrapTo180( qFP16_t x );
    qFP16_t qFP16_Sin( qFP16_t x );
    qFP16_t qFP16_Cos( qFP16_t x );
    qFP16_t qFP16_Tan( qFP16_t x );
    qFP16_t qFP16_Atan2( qFP16_t y , qFP16_t x );
    qFP16_t qFP16_Atan( qFP16_t x );
    qFP16_t qFP16_Asin( qFP16_t x );
    qFP16_t qFP16_Acos( qFP16_t x );
    qFP16_t qFP16_Cosh( qFP16_t x );
    qFP16_t qFP16_Sinh( qFP16_t x );
    qFP16_t qFP16_Tanh( qFP16_t x );
    
    qFP16_t qFP16_Polyval( qFP16_t *p, size_t n, qFP16_t x );
    
    qFP16_t qFP16_IPow( qFP16_t x, qFP16_t y );
    qFP16_t qFP16_Pow( qFP16_t x, qFP16_t y );

    char* qFP16_FPtoA( qFP16_t num, char *str, int decimals );
    qFP16_t qFP16_AtoFP(char *s);

#ifdef __cplusplus
}
#endif

#endif