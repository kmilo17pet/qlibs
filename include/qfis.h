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

    /**
    * @brief An enum with all the possible values to specify a membership 
    * function.
    */
    typedef enum {
        trimf = 0,      /*!< Triangular Membership function f(a,b,c)*/
        trapmf,         /*!< Trapezoidal Membership function f(a,b,c,d)*/
        gbellmf,        /*!< Generalized bell-shaped Membership function f(a,b,c)*/
        gaussmf,        /*!< Gaussian  Membership function f(a,b)*/
        gauss2mf,       /*!< Gaussian combination Membership function f(a,b,c,d)*/
        sigmf,          /*!< Sigmoidal  Membership function f(a,b)*/
        dsigmf,         /*!< Difference between two sigmoidal Membership functions f(a,b,c,d)*/
        psigmf,         /*!< Product of two sigmoidal membership functions f(a,b,c,d)*/
        pimf,           /*!< Pi-shaped membership function f(a,b,c,d)*/
        smf,            /*!< S-shaped membership function f(a,b)*/
        zmf,            /*!< Z-shaped membership function f(a,b)*/
        singletonmf,    /*!< Singleton Membership Function f(a)*/
        constant,       /*!< Constant membership function f(a) [Only for Sugeno FIS]*/
        linear          /*!< Linear membership function f(...) [Only for Sugeno FIS]*/
    } qFIS_MF_Name_t;

    /**
    * @brief An enum with all the allowed supported operators
    */
    typedef enum {
        qFIS_MIN = 0,   /*!< Minimal value*/
        qFIS_PROD,      /*!< Product*/
        qFIS_MAX,       /*!< Maximum value*/
        qFIS_PROBOR,    /*!< Probabilistic OR*/
        qFIS_SUM        /*!< Sum*/
    } qFIS_Operator_t;

    /**
    * @brief An enum with all the allowed parameters that can be set on a FIS instance
    */
    typedef enum {
        qFIS_Implication,   /*!< Only ::qFIS_MIN and qFIS_PROD supported*/
        qFIS_Aggregation,   /*!< Only ::qFIS_MAX, qFIS_PROBOR and qFIS_SUM supported*/
        qFIS_AND,           /*!< Only ::qFIS_MIN and qFIS_PROD supported*/
        qFIS_OR,            /*!< Only ::qFIS_MAX and qFIS_PROBOR supported*/
        qFIS_EvalPoints     /*!< The number of points for de-fuzzification*/
    } qFIS_Parameter_t;

    /**
    * @brief An enum with the types of inference systems supported by qFIS.
    */
    typedef enum {
        Mamdani = 0,
        Sugeno
    } qFIS_Type_t;

    /**
    * @brief A FIS I/O object
    * @details The instance should be initialized using the qFIS_SetIO() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        float lo, up, value;
    } qFIS_IO_t;

    /**
    * @brief A FIS Membership Function
    * @details The instance should be initialized using the qFIS_SetMF() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        size_t index;
        float (*shape)( const float x, const float * const points );
        const float *points;
        float fuzzVal;
    } qFIS_MF_t;

    /**
    * @brief A FIS(Fuzzy Inference System) object
    * @details The instance should be initialized using the qFIS_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        qFIS_IO_t *input, *output;
        qFIS_MF_t *inMF, *outMF;
        size_t rule_cols;
        size_t nInputs, nOutputs;
        size_t nMFInputs, nMFOutputs;
        size_t evalPoints;
        float (*andMethod)( const float a, const float b );
        float (*orMethod)( const float a, const float b );
        float (*implication)( const float a, const float b );
        float (*aggregation)( const float a, const float b );
        float (*mUnion)( const float a, const float b );
        qFIS_Type_t type;
    } qFIS_t;

    typedef int16_t qFIS_Rules_t;
    typedef int qFIS_Tag_t;

    
    #define _QFIS_RULES_END         ( INT16_MIN + 1 )
    #define _QFIS_AND               ( INT16_MIN + 2 )
    #define _QFIS_OR                ( INT16_MIN + 3 )
    #define _QFIS_THEN              ( INT16_MIN + 4 )
    #define _QFIS_RULE_END          ( INT16_MIN + 5 )

    /*Rules constructs*/
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
    int qFIS_SetParameter( qFIS_t *f,
                           qFIS_Parameter_t p,
                           int x );

    /**
    * @brief Setup and initialize the FIS instance.
    * @note Default configuration : AND = Min, OR = Max, Implication = Min
    * Aggregation = Max, EvalPoints = 100
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] t Type of inference.
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
    int qFIS_Setup( qFIS_t *f,
                    qFIS_Type_t t, 
                    qFIS_IO_t * const inputs,
                    size_t ni,
                    qFIS_IO_t * const outputs,
                    size_t no,
                    qFIS_MF_t *mf_inputs,
                    size_t nmi,
                    qFIS_MF_t *mf_outputs,
                    size_t nmo );

    /**
    * @brief Set the tag and limits for the specified FIS IO
    * @note limits do not apply in Sugeno outputs
    * @param[in] c An array with the required inputs or outputs as IO objects.
    * @param[in] tag The used-defined tag
    * @param[in] min Minimum allowed value for this IO
    * @param[in] min Max allowed value for this IO
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetIO( qFIS_IO_t *v,
                    qFIS_Tag_t tag,
                    float min,
                    float max );

    /**
    * @brief Set the IO tag and points for the specified Membership function
    * @param[in] m An array with the required membership functions as MF objects.
    * @param[in] io_tag The I/O tag related with this membership function
    * @param[in] mf_tag The user-defined tag for this membership function
    * @param[in] shape The wanted shape for this membership function
    * @param[in] cp Points of the membership function.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetMF( qFIS_MF_t *m,
                    qFIS_Tag_t io_tag,
                    qFIS_Tag_t mf_tag,
                    qFIS_MF_Name_t shape,
                    const float *cp );

    /**
    * @brief Perform the fuzzification operation over the crisp inputs on the 
    * requested FIS object
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Fuzzify( qFIS_t *f );

    /**
    * @brief Perform the inference process on the requested FIS object
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] r The rules set.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Inference( qFIS_t *f,
                        const qFIS_Rules_t * const r );

    /**
    * @brief Perform the de-fuzzification operation to compute the crisp outputs.
    * @note This API uses the Centroid method.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Defuzzify( qFIS_t *f );

#ifdef __cplusplus
}
#endif

#endif
