# qtools

A collection of tools for embedded systems including:

- qFP16 : Q16.16 Fixed point math 
- qBitField: A bit-field manipulation library
- qSmoother : Filters to smooth noisy signals
- qPID : PID controller with derivative filter, anti-windup, tracking-mode and auto-tunning.
- qLTISys : Recursive LTI systems evaluation by transfer functions(continuous and discrete).
- qRMS : Recursive Root Mean Square(RMS) calculation of a signal.
- qCRC : Cyclic Redundancy Check (CRC) calculator (8/16/32 bits)
- qTDL : Tapped Delay Line in O(1). 

## Draft examples

### A simple fixed-point calculationn

$$x_{1,2} = {-b+\sqrt{b^2 - 4ac} \over 2a}.$$

```c
#include "stdio.h"
#include "stdlib.h"
#include "qfp16.h

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

$$G_p(s) = {{2s^2 + 3s + 6} \over {s^3 + 6s^2+ 11s + 16} }$$

```c
#include "stdio.h"
#include "stdlib.h"
#include "bsp.h"
#include "qltisys.h

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

$$G_p(z^{-1}) = {{0.1 + 0.2z^{-1} + 0.3z^{-2}} \over {1 -0.85z^{-1} + 0.02z^{-2}} }$$

```c
#include "stdio.h"
#include "stdlib.h"
#include "bsp.h"
#include "qltisys.h

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

### A speed control usign the PID algorithm 

```c
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
    xTaskCreate( xTaskPIDspeedControl, "speedControl",  1024, &speedControl,   configMAX_PRIORITIES - 1 ,NULL );
    vTaskStartScheduler();
    for( ;; );    
    return EXIT_SUCCESS;
}
```

### Smoothing a noisy signal by using a gaussian filter
```c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bsp.h"
#include "qssmoother.h"
#include "AppSignalProcessing.h"

#define SMOOTHER_WINDOW_SIZE    ( 10 )
#define SMOOTHER_SAMPLE_TIME    ( 100 )

void xTaskSignalProcessing( void *arg )
{
    qSSmoother_GAUSSIAN_t *smoother = (qSSmoother_GAUSSIAN_t *)arg;
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
    qSSmoother_GAUSSIAN_t smoother;
    float win_kernel[ 2*SMOOTHER_WINDOW_SIZE ]; /*storage for the window and the kernel*/
    float params[ 2 ] = { 0.5f, SMOOTHER_WINDOW_SIZE/2.0f }; /*sigma and offset*/
    int ret;
    
    BSP_SystemInit( );
    ret = qSSmoother_Setup( &smoother, QSSMOOTHER_TYPE_GAUSSIAN, params, win_kernel, sizeof(win_kernel)/sizeof(win_kernel[0]) );
    if ( 0 == ret ) {
        puts( "ERROR: Cant configure the smoother filter" );
    }
    xTaskCreate( xTaskSignalProcessing, "signalProcessing",  512, &smoother,  2, NULL );
    vTaskStartScheduler();
    for( ;; );    
    return EXIT_SUCCESS;
}
```



