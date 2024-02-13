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
    for ( xBit= 0U ; xBit < nBits ; ++xBit ) {
        /*if the LSB bit is set, set the reflection of it*/
        if ( 0U != ( xData & 0x01U ) ) {
            /*cstat -MISRAC2012-Rule-10.8 -ATH-shift-bounds -MISRAC2012-Rule-12.2 -CERT-INT34-C_b*/
            /*cppcheck-suppress misra-c2012-10.8 */
            r |= (uint32_t)( 1U << ( ( nBits - 1U ) - xBit ) );
            /*cstat +MISRAC2012-Rule-10.8 +ATH-shift-bounds +MISRAC2012-Rule-12.2 +CERT-INT34-C_b*/
        }
        xData >>= 1U;
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
    uint32_t crc = 0U;
    /*cstat -ATH-cmp-unsign-pos*/
    if ( ( NULL != pData ) && ( length > 0U ) && ( mode >= QCRC8 ) && ( mode <= QCRC32 ) ) {
    /*cstat +ATH-cmp-unsign-pos*/
        size_t i;
        uint8_t xBit;
        const uint32_t widthValues[ 3 ] = { 8UL, 16UL, 32UL };
        const uint32_t width = widthValues[ mode ];
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        uint8_t const * const msg = pData;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        const uint32_t wd1 = (uint32_t)width - 8U;
        const uint32_t topBit = (uint32_t)1U << ( width - 1U );
        const uint32_t bitMask = ( 0xFFFFFFFFU >> ( 32U - width ) );
        poly &= bitMask;
        xorOut &= bitMask;
        crc = init;
        /*Perform modulo-2 division, a byte at a time. */
        for ( i = 0U ; i < length ; ++i ) {
            /*cstat -CERT-INT34-C_a*/
            crc ^= ( 0U != refIn ) ?
                   ( qCRCx_Reflect( (uint32_t)msg[ i ], 8U ) <<  wd1 ) :
                   ( (uint32_t)msg[ i ] << wd1 );
            /*cstat +CERT-INT34-C_a*/
            for ( xBit = 8U ; xBit > 0U ; --xBit ) {
                /*try to divide the current data bit*/
                crc = ( 0U != ( crc & topBit ) ) ? ( ( crc << 1U ) ^ poly )
                                                 : ( crc << 1U );
            }
        }
        crc = ( 0U != refOut ) ? ( qCRCx_Reflect( crc, (uint8_t)width )^xorOut )
                               : ( crc^xorOut );
        crc &= bitMask;
    }

    return crc;
}
/*============================================================================*/
