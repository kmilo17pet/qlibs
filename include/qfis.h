/*!
 * @file qfis.h
 * @author J. Camilo Gomez C.
 * @version 1.4.4
 * @note This file is part of the qLibs distribution.
 * @brief Fuzzy Inference System (FIS) Engine
 **/

#ifndef QFIS_H
#define QFIS_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <limits.h>

    /** @addtogroup  qfis Fuzzy Inference System Engine
    * @brief API for the qFIS Fuzzy Inference System Engine
    *  @{
    */

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
        linsmf,         /*!< Linear s-shaped saturation membership function f(a,b)*/
        linzmf,         /*!< Linear z-shaped saturation membership function f(a,b)*/
        rectmf,         /*!< Rectangle Membership Function f(s,e)*/
        cosmf,          /*!< Cosine Membership Function f(c,w)*/
        constantmf,     /*!< Constant membership function f(a) [Only for ::Sugeno FIS ]*/
        linearmf,       /*!< Linear membership function f(...) [Only for ::Sugeno FIS ]*/
        tlinsmf,        /*!< Tsukamoto s-shaped saturation membership function f(a,b) [Only for ::Tsukamoto FIS ]*/
        tlinzmf,        /*!< Tsukamoto linzmf membership function f(a,b) [ Only for ::Tsukamoto FIS ]*/
        tconcavemf,     /*!< Tsukamoto z-shaped saturation membership function f(i,e) [Only for ::Tsukamoto FIS ]*/
        tsigmf,         /*!< Tsukamoto Sigmoid membership function f(a,c) [ Only for ::Tsukamoto FIS ]*/
        tsmf,           /*!< Tsukamoto S-Shape membership function f(a,b) [ Only for ::Tsukamoto FIS ]*/
        tzmf,           /*!< Tsukamoto Z-Shape membership function f(a,b) [ Only for ::Tsukamoto FIS ]*/
        /*! @cond  */
        _NUM_MFS        /*!< Number of supported membership functions*/ //skipcq: CXX-E2000
        /*! @endcond  */
    } qFIS_MF_Name_t;

    /**
    * @brief An enum with all the possible de-Fuzzyfication methods.
    */
    typedef enum {
        centroid = 0,   /*!< Center of gravity of the fuzzy set along the x-axis [ Only for ::Mamdani FIS ]*/
        bisector,       /*!< Vertical line that divides the fuzzy set into two sub-regions of equal area [ Only for ::Mamdani FIS ]**/
        mom,            /*!< Middle of Maximum [ Only for ::Mamdani FIS ]**/
        lom,            /*!< Largest of Maximum [ Only for ::Mamdani FIS ]**/
        som,            /*!< Smallest of Maximum [ Only for ::Mamdani FIS ]**/
        wtaver,         /*!< Weighted average of all rule outputs [ Only for ::Sugeno and ::Tsukamoto FIS ]*/
        wtsum,          /*!< Weighted sum of all rule outputs [ Only for ::Sugeno FIS ]*/
        /*! @cond  */
        _NUM_DFUZZ      /*!< Number of supported defuzzification methods*/ //skipcq: CXX-E2000
        /*! @endcond  */
    } qFIS_DeFuzz_Method_t;

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
        Mamdani = 0,        /*!< Mamdani inference system. The output of each rule its a fuzzy logic set.*/
        Sugeno,             /*!< Takagi-Sugeno inference system. The output of each rule its a function either linear or constant.*/
        Tsukamoto           /*!< Mamdani inference system. The output of each rule its a fuzzy logic set represented with a monotonic membership function.*/
    } qFIS_Type_t;

    /*! @cond  */
    typedef enum {
        DeFuzz_Init,
        DeFuzz_Compute,
        DeFuzz_End
    } qFIS_DeFuzzState_t;
    /*! @endcond  */

    /*! @cond  */
    typedef struct
    {
        float min, max, value;
    } qFIS_IO_Base_t;
    /*! @endcond  */

    /**
    * @brief A FIS Input object
    * @details The instance should be initialized using the qFIS_InputSetup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        qFIS_IO_Base_t b;
        /*! @endcond  */
    } qFIS_Input_t;

    /**
    * @brief A FIS Output object
    * @details The instance should be initialized using the qFIS_OutputSetup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        qFIS_IO_Base_t b;
        void *owner;
        float *xag, *yag;
        float res, x, y, data[ 4 ];
        /*! @endcond  */
    } qFIS_Output_t;

    /*! @cond  */
    typedef float (*qFIS_MF_Fcn_t)( const qFIS_IO_Base_t * const in, const float *p, const size_t n );
    /*! @endcond  */

    /**
    * @brief A FIS Membership Function
    * @details The instance should be initialized using the qFIS_SetMF() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct
    {
        /*! @cond  */
        qFIS_MF_Fcn_t shape;
        const float *points;
        float fx, h;
        size_t index;
        /*! @endcond  */
    } qFIS_MF_t;

    #define FIS_RULE_ITEM_SIZE  1

    #if ( FIS_RULE_ITEM_SIZE == 1 )

        /**
        * @brief Type definition to instantiate a set of fuzzy rules
        * @details Rules are defined by combining I/O and membership function tags
        * with the following statements:
        *
        * #QFIS_RULES_BEGIN, #IF, #IS, #IS_NOT, #AND, #OR, #THEN, #END and
        * #QFIS_RULES_END
        *
        * Example:
        * @code{.c}
        * static const qFIS_Rules_t rules[] = {
        *     QFIS_RULES_BEGIN
        *         IF service IS service_poor OR food IS food_rancid THEN tip IS tip_cheap END
        *         IF service IS service_good THEN tip IS tip_average END
        *         IF service IS service_excellent OR food IS food_delicious THEN tip IS tip_generous END
        *     QFIS_RULES_END
        * };
        * @endcode
        */
        typedef int8_t qFIS_Rules_t;
        #define FIS_RULES_MIN_VALUE     INT8_MIN
    #else
        /**
        * @brief Type definition to instantiate a set of fuzzy rules
        * @details Rules are defined by combining I/O and membership function tags
        * with the following statements:
        *
        * #QFIS_RULES_BEGIN, #IF, #IS, #IS_NOT, #AND, #OR, #THEN, #END and
        * #QFIS_RULES_END
        *
        * Example:
        * @code{.c}
        * static const qFIS_Rules_t rules[] = {
        *     QFIS_RULES_BEGIN
        *         IF service IS service_poor OR food IS food_rancid THEN tip IS tip_cheap END
        *         IF service IS service_good THEN tip IS tip_average END
        *         IF service IS service_excellent OR food IS food_delicious THEN tip IS tip_generous END
        *     QFIS_RULES_END
        * };
        * @endcode
        */
        typedef int16_t qFIS_Rules_t;
        #define FIS_RULES_MIN_VALUE     INT16_MIN
    #endif

    /*! @cond  */
    typedef int qFIS_Tag_t;
    typedef float (*qFIS_DeFuzz_Fcn_t)( qFIS_Output_t * const o, const qFIS_DeFuzzState_t stage );
    /*! @endcond  */

    /**
    * @brief A FIS(Fuzzy Inference System) object
    * @details The instance should be initialized using the qFIS_Setup() API.
    * @note Do not access any member of this structure directly.
    */
    typedef struct _qFIS_s //skipcq: CXX-E2000
    {
        /*! @cond  */
        qFIS_Input_t *input;
        qFIS_Output_t *output;
        qFIS_MF_t *inMF, *outMF;
        float (*andOp)( const float a, const float b );
        float (*orOp)( const float a, const float b );
        float (*implicate)( const float a, const float b );
        float (*aggregate)( const float a, const float b );
        size_t (*inferenceState)( struct _qFIS_s * const f, size_t i );
        size_t (*aggregationState)( struct _qFIS_s * const f, size_t i );
        qFIS_DeFuzz_Fcn_t deFuzz;
        float *ruleWeight;
        float *wi;
        const qFIS_Rules_t *rules;
        size_t rule_cols;
        size_t nInputs, nOutputs;
        size_t nMFInputs, nMFOutputs;
        size_t nPoints;
        size_t nRules, ruleCount;
        float rStrength;
        qFIS_Rules_t lastConnector;
        qFIS_Type_t type;
        /*! @endcond  */
    } qFIS_t;

    /*! @cond  */
    #define _QFIS_RULES_END         ( FIS_RULES_MIN_VALUE + 1 ) //skipcq: CXX-E2000
    #define _QFIS_AND               ( FIS_RULES_MIN_VALUE + 2 ) //skipcq: CXX-E2000
    #define _QFIS_OR                ( FIS_RULES_MIN_VALUE + 3 ) //skipcq: CXX-E2000
    #define _QFIS_THEN              ( FIS_RULES_MIN_VALUE + 4 ) //skipcq: CXX-E2000

    #define _QFIS_IF_STATEMENT      ,                   //skipcq: CXX-E2000
    #define _QFIS_AND_STATEMENT     +1),_QFIS_AND,      //skipcq: CXX-E2000
    #define _QFIS_OR_STATEMENT      +1),_QFIS_OR,       //skipcq: CXX-E2000
    #define _QFIS_THEN_STATEMENT    +1),_QFIS_THEN,     //skipcq: CXX-E2000
    #define _QFIS_IS_STATEMENT      ,(                  //skipcq: CXX-E2000
    #define _QFIS_IS_NOT_STATEMENT  ,-(                 //skipcq: CXX-E2000
    #define _QFIS_END_STATEMENT     +1)                 //skipcq: CXX-E2000
    /*! @endcond  */

    /*Rules build keywords*/

    /**
    * @brief Start a Fuzzy rule set.
    * The #QFIS_RULES_BEGIN statement is used to declare the starting point of
    * a FIS rule set. It should be placed at the start of the rules enumeration.
    * #QFIS_RULES_END declare the end of the FIS rule set.
    * @see #QFIS_RULES_END
    * @warning Only one segment is allowed inside a fuzzy rule set.
    * @note It must always be used together with a matching #QFIS_RULES_END
    * statement.
    * Example:
    * @code{.c}
    * static const qFIS_Rules_t rules[] = {
    *     QFIS_RULES_BEGIN
    *
    *     QFIS_RULES_END
    * };
    * @endcode
    */
    #define QFIS_RULES_BEGIN        ( FIS_RULES_MIN_VALUE )

    /**
    * @brief Ends a Fuzzy rule set.
    * The #QFIS_RULES_END statement is used to finalize the declaration of a
    * FIS rule set. It should be placed at the end of the rules enumeration.
    * #QFIS_RULES_BEGIN declare the start point of the FIS rule set.
    * @see #QFIS_RULES_BEGIN
    * @warning Only one segment is allowed inside a fuzzy rule set.
    * @note It must always be used together with a matching #QFIS_RULES_BEGIN
    * statement.
    * Example:
    * @code{.c}
    * static const qFIS_Rules_t rules[] = {
    *     QFIS_RULES_BEGIN
    *
    *     QFIS_RULES_END
    * };
    * @endcode
    */
    #define QFIS_RULES_END          ,_QFIS_RULES_END

    /** @brief Rule statement to begin a rule sentence */
    #define IF                      _QFIS_IF_STATEMENT
    /** @brief Rule statement to represent the AND connector */
    #define AND                     _QFIS_AND_STATEMENT
    /** @brief Rule statement to represent the OR connector */
    #define OR                      _QFIS_OR_STATEMENT
    /** @brief Rule statement to represent the implication */
    #define THEN                    _QFIS_THEN_STATEMENT
    /** @brief Rule statement to represent a premise*/
    #define IS                      _QFIS_IS_STATEMENT
    /** @brief Rule statement to represent a negated premise */
    #define IS_NOT                  _QFIS_IS_NOT_STATEMENT
    /** @brief Rule statement to end a rule sentence */
    #define END                     _QFIS_END_STATEMENT

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
    * @brief Change the default de-Fuzzification method of the FIS instance.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] m The de-fuzzification method: use one of the following :
    *  ::centroid, ::bisector, ::mom, ::lom, ::som, ::wtaver, ::wtsum
    * @note ::centroid, ::bisector, ::mom, ::lom and ::som only apply for a
    * ::Mamdani FIS
    * @note ::wtaver and ::wtsum only apply for a ::Sugeno FIS.
    * @note ::wtaver only apply for a ::Tsukamoto FIS
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetDeFuzzMethod( qFIS_t * const f,
                              qFIS_DeFuzz_Method_t m );

    /**
    * @brief Setup and initialize the FIS instance.
    * @note Default configuration : AND = Min, OR = Max, Implication = Min
    * Aggregation = Max, EvalPoints = 100
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] t Type of inference ::Mamdani, ::Sugeno or ::Tsukamoto.
    * @param[in] inputs An array with all the system inputs as qFIS_Input_t
    * objects.
    * @param[in] ni The number of bytes used by @a inputs. Use the sizeof operator.
    * @param[in] outputs An array with all the system outputs as qFIS_Output_t
    * objects.
    * @param[in] no The number of bytes used by @a outputs. Use the sizeof operator.
    * @param[in] mf_inputs An array with all the membership functions related to
    * the inputs. This should be an array of qFIS_MF_t objects.
    * @param[in] nmi The number of bytes used by @a mf_inputs. Use the sizeof
    * operator.
    * @param[in] mf_outputs An array with all the membership functions related to
    * the outputs. This should be an array of qFIS_MF_t objects.
    * @param[in] nmo The number of bytes used by @a mf_outputs. Use the sizeof
    * operator.
    * @param[in] r The rules set.
    * @param[in] wi An array of size @a n were the rule strengths will be stored.
    * @param[in] n Number of rules
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Setup( qFIS_t * const f,
                    const qFIS_Type_t t,
                    qFIS_Input_t * const inputs,
                    const size_t ni,
                    qFIS_Output_t * const outputs,
                    const size_t no,
                    qFIS_MF_t * const mf_inputs,
                    const size_t nmi,
                    qFIS_MF_t * const mf_outputs,
                    const size_t nmo,
                    const qFIS_Rules_t * const r,
                    float *wi,
                    const size_t n
                    );

    /**
    * @brief Setup the input with the specified tag and set limits for it
    * @param[in] v An array with the FIS inputs as a qFIS_Input_t array.
    * @param[in] t The input tag
    * @param[in] min Minimum allowed value for this input
    * @param[in] max Max allowed value for this input
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_InputSetup( qFIS_Input_t * const v,
                         const qFIS_Tag_t t,
                         const float min,
                         const float max );

    /**
    * @brief Setup the output with the specified tag and set limits for it
    * @note limits do not apply in ::Sugeno outputs
    * @param[in] v An array with the FIS outputs as a qFIS_Output_t array.
    * @param[in] t The output tag
    * @param[in] min Minimum allowed value for this output
    * @param[in] max Max allowed value for this output
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_OutputSetup( qFIS_Output_t * const v,
                          const qFIS_Tag_t t,
                          const float min,
                          const float max );

    /**
    * @brief Set a crisp value of the input with the specified tag.
    * @param[in] v An array with the FIS inputs as a qFIS_Input_t array.
    * @param[in] t The input tag
    * @param[in] value The crisp value to set
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetInput( qFIS_Input_t * const v,
                       const qFIS_Tag_t t,
                       const float value );

    /**
    * @brief Get the de-fuzzified crisp value from the the output  with the
    * specified tag.
    * @param[in] v An array with the FIS inputs as a qFIS_Output_t array.
    * @param[in] t The output tag
    * @return The requested de-fuzzified crips value.
    */
    float qFIS_GetOutput( const qFIS_Output_t * const v,
                          const qFIS_Tag_t t );

    /**
    * @brief Set the IO tag and points for the specified membership function
    * @param[in] m An array with the required membership functions as qFIS_MF_t
    * objects.
    * @param[in] io The I/O tag related with this membership function
    * @param[in] mf The user-defined tag for this membership function
    * @param[in] s The wanted shape/form for this membership function, can
    * be one of the following: ::trimf, ::trapmf, ::gbellmf, ::gaussmf,
    * ::gauss2mf, ::sigmf, ::dsigmf, ::psigmf, ::pimf, ::smf, ::zmf,
    * ::singletonmf, ::concavemf, ::spikemf, ::linsmf, ::linzmf, ::rectmf,
    * ::cosmf.
    * @note For ::Sugeno FIS, an output membership function should be one of the
    * following: ::constantmf, ::linearmf.
    * @note For ::Tsukamoto FIS, an output membership function should be one the
    * following monotonic functions : ::tlinsmf, ::tlinzmf, ::tsmf, ::tzmf,
    * ::tconcavemf
    * @note To set a custom user-defined membership function, set this argument
    * as ::custommf and pass a pointer to the desired function on the
    * @a custom_mf argument.
    * @param[in] custom_mf Custom user-defined membership function. To ignore
    * pass @c NULL as argument.
    * @param[in] cp Points or coefficients of the membership function.
    * @param[in] h Height of the membership function.
    * @note Height parameter @a h does not apply for output membership functions
    * on ::Sugeno and ::Tsukamoto inference systems. [ 0 <= h <= 1]
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetMF( qFIS_MF_t * const m,
                    const qFIS_Tag_t io,
                    const qFIS_Tag_t mf,
                    const qFIS_MF_Name_t s,
                    qFIS_MF_Fcn_t custom_mf,
                    const float *cp,
                    const float h );

    /**
    * @brief Set location to store the aggregated region for supplied FIS output
    * @note This feature only applies to Mamdani systems.
    * @warning Array size of @a x and @a y should be greater or equal to the
    * number of evaluation points of the FIS instance.
    * @pre The FIS instance and the output should be previously configured
    * initialized with qFIS_Setup() and qFIS_OutputSetup() respectively.
    * @param[in] o An array of type qFIS_Output_t with the FIS outputs.
    * @param[in] t The output tag
    * @param[in] x Array where the x-axis points of the aggregated output will be
    * stored
    * @param[in] y Array where the y-axis points of the aggregated output will be
    * stored
    * @param[in] n Number of elements in  @a x or @a y.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_StoreAggregatedRegion( qFIS_Output_t * const o,
                                    const qFIS_Tag_t t,
                                    float *x,
                                    float *y,
                                    const size_t n );

    /**
    * @brief Perform the fuzzification operation over the crisp inputs on the
    * requested FIS object
    * @pre I/Os and fuzzy sets must be previously initialized by qFIS_InputSetup(),
    * qFIS_OutputSetup(), qFIS_SetMF() and qFIS_Setup() respectively.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Fuzzify( qFIS_t * const f );

    /**
    * @brief Perform the inference process on the requested FIS object
    * @pre The instance should have already invoked the fuzzification operation
    * successfully with qFIS_Fuzzify()
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_Inference( qFIS_t * const f );

    /**
    * @brief Perform the de-Fuzzification operation to compute the crisp outputs.
    * @pre The instance should have already invoked the inference process
    * successfully with qFIS_Inference()
    * @note By default, this function, this API uses the Centroid method on
    * ::Mamdani type FIS and weight-average on ::Sugeno type FIS. To change
    * the default settings use the qFIS_SetDeFuzzMethod() function.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_DeFuzzify( qFIS_t * const f );

    /**
    * @brief Set weights to the rules of the inference system.
    * @pre I/Os and fuzzy sets must be previously initialized by qFIS_InputSetup(),
    * qFIS_OutputSetup(), qFIS_SetMF() and qFIS_Setup() respectively.
    * @param[in] f A pointer to the Fuzzy Inference System instance.
    * @param[in] rWeights An array with the values of every rule weight;
    * @return 1 on success, otherwise return 0.
    */
    int qFIS_SetRuleWeights( qFIS_t * const f,
                             float *rWeights );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif
