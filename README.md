[![Built for](https://img.shields.io/badge/built%20for-microcontrollers-lightgrey?logo=WhiteSource)](https://github.com/kmilo17pet/qTools)
[![CodeFactor](https://www.codefactor.io/repository/github/kmilo17pet/qlibs/badge)](https://www.codefactor.io/repository/github/kmilo17pet/qlibs)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/14d566939d2e4d4181088cc1c6666fa3)](https://www.codacy.com/gh/kmilo17pet/qTools/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kmilo17pet/qTools&amp;utm_campaign=Badge_Grade)
[![CodeInspectorScore](https://api.codiga.io/project/27197/score/svg)](https://app.codiga.io/project/27197/dashboard)
[![CodeInspectorGrade](https://api.codiga.io/project/27197/status/svg)](https://app.codiga.io/project/27197/dashboard)
[![Softacheck](https://softacheck.com/app/repository/kmilo17pet/qlibs/badge)](https://softacheck.com/app/repository/kmilo17pet/qlibs/issues)
[![Documentation](https://softacheck.com/app/repository/kmilo17pet/qlibs/documentation/badge)](https://softacheck.com/app/docs/kmilo17pet/qlibs/)
[![MISRAC2012](https://img.shields.io/badge/MISRAC2012-Compliant-blue.svg?logo=c)](https://en.wikipedia.org/wiki/MISRA_C)
[![CERT](https://img.shields.io/badge/CERT-Compliant-blue.svg?logo=c)](https://wiki.sei.cmu.edu/confluence/display/seccode/SEI+CERT+Coding+Standards)
[![C Standard](https://img.shields.io/badge/STD-C99-green.svg?logo=c)](https://en.wikipedia.org/wiki/C99)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/kmilo17pet/qTools/graphs/commit-activity)
[![License](https://img.shields.io/github/license/kmilo17pet/qTools)](https://github.com/kmilo17pet/qTools/blob/main/LICENSE) 

# qlibs : A collection of useful libraries for embedded systems

<details><summary>qFP16 : Q16.16 Fixed-point math</summary>
    
  - Basic operations
  - Trigonometric functions
  - Exponential functions
</details>

<details><summary>qSSmoother : Filters to smooth noisy signals</summary>
    
  - `LPF1`: _Low Pass Filter Order 1_
  - `LPF2`: _Low Pass Filter Order 2_
  - `MWM1`: _Moving Window Median O(n)_
  - `MWM2`: _Moving Window Median O(1): With TDL(works efficient for large windows)_
  - `MOR1`: _Moving Outlier Removal O(n)_
  - `MOR2`: _Moving Outlier Removal O(1): With TDL(works efficient for large windows)_
  - `GMWF`: _Gaussian filter_
  - `KLMN`: _Scalar Kalman filter_
  - `EXPW`: _Exponential weighting filter_
</details>

<details><summary>qPID : Closed Loop PID Controller</summary>
    
  - Derivative filter
  - Anti-Windup
  - Tracking Mode
  - Auto-tunning 
  - Additive MRAC
</details>

<details><summary>qLTISys : Recursive LTI systems evaluation by transfer functions</summary>
    
  - Continuous
  - Discrete
</details>

<details><summary>qCRC : Generic Cyclic Redundancy Check (CRC) calculator</summary>
    
  - CRC8
  - CRC16
  - CRC32
</details>

- qFIS : Fuzzy Inference System ( Mamdani supported only )
- qBitField: A bit-field manipulation library
- qTDL : Tapped Delay Line in O(1). 
- qRMS : Recursive Root Mean Square(RMS) calculation of a signal.

## Draft examples

### Working with a bitfield
```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "qbitfield.h"

int main( int argc, char *argv[] ) 
{
    qBitField_t vPort; /*create the bitfield instance*/
    uint8_t vPortArea[ QBITFIELD_SIZE(48) ] = { 0 }; /*Create the bitfield storage area to hold 48bits*/
    uint16_t rWord; 
    
    qBitField_Setup( &vPort, vPortArea, sizeof(vPortArea) );
    qBitField_ClearAll( &vPort);
    /*we are going to write the following value in the bitfield = 0x8177AA55FF88*/
    qBitField_WriteUINTn( &vPort, 0, 0xAA55FF88, 32 ); /*write the first 32 bits*/
    qBitField_WriteUINTn( &vPort, 32, 0x77, 8 );    /*write one of the last two bytes*/
    qBitField_WriteBit( &vPort, 47, 1 );           /*write the last bit of the last byte*/
    qBitField_WriteBit( &vPort, 40, 1 );          /*write the first bit of the last byte*/
    rWord = (uint16_t)qBitField_ReadUINTn( &vPort, 20, 16 );   /*read a word at offset 24*/
    printf("%02X %02X %02X %02X %02X %02X\r\n", vPortArea[0], vPortArea[1], vPortArea[2], vPortArea[3], vPortArea[4], vPortArea[5]);
    return EXIT_SUCCESS;
}
```


### A simple fixed-point calculation

![ec1](https://latex.codecogs.com/svg.latex?\Large&space;x=\frac{-b+\sqrt{b^2-4ac}}{2a}) 


```c
#include <stdio.h>
#include <stdlib.h>
#include "qfp16.h"

int main( int argc, char *argv[] ) 
{
    qFP16_t a = qFP16_Constant( 1.5 );
    qFP16_t b = qFP16_Constant( 5.2 );
    qFP16_t c = qFP16_Constant( 4.0 );
    qFP16_t tmp;
    char ans[ 10 ];
    
    tmp = qFP16_Mul( qFP16_IntToFP( 4 ), qFP16_Mul( a, c ) );
    tmp = qFP16_Add( -b, qFP16_Sqrt( qFP16_Sub( qFP16_Mul( b, b ), tmp  ) ) );
    tmp = qFP16_Div( tmp, qFP16_Mul( qFP16_IntToFP( 2 ), a ) );
    printf( " result = %s \r\n" , qFP16_FPToA( tmp, ans, 4 ) );
    return EXIT_SUCCESS;
}
```

### Simulating a 3rd-order continuous transfer function

![ec2](https://latex.codecogs.com/svg.latex?\Large&space;G_p(s)=\frac{2s^2+3s+6}{s^3+6s^2+11s+16}) 


```c
#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"
#include "qltisys.h"

#define SYS_ORDER   ( 3 )
const TickType_t  dt = 50; /*50mS time-step*/
qLTISys_t system;
float num[ SYS_ORDER+1 ] = { 0.0f, 2.0f, 3.0f, 6.0f };
float den[ SYS_ORDER+1 ] = { 1.0f, 6.0f, 11.0f, 16.0f };
qLTISys_ContinuosX_t x[ SYS_ORDER ] = { { { 0.0f } }, { { 0.0f } },{ { 0.0f } } };

void xTaskSystemSimulate( void *arg )
{
    qLTISys_Setup( &system, num, den, x, 0, SYS_ORDER+1, (float)dt/1000.0f ) );
    float ut, yt;
    for( ;; ) {
        ut = BSP_InputGet();
        yt = qLTISys_Excite( &system, ut );
        vTaskDelay( dt / portTICK_RATE_MS) ;
        printf( "u(t) = %0.2f   y(t)  = %0.2f \r\n", ut, yt );
    }
}
```

### Simulating a 2nd-order discrete transfer function

![ec2](https://latex.codecogs.com/svg.latex?\Large&space;G_p(z^{-1})=\frac{0.1+0.2z^{-1}+0.3z^{-2}}{1-0.85z^{-1}+0.02z^{-2}}) 


```c
#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"
#include "qltisys.h"

#define NB  ( 3 )
#define NA  ( 2 )
const TickType_t  Ts = 100; /*100mS sample time*/
qLTISys_t system;
float num[ NB ] = { 0.1f, 0.2f, 0.3f };
float den[ NA+1 ] = { 1.0f, -0.85f, 0.02};
qLTISys_DiscreteX_t xk[ NB ] = { 0.0f, 0.0f, 0.0f };

void xTaskSystemSimulate( void *arg )
{
    qLTISys_Setup( &system, num, den, xk, NB, NA, QLTISYS_DISCRETE );
    float uk, yk;
    for( ;; ) {
        uk = BSP_InputGet();
        yk = qLTISys_Excite( &system, uk );
        vTaskDelay( Ts / portTICK_RATE_MS) ;
        printf( "u(k) = %0.2f   y(k)  = %0.2f \r\n", uk, yk );
    }
}
```

### A speed control using the PID algorithm 

```c
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp.h"
#include "qpid.h"

const TickType_t  dt = 50; /*50mS time-step*/
void xTaskPIDspeedControl( void *arg );

void xTaskPIDspeedControl( void *arg )
{
    qPID_controller_t *controller = (qPID_controller_t *)arg;
    float processMeasurement;
    float controlOutput;
    for ( ;; ) {
        processMeasurement = BSP_ScaletoSpeed ( BSP_AnalogRead( BSP_AI_SPEED_CHANNEL ) );
        controlOutput = qPID_Control( controller, processMeasurement );
        BSP_PWMSet( BSP_AO_SPEED_PWM_CHANNEL, BSP_ScaletoPWM( controlOutput ) ); 
        vTaskDelay( dt / portTICK_RATE_MS) ;
    }
}

int main( int argc, char *argv[] ) 
{
    qPID_controller_t speedControl;
    int ret;
    
    BSP_SystemInit( );
    ret = qPID_Setup( &speedControl, 1, 0.1, 0, (float)dt/1000.0f );
    if ( 0 == ret ) {
        puts( "ERROR: Cant configure PID controller" );
    }
    qPID_SetSaturation( &speedControl, 0.0f, 100.0f );
    /* Create the task that handles the speed control. */
    xTaskCreate( xTaskPIDspeedControl, "speedControl", 1024, &speedControl, configMAX_PRIORITIES - 1 ,NULL );
    vTaskStartScheduler();
    for( ;; );
    return EXIT_SUCCESS;
}
```

### Smoothing a noisy signal by using a gaussian filter
```c
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp.h"
#include "qssmoother.h"
#include "AppSignalProcessing.h"

#define SMOOTHER_WINDOW_SIZE    ( 10 )
#define SMOOTHER_SAMPLE_TIME    ( 100 )

void xTaskSignalProcessing( void *arg )
{
    qSSmoother_GMWF_t *smoother = (qSSmoother_GMWF_t *)arg;
    float noisySignal;
    float smoothedSignal;
    for ( ;; ) {
        noisySignal = BSP_ScaletoSomething( BSP_AnalogRead( BSP_AI_SPEED_CHANNEL ) );
        smoothedSignal = qSSmoother_Perform( smoother, noisySignal );
        AppSignalProcess( smoothedSignal );
        vTaskDelay( SMOOTHER_SAMPLE_TIME / portTICK_RATE_MS ) ;
    }
}

int main( int argc, char *argv[] ) 
{
    qSSmoother_GMWF_t smoother;
    float win_kernel[ 2*SMOOTHER_WINDOW_SIZE ]; /*storage for the window and the kernel*/
    float params[ 2 ] = { 0.5f, SMOOTHER_WINDOW_SIZE/2.0f }; /*sigma and offset*/
    int ret;
    
    BSP_SystemInit( );
    ret = qSSmoother_Setup( &smoother, QSSMOOTHER_TYPE_GMWF, params, win_kernel, sizeof(win_kernel)/sizeof(win_kernel[0]) );
    if ( 0 == ret ) {
        puts( "ERROR: Cant configure the smoother filter" );
    }
    xTaskCreate( xTaskSignalProcessing, "signalProcessing", 512, &smoother, 2, NULL );
    vTaskStartScheduler();
    for( ;; );
    return EXIT_SUCCESS;
}
```


### An Mamdani Fuzzy Inference System example 
FlexNav system taken from here: https://www.researchgate.net/publication/3955309_FLEXnav_fuzzy_logic_expert_rule-based_position_estimation_for_mobile_robots_on_rugged_terrain


```c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "qfis.h"

int main( int argc, char *argv[] ) 
{
    /* FIS Object */
    qFIS_t flexnav;
    /* I/O Fuzzy Objects */
    qFIS_IO_t flexnav_inputs[4], flexnav_outputs[2];
    /* I/O Membership Objects */
    qFIS_MF_t MFin[12], MFout[6];
    /*  I/O Names */
    enum { wt, dax, day, ae };
    /*  I/O Membership functions tags */
    enum { phit, thetat };
    enum { wtSLOW, wtMED, wtFAST, daxLOW, daxMED, daxHIGH, dayLOW, dayMED, dayHIGH, aeLOW, aeMED, aeHIGH };
    enum { phitGYRO, phitBOTH, phitACCEL, thetatGYRO, thetatBOTH, thetatACCEL }; 

    static const qFIS_Rules_t rules[] = {
        QFIS_RULES_BEGIN
            IF wt IS_NOT wtSLOW THEN phit IS phitGYRO AND thetat IS thetatGYRO END
            IF dax IS daxHIGH THEN thetat IS thetatGYRO END
            IF day IS dayHIGH THEN thetat IS thetatGYRO END
            IF ae IS aeHIGH THEN phit IS phitGYRO AND thetat IS thetatGYRO END
            IF wt IS wtSLOW AND dax IS daxLOW AND ae IS aeLOW THEN phit IS phitACCEL END
            IF wt IS wtSLOW AND day IS dayLOW AND ae IS aeLOW THEN thetat IS thetatACCEL END
            IF wt IS wtSLOW AND dax IS daxLOW AND ae IS aeMED THEN phit IS phitBOTH END
            IF wt IS wtSLOW AND day IS dayLOW AND ae IS aeMED THEN thetat IS thetatBOTH END
            IF wt IS wtSLOW AND dax IS daxMED AND ae IS aeLOW THEN phit IS phitBOTH END
            IF wt IS wtSLOW AND day IS dayMED AND ae IS aeLOW THEN thetat IS thetatBOTH END
            IF wt IS wtMED AND dax IS daxLOW AND ae IS aeLOW THEN phit IS phitBOTH END
            IF wt IS wtMED AND day IS dayLOW AND ae IS aeLOW THEN thetat IS thetatBOTH END
            IF wt IS wtMED AND dax IS_NOT daxLOW THEN phit IS phitGYRO END
            IF wt IS wtMED AND day IS_NOT dayLOW THEN thetat IS thetatGYRO END
            IF wt IS wtMED AND ae IS_NOT aeLOW THEN phit IS phitGYRO AND thetat IS thetatGYRO END
        QFIS_RULES_END
    };

    /* Add membership functions to the inputs */
    const float wtSLOW_p[] = {-0.2f ,0.0f ,0.2f};
    const float wtMED_p[] = {0.1f ,0.25f ,0.4f};
    const float wtFAST_p[] = {0.3f ,0.5f ,0.7f};
    const float daxLOW_p[] = {-1.0f ,0.0f ,2.0f};
    const float daxMED_p[] = { 1.0f ,2.5f ,4.0f};
    const float daxHIGH_p[] = {3.0f ,5.0f ,7.0f};
    const float dayLOW_p[] = {-2.0f ,0.0f ,2.0f};
    const float dayMED_p[] = { 1.0f ,2.5f ,4.0f};
    const float dayHIGH_s[] = { 3.0f ,5.0f ,7.0f};
    const float aeLOW_p[] = {-8.0f ,0.0 ,8.0f};
    const float aeMED_p[] = { 5.0f ,10.0f ,15.0f};
    const float aeHIGH_s[] = { 12.0f ,20.0f ,28.0};
    qFIS_SetMF( MFin, wt, wtSLOW, trimf, wtSLOW_p );
    qFIS_SetMF( MFin, wt, wtMED, trimf, wtMED_p );
    qFIS_SetMF( MFin, wt, wtFAST, trimf, wtFAST_p );
    qFIS_SetMF( MFin, dax, daxLOW, trimf, daxLOW_p );
    qFIS_SetMF( MFin, dax, daxMED, trimf, daxMED_p );
    qFIS_SetMF( MFin, dax, daxHIGH, trimf, daxHIGH_p );
    qFIS_SetMF( MFin, day, dayLOW, trimf, dayLOW_p );
    qFIS_SetMF( MFin, day, dayMED, trimf, dayMED_p );
    qFIS_SetMF( MFin, day, dayHIGH, trimf, dayHIGH_s );
    qFIS_SetMF( MFin, ae, aeLOW, trimf, aeLOW_p );
    qFIS_SetMF( MFin, ae, aeMED, trimf, aeMED_p );
    qFIS_SetMF( MFin, ae, aeHIGH, trimf, aeHIGH_s );
    /* Add membership functions to the outputs */
    const float phitGYRO_p[] ={ -0.4f ,0.0f ,0.4f };
    const float phitBOTH_p[] ={ 0.2f ,0.5f ,0.8f };
    const float phitACCEL_p[] ={ 0.6f ,1.0f ,1.4f };
    const float thetatGYRO_p[] ={ -0.4f ,0.0f ,0.4f };
    const float thetatBOTH_p[] ={ 0.2f ,0.5f ,0.8f };
    const float thetatACCEL_p[] ={ 0.6f ,1.0f ,1.4f };
    qFIS_SetMF( MFout, phit, phitGYRO, trimf, phitGYRO_p );
    qFIS_SetMF( MFout, phit, phitBOTH, trimf, phitBOTH_p );
    qFIS_SetMF( MFout, phit, phitACCEL, trimf, phitACCEL_p );
    qFIS_SetMF( MFout, thetat, thetatGYRO, trimf, thetatGYRO_p );
    qFIS_SetMF( MFout, thetat, thetatBOTH, trimf, thetatBOTH_p );
    qFIS_SetMF( MFout, thetat, thetatACCEL, trimf, thetatACCEL_p );

    qFIS_Setup( &flexnav, Mamdani,
                flexnav_inputs, sizeof(flexnav_inputs),
                flexnav_outputs, sizeof(flexnav_outputs),
                MFin, sizeof(MFin),
                MFout, sizeof(MFout) );

    /* Set the crisp inputs */
    flexnav_inputs[ wt ].value = 0.0;
    flexnav_inputs[ dax ].value = 3.0;
    flexnav_inputs[ day ].value = 0.0;
    flexnav_inputs[ ae ].value = 0.0;

    qFIS_Fuzzify( &flexnav ); 
    qFIS_Inference( &flexnav, rules );
    qFIS_Defuzzify( &flexnav );
    /* Get the crisp outputs */
    printf( "- flexnav_outputs[phit].value=%g\r\n"
            "- flexnav_outputs[thetat].value=%g\r\n", 
            flexnav_outputs[phit].value, 
            flexnav_outputs[thetat].value );

    return EXIT_SUCCESS;
}

```