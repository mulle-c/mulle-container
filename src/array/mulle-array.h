//
//  mulle-array.h
//  mulle-container
//
//  Created by Nat! on 04/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
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

#ifndef mulle_array_h__
#define mulle_array_h__

#include "mulle--array.h"
#include "mulle-container-operation.h"

#include <assert.h>

//
// mulle-array is a mulle--array with the allocator and callback embedded.
//
// You can not insert callback->notakey. If a value doesn't exist, you will
// get back callback->notakey as "not found" (not necessarily NULL).
//
// You can use the mulle_container_keycallback_int callbacks to store "int"
// into mulle-array and use mulle_int_as_pointer() and mulle_pointer_as_int()
// to convert them for array consumption. But it's much better and more
// efficient to use mulle-structarray for this.
//
#define MULLE_ARRAY_BASE                                         \
   MULLE__ARRAY_BASE;                                            \
   struct mulle_container_keycallback   *callback;               \
   struct mulle_allocator               *allocator


struct mulle_array
{
   MULLE_ARRAY_BASE;
};


#define MULLE_ARRAY_DATA( storage, count, xcallback, xallocator) \
   ((struct mulle_pointerarray)                                  \
   {                                                             \
      ._storage         = (storage),                             \
      ._curr            = (storage),                             \
      ._sentinel        = &(storage)[ count],                    \
      ._initial_storage = (storage),                             \
      .callback         = (xcallback),                           \
      .allocator        = (xallocator)                           \
   })



MULLE_C_NONNULL_THIRD
static inline void    mulle_array_init( struct mulle_array *array,
                                        size_t length,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   if( ! array)
      return;

   _mulle__array_init( (struct mulle__array *) array, length, allocator);

   array->callback  = callback;
   array->allocator = allocator;
}


#define mulle_array_init_default( array, callback)   \
   mulle_array_init( array, 8, callback, NULL)


static inline void   _mulle_array_done( struct mulle_array *array)
{
   _mulle__array_done( (struct mulle__array *) array,
                       array->callback,
                       array->allocator);
}


static inline void   mulle_array_done( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_done( (struct mulle__array *) array,
                       array->callback,
                       array->allocator);
#ifdef DEBUG
   mulle_memset_uint32( array, 0xDEADDEAD, sizeof( *array));
#endif
}


MULLE__CONTAINER_GLOBAL
struct mulle_array    *
   mulle_array_create( size_t capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator);


#define mulle_array_create_default( callback)   \
   mulle_array_create( 8, callback, NULL)

static inline void   mulle_array_destroy( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_destroy( (struct mulle__array *) array,
                          array->callback,
                          array->allocator);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle_array_get_storage( struct mulle_array *array)
{
   return( _mulle__array_get_storage( (struct mulle__array *) array));
}



MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_array_get_size( struct mulle_array *array)
{
   return( _mulle__array_get_size( (struct mulle__array *) array));
}


static inline size_t
   mulle_array_get_size( struct mulle_array *array)
{
   return( mulle__array_get_size( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_array_get_guaranteed_size( struct mulle_array *array)
{
   return( _mulle__array_get_guaranteed_size( (struct mulle__array *) array));
}


static inline size_t
	mulle_array_get_guaranteed_size( struct mulle_array *array)
{
   return( mulle__array_get_guaranteed_size( (struct mulle__array *) array));
}



MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_array_get_count( struct mulle_array *array)
{
   return( _mulle__array_get_count( (struct mulle__array *) array));
}


static inline size_t
   mulle_array_get_count( struct mulle_array *array)
{
   return( mulle__array_get_count( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_array_is_full( struct mulle_array *array)
{
   return( _mulle__array_is_full( (struct mulle__array *) array));
}


static inline int
   mulle_array_is_full( struct mulle_array *array)
{
   return( mulle__array_is_full( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_container_keycallback  *
   _mulle_array_get_callback( struct mulle_array *array)
{
   return( array->callback);
}


static inline struct mulle_container_keycallback *
   mulle_array_get_callback( struct mulle_array *array)
{
   return( array ? array->callback : NULL);
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_allocator  *
   _mulle_array_get_allocator( struct mulle_array *array)
{
   return( array->allocator);
}


static inline struct mulle_allocator *
   mulle_array_get_allocator( struct mulle_array *array)
{
   return( array ? array->allocator : NULL);
}


# pragma mark - memory operations

MULLE_C_NONNULL_FIRST
static inline void **
   _mulle_array_guarantee( struct mulle_array *array,
                           size_t length)
{
   return( _mulle__array_guarantee( (struct mulle__array *) array,
                                    length,
                                    array->allocator));
}


static inline void **
   mulle_array_guarantee( struct mulle_array *array,
                          size_t length)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_guarantee( (struct mulle__array *) array,
                                    length,
                                    array->allocator));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_array_grow( struct mulle_array *array)
{
   _mulle__array_grow( (struct mulle__array *) array,
                        array->allocator);
}


static inline void
   mulle_array_grow( struct mulle_array *array)
{
   if( array)
      _mulle__array_grow( (struct mulle__array *) array,
                           array->allocator);
}


# pragma mark - operations

static inline void   mulle_array_remove_in_range( struct mulle_array *array,
                                                  struct mulle_range range)
{
   if( array)
      _mulle__array_remove_in_range( (struct mulle__array *) array,
                                     range,
                                     array->callback,
                                     array->allocator);
}


static inline void   _mulle_array_remove( struct mulle_array *array,
                                          void *item)
{
   _mulle__array_remove( (struct mulle__array *) array,
                         item,
                         array->callback,
                         array->allocator);
}


static inline void   mulle_array_remove( struct mulle_array *array,
                                         void *item)
{
   if( array)
      _mulle__array_remove( (struct mulle__array *) array,
                            item,
                            array->callback,
                            array->allocator);
}


static inline void   _mulle_array_remove_unique( struct mulle_array *array,
                                               void *item)
{
   _mulle__array_remove_unique( (struct mulle__array *) array,
                              item,
                              array->callback,
                              array->allocator);
}


//
// Remove first occurence of item. Other occurences of item will remain.
// Will remove from back to front
//
static inline void   mulle_array_remove_unique( struct mulle_array *array,
                                              void *item)
{
   if( array)
      _mulle__array_remove_unique( (struct mulle__array *) array,
                                 item,
                                 array->callback,
                                 array->allocator);
}




MULLE_C_NONNULL_FIRST
static inline void   _mulle_array_reset( struct mulle_array *array)
{
   _mulle__array_reset( (struct mulle__array *) array,
                        array->callback,
                        array->allocator);
}


static inline void   mulle_array_reset( struct mulle_array *array)
{
   if( array)
      _mulle_array_reset( array);
}


// will use callbacks of array to determine equality
static inline int   mulle_array_is_equal( struct mulle_array *array,
                                          struct mulle_array *other)
{
   if( ! array || ! other)
      return( array == other);

   assert( array->callback->is_equal == other->callback->is_equal);
   return( _mulle__array_is_equal( (struct mulle__array *) array,
                                   (struct mulle__array *) other,
                                   array->callback));
}


static inline void    mulle_array_add( struct mulle_array *array,
                                       void  *p)
{
   if( array)
      _mulle__array_add( (struct mulle__array *) array,
                         p,
                         array->callback,
                         array->allocator);
}


MULLE_C_NONNULL_FIRST_THIRD
static inline void    _mulle_array_set( struct mulle_array *array,
                                        size_t i,
                                        void  *p)
{
   _mulle__array_set( (struct mulle__array *) array,
                      i,
                      p,
                      array->callback,
                      array->allocator);
}


MULLE_C_NONNULL_THIRD
static inline void    mulle_array_set( struct mulle_array *array,
                                       size_t i,
                                       void  *p)
{
   if( array)
      _mulle_array_set( array, i, p);
}

/*
 *  Loop over all items. This works as long as you don't remove
 *  anything from the array. It will not retrieve newly added elements.
 *
 *  size_t  i, n;
 *  size_t  *item;
 *
 *  for( i = 0, n = mulle_array_get_count( array); i < n; i++)
 *  {
 *     item = mulle_array_get( array, i);
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
 *  for( i = mulle_array_get_count( array); i;)
 *  {
 *     item = mulle_array_get( array, --i);
 *     printf( "%s\n", (char *) item);
 *  }
 *
 */
static inline void   *mulle_array_get( struct mulle_array *array, size_t index)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_get( (struct mulle__array *) array, index));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline size_t
   _mulle_array_get_in_range( struct mulle_array *array,
                              struct mulle_range range,
                              void *buf)
{
   return( _mulle__array_get_in_range( (struct mulle__array *) array,
                                       range,
                                       buf));
}


static inline size_t
   mulle_array_get_in_range( struct mulle_array *array,
                             struct mulle_range range,
                             void *buf)
{
   return( mulle__array_get_in_range( (struct mulle__array *) array,
                                      range,
                                      buf));
}


static inline void   *mulle_array_get_last( struct mulle_array *array)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_get_last( (struct mulle__array *) array));
}


static inline void   mulle_array_remove_last( struct mulle_array *array)
{
   if( array)
      _mulle__array_remove_last( (struct mulle__array *) array,
                                 array->callback,
                                 array->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_array_add_array( struct mulle_array *array,
                           struct mulle_array *other,
                           struct mulle_range range)
{
   _mulle__array_add_array( (struct mulle__array *) array,
                            (struct mulle__array *) other,
                            range,
                            array->callback,
                            array->allocator);
}

static inline void
   mulle_array_add_array( struct mulle_array *array,
                          struct mulle_array *other,
                          struct mulle_range range)
{
   if( array)
      _mulle_array_add_array( array, other, range);
}


static inline struct mulle_array   *mulle_array_copy( struct mulle_array *array)
{
   struct mulle_array   *other;

   if( ! array)
      return( NULL);

   // can't allow creation to be done by struct mulle__map
   other = mulle_array_create( _mulle_array_get_count( array),
                               array->callback,
                               array->allocator);
   _mulle__array_copy_items( (struct mulle__array *) other,
                             (struct mulle__array *) array,
                             array->callback,
                             array->allocator);
   return( other);
}

#pragma mark - search

static inline uintptr_t
   mulle_array_find_in_range_identical( struct mulle_array *array,
                                        void *obj,
                                        struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range_identical( (struct mulle__array *) array,
                                                   obj,
                                                   range));
}


static inline uintptr_t
   mulle_array_find_in_range( struct mulle_array *array,
                              void *obj,
                              struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range( (struct mulle__array *) array,
                                        obj,
                                        range,
                                        array->callback));
}


static inline uintptr_t
   mulle_array_find( struct mulle_array *array, void *obj)
{
   return( mulle_array_find_in_range( array, obj, mulle_range_make_all()));
}



MULLE_C_NONNULL_FIRST
static inline void
   _mulle_array_qsort_r_inline( struct mulle_array *array,
                                mulle_pointerarray_cmp_t *compare,
                                void *userinfo)
{
   _mulle__array_qsort_r_inline( (struct mulle__array *) array,
                                 compare,
                                 userinfo);
}


static inline void
   mulle_array_qsort_r_inline( struct mulle_array *array,
                               mulle_pointerarray_cmp_t *compare,
                               void *userinfo)
{
   if( array)
      _mulle_array_qsort_r_inline( array, compare, userinfo);
}


# pragma mark - debugging

// use default allocator for returned string
MULLE_C_NONNULL_FIRST
static inline char   *_mulle_array_describe( struct mulle_array *array)
{
   return( _mulle__array_describe( (struct mulle__array *) array,
                                    array->callback,
                                    NULL));
}

static inline char
   *mulle_array_describe( struct mulle_array *array)
{
   return( mulle__array_describe( (struct mulle__array *) array,
                                   array->callback,
                                   NULL));
}


static inline int   mulle_array_member( struct mulle_array *array,
                                        void *p)
{
   return( mulle__array_member( (struct mulle__array *) array, p, array->callback));
}


#pragma mark - enumeration

/*
 *  If the array gets manipulated in the loop, enumeration will likely fail.
 *
 *  struct mulle_arrayenumerator   rover;
 *  void                           *item;
 *
 *  rover = mulle_array_enumerate( array);
 *  while( mulle_arrayenumerator_next( &rover, &item))
 *     printf( "%s\n", (char *) item);
 *  mulle_arrayenumerator_done( &rover);
 *
 * or use
 *
 *  char   *item;
 *
 *  mulle_array_for( array, item)
 *     printf( "%s\n", item);
 */
#define MULLE_ARRAYENUMERATOR_BASE   MULLE__ARRAYENUMERATOR_BASE

struct mulle_arrayenumerator
{
   MULLE_ARRAYENUMERATOR_BASE;
};


//
// lots of shit code, because C compilers are broken and we want to
// have one '_' less in our type, yet can't alias struct x = struct y...
// (without typedef), but we also want to avoid base.base.base.base..
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_arrayenumerator
   _mulle_array_enumerate( struct mulle_array *array)
{
   struct mulle_arrayenumerator    rval;
   struct mulle__arrayenumerator   tmp;

   tmp = _mulle__array_enumerate( (struct mulle__array *) array, array->callback);
   memcpy( &rval, &tmp, sizeof( struct mulle__arrayenumerator));
   return( rval);
}


static inline struct mulle_arrayenumerator
   mulle_array_enumerate( struct mulle_array *array)
{
   struct mulle_arrayenumerator    rval;
   struct mulle__arrayenumerator   tmp;

   tmp = array
         ? mulle__array_enumerate( (struct mulle__array *) array, array->callback)
         : mulle__arrayenumerator_empty;
   memcpy( &rval, &tmp, sizeof( struct mulle__arrayenumerator));
   return( rval);
}


// item is unchanged if rval is 0 (!)
MULLE_C_NONNULL_FIRST
static inline int
   _mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover, void **item)
{
   return( _mulle__arrayenumerator_next( (struct mulle__arrayenumerator *) rover, item));
}


// item is unchanged if rval is 0 (!)
static inline int
   mulle_arrayenumerator_next( struct mulle_arrayenumerator *rover, void **item)
{
   return( mulle__arrayenumerator_next( (struct mulle__arrayenumerator *) rover, item));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
{
}


static inline void   mulle_arrayenumerator_done( struct mulle_arrayenumerator *rover)
{
}


#pragma mark - reverse enumeration

/*
 *  Enumeration is likely to fail, if the array gets manipulated in the
 *  loop.
 *
 *  struct mulle_arrayreverseenumerator   rover;
 *  void                                  *item;
 *
 *  rover = mulle_array_reverseenumerate( array);
 *  while( mulle_arrayreverseenumerator_next( &rover, &item))
 *     printf( "%s", item);
 *  mulle_arrayreverseenumerator_done( &rover);
 *  printf( "\n");
 *
 *
 * or use
 *
 *  char   *item;
 *
 *  mulle_array_for_reverse( array, item)
 *     printf( "%s\n", item);
 */
#define MULLE_ARRAYREVERSENUMERATOR_BASE   MULLE__ARRAYREVERSEENUMERATOR_BASE


struct mulle_arrayreverseenumerator
{
   MULLE_ARRAYREVERSENUMERATOR_BASE;
};


MULLE_C_NONNULL_FIRST
static inline struct mulle_arrayreverseenumerator
   _mulle_array_reverseenumerate( struct mulle_array *array)
{
   struct mulle__arrayreverseenumerator   tmp;
   struct mulle_arrayreverseenumerator    rval;

   tmp = _mulle__array_reverseenumerate( (struct mulle__array *) array, array->callback);
   memcpy( &rval, &tmp, sizeof( struct mulle__arrayreverseenumerator));
   return( rval);
}


static inline struct mulle_arrayreverseenumerator
   mulle_array_reverseenumerate( struct mulle_array *array)
{
   struct mulle__arrayreverseenumerator   tmp;
   struct mulle_arrayreverseenumerator    rval;

   tmp = array
         ? mulle__array_reverseenumerate( (struct mulle__array *) array, array->callback)
         : mulle__arrayreverseenumerator_empty;
   memcpy( &rval, &tmp, sizeof( struct mulle__arrayreverseenumerator));
   return( rval);
}


// item is unchanged if rval is 0 (!)
MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle_arrayreverseenumerator_next( struct mulle_arrayreverseenumerator *rover,
                                       void **item)
{
   return( _mulle__arrayreverseenumerator_next( (struct mulle__arrayreverseenumerator *) rover,
                                                item));
}


// item is unchanged if rval is 0 (!)
static inline int
   mulle_arrayreverseenumerator_next( struct mulle_arrayreverseenumerator *rover,
                                      void **item)
{
   return( mulle__arrayreverseenumerator_next( (struct mulle__arrayreverseenumerator *) rover,
                                                item));
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle_arrayreverseenumerator_done( struct mulle_arrayreverseenumerator *rover)
{
}


static inline void
   mulle_arrayreverseenumerator_done( struct mulle_arrayreverseenumerator *rover)
{
}




// created by make-container-do.sh --flexible mulle-array.c
// handtweaked initializer

#define mulle_array_do( name, xcallback)                                     \
   for( struct mulle_array                                                   \
           name ## __container = { .callback = xcallback },                  \
           *name = &name ## __container,                                     \
           *name ## __i = NULL;                                              \
        ! name ## __i;                                                       \
        name ## __i =                                                        \
        (                                                                    \
           _mulle_array_done( &name ## __container),                         \
           (void *) 0x1                                                      \
        )                                                                    \
      )                                                                      \
      for( int  name ## __j = 0;    /* break protection */                   \
           name ## __j < 1;                                                  \
           name ## __j++)

#define mulle_array_do_flexible( name, stackcount, callback)                    \
   void   *name ## __storage[ stackcount];                                      \
   for( struct mulle_array                                                      \
           name ## __container =                                                \
              MULLE_ARRAY_DATA( name ## __storage, stackcount, callback, NULL), \
           *name = &name ## __container,                                        \
           *name ## __i = NULL;                                                 \
        ! name ## __i;                                                          \
        name ## __i =                                                           \
        (                                                                       \
           _mulle_array_done( &name ## __container),                            \
           (void *) 0x1                                                         \
        )                                                                       \
      )                                                                         \
      for( int  name ## __j = 0;    /* break protection */                      \
           name ## __j < 1;                                                     \
           name ## __j++)


// created by make-container-for.sh src/array/mulle-array.c

#define mulle_array_for( name, item)                                              \
   assert( sizeof( item) == sizeof( void *));                                     \
   for( struct mulle_arrayenumerator                                              \
           rover__ ## item = mulle_array_enumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                 \
        ! rover__  ## item ## __i;                                                \
        rover__ ## item ## __i = (_mulle_arrayenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                   \
      while( _mulle_arrayenumerator_next( &rover__ ## item, (void **) &item))


// created by make-container-for.sh --reverse src/array/mulle-array.c

#define mulle_array_for_reverse( name, item)                                             \
   assert( sizeof( item) == sizeof( void *));                                            \
   for( struct mulle_arrayreverseenumerator                                              \
           rover__ ## item = mulle_array_reverseenumerate( name),                        \
           *rover__  ## item ## __i = (void *) 0;                                        \
        ! rover__  ## item ## __i;                                                       \
        rover__ ## item ## __i = (_mulle_arrayreverseenumerator_done( &rover__ ## item), \
                                   (void *) 1))                                          \
      while( _mulle_arrayreverseenumerator_next( &rover__ ## item, (void **) &item))


#endif /* mulle_array_h */


