/*!
 * @file qtypegeneric.h
 * @author J. Camilo Gomez C.
 * @version 1.01
 * @note This file is part of the qLibs distribution.
 * @brief Type-Generic utilities
 **/

#ifndef QTYPEGENERIC_H
#define QTYPEGENERIC_H

#ifdef __cplusplus
extern "C" {
#endif

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdbool.h>


    /** @addtogroup  qtypegeneric qTypeGeneric
    * @brief Type-generic utilities
    *  @{
    */

    /*! @cond  */
    typedef int (*qTypeGeneric_CompareFcn_t)( const void *, const void *, void * );
    /*! @endcond  */

    /**
    * @brief Swaps the data pointed by b @a x and @a y
    * @param[in,out] x Pointer to first element.
    * @param[in,out] x Pointer to the second element.
    * @param[in] size The size of the data to be swapped.
    * @return none.
    */
    void qTypeGeneric_Swap( void *x,
                            void *y,
                            size_t n );

    /**
    * @brief Sorts the given array pointed to by @a pbase in ascending order. 
    * The array contains @a n elements of @a size bytes. Function pointed to
    * by @a cmp is used for object comparison.
    * @remark This algorithm uses a non-recursive variant of the quicksort 
    * algorithm.
    * @param[in,out] pbase Pointer to the array to sort.
    * @param[in] n Number of elements in the array.
    * @param[in] size size of each element in the array in bytes
    * @param[in] cmp Comparison function which returns ​a negative integer value
    * if the first argument is less than the second, a positive integer value 
    * if the first argument is greater than the second and zero if the arguments
    * are equivalent.
    * The signature of the comparison function should be equivalent to the 
    * following:
    * @code{.c}
    *  int cmp( const void *a, const void *b, void *arg );
    * @endcode
    * The function must not modify the objects passed to it and must return 
    * consistent results when called for the same objects, regardless of their 
    * positions in the array.
    * @param[in] arg Additional information (e.g., collating sequence), passed
    * to @c cmp as the third argument
    * @return none.
    */
    void qTypeGeneric_Sort( void * const pbase,
                            size_t n,
                            size_t size,
                            qTypeGeneric_CompareFcn_t cmp,
                            void *arg );

    /**
    * @brief Reverse the given array pointed to by @a pbase.
    * Operation takes place on the portion of the array that starts at position 
    * @a init to position @a end.
    * @param[in,out] pbase Pointer to the array to reverse.
    * @param[in] size Size of each element in the array in bytes
    * @param[in] init Position of the first element.
    * @param[in] end Position of the last element.
    * @return none.
    */
    void qTypeGeneric_Reverse( void * const pbase,
                               const size_t size,
                               const size_t init,
                               const size_t end );

    /**
    * @brief Rotates @a k elements of the array pointed to by @a pbase. The array
    * contains @a n elements of @a size bytes. Rotation direction is determined
    * by the sign of @a k, the means a positive value performs a right-rotation
    * and a negative value a left-rotation.
    * @param[in,out] pbase Pointer to the array to reverse.
    * @param[in] size Size of each element in the array in bytes
    * @param[in] n Number of elements in the array.
    * @param[in] k Positions to rotate.
    * @return none.
    */
    void qTypeGeneric_Rotate( void * const pbase,
                              const size_t size,
                              const size_t n,
                              const int k );

    /**
    * @brief Set the data pointed by @a ref to every element of the array 
    * pointed by @a pbase. The array contains @a n elements of @a size bytes
    * @param[in,out] pbase Pointer to the array to reverse.
    * @param[in] size Size of each element in the array in bytes
    * @param[in] n Number of elements in the array.
    * @param[in] ref Pointer to the value to be set.
    * @return This function returns a pointer to the memory area @a pbase
    */
    void* qTypeGeneric_Set( void * const pbase,
                            const size_t size,
                            const size_t n,
                            const void * const ref );

    /**
    * @brief Performs a linear search over an array of @a n elements 
    * pointed to by @a pbase for a member that matches the object pointed to 
    * by @a key.
    * The size of each member is specified by @a size. The array contents
    * should be sorted in ascending order according to the @a compar
    * comparison function. This routine should take two arguments pointing to
    * the @a key and to an array member, in that order, and should return an
    * integer less than, equal to, or greater than zero if the @a key object
    * is respectively less than, matching, or greater than the array member.
    * @param[in] key This is the pointer to the object that serves as key for 
    * the search, type-casted as a void*.
    * @param[in] pbase This is the pointer to the first object of the array 
    * where the search is performed, type-casted as a void*.
    * @param[in] n This is the number of elements in the array pointed by base.
    * @param[in] size This is the size in bytes of each element in the array.
    * @param[in] cmp This is the function that compares two elements.
    * The signature of the comparison function should be equivalent to the 
    * following:
    * @code{.c}
    *  int cmp( const void *a, const void *b, void *arg );
    * @endcode.
    * Comparison function which returns ​a negative integer value if the first 
    * argument is less than the second, a positive integer value if the first 
    * argument is greater than the second and zero if the arguments are 
    * equivalent. @a key is passed as the first argument, an element from the 
    * array as the second.
    * The function must not modify the objects passed to it and must return 
    * consistent results when called for the same objects, regardless of their 
    * positions in the array.
    * @param[in] arg Additional information (e.g., collating sequence), passed
    * to @a compar as the third argument
    * @return This function returns a pointer to an entry in the array that 
    * matches the search key. If key is not found, a @c NULL pointer is returned.
    */
    void* qTypeGeneric_LSearch( const void *key,
                                const void *pbase,
                                const size_t n,
                                const size_t size,
                                qTypeGeneric_CompareFcn_t compar,
                                void *arg );

    /**
    * @brief Performs a binary search over an array of @a n elements 
    * pointed to by @a pbase for a member that matches the object pointed to 
    * by @a key.
    * The size of each member is specified by @a size. The array contents
    * should be sorted in ascending order according to the @a compar
    * comparison function. This routine should take two arguments pointing to
    * the @a key and to an array member, in that order, and should return an
    * integer less than, equal to, or greater than zero if the @a key object
    * is respectively less than, matching, or greater than the array member.
    * @param[in] key This is the pointer to the object that serves as key for 
    * the search, type-casted as a void*.
    * @param[in] pbase This is the pointer to the first object of the array 
    * where the search is performed, type-casted as a void*.
    * @param[in] n This is the number of elements in the array pointed by base.
    * @param[in] size This is the size in bytes of each element in the array.
    * @param[in] cmp This is the function that compares two elements.
    * The signature of the comparison function should be equivalent to the 
    * following:
    * @code{.c}
    *  int cmp( const void *a, const void *b, void *arg );
    * @endcode.
    * Comparison function which returns ​a negative integer value if the first 
    * argument is less than the second, a positive integer value if the first 
    * argument is greater than the second and zero if the arguments are 
    * equivalent. @a key is passed as the first argument, an element from the 
    * array as the second.
    * The function must not modify the objects passed to it and must return 
    * consistent results when called for the same objects, regardless of their 
    * positions in the array.
    * @param[in] arg Additional information (e.g., collating sequence), passed
    * to @a compar as the third argument
    * @return This function returns a pointer to an entry in the array that 
    * matches the search key. If key is not found, a @c NULL pointer is returned.
    */
    void* qTypeGeneric_BSearch( const void *key,
                                const void *pbase,
                                const size_t n,
                                const size_t size,
                                qTypeGeneric_CompareFcn_t compar,
                                void *arg );

    /** @}*/

#ifdef __cplusplus
}
#endif

#endif