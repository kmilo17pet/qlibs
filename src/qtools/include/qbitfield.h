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

    /**
    * @brief Setup a initialize a BitField instance.
    * @param[in] instance A pointer to the BitField instance
    * @param[in] area  A pointer to the memory block to hold the BitField. 
    * Should be an uint8_t array of size #BITFIELD_SIZE(n), where n, is the 
    * number of bits inside the BitField.
    * @param[in] area_size The number of bytes in @a area.
    * @return 1 on success, otherwise return 0.
    */ 
    int qBitField_Setup( qBitField_t *instance, void *area, size_t area_size );

    /**
    * @brief Clear all the bits in the BitField.
    * @param[in] instance A pointer to the BitField instance
    * @return 1 on success, otherwise return 0.
    */     
    int qBitField_ClearAll( qBitField_t *instance );

    /**
    * @brief Set all the bits in the BitField.
    * @param[in] instance A pointer to the BitField instance
    * @return 1 on success, otherwise return 0.
    */       
    int qBitField_SetAll( qBitField_t *instance );

    /**
    * @brief Sets one bit in a BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index The bit-index.
    * @return 1 on success, otherwise return 0.
    */       
    int qBitField_SetBit( qBitField_t *instance, size_t index );

    /**
    * @brief Clears one bit in a BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index The bit-index.
    * @return 1 on success, otherwise return 0.
    */        
    int qBitField_ClearBit( qBitField_t *instance, size_t index );

    /**
    * @brief Toggles (i.e. reverses the state of) a bit in a BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index The bit-index.
    * @return 1 on success, otherwise return 0.
    */     
    int qBitField_ToggleBit( qBitField_t *instance, size_t index );

    /**
    * @brief Retrieve the state of a bit in a bitfield
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index The bit-index.
    * @return The value of the bit at @a index.
    */      
    uint8_t qBitField_ReadBit( const qBitField_t *instance, size_t index );

    /**
    * @brief Writes one bit in a bitfield
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index The bit-index.
    * @param[in] value The boolean value to write.
    * @return 1 on success, otherwise return 0.
    */         
    int qBitField_WriteBit( qBitField_t *instance, size_t index, uint8_t value );

    /**
    * @brief Reads an unsigned 32-bit value from the BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index  The bit-index taken as offset.
    * @param[in] xBits The number of bits to read. ( max allowed : 32 bits )
    * @return The value from the bitfield from the desired index
    */     
    uint32_t qBitField_ReadUINTn( qBitField_t *instance, size_t index, size_t xBits );

    /**
    * @brief Writes an unsigned n-bit value from the BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index  The bit-index taken as offset.
    * @param[in] value The value to write.
    * @param[in] xBits The number of bits to read. ( max allowed : 32 bits )
    * @return 1 on success, otherwise return 0.
    */         
    int qBitField_WriteUINTn( qBitField_t *instance, size_t index, uint32_t value, size_t xBits );

    /**
    * @brief Reads a 32-bit floating point value from the BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index  The bit-index taken as offset.
    * @return The floating point value from the BitField at the desired index
    */       
    float qBitField_ReadFloat( const qBitField_t *instance, size_t index );

    /**
    * @brief Writes a 32-bit floating point value to the BitField
    * @param[in] instance A pointer to the BitField instance
    * @param[in] index  The bit-index taken as offset.
    * @param[in] value The floating point value to write.
    * @return 1 on success, otherwise return 0.
    */       
    int qBitField_WriteFloat( qBitField_t *instance, size_t index, float value );

    /**
    * @brief Copies @a n bytes from the bit-field instance to a designed memory area.
    * @param[in] instance A pointer to the BitField instance
    * @param[in] dst  Pointer to the destination array where the content is to
    * be copied.
    * @param[in] n Number of bytes to copy.
    * @return Destination is returned on success, otherwise NULL.
    */       
    void* qBitField_Dump( qBitField_t *instance, void* dst, size_t n );

#ifdef __cplusplus
}
#endif

#endif