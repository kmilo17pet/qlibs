#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "qcirmed.h"
#include "qssmoother.h"
#include "qcsys.h"
#include "qdsys.h"

int main(int argc, char *argv[]){
    float testsamples[] = { -50, -55, -54, -64, -2, -54, -46, -51, -25, -55, -58, -90, -20, -21 ,-23, -25 -26, -27  };

    qSSmoother_LPF1_t filter1;
    qSSmoother_LPF2_t filter2;
    qSSmoother_MWM_t filter3;
    qSSmoother_MWOR_t filter4;
    qSSmoother_GAUSSIAN_t filter5;
    size_t i, j;
    float out1, out2, out3, out4, out5;
    
    float alfa[]={ 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 0.95f};

    float cf3_w[5];
    float cf4_w[5];
   
    float cf5_w[3], kernel[3];

    for(j=0; j<sizeof(alfa)/sizeof(alfa[0]);j++){

        qSSmoother_Setup_LPF1( &filter1, alfa[j] );
        qSSmoother_Setup_LPF2( &filter2, alfa[j] );
        qSSmoother_Setup_MWM( &filter3, cf3_w, sizeof(cf3_w)/sizeof(cf3_w[0]) );
        qSSmoother_Setup_MWOR( &filter4, cf4_w, sizeof(cf4_w)/sizeof(cf4_w[0]), alfa[j] );
        qSSmoother_Setup_GAUSSIAN( &filter5, cf5_w, kernel, sizeof(cf5_w)/sizeof(cf5_w[0]), alfa[j], 1 );
        printf("alfa = %f\r\n", alfa[j]);
        printf("NONE\t\tLPF1\t\tLPF2\t\tMWM\t\tMWOR\t\tGAUSS\r\n ");
        for( i=0;i<sizeof(testsamples)/sizeof(testsamples[0]);i++){
            out1 = qSSmoother_Perform( &filter1, testsamples[i] );
            out2 = qSSmoother_Perform( &filter2, testsamples[i] );
            out3 = qSSmoother_Perform( &filter3, testsamples[i] );
            out4 = qSSmoother_Perform( &filter4, testsamples[i] );
            out5 = qSSmoother_Perform( &filter5, testsamples[i] );
            printf("%f\t%f\t%f\t%f\t%f\t%f\r\n ",testsamples[i], out1, out2, out3, out4, out5 );
        }
        printf("\r\n");
    }


    qCSys_t sys;
    float num[4] = {0.0f, 2.0f, 3.0f, 6.0f};
    float den[4] = {1.0f, 6.0f, 11.0f, 16.0f};
    float x[4] = { 0.0f };
    qCSys_Setup( &sys, num, den, x, 4, 0.01f );
    for( i=0;i<600;i++){
        //printf("%g \r\n",qCSys_Excite( &sys, 1.0f ) );
    }

    puts("_____________________________");
    qDSys_t sysd;
    float nd[] = {0.1f, 0.2f, 0.3f };
    float dd[] = {1.0f, -0.85f, 0.0f };
    float vk[2] = { 0.0f };
    qDSys_Setup( &sysd, nd, dd, vk, 3, 1 );
    for( i=0;i<35;i++){
        //printf("%g \r\n",qDSys_Excite( &sysd, 1.0f ) );
    }
    (void)argc;
    (void)argv;
    return EXIT_SUCCESS;
}