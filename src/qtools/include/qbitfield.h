/*!
 * @file qbitfield.h
 * @author J. Camilo Gomez C.
 * @version 1.05
 * @note This file is part of the qTools distribution.
 * @brief A bit-field manipulation library.
 **/

#ifndef QBITFIELD_H
#define QBITFIELD_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdint.h>
    #include <string.h>

    typedef struct
    {
        /*! @cond  */
        uint32_t *field; /*-> points to the used-defined memory block to hold the bitfield storage */
        size_t size;     /*-> Bits capacity*/
        size_t nSlots;   /*-> Number of 32-bit slots for the bitfield storage*/
        /*! @endcond  */
    } qBitField_t;

    #define QBITFIELD_SIZE( NBITS )     ( 4* ( ( ( NBITS-1 ) / 32 ) + 1 ) )

    int qBitField_Setup( qBitField_t *instance, void *area, size_t area_size );
    int qBitField_ClearAll( qBitField_t *instance );
    int qBitField_SetAll( qBitField_t *instance );
    int qBitField_SetBit( qBitField_t *instance, size_t index );
    int qBitField_ClearBit( qBitField_t *instance, size_t index );
    int qBitField_ToggleBit( qBitField_t *instance, size_t index );
    uint8_t qBitField_ReadBit( const qBitField_t *instance, size_t index );
    int qBitField_WriteBit( qBitField_t *instance, size_t index, uint8_t value );
    uint32_t qBitField_ReadUINTn( qBitField_t *instance, size_t index, size_t xBits );
    int qBitField_WriteUINTn( qBitField_t *instance, size_t index, uint32_t value, size_t xBits );
    float qBitField_ReadFloat( const qBitField_t *instance, size_t index );
    int qBitField_WriteFloat( qBitField_t *instance, size_t index, float value );
    void* qBitField_Dump( qBitField_t *instance, void* dst, size_t n );

#ifdef __cplusplus
}
#endif

#endif