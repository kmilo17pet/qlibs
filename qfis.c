/*!
 * @file qfis.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qfis.h"
#include "qffmath.h"
#include <stdbool.h>

typedef float (*qFIS_FuzzyOperator_t)( const float a, const float b );

static float qFIS_TriMF( const qFIS_IO_Base_t * const in,
                         const float *p,
                         const size_t n );
static float qFIS_TrapMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_GBellMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_GaussMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_Gauss2MF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n );
static float qFIS_SigMF( const qFIS_IO_Base_t * const in,
                         const float *p,
                         const size_t n );
static float qFIS_TSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_DSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_PSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_SMF( const qFIS_IO_Base_t * const in,
                       const float *p,
                       const size_t n );
static float qFIS_TSMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_ZMF( const qFIS_IO_Base_t * const in,
                       const float *p,
                       const size_t n );
static float qFIS_LinSMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_LinZMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_TZMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_PiMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_SingletonMF( const qFIS_IO_Base_t * const in,
                               const float *p,
                               const size_t n );
static float qFIS_ConcaveMF( const qFIS_IO_Base_t * const in,
                             const float *p,
                             const size_t n );
static float qFIS_TConcaveMF( const qFIS_IO_Base_t * const in,
                              const float *p,
                              const size_t n );
static float qFIS_SpikeMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_TLinSMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_TLinZMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_RectangleMF( const qFIS_IO_Base_t * const in,
                               const float *p,
                               const size_t n );
static float qFIS_CosineMF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n );
static float qFIS_ConstantMF( const qFIS_IO_Base_t * const in,
                              const float *p,
                              const size_t n );
static float qFIS_LinearMF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n );
static float qFIS_Min( const float a,
                       const float b );
static float qFIS_Max( const float a,
                       const float b );
static float qFIS_Prod( const float a,
                        const float b );
static float qFIS_ProbOR( const float a,
                          const float b );
static float qFIS_Sum( const float a,
                       const float b );
static float qFIS_Bound( float y,
                         const float yMin,
                         const float yMax );
static void qFIS_EvalInputMFs( qFIS_t * const f );
static void qFIS_TruncateInputs( qFIS_t * const f );
static float qFIS_ParseFuzzValue( qFIS_MF_t * const mfIO,
                                  qFIS_Rules_t index );
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( const qFIS_t * const f );
static float qFIS_GetNextX( const float init,
                            const float res,
                            const size_t i );
static size_t qFIS_InferenceAntecedent( struct _qFIS_s * const f,
                                        size_t i );
static size_t qFIS_InferenceConsequent( struct _qFIS_s * const f,
                                        size_t i );
static size_t qFIS_InferenceReachEnd( struct _qFIS_s * const f,
                                      size_t i );
static float qFIS_DeFuzz_Centroid( qFIS_Output_t * const o,
                                   const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_Bisector( qFIS_Output_t * const o,
                                   const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_LOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_SOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_MOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_WtAverage( qFIS_Output_t * const o,
                                    const qFIS_DeFuzzState_t stage );
static float qFIS_DeFuzz_WtSum( qFIS_Output_t * const o,
                                const qFIS_DeFuzzState_t stage );
static void qFIS_Aggregate( qFIS_t * const f );

#define QFIS_INFERENCE_ERROR         ( 0U )

/*============================================================================*/
int qFIS_SetParameter( qFIS_t * const f,
                       const qFIS_Parameter_t p,
                       const qFIS_ParamValue_t x )
{
    int retVal = 0;
    typedef float (*methods_fcn)( const float a, const float b );
    static const methods_fcn method[ 5 ] = { &qFIS_Min, &qFIS_Prod, &qFIS_Max,
                                             &qFIS_ProbOR, &qFIS_Sum
                                           };

    if ( NULL != f ) {
        switch ( p ) {
            case qFIS_Implication:
                if ( x <= qFIS_PROD ) {
                    /*cppcheck-suppress misra-c2012-11.1 */
                    f->implicate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_Aggregation:
                if ( ( x >= qFIS_MAX ) && ( x <= qFIS_SUM ) ) {
                    /*cppcheck-suppress misra-c2012-11.1 */
                    f->aggregate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_AND:
                if ( x <= qFIS_PROD ) {
                    /*cppcheck-suppress misra-c2012-11.1 */
                    f->andOp = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_OR:
                if ( ( x >= qFIS_MAX ) && ( x <= qFIS_PROBOR ) ) {
                    /*cppcheck-suppress misra-c2012-11.1 */
                    f->orOp = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_EvalPoints:
                if ( (int)x >= 20 ) {
                    f->nPoints = (size_t)x;
                    retVal = 1;
                }
                break;
            default:
                break;
        }
    }

    return retVal;
}
/*============================================================================*/
int qFIS_SetDeFuzzMethod( qFIS_t * const f,
                          qFIS_DeFuzz_Method_t m )
{
    int retVal = 0;

    static const qFIS_DeFuzz_Fcn_t method[ _NUM_DFUZZ ] = { &qFIS_DeFuzz_Centroid,
                                                            &qFIS_DeFuzz_Bisector,
                                                            &qFIS_DeFuzz_MOM,
                                                            &qFIS_DeFuzz_LOM,
                                                            &qFIS_DeFuzz_SOM,
                                                            &qFIS_DeFuzz_WtAverage,
                                                            &qFIS_DeFuzz_WtSum
                                                          };

    if ( ( NULL != f ) || ( m < _NUM_DFUZZ ) ) {
        if ( ( ( Mamdani == f->type ) && ( m <= som ) ) ||
             ( ( Sugeno == f->type ) && ( m >= wtaver ) && ( m <= wtsum ) ) ||
             ( ( Tsukamoto == f->type ) && ( wtaver == m ) )) {
            /*cppcheck-suppress misra-c2012-11.1 */
            f->deFuzz = method[ m ];
            retVal = 1;
        }
    }
    return retVal;
}
/*============================================================================*/
int qFIS_Setup( qFIS_t * const f,
                const qFIS_Type_t t,
                qFIS_Input_t * const inputs,
                const size_t ni,
                qFIS_Output_t * const outputs,
                const size_t no,
                qFIS_MF_t * const mf_inputs,
                const size_t nmi,
                qFIS_MF_t *const mf_outputs,
                const size_t nmo,
                const qFIS_Rules_t * const r,
                float *wi,
                const size_t n )
{
    int retVal = 0;

    if ( ( NULL != f ) && ( t <= Tsukamoto ) && ( NULL != r ) && ( NULL != wi ) ) {
        size_t i;

        f->nInputs = ni/sizeof(qFIS_Input_t);
        f->nOutputs = no/sizeof(qFIS_Output_t);
        f->nMFInputs = nmi/sizeof(qFIS_MF_t);
        f->nMFOutputs = nmo/sizeof(qFIS_MF_t);
        f->input = inputs;
        f->output = outputs;
        f->inMF = mf_inputs;
        f->outMF = mf_outputs;
        f->wi = wi;
        f->rules = r;
        f->nRules = n;
        f->type = t;
        retVal += qFIS_SetParameter( f, qFIS_EvalPoints, (qFIS_ParamValue_t)100 );
        retVal += qFIS_SetParameter( f, qFIS_AND, qFIS_MIN );
        retVal += qFIS_SetParameter( f, qFIS_OR, qFIS_MAX );
        retVal += qFIS_SetParameter( f, qFIS_Implication, qFIS_MIN );
        retVal += qFIS_SetParameter( f, qFIS_Aggregation, qFIS_MAX );
        /*cppcheck-suppress misra-c2012-10.6 */
        retVal = ( 5 == retVal ) ? 1 : 0;
        f->deFuzz = ( Mamdani == t ) ? &qFIS_DeFuzz_Centroid
                                     : &qFIS_DeFuzz_WtAverage;
        f->ruleWeight = NULL;
        for ( i = 0U ; i < f->nOutputs ; ++i ) {
            /*cstat -CERT-FLP36-C*/
            f->output[ i ].res = ( f->output[ i ].b.max - f->output[ i ].b.min )/(float)f->nPoints;
            /*cstat +CERT-FLP36-C*/
            f->output[ i ].owner = f;
        }
    }

    return retVal;
}
/*============================================================================*/
int qFIS_InputSetup( qFIS_Input_t * const v,
                     const qFIS_Tag_t t,
                     const float min,
                     const float max )
{
    int retVal = 0;

    if ( ( NULL != v ) && ( t >= 0 ) ) {
        v[ t ].b.min = min;
        v[ t ].b.max = max;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_OutputSetup( qFIS_Output_t * const v,
                      const qFIS_Tag_t t,
                      const float min,
                      const float max )
{
    int retVal = 0;

    if ( ( NULL != v ) && ( t >= 0 ) ) {
        v[ t ].data[ 0 ] = 0.0F;
        v[ t ].data[ 1 ] = 0.0F;
        v[ t ].data[ 2 ] = 0.0F;
        v[ t ].data[ 3 ] = 0.0F;
        v[ t ].x = 0.0F;
        v[ t ].y = 0.0F;
        v[ t ].b.min = min;
        v[ t ].b.max = max;
        v[ t ].xag = NULL;
        v[ t ].yag = NULL;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_SetInput( qFIS_Input_t * const v,
                   const qFIS_Tag_t t,
                   const float value )
{
    int retVal = 0;

    if ( ( NULL != v ) && ( t >= 0 ) ) {
        v[ t ].b.value = value;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
float qFIS_GetOutput( const qFIS_Output_t * const v,
                      const qFIS_Tag_t t )
{
    float retVal = 0.0F;

    if ( ( NULL != v ) && ( t >= 0 ) ) {
        retVal = v[ t ].b.value;
    }

    return retVal;
}

/*============================================================================*/
int qFIS_SetMF( qFIS_MF_t * const m,
                const qFIS_Tag_t io,
                const qFIS_Tag_t mf,
                const qFIS_MF_Name_t s,
                qFIS_MF_Fcn_t custom_mf,
                const float *cp,
                float h )
{
    int retVal = 0;
    static const qFIS_MF_Fcn_t fShape[ _NUM_MFS ] = {  &qFIS_ConstantMF,
    /* Conventional membership functions, applies on any antecedent*/
    &qFIS_TriMF, &qFIS_TrapMF, &qFIS_GBellMF, &qFIS_GaussMF, &qFIS_Gauss2MF,
    &qFIS_SigMF, &qFIS_DSigMF, &qFIS_PSigMF, &qFIS_PiMF, &qFIS_SMF, &qFIS_ZMF,
    &qFIS_SingletonMF, &qFIS_ConcaveMF, &qFIS_SpikeMF,
    &qFIS_LinSMF, &qFIS_LinZMF, &qFIS_RectangleMF, &qFIS_CosineMF,
    /* Only for Sugeno consequents*/
    &qFIS_ConstantMF, &qFIS_LinearMF,
    /* Only for Tsukamoto consequents*/
    &qFIS_TLinSMF, &qFIS_TLinZMF, &qFIS_TConcaveMF,&qFIS_TSigMF, &qFIS_TSMF,
    &qFIS_TZMF
    };

    if ( ( NULL != m ) && ( io >= 0 ) && ( mf >= 0 ) && ( s < _NUM_MFS ) ) {
        if ( NULL != custom_mf ) {
            m[ mf ].shape = custom_mf; /*user-defined membership function*/
        }
        else {
            /*cppcheck-suppress misra-c2012-11.1 */
            m[ mf ].shape = fShape[ s ];
        }
        m[ mf ].index = (size_t)io;
        m[ mf ].points = cp;
        m[ mf ].fx = 0.0F;
        m[ mf ].h = qFIS_Bound( h, 0.0F, 1.0F );
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_StoreAggregatedRegion( qFIS_Output_t * const o,
                                const qFIS_Tag_t t,
                                float *x,
                                float *y,
                                const size_t n )
{
    int retVal = 0;

    if ( ( NULL != o ) && ( t >= 0 ) && ( NULL != x ) && ( NULL != y ) ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        qFIS_t *f = (qFIS_t *)o[ t ].owner;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        if ( n >= f->nPoints ) {
            o[ t ].xag = x;
            o[ t ].yag = y;
            retVal = 1;
        }
    }

    return retVal;
}
/*============================================================================*/
static void qFIS_EvalInputMFs( qFIS_t * const f )
{
    size_t i;
    qFIS_MF_t *mf;

    for ( i = 0U ; i < f->nMFInputs ; ++i ) {
        mf = &f->inMF[ i ];
        /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
        /*cppcheck-suppress misra-c2012-11.3 */
        mf->fx = mf->h*mf->shape( (qFIS_IO_Base_t*)&f->input[ mf->index ],
                                  mf->points,
                                  1U );
        /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
    }
}
/*============================================================================*/
static void qFIS_TruncateInputs( qFIS_t * const f )
{
    size_t i;

    for ( i = 0U ; i < f->nInputs ; ++i ) {
        f->input[ i ].b.value = qFIS_Bound( f->input[ i ].b.value,
                                            f->input[ i ].b.min,
                                            f->input[ i ].b.max );
    }
}
/*============================================================================*/
int qFIS_Fuzzify( qFIS_t * const f )
{
    int retVal = 0;

    if ( NULL != f ) {
        qFIS_TruncateInputs( f );
        qFIS_EvalInputMFs( f );
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
static float qFIS_ParseFuzzValue( qFIS_MF_t * const mfIO,
                                  qFIS_Rules_t index )
{
    uint8_t neg = ( index < 0 ) ? 1U : 0U ;
    float y;

    if ( 0U != neg ) {
        index = -index;
    }
    /*cstat -CERT-INT32-C_a*/
    y = qFIS_Bound( mfIO[ index - 1 ].fx, 0.0F, 1.0F );
    /*cstat +CERT-INT32-C_a*/
    /*cppcheck-suppress misra-c2012-12.1 */
    y = ( 0U != neg ) ? ( 1.0F - y ) : y ;

    return y;
}
/*============================================================================*/
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( const qFIS_t * const f )
{
    qFIS_FuzzyOperator_t oper;

    switch ( f->lastConnector ) {
        case _QFIS_AND:
            oper = f->andOp;
            break;
        case _QFIS_OR:
            oper = f->orOp;
            break;
        default:
            oper = &qFIS_Sum;
            break;
    }

    return oper;
}
/*============================================================================*/
static size_t qFIS_InferenceAntecedent( struct _qFIS_s * const f,
                                        size_t i )
{
    qFIS_Rules_t inIndex, MFInIndex, connector;
    qFIS_FuzzyOperator_t op;
    /*cstat -CERT-INT30-C_a*/
    inIndex = f->rules[ i ];
    MFInIndex = f->rules[ i + 1U ];
    connector = f->rules[ i + 2U ];
    /*cstat -CERT-INT30-C_a*/
    op = qFIS_GetFuzzOperator( f );
    f->rStrength = op( f->rStrength, qFIS_ParseFuzzValue( f->inMF, MFInIndex ) );

    if ( ( inIndex < 0 ) || ( (size_t)inIndex > f->nInputs ) ) {
        i = QFIS_INFERENCE_ERROR;
    }
    else {
        if ( ( _QFIS_AND == connector ) || ( _QFIS_OR == connector ) ) {
            f->lastConnector = connector;
            f->inferenceState = &qFIS_InferenceAntecedent;
            i += 2U;
        }
        else if ( _QFIS_THEN == connector ) {
            f->inferenceState = &qFIS_InferenceReachEnd;
            i += 2U;
        }
        else {
            i = QFIS_INFERENCE_ERROR;
        }
    }

    return i;
}
/*============================================================================*/
static size_t qFIS_InferenceReachEnd( struct _qFIS_s * const f,
                                      size_t i )
{
    qFIS_Rules_t  connector;

    connector = ( f->nOutputs > 1U )? f->rules[ i + 2U ] : -1;
    i += 2U;
    if ( _QFIS_AND != connector ) {
        f->inferenceState = &qFIS_InferenceAntecedent;
        f->lastConnector = -1;
        f->wi[ f->ruleCount ] = f->rStrength;
        if ( NULL != f->ruleWeight ) {
            f->wi[ f->ruleCount ] *= qFIS_Bound( f->ruleWeight[ f->ruleCount ], 0.0F, 1.0F );
        }
        f->rStrength = 0.0F;
        ++f->ruleCount;
        --i;
    }

    return i;
}
/*============================================================================*/
static size_t qFIS_AggregationFindConsequent( struct _qFIS_s * const f,
                                              size_t i )
{
    while ( _QFIS_THEN != f->rules[ i++ ] ) {}
    f->aggregationState = &qFIS_InferenceConsequent;
    /*cstat -MISRAC2012-Rule-2.2_c*/
    return --i; /*!ok*/
    /*cstat +MISRAC2012-Rule-2.2_c*/
}
/*============================================================================*/
static size_t qFIS_InferenceConsequent( struct _qFIS_s * const f,
                                        size_t i )
{
    qFIS_Rules_t outIndex, MFOutIndex, connector;
    uint8_t neg = 0U;

    outIndex = f->rules[ i ];
    MFOutIndex = f->rules[ i + 1U ];
    connector = ( f->nOutputs > 1U )? f->rules[ i + 2U ] : -1;
    if ( MFOutIndex < 0 ) {
        MFOutIndex = -MFOutIndex;
        neg = 1U;
    }
    MFOutIndex -= 1;

    if ( f->wi[ f->ruleCount ] > 0.0F ) {
        qFIS_Output_t *o = &f->output[ outIndex ];
        qFIS_MF_t *m = &f->outMF[ MFOutIndex ];

        if ( Mamdani == f->type ) {
            float v;
            /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
            /*cppcheck-suppress misra-c2012-11.3 */
            v = m->h*m->shape( (qFIS_IO_Base_t*)o, m->points, 1U );
            /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
            /*cppcheck-suppress misra-c2012-12.1 */
            v = ( 1U == neg )? ( 1.0F - v ) : v;
            o->y = f->aggregate( o->y, f->implicate( f->wi[ f->ruleCount ], v ) );
        }
        else { /* Sugeno and Tsukamoto*/
            float zi;
            /*cstat -MISRAC2012-Rule-11.3 -CERT-EXP39-C_d*/
            /*cppcheck-suppress misra-c2012-11.3 */
            zi = m->shape( (qFIS_IO_Base_t*)f->input, m->points, f->nInputs );
            /*cstat +MISRAC2012-Rule-11.3 +CERT-EXP39-C_d*/
            o->data[ 0 ] += zi*f->wi[ f->ruleCount ];
            o->data[ 1 ] += f->wi[ f->ruleCount ];
        }
    }

    i += 2U;
    if ( _QFIS_AND != connector ) {
        f->aggregationState = &qFIS_AggregationFindConsequent;
        ++f->ruleCount;
        --i;
    }

    return i;
}
/*============================================================================*/
static void qFIS_Aggregate( qFIS_t * const f )
{
    if ( NULL != f ) {
        if ( QFIS_RULES_BEGIN == f->rules[ 0 ] ) {
            size_t i = 1U;

            f->aggregationState = &qFIS_AggregationFindConsequent;
            f->ruleCount = 0U;
            while ( ( _QFIS_RULES_END != f->rules[ i ] ) && ( f->ruleCount < f->nRules ) ) {
                i = f->aggregationState( f, i );
                if ( QFIS_INFERENCE_ERROR == i ) {
                    break;
                }
                ++i;
            }
        }
    }
}
/*============================================================================*/
static float qFIS_DeFuzz_Centroid( qFIS_Output_t * const o,
                                   const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    switch ( stage ) {
        case DeFuzz_Compute:
            o->data[ 0 ] += o->x*o->y;
            o->data[ 1 ] += o->y;
            break;
        case DeFuzz_Init:
            o->data[ 0 ] = 0.0F; /*store sum(y)*/
            o->data[ 1 ] = 0.0F; /*store sum(x*y)*/
            break;
        case DeFuzz_End:
            d = o->data[ 0 ]/o->data[ 1 ]; /*sum(x*y)/sum(y)*/
            break;
        default:
            break;
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_Bisector( qFIS_Output_t * const o,
                                   const qFIS_DeFuzzState_t stage )
{
    size_t k;
    float d = 0.0F;
    qFIS_t *f;

    switch ( stage ) {
        case DeFuzz_Compute:
            o->data[ 0 ] += o->y;
            break;
        case DeFuzz_Init:
            o->data[ 0 ] = 0.0F; /*store sum(y)*/
            break;
        case DeFuzz_End:
            o->data[ 1 ] = 0.0F;
            /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
            /*cppcheck-suppress misra-c2012-11.5 */
            f = (qFIS_t *)o->owner;
            /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
            for ( k = 0U ; k < f->nPoints ; ++k ) {
                o->y = 0.0F;
                o->x = qFIS_GetNextX( o->b.min, o->res, k );
                o->b.value = o->x;
                qFIS_Aggregate( f );
                o->data[ 1 ] += o->y;
                if ( o->data[ 1 ] >= ( 0.5F*o->data[ 0 ] ) ) {
                    break;
                }
            }
            d = o->x;
            break;
        default:
            break;
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_LOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    switch ( stage ) {
        case DeFuzz_Compute:
            if ( o->y >= o->data[ 0 ] ) {
                o->data[ 0 ] = o->y;
                o->data[ 1 ] = o->x;
            }
            break;
        case DeFuzz_Init:
            o->data[ 0 ] = -1.0F; /*yMax*/
            o->data[ 1 ] = o->b.max; /*xLargest*/
            break;
        case DeFuzz_End:
            d = o->data[ 1 ];
            break;
        default:
            break;
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_SOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    switch ( stage ) {
        case DeFuzz_Compute:
            if ( o->y > o->data[ 0 ] ) {
                o->data[ 0 ] = o->y;
                o->data[ 1 ] = o->x;
            }
            break;
        case DeFuzz_Init:
            o->data[ 0 ] = -1.0F; /*yMax*/
            o->data[ 1 ] = o->b.min; /*xSmallest*/
            break;
        case DeFuzz_End:
            d = o->data[ 1 ];
            break;
        default:
            break;
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_MOM( qFIS_Output_t * const o,
                              const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    switch ( stage ) {
        case DeFuzz_Compute:
            if ( o->y > o->data[ 0 ] ) {
                o->data[ 0 ] = o->y;
                o->data[ 1 ] = o->x;
                o->data[ 2 ] = o->x;
                o->data[ 3 ] = 1.0F;
            }
            else if ( qFFMath_IsEqual( o->y , o->data[ 0 ] ) && ( o->data[ 3 ] > 0.0F ) ) {
                o->data[ 2 ] = o->x;
            }
            else if ( o->y < o->data[ 0 ] ) {
                o->data[ 3 ] = -1.0F;
            }
            else {
                /*nothing to do*/
            }
            break;
        case DeFuzz_Init:
            o->data[ 0 ] = -1.0F; /* yMax */
            o->data[ 1 ] = o->b.min; /*xSmallest*/
            o->data[ 2 ] = o->b.max; /*xLargest*/
            o->data[ 3 ] = -1.0F; /*sp*/
            break;
        case DeFuzz_End:
            d = 0.5F*( o->data[ 1 ] + o->data[ 2 ] );
            break;
        default:
            break;
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_WtAverage( qFIS_Output_t * const o,
                                    const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    if ( DeFuzz_End == stage ) {
        d = o->data[ 0 ]/o->data[ 1 ];
    }

    return d;
}
/*============================================================================*/
static float qFIS_DeFuzz_WtSum( qFIS_Output_t * const o,
                                const qFIS_DeFuzzState_t stage )
{
    float d = 0.0F;

    if ( DeFuzz_End == stage ) {
        d = o->data[ 0 ];
    }

    return d;
}
/*============================================================================*/
int qFIS_DeFuzzify( qFIS_t * const f )
{
    int retVal = 0;

    if ( NULL != f ) {
        size_t i;

        for ( i = 0U; i < f->nOutputs ; ++i ) {
            f->deFuzz( &f->output[ i ] , DeFuzz_Init );
        }

        if ( Mamdani == f->type  ) {
            size_t k;

            for ( k = 0U ; k < f->nPoints ; ++k ) {
                for ( i = 0U; i < f->nOutputs ; ++i ) { /* initialize*/
                    f->output[ i ].y = 0.0F;
                    f->output[ i ].x = qFIS_GetNextX( f->output[ i ].b.min,
                                                      f->output[ i ].res,
                                                      k );
                    f->output[ i ].b.value = f->output[ i ].x;
                }
                qFIS_Aggregate( f );
                for ( i = 0U; i < f->nOutputs ; ++i ) {
                    f->deFuzz( &f->output[ i ] , DeFuzz_Compute );
                    if ( NULL != f->output[ i ].xag ) { /*store aggregated*/
                        f->output[ i ].xag[ k ] = f->output[ i ].x;
                        f->output[ i ].yag[ k ] = f->output[ i ].y;
                    }
                }
            }
        }
        else { /*Sugeno and Tsukamoto systems*/
            for ( i = 0U; i < f->nOutputs ; ++i ) { /* initialize*/
                f->output[ i ].data[ 0 ] = 0.0F; /*store sum wi*/
                f->output[ i ].data[ 1 ] = 0.0F; /*store sum zi*wi*/
            }
            qFIS_Aggregate( f );
            for ( i = 0U; i < f->nOutputs ; ++i ) { /* initialize*/
                f->deFuzz( &f->output[ i ] , DeFuzz_Compute );
            }
        }

        for ( i = 0U; i < f->nOutputs ; ++i ) {
            f->output[ i ].b.value = f->deFuzz( &f->output[ i ] , DeFuzz_End );
            f->output[ i ].b.value = qFIS_Bound( f->output[ i ].b.value,
                                                 f->output[ i ].b.min,
                                                 f->output[ i ].b.max );
        }
    }

    return retVal;
}
/*============================================================================*/
int qFIS_SetRuleWeights( qFIS_t * const f,
                         float *rWeights )
{
    int retVal = 0;

    if ( NULL != f ) {
        f->ruleWeight = rWeights;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_Inference( qFIS_t * const f )
{
    int retVal = 0;

    if ( NULL != f  ) {
        size_t i = 0U;

        if ( QFIS_RULES_BEGIN == f->rules[ 0 ] ) {
            f->inferenceState = &qFIS_InferenceAntecedent;
            f->rStrength = 0.0F;
            f->lastConnector = -1;
            f->ruleCount = 0U;
            i = 1U;
            while ( ( _QFIS_RULES_END != f->rules[ i ] ) && ( f->ruleCount < f->nRules ) ) {
                i = f->inferenceState( f, i );
                if ( QFIS_INFERENCE_ERROR == i ) {
                    break;
                }
                ++i;
            }
        }
        if ( ( _QFIS_RULES_END == f->rules[ i ] ) && ( f->ruleCount == f->nRules) ) {
            retVal = 1;
        }
    }

    return retVal;
}
/*============================================================================*/
static float qFIS_GetNextX( const float init,
                            const float res,
                            const size_t i )
{
    /*cstat -CERT-FLP36-C*/
    return init + ( ( (float)i + 0.5F )*res );
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static float qFIS_TriMF( const qFIS_IO_Base_t * const in,
                         const float *p,
                         const size_t n )
{
    float a, b, c, tmp;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    c = p[ 2 ];
    tmp = qFIS_Min( ( x - a )/( b - a ) , ( c - x )/( c - b ) );

    return qFIS_Max( tmp , 0.0F );
}
/*============================================================================*/
static float qFIS_TrapMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    float a, b, c, d, tmp;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    c = p[ 2 ];
    d = p[ 3 ];
    tmp = qFIS_Min( ( x - a )/( b - a ) , 1.0F );
    tmp = qFIS_Min( tmp, ( d - x )/( d - c ) ) ;

    return qFIS_Max( tmp , 0.0F );
}
/*============================================================================*/
static float qFIS_GBellMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n )
{
    float a, b, c;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    c = p[ 2 ];

    return ( 1.0F/( 1.0F + QLIB_POW( QLIB_ABS( ( x - c )/a ) , 2.0F*b ) ) );
}
/*============================================================================*/
static float qFIS_GaussMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n )
{
    float a, c, tmp;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    c = p[ 1 ];
    tmp = ( x - c )/a;

    return QLIB_EXP( -0.5F*tmp*tmp );
}
/*============================================================================*/
static float qFIS_Gauss2MF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n )
{
    float c1, c2, f1, f2;
    float x = in[ 0 ].value;

    c1 = p[ 1 ];
    c2 = p[ 3 ];
    f1 = ( x <= c1 ) ? qFIS_GaussMF( in , p, n ) : 1.0F;
    f2 = ( x <= c2 ) ? qFIS_GaussMF( in , &p[ 2 ], n ) : 1.0F;

    return f1*f2;
}
/*============================================================================*/
static float qFIS_SigMF( const qFIS_IO_Base_t * const in,
                         const float *p,
                         const size_t n )
{
    float a, b;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];

    return 1.0F/( 1.0F + QLIB_EXP( -a*( x - b ) ) );
}
/*============================================================================*/
static float qFIS_TSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    float a, b, y;
    float x = in[ 0 ].value;
    float min = in[ 0 ].min;
    float max = in[ 0 ].max;
    (void)n;

    a = p[ 0 ]; /*slope*/
    b = p[ 1 ]; /*inflection*/
    if ( qFFMath_IsEqual( x, 1.0F ) ) {
        if ( a >= 0.0F ) {
            y = max;
        }
        else {
            y = min;
        }
    }
    else if ( qFFMath_IsEqual( x, 0.0F ) ) {
        if ( a >= 0.0F ) {
            y = min;
        }
        else {
            y = max;
        }
    }
    else {
        /*cstat -MISRAC2012-Dir-4.11_a*/
        y = b - ( QLIB_LOG( ( 1.0F/x ) - 1.0F )/a );
        /*cstat +MISRAC2012-Dir-4.11_a*/
    }

    return y;
}
/*============================================================================*/
static float qFIS_DSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    return QLIB_ABS( qFIS_SigMF( in , p, n ) - qFIS_SigMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_PSigMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    return QLIB_ABS( qFIS_SigMF( in , p, n )*qFIS_SigMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_SMF( const qFIS_IO_Base_t * const in,
                       const float *p,
                       const size_t n )
{
    float a, b, tmp, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( x <= a ) {
        y =  0.0F;
    }
    else if ( x >= b ) {
        y = 1.0F;
    }
    else if ( ( x >= a ) && ( x <= ( ( a + b )*0.5F ) ) ) {
        tmp = ( x - a )/( b - a );
        y = 2.0F*tmp*tmp;
    }
    else if ( ( x <= b ) && ( x >= ( ( a + b )*0.5F ) ) ) {
        tmp = ( x - b )/( b - a );
        y = ( 1.0F - ( 2.0F*tmp*tmp ) );
    }
    else {
        y = 0.0F;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TSMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n )
{
    float diff, a, b, ta, tb, ma, mb;
    float x = in[ 0 ].value;
    (void)n;
    qFIS_IO_Base_t tmp = { 0.0F, 0.0F, 0.0F };

    a = p[ 0 ]; /*start*/
    b = p[ 1 ]; /*end*/
    diff = b - a;
    diff = 0.5F*diff*diff;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Dir-4.11_b*/
    ta = a + QLIB_SQRT( x*diff );
    tmp.value = ta;
    ma = qFIS_SMF( &tmp, p, n );
    tb = b + QLIB_SQRT( -( x - 1.0F )*diff );
    tmp.value = tb;
    mb = qFIS_SMF( &tmp, p, n );
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Dir-4.11_b*/
    return  ( QLIB_ABS( x - ma ) < QLIB_ABS( x - mb ) ) ? ta : tb;
}
/*============================================================================*/
static float qFIS_ZMF( const qFIS_IO_Base_t * const in,
                       const float *p,
                       const size_t n )
{
    float a, b, tmp, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( x <= a ) {
        y = 1.0F;
    }
    else if ( x >= b ) {
        y = 0.0F;
    }
    else if ( ( x >= a ) && ( x <= ( ( a + b )*0.5F ) ) ) {
        tmp = ( x - a )/( b - a );
        y = 1.0F - ( 2.0F*tmp*tmp );
    }
    else if ( ( x <= b ) && ( x >= ( ( a + b )*0.5F ) ) ) {
        tmp = ( x - b )/( b - a );
        y = 2.0F*tmp*tmp;
    }
    else {
        y = 0.0F;
    }

    return y;
}
/*============================================================================*/
static float qFIS_LinSMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    float a, b, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( a < b ) {
        if ( x < a ) {
            y = 0.0F;
        }
        else if ( x > b ) {
            y = 1.0F;
        }
        else {
            y = ( x - a )/( b - a );
        }
    }
    else if ( qFFMath_IsEqual( a, b ) ) {
        y = ( x < a ) ? 0.0F : 1.0F;
    }
    else {
        y = 0.0F;
    }

    return y;
}
/*============================================================================*/
static float qFIS_LinZMF( const qFIS_IO_Base_t * const in,
                          const float *p,
                          const size_t n )
{
    float a, b, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( a < b ) {
        if ( x < a ) {
            y = 1.0F;
        }
        else if ( x > b ) {
            y = 0.0F;
        }
        else {
            y = ( a - x )/( a - b );
        }
    }
    else if ( qFFMath_IsEqual( a, b ) ) {
        y = ( x < a ) ? 1.0F : 0.0F;
    }
    else {
        y = 0.0F;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TZMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n )
{
    float diff, a, b, ta, tb, ma, mb;
    float x = in[ 0 ].value;
    (void)n;
    qFIS_IO_Base_t tmp = { 0.0F, 0.0F, 0.0F };

    a = p[ 0 ]; /*start*/
    b = p[ 1 ]; /*end*/
    diff = b - a;
    diff = 0.5F*diff*diff;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Dir-4.11_b*/
    ta = a + QLIB_SQRT( -( x - 1.0F )*diff );
    tmp.value = ta;
    ma = qFIS_SMF( &tmp, p, n );
    tb = b + QLIB_SQRT( x*diff );
    tmp.value = tb;
    mb = qFIS_SMF( &tmp, p, n );
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Dir-4.11_b*/
    return  ( QLIB_ABS( x - ma ) < QLIB_ABS( x - mb ) ) ? ta : tb;
}
/*============================================================================*/
static float qFIS_PiMF( const qFIS_IO_Base_t * const in,
                        const float *p,
                        const size_t n )
{
    return QLIB_ABS( qFIS_SMF( in , p, n )*qFIS_ZMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_SingletonMF( const qFIS_IO_Base_t * const in,
                               const float *p,
                               const size_t n )
{
    float x = in[ 0 ].value;
    (void)n;

    return ( qFFMath_IsEqual( x, p[ 0 ] ) ) ? 1.0F : 0.0F;
}
/*============================================================================*/
static float qFIS_ConcaveMF( const qFIS_IO_Base_t * const in,
                             const float *p,
                             const size_t n )
{
    float x = in[ 0 ].value;
    float i, e, y;
    (void)n;

    i = p[ 0 ];
    e = p[ 1 ];
    if ( ( i <= e ) && ( x < e ) ) {
        y = ( e - i )/( ( 2.0F*e ) - i -x );
    }
    else if ( ( i > e ) && ( x > e ) ) {
        y = ( i - e )/( -( 2.0F*e ) + i +x );
    }
    else {
        y = 1.0F;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TConcaveMF( const qFIS_IO_Base_t * const in,
                              const float *p,
                              const size_t n )
{
    float i, e;

    i = p[ 0 ];
    e = p[ 1 ];

    return ( ( i - e )/qFIS_ConcaveMF( in, p, n ) ) + ( 2.0F*e ) - i;
}
/*============================================================================*/
static float qFIS_SpikeMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n )
{
    float x = in[ 0 ].value;
    float w, c;
    (void)n;

    w = p[ 0 ];
    c = p[ 1 ];

    return QLIB_EXP( -QLIB_ABS( 10.0F*( x - c )/w ) );
}
/*============================================================================*/
static float qFIS_TLinSMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n )
{
    float x = in[ 0 ].value;
    float a, b;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];

    return ( ( b - a )*x ) + a;
}
/*============================================================================*/
static float qFIS_TLinZMF( const qFIS_IO_Base_t * const in,
                           const float *p,
                           const size_t n )
{
    float x = in[ 0 ].value;
    float a, b;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];

    return + a - ( ( a - b )*x );
}
/*============================================================================*/
static float qFIS_RectangleMF( const qFIS_IO_Base_t * const in,
                               const float *p,
                               const size_t n )
{
    float x = in[ 0 ].value;
    float s, e;
    (void)n;

    s = p[ 0 ];
    e = p[ 1 ];

    return ( ( x >= s ) && ( x <= e ) ) ? 1.0F : 0.0F;
}
/*============================================================================*/
static float qFIS_CosineMF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n )
{
    float x = in[ 0 ].value;
    float c, w, y;
    const float pi = 3.14159265358979323846F;
    (void)n;

    c = p[ 0 ];
    w = p[ 1 ];
    if ( ( x < ( c - ( 0.5F*w ) ) ) || ( x > ( c + ( 0.5F*w ) ) ) ) {
        y = 0.0F;
    }
    else {
        y = 0.5F*( 1.0F + QLIB_COS( 2.0F/w*pi*( x - c) ) );
    }

    return y;
}
/*============================================================================*/
static float qFIS_ConstantMF( const qFIS_IO_Base_t * const in,
                              const float *p,
                              const size_t n )
{
    (void)in;
    (void)n;
    return p[ 0 ];
}
/*============================================================================*/
static float qFIS_LinearMF( const qFIS_IO_Base_t * const in,
                            const float *p,
                            const size_t n )
{
    float px = 0.0F;
    size_t i;

    for ( i = 0U ; i < n ; ++i ) {
        px += in[ i ].value*p[ i ];
    }
    px += p[ i ];

    return px;
}
/*============================================================================*/
static float qFIS_Min( const float a,
                       const float b )
{
    return qFIS_Bound( ( a < b ) ? a : b, 0.0F, 1.0F );
}
/*============================================================================*/
static float qFIS_Max( const float a,
                       const float b )
{
    return qFIS_Bound( ( a > b ) ? a : b, 0.0F, 1.0F );
}
/*============================================================================*/
static float qFIS_Prod( const float a,
                        const float b )
{
    return qFIS_Bound( a*b, 0.0F, 1.0F );
}
/*============================================================================*/
static float qFIS_ProbOR( const float a,
                          const float b )
{
    return qFIS_Bound( a + b - ( a*b ), 0.0F, 1.0F );
}
/*============================================================================*/
static float qFIS_Sum( const float a,
                       const float b )
{
    return qFIS_Bound( a + b, 0.0F, 1.0F );
}
/*============================================================================*/
static float qFIS_Bound( float y,
                         const float yMin,
                         const float yMax )
{
    (void)qFFMath_InRangeCoerce( &y, yMin, yMax );

    return y;
}
/*============================================================================*/
