/*!
 * @file qcrc.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qcrc.h"

typedef struct _qCRC8_params_s{
    uint8_t poly;
    uint8_t init;
    uint8_t xorout;
    uint8_t (*act)( const struct _qCRC8_params_s *opt, uint8_t *data, size_t length );
} qCRC8_params_t;

typedef struct _qCRC16_params_s{
    uint16_t poly;
    uint16_t init;
    uint16_t xorout;
    uint16_t (*act)( const struct _qCRC16_params_s *opt, uint8_t *data, size_t length );
} qCRC16_params_t;

typedef struct _qCRC32_params_s{
    uint32_t poly;
    uint32_t init;
    uint32_t xorout;
    uint32_t (*act)( const struct _qCRC32_params_s *opt, uint8_t *data, size_t length );
} qCRC32_params_t;

static uint8_t qCRC8_False( const qCRC8_params_t *opt, uint8_t *data, size_t length );
static uint8_t qCRC8_True( const qCRC8_params_t *opt, uint8_t *data, size_t length );

static uint16_t qCRC16_False( const qCRC16_params_t *opt, uint8_t *data, size_t length );
static uint16_t qCRC16_True( const qCRC16_params_t *opt, uint8_t *data, size_t length );

static uint32_t qCRC32_False( const qCRC32_params_t *opt, uint8_t *data, size_t length );
static uint32_t qCRC32_True( const qCRC32_params_t *opt, uint8_t *data, size_t length );

const qCRC8_params_t CRC8_Mode[ ] = {
    { 0x07, 0x00, 0x00, &qCRC8_False },        /*CRC8       : Poly = 0x07, Init = 0x00, RefIn = false, RefOut = false, Xorout = 0x00*/
    { 0x07, 0x00, 0x55, &qCRC8_False },        /*ITU        : Poly = 0x07, Init = 0x00, RefIn = false, RefOut = false, Xorout = 0x55*/
    { 0xE0, 0xFF, 0x00, &qCRC8_True  },        /*ROHC       : Poly = 0x07, Init = 0xFF, RefIn = true , RefOut = true , Xorout = 0x00*/
    { 0x8C, 0x00, 0x00, &qCRC8_True  },        /*MAXIM      : Poly = 0x31, Init = 0x00, RefIn = true , RefOut = true , Xorout = 0x00*/
    { 0x9B, 0xFF, 0x00, &qCRC8_False },        /*CDMA2000   : Poly = 0x9B, Init = 0xFF, RefIn = false, RefOut = false, Xorout = 0x00*/
    { 0x9C, 0x00, 0x00, &qCRC8_True  },        /*DARC       : Poly = 0x39, Init = 0x00, RefIn = true,  RefOut = true , Xorout = 0x00*/
    { 0xD5, 0x00, 0x00, &qCRC8_False },        /*DVB-S2     : Poly = 0xD5, Init = 0x00, RefIn = false, RefOut = false, Xorout = 0x00*/
    { 0xB8, 0xFF, 0x00, &qCRC8_True  },        /*EBU        : Poly = 0x1D, Init = 0xFF, RefIn = true,  RefOut = true , Xorout = 0x00*/
    { 0x1D, 0xFD, 0x00, &qCRC8_False },        /*I-CODE     : Poly = 0x1D, Init = 0xFD, RefIn = false, RefOut = false, Xorout = 0x00*/
    { 0xD9, 0x00, 0x00, &qCRC8_True  },        /*WCDMA      : Poly = 0x9B, Init = 0x00, RefIn = true,  RefOut = true,  Xorout = 0x00*/
};

const qCRC16_params_t CRC16_Mode[ ] = {
    { 0xA001 , 0x0000, 0x0000, &qCRC16_True  }, /*IBM       : Poly = 0x8005, Init = 0x0000, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0xA001 , 0x0000, 0xFFFF, &qCRC16_True  }, /*MAXIM     : Poly = 0x8005, Init = 0x0000, RefIn = true , RefOut = true , Xorout = 0xFFFF*/
    { 0xA001 , 0xFFFF, 0xFFFF, &qCRC16_True  }, /*USB       : Poly = 0x8005, Init = 0xFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFF*/
    { 0xA001 , 0xFFFF, 0x0000, &qCRC16_True  }, /*MODBUS    : Poly = 0x8005, Init = 0xFFFF, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0x8408 , 0x0000, 0x0000, &qCRC16_True  }, /*CCITT     : Poly = 0x1021, Init = 0x0000, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0x1021 , 0xFFFF, 0x0000, &qCRC16_False }, /*CCITT-F   : Poly = 0x1021, Init = 0xFFFF, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x8408 , 0xFFFF, 0xFFFF, &qCRC16_True  }, /*X25       : Poly = 0x1021, Init = 0xFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFF*/
    { 0x1021 , 0x0000, 0x0000, &qCRC16_False }, /*XMODEM    : Poly = 0x1021, Init = 0xFFFF, RefIn = false, RefOut = false, Xorout = 0xFFFF*/
    { 0xA6BC , 0x0000, 0xFFFF, &qCRC16_True  }, /*DNP       : Poly = 0x3D65, Init = 0xFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFF*/
    { 0x1021 , 0x1D0F, 0x0000, &qCRC16_False }, /*AUG-CCITT : Poly = 0x1021, Init = 0x1D0F, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x8005 , 0x0000, 0x0000, &qCRC16_False }, /*BUYPASS   : Poly = 0x8005, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0xC867 , 0xFFFF, 0x0000, &qCRC16_False }, /*CDMA2000  : Poly = 0xC867, Init = 0xFFFF, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x8005 , 0x800D, 0x0000, &qCRC16_False }, /*DDS_110   : Poly = 0x8005, Init = 0x800D, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x0589 , 0x0000, 0x0001, &qCRC16_False }, /*DECT_R    : Poly = 0x0589, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0x0001*/
    { 0x0589 , 0x0000, 0x0000, &qCRC16_False }, /*DECT_X    : Poly = 0x0589, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x3D65 , 0x0000, 0xFFFF, &qCRC16_False }, /*EN13757   : Poly = 0x3D65, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0xFFFF*/
    { 0x1021 , 0xFFFF, 0xFFFF, &qCRC16_False }, /*GENIBUS   : Poly = 0x1021, Init = 0xFFFF, RefIn = false, RefOut = false, Xorout = 0xFFFF*/
    { 0x8408 , 0xFFFF, 0x0000, &qCRC16_True  }, /*MCRF4XX   : Poly = 0x1021, Init = 0xFFFF, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0x8408 , 0xB2AA, 0x0000, &qCRC16_True  }, /*RIELLO    : Poly = 0x1021, Init = 0xB2AA, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0x8BB7 , 0x0000, 0x0000, &qCRC16_False }, /*T10_DIF   : Poly = 0x8BB7, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0xA097 , 0x0000, 0x0000, &qCRC16_False }, /*TELEDISK  : Poly = 0xA097, Init = 0x0000, RefIn = false, RefOut = false, Xorout = 0x0000*/
    { 0x8408 , 0x89EC, 0x0000, &qCRC16_True  }, /*TMS37157  : Poly = 0x1021, Init = 0x89EC, RefIn = true , RefOut = true , Xorout = 0x0000*/
    { 0x8408 , 0xC6C6, 0x0000, &qCRC16_True  }, /*CRC-A     : Poly = 0x1021, Init = 0xC6C6, RefIn = false, RefOut = false, Xorout = 0x0000*/
};

const qCRC32_params_t CRC32_Mode[ ] = {
    { 0xEDB88320uL , 0xFFFFFFFFuL, 0xFFFFFFFFuL, &qCRC32_True  }, /*CRC32     : Poly = 0x04C11DB7, Init = 0xFFFFFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFFFFFF*/
    { 0x04C11DB7uL , 0xFFFFFFFFuL, 0x00000000uL, &qCRC32_False }, /*MPEG2     : Poly = 0x04C11DB7, Init = 0xFFFFFFFF, RefIn = false, RefOut = false, Xorout = 0x00000000*/
    { 0x04C11DB7uL , 0xFFFFFFFFuL, 0xFFFFFFFFuL, &qCRC32_False }, /*BZIP2     : Poly = 0x04C11DB7, Init = 0xFFFFFFFF, RefIn = false, RefOut = false, Xorout = 0xFFFFFFFF*/
    { 0x82F63B78uL , 0xFFFFFFFFuL, 0xFFFFFFFFuL, &qCRC32_True  }, /*32C       : Poly = 0x1EDC6F41, Init = 0xFFFFFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFFFFFF*/
    { 0xD419CC15uL , 0xFFFFFFFFuL, 0xFFFFFFFFuL, &qCRC32_True  }, /*32D       : Poly = 0xA833982B, Init = 0xFFFFFFFF, RefIn = true , RefOut = true , Xorout = 0xFFFFFFFF*/
    { 0x04C11DB7uL , 0x00000000uL, 0xFFFFFFFFuL, &qCRC32_False }, /*POSIX     : Poly = 0x04C11DB7, Init = 0x00000000, RefIn = false, RefOut = false, Xorout = 0xFFFFFFFF*/
    { 0x814141ABuL , 0x00000000uL, 0x00000000uL, &qCRC32_False }, /*32Q       : Poly = 0x814141AB, Init = 0x00000000, RefIn = false, RefOut = false, Xorout = 0x00000000*/
    { 0xEDB88320uL , 0xFFFFFFFFuL, 0x00000000uL, &qCRC32_True  }, /*JAMCRC    : Poly = 0x04C11DB7, Init = 0xFFFFFFFF, RefIn = true , RefOut = true , Xorout = 0x00000000*/
    { 0x000000AFuL , 0x00000000uL, 0x00000000uL, &qCRC32_False }, /*XFER      : Poly = 0x000000AF, Init = 0x00000000, RefIn = false, RefOut = false, Xorout = 0x00000000*/
};

/*============================================================================*/
uint16_t qCRC8_Compute( qCRC8_Mode_t mode, uint8_t *data, size_t length )
{
    uint8_t crc = 0x00u;
    if( ( NULL != data ) && ( length > 0u ) && ( mode >= 0 ) && ( (size_t)mode < ( sizeof(CRC8_Mode)/sizeof(CRC8_Mode[0]) ) ) ) {
        crc = CRC8_Mode[ mode ].act( &CRC8_Mode[ mode ], data, length ); 
    }   
    return crc;
}
/*============================================================================*/
static uint8_t qCRC8_False( const qCRC8_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint8_t crc = opt->init;
    
    while( 0u != length-- ) {
        crc ^= *data++; 
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x80u ) )?  (uint8_t)( ( crc << 1 )^opt->poly ) : (uint8_t)( crc << 1 );
        }
    }
    return crc^opt->xorout;
}
/*============================================================================*/
static uint8_t qCRC8_True( const qCRC8_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint8_t crc = opt->init;
    
    while( 0u != length-- ) {
        crc ^= *data++; 
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x01u ) )?  ( ( crc >> 1 )^opt->poly ) : ( crc >> 1 );
        }
    }
    return crc^opt->xorout;  
}
/*============================================================================*/
uint16_t qCRC16_Compute( qCRC16_Mode_t mode, uint8_t *data, size_t length )
{
    uint16_t crc = 0x0000u;
    if( ( NULL != data ) && ( length > 0u ) && ( mode >= 0 ) && ( (size_t)mode < ( sizeof(CRC16_Mode)/sizeof(CRC16_Mode[0]) ) ) ) {
        crc = CRC16_Mode[ mode ].act( &CRC16_Mode[ mode ], data, length ); 
    }   
    return crc;
}
/*============================================================================*/
static uint16_t qCRC16_False( const qCRC16_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint16_t crc = opt->init;

    while ( 0u != length-- ) {
        crc ^= (uint16_t)( (uint16_t)( *data++ ) << 8u );
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x8000u ) )?  (uint16_t)( ( crc << 1 )^opt->poly ) : (uint16_t)( crc << 1 );
        }
    }
    return crc^opt->xorout;
}
/*============================================================================*/
static uint16_t qCRC16_True( const qCRC16_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint16_t crc = opt->init;

    while ( 0u != length-- ) {
        crc ^= (uint16_t)( *data++ );
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x0001u ) )?  ( ( crc >> 1 )^opt->poly ) : ( crc >> 1 );
        }
    }
    return crc^opt->xorout;   
}
/*============================================================================*/
uint32_t qCRC32_Compute( qCRC32_Mode_t mode, uint8_t *data, size_t length )
{
    uint32_t crc = 0x00000000uL;
    if( ( NULL != data ) && ( length > 0u ) && ( mode >= 0 ) && ( (size_t)mode < ( sizeof(CRC32_Mode)/sizeof(CRC32_Mode[0]) ) ) ) {
        crc = CRC32_Mode[ mode ].act( &CRC32_Mode[ mode ], data, length ); 
    }   
    return crc;
}
/*============================================================================*/
static uint32_t qCRC32_False( const qCRC32_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint32_t crc = opt->init;

    while ( 0u != length-- ) {
        crc ^= (uint32_t)( *data++ ) << 24;
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x80000000u ) )?  ( ( crc << 1 )^opt->poly ) : ( crc << 1 );
        }
    }
    return crc^opt->xorout;
}
/*============================================================================*/
static uint32_t qCRC32_True( const qCRC32_params_t *opt, uint8_t *data, size_t length )
{
    uint8_t i;
    uint32_t crc = opt->init;

    while ( 0u != length-- ) {
        crc ^= (uint32_t)( *data++ );
        for ( i = 0u; i < 8u; ++i ) {
            crc = ( 0u != ( crc & 0x00000001u ) )?  ( ( crc >> 1 )^opt->poly ) : ( crc >> 1 );
        }
    }
    return crc^opt->xorout;   
}
/*============================================================================*/