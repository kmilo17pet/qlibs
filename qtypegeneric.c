/*!
 * @file qtypegeneric.c
 * @author J. Camilo Gomez C.
 * @note This file is part of the qLibs distribution.
 **/

#include "qtypegeneric.h"
#include <string.h>

/*! @cond  */
typedef struct {
    uint8_t *lo, *hi;
} qTypeGeneric_SortStackNode_t;
/*! @endcond  */

static void qTypeGeneric_SortStackPush( qTypeGeneric_SortStackNode_t **top,
                                        uint8_t *low,
                                        uint8_t *high );

/*============================================================================*/
void qTypeGeneric_Swap( void * const x,
                        void * const y,
                        size_t n )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
    /*cppcheck-suppress misra-c2012-11.5 */
    uint8_t * const a = (uint8_t * const)x;
    uint8_t *b = (uint8_t * const)y;
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
    size_t i = 0U, j = 0U;
    do {
        uint8_t tmp = a[ i ];
        /*cstat -CERT-INT30-C_a*/
        a[ i++ ] = b[ j ];
        b[ j++ ] = tmp;
        /*cstat +CERT-INT30-C_a*/
    } while( --n > 0U );
}
/*============================================================================*/
static void qTypeGeneric_SortStackPush( qTypeGeneric_SortStackNode_t **top,
                                        uint8_t *low,
                                        uint8_t *high )
{
    top[ 0 ]->lo = low;
    top[ 0 ]->hi = high;
    ++top[ 0 ];
}
/*============================================================================*/
void qTypeGeneric_Sort( void * const pbase,
                        size_t n,
                        size_t size,
                        qTypeGeneric_CompareFcn_t cmp,
                        void *arg )
{
    /*cstat -MISRAC2012-Rule-18.4*/
    if ( ( NULL != pbase ) && ( size > 0U ) && ( n > 0U ) && ( NULL != cmp ) ) {
        const size_t max_thresh = 4U*size;
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        uint8_t *base_ptr = (uint8_t *)pbase;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        uint8_t * const end_ptr = &base_ptr[ size*( n - 1U ) ];
        uint8_t *tmp_ptr = base_ptr, *run_ptr;
        uint8_t *thresh = ( end_ptr < ( base_ptr + max_thresh) ) ? end_ptr : ( base_ptr + max_thresh ) ;

        if ( n > 4u ) {
            uint8_t *lo = base_ptr, *hi = &lo[ size*( n - 1U ) ];
            qTypeGeneric_SortStackNode_t stack[ 8U*sizeof(size_t) ], *top = stack;

            qTypeGeneric_SortStackPush( &top, NULL, NULL );
            while ( stack < top ) {
                uint8_t *left_ptr, *right_ptr;
                /*cstat -ATH-div-0-unchk-param -CERT-INT33-C_h*/
                /*cppcheck-suppress misra-c2012-10.8 */
                uint8_t *mid = &lo[ size*( ( (size_t)( hi - lo )/size ) >> 1U ) ];
                /*cstat +ATH-div-0-unchk-param +CERT-INT33-C_h*/
                if ( cmp( mid, lo, arg ) < 0 ) {
                    qTypeGeneric_Swap( mid, lo, size );
                }
                if ( cmp( hi, mid, arg ) < 0 ) {
                    qTypeGeneric_Swap( mid, hi, size );
                }
                else {
                    /*cppcheck-suppress misra-c2012-15.1 */
                    goto jump_over; /*MISRAC deviation allowed*/
                }
                if ( cmp( mid, lo, arg ) < 0 ) {
                    qTypeGeneric_Swap( mid, lo, size );
                }

                jump_over:
                left_ptr  = lo + size;
                right_ptr = hi - size;
                do {
                    while ( cmp( left_ptr, mid, arg ) < 0 ) {
                        /*cppcheck-suppress misra-c2012-10.3 */
                        left_ptr += size;
                    }
                    while ( cmp( mid, right_ptr, arg ) < 0 ) {
                        /*cppcheck-suppress misra-c2012-10.3 */
                        right_ptr -= size;
                    }

                    if ( left_ptr < right_ptr ) {
                        qTypeGeneric_Swap( left_ptr, right_ptr, size );
                        if ( mid == left_ptr ) {
                            mid = right_ptr;
                        }
                        else if ( mid == right_ptr ) {
                            mid = left_ptr;
                        }
                        else {
                            /*nothing to do here*/
                        }
                        /*cppcheck-suppress misra-c2012-10.3 */
                        left_ptr += size;
                        /*cppcheck-suppress misra-c2012-10.3 */
                        right_ptr -= size;
                    }
                    else if ( left_ptr == right_ptr ) {
                        /*cppcheck-suppress misra-c2012-10.3 */
                        left_ptr += size;
                        /*cppcheck-suppress misra-c2012-10.3 */
                        right_ptr -= size;
                        break;
                    }
                    else {
                        /*nothing to do here*/
                    }
                } while (left_ptr <= right_ptr);

                /*cppcheck-suppress misra-c2012-10.8 */
                if ( (size_t)( right_ptr - lo ) <= max_thresh ) {
                    /*cppcheck-suppress misra-c2012-10.8 */
                    if ( (size_t)( hi - left_ptr ) <= max_thresh ) {
                        --top; /*POP form the stack*/
                        lo = top->lo;
                        hi = top->hi;
                    }
                    else {
                        lo = left_ptr;
                    }
                }
                /*cppcheck-suppress misra-c2012-10.8 */
                else if ( (size_t)( hi - left_ptr ) <= max_thresh ) {
                    hi = right_ptr;
                }
                else if ( ( right_ptr - lo ) > ( hi - left_ptr ) ) {
                    qTypeGeneric_SortStackPush( &top, lo, right_ptr );
                    lo = left_ptr;
                }
                else {
                    qTypeGeneric_SortStackPush( &top, left_ptr, hi );
                    hi = right_ptr;
                }
            }
        }
        /*cppcheck-suppress misra-c2012-10.3 */
        for ( run_ptr = tmp_ptr + size ; run_ptr <= thresh ; run_ptr += size ) {
            if ( cmp( run_ptr, tmp_ptr, arg ) < 0 ) {
                tmp_ptr = run_ptr;
            }
        }

        if ( tmp_ptr != base_ptr ) {
            qTypeGeneric_Swap( tmp_ptr, base_ptr, size );
        }
        run_ptr = base_ptr + size;
        /*cppcheck-suppress misra-c2012-10.3 */
        while ( (run_ptr += size ) <= end_ptr ) {
            tmp_ptr = run_ptr - size;
            while ( cmp( run_ptr, tmp_ptr, arg ) < 0 ) {
                /*cppcheck-suppress misra-c2012-10.3 */
                tmp_ptr -= size;
            }
            /*cppcheck-suppress misra-c2012-10.3 */
            tmp_ptr += size;
            if ( tmp_ptr != run_ptr ) {
                uint8_t *tra = run_ptr + size;
                while ( --tra >= run_ptr ) {
                    uint8_t c = *tra;
                    uint8_t *hi = tra;
                    uint8_t *lo = tra;
                    /*cppcheck-suppress misra-c2012-10.3 */
                    while ( (lo -= size) >= tmp_ptr ) {
                        *hi = *lo;
                        hi = lo;
                    }
                    *hi = c;
                }
            }
        }
    }
    /*cstat +MISRAC2012-Rule-18.4*/
}
/*============================================================================*/
void qTypeGeneric_Reverse( void * const pbase,
                           const size_t size,
                           const size_t init,
                           const size_t end )
{
    if ( ( NULL != pbase ) && ( size > 0U ) && ( end > init ) ) {
        size_t s = size*init;
        size_t e = size*end;
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        uint8_t *v = (uint8_t*)pbase;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        while( s < e ) {
            qTypeGeneric_Swap( &v[ s ], &v[ e ], size );
            s += size;
            e -= size;
        }
    }
}
/*============================================================================*/
void qTypeGeneric_Rotate( void * const pbase,
                          const size_t size,
                          const size_t n,
                          const int k )
{
    if ( ( NULL != pbase ) && ( 0 != k ) && ( n > 0U ) ) {
        size_t r;

        if ( k > 0 ) {
            r = (size_t)k;
            r %= n;
            qTypeGeneric_Reverse( pbase, size, n - r, n - 1U );
            qTypeGeneric_Reverse( pbase, size, 0U, n - r - 1U );
            qTypeGeneric_Reverse( pbase, size, 0U, n - 1U );
        }
        else {
            r = (size_t)(-k);
            r %= n;
            qTypeGeneric_Reverse( pbase, size, 0U, r - 1U );
            qTypeGeneric_Reverse( pbase, size, r, n - 1U );
            qTypeGeneric_Reverse( pbase, size, 0U, n - 1U );
        }
    }
}
/*============================================================================*/
void* qTypeGeneric_Set( void * const pbase,
                        const size_t size,
                        const size_t n,
                        const void * const ref )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
    /*cppcheck-suppress misra-c2012-11.5 */
    uint8_t *p = (uint8_t*)pbase;
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
    void *retVal = NULL;

    if ( ( NULL != pbase ) && ( size > 0U ) && ( n > 0U ) && ( NULL != ref ) ) {
        size_t i;

        for ( i = 0U ; i < n ; i++ ) {
            retVal = memcpy( &p[ size*i ], ref, size );
        }
    }

    return retVal;
}
/*============================================================================*/
void* qTypeGeneric_LSearch( const void *key,
                            const void *pbase,
                            const size_t n,
                            const size_t size,
                            qTypeGeneric_CompareFcn_t compar,
                            void *arg )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
    /*cppcheck-suppress misra-c2012-11.5 */
    const uint8_t *pb = (const uint8_t *)pbase;
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
    void* retVal = NULL;
    size_t i;

    for ( i = 0U ; i < n ; ++i ) {
        const uint8_t *element = &pb[ i*size ];

        if ( 0 == compar( key, element, arg) ) {
            /*cstat -MISRAC2012-Rule-11.8*/
            /*cppcheck-suppress [ misra-c2012-11.8, cert-EXP05-C ]*/
            retVal = (void*)element;
            /*cstat +MISRAC2012-Rule-11.8*/
            break;
        }
    }
    return retVal;
}
/*============================================================================*/
void* qTypeGeneric_BSearch( const void *key,
                            const void *pbase,
                            const size_t n,
                            const size_t size,
                            qTypeGeneric_CompareFcn_t compar,
                            void *arg )
{
    /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
    /*cppcheck-suppress misra-c2012-11.5 */
    const uint8_t *base = (const uint8_t *)pbase;
    /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
    size_t lim  = n;
    const uint8_t *p;
    void *retVal = NULL;

    while ( 0U != lim ) {
        int cmp;

        p = &base[ ( lim >> 1U )*size ];
        cmp = compar( key, p, arg );
        if ( 0 == cmp ) {
            /*cstat -MISRAC2012-Rule-11.8*/
            /*cppcheck-suppress [ misra-c2012-11.8, cert-EXP05-C ]*/
            retVal = (void *)p;
            /*cstat +MISRAC2012-Rule-11.8*/
            break;
        }
        else if ( cmp > 0 ) {
            base = &p[ size ];
            lim--;
        }
        else {
            /*nothing to do here*/
        }
        lim >>= 1U;
    }
    return retVal;
}
/*============================================================================*/
int qTypeGeneric_ForEach( void *pbase,
                          const size_t size,
                          const size_t n,
                          qTypeGeneric_ForEachFcn_t f,
                          const bool dir,
                          void *arg )
{
    int retVal = 0;

    if ( ( NULL != pbase ) && ( NULL != f ) && ( n > 0U ) ) {
        /*cstat -MISRAC2012-Rule-11.5 -CERT-EXP36-C_b*/
        /*cppcheck-suppress misra-c2012-11.5 */
        uint8_t *pb = (uint8_t *)pbase;
        /*cstat +MISRAC2012-Rule-11.5 +CERT-EXP36-C_b*/
        if ( 1 != f( -1, NULL, arg ) ) {
            size_t i;
            uint8_t *element;

            if ( false == dir ) {
                for ( i = 0U ; i < n ; ++i ) {
                    element = &pb[ i*size ];
                    retVal = f( (int)i , element, arg );
                    if ( 1 == retVal ) {
                        break;
                    }
                }
            }
            else {
                i = n;
                while ( i-- > 0U ) {
                    element = &pb[ i*size ];
                    retVal = f( (int)i, element, arg );
                    if ( 1 == retVal ) {
                        break;
                    }
                }
            }
        }
        (void)f( (int)n, NULL, arg );
    }

    return retVal;
}
/*============================================================================*/