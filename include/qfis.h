/*!
 * @file qfis.h
 * @author J. Camilo Gomez C.
 * @version 1.00
 * @note This file is part of the qLibs distribution.
 * @brief Fuzzy Inference System: only Mamdani supported (for now)
 **/

#ifndef QFIS_H
#define QFIS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <float.h>
    #include <stdint.h>
    #include <limits.h>
    #include <math.h>

    typedef enum {
        trimf = 0,
        trapmf,
        gbellmf,
        gaussmf,
        gauss2mf,
        sigmf,
        dsigmf,
        psigmf,
        pimf,
        smf,
        zmf,
        singletonmf
    } qFIS_MF_Name_t;

    typedef enum {
        qFIS_MIN = 0,
        qFIS_MAX,
        qFIS_PROD,
        qFIS_PROBOR
    } qFIS_FuzzMethod_t;

    typedef enum {
        Mamdani = 0,
        Sugeno
    } qFIS_Type_t;

    typedef struct {
        float min, max, value;
    } qFIS_IO_t;

    typedef struct {
        size_t index;
        float (*shape)( const float x, const float * const points );
        float points[ 4 ];
        float fuzzVal;
    } qFIS_MF_t;

    typedef struct {
        qFIS_IO_t *input, *output;
        qFIS_MF_t *inMF, *outMF;
        size_t rule_cols;
        size_t nInputs, nOutputs;
        size_t nMFInputs, nMFOutputs;
        size_t evalPoints;
        float (*andMethod)( const float a, const float b );
        float (*orMethod)( const float a, const float b );
        qFIS_Type_t type;
    } qFIS_t;

    typedef int16_t qFIS_Rules_t;
    typedef int qFIS_Tag_t;

    #define QFIS_RULES_BEGIN        ( INT16_MIN )
    #define _QFIS_RULES_END         ( INT16_MIN + 1 )
    #define _QFIS_AND               ( INT16_MIN + 2 )
    #define _QFIS_OR                ( INT16_MIN + 3 )
    #define _QFIS_THEN              ( INT16_MIN + 4 )
    #define _QFIS_RULE_END          ( INT16_MIN + 5 )

    #define QFIS_RULES_END          ,_QFIS_RULES_END
    #define IF                      ,
    #define AND                     +1),_QFIS_AND,
    #define OR                      +1),_QFIS_OR,
    #define THEN                    +1),_QFIS_THEN,
    #define IS                      ,(
    #define IS_NOT                  ,-(
    #define END                     +1)

    #define QFIS_IGNORE             (0.0f) 

    int qFIS_Setup( qFIS_t *f,
                    qFIS_Type_t t, 
                    size_t ep, 
                    qFIS_FuzzMethod_t and_m,
                    qFIS_FuzzMethod_t or_m,
                    qFIS_IO_t * const inputs,
                    size_t ni,
                    qFIS_IO_t * const outputs,
                    size_t no,
                    qFIS_MF_t *mfinputs,
                    size_t nmfins,
                    qFIS_MF_t *mfoutputs,
                    size_t nmfouts );
    int qFIS_SetIO( qFIS_IO_t *v,
                    qFIS_Tag_t tag,
                    float min,
                    float max );
    int qFIS_SetMF( qFIS_MF_t *m,
                    qFIS_Tag_t io_tag,
                    qFIS_Tag_t mf_tag,
                    qFIS_MF_Name_t shape,
                    float a,
                    float b,
                    float c,
                    float d );
    int qFIS_Fuzzify( qFIS_t *f );
    int qFIS_Inference( qFIS_t *f,
                        const qFIS_Rules_t * const r );
    int qFIS_Defuzzify( qFIS_t *f );

#ifdef __cplusplus
}
#endif

#endif
