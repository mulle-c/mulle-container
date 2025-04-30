//
//  mulle-pointerarray.h
//  mulle-container
//
//  Created by Nat! on 10.03.15.
//  Copyright (c) 2015 Mulle kybernetiK. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
#ifndef mulle__pointerarray_h__
#define mulle__pointerarray_h__

#include "include.h"
#include "mulle-container-operation.h"
#include "mulle-qsort.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


struct mulle_pointers
{
   void     **pointers;
   size_t   count;
};


static inline struct mulle_pointers   mulle_pointers_make( void **pointers,
                                                           size_t count)
{
   return( (struct mulle_pointers) { .pointers = pointers, .count = count });
}


// just frees the struct, does not free individual pointers
// is done the right verb ?
static inline void   mulle_pointers_done( struct mulle_pointers p,
                                          struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, p.pointers);
}


//
// mulle__pointerarray is a growing array of pointers, it just reallocs.
// You can remove the last item via "pop", in general you would eschew
// random access removal for a copy operation. But there is now
// actually a remove operation:
//   mulle__pointerarray_remove_in_range, but it's very slow.
//
#define _MULLE__POINTERARRAY_BASE    \
    void      **_storage;            \
    void      **_curr;               \
    void      **_sentinel;           \
    void      **_initial_storage


#ifndef MULLE__CONTAINER_MISER_MODE

#define MULLE__POINTERARRAY_BASE          \
    _MULLE__POINTERARRAY_BASE;            \
    uintptr_t  _n_mutations

#else

#define MULLE__POINTERARRAY_BASE          \
    _MULLE__POINTERARRAY_BASE

#endif



struct mulle__pointerarray
{
   MULLE__POINTERARRAY_BASE;
};


#define MULLE__POINTERARRAY_DATA( storage, count) \
   ((struct mulle__pointerarray)                  \
   {                                              \
      ._storage         = (storage),              \
      ._curr            = (storage),              \
      ._sentinel        = &(storage)[ count],     \
      ._initial_storage = (storage)               \
   })


static inline struct mulle__pointerarray  *
   mulle__pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__pointerarray));
   return( array);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarray_init_with_static_pointers( struct mulle__pointerarray *array,
                                                   void **storage,
                                                   size_t capacity)
{
   assert( storage);
   assert( capacity);

   array->_storage         =
   array->_initial_storage = storage;
   array->_curr            = array->_storage;
   array->_sentinel        = &array->_curr[ capacity];
}


// if capacity is zero, this is just a memset 0. So the init can be left out
// for already zeroed memory
MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerarray_init( struct mulle__pointerarray *array,
                                                size_t capacity,
                                                struct mulle_allocator *allocator)
{
   memset( array, 0, sizeof( *array));

   if( capacity)
   {
      array->_storage         = mulle_allocator_malloc( allocator,
                                                        capacity * sizeof( void *));
      array->_curr            = array->_storage;
      array->_sentinel        = &array->_curr[ capacity];
   }
}


MULLE_C_NONNULL_FIRST
static inline void  _mulle__pointerarray_done( struct mulle__pointerarray *array,
                                               struct mulle_allocator *allocator)
{
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif

   if( array->_storage != array->_initial_storage)
      mulle_allocator_free( allocator, array->_storage);
#ifdef DEBUG
   mulle_memset_uint32( array, 0xDEADDEAD, sizeof( struct mulle__pointerarray));
#endif
}


static inline void  mulle__pointerarray_done( struct mulle__pointerarray *array,
                                              struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerarray_done( array, allocator);
}

static inline struct mulle__pointerarray *
   mulle__pointerarray_create( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( *array));
   _mulle__pointerarray_init( array, 0, allocator);
   return( array);
}


static inline void  mulle__pointerarray_destroy( struct mulle__pointerarray *array,
                                                 struct mulle_allocator *allocator)
{
   if( array)
   {
      _mulle__pointerarray_done( array, allocator);
      mulle_allocator_free( allocator, array);
   }
}


//
// this will suck the victimless itemless, but keeps the original contents
// of array.
// TODO: rename to eviscarate ?
//
void   _mulle__pointerarray_absorb_array( struct mulle__pointerarray *array,
                                          struct mulle_allocator *allocator,
                                          struct mulle__pointerarray *victim,
                                          struct mulle_allocator *victim_allocator);


static inline void   mulle__pointerarray_absorb_array( struct mulle__pointerarray *array,
                                                       struct mulle_allocator *allocator,
                                                       struct mulle__pointerarray *victim,
                                                       struct mulle_allocator *victim_allocator)
{
   if( array && victim)
      _mulle__pointerarray_absorb_array( array, allocator, victim, victim_allocator);
}


# pragma mark - petty accessors


// there can be no useful "lenient" variant (used by MulleJS)
MULLE_C_NONNULL_FIRST
static inline void **  // deprecated name
   _mulle__pointerarray_get_pointers( struct mulle__pointerarray *array)
{
   return( array->_storage);
}


MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__pointerarray_get_storage( struct mulle__pointerarray *array)
{
   return( array->_storage);
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__pointerarray_get_size( struct mulle__pointerarray *array)
{
   return( (size_t) (array->_sentinel - array->_storage));
}


static inline size_t
   mulle__pointerarray_get_size( struct mulle__pointerarray *array)
{
   return( array ? _mulle__pointerarray_get_size( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( (size_t) (array->_curr - array->_storage));
}


static inline size_t
   mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( array ? _mulle__pointerarray_get_count( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle__pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
{
   return( (size_t) (array->_sentinel - array->_curr));
}


static inline size_t
   mulle__pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
{
   return( array ? (size_t) (array->_sentinel - array->_curr) : 0);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerarray_is_full( struct mulle__pointerarray *array)
{
   return( array->_curr == array->_sentinel);
}


static inline int
   mulle__pointerarray_is_full( struct mulle__pointerarray *array)
{
   return( array ? _mulle__pointerarray_is_full( array) : 1);
}


# pragma mark - memory operations

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void **  _mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                         size_t length,
                                         struct mulle_allocator *allocator);

static inline void **
   mulle__pointerarray_guarantee( struct mulle__pointerarray *array,
                                  size_t length,
                                  struct mulle_allocator *allocator)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_guarantee( array, length, allocator));
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator);

static inline
void   mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                 struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerarray_grow( array, allocator);
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                     void *notakey);


static inline void
   mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                void *notakey)
{
   if( array)
      _mulle__pointerarray_compact( array, notakey);
}


// there can be no useful "lenient" variant (used by MulleJS)
MULLE_C_NONNULL_FIRST
static inline void **
   _mulle__pointerarray_advance( struct mulle__pointerarray *array,
                                 size_t length,
                                 struct mulle_allocator *allocator)
{
   void   **reserved;

   _mulle__pointerarray_guarantee( array, length, allocator);

   reserved      = array->_curr;
   array->_curr += length;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
   return( reserved);
}


# pragma mark - operations

// if you guarantee that there is enough space for the add beforehand
// you can use this faster version
MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerarray_add_guaranteed( struct mulle__pointerarray *array,
                                            void *pointer)
{
   assert( array->_curr < array->_sentinel);
   *array->_curr++ = pointer;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarray_add( struct mulle__pointerarray *array,
                             void *pointer,
                             struct mulle_allocator *allocator)
{
   if( array->_curr == array->_sentinel)
      _mulle__pointerarray_grow( array, allocator);

   *array->_curr++ = pointer;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}


static inline void
   mulle__pointerarray_add( struct mulle__pointerarray *array,
                            void  *pointer,
                            struct mulle_allocator *allocator)
{
   if( array)
      return( _mulle__pointerarray_add( array, pointer, allocator));
}


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_add_pointerarray( struct mulle__pointerarray *array,
                                              struct mulle__pointerarray *other,
                                              struct mulle_range range,
                                              struct mulle_allocator *allocator);


static inline void
   mulle__pointerarray_add_pointerarray( struct mulle__pointerarray *array,
                                         struct mulle__pointerarray *other,
                                         struct mulle_range range,
                                         struct mulle_allocator *allocator)
{
   if( array)
      return( _mulle__pointerarray_add_pointerarray( array, other, range, allocator));
}


MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__pointerarray_pop( struct mulle__pointerarray *array)
{
   if( array->_curr > array->_storage)
   {
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
      array->_n_mutations++;
#endif
      return( *--array->_curr);
   }
   return( NULL);
}


static inline void   *
   mulle__pointerarray_pop( struct mulle__pointerarray *array)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_pop( array));
}




MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__pointerarray_get( struct mulle__pointerarray *array, size_t i)
{
   assert( array);
   assert( &array->_storage[ i] >= array->_storage);
   assert( &array->_storage[ i] < array->_curr);

   return( array->_storage[ i]);
}


MULLE_C_NONNULL_FIRST
static inline void   **
   _mulle__pointerarray_get_address( struct mulle__pointerarray *array, size_t i)
{
   assert( array);
   assert( &array->_storage[ i] >= array->_storage);
   assert( &array->_storage[ i] < array->_curr);

// do we count this as a mutation ? No
   return( &array->_storage[ i]);
}


/*
 *  Loop over all items. This works as long as you don't remove
 *  anything from the array. It will not retrieve newly added elements.
 *
 *  size_t        i, n;
 *  size_t        *item;
 *
 *  for( i = 0, n = mulle__pointerarray_get_count( array); i < n; i++)
 *  {
 *     item = mulle__pointerarray_get( array, i);
 *     printf( "%s\n", (char *) item);
 *  }
 *
 *  Reverse loop over all items. This works as long as you don't remove
 *  anything but the last element from the array. It will not retrieve newly 
 *  added elements.
 *
 *  size_t        i;
 *  size_t        *item;
 *
 *  for( i = mulle__pointerarray_get_count( array); i;)
 *  {
 *     item = mulle__pointerarray_get( array, --i);
 *     printf( "%s\n", (char *) item);
 *  }
 *
 */
static inline void   *
   mulle__pointerarray_get( struct mulle__pointerarray *array,
                            size_t i)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_get( array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
size_t
   _mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                      struct mulle_range range,
                                      void *buf);

static inline size_t
   mulle__pointerarray_get_in_range( struct mulle__pointerarray *array,
                                     struct mulle_range range,
                                     void *buf)
{
   if( ! array)
      return( 0);
   return( _mulle__pointerarray_get_in_range( array, range, buf));
}

//
// return value is the index of a compacted array, but it doesn't compact
// yet
//
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t   _mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                                void *p,
                                                struct mulle_range range);

static inline uintptr_t
   mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                      void *p,
                                      struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerarray_find_in_range( array, p, range));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__pointerarray_find( struct mulle__pointerarray *array,
                              void *p)
{
   return( _mulle__pointerarray_find_in_range( array, p, mulle_range_make_all()));
}


static inline uintptr_t
   mulle__pointerarray_find( struct mulle__pointerarray *array,
                             void *p)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerarray_find( array, p));
}

// removal


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   array->_curr = array->_storage;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}


static inline void
   mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   if( array)
      _mulle__pointerarray_reset( array);
}


// other may be NULL
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
int    _mulle__pointerarray_is_equal( struct mulle__pointerarray *array,
                                      struct mulle__pointerarray *other);


MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                             struct mulle_range range);

static inline
void
   mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                        struct mulle_range range)
{
   if( array)
      _mulle__pointerarray_remove_in_range( array, range);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerarray_remove( struct mulle__pointerarray *array,
                                    void *p)
{
   size_t  i;
   void    *item;

   //
   // Removal back to front is cool, as long as we are the only one
   // modifying the array. We don't use an enumerator here, but an index is
   // safe...
   //
   for( i = _mulle__pointerarray_get_count( array); i;)
   {
      item = _mulle__pointerarray_get( array, --i);
      if( p == item)
         _mulle__pointerarray_remove_in_range( array,
                                               mulle_range_make( i, 1));
   }
}

static inline
void
   mulle__pointerarray_remove( struct mulle__pointerarray *array,
                               void *p)
{
   if( array)
      _mulle__pointerarray_remove( array, p);
}


MULLE_C_NONNULL_FIRST
static inline
void   *_mulle__pointerarray_remove_unique( struct mulle__pointerarray *array,
                                            void *p)
{
   size_t  i;
   void    *item;

   for( i = _mulle__pointerarray_get_count( array); i;)
   {
      item = _mulle__pointerarray_get( array, --i);
      if( p == item)
      {
         _mulle__pointerarray_remove_in_range( array,
                                               mulle_range_make( i, 1));
         return( item);
      }
   }
   return( NULL);
}

static inline
void *
   mulle__pointerarray_remove_unique( struct mulle__pointerarray *array,
                               void *p)
{
   if( array)
      return( _mulle__pointerarray_remove_unique( array, p));
   return( NULL);
}


//


typedef int   mulle_pointerarray_cmp_t( void **, void **, void *);

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarray_qsort_r_inline( struct mulle__pointerarray *array,
                                        mulle_pointerarray_cmp_t *compare,
                                        void *userinfo)
{
   _mulle_qsort_r_inline( array->_storage,
                          _mulle__pointerarray_get_count( array),
                          sizeof( void *),
                          (mulle_qsort_cmp_t *) compare,
                          userinfo);
}


static inline void
   mulle__pointerarray_qsort_r_inline( struct mulle__pointerarray *array,
                                       mulle_pointerarray_cmp_t *compare,
                                       void *userinfo)
{
   if( array)
      _mulle__pointerarray_qsort_r_inline( array, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarray_qsort_r( struct mulle__pointerarray *array,
                                 mulle_pointerarray_cmp_t *compare,
                                 void *userinfo)
{
   mulle_qsort_r( array->_storage,
                  _mulle__pointerarray_get_count( array),
                  sizeof( void *),
                  (mulle_qsort_cmp_t *) compare,
                  userinfo);
}


static inline void
   mulle__pointerarray_qsort_r( struct mulle__pointerarray *array,
                                       mulle_pointerarray_cmp_t *compare,
                                       void *userinfo)
{
   if( array)
      _mulle__pointerarray_qsort_r( array, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void  *
   _mulle__pointerarray_get_last( struct mulle__pointerarray *array)
{
   if( array->_curr == array->_storage)
      return( NULL);
   return( array->_curr[ -1]);
}


static inline void  *
   mulle__pointerarray_get_last( struct mulle__pointerarray *array)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_get_last( array));
}



MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle__pointerarray_set( struct mulle__pointerarray *array,
                             size_t i,
                             void *p)
{
   void   *old;

   assert( array);
   assert( &array->_storage[ i] < array->_curr);

   old = array->_storage[ i];
   array->_storage[ i] = p;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
   return( old);
}

static inline void   *
   mulle__pointerarray_set( struct mulle__pointerarray *array,
                            size_t i,
                            void *p)
{
   if( ! array || &array->_storage[ i] >= array->_curr)
      return( NULL);
   return( _mulle__pointerarray_set( array, i, p));
}


#pragma mark - random access set/get


//
// you can random/access with "set" here, if the pointerarray is too small
// it's grown and the new space is zeroed
//
MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerarray_set_zeroing( struct mulle__pointerarray *array,
                                                       size_t i,
                                                       void *value,
                                                       struct mulle_allocator *allocator)
{
   void     **space;
   size_t   n;
   size_t   zeroes;

   n = _mulle__pointerarray_get_count( array);
   if( i < n)
   {
      _mulle__pointerarray_set( array, i, value);
      return;
   }

   zeroes = i - n;
   space  = _mulle__pointerarray_advance( array, zeroes + 1, allocator);
   memset( space, 0, zeroes * sizeof( void *));
   space[ zeroes] = value;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   array->_n_mutations++;
#endif
}


static inline void   mulle__pointerarray_set_zeroing( struct mulle__pointerarray *array,
                                                      size_t       i,
                                                      void *value,
                                                      struct mulle_allocator *allocator)
{
   if( ! array)
      return;
   _mulle__pointerarray_set_zeroing( array, i, value, allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle__pointerarray_get_zeroing( struct mulle__pointerarray *array,
                                                        size_t i)
{
   size_t   n;

   n = _mulle__pointerarray_get_count( array);
   if( i < n)
      return( _mulle__pointerarray_get( array, i));
   return( NULL);
}


static inline void   *mulle__pointerarray_get_zeroing( struct mulle__pointerarray *array,
                                                       size_t i)

{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_get_zeroing( array, i));
}


// makes no sense if array is nil
MULLE_C_NONNULL_FIRST
static inline void
   **_mulle__pointerarray_get_zeroing_address( struct mulle__pointerarray *array,
                                               size_t i,
                                               struct mulle_allocator *allocator)
{
   void     **space;
   size_t   n;
   size_t   zeroes;

   n = _mulle__pointerarray_get_count( array);
   if( i >= n)
   {
      zeroes = i - n + 1;
      space  = _mulle__pointerarray_advance( array, zeroes, allocator);
      memset( space, 0, zeroes * sizeof( void *));
   }
   return( _mulle__pointerarray_get_address( array, i));
}



MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_pointers
   _mulle__pointerarray_extract_pointers( struct mulle__pointerarray *buffer,
                                          struct mulle_allocator *allocator);


#pragma mark - enumeration


/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop. 
 *
 *  struct mulle__pointerarrayenumerator   rover;
 *  void                                   *item;
 *
 *  rover = mulle__pointerarray_enumerate( array);
 *  while( mulle__pointerarrayenumerator_next( &rover, &item))
 *     printf( "%s\n", (char *) item);
 *  mulle__pointerarrayenumerator_done( &rover);
 *
 */

#define _MULLE__POINTERARRAYENUMERATOR_BASE   \
   void   **_curr;                            \
   void   **_sentinel

//
// could also use MULLE__CONTAINER_MISER_MODE, but enumerators are not to
// be stored over the length of a function call
//
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT

#define MULLE__POINTERARRAYENUMERATOR_BASE    \
    _MULLE__POINTERARRAYENUMERATOR_BASE;      \
    struct mulle__pointerarray *_array;       \
    uintptr_t  _n_mutations
#else

#define MULLE__POINTERARRAYENUMERATOR_BASE    \
    _MULLE__POINTERARRAYENUMERATOR_BASE

#endif


struct mulle__pointerarrayenumerator
{
   MULLE__POINTERARRAYENUMERATOR_BASE;
};


#define mulle__pointerarrayenumerator_empty  \
   ((struct mulle__pointerarrayenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct mulle__pointerarrayenumerator
   _mulle__pointerarray_enumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   assert( array);

   rover._curr     = array->_storage;
   rover._sentinel = array->_curr;

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   rover._n_mutations = array->_n_mutations;
   rover._array       = array;
#endif
   assert( rover._sentinel >= rover._curr);

   return( rover);
}


static inline struct mulle__pointerarrayenumerator
   mulle__pointerarray_enumerate( struct mulle__pointerarray *array)
{
   if( ! array)
      return( mulle__pointerarrayenumerator_empty);

   return( _mulle__pointerarray_enumerate( array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover,
                                        void **item)
{
   if( rover->_curr < rover->_sentinel)
   {
      // assert here coz rover->_array could be NULL
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
      assert( rover->_array->_n_mutations == rover->_n_mutations && "array was modified during enumeration");
#endif
      *item = *rover->_curr++;
      return( 1);
   }
   *item = NULL;
   return( 0);
}


static inline int
   mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover,
                                       void **item)
{
   void   *dummy;

   if( ! item)
      item = &dummy;
   if( rover)
      return( _mulle__pointerarrayenumerator_next( rover, item));
   *item = NULL;
   return( 0);
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


#pragma mark - reverse enumeration


/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop. 
 *
 *  struct mulle__pointerarrayreverseenumerator   rover;
 *  void                                          *item;
 *
 *  rover = mulle__pointerarray_reverseenumerate( array);
 *  while( mulle__pointerarrayreverseenumerator_next( &rover, &item))
 *     printf( "%s\n", (char *) item);
 *  mulle__pointerarrayreverseenumerator_done( &rover);
 *
 */
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT

#define MULLE__POINTERARRAYREVERSEENUMERATOR_BASE    \
    _MULLE__POINTERARRAYENUMERATOR_BASE;             \
    struct mulle__pointerarray *_array;              \
    uintptr_t  _n_mutations

#else

#define MULLE__POINTERARRAYREVERSEENUMERATOR_BASE    \
    _MULLE__POINTERARRAYENUMERATOR_BASE

#endif


struct mulle__pointerarrayreverseenumerator
{
   MULLE__POINTERARRAYREVERSEENUMERATOR_BASE;
};


#define mulle__pointerarrayreverseenumerator_empty  \
   ((struct mulle__pointerarrayreverseenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct  mulle__pointerarrayreverseenumerator
   _mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayreverseenumerator   rover;

   assert( array);

   rover._curr     = array->_curr;
   rover._sentinel = array->_storage;
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   rover._n_mutations = array->_n_mutations;
   rover._array       = array;
#endif
   assert( rover._sentinel <= rover._curr);

   return( rover);
}


static inline struct mulle__pointerarrayreverseenumerator
   mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array)
{
   if( ! array)
      return( mulle__pointerarrayreverseenumerator_empty);

   return( _mulle__pointerarray_reverseenumerate( array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerarrayreverseenumerator_next( struct mulle__pointerarrayreverseenumerator *rover,
                                               void **item)
{
   if( rover->_curr == rover->_sentinel)
   {
      *item = NULL;
      return( 0);
   }

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   assert( rover->_array->_n_mutations == rover->_n_mutations && "array was modified during enumeration");
#endif

   *item = *--rover->_curr;
   return( 1);
}


static inline int
   mulle__pointerarrayreverseenumerator_next( struct mulle__pointerarrayreverseenumerator *rover,
                                              void **item)
{
   void   *dummy;

   if( ! item)
       item = &dummy;
   if( rover)
      return( _mulle__pointerarrayreverseenumerator_next( rover, item));
   *item = NULL;
   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerarrayreverseenumerator_done( struct mulle__pointerarrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle__pointerarrayreverseenumerator_done( struct mulle__pointerarrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}



#pragma mark - enumerator convenience

static inline int   mulle__pointerarray_member( struct mulle__pointerarray *array,
                                                void *p)
{
   struct  mulle__pointerarrayenumerator   rover;
   void                                    *q;

   rover = _mulle__pointerarray_enumerate( array);
   {
      while( _mulle__pointerarrayenumerator_next( &rover, &q))
         if( q == p)
            break;
   }
   mulle__pointerarrayenumerator_done( &rover);

   return( q == p);
}


// created by make-container-do.sh --flexible mulle--pointerarray.c

#define mulle__pointerarray_do( name)                              \
   for( struct mulle__pointerarray                                 \
           name ## __container = { 0 },                            \
           *name = &name ## __container,                           \
           *name ## __i = NULL;                                    \
        ! name ## __i;                                             \
        name ## __i =                                              \
        (                                                          \
           _mulle__pointerarray_done( &name ## __container, NULL), \
           (void *) 0x1                                            \
        )                                                          \
      )                                                            \
      for( int  name ## __j = 0;    /* break protection */         \
           name ## __j < 1;                                        \
           name ## __j++)

#define mulle__pointerarray_do_flexible( name, stackcount)              \
   void   *name ## __storage[ stackcount];                              \
   for( struct mulle__pointerarray                                      \
           name ## __container =                                        \
              MULLE__POINTERARRAY_DATA( name ## __storage, stackcount), \
           *name = &name ## __container,                                \
           *name ## __i = NULL;                                         \
        ! name ## __i;                                                  \
        name ## __i =                                                   \
        (                                                               \
           _mulle__pointerarray_done( &name ## __container, NULL),      \
           (void *) 0x1                                                 \
        )                                                               \
      )                                                                 \
      for( int  name ## __j = 0;    /* break protection */              \
           name ## __j < 1;                                             \
           name ## __j++)


// created by make-container-for.sh src/array/pointer/mulle--pointerarray.c

#define mulle__pointerarray_for( name, item)                                               \
   assert( sizeof( item) == sizeof( void *));                                              \
   for( struct mulle__pointerarrayenumerator                                               \
           rover__ ## item = mulle__pointerarray_enumerate( name),                         \
           *rover___  ## item ## __i = (void *) 0;                                         \
        ! rover___  ## item ## __i;                                                        \
        rover___ ## item ## __i = (_mulle__pointerarrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                            \
      while( _mulle__pointerarrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/pointer/mulle--pointerarray.c

#define mulle__pointerarray_for_reverse( name, item)                                              \
   assert( sizeof( item) == sizeof( void *));                                                     \
   for( struct mulle__pointerarrayreverseenumerator                                               \
           rover__ ## item = mulle__pointerarray_reverseenumerate( name),                         \
           *rover___  ## item ## __i = (void *) 0;                                                \
        ! rover___  ## item ## __i;                                                               \
        rover___ ## item ## __i = (_mulle__pointerarrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                                   \
      while( _mulle__pointerarrayreverseenumerator_next( &rover__ ## item, (void **) &item))


//
// Experimental:
//
// Running a loop over an array, where people might add or remove stuff
// in the middle of the loop. regular for is fine if you just add stuff
//
//#define mulle__pointerarray_safefor( name, item)
//   assert( sizeof( item) == sizeof( void *));
//   mulle__pointerarray_do_flexible( name ## __tmp, 16)
//   {
//      void   *name ## __value;
//
//      mulle__pointerarray_for( name, name ## __value)
//      {
//         mulle__pointerarray_add( name ## __tmp, name ## __value);
//      }
//
//      mulle__pointerarray_for( name ## __tmp, item)
//
//
//#define mulle__pointerarray_safefor_end( name)
//   }

#endif
