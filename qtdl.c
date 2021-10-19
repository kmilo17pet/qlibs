/*!
 * @file qtdl.c   
 * @author J. Camilo Gomez C.
 * @note This file is part of the qTools distribution.
 **/

#include "qtdl.h"

static void qTDL_InsertNewest( qTDL_t *q, float data );
static void qTDL_RemoveOldest( qTDL_t *q );

/*============================================================================*/
void qTDL_Setup( qTDL_t *q, float *area, size_t n, float initval )
{   
    q->itemcount = n;
    q->head = area;
    qTDL_Flush( q, initval );
}
/*============================================================================*/
void qTDL_Flush( qTDL_t *q, float initval )
{
    size_t i;

    q->tail = &q->head[ q->itemcount ];
    q->wr = q->head;
    /*cstat -CERT-INT30-C_a*/
    q->rd = &q->head[ q->itemcount - 1u ];    
    /*cstat +CERT-INT30-C_a*/
    for ( i = 0u ; i < q->itemcount ; ++i ) { /*initialize the queue*/
        qTDL_InsertNewest( q, initval ); /*the queue should be always full*/
    }
}
/*============================================================================*/
static void qTDL_InsertNewest( qTDL_t *q, float data )
{
    q->wr[ 0 ] = data;
    q->wr++;
    if ( q->wr >= q->tail ) {
        q->wr = q->head;
    }  
}
/*============================================================================*/
static void qTDL_RemoveOldest( qTDL_t *q )
{
    if ( ++q->rd >= q->tail ) {
        q->rd = q->head;
    }
}
/*============================================================================*/
float qTDL_GetOldest( qTDL_t *q )
{
    return  ( ( q->rd + 1u ) >= q->tail )? q->head[ 0 ] : q->rd[ 1 ]; 
}
/*============================================================================*/
float qTDL_GetAtIndex( qTDL_t *q, size_t i )
{
    return ( ( q->wr >= q->rd ) && ( ( q->head + i ) >= q->wr ) ) ? q->rd[ q->itemcount - i ] :  *( q->rd - i );   
}
/*============================================================================*/
float qTDL_GetRecent( qTDL_t *q )
{
    return q->rd[ 0 ];
}
/*============================================================================*/
void qTDL_InsertSample( qTDL_t *q, float data )
{
    qTDL_RemoveOldest( q );
    qTDL_InsertNewest( q, data );
}
/*============================================================================*/