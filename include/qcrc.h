/*!
 * @file qcrc.h
 * @author J. Camilo Gomez C.
 * @version 1.02
 * @note This file is part of the qTools distribution.
 * @brief CRC calculation library.
 **/

#ifndef QCRC_H
#define QCRC_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>

    typedef enum{
        QCRC8 = 0,
        QCRC16,
        QCRC32,
    } qCRC_Mode_t;  

    /**
    * @brief Calculates in one pass the common @a width bit CRC value for a block
    * of data that is passed to the function together with a parameter indicating 
    * the @a length.
    * @param[in] mode To select the CRC calculation mode. Only the following
    * values are supported: QCRC8, QCRC16 and QCRC32.
    * @param[in] data A pointer to the block of data.
    * @param[in] length The number of bytes in @a data.
    * @param[in] poly CRC polynomial value.
    * @param[in] init CRC initial value.
    * @param[in] refIn If true, the input data is reflected before processing.
    * @param[in] refOut If true, the CRC result is reflected before output.
    * @param[in] xorOut The final XOR value.
    * @return The CRC value for @a data.
    */      
    uint32_t qCRCx( const qCRC_Mode_t mode, void *data, const size_t length, uint32_t poly, const uint32_t init, const uint8_t refIn, const uint8_t refOut, uint32_t xorOut );

#ifdef __cplusplus
}
#endif

#endif