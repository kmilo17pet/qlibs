[![Built for](https://img.shields.io/badge/built%20for-microcontrollers-lightgrey?logo=WhiteSource)](https://github.com/kmilo17pet/QuarkTS)
[![CodeFactor](https://www.codefactor.io/repository/github/kmilo17pet/qtools/badge/main)](https://www.codefactor.io/repository/github/kmilo17pet/qtools/overview/main)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/14d566939d2e4d4181088cc1c6666fa3)](https://www.codacy.com/gh/kmilo17pet/qTools/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kmilo17pet/qTools&amp;utm_campaign=Badge_Grade)
[![CodeInspectorScore](https://www.code-inspector.com/project/27197/score/svg)](https://frontend.code-inspector.com/project/27197/dashboard)
[![CodeInspectorGrade](https://www.code-inspector.com/project/27197/status/svg)](https://frontend.code-inspector.com/project/27197/dashboard)
[![MISRAC2012](https://img.shields.io/badge/MISRAC2012-Compliant-blue.svg?logo=c)](https://en.wikipedia.org/wiki/MISRA_C)
[![CERT](https://img.shields.io/badge/CERT-Compliant-blue.svg?logo=c)](https://wiki.sei.cmu.edu/confluence/display/seccode/SEI+CERT+Coding+Standards)
[![C Standard](https://img.shields.io/badge/STD-C99-green.svg?logo=c)](https://en.wikipedia.org/wiki/C99)
[![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)](https://github.com/kmilo17pet/qTools/graphs/commit-activity)
[![License](https://img.shields.io/github/license/kmilo17pet/qTools)](https://github.com/kmilo17pet/qTools/blob/main/LICENSE) 

# qtools


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
float x[ SYS_ORDER ] = { 0.0f };

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
float xk[ NB ] = { 0.0f };

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
    /* Create the tast that handles the speed control. */
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
    qSSmoother_GMWF_t *smoother = (qSSmoother_GAUSSIAN_t *)arg;
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



