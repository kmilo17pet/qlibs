/*!
 * @file qcirmed.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qcirmed.h"

static void qCirMed_PlaceBack( qCirMed_t *b, float xk );
static void qCirMed_RemoveRecent( qCirMed_t *b );

/*============================================================================*/
int qCirMed_Setup( qCirMed_t *b, float *dataarea, size_t n, float initval )
{
    int retVal = 0;
    if ( NULL != b ) {
        b->itemcount = n;
        b->head = dataarea;
        retVal = qCirMed_Reset( b, initval );
    }
    return retVal;
}
/*============================================================================*/
int qCirMed_Reset( qCirMed_t *b, float initval )
{
    int retVal = 0;
    if ( NULL != b ) {
        size_t i;
        b->recent = initval;
        b->oldest = initval;
        b->sum = 0.0f;
        for ( i = 0u; i < b->itemcount; ++i ) {
            b->head[ i ] = initval;
            b->sum += initval;
        }
        
        b->tail = &b->head[ b->itemcount ];
        b->itemswaiting = 0u;
        b->writer = b->head;
        b->reader = &b->head[ b->itemcount - 1u ];
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/
static void qCirMed_PlaceBack( qCirMed_t *b, float xk )
{
    b->writer[ 0 ] = xk;
    b->writer++;
    if ( b->writer >= b->tail ) {
        b->writer = b->head;
    }
    ++b->itemswaiting;
}
/*============================================================================*/
static void qCirMed_RemoveRecent( qCirMed_t *b )
{
    if ( b->itemswaiting > 0u ) {
        b->reader++;
        if ( b->reader >= b->tail ) {
            b->reader  = b->head;
        }
        --b->itemswaiting;
    }
}
/*============================================================================*/
int qCirMed_Insert( qCirMed_t *b, float xk )
{
    int retVal = 0;
    if ( NULL != b ) {
        b->sum += xk - b->oldest;
        b->recent = xk;
        
        if ( b->itemswaiting < b->itemcount ) {
            qCirMed_PlaceBack( b, xk );
        }
        else {
            qCirMed_RemoveRecent( b );
            qCirMed_PlaceBack( b, xk );
        }
        
        if ( b->itemswaiting == b->itemcount ) {
            b->oldest = ( ( b->reader + 1 ) >= b->tail )? b->head[ 0 ] : b->reader[ 1 ];
        }
        else {
            b->oldest = *( b->tail - 1 );  
        }
        retVal = 1;
    }
    return retVal;
}
/*============================================================================*/