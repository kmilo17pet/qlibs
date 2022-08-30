/*!
 * @file qfis.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qfis.h"

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
static float qFIS_Min( const float a,
                       const float b );
static float qFIS_Max( const float a,
                       const float b );
static float qFIS_Prod( const float a,
                        const float b );
static float qFIS_ProbOR( const float a,
                          const float b );
static float qFIS_Sat( float y );


static void qFIS_EvalInputMFs( qFIS_t *f );
static void qFIS_TruncateInputs( qFIS_t *f );
static float qFIS_ParseFuzzValue( qFIS_MF_t *mfIO, qFIS_Rules_t index );

/*============================================================================*/
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
                size_t nmfouts )
{
    int retVal = 0;
  
    if ( NULL != f ) {
        static const float (*method[ 4 ])( const float a, const float b ) = 
        { &qFIS_Min, &qFIS_Max, &qFIS_Prod, &qFIS_ProbOR };
        
        f->type = t;
        f->evalPoints = ep;
        f->nInputs = ni/sizeof(qFIS_IO_t);
        f->nOutputs = no/sizeof(qFIS_IO_t);
        f->nMFInputs = nmfins/sizeof(qFIS_MF_t);
        f->nMFOutputs = nmfouts/sizeof(qFIS_MF_t);
        f->andMethod = method[ and_m ];
        f->orMethod = method[ or_m ];
        f->input = inputs;
        f->output = outputs;
        f->inMF = mfinputs;
        f->outMF = mfoutputs;
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
        v[ tag ].min = min;
        v[ tag ].max = max;
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
int qFIS_SetMF( qFIS_MF_t *m,
                qFIS_Tag_t io_tag,
                qFIS_Tag_t mf_tag,
                qFIS_MF_Name_t shape,
                float a,
                float b,
                float c,
                float d )
{
    int retVal = 0;

    if ( ( NULL != m ) && ( io_tag >= 0 ) && ( mf_tag >= 0 ) ) {
        static const float (*fshape[ 12 ])( const float x, const float * const points ) = 
        { &qFIS_TriMF, &qFIS_TrapMF, &qFIS_GBellMF, &qFIS_GaussMF, &qFIS_Gauss2MF,
          &qFIS_SigMF, &qFIS_DSigMF, &qFIS_PSigMF, &qFIS_PiMF, &qFIS_SMF,
          &qFIS_ZMF, &qFIS_SingletonMF};
        
        m[ mf_tag ].shape = fshape[ shape ];
        m[ mf_tag ].index = (size_t)io_tag;
        m[ mf_tag ].points[ 0 ] = a;
        m[ mf_tag ].points[ 1 ] = b;
        m[ mf_tag ].points[ 2 ] = c;
        m[ mf_tag ].points[ 3 ] = d;
        m[ mf_tag ].fuzzVal = 0.0f;
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
    y = qFIS_Sat( mfIO[ index - 1 ].fuzzVal );
    /*cstat +CERT-INT32-C_a*/
    y = ( 0u != neg )? ( 1.0f - y ) : y ;
    return y;
}
/*============================================================================*/
int qFIS_Inference( qFIS_t *f,
                    const qFIS_Rules_t * const r )
{
    size_t i = 1u;
    qFIS_Rules_t lastConnector = -1;
    float ruleStrength = 0.0f;
    int16_t inputIndex, MFInputIndex, MFOutputIndex, connector;
    int retVal = 0;
    int rule_count = 0;
    enum rState_Def { 
        ANTECEDENT_IN = 0,
        CONSEQUENT_OUT = 2
    } ruleState = ANTECEDENT_IN;

    if ( ( NULL != f ) && ( NULL != r ) ) {
        if ( QFIS_RULES_BEGIN == r[ 0 ] ) {
            /*cstat -MISRAC2012-Rule-14.2*/
            for ( i = 1u ; _QFIS_RULES_END != r[ i ] ; ++i ) {
                if ( ANTECEDENT_IN == ruleState) {
                    inputIndex = r[ i ];
                    MFInputIndex = r[ i + 1u ];
                    connector = r[ i + 2u ];
                    switch ( lastConnector ) {
                        case _QFIS_AND:
                            ruleStrength = f->andMethod( ruleStrength, qFIS_ParseFuzzValue( f->inMF, MFInputIndex ) );
                            break;
                        case _QFIS_OR:
                            ruleStrength = f->orMethod( ruleStrength, qFIS_ParseFuzzValue( f->inMF, MFInputIndex ) );
                            break;
                        case -1: 
                            ruleStrength = qFIS_ParseFuzzValue( f->inMF, MFInputIndex );
                            break;
                        default:
                            break;
                    }

                    if ( ( inputIndex < 0 ) || ( (size_t)inputIndex > f->nInputs ) ) {
                        break;
                    }

                    if ( ( _QFIS_AND == connector ) || ( _QFIS_OR == connector ) ) {
                        lastConnector = connector;
                        ruleState = ANTECEDENT_IN;
                        i += 2u;
                        continue;
                    }
                    else if ( _QFIS_THEN == connector ) {
                        ruleState = CONSEQUENT_OUT;
                        i += 2u;
                        continue;
                    }
                    else {
                        break;
                    }
                }
                else if ( CONSEQUENT_OUT == ruleState ) {
                    /*outputIndex = r[ i ];*/
                    MFOutputIndex = r[ i + 1u ] - 1;
                    connector = ( f->nOutputs > 1u )? r[ i + 2u ] : -1;
                    i += 2u;
                    switch ( f->type ) {
                        case Mamdani:
                            f->outMF[ MFOutputIndex ].fuzzVal = qFIS_Max( f->outMF[ MFOutputIndex ].fuzzVal, ruleStrength ); /*aggregation using max*/
                            break;
                        default:
                            break;
                    }
                    if ( _QFIS_AND != connector ) {
                        ruleState = ANTECEDENT_IN;
                        lastConnector = -1;
                        ruleStrength = 0.0f;
                        rule_count++;
                        i--;
                    }
                }
                else {
                    break;
                }
            }
            /*cstat +MISRAC2012-Rule-14.2*/
        }
        retVal = ( _QFIS_RULES_END == r[ i ] )? rule_count : 0;
    }
    return retVal;
}
/*============================================================================*/
int qFIS_Defuzzify( qFIS_t *f )
{
    int retValue = 0;

    if ( NULL != f ) {
        size_t i, j;
        float x, z, fx, int_Fx, int_xFx, res;
        int tag;
        
        for ( tag = 0 ; (size_t)tag < f->nOutputs ; ++tag ) {
            int_Fx = 0.0f;
            int_xFx = 0.0f; 
            /*cstat -CERT-FLP36-C*/
            res = ( ( f->output[ tag ].max - f->output[ tag ].min )/(float)f->evalPoints );
            /*cstat +CERT-FLP36-C*/
            for ( i = 0 ; i < ( f->evalPoints + 1u ) ; ++i ) {
                /*cstat -CERT-FLP36-C*/
                x = f->output[ tag ].min + ( (float)i*res );
                /*cstat +CERT-FLP36-C*/
                if ( x > f->output[tag].max ) {
                    x = f->output[tag].max;
                }
                fx = 0.0f;
                for ( j = 0 ; j < f->nMFOutputs ; ++j ) {
                    if ( f->outMF[ j ].index == (size_t)tag ) {
                        z = f->outMF[ j ].shape( x , f->outMF[ j ].points );
                        fx = qFIS_Max( fx, ( z > f->outMF[ j ].fuzzVal )? f->outMF[ j ].fuzzVal : z );
                    }
                }
                int_xFx += x*fx;
                int_Fx += fx;
                if ( x >= f->output[ tag ].max ) {
                    break;
                }
            }
            f->output[ tag ].value = ( int_xFx/int_Fx );
        }
        retValue = 1;
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
        mf->fuzzVal = mf->shape( f->input[ mf->index ].value, mf->points );
    }
}
/*============================================================================*/
static void qFIS_TruncateInputs( qFIS_t *f )
{
    size_t i;
    for( i = 0 ; i < f->nInputs ; i++ ) { 
        if ( f->input[ i ].value > f->input[ i ].max ) {
            f->input[ i ].value = f->input[ i ].max;
        }
        if ( f->input[ i ].value < f->input[ i ].min ) {
            f->input[ i ].value = f->input[ i ].min;
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
    float a, b, y;
    a = points[ 0 ];
    b = points[ 1 ];
    if ( x <= a ) {
        y = 1.0f;
    }
    else if ( x >= b ) {
        y = 0.0f;
    }
    else if ( ( x >= a ) && ( x <= ( ( a + b )*0.5f ) ) ) {
        float tmp = ( x - a )/( b - a );
        y = 1.0f - ( 2.0f*tmp*tmp );
    }
    else if ( ( x <= b ) && ( x >= ( ( a + b )*0.5f ) ) ) {
        float tmp = ( x - b )/( b - a );
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