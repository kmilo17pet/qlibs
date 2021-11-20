/*!
 * @file qbitfield.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/
#include "qbitfield.h"

static const size_t qBitField_LBit = (size_t)( sizeof(uint32_t) * 8uL );

static uint32_t qBitField_Mask( const size_t index );
static size_t qBitField_BitSlot( const size_t index );
static uint32_t qBitField_BitGet( const qBitField_t * const b,
                                  const size_t index );
static void qBitField_BitSet( qBitField_t * const b,
                              size_t index );
static void qBitField_BitClear( qBitField_t *const b,
                                const size_t index );
static void qBitField_BitToggle( qBitField_t *const b,
                                 const size_t index );
static uint32_t qBitField_SafeMask( const uint32_t val,
                                    const size_t x,
                                    const size_t nbits );
static size_t qBitField_Offset( const size_t index );
static uint32_t qBitField_Read_uint32( const qBitField_t *instance,
                                       const size_t index );
static void qBitField_Write_uint32( qBitField_t *instance,
                                    const size_t index,
                                    const uint32_t value );

/*============================================================================*/
static uint32_t qBitField_Mask( const size_t index )
{
    return (uint32_t)1uL << ( index % qBitField_LBit );
}
/*============================================================================*/
static size_t qBitField_BitSlot( const size_t index )
{
    return index/qBitField_LBit;
}
/*============================================================================*/
static uint32_t qBitField_BitGet( const qBitField_t * const b,
                                  const size_t index )
{
    const size_t slot = qBitField_BitSlot( index );
    return  ( b->field[ slot ] >> ( index % qBitField_LBit ) ) & 1uL;
}
/*============================================================================*/
static void qBitField_BitSet( qBitField_t * const b,
                              size_t index )
{
    const size_t slot = qBitField_BitSlot( index );
    b->field[ slot ] |= qBitField_Mask( index );
}
/*============================================================================*/
static void qBitField_BitClear( qBitField_t * const b,
                                const size_t index )
{
    const size_t slot = qBitField_BitSlot( index );
    b->field[ slot ] &= ~qBitField_Mask( index );
}
/*============================================================================*/
static void qBitField_BitToggle( qBitField_t * const b,
                                 const size_t index )
{
    const size_t slot = qBitField_BitSlot( index );
    b->field[ slot ] ^= qBitField_Mask( index );
}
/*============================================================================*/
static uint32_t qBitField_SafeMask( const uint32_t val,
                                    const size_t x,
                                    const size_t nbits )
{
    /*cstat -ATH-shift-bounds -MISRAC2012-Rule-12.2 -CERT-INT34-C_b*/
    return val >> ( (uint32_t)( x - nbits ) );
    /*cstat +ATH-shift-bounds -MISRAC2012-Rule-12.2 +CERT-INT34-C_b*/
}
/*============================================================================*/
static size_t qBitField_Offset( const size_t index )
{
    return index & (size_t)31u;
}
/*============================================================================*/
static uint32_t qBitField_MaskMerge( const uint32_t w,
                                     const uint32_t value,
                                     const uint32_t mask )
{
    return value ^ ( ( w ^ value ) & mask );
}
/*============================================================================*/
int qBitField_Setup( qBitField_t * const b,
                     void * const area,
                     const size_t area_size )
{
    int retValue = 0;

    if ( ( NULL != b ) && ( NULL != area ) && ( area_size > 0u ) ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        b->field = (uint32_t *)area;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        b->size = area_size*8u;
        b->nSlots = area_size/sizeof(uint32_t);
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_ClearAll( qBitField_t * const b )
{
    int retValue = 0;

    if ( NULL != b ) {
        (void)memset( b->field, 0, b->size/8u );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_SetAll( qBitField_t * const b )
{
    int retValue = 0;

    if ( NULL != b ) {
        (void)memset( b->field, 0xFF, b->size/8u );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_SetBit( qBitField_t * const b,
                      const size_t index )
{
    int retValue = 0;

    if ( NULL != b ) {
        qBitField_BitSet( b, index );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_ClearBit( qBitField_t * const b,
                        const size_t index )
{
    int retValue = 0;

    if ( NULL != b ) {
        qBitField_BitClear( b, index );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_ToggleBit( qBitField_t * const b,
                         const size_t index )
{
    int retValue = 0;

    if ( NULL != b ) {
        qBitField_BitToggle( b, index );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
uint8_t qBitField_ReadBit( const qBitField_t *b,
                           const size_t index )
{
    uint8_t retValue = 0u;

    if ( NULL != b ) {
        retValue = ( 0uL != qBitField_BitGet( b, index ) )? 1u : 0u;
    }

    return retValue;
}
/*============================================================================*/
int qBitField_WriteBit( qBitField_t * const b,
                        const size_t index,
                        uint8_t value )
{
    int retValue = 0;

    if ( NULL != b ) {
        if ( 0u != value ) {
            qBitField_BitSet( b, index );
        }
        else {
            qBitField_BitClear( b, index );
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
uint32_t qBitField_ReadUINTn( const qBitField_t * const b,
                              const size_t index,
                              size_t xBits )
{
    uint32_t retValue = 0uL;

    if ( ( NULL != b ) && ( xBits <= 32u ) ) {
        if ( 1u == xBits ) {
            retValue = (uint32_t)qBitField_ReadBit( b, index );
        }
        else if ( 32u == xBits ) {
            retValue = qBitField_Read_uint32( b, index );
        }
        else {
            retValue = qBitField_Read_uint32( b, index );
            retValue &= qBitField_SafeMask( 0xFFFFFFFFuL, 32u, xBits );
        }
    }

    return retValue;
}
/*============================================================================*/
int qBitField_WriteUINTn( qBitField_t * const b,
                          const size_t index,
                          uint32_t value,
                          size_t xBits )
{
    int retValue = 0;

    if ( ( NULL != b ) && ( xBits <= 32u ) ) {
        uint32_t w, mask;

        if ( 1u == xBits ) {
            (void)qBitField_WriteBit( b, index, (uint8_t)value );
        }
        else if ( 32u == xBits ) {
            qBitField_Write_uint32( b, index, value );
        }
        else {
            w = qBitField_Read_uint32( b, index );
            value &= qBitField_SafeMask( 0xFFFFFFFFuL, 32u, xBits );
            /*cstat -ATH-overflow*/
            mask = (uint32_t)0xFFFFFFFFuL << xBits;
            /*cstat +ATH-overflow*/
            qBitField_Write_uint32( b, index, qBitField_MaskMerge( w, value, mask ) );
        }
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
float qBitField_ReadFloat( const qBitField_t * const b,
                           const size_t index )
{
    float retValue = 0.0f;

    if ( NULL != b ) {
        uint32_t rval;
        rval = qBitField_Read_uint32( b, index );
        (void)memcpy( &retValue, &rval, sizeof(float) );
    }

    return retValue;
}
/*============================================================================*/
int qBitField_WriteFloat( qBitField_t * const b,
                          const size_t index,
                          float value )
{
    int retValue = 0;

    if ( NULL != b ) {
        uint32_t fval = 0uL;
        (void)memcpy( &fval, &value, sizeof(float) );
        qBitField_Write_uint32( b, index, fval );
        retValue = 1;
    }

    return retValue;
}
/*============================================================================*/
void* qBitField_Dump( const qBitField_t * const b,
                      void * const dst,
                      size_t n )
{
    void *retValue = NULL;

    if ( ( NULL != b ) && ( NULL != dst ) ) {
        if ( n <= ( b->size/8u ) ) {
            retValue = memcpy( dst, (const void*)b->field, n );
        }
    }

    return retValue;
}
/*============================================================================*/
static uint32_t qBitField_Read_uint32( const qBitField_t *b,
                                       const size_t index )
{
    size_t slot, of, bits_taken;
    uint32_t result;

    slot = qBitField_BitSlot( index );
    of = qBitField_Offset( index );
    result = b->field[ slot ] >> of;
    bits_taken  = 32u - of;
    if ( ( 0u != of ) && ( ( index + bits_taken ) < b->size ) ) {
        /*cstat -CERT-INT30-C_a -ATH-shift-bounds -MISRAC2012-Rule-12.2 -CERT-INT34-C_b*/
        result |= b->field[ slot + 1u ] << (uint32_t)bits_taken;
        /*cstat +CERT-INT30-C_a +ATH-shift-bounds +MISRAC2012-Rule-12.2 +CERT-INT34-C_b*/
    }

    return result;
}
/*============================================================================*/
static void qBitField_Write_uint32( qBitField_t *b,
                                    const size_t index,
                                    const uint32_t value )
{
    uint32_t mask;
    size_t slot, of;

    slot = qBitField_BitSlot( index );
    of = qBitField_Offset( index );
    if ( 0u == of ) {
        b->field[ slot ] = value;
    }
    else {
        mask = qBitField_SafeMask( 0xFFFFFFFFuL, qBitField_LBit, of );
        b->field[ slot ] = ( value << of ) | ( b->field[ slot ] & mask );
        if ( ++slot < b->nSlots ) {
            b->field[ slot ] = qBitField_SafeMask( value, 32u, of ) |
                               ( b->field[ slot ] & ( ~mask ) );
        }
    }
}
/*============================================================================*/
