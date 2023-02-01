/*!
 * @file qcrc.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qcrc.h"

static uint32_t qCRCx_Reflect( uint32_t xData,
                               const uint8_t nBits );

/*============================================================================*/
static uint32_t qCRCx_Reflect( uint32_t xData,
                               const uint8_t nBits )
{
    uint32_t  r = 0;
    uint8_t xBit;
    /*Reflect the data about the center bit*/
    for ( xBit= 0u ; xBit < nBits ; ++xBit ) {
        /*if the LSB bit is set, set the reflection of it*/
        if ( 0u != ( xData & 0x01u ) ) {
            /*cstat -MISRAC2012-Rule-10.8 -ATH-shift-bounds -MISRAC2012-Rule-12.2 -CERT-INT34-C_b*/
            /*cppcheck-suppress misra-c2012-10.8 */
            r |= (uint32_t)( 1u << ( ( nBits - 1u ) - xBit ) );
            /*cstat +MISRAC2012-Rule-10.8 +ATH-shift-bounds +MISRAC2012-Rule-12.2 +CERT-INT34-C_b*/
        }
        xData >>= 1u;
    }

    return r;
}
/*============================================================================*/
uint32_t qCRCx( const qCRC_Mode_t mode,
                const void * const pData,
                const size_t length,
                uint32_t poly,
                const uint32_t init,
                const uint8_t refIn,
                const uint8_t refOut,
                uint32_t xorOut )
{
    uint32_t crc = 0u;
    /*cstat -ATH-cmp-unsign-pos*/
    if ( ( NULL != pData ) && ( length > 0u ) && ( mode >= QCRC8 ) && ( mode <= QCRC32 ) ) {
    /*cstat +ATH-cmp-unsign-pos*/
        size_t i;
        uint8_t xBit;
        const uint32_t widthValues[ 3 ] = { 8uL, 16uL, 32uL };
        const uint32_t width = widthValues[ mode ];
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        uint8_t const * const msg = pData;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        const uint32_t wd1 = (uint32_t)width - 8u;
        const uint32_t topBit = (uint32_t)1u << ( width - 1u );
        const uint32_t bitMask = ( 0xFFFFFFFFu >> ( 32u - width ) );
        poly &= bitMask;
        xorOut &= bitMask;
        crc = init;
        /*Perform modulo-2 division, a byte at a time. */
        for ( i = 0u ; i < length ; ++i ) {
            /*cstat -CERT-INT34-C_a*/
            crc ^= ( 0u != refIn ) ?
                   ( qCRCx_Reflect( (uint32_t)msg[ i ], 8u ) <<  wd1 ) :
                   ( (uint32_t)msg[ i ] << wd1 );
            /*cstat +CERT-INT34-C_a*/
            for ( xBit = 8u ; xBit > 0u ; --xBit ) {
                /*try to divide the current data bit*/
                crc = ( 0u != ( crc & topBit ) ) ? ( ( crc << 1u ) ^ poly )
                                                 : ( crc << 1u );
            }
        }
        crc = ( 0u != refOut ) ? ( qCRCx_Reflect( crc, (uint8_t)width )^xorOut )
                               : ( crc^xorOut );
        crc &= bitMask;
    }

    return crc;
}
/*============================================================================*/
