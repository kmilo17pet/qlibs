/*!
 * @file qfis.h
 * @author J. Camilo Gomez C.
 * @version 1.11
 * @note This file is part of the qLibs distribution.
 * @brief Fuzzy Inference System
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

    /**
    * @brief An enum with all the possible values to specify a membership 
    * function.
    */
    typedef enum {
        custommf = 0,   /*!< Custom user-defined Membership function*/
        trimf,          /*!< Triangular Membership function f(a,b,c)*/
        trapmf,         /*!< Trapezoidal Membership function f(a,b,c,d)*/
        gbellmf,        /*!< Generalized bell-shaped Membership function f(a,b,c)*/
        gaussmf,        /*!< Gaussian  Membership function f(s,c)*/
        gauss2mf,       /*!< Gaussian combination Membership function f(s1,c1,s2,c2)*/
        sigmf,          /*!< Sigmoidal  Membership function f(a,c)*/
        dsigmf,         /*!< Difference between two sigmoidal Membership functions f(a1,c1,a2,c2)*/
        psigmf,         /*!< Product of two sigmoidal membership functions f(a1,c1,a2,c2)*/
        pimf,           /*!< Pi-shaped membership function f(a,b,c,d)*/
        smf,            /*!< S-shaped membership function f(a,b)*/
        zmf,            /*!< Z-shaped membership function f(a,b)*/
        singletonmf,    /*!< Singleton Membership Function f(a)*/
        concavemf,      /*!< Concave Membership Function f(i,e)*/
        spikemf,        /*!< Spike Membership Function f(w,c)*/
        rampmf,         /*!< Ramp Membership Function f(s,e) */
        rectmf,         /*!< Rectangle Membership Function f(s,e)*/
        constantmf,     /*!< Constant membership function f(a) [Only for ::Sugeno FIS]*/
        linearmf,       /*!< Linear membership function f(...) [Only for ::Sugeno FIS]*/
        trampmf,        /*!< Tsukamoto Ramp membership function f(h,s,e) [Only for ::Tsukamoto FIS]*/
        tconcavemf,     /*!< Tsukamoto Concave membership function f(i,e) [Only for ::Tsukamoto FIS]*/
        tsigmf,         /*!< Tsukamoto Sigmoid membership function f(a,c) [Only for ::Tsukamoto FIS]*/
        tsmf,           /*!< Tsukamoto S-Shape membership function f(a,b) [Only for ::Tsukamoto FIS]*/
        tzmf            /*!< Tsukamoto Z-Shape membership function f(a,b) [Only for ::Tsukamoto FIS]*/
    } qFIS_MF_Name_t;

    /**
    * @brief An enum with the supported parameter values
    */
    typedef enum {
        qFIS_MIN = 0,   /*!< Minimal value*/
        qFIS_PROD,      /*!< Product*/
        qFIS_MAX,       /*!< Maximum value*/
        qFIS_PROBOR,    /*!< Probabilistic OR*/
        qFIS_SUM        /*!< Sum*/
    } qFIS_ParamValue_t;

    /**
    * @brief An enum with the allowed parameters that can be set on a FIS instance
    */
    typedef enum {
        qFIS_Implication,   /*!< Only ::qFIS_MIN and qFIS_PROD supported*/
        qFIS_Aggregation,   /*!< Only ::qFIS_MAX, qFIS_PROBOR and qFIS_SUM supported*/
        qFIS_AND,           /*!< Only ::qFIS_MIN and qFIS_PROD supported*/
        qFIS_OR,            /*!< Only ::qFIS_MAX and qFIS_PROBOR supported*/
        qFIS_EvalPoints     /*!< The number of points for de-fuzzification*/
    } qFIS_Parameter_t;

    /**
    * @brief An enum with the inference system types supported by qFIS
    */
    typedef enum {
        Mamdani = 0,
        Sugeno,
        Tsukamoto
    } qFIS_Type_t;

    /**
    * @brief A FIS I/O object
    * @details The instance should be initialized using the qFIS_SetIO() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        float lo, up, value, zi_wi, wi;
    } qFIS_IO_t;

    typedef float (*qFIS_MF_Fcn_t)( const qFIS_IO_t * const in, const float *p, const size_t n );

    /**
    * @brief A FIS Membership Function
    * @details The instance should be initialized using the qFIS_SetMF() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        qFIS_MF_Fcn_t shape;
        const float *points;
        float fx, h;
        size_t index;
    } qFIS_MF_t;


    typedef int16_t qFIS_Rules_t;
    typedef int qFIS_Tag_t;

    /**
    * @brief A FIS(Fuzzy Inference System) object
    * @details The instance should be initialized using the qFIS_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct _qFIS_s
    {
        qFIS_IO_t *input, *output;
        qFIS_MF_t *inMF, *outMF;
        size_t rule_cols;
        size_t nInputs, nOutputs;
        size_t nMFInputs, nMFOutputs;
        size_t nPoints;
        float (*andOp)( const float a, const float b );
        float (*orOp)( const float a, const float b );
        float (*implicate)( const float a, const float b );
        float (*aggregate)( const float a, const float b );
        float (*mUnion)( const float a, const float b );
        qFIS_Type_t type;
        float rStrength;
        qFIS_Rules_t lastConnector;
        size_t (*inferenceState)( struct _qFIS_s *f, const qFIS_Rules_t * const r, size_t i );
        int (*deFuzz)( struct _qFIS_s *f );
        int ruleCount;
    } qFIS_t;

    #define _QFIS_RULES_END         ( INT16_MIN + 1 )
    #define _QFIS_AND               ( INT16_MIN + 2 )
    #define _QFIS_OR                ( INT16_MIN + 3 )
    #define _QFIS_THEN              ( INT16_MIN + 4 )
    #define _QFIS_RULE_END          ( INT16_MIN + 5 )

    /*Rules build keywords*/
    #define QFIS_RULES_BEGIN        ( INT16_MIN )
    #define QFIS_RULES_END          ,_QFIS_RULES_END
    #define IF                      ,
    #define AND                     +1),_QFIS_AND,
    #define OR                      +1),_QFIS_OR,
    #define THEN                    +1),_QFIS_THEN,
    #define IS                      ,(
    #define IS_NOT                  ,-(
    #define END                     +1)

    /**
    * @brief Set parameters of the FIS instance.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] p The requested parameter to change/set.
    * @param[in] x The value of the parameter to set.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetParameter( qFIS_t * const f,
                           const qFIS_Parameter_t p,
                           const qFIS_ParamValue_t x );

    /**
    * @brief Setup and initialize the FIS instance.
    * @note Default configuration : AND = Min, OR = Max, Implication = Min
    * Aggregation = Max, EvalPoints = 100
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] t Type of inference ::Mamdani or ::Sugeno.
    * @param[in] inputs An array with all the system inputs as IO objects.
    * @param[in] ni The number of bytes used by @a inputs. Use the sizeof operator.
    * @param[in] outputs An array with all the system outputs as IO objects.
    * @param[in] no The number of bytes used by @a inputs. Use the sizeof operator.
    * @param[in] mf_inputs An array with all the membership functions related to
    * the inputs. This should be an array of MF objects.
    * @param[in] nmi The number of bytes used by @a mf_inputs. Use the sizeof 
    * operator.
    * @param[in] mf_outputs An array with all the membership functions related to
    * the outputs. This should be an array of MF objects.
    * @param[in] nmo The number of bytes used by @a mf_outputs. Use the sizeof 
    * operator.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Setup( qFIS_t * const f,
                    const qFIS_Type_t t, 
                    qFIS_IO_t * const inputs,
                    const size_t ni,
                    qFIS_IO_t * const outputs,
                    const size_t no,
                    qFIS_MF_t * const mf_inputs,
                    const size_t nmi,
                    qFIS_MF_t * const mf_outputs,
                    const size_t nmo );

    /**
    * @brief Set the tag and limits for the specified FIS IO
    * @note limits do not apply in ::Sugeno outputs
    * @param[in] c An array with the required inputs or outputs as qFIS_IO_t
    * objects.
    * @param[in] tag The used-defined tag
    * @param[in] min Minimum allowed value for this IO
    * @param[in] min Max allowed value for this IO
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetIO( qFIS_IO_t * const v,
                    const qFIS_Tag_t tag,
                    const float min,
                    const float max );

    /**
    * @brief Set the IO tag and points for the specified membership function
    * @param[in] m An array with the required membership functions as qFIS_MF_t
    * objects.
    * @param[in] io_tag The I/O tag related with this membership function
    * @param[in] mf_tag The user-defined tag for this membership function
    * @param[in] shape The wanted shape/form for this membership function, cam
    * be one of the following: ::trimf, ::trapmf, ::gbellmf, ::gaussmf, 
    * ::gauss2mf, ::sigmf, ::dsigmf, ::psigmf, ::pimf, ::smf, ::zmf, 
    * ::singletonmf, ::concavemf, ::spikemf, ::rampmf, ::rectmf.
    * @note For ::Sugeno FIS, an output membership function should be one of the
    * following: ::constantmf, ::linearmf.
    * @note For ::Tsukamoto FIS, an output membership function should be one the
    * following monotonic functions : trampmf, tsigmf, tsmf, tzmf, tconcavemf
    * @note To set a custom user-defined membership function, set this argument
    * as ::custommf and pass a pointer to the desired function on the 
    * @a custom_mf argument.
    * @param[in] custom_mf Custom user-defined membership function. To ignore
    * pass NULL as argument.
    * @param[in] cp Points or coefficients of the membership function.
    * @param[in] h Height of the membership function.
    * @note Heigth parameter does not apply for output membership functions on
    * ::Sugeno and ::Tsukamoto inference systems.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetMF( qFIS_MF_t * const m,
                    const qFIS_Tag_t io_tag,
                    const qFIS_Tag_t mf_tag,
                    const qFIS_MF_Name_t shape,
                    qFIS_MF_Fcn_t custom_mf,
                    const float *cp,
                    float h );

    /**
    * @brief Perform the fuzzification operation over the crisp inputs on the 
    * requested FIS object
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Fuzzify( qFIS_t * const f );

    /**
    * @brief Perform the inference process on the requested FIS object
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] r The rules set.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Inference( qFIS_t * const f,
                        const qFIS_Rules_t * const r );

    /**
    * @brief Perform the de-Fuzzification operation to compute the crisp outputs.
    * @note This API uses the Centroid method on ::Mamdani type FIS and
    * weight-average on ::Sugeno type FIS.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_DeFuzzify( qFIS_t * const f );

#ifdef __cplusplus
}
#endif

#endif
