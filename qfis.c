/*!
 * @file qfis.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qfis.h"

typedef float (*qFIS_FuzzyOperator_t)( const float a, const float b );

static float qFIS_TriMF( const float x,
                         const float * const points );
static float qFIS_TrapMF( const float x,
                          const float * const points );
static float qFIS_GBellMF( const float x,
                           const float * const points );
static float qFIS_GaussMF( const float x,
                           const float * const points );
static float qFIS_Gauss2MF( const float x,
                            const float * const points );
static float qFIS_SigMF( const float x,
                         const float * const points );
static float qFIS_DSigMF( const float x,
                          const float * const points );
static float qFIS_PSigMF( const float x,
                          const float * const points );
static float qFIS_SMF( const float x,
                       const float * const points );
static float qFIS_ZMF( const float x,
                       const float * const points );
static float qFIS_PiMF( const float x,
                        const float * const points );
static float qFIS_SingletonMF( const float x,
                               const float * const points );
static float qFIS_Constant( const float x,
                        const float * const points );
static float qFIS_Linear( const float x,
                        const float * const points );
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
static float qFIS_SugenoMF( qFIS_IO_t *x,
                            const float *a,
                            size_t ni );
static void qFIS_EvalInputMFs( qFIS_t *f );
static void qFIS_TruncateInputs( qFIS_t *f );
static float qFIS_ParseFuzzValue( qFIS_MF_t *mfIO,
                                  qFIS_Rules_t index );
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( qFIS_t *f );
static int qFIS_MamdaniDeFuzz( qFIS_t *f );
static int qFIS_SugenoDeFuzz( qFIS_t *f );

static size_t qFIS_InferenceAntecedent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i );
static size_t qFIS_InferenceConsequent( struct _qFIS_s *f,
                                        const qFIS_Rules_t * const r,
                                        size_t i );
static void qFIS_InferenceInit( qFIS_t *f );

#define QFIS_INFERENCE_ERROR         ( 0u )

/*============================================================================*/
int qFIS_SetParameter( qFIS_t *f,
                       qFIS_Parameter_t p,
                       int x )
{
    int retVal = 0;
    typedef float (*methods_fcn)( const float a, const float b );
    static const methods_fcn method[ 5 ] = { &qFIS_Min, &qFIS_Prod, &qFIS_Max,
                                             &qFIS_ProbOR, &qFIS_Sum };

    if ( NULL != f ) {
        switch( p ) {
            case qFIS_Implication:
                if ( x <= (int)qFIS_PROD ) {
                    f->implicate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_Aggregation:
                if ( ( x >= (int)qFIS_MAX ) && ( x <= (int)qFIS_SUM ) ) {
                    f->aggregate = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_AND:
                if ( x <= (int)qFIS_PROD ) {
                    f->andOp = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_OR:
                if ( ( x >= (int)qFIS_MAX ) && ( x <= (int)qFIS_PROBOR ) ) {
                    f->orOp = method[ x ];
                    retVal = 1;
                }
                break;
            case qFIS_EvalPoints:
                if ( x >= 20 ) {
                    f->evalPoints = (size_t)x;
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
int qFIS_Setup( qFIS_t *f,
                qFIS_Type_t t,
                qFIS_IO_t * const inputs,
                size_t ni,
                qFIS_IO_t * const outputs,
                size_t no,
                qFIS_MF_t *mf_inputs,
                size_t nmi,
                qFIS_MF_t *mf_outputs,
                size_t nmo )
{
    int retVal = 0;
  
    if ( NULL != f ) {
        f->type = t;
        f->evalPoints = 100u;
        f->nInputs = ni/sizeof(qFIS_IO_t);
        f->nOutputs = no/sizeof(qFIS_IO_t);
        f->nMFInputs = nmi/sizeof(qFIS_MF_t);
        f->nMFOutputs = nmo/sizeof(qFIS_MF_t);
        f->andOp = &qFIS_Min;
        f->orOp = &qFIS_Max;
        f->implicate = &qFIS_Min;
        f->aggregate = &qFIS_Max;
        f->mUnion = &qFIS_Max;
        f->input = inputs;
        f->output = outputs;
        f->inMF = mf_inputs;
        f->outMF = mf_outputs;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qFIS_SetIO( qFIS_IO_t *v,
                qFIS_Tag_t tag,
                float min,
                float max )
{
    int retVal = 0;

    if ( ( NULL != v ) && ( tag >= 0 ) ) {
        v[ tag ].lo = min;
        v[ tag ].up = max;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qFIS_SetMF( qFIS_MF_t *m,
                qFIS_Tag_t io_tag,
                qFIS_Tag_t mf_tag,
                qFIS_MF_Name_t shape,
                const float *cp )
{
    int retVal = 0;
    typedef float (* shapes_fcn )( const float x, const float * const points );
    static const shapes_fcn fShape[ 14 ] = { &qFIS_TriMF, &qFIS_TrapMF,
                                             &qFIS_GBellMF, &qFIS_GaussMF,
                                             &qFIS_Gauss2MF, &qFIS_SigMF,
                                             &qFIS_DSigMF, &qFIS_PSigMF,
                                             &qFIS_PiMF, &qFIS_SMF,
                                             &qFIS_ZMF, &qFIS_SingletonMF,
                                             &qFIS_Constant, &qFIS_Linear };

    if ( ( NULL != m ) && ( io_tag >= 0 ) && ( mf_tag >= 0 ) && ( shape <= linear ) ) {
        m[ mf_tag ].shape = fShape[ shape ];
        m[ mf_tag ].index = (size_t)io_tag;
        m[ mf_tag ].points = cp;
        m[ mf_tag ].fx = 0.0f;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qFIS_Fuzzify( qFIS_t *f )
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
static float qFIS_ParseFuzzValue( qFIS_MF_t *mfIO,
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
    y = ( 0u != neg )? ( 1.0f - y ) : y ;
    return y;
}
/*============================================================================*/
static qFIS_FuzzyOperator_t qFIS_GetFuzzOperator( qFIS_t *f )
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
    size_t ni;
    float zi;

    outIndex = r[ i ];
    MFOutIndex = r[ i + 1u ] - 1;
    connector = ( f->nOutputs > 1u )? r[ i + 2u ] : -1;
    i += 2u;

    switch ( f->type ) {
        case Mamdani:
            f->outMF[ MFOutIndex ].fx = f->aggregate( f->outMF[ MFOutIndex ].fx, f->rStrength );
            break;
        case Sugeno:
            ni = ( &qFIS_Constant == f->outMF[ MFOutIndex ].shape )? 0u : f->nInputs;
            zi = qFIS_SugenoMF( f->input, f->outMF[ MFOutIndex ].points, ni );
            f->output[ outIndex ].up += zi*f->rStrength;
            f->output[ outIndex ].lo += f->rStrength;
            break;
        default:
            break;
    }
    
    if ( _QFIS_AND != connector ) {
        f->inferenceState = &qFIS_InferenceAntecedent;
        f->lastConnector = -1;
        f->rStrength = 0.0f;
        f->ruleCount++;
        i--;
    }

    return i;
}
/*============================================================================*/
static void qFIS_InferenceInit( qFIS_t *f )
{
    f->inferenceState = &qFIS_InferenceAntecedent;
    f->rStrength = 0.0f;
    f->lastConnector = -1;
    f->ruleCount = 0;

    if ( Sugeno == f->type ) {
        size_t j;
        for( j = 0 ; j < f->nOutputs ; ++j ) {
            f->output[ j ].lo = 0.0f;
            f->output[ j ].up = 0.0f;
        }
    }
}
/*============================================================================*/
int qFIS_Inference( qFIS_t *f,
                    const qFIS_Rules_t * const r )
{
    int retVal = 0;

    if ( ( NULL != f ) && ( NULL != r ) ) {
        size_t i = 0u;

        if ( QFIS_RULES_BEGIN == r[ 0 ] ) {
            qFIS_InferenceInit( f );
            i = 1u;

            while( _QFIS_RULES_END != r[ i ] ) {
                i = f->inferenceState( f, r, i );
                if ( QFIS_INFERENCE_ERROR == i ) {
                    break;
                }
                ++i;
            }
        }

        retVal = ( _QFIS_RULES_END == r[ i ] )? f->ruleCount : 0;
    }

    return retVal;
}
/*============================================================================*/
static int qFIS_MamdaniDeFuzz( qFIS_t *f )
{
    size_t i, j;
    float x, z, fx, int_Fx, int_xFx, res;
    int tag;

    for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
        int_Fx = 0.0f;
        int_xFx = 0.0f;
        /*cstat -CERT-FLP36-C*/
        res = ( ( f->output[ tag ].up - f->output[ tag ].lo )/(float)f->evalPoints );
        /*cstat +CERT-FLP36-C*/
        for ( i = 0u ; i < ( f->evalPoints + 1u ) ; ++i ) {
            /*cstat -CERT-FLP36-C*/
            x = f->output[ tag ].lo + ( (float)i*res );
            /*cstat +CERT-FLP36-C*/
            if ( x > f->output[tag].up ) {
                x = f->output[tag].up;
            }
            fx = 0.0f;
            for ( j = 0u ; j < f->nMFOutputs ; ++j ) {
                if ( f->outMF[ j ].index == (size_t)tag ) {
                    z = f->outMF[ j ].shape( x , f->outMF[ j ].points );
                    fx = f->mUnion( fx, f->implicate( z, f->outMF[ j ].fx ) );
                }
            }
            int_xFx += x*fx;
            int_Fx += fx;
            if ( x >= f->output[ tag ].up ) {
                break;
            }
        }
        f->output[ tag ].value = ( int_xFx/int_Fx );
    }
    return 1;
}
/*============================================================================*/
static int qFIS_SugenoDeFuzz( qFIS_t *f )
{
    size_t j;
    for( j = 0 ; j < f->nOutputs ; ++j ) {
        f->output[ j ].value = f->output[ j ].up/f->output[ j ].lo;
    }
    return 1;
}
/*============================================================================*/
int qFIS_DeFuzzify( qFIS_t *f )
{
    int retValue = 0;

    if ( NULL != f ) {
        retValue = ( Mamdani == f->type )? qFIS_MamdaniDeFuzz( f ) :
                                           qFIS_SugenoDeFuzz( f );
    }
    return retValue;
}
/*============================================================================*/
static void qFIS_EvalInputMFs( qFIS_t *f )
{
    size_t i;
    qFIS_MF_t *mf;

    for ( i = 0 ; i < f->nMFInputs ; i++ ) {
        mf = &f->inMF[ i ];
        mf->fx = mf->shape( f->input[ mf->index ].value, mf->points );
    }
}
/*============================================================================*/
static void qFIS_TruncateInputs( qFIS_t *f )
{
    size_t i;
    for( i = 0 ; i < f->nInputs ; i++ ) {
        if ( f->input[ i ].value > f->input[ i ].up ) {
            f->input[ i ].value = f->input[ i ].up;
        }
        if ( f->input[ i ].value < f->input[ i ].lo ) {
            f->input[ i ].value = f->input[ i ].lo;
        }
    }
}
/*============================================================================*/
static float qFIS_TriMF( const float x,
                         const float * const points )
{
    float a, b, c, tmp;
    a = points[ 0 ];
    b = points[ 1 ];
    c = points[ 2 ];
    tmp = qFIS_Min( ( x - a )/( b - a ) , ( c - x )/( c - b ) );
    return qFIS_Max( tmp , 0.0f );
}
/*============================================================================*/
static float qFIS_TrapMF( const float x,
                          const float * const points )
{
    float a, b, c, d, tmp;
    a = points[ 0 ];
    b = points[ 1 ];
    c = points[ 2 ];
    d = points[ 3 ];
    tmp = qFIS_Min( ( x - a )/( b - a ) , 1.0f );
    tmp = qFIS_Min( tmp, ( d - x )/( d - c ) ) ;
    return qFIS_Max( tmp , 0.0f );
}
/*============================================================================*/
static float qFIS_GBellMF( const float x,
                           const float * const points )
{
    float a, b, c;
    a = points[ 0 ];
    b = points[ 1 ];
    c = points[ 2 ];
    return ( 1.0f/( 1.0f + powf( fabsf( ( x - c )/a ) , 2.0f*b ) ) );
}
/*============================================================================*/
static float qFIS_GaussMF( const float x,
                           const float * const points )
{
    float a, c, tmp;
    a = points[ 0 ];
    c = points[ 1 ];
    tmp = ( x - c )/a;
    return expf( -0.5f*tmp*tmp );
}
/*============================================================================*/
static float qFIS_Gauss2MF( const float x,
                            const float * const points )
{
    float c1, c2, f1, f2;
    c1 = points[ 1 ];
    c2 = points[ 3 ];
    f1 = ( x <= c1 )? qFIS_GaussMF( x , points ) : 1.0f;
    f2 = ( x <= c2 )? qFIS_GaussMF( x , &points[ 2 ] ) : 1.0f;
    return f1*f2;
}
/*============================================================================*/
static float qFIS_SigMF( const float x,
                         const float * const points )
{
    float a, b;
    a = points[ 0 ];
    b = points[ 1 ];
    return 1.0f/( 1.0f + expf( -a*( x - b ) ) );
}
/*============================================================================*/
static float qFIS_DSigMF( const float x,
                          const float * const points )
{
    return fabsf( qFIS_SigMF( x , points ) - qFIS_SigMF( x , &points[ 2 ] ) );
}
/*============================================================================*/
static float qFIS_PSigMF( const float x,
                       const float * const points )
{
    return fabsf( qFIS_SigMF( x , points ) * qFIS_SigMF( x , &points[ 2 ] ) );
}
/*============================================================================*/
static float qFIS_SMF( const float x,
                       const float * const points )
{
    float a, b, tmp, y;
    a = points[ 0 ];
    b = points[ 1 ];
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
static float qFIS_ZMF( const float x,
                       const float * const points )
{
    float a, b, tmp, y;
    a = points[ 0 ];
    b = points[ 1 ];
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
static float qFIS_PiMF( const float x,
                        const float * const points )
{
    return fabsf( qFIS_SMF( x , points )*qFIS_ZMF( x , &points[ 2 ] ) );
}
/*============================================================================*/
static float qFIS_SingletonMF( const float x,
                               const float * const points )
{
    return ( ( fabsf( x - points[ 0 ] ) <= FLT_MIN )? 1.0f : 0.0f );
}
/*============================================================================*/
static float qFIS_Constant( const float x,
                        const float * const points )
{
    (void)x;
    (void)points;
    return 0.0f;
}
/*============================================================================*/
static float qFIS_Linear( const float x,
                        const float * const points )
{
    (void)x;
    (void)points;
    return 0.0f;
}
/*============================================================================*/
static float qFIS_Min( const float a,
                       const float b )
{
    return qFIS_Sat( ( a < b )? a : b );
}
/*============================================================================*/
static float qFIS_Max( const float a,
                       const float b )
{
    return qFIS_Sat( ( a > b )? a : b );
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
static float qFIS_SugenoMF( qFIS_IO_t *x,
                            const float *a,
                            size_t ni )
{
    float px = 0.0f;
    if ( ni > 0u ) {
        size_t i;
        for( i = 0u ; i < ni ; ++i ) {
            px += x[ i ].value*a[ i ];
        }
        px += a[ i ];
    }
    else { /*constant membership function*/
        px = a[ 0 ];
    }
    return px;
}
/*============================================================================*/