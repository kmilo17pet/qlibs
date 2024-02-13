/*!
 * @file qtdl.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qtdl.h"

static void qTDL_InsertNewest( qTDL_t * const q, float sample );
static void qTDL_RemoveOldest( qTDL_t * const q );

/*============================================================================*/
void qTDL_Setup( qTDL_t * const q,
                 float * const area,
                 const size_t n,
                 const float initVal )
{
    q->itemCount = n;
    q->head = area;
    qTDL_Flush( q, initVal );
}
/*============================================================================*/
void qTDL_Flush( qTDL_t * const q,
                 const float initVal )
{
    size_t i;

    q->tail = &q->head[ q->itemCount ];
    q->wr = q->head;
    /*cstat -CERT-INT30-C_a*/
    q->rd = &q->head[ q->itemCount - 1U ];
    /*cstat +CERT-INT30-C_a*/
    for ( i = 0U ; i < q->itemCount ; ++i ) { /*initialize the queue*/
        qTDL_InsertNewest( q, initVal ); /*the queue should be always full*/
    }
}
/*============================================================================*/
static void qTDL_InsertNewest( qTDL_t * const q,
                               const float sample )
{
    q->wr[ 0 ] = sample;
    q->wr++;
    if ( q->wr >= q->tail ) {
        q->wr = q->head;
    }
}
/*============================================================================*/
static void qTDL_RemoveOldest( qTDL_t * const q )
{
    if ( ++q->rd >= q->tail ) {
        q->rd = q->head;
    }
}
/*============================================================================*/
float qTDL_GetOldest( const qTDL_t * const q )
{
    return  ( ( q->rd + 1U ) >= q->tail ) ? q->head[ 0 ] : q->rd[ 1 ];
}
/*============================================================================*/
float qTDL_GetAtIndex( const qTDL_t * const q,
                       const size_t i )
{
    return ( ( q->wr >= q->rd ) && ( ( q->head + i ) >= q->wr ) ) ? q->rd[ q->itemCount - i ]
                                                                  : *( q->rd - i );
}
/*============================================================================*/
float qTDL_GetRecent( const qTDL_t * const q )
{
    return q->rd[ 0 ];
}
/*============================================================================*/
void qTDL_InsertSample( qTDL_t * const q,
                        const float sample )
{
    qTDL_RemoveOldest( q );
    qTDL_InsertNewest( q, sample );
}
/*============================================================================*/