/*!
 * @file qcrc.h
 * @author J. Camilo Gomez C.
 * @version 1.01
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

    typedef enum {
        QCRC8_MODE_CRC8 = 0,
        QCRC8_MODE_ITU = 1,
        QCRC8_MODE_ROHC = 2,
        QCRC8_MODE_MAXIM = 3,
        QCRC8_MODE_CDMA2000 = 4,
        QCRC8_MODE_DARC = 5,
        QCRC8_MODE_DVB_S2 = 6,
        QCRC8_MODE_EBU = 7,
        QCRC8_MODE_ICODE = 8,
        QCRC8_MODE_WCDMA = 9,
    } qCRC8_Mode_t;

    typedef enum {
        QCRC16_MODE_IBM = 0,
        QCRC16_MODE_ARC = 0,
        QCRC16_MODE_MAXIM = 1,
        QCRC16_MODE_USB = 2,
        QCRC16_MODE_MODBUS = 3,
        QCRC16_MODE_CCITT = 4,
        QCRC16_MODE_KERMIT = 4,
        QCRC16_MODE_CCITT_FALSE = 5,
        QCRC16_MODE_X25 = 6,
        QCRC16_MODE_XMODEM = 7,
        QCRC16_MODE_DNP = 8,
        QCRC16_MODE_AUG_CCITT = 9,
        QCRC16_MODE_BUYPASS = 10,
        QCRC16_MODE_CDMA2000 = 11,
        QCRC16_MODE_DDS_110 = 12,
        QCRC16_MODE_DECT_R = 13,
        QCRC16_MODE_DECT_X = 14,
        QCRC16_MODE_EN13757 = 15,
        QCRC16_MODE_GENIBUS = 16,
        QCRC16_MODE_MCRF4XX = 17,
        QCRC16_MODE_RIELLO = 18,/*pending for validation*/
        QCRC16_MODE_T10_DIF = 19,
        QCRC16_MODE_TELEDISK = 20,
        QCRC16_MODE_TMS37157 = 21, /*pending for validation*/
        QCRC16_MODE_CRC_A = 22,/*pending for validation*/
    } qCRC16_Mode_t;

    typedef enum {
        QCRC32_MODE_CRC32 = 0,
        QCRC32_MODE_MPEG2 = 1,
        QCRC32_MODE_BZIP2 = 2,
        QCRC32_MODE_32C = 3,
        QCRC32_MODE_32D = 4,
        QCRC32_MODE_POSIX = 5,
        QCRC32_MODE_32Q = 6,
        QCRC32_MODE_JAMCRC = 7,
        QCRC32_MODE_XFER = 8,
    } qCRC32_Mode_t;
    
    /**
    * @brief Calculates in one pass the common 8 bit CRC value for a byte array
    * that is passed to the function together with a parameter indicating 
    * the length.
    * @param[in] mode To select the CRC calculation mode. 
    * @param[in] data An array of @a length bytes
    * @param[in] The number of bytes in @a data
    * @return The CRC value for @a data.
    */        
    uint16_t qCRC8_Compute( qCRC8_Mode_t mode, uint8_t *data, size_t length );

    /**
    * @brief Calculates in one pass the common 16 bit CRC value for a byte array
    * that is passed to the function together with a parameter indicating 
    * the length.
    * @param[in] mode To select the CRC calculation mode. 
    * @param[in] data An array of @a length bytes
    * @param[in] The number of bytes in @a data
    * @return The CRC value for @a data.
    */      
    uint16_t qCRC16_Compute( qCRC16_Mode_t mode, uint8_t *data, size_t length );

    /**
    * @brief Calculates in one pass the common 32 bit CRC value for a byte array
    * that is passed to the function together with a parameter indicating 
    * the length.
    * @param[in] mode To select the CRC calculation mode. 
    * @param[in] data An array of @a length bytes
    * @param[in] The number of bytes in @a data
    * @return The CRC value for @a data.
    */  
    uint32_t qCRC32_Compute( qCRC32_Mode_t mode, uint8_t *data, size_t length );

#ifdef __cplusplus
}
#endif

#endif