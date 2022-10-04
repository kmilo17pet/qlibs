/*!
 * @file qcrc.h
 * @author J. Camilo Gomez C.
 * @version 1.04
 * @note This file is part of the qLibs distribution.
 * @brief CRC calculation library.
 **/

#ifndef QCRC_H
#define QCRC_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>

    /** @addtogroup  qcrc Generic Cyclic Redundancy Check (CRC) calculator
    * @brief API for the qCRC Generic Cyclic Redundancy Check (CRC) calculator
    *  @{
    */

    /**
    * @brief Enumeration with all the supported cyclic redundancy checks
    */ 
    typedef enum {
        QCRC8 = 0,      /*!< 8-Bit Cyclic Redundancy Check*/
        QCRC16,         /*!< 16-Bit Cyclic Redundancy Check*/
        QCRC32,         /*!< 32-Bit Cyclic Redundancy Check*/
    } qCRC_Mode_t;

    /**
    * @brief Calculates in one pass the common @a width bit CRC value for a
    * block of data that is passed to the function together with a parameter
    * indicating the @a length.
    * @param[in] mode To select the CRC calculation mode. Only the following
    * values are supported: QCRC8, QCRC16 and QCRC32.
    * @param[in] pData A pointer to the block of data.
    * @param[in] length The number of bytes in @a data.
    * @param[in] poly CRC polynomial value.
    * @param[in] init CRC initial value.
    * @param[in] refIn If true, the input data is reflected before processing.
    * @param[in] refOut If true, the CRC result is reflected before output.
    * @param[in] xorOut The final XOR value.
    * @return The CRC value for @a data.
    */
    uint32_t qCRCx( const qCRC_Mode_t mode,
                    const void * const pData,
                    const size_t length,
                    uint32_t poly,
                    const uint32_t init,
                    const uint8_t refIn,
                    const uint8_t refOut,
                    uint32_t xorOut );

    /** @brief CRC-8 with poly = 0x07 init = 0x00 refIn = false refOut = false
     * xorOut= 0x00 */
    #define qCRC8( pData, length )                                          \
    qCRCx( QCRC8, pData, length, 0x07uL, 0x00uL, 0u, 0u, 0x00uL );          \

    /** @brief CRC-8/CDMA2000 with poly = 0x9B init = 0xFF refIn = false
     * refOut = false  xorOut= 0x00 */
    #define qCRC8_CDMA2000( pData, length )                                 \
    qCRCx( QCRC8, pData, length, 0x9BuL, 0xFFuL, 0u, 0u, 0x00uL );          \

    /** @brief CRC-8/CDMA2000 with poly = 0x39 init = 0x00 refIn = true
     * refOut = true  xorOut= 0x00 */
    #define qCRC8_DARC( pData, length )                                     \
    qCRCx( QCRC8, pData, length, 0x39uL, 0x00uL, 1u, 1u, 0x00uL );          \

    /** @brief CRC-8/CDMA2000 with poly = 0xD5 init = 0x00 refIn = false
     * refOut = false xorOut= 0x00 */
    #define qCRC8_DVB_S2( pData, length )                                   \
    qCRCx( QCRC8, pData, length, 0xD5uL, 0x00uL, 0u, 0u, 0x00uL );          \

    /** @brief CRC-8/EBU with poly = 0x1D init = 0xFF refIn = true refOut = true
     * xorOut= 0x00 */
    #define qCRC8_EBU( pData, length )                                      \
    qCRCx( QCRC8, pData, length, 0x1DuL, 0xFFuL, 1u, 1u, 0x00uL );          \

    /** @brief CRC-8/I-CODE with poly = 0x1D init = 0xFD refIn = false
     * refOut = false  xorOut= 0x00 */
    #define qCRC8_I_CODE( pData, length )                                   \
    qCRCx( QCRC8, pData, length, 0x1DuL, 0xFDuL, 0u, 0u, 0x00uL );          \

    /** @brief CRC-8/ITU with poly = 0x07 init = 0x00 refIn = false
     * refOut = false  xorOut= 0x55 */
    #define qCRC8_ITU( pData, length )                                      \
    qCRCx( QCRC8, pData, length, 0x07uL, 0x00uL, 0u, 0u, 0x55uL );          \

    /** @brief CRC-8/MAXIM with poly = 0x31 init = 0x00 refIn = true 
     * refOut = true  xorOut= 0x00 */
    #define qCRC8_MAXIM( pData, length )                                    \
    qCRCx( QCRC8, pData, length, 0x31uL, 0x00uL, 1u, 1u, 0x00uL );          \

    /** @brief CRC-8/ROHC with poly = 0x07 init = 0xFF refIn = true 
     * refOut = true  xorOut= 0x00 */
    #define qCRC8_ROHC( pData, length )                                     \
    qCRCx( QCRC8, pData, length, 0x07uL, 0xFFuL, 1u, 1u, 0x00uL );          \

    /** @brief CRC-8/WCDMA with poly = 0x9B init = 0x00 refIn = true 
     * refOut = true  xorOut= 0x00 */
    #define qCRC8_WCDMA( pData, length )                                    \
    qCRCx( QCRC8, pData, length, 0x9BuL, 0x00uL, 1u, 1u, 0x00uL );          \

    /** @brief CRC-16/CCITT-FALSE with poly = 0x1021 init = 0xFFFF 
     * refIn = false refOut = false  xorOut= 0x0000 */
    #define qCRC16_CCITT_FALSE( pData, length )                             \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xFFFFuL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/ARC with poly = 0x8005 init = 0x0000 refIn = true r
     * efOut = true  xorOut= 0x0000 */
    #define qCRC16_ARC( pData, length )                                     \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0x0000uL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/AUG-CCITT with poly = 0x1021 init = 0x1D0F refIn = false
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_AUG_CCITT( pData, length )                               \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0x1D0FuL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/BUYPASS with poly = 0x8005 init = 0x0000 refIn = false
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_BUYPASS( pData, length )                                 \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0x0000uL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/CDMA2000 with poly = 0xC867 init = 0xFFFF refIn = false
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_CDMA2000( pData, length )                                \
    qCRCx( QCRC16, pData, length, 0xC867uL, 0xFFFFuL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/DDS-110 with poly = 0x8005 init = 0x800D refIn = false
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_DDS_110( pData, length )                                 \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0x800DuL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/DECT-R with poly = 0x0589 init = 0x0000 refIn = false
     * refOut = false  xorOut= 0x0001 */
    #define qCRC16_DECT_R( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x0589uL, 0x0000uL, 0u, 0u, 0x0001uL );   \

    /** @brief CRC-16/DECT-X with poly = 0x0589 init = 0x0000 refIn = false
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_DECT_X( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x0589uL, 0x0000uL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/DNP with poly = 0x3D98 init = 0x0000 refIn = true 
     * refOut = true xorOut= 0xFFFF */
    #define qCRC16_DNP( pData, length )                                     \
    qCRCx( QCRC16, pData, length, 0x3D65uL, 0x0000uL, 1u, 1u, 0xFFFFuL );   \

    /** @brief CRC-16/EN-13757 with poly = 0x3D65 init = 0x0000 refIn = false
     * refOut = false  xorOut= 0xFFFF */
    #define qCRC16_EN_13757( pData, length )                                \
    qCRCx( QCRC16, pData, length, 0x03D65uL, 0x0000uL, 0u, 0u, 0xFFFFuL );  \

    /** @brief CRC-16/GENIBUS with poly = 0x1021 init = 0xFFFF refIn = false 
     * refOut = false  xorOut= 0xFFFF */
    #define qCRC16_GENIBUS( pData, length )                                 \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xFFFFuL, 0u, 0u, 0xFFFFuL );   \

    /** @brief CRC-16/MAXIM with poly = 0x8005 init = 0x0000 refIn = true 
     * refOut = true xorOut= 0xFFFF */
    #define qCRC16_MAXIM( pData, length )                                   \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0x0000uL, 1u, 1u, 0xFFFFuL );   \

    /** @brief CRC-16/MCRF4XX with poly = 0x1021 init = 0xFFFF refIn = true 
     * refOut = true xorOut= 0x0000 */
    #define qCRC16_MCRF4XX( pData, length )                                 \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xFFFFuL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/RIELLO with poly = 0x1021 init = 0xB2AA refIn = true 
     * refOut = true  xorOut= 0x0000 */
    #define qCRC16_RIELLO( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xB2AAuL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/DECT-X with poly = 0x8bb7 init = 0x0000 refIn = false 
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_T10_DIF( pData, length )                                 \
    qCRCx( QCRC16, pData, length, 0x8BB7uL, 0x0000uL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/TELEDISK with poly = 0xA097 init = 0x0000 refIn = false 
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_TELEDISK( pData, length )                                \
    qCRCx( QCRC16, pData, length, 0xA097uL, 0x0000uL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-16/TMS37157 with poly = 0x1021 init = 0x89EC refIn = true 
     * refOut = true  xorOut= 0x0000 */
    #define qCRC16_TMS37157( pData, length )                                \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0x89ECuL, 1u, 1u, 0x0000uL );   \

/** @brief CRC-16/USB with poly = 0x8005 init = 0xFFFF refIn = true 
 * refOut = true  xorOut= 0xFFFF */
    #define qCRC16_USB( pData, length )                                     \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0xFFFFuL, 1u, 1u, 0xFFFFuL );   \

    /** @brief CRC-A with poly = 0x1021 init = 0xC6C6 refIn = true 
     * refOut = true  xorOut= 0x0000 */
    #define qCRC16_A( pData, length )                                       \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xC6C6uL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/KERMIT with poly = 0x1021 init = 0x0000 refIn = true 
     * refOut = true  xorOut= 0x0000 */
    #define qCRC16_KERMIT( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0x0000uL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/MODBUS with poly = 0x8005 init = 0xFFFF refIn = true 
     * refOut = true  xorOut= 0x0000 */
    #define qCRC16_MODBUS( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x8005uL, 0xFFFFuL, 1u, 1u, 0x0000uL );   \

    /** @brief CRC-16/X-25 with poly = 0x1021 init = 0xFFFF refIn = true 
     * refOut = true  xorOut= 0xFFFF */
    #define qCRC16_X_25( pData, length )                                    \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0xFFFFuL, 1u, 1u, 0xFFFFuL );   \

    /** @brief CRC-16/XMODEM with poly = 0x1021 init = 0x0000 refIn = false 
     * refOut = false  xorOut= 0x0000 */
    #define qCRC16_XMODEM( pData, length )                                  \
    qCRCx( QCRC16, pData, length, 0x1021uL, 0x0000uL, 0u, 0u, 0x0000uL );   \

    /** @brief CRC-32 with poly = 0x04C11DB7 init = 0xFFFFFFFF refIn = true 
     * refOut = true xorOut= 0xFFFFFFFF */
    #define qCRC32( pData, length )                                         \
    qCRCx( QCRC32, pData, length, 0x04C11DB7uL, 0xFFFFFFFFuL, 1u, 1u, 0xFFFFFFFFuL );   \

    /** @brief CRC-32/BZIP2 with poly = 0x04C11DB7 init = 0xFFFFFFFF 
     * refIn = false refOut = false xorOut= 0xFFFFFFFF */
    #define qCRC32_BZIP2( pData, length )                                   \
    qCRCx( QCRC32, pData, length, 0x04C11DB7uL, 0xFFFFFFFFuL, 0u, 0u, 0xFFFFFFFFuL );   \

    /** @brief CRC-32C with poly = 0x1EDC6F41 init = 0xFFFFFFFF refIn = true 
     * refOut = true  xorOut= 0xFFFFFFFF */
    #define qCRC32_C( pData, length )                                       \
    qCRCx( QCRC32, pData, length, 0x1EDC6F41uL, 0xFFFFFFFFuL, 1u, 1u, 0xFFFFFFFFuL );   \

    /** @brief CRC-32D with poly = 0xA833982B init = 0xFFFFFFFF refIn = true 
     * refOut = true  xorOut= 0xFFFFFFFF */
    #define qCRC32_D( pData, length )                                       \
    qCRCx( QCRC32, pData, length, 0xA833982BuL, 0xFFFFFFFFuL, 1u, 1u, 0xFFFFFFFFuL );   \

    /** @brief CRC-32/JAMCRC with poly = 0x04C11DB7 init = 0xFFFFFFFF 
     * refIn = true refOut = true  xorOut= 0x00000000 */
    #define qCRC32_JAMCRC( pData, length )                                  \
    qCRCx( QCRC32, pData, length, 0x04C11DB7uL, 0xFFFFFFFFuL, 1u, 1u, 0x00000000uL );   \

    /** @brief CRC-32/MPEG2 with poly = 0x04C11DB7 init = 0xFFFFFFFF 
     * refIn = false refOut = false  xorOut= 0x00000000 */
    #define qCRC32_MPEG2( pData, length )                                   \
    qCRCx( QCRC32, pData, length, 0x04C11DB7uL, 0xFFFFFFFFuL, 0u, 0u, 0x00000000uL );   \

    /** @brief CRC-32/POSIX with poly = 0x04C11DB7 init = 0x00000000 
     * refIn = false refOut = false  xorOut= 0xFFFFFFFF */
    #define qCRC32_POSIX( pData, length )                                   \
    qCRCx( QCRC32, pData, length, 0x04C11DB7uL, 0x00000000uL, 0u, 0u, 0xFFFFFFFFuL );   \

    /** @brief CRC-32Q with poly = 0x814141AB init = 0x00000000 
     * refIn = false refOut = false  xorOut= 0x00000000 */
    #define qCRC32_Q( pData, length )                                       \
    qCRCx( QCRC32, pData, length, 0x814141ABuL, 0x00000000uL, 0u, 0u, 0x00000000uL );   \

    /** @brief CRC-32/XFER with poly = 0x000000AF init = 0x00000000 
     * refIn = false refOut = false  xorOut= 0x00000000 */
    #define qCRC32_XFER( pData, length )                                    \
    qCRCx( QCRC32, pData, length, 0x000000AFuL, 0x00000000uL, 0u, 0u, 0x00000000uL );   \

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif