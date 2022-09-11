/*!
 * @file qfis.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qfis.h"
#include <math.h>

typedef float (*qFIS_FuzzyOperator_t)( const float a, const float b );

static float qFIS_TriMF( const qFIS_IO_t * const in,
                         const float *p,
                         const size_t n );
static float qFIS_TrapMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_GBellMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_GaussMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_Gauss2MF( const qFIS_IO_t * const in,
                            const float *p,
                            const size_t n );
static float qFIS_SigMF( const qFIS_IO_t * const in,
                         const float *p,
                         const size_t n );
static float qFIS_TSigMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_DSigMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_PSigMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_SMF( const qFIS_IO_t * const in,
                       const float *p,
                       const size_t n );
static float qFIS_TSMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_ZMF( const qFIS_IO_t * const in,
                       const float *p,
                       const size_t n );
static float qFIS_TZMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_PiMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n );
static float qFIS_SingletonMF( const qFIS_IO_t * const in,
                               const float *p,
                               const size_t n );
static float qFIS_ConcaveMF( const qFIS_IO_t * const in,
                             const float *p,
                             const size_t n );
static float qFIS_TConcaveMF( const qFIS_IO_t * const in,
                              const float *p,
                              const size_t n );
static float qFIS_SpikeMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_RampMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n );
static float qFIS_TRampMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n );
static float qFIS_RectangleMF( const qFIS_IO_t * const in,
                               const float *p,
                               const size_t n );
static float qFIS_CosineMF( const qFIS_IO_t * const in,
                            const float *p,
                            const size_t n );
static float qFIS_ConstantMF( const qFIS_IO_t * const in,
                              const float *p,
                              const size_t n );
static float qFIS_LinearMF( const qFIS_IO_t * const in,
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
static float qFIS_Sat( float y );

static void qFIS_EvalInputMFs( qFIS_t * const f );
static void qFIS_TruncateInputs( qFIS_t * const f );
static float qFIS_ParseFuzzValue( qFIS_MF_t * const mfIO,
                                  qFIS_Rules_t index );
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( qFIS_t * const f );

static float qFIS_OutputAggregate( qFIS_t * const f,
                                   const int tag,
                                   const float xi );
static float qFIS_GetResolution( const qFIS_t * const f,
                                 const int tag );
static float qFIS_GetNextX( const float init,
                            const float res,
                            const size_t i );
static int qFIS_DeFuzzCentroid( qFIS_t * const f );
static int qFIS_DeFuzzBisector( qFIS_t * const f );
static int qFIS_DeFuzzLOM( qFIS_t * const f );
static int qFIS_DeFuzzSOM( qFIS_t * const f );
static int qFIS_DeFuzzMOM( qFIS_t * const f );
static int qFIS_DeFuzzWtAverage( qFIS_t * const f );
static int qFIS_DeFuzzWtSum( qFIS_t * const f );

static size_t qFIS_InferenceAntecedent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i );
static size_t qFIS_InferenceConsequent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i );
static void qFIS_InferenceInit( qFIS_t * const f );

#define QFIS_INFERENCE_ERROR         ( 0u )

/*============================================================================*/
int qFIS_SetParameter( qFIS_t * const f,
                       const qFIS_Parameter_t p,
                       const qFIS_ParamValue_t x )
{
    int retVal = 0;
    typedef float (*methods_fcn)( const float a, const float b );
    static const methods_fcn method[ 5 ] = { &qFIS_Min, &qFIS_Prod, &qFIS_Max,
                                             &qFIS_ProbOR, &qFIS_Sum };

    if ( NULL != f ) {
        switch( p ) {
            case qFIS_Implication:
                if ( x <= qFIS_PROD ) {
                    f->implicate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_Aggregation:
                if ( ( x >= qFIS_MAX ) && ( x <= qFIS_SUM ) ) {
                    f->aggregate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_AND:
                if ( x <= qFIS_PROD ) {
                    f->andOp = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_OR:
                if ( ( x >= qFIS_MAX ) && ( x <= qFIS_PROBOR ) ) {
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
    typedef int (*deFuzz_fcn_t)( qFIS_t * const f );
    static const deFuzz_fcn_t method[ 7 ] = { &qFIS_DeFuzzCentroid,
                                              &qFIS_DeFuzzBisector, 
                                              &qFIS_DeFuzzMOM, &qFIS_DeFuzzLOM,
                                              &qFIS_DeFuzzSOM, 
                                              &qFIS_DeFuzzWtAverage,
                                              &qFIS_DeFuzzWtSum
                                             };

    if ( ( NULL != f ) || ( m < wtsum ) ) {
        
        if ( ( ( Mamdani == f->type ) && ( m < SOM ) ) ||
             ( ( Sugeno == f->type ) && ( m >= wtaver ) && ( m <= wtsum ) ) ||
             ( ( Tsukamoto == f->type ) && ( wtaver == m ) )) {
            f->deFuzz = method[ m ];
            retVal = 1;
        }
    }

    return retVal;
}
/*============================================================================*/
int qFIS_Setup( qFIS_t * const f,
                const qFIS_Type_t t,
                qFIS_IO_t * const inputs,
                const size_t ni,
                qFIS_IO_t * const outputs,
                const size_t no,
                qFIS_MF_t * const mf_inputs,
                const size_t nmi,
                qFIS_MF_t *const mf_outputs,
                const size_t nmo )
{
    int retVal = 0;
  
    if ( ( NULL != f ) && ( t <= Tsukamoto ) ) {
        f->type = t;
        f->nInputs = ni/sizeof(qFIS_IO_t);
        f->nOutputs = no/sizeof(qFIS_IO_t);
        f->nMFInputs = nmi/sizeof(qFIS_MF_t);
        f->nMFOutputs = nmo/sizeof(qFIS_MF_t);
        f->input = inputs;
        f->output = outputs;
        f->inMF = mf_inputs;
        f->outMF = mf_outputs;
        f->mUnion = &qFIS_Max;
        retVal += qFIS_SetParameter( f, qFIS_EvalPoints, (qFIS_ParamValue_t)100 );
        retVal += qFIS_SetParameter( f, qFIS_AND, qFIS_MIN );
        retVal += qFIS_SetParameter( f, qFIS_OR, qFIS_MAX );
        retVal += qFIS_SetParameter( f, qFIS_Implication, qFIS_MIN );
        retVal += qFIS_SetParameter( f, qFIS_Aggregation, qFIS_MAX );
        retVal = ( retVal == 5 ) ? 1 : 0;
        f->deFuzz = ( Mamdani == t ) ? &qFIS_DeFuzzCentroid : &qFIS_DeFuzzWtAverage;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_SetIO( qFIS_IO_t * const v,
                const qFIS_Tag_t tag,
                const float min,
                const float max )
{
    int retVal = 0;

    if ( ( NULL != v ) && ( tag >= 0 ) ) {
        v[ tag ].min = min;
        v[ tag ].max = max;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
int qFIS_SetMF( qFIS_MF_t * const m,
                const qFIS_Tag_t io_tag,
                const qFIS_Tag_t mf_tag,
                const qFIS_MF_Name_t shape,
                qFIS_MF_Fcn_t custom_mf,
                const float *cp,
                float h )
{
    int retVal = 0;
    static const qFIS_MF_Fcn_t fShape[ 25 ] = { &qFIS_ConstantMF,
                                                &qFIS_TriMF, &qFIS_TrapMF,
                                                &qFIS_GBellMF, &qFIS_GaussMF,
                                                &qFIS_Gauss2MF, &qFIS_SigMF,
                                                &qFIS_DSigMF, &qFIS_PSigMF,
                                                &qFIS_PiMF, &qFIS_SMF,
                                                &qFIS_ZMF, &qFIS_SingletonMF,
                                                &qFIS_ConcaveMF, &qFIS_SpikeMF,
                                                &qFIS_RampMF, &qFIS_RectangleMF,
                                                &qFIS_CosineMF,
                                                &qFIS_ConstantMF, &qFIS_LinearMF,
                                                &qFIS_TRampMF, &qFIS_TConcaveMF,
                                                &qFIS_TSigMF, &qFIS_TSMF,
                                                &qFIS_TZMF };

    if ( ( NULL != m ) && ( io_tag >= 0 ) && ( mf_tag >= 0 ) && ( shape <= tzmf ) ) {
        if ( NULL != custom_mf ) {
            m[ mf_tag ].shape = custom_mf; /*user-defined membership function*/
        }
        else {
            m[ mf_tag ].shape = fShape[ shape ];
        }
        if ( h > 1.0f ) {
            h = 1.0f;
        }
        if ( h < 0.0f ) {
            h = 0.0f;
        }
        m[ mf_tag ].index = (size_t)io_tag;
        m[ mf_tag ].points = cp;
        m[ mf_tag ].fx = 0.0f;
        m[ mf_tag ].h = h;
        retVal = 1;
    }

    return retVal;
}
/*============================================================================*/
static void qFIS_EvalInputMFs( qFIS_t * const f )
{
    size_t i;
    qFIS_MF_t *mf;

    for ( i = 0 ; i < f->nMFInputs ; ++i ) {
        mf = &f->inMF[ i ];
        mf->fx = mf->h*mf->shape( &f->input[ mf->index ], mf->points, 1u );
    }
}
/*============================================================================*/
static void qFIS_TruncateInputs( qFIS_t *f )
{
    size_t i;

    for ( i = 0 ; i < f->nInputs ; ++i ) {
        if ( f->input[ i ].value > f->input[ i ].max ) {
            f->input[ i ].value = f->input[ i ].max;
        }
        if ( f->input[ i ].value < f->input[ i ].min ) {
            f->input[ i ].value = f->input[ i ].min;
        }
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
    uint8_t neg = (uint8_t)( index < 0 );
    float y;

    if ( 0u != neg ) {
        index = -index;
    }
    /*cstat -CERT-INT32-C_a*/
    y = qFIS_Sat( mfIO[ index - 1 ].fx );
    /*cstat +CERT-INT32-C_a*/
    y = ( 0u != neg ) ? ( 1.0f - y ) : y ;

    return y;
}
/*============================================================================*/
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( qFIS_t * const f )
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
static size_t qFIS_InferenceAntecedent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i )
{
    int16_t inIndex, MFInIndex, connector;
    qFIS_FuzzyOperator_t oper;
    /*cstat -CERT-INT30-C_a*/
    inIndex = r[ i ];
    MFInIndex = r[ i + 1u ];
    connector = r[ i + 2u ];
    /*cstat -CERT-INT30-C_a*/
    oper = qFIS_GetFuzzOperator( f );
    f->rStrength = oper( f->rStrength,
                         qFIS_ParseFuzzValue( f->inMF, MFInIndex ) );

    if ( ( inIndex < 0 ) || ( (size_t)inIndex > f->nInputs ) ) {
        i = QFIS_INFERENCE_ERROR;
    }
    else {
        if ( ( _QFIS_AND == connector ) || ( _QFIS_OR == connector ) ) {
            f->lastConnector = connector;
            f->inferenceState = &qFIS_InferenceAntecedent;
            i += 2u;
        }
        else if ( _QFIS_THEN == connector ) {
            f->inferenceState = &qFIS_InferenceConsequent;
            i += 2u;
        }
        else {
            i = QFIS_INFERENCE_ERROR;
        }
    }

    return i;
}
/*============================================================================*/
static size_t qFIS_InferenceConsequent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i )
{
    int16_t outIndex, MFOutIndex, connector;
    float zi;

    outIndex = r[ i ];
    MFOutIndex = r[ i + 1u ] - 1;
    connector = ( f->nOutputs > 1u )? r[ i + 2u ] : -1;
    i += 2u;

    switch ( f->type ) {
        case Mamdani:
            f->outMF[ MFOutIndex ].fx = f->aggregate( f->outMF[ MFOutIndex ].fx,
                                                      f->rStrength );
            break;
        case Sugeno: case Tsukamoto:
            zi = f->outMF[ MFOutIndex ].shape( f->input,
                                               f->outMF[ MFOutIndex ].points,
                                               f->nInputs );
            f->output[ outIndex ].zi_wi += zi*f->rStrength;
            f->output[ outIndex ].wi += f->rStrength;
            break;
        default:
            break;
    }

    if ( _QFIS_AND != connector ) {
        f->inferenceState = &qFIS_InferenceAntecedent;
        f->lastConnector = -1;
        f->rStrength = 0.0f;
        ++f->ruleCount;
        i--;
    }

    return i;
}
/*============================================================================*/
static void qFIS_InferenceInit( qFIS_t * const f )
{
    size_t i;

    f->inferenceState = &qFIS_InferenceAntecedent;
    f->rStrength = 0.0f;
    f->lastConnector = -1;
    f->ruleCount = 0;

    for ( i = 0 ; i < f->nOutputs ; ++i ) {
        f->output[ i ].zi_wi = 0.0f;
        f->output[ i ].wi = 0.0f;
    }
    
}
/*============================================================================*/
int qFIS_Inference( qFIS_t * const f,
                    const qFIS_Rules_t * const r )
{
    int retVal = 0;

    if ( ( NULL != f ) && ( NULL != r ) ) {
        size_t i = 0u;

        if ( QFIS_RULES_BEGIN == r[ 0 ] ) {
            qFIS_InferenceInit( f );
            i = 1u;

            while ( _QFIS_RULES_END != r[ i ] ) {
                i = f->inferenceState( f, r, i );
                if ( QFIS_INFERENCE_ERROR == i ) {
                    break;
                }
                ++i;
            }
        }

        retVal = ( _QFIS_RULES_END == r[ i ] ) ? f->ruleCount : 0;
    }

    return retVal;
}
/*============================================================================*/
static float qFIS_OutputAggregate( qFIS_t * const f,
                                   const int tag,
                                   const float xi )
{
    float z, fx = 0.0f;
    size_t j;
    qFIS_IO_t x;
    
    x.value = xi;
    for ( j = 0u ; j < f->nMFOutputs ; ++j ) {
        if ( f->outMF[ j ].index == (size_t)tag ) {
            z = f->outMF[ j ].shape( &x, f->outMF[ j ].points, 1u );
            z *= f->outMF[ j ].h;
            fx = f->mUnion( fx, f->implicate( z, f->outMF[ j ].fx ) );
        }
    }

    return fx;
}
/*============================================================================*/
static float qFIS_GetResolution( const qFIS_t * const f,
                                 const int tag )
{
    /*cstat -CERT-FLP36-C*/
    return ( ( f->output[ tag ].max - f->output[ tag ].min )/(float)f->nPoints );
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static float qFIS_GetNextX( const float init,
                            const float res,
                            const size_t i )
{
    /*cstat -CERT-FLP36-C*/
    return init + ( (float)i*res );
    /*cstat +CERT-FLP36-C*/
}
/*============================================================================*/
static int qFIS_DeFuzzCentroid( qFIS_t * const f )
{
    size_t i;
    float x, fx, int_Fx, int_xFx, res;
    int tag;

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        int_Fx = 0.0f;
        int_xFx = 0.0f;
        res = qFIS_GetResolution( f, tag );
        for ( i = 0u ; i < ( f->nPoints + 1u ) ; ++i ) {
            x = qFIS_GetNextX( f->output[ tag ].min, res, i );
            fx = qFIS_OutputAggregate( f, tag, x );
            int_xFx += x*fx;
            int_Fx += fx;
        }
        f->output[ tag ].value = ( int_xFx/int_Fx );
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzBisector( qFIS_t * const f )
{
    size_t i;
    float fx, res;
    int tag;
    struct bisector_s {
        float init, x, a, sign;
        size_t i;
    };

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        struct bisector_s /* l = left, r = right */
        l = { f->output[ tag].min , f->output[ tag].min , 0.0f, 1.0f, 0 },
        r = { f->output[ tag].max , f->output[ tag].max , 0.0f, -1.0f, 0 };
        res = qFIS_GetResolution( f, tag );
        for( i = f->nPoints ; i > 0u ; --i ) {
            struct bisector_s *b;
            b = ( l.a <= r.a ) ? &l : &r;
            b->x = qFIS_GetNextX( b->init, b->sign*res, b->i );
            fx = qFIS_OutputAggregate( f, tag, b->x );
            b->a += fx; /*update area*/
            ++b->i;
        }
        f->output[ tag ].value = ( ( l.a*r.x ) + ( r.a*l.x ) )/( l.a + r.a );
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzLOM( qFIS_t * const f )
{
    size_t i;
    float x, fx, yMax, xLargest, res;
    int tag;

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        yMax = -1.0f;
        xLargest = f->output[ tag ].max;
        res = qFIS_GetResolution( f, tag );
        for ( i = 0u ; i < ( f->nPoints + 1u ) ; ++i ) {
            x = qFIS_GetNextX( f->output[ tag ].min, res, i );
            fx = qFIS_OutputAggregate( f, tag, x );
            if ( fx >= yMax ) {
                yMax = fx;
                xLargest = x;
            }
        }
        f->output[ tag ].value = xLargest;
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzSOM( qFIS_t * const f )
{
    size_t i;
    float x, fx, yMax, xSmallest, res;
    int tag;

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        yMax = -1.0f;
        xSmallest = f->output[ tag ].min;
        res = qFIS_GetResolution( f, tag );
        for ( i = 0u ; i < ( f->nPoints + 1u ) ; ++i ) {
            x = qFIS_GetNextX( f->output[ tag ].min, res, i );
            fx = qFIS_OutputAggregate( f, tag, x );
            if ( fx > yMax ) {
                yMax = fx;
                xSmallest = x;
            }
        }
        f->output[ tag ].value = xSmallest;
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzMOM( qFIS_t * const f )
{
    size_t i;
    float x, fx, yMax, xLargest, xSmallest, res;
    int tag;
    uint8_t sp;

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        yMax = -1.0f;
        xSmallest = f->output[ tag ].min;
        xLargest = f->output[ tag ].max;
        sp = 0u;
        res = qFIS_GetResolution( f, tag );
        for ( i = 0u ; i < ( f->nPoints + 1u ) ; ++i ) {
            x = qFIS_GetNextX( f->output[ tag ].min, res, i );
            fx = qFIS_OutputAggregate( f, tag, x );
            if ( fx > yMax ) {
                yMax = fx;
                xSmallest = x;
                xLargest = x;
                sp = 1u;
            }
            else if ( ( 1u == sp ) && ( fabsf( fx - yMax ) <= FLT_MIN ) ) {
                xLargest = x;
            }
            else if ( fx < yMax ) {
                sp = 0u;
            }
            else {
              /*nothing to do*/
            }
        }
        f->output[ tag ].value = 0.5f*( xSmallest + xLargest );
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzWtAverage( qFIS_t * const f )
{
    size_t j;

    for ( j = 0 ; j < f->nOutputs ; ++j ) {
        f->output[ j ].value = f->output[ j ].zi_wi/f->output[ j ].wi;
    }

    return 1;
}
/*============================================================================*/
static int qFIS_DeFuzzWtSum( qFIS_t * const f )
{
    size_t j;

    for ( j = 0 ; j < f->nOutputs ; ++j ) {
        f->output[ j ].value = f->output[ j ].zi_wi;
    }

    return 1;
}
/*============================================================================*/
int qFIS_DeFuzzify( qFIS_t * const f )
{
    int retValue = 0;

    if ( NULL != f ) {
        retValue = f->deFuzz( f );
    }

    return retValue;
}
/*============================================================================*/
static float qFIS_TriMF( const qFIS_IO_t * const in,
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

    return qFIS_Max( tmp , 0.0f );
}
/*============================================================================*/
static float qFIS_TrapMF( const qFIS_IO_t * const in,
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
    tmp = qFIS_Min( ( x - a )/( b - a ) , 1.0f );
    tmp = qFIS_Min( tmp, ( d - x )/( d - c ) ) ;

    return qFIS_Max( tmp , 0.0f );
}
/*============================================================================*/
static float qFIS_GBellMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n )
{
    float a, b, c;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    c = p[ 2 ];

    return ( 1.0f/( 1.0f + powf( fabsf( ( x - c )/a ) , 2.0f*b ) ) );
}
/*============================================================================*/
static float qFIS_GaussMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n )
{
    float a, c, tmp;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    c = p[ 1 ];
    tmp = ( x - c )/a;

    return expf( -0.5f*tmp*tmp );
}
/*============================================================================*/
static float qFIS_Gauss2MF( const qFIS_IO_t * const in,
                            const float *p,
                            const size_t n )
{
    float c1, c2, f1, f2;
    float x = in[ 0 ].value;

    c1 = p[ 1 ];
    c2 = p[ 3 ];
    f1 = ( x <= c1 ) ? qFIS_GaussMF( in , p, n ) : 1.0f;
    f2 = ( x <= c2 ) ? qFIS_GaussMF( in , &p[ 2 ], n ) : 1.0f;

    return f1*f2;
}
/*============================================================================*/
static float qFIS_SigMF( const qFIS_IO_t * const in,
                         const float *p,
                         const size_t n )
{
    float a, b;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];

    return 1.0f/( 1.0f + expf( -a*( x - b ) ) );
}
/*============================================================================*/
static float qFIS_TSigMF( const qFIS_IO_t * const in,
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
    if ( fabsf( x - 1.0f ) <= FLT_MIN ) { /* x == 1 ? */
        if ( a >= 0.0f ){
            y = max;
        }
        else{
            y = min;
        }
    }
    else if ( fabsf( x ) <= FLT_MIN ) { /* x == 0 ? */
        if ( a >= 0.0f ){
            y = min;
        }
        else{
            y = max;
        }
    }
    else {
        /*cstat -MISRAC2012-Dir-4.11_a*/
        y = b - ( logf( ( 1.0f/x ) - 1.0f )/a );
        /*cstat +MISRAC2012-Dir-4.11_a*/
    }

    return y;
}
/*============================================================================*/
static float qFIS_DSigMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n )
{
    return fabsf( qFIS_SigMF( in , p, n ) - qFIS_SigMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_PSigMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n )
{
    return fabsf( qFIS_SigMF( in , p, n )*qFIS_SigMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_SMF( const qFIS_IO_t * const in,
                       const float *p,
                       const size_t n )
{
    float a, b, tmp, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( x <= a ) {
        y =  0.0f;
    }
    else if ( x >= b ) {
        y = 1.0f;
    }
    else if ( ( x >= a ) && ( x <= ( ( a + b )*0.5f ) ) ) {
        tmp = ( x - a )/( b - a );
        y = 2.0f*tmp*tmp;
    }
    else if ( ( x <= b ) && ( x >= ( ( a + b )*0.5f ) ) ) {
        tmp = ( x - b )/( b - a );
        y = ( 1.0f - ( 2.0f*tmp*tmp ) );
    }
    else {
        y = 0.0f;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TSMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n )
{
    float diff, a, b, ta, tb, ma, mb;
    float x = in[ 0 ].value;
    (void)n;
    qFIS_IO_t tmp;


    a = p[ 0 ]; /*start*/
    b = p[ 1 ]; /*end*/
    diff = b - a;
    diff = 0.5f*diff*diff;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Dir-4.11_b*/
    ta = a + sqrtf( x*diff );
    tmp.value = ta;
    ma = qFIS_SMF( &tmp, p, n );
    tb = b + sqrtf( -( x - 1.0f )*diff );
    tmp.value = tb;
    mb = qFIS_SMF( &tmp, p, n );
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Dir-4.11_b*/
    return  ( fabsf( x - ma ) < fabsf( x - mb ) ) ? ta : tb;
}
/*============================================================================*/
static float qFIS_ZMF( const qFIS_IO_t * const in,
                       const float *p,
                       const size_t n )
{
    float a, b, tmp, y;
    float x = in[ 0 ].value;
    (void)n;

    a = p[ 0 ];
    b = p[ 1 ];
    if ( x <= a ) {
        y = 1.0f;
    }
    else if ( x >= b ) {
        y = 0.0f;
    }
    else if ( ( x >= a ) && ( x <= ( ( a + b )*0.5f ) ) ) {
        tmp = ( x - a )/( b - a );
        y = 1.0f - ( 2.0f*tmp*tmp );
    }
    else if ( ( x <= b ) && ( x >= ( ( a + b )*0.5f ) ) ) {
        tmp = ( x - b )/( b - a );
        y = 2.0f*tmp*tmp;
    }
    else {
        y = 0.0f;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TZMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n )
{
    float diff, a, b, ta, tb, ma, mb;
    float x = in[ 0 ].value;
    (void)n;
    qFIS_IO_t tmp;

    a = p[ 0 ]; /*start*/
    b = p[ 1 ]; /*end*/
    diff = b - a;
    diff = 0.5f*diff*diff;
    /*cstat -MISRAC2012-Dir-4.11_a -MISRAC2012-Dir-4.11_b*/
    ta = a + sqrtf( -( x - 1.0f )*diff );
    tmp.value = ta;
    ma = qFIS_SMF( &tmp, p, n );
    tb = b + sqrtf( x*diff );
    tmp.value = tb;
    mb = qFIS_SMF( &tmp, p, n );
    /*cstat +MISRAC2012-Dir-4.11_a +MISRAC2012-Dir-4.11_b*/
    return  ( fabsf( x - ma ) < fabsf( x - mb ) ) ? ta : tb;
}
/*============================================================================*/
static float qFIS_PiMF( const qFIS_IO_t * const in,
                        const float *p,
                        const size_t n )
{
    return fabsf( qFIS_SMF( in , p, n )*qFIS_ZMF( in , &p[ 2 ], n ) );
}
/*============================================================================*/
static float qFIS_SingletonMF( const qFIS_IO_t * const in,
                               const float *p,
                               const size_t n )
{
    float x = in[ 0 ].value;
    (void)n;

    return ( ( fabsf( x - p[ 0 ] ) <= FLT_MIN ) ? 1.0f : 0.0f );
}
/*============================================================================*/
static float qFIS_ConcaveMF( const qFIS_IO_t * const in,
                             const float *p,
                             const size_t n )
{
    float x = in[ 0 ].value;
    float i, e, y;
    (void)n;

    i = p[ 0 ];
    e = p[ 1 ];

    if ( ( i <= e ) && ( x < e ) ) {
        y = ( e - i )/( ( 2.0f*e ) - i -x );
    }
    else if ( ( i > e ) && ( x > e ) ) {
        y = ( i - e )/( -( 2.0f*e ) + i +x );
    }
    else {
        y = 1.0f;
    }

    return y;
}
/*============================================================================*/
static float qFIS_TConcaveMF( const qFIS_IO_t * const in,
                              const float *p,
                              const size_t n )
{
    float i, e;
    
    i = p[ 0 ];
    e = p[ 1 ];
    
    return ( ( i - e )/qFIS_ConcaveMF( in, p, n ) ) + ( 2.0f*e ) - i;
}
/*============================================================================*/
static float qFIS_SpikeMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n )
{
    float x = in[ 0 ].value;
    float w, c;
    (void)n;

    w = p[ 0 ];
    c = p[ 1 ];

    return expf( -fabsf( 10.0f*( x - c )/w ) );
}
/*============================================================================*/
static float qFIS_RampMF( const qFIS_IO_t * const in,
                          const float *p,
                          const size_t n )
{
    float x = in[ 0 ].value;
    float s, e, y;
    (void)n;

    s = p[ 0 ];
    e = p[ 1 ];

    if ( fabsf( x - e ) <= FLT_MIN ) {
        y = 1.0f;
    }
    else {
        if ( s < e ) {
            if ( x <= s ) {
                y = 0.0f;
            }
            else if ( x >= e ) {
                y = 1.0f;
            }
            else {
                y = ( x - s )/( e - s );
            }
        }
        else {
            if ( x >= s ) {
                y = 0.0f;
            }
            else if ( x <= e ) {
                y = 1.0f;
            }
            else {
                y = ( s - x )/( s - e );
            }
        }
    }

    return y;
}
/*============================================================================*/
static float qFIS_TRampMF( const qFIS_IO_t * const in,
                           const float *p,
                           const size_t n )
{
    float x = in[ 0 ].value;
    float s, e;
    (void)n;

    s = p[ 0 ];
    e = p[ 1 ];

    return ( ( e - s )*x ) + s;
}
/*============================================================================*/
static float qFIS_RectangleMF( const qFIS_IO_t * const in,
                               const float *p,
                               const size_t n )
{
    float x = in[ 0 ].value;
    float s, e;
    (void)n;

    s = p[ 0 ];
    e = p[ 1 ];

    return ( ( x >= s ) && ( x <= e ) ) ? 1.0f : 0.0f;
}
/*============================================================================*/
static float qFIS_CosineMF( const qFIS_IO_t * const in,
                            const float *p,
                            const size_t n )
{
    float x = in[ 0 ].value;
    float c, w, y;
    const float pi = 3.14159265358979323846f;
    (void)n;

    c = p[ 0 ];
    w = p[ 1 ];

    if ( ( x < ( c - ( 0.5*w ) ) ) || ( x > ( c + ( 0.5f*w ) ) ) ) {
        y = 0.0f;
    }
    else {
        y = 0.5f*( 1.0f + cosf( 2.0f/w*pi*( x - c) ) );
    }

    return y;

}
/*============================================================================*/
static float qFIS_ConstantMF( const qFIS_IO_t * const in,
                              const float *p,
                              const size_t n )
{
    (void)in;
    (void)n;
    return p[ 0 ];
}
/*============================================================================*/
static float qFIS_LinearMF( const qFIS_IO_t * const in,
                            const float *p,
                            const size_t n )
{
    float px = 0.0f;
    size_t i;
    
    for ( i = 0u ; i < n ; ++i ) {
        px += in[ i ].value*p[ i ];
    }
    px += p[ i ];

    return px;
}
/*============================================================================*/
static float qFIS_Min( const float a,
                       const float b )
{
    return qFIS_Sat( ( a < b ) ? a : b );
}
/*============================================================================*/
static float qFIS_Max( const float a,
                       const float b )
{
    return qFIS_Sat( ( a > b ) ? a : b );
}
/*============================================================================*/
static float qFIS_Prod( const float a,
                        const float b )
{
    return qFIS_Sat( a*b );
}
/*============================================================================*/
static float qFIS_ProbOR( const float a,
                          const float b )
{
    return qFIS_Sat( a + b - ( a*b ) );
}
/*============================================================================*/
static float qFIS_Sum( const float a,
                       const float b )
{
    return qFIS_Sat( a + b );
}
/*============================================================================*/
static float qFIS_Sat( float y )
{
    if ( y < 0.0f ) {
        y = 0.0f;
    }

    if ( y > 1.0f ) {
        y = 1.0f;
    }

    return y;
}
/*============================================================================*/