#include "qbitfield.h"


#define LONG_BIT                (size_t) (sizeof(uint32_t) * 8uL )
#define BITMASK(b)              ( (uint32_t)1uL << ((b) % LONG_BIT) )
#define BITSLOT(b)              ( (b) / LONG_BIT) /*((b) >> 6 )*/
#define BITGET(a, b)            ( ((a)->field[BITSLOT(b)] >> ((b) % LONG_BIT)) & 1uL)
#define BITSET(a, b)            ( (a)->field[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b)          ( (a)->field[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b)           ( (a)->field[BITSLOT(b)] & BITMASK(b))
#define BITTOGGLE(a, b)         ( (a)->field[BITSLOT(b)] ^= BITMASK(b))
#define BITNSLOTS(nb)           ( (nb + LONG_BIT - 1) / LONG_BIT)
#define BITOFFSET(index)        ( (index) & (uint32_t)31u)
#define BITMASKMERGE(a,b,abits) ( (b) ^ (((a) ^ (b)) & (abits)))
#define BITMASK32(nbits)        ( ( 0u != (nbits) )? ~(uint32_t)0 >> (sizeof(uint32_t)*8uL-(nbits)) : (uint32_t)0) 


static uint32_t qBitField_Read_uint32( const qBitField_t *instance, size_t index );
static void qBitField_Write_uint32( qBitField_t *instance, size_t index, uint32_t value );

/*============================================================================*/
int qBitField_Setup( qBitField_t *instance, void *area, size_t area_size )
{
    int retValue = 0;
    if ( ( NULL == instance ) && ( NULL == area ) && ( area_size > 0u ) ) {
        instance->field = area;
        instance->size = area_size*8u;
        instance->nSlots = area_size/sizeof(uint32_t);      
        retValue = 1; 
    }
    return retValue;
}
/*============================================================================*/
int qBitField_ClearAll( qBitField_t *instance )
{
    int retValue = 0;
    if ( NULL == instance ) {
        (void)memset( instance->field, 0, instance->size/8u );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
int qBitField_SetAll( qBitField_t *instance )
{
    int retValue = 0;
    if ( NULL == instance ) {
        (void)memset( instance->field, 0xFF, instance->size/8u );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
int qBitField_SetBit( qBitField_t *instance, size_t index )
{
    int retValue = 0;
    if ( NULL == instance ) {
        BITSET( instance, index );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
int qBitField_ClearBit( qBitField_t *instance, size_t index )
{
    int retValue = 0;
    if ( NULL == instance ) {
        BITCLEAR( instance, index );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
int qBitField_ToggleBit( qBitField_t *instance, size_t index )
{
    int retValue = 0;
    if ( NULL == instance ) {
        BITTOGGLE( instance, index );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
uint8_t qBitField_ReadBit( const qBitField_t *instance, size_t index )
{
    uint8_t retValue = 0u;
    if ( NULL == instance ) {
        retValue = ( 0u != BITGET( instance, index ) )? 1u : 0u;
    }
    return retValue;
}
/*============================================================================*/
int qBitField_WriteBit( qBitField_t *instance, size_t index, uint8_t value )
{
    int retValue = 0;
    if ( NULL == instance ) {
        if ( 0u != value ) {
            BITSET( instance, index );
        }
        else {
            BITCLEAR( instance, index );
        }
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
uint32_t qBitField_ReadUINTn( qBitField_t *instance, size_t index, size_t xBits )
{
    uint32_t retValue = 0uL;
    if ( NULL == instance ) {
        if ( xBits <= 32u ) {
            if ( 1u == xBits ) {
                retValue = (uint32_t)qBitField_ReadBit( instance, index );
            }
            else if ( 32u == xBits ) {
                retValue = qBitField_Read_uint32( instance, index );
            }
            else {
                retValue = qBitField_Read_uint32( instance, index );
                retValue &= ( ( ~((uint32_t)0) ) >> ( 32u - xBits ) ); /*safe mask*/    /*ATH-shift-bounds,MISRAC2012-Rule-12.2 deviation allowed*/
            }        
        }
    }
    return retValue;
}
/*============================================================================*/
int qBitField_WriteUINTn( qBitField_t *instance, size_t index, uint32_t value, size_t xBits )
{
    int retValue = 0;
    if ( NULL == instance ) {
        uint32_t w, mask;
        if( xBits <= 32u ) {
            if( 1u == xBits ) {
                qBitField_WriteBit( instance, index, (uint8_t)value );
            }
            else if( 32u == xBits ) {
                qBitField_Write_uint32( instance, index, value );
            }
            else {
                w = qBitField_Read_uint32( instance, index );
                value &= ( ( ~((uint32_t)0) ) >> ( 32u - xBits ) ); /*safe mask*/ /*ATH-shift-bounds,MISRAC2012-Rule-12.2 deviation allowed*/
                mask = ( ~((uint32_t)0) ) << xBits;  /*!#ok*/
                qBitField_Write_uint32( instance, index, BITMASKMERGE( w, value, mask ) );         
            }        
        }
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
float qBitField_ReadFloat( const qBitField_t *instance, size_t index )
{
    float retValue = 0.0f;
    if ( NULL == instance ) {
        uint32_t rval;
        rval = qBitField_Read_uint32( instance, index );
        (void)memcpy( &retValue, &rval, sizeof(float) );
    }
    return retValue;
}
/*============================================================================*/
int qBitField_WriteFloat( qBitField_t *instance, size_t index, float value )
{
    int retValue = 0;
    if ( NULL == instance ) {
        uint32_t fval;
        (void)memcpy( &fval, &value, sizeof(float) );
        qBitField_Write_uint32( instance, index, fval );
        retValue = 1;
    }
    return retValue;
}
/*============================================================================*/
void* qBitField_Dump( qBitField_t *instance, void* dst, size_t n )
{
    void *retValue = NULL;
    if ( NULL == instance ) {
        if( n <= ( instance->size/8u ) ){
            retValue = memcpy( dst, instance->field, n );
        }
    }
    return retValue;
}
/*============================================================================*/
static uint32_t qBitField_Read_uint32( const qBitField_t *instance, size_t index )
{
    size_t slot;
    uint8_t of, bits_taken;
    uint32_t result;

    slot = BITSLOT(index);
    of = (uint8_t)BITOFFSET( index );
    result = instance->field[ slot ] >> of;
    bits_taken  = (uint8_t)( 32u - of );
    
    if ( ( 0u != of ) && ( ( index + bits_taken ) < instance->size ) ) {
        result |= instance->field[ slot+1u ] << (uint32_t)bits_taken ; /*ATH-shift-bounds,MISRAC2012-Rule-12.2 deviation allowed*/
    }
    return result;
}
/*============================================================================*/
static void qBitField_Write_uint32( qBitField_t *instance, size_t index, uint32_t value )
{
    uint8_t of;
    uint32_t mask;
    size_t slot;
    slot = BITSLOT( index );
    of = (uint8_t)BITOFFSET( index );
    if ( 0u == of ) {
        instance->field[ slot ] = value;
    }
    else {
        mask = BITMASK32(of);
        instance->field[ slot   ] = ( value<<of ) | ( instance->field[slot] & mask )    ;
        if( slot+1u < instance->nSlots ){
            instance->field[ slot+1u ] = ( value >> (32u - of) ) | ( instance->field[ slot+1u ] & ( ~mask ) );  
        }
     }
}
/*============================================================================*/