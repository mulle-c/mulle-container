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
#ifndef mulle_pointerarray_h__
#define mulle_pointerarray_h__

#include "mulle--pointerarray.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle_pointerarray adds 'allocator' to the struct of
// mulle__pointerarray, otherwise its the same
//
#define MULLE_POINTERARRAY_BASE          \
   MULLE__POINTERARRAY_BASE;             \
   struct mulle_allocator   *allocator    // public...


struct mulle_pointerarray
{
   MULLE_POINTERARRAY_BASE;
};


#define MULLE_POINTERARRAY_DATA( storage, count, xallocator) \
   ((struct mulle_pointerarray)                              \
   {                                                         \
      ._storage         = (storage),                         \
      ._curr            = (storage),                         \
      ._sentinel        = &(storage)[ count],                \
      ._initial_storage = (storage),                         \
      .allocator        = (xallocator)                       \
   })


MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerarray_init( struct mulle_pointerarray *array,
                                               size_t capacity,
                                               struct mulle_allocator *allocator)
{
   _mulle__pointerarray_init( (struct mulle__pointerarray *) array,
                              capacity,
                              allocator);

   array->allocator = allocator;
}

#define _mulle_pointerarray_init_default( array)  \
   _mulle_pointerarray_init( array, 8, NULL)


static inline void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                              size_t capacity,
                                              struct mulle_allocator *allocator)
{
   if( array)
      _mulle_pointerarray_init( array, capacity, allocator);
}


#define mulle_pointerarray_init_default( array)  \
   mulle_pointerarray_init( array, 8, NULL)


static inline void
   mulle_pointerarray_init_with_static_pointer( struct mulle_pointerarray *array,
                                                void **pointers,
                                                size_t capacity,
                                                struct mulle_allocator *allocator)
{
   if( array)
   {
      _mulle__pointerarray_init_with_static_pointers( (struct mulle__pointerarray *) array,
                                                      pointers,
                                                      capacity);
      array->allocator = allocator;
   }
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerarray_done( struct mulle_pointerarray *array)
{
   _mulle__pointerarray_done( (struct mulle__pointerarray *) array,
                              array->allocator);
}


static inline void   mulle_pointerarray_done( struct mulle_pointerarray *array)
{
   if( array)
      _mulle__pointerarray_done( (struct mulle__pointerarray *) array,
                                 array->allocator);
}


static inline struct mulle_pointerarray  *
   mulle_pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_pointerarray));
   return( array);
}


static inline struct mulle_pointerarray *
   mulle_pointerarray_create( struct mulle_allocator *allocator)
{
   struct mulle_pointerarray   *array;

   array = mulle_pointerarray_alloc( allocator);
   _mulle_pointerarray_init( array, 8, allocator);
   return( array);
}

#define mulle_pointerarray_create_default()  \
   mulle_pointerarray_create( NULL)


static inline void   mulle_pointerarray_destroy( struct mulle_pointerarray *array)
{
   if( array)
      mulle__pointerarray_destroy( (struct mulle__pointerarray *) array,
                                    array->allocator);
}


static inline void  _mulle_pointerarray_absorb_array( struct mulle_pointerarray *array,
                                                     struct mulle_pointerarray *victim)
{
   _mulle__pointerarray_absorb_array( (struct mulle__pointerarray *) array,
                                      array->allocator,
                                      (struct mulle__pointerarray *) victim,
                                      victim->allocator);
}


// the victim is empty afterwards, owner ship of contents has transferred to
// array...

static inline void   mulle_pointerarray_absorb_array( struct mulle_pointerarray *array,
                                                      struct mulle_pointerarray *victim)
{
   if( array && victim)
      _mulle_pointerarray_absorb_array( array, victim);
}


# pragma mark - petty accessors


MULLE_C_NONNULL_FIRST
static inline void **
   _mulle_pointerarray_get_storage( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_storage( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_pointerarray_get_size( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle_pointerarray_get_size( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_get_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_pointerarray_get_count( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle_pointerarray_get_count( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_pointerarray_get_guaranteed_size( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


static inline size_t
	mulle_pointerarray_get_guaranteed_size( struct mulle__pointerarray *array)
{
   return( mulle__pointerarray_get_guaranteed_size( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_pointerarray_is_full( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


static inline int
   mulle_pointerarray_is_full( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_is_full( (struct mulle__pointerarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator *
	_mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
{
   return( array->allocator);
}


static inline struct mulle_allocator *
   mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
{
   return( array ? array->allocator : NULL);
}


# pragma mark - memory operations

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle_pointerarray_guarantee( struct mulle_pointerarray *array,
                                  size_t length)
{
   return( _mulle__pointerarray_guarantee( (struct mulle__pointerarray *) array,
                                           length,
                                           array->allocator));
}


static inline void **
   mulle_pointerarray_guarantee( struct mulle_pointerarray *array,
                                 size_t length)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_guarantee( (struct mulle__pointerarray *) array,
                                           length,
                                           array->allocator));
}


// there can be no useful "lenient" variant
MULLE_C_NONNULL_FIRST
static inline void **
   _mulle_pointerarray_advance( struct mulle_pointerarray *array,
                                size_t length)
{
   return( _mulle__pointerarray_advance( (struct mulle__pointerarray *) array,
                                         length,
                                         array->allocator));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_grow( struct mulle_pointerarray *array)
{
   _mulle__pointerarray_grow( (struct mulle__pointerarray *) array,
                              array->allocator);
}

static inline void
   mulle_pointerarray_grow( struct mulle_pointerarray *array)
{
   if( array)
      _mulle__pointerarray_grow( (struct mulle__pointerarray *) array,
                                 array->allocator);
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_compact( struct mulle_pointerarray *array,
                                void *notakey)
{
   _mulle__pointerarray_compact( (struct mulle__pointerarray *) array, notakey);
}


static inline void
   mulle_pointerarray_compact( struct mulle_pointerarray *array,
                               void *notakey)
{
   mulle__pointerarray_compact( (struct mulle__pointerarray *) array, notakey);
}



# pragma mark - operations

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_add_guaranteed( struct mulle_pointerarray *array,
                                       void *p)
{
   _mulle__pointerarray_add_guaranteed( (struct mulle__pointerarray *) array,
                                        p);
}


MULLE_C_NONNULL_FIRST
static inline void
	_mulle_pointerarray_add( struct mulle_pointerarray *array, void *p)
{
   _mulle__pointerarray_add( (struct mulle__pointerarray *) array,
                              p,
                              array->allocator);
}

static inline void
   mulle_pointerarray_add( struct mulle_pointerarray *array, void  *p)
{
   if( array)
      _mulle__pointerarray_add( (struct mulle__pointerarray *) array,
                                 p,
                                 array->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_add_pointerarray( struct mulle_pointerarray *array,
                                         struct mulle_pointerarray *other,
                                         struct mulle_range range)
{
   _mulle__pointerarray_add_pointerarray( (struct mulle__pointerarray *) array,
                                          (struct mulle__pointerarray *) other,
                                          range,
                                          array->allocator);
}

static inline void
   mulle_pointerarray_add_pointerarray( struct mulle_pointerarray *array,
                                        struct mulle_pointerarray *other,
                                        struct mulle_range range)
{
   if( array)
      _mulle_pointerarray_add_pointerarray( array, other, range);
}


/*
 *  Loop over all items. This works as long as you don't remove
 *  anything from the array. It will not retrieve newly added elements.
 *  You could loop over them if you don't cache the count in 'n'.
 *
 *  size_t  i, n;
 *  size_t  *item;
 *
 *  for( i = 0, n = mulle_pointerarray_get_count( array); i < n; i++)
 *  {
 *     item = mulle_pointerarray_get( array, i);
 *     printf( "%s\n", (char *) item);
 *  }
 *
 *  Reverse loop over all items. This works as long as you don't remove
 *  anything but the last element from the array. It will not retrieve newly 
 *  added elements.
 *
 *  size_t  i;
 *  size_t  *item;
 *
 *  for( i = mulle_pointerarray_get_count( array); i;)
 *  {
 *     item = mulle_pointerarray_get( array, --i);
 *     printf( "%s\n", (char *) item);
 *  }
 *
 */
MULLE_C_NONNULL_FIRST
static inline void  *
	_mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


static inline void  *
   mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
{
   return( mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline size_t
   _mulle_pointerarray_get_in_range( struct mulle_pointerarray *array,
                                     struct mulle_range range,
                                     void *buf)
{
   return( _mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                              range,
                                              buf));
}

static inline size_t
   mulle_pointerarray_get_in_range( struct mulle_pointerarray *array,
                                    struct mulle_range range,
                                    void *buf)
{
   return( mulle__pointerarray_get_in_range( (struct mulle__pointerarray *) array,
                                             range,
                                             buf));
}



MULLE_C_NONNULL_FIRST
static inline void  *
   _mulle_pointerarray_get_last( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_last( (struct mulle__pointerarray *) array));
}

static inline void  *
   mulle_pointerarray_get_last( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_get_last( (struct mulle__pointerarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t
	_mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
{
   return( _mulle__pointerarray_find( (struct mulle__pointerarray *) array,
                                      p));
}


static inline uintptr_t
   mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
{
   return( mulle__pointerarray_find( (struct mulle__pointerarray *) array, p));
}


// removal



MULLE_C_NONNULL_FIRST
static inline void  *
   _mulle_pointerarray_pop( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_pop( (struct mulle__pointerarray *) array));
}

static inline void *
   mulle_pointerarray_pop( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_pop( (struct mulle__pointerarray *) array));
}



MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerarray_remove_in_range( struct mulle_pointerarray *array,
                                            struct mulle_range range)
{
   _mulle__pointerarray_remove_in_range( (struct mulle__pointerarray *) array,
                                         range);
}


static inline
void   mulle_pointerarray_remove_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range)
{
   if( array)
      _mulle_pointerarray_remove_in_range( array, range);
}



MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerarray_remove_at_index( struct mulle_pointerarray *array,
                                            uintptr_t location)
{
   _mulle__pointerarray_remove_in_range( (struct mulle__pointerarray *) array,
                                         mulle_range_make( location, 1));
}


static inline
void   mulle_pointerarray_remove_at_index( struct mulle_pointerarray *array,
                                           uintptr_t location)
{
   if( array)
      _mulle__pointerarray_remove_in_range( (struct mulle__pointerarray *) array,
                                            mulle_range_make( location, 1));
}




MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerarray_remove( struct mulle_pointerarray *array,
                                   void *p)
{
   size_t  i;
   void    *item;

   //
   // Removal back to front is cool, as long as we are the only one
   // modifying the array. We don't use an enumerator here, but an index is
   // safe...
   //
   for( i = _mulle_pointerarray_get_count( array); i;)
   {
      item = _mulle_pointerarray_get( array, --i);
      if( p == item)
         _mulle_pointerarray_remove_in_range( array,
                                              mulle_range_make( i, 1));
   }
}


static inline
void   mulle_pointerarray_remove( struct mulle_pointerarray *array,
                                  void *p)
{
   if( array)
      _mulle_pointerarray_remove( array, p);
}


MULLE_C_NONNULL_FIRST
static inline
void   _mulle_pointerarray_insert_in_range( struct mulle_pointerarray *array,
                                            struct mulle_range range,
                                            void **pointers)
{
   _mulle__pointerarray_insert_in_range( (struct mulle__pointerarray *) array,
                                         range,
                                         pointers,
                                         array->allocator);
}


static inline
void   mulle_pointerarray_insert_in_range( struct mulle_pointerarray *array,
                                           struct mulle_range range,
                                           void **pointers)
{
   if( array)
      _mulle_pointerarray_insert_in_range( array, range, pointers);
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void
   _mulle_pointerarray_insert( struct mulle_pointerarray *array,
                               uintptr_t location,
                               void *p)
{
   _mulle__pointerarray_insert( (struct mulle__pointerarray *) array,
                                location,
                                p,
                                array->allocator);
}


MULLE_C_NONNULL_THIRD
static inline void
   mulle_pointerarray_insert( struct mulle_pointerarray *array,
                              uintptr_t location,
                              void *p)
{
   if( array)
      _mulle_pointerarray_insert( array, location, p);
}


MULLE_C_NONNULL_FIRST
static inline
void   * _mulle_pointerarray_remove_unique( struct mulle_pointerarray *array,
                                            void *p)
{
   return( _mulle__pointerarray_remove_unique( (struct mulle__pointerarray *) array,
                                               p));
}


//
// mulle_pointerarray_remove_unique returns 'p' if it was actually removed
// otherwise NULL. mulle_array can't do this, because it may have already
// released 'p'
//
static inline
void   *mulle_pointerarray_remove_unique( struct mulle_pointerarray *array,
                                          void *p)
{
   if( array)
      return( _mulle_pointerarray_remove_unique( array, p));
   return( NULL);
}



// sorting

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_qsort_r_inline( struct mulle_pointerarray *array,
                                       mulle_pointerarray_cmp_t *compare,
                                       void *userinfo)
{
   _mulle__pointerarray_qsort_r_inline( (struct mulle__pointerarray *) array,
                                         compare, 
                                         userinfo);
}


static inline void
   mulle_pointerarray_qsort_r_inline( struct mulle_pointerarray *array,
                                      mulle_pointerarray_cmp_t *compare,
                                      void *userinfo)
{
   if( array)
      _mulle_pointerarray_qsort_r_inline( array, compare, userinfo);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_qsort_r( struct mulle_pointerarray *array,
                                mulle_pointerarray_cmp_t *compare,
                                void *userinfo)
{
   _mulle__pointerarray_qsort_r( (struct mulle__pointerarray *) array,
                                 compare,
                                 userinfo);
}


static inline void
   mulle_pointerarray_qsort_r( struct mulle_pointerarray *array,
                               mulle_pointerarray_cmp_t *compare,
                               void *userinfo)
{
   if( array)
      _mulle_pointerarray_qsort_r( array, compare, userinfo);
}




MULLE_C_NONNULL_FIRST
static inline void   *_mulle_pointerarray_set( struct mulle_pointerarray *array,
                                               size_t i,
                                               void *p)
{
   return( _mulle__pointerarray_set( (struct mulle__pointerarray *) array,
                                     i,
                                     p));

}


static inline void   *mulle_pointerarray_set( struct mulle_pointerarray *array,
                                              size_t i,
                                              void *p)
{
   return( mulle__pointerarray_set( (struct mulle__pointerarray *) array,
                                     i,
                                     p));
}


#pragma mark - random access set/get

//
// you can random/access with "set" here, if the pointerarray is too small
// it's grown and the new space is zeroed
//
MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerarray_set_zeroing( struct mulle_pointerarray *array,
                                                      size_t i,
                                                      void *value)
{
   _mulle__pointerarray_set_zeroing( (struct mulle__pointerarray *) array,
                                     i,
                                     value,
                                     array->allocator);
}


static inline void   mulle_pointerarray_set_zeroing( struct mulle_pointerarray *array,
                                                     size_t i,
                                                     void *value)
{
   if( ! array)
      return;
   _mulle_pointerarray_set_zeroing( array, i, value);
}


MULLE_C_NONNULL_FIRST
static inline void   *_mulle_pointerarray_get_zeroing( struct mulle_pointerarray *array,
                                                       size_t i)
{
   return( _mulle__pointerarray_get_zeroing( (struct mulle__pointerarray *) array,
                                             i));
}


static inline void   *mulle_pointerarray_get_zeroing( struct mulle_pointerarray *array,
                                                      size_t i)
{
   if( ! array)
      return( NULL);
   return( _mulle_pointerarray_get_zeroing( array, i));
}


// makes no sense if array is nil
MULLE_C_NONNULL_FIRST
static inline void
   **_mulle_pointerarray_get_zeroing_address( struct mulle_pointerarray *array,
                                              size_t i)
{
   return( _mulle__pointerarray_get_zeroing_address( (struct mulle__pointerarray *) array,
                                                      i,
                                                      array->allocator));
}



#pragma mark - reset

MULLE_C_NONNULL_FIRST
static inline void
   _mulle_pointerarray_reset( struct mulle_pointerarray *array)
{
   _mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


static inline void
   mulle_pointerarray_reset( struct mulle_pointerarray *array)
{
   mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


// will use callbacks of array to determine equality
static inline int   mulle_pointerarray_is_equal( struct mulle_pointerarray *array,
                                                 struct mulle_pointerarray *other)
{
   if( ! array || ! other)
      return( array == other);

   return( _mulle__pointerarray_is_equal( (struct mulle__pointerarray *) array,
                                          (struct mulle__pointerarray *) other));
}


#pragma mark - enumeration

/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop. 
 *
 *  struct mulle_pointerarrayenumerator   rover;
 *  void                                  *item;
 *
 *  rover = mulle_pointerarray_enumerate( array);
 *  while( mulle_pointerarrayenumerator_next( &rover, &item))
 *     printf( "%s\n", (char *) item);
 *  mulle_pointerarrayenumerator_done( &rover);
 *
 */

#define MULLE_POINTERARRAYENUMERATOR_BASE MULLE__POINTERARRAYENUMERATOR_BASE

struct mulle_pointerarrayenumerator
{
   MULLE_POINTERARRAYENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerarrayenumerator
   _mulle_pointerarray_enumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;
   struct mulle__pointerarrayenumerator  tmp;

   tmp = _mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle_pointerarrayenumerator));
   return( rover);
}


static inline struct mulle_pointerarrayenumerator
   mulle_pointerarray_enumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;
   struct mulle__pointerarrayenumerator  tmp;

   tmp = mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle_pointerarrayenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline int
	_mulle_pointerarrayenumerator_next( struct mulle_pointerarrayenumerator *rover, 
                                      void **item)
{
   return( _mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover,
                                                item));
}

static inline int
	mulle_pointerarrayenumerator_next( struct mulle_pointerarrayenumerator *rover, 
                                     void **item)
{
   return( mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover,
                                                item));
}


MULLE_C_NONNULL_FIRST
static inline void
	_mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


#pragma mark - reverseenumeration

/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop.  
 *
 *  struct mulle_pointerarrayreverseenumerator   rover;
 *  void                                         *item;
 *
 *  rover = mulle_pointerarray_reverseenumerate( array);
 *  while( mulle_pointerarrayreverseenumerator_next( &rover, &item))
 *     printf( "%s", item);
 *  mulle_pointerarrayreverseenumerator_done( &rover);
 *  printf( "\n");
 *
 */
#define MULLE_POINTERARRAYREVERSEENUMERATOR_BASE  \
   MULLE__POINTERARRAYREVERSEENUMERATOR_BASE

struct mulle_pointerarrayreverseenumerator
{
   MULLE_POINTERARRAYREVERSEENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct  mulle_pointerarrayreverseenumerator
   _mulle_pointerarray_reverseenumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayreverseenumerator    rover;
   struct mulle__pointerarrayreverseenumerator   tmp;

   tmp = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__pointerarrayreverseenumerator));
   return( rover);
}


static inline struct  mulle_pointerarrayreverseenumerator
   mulle_pointerarray_reverseenumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayreverseenumerator    rover;
   struct mulle__pointerarrayreverseenumerator   tmp;

   tmp = mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__pointerarrayreverseenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST
static inline int
	_mulle_pointerarrayreverseenumerator_next( struct mulle_pointerarrayreverseenumerator *rover,
                                              void **item)
{
   return( _mulle__pointerarrayreverseenumerator_next( (struct mulle__pointerarrayreverseenumerator *) rover,
                                                       item));
}

MULLE_C_NONNULL_FIRST
static inline int
	mulle_pointerarrayreverseenumerator_next( struct mulle_pointerarrayreverseenumerator *rover,
                                             void **item)
{
   return( mulle__pointerarrayreverseenumerator_next( (struct mulle__pointerarrayreverseenumerator *) rover,
                                                      item));
}


MULLE_C_NONNULL_FIRST
static inline void
	_mulle_pointerarrayreverseenumerator_done( struct mulle_pointerarrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


static inline void
   mulle_pointerarrayreverseenumerator_done( struct mulle_pointerarrayreverseenumerator *rover)
{
   MULLE_C_UNUSED( rover);
}


#pragma mark - enumerator convenience

static inline int   
  mulle_pointerarray_member( struct mulle_pointerarray *array, void *p)
{
   return( mulle__pointerarray_member( (struct mulle__pointerarray *) array, p));
}



//
// we have to keep storage out of the for loop
//

// created by make-container-do.sh -ls --flexible mulle-pointerarray.c

#define mulle_pointerarray_do( name)                        \
   for( struct mulle_pointerarray                           \
           name ## __container = { 0 },                     \
           *name = &name ## __container,                    \
           *name ## __i = NULL;                             \
        ! name ## __i;                                      \
        name ## __i =                                       \
        (                                                   \
           mulle_pointerarray_done( name),                  \
           (void *) 0x1                                     \
        )                                                   \
      )                                                     \
      for( int  name ## __j = 0;    /* break protection */  \
           name ## __j < 1;                                 \
           name ## __j++)

#define mulle_pointerarray_do_flexible( name, stackcount)                    \
   void   *name ## __storage[ stackcount];                                   \
   for( struct mulle_pointerarray                                            \
           name ## __container =                                             \
              MULLE_POINTERARRAY_DATA( name ## __storage, stackcount, NULL), \
           *name = &name ## __container,                                     \
           *name ## __i = NULL;                                              \
        ! name ## __i;                                                       \
        name ## __i =                                                        \
        (                                                                    \
           mulle_pointerarray_done( name),                                   \
           (void *) 0x1                                                      \
        )                                                                    \
      )                                                                      \
      for( int  name ## __j = 0;    /* break protection */                   \
           name ## __j < 1;                                                  \
           name ## __j++)


// -----------------------
// Conveniences for iteration.
// -----------------------
// Instead of
//
//   struct  mulle_pointerarrayenumerator   rover;
//   UIView                                 *view;
//
//   rover = mulle_pointerarray_enumerate( &_subviews);
//   while(  _mulle_pointerarrayenumerator_next( &rover, (void **) &view))
//   {
//   }
//   mulle_pointerarrayenumerator_done( &rover);
//
// write:
//
//   UIView   *view;
//
//   mulle_pointerarray_for( &_subviews, view)
//   {
//   }
//
// exploits that mulle_pointerarrayenumerator_done doesn't do anything.
//

// created by make-container-for.sh src/array/pointer/mulle-pointerarray.c

#define mulle_pointerarray_for( name, item)                                              \
   assert( sizeof( item) == sizeof( void *));                                            \
   for( struct mulle_pointerarrayenumerator                                              \
           rover__ ## item = mulle_pointerarray_enumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                        \
        ! rover__  ## item ## __i;                                                       \
        rover__ ## item ## __i = (_mulle_pointerarrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                          \
      while( _mulle_pointerarrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/pointer/mulle-pointerarray.c

#define mulle_pointerarray_for_reverse( name, item)                                             \
   assert( sizeof( item) == sizeof( void *));                                                   \
   for( struct mulle_pointerarrayreverseenumerator                                              \
           rover__ ## item = mulle_pointerarray_reverseenumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                               \
        ! rover__  ## item ## __i;                                                              \
        rover__ ## item ## __i = (_mulle_pointerarrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                                 \
      while( _mulle_pointerarrayreverseenumerator_next( &rover__ ## item, (void **) &item))

#endif
