//
//  mulle--pointerpairarray.h
//  mulle-container
//
//  Created by Nat! on 8.05.16.
//  Copyright (c) 2016 Mulle kybernetiK. All rights reserved.
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
#ifndef mulle__pointerpairarray_h__
#define mulle__pointerpairarray_h__

#include "mulle-pointerpair.h"

#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle__pointerpairarray, simple growing array of pointer pairs
// (kind of like a associative array)
// You can also use it as stack
#define MULLE__POINTERPAIRARRAY_BASE         \
   struct mulle_pointerpair    *_storage;    \
   struct mulle_pointerpair    *_curr;       \
   struct mulle_pointerpair    *_sentinel


struct mulle__pointerpairarray
{
   MULLE__POINTERPAIRARRAY_BASE;
};


static inline struct mulle__pointerpairarray  *
   mulle__pointerpairarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointerpairarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointerpairarray));
   return( array);
}


static inline void
   _mulle__pointerpairarray_init( struct mulle__pointerpairarray *array,
                                  unsigned int  capacity,
                                  struct mulle_allocator *allocator)
{
   *array = (struct mulle__pointerpairarray) { 0 };

   if( capacity)
   {
      array->_storage  = mulle_allocator_calloc( allocator,
                                                 capacity,
                                                 sizeof( struct mulle_pointerpair));
      array->_curr     = array->_storage;
      array->_sentinel = &array->_curr[ capacity];
   }
}


static inline void
   _mulle__pointerpairarray_done( struct mulle__pointerpairarray *array,
                                  struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->_storage);
#ifdef DEBUG   
   memset( array, 0xFD, sizeof( struct mulle__pointerpairarray));
#endif   
}


static inline struct mulle__pointerpairarray  *
   mulle__pointerpairarray_create( struct mulle_allocator *allocator)
{
   struct mulle__pointerpairarray   *array;

   array = mulle__pointerpairarray_alloc( allocator);
   _mulle__pointerpairarray_init( array, 0, allocator);
   return( array);
}


static inline void
   _mulle__pointerpairarray_destroy( struct mulle__pointerpairarray *array,
                                     struct mulle_allocator *allocator)
{
   _mulle__pointerpairarray_done( array, allocator);
   mulle_allocator_free( allocator, array);
}


static inline void
   mulle__pointerpairarray_destroy( struct mulle__pointerpairarray *array,
                                    struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerpairarray_destroy( array, allocator);
}


static inline void
   mulle__pointerpairarray_reset( struct mulle__pointerpairarray *array)
{
   if( array)
      array->_curr = array->_storage;
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__pointerpairarray_get_size( struct mulle__pointerpairarray *array)
{
   return( (unsigned int) (array->_sentinel - array->_storage));
}


static inline unsigned int
   mulle__pointerpairarray_get_size( struct mulle__pointerpairarray *array)
{
   return(  array ? _mulle__pointerpairarray_get_size( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
{
   return( (unsigned int) (array->_curr - array->_storage));
}


static inline unsigned int
   mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
{
   return( array ? _mulle__pointerpairarray_get_count( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle__pointerpairarray_get_guaranteed_size( struct mulle__pointerpairarray *array)
{
   return( (unsigned int) (array->_sentinel - array->_curr));
}


static inline unsigned int
   mulle__pointerpairarray_get_guaranteed_size( struct mulle__pointerpairarray *array)
{
   return( array ? (unsigned int) (array->_sentinel - array->_curr) : 0);
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle__pointerpairarray_is_full( struct mulle__pointerpairarray *array)
{
   return( array->_curr == array->_sentinel);
}


static inline int
   mulle__pointerpairarray_is_full( struct mulle__pointerpairarray *array)
{
   return( array ? _mulle__pointerpairarray_is_full( array) : 1);
}


# pragma mark - operations

// intentionally not static inline
MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
struct mulle_pointerpair  *
   _mulle__pointerpairarray_guarantee( struct mulle__pointerpairarray *array,
                                       unsigned int length,
                                       struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
void   _mulle__pointerpairarray_grow( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator);

MULLE__CONTAINER_GLOBAL
void
   _mulle__pointerpairarray_remove_in_range( struct mulle__pointerpairarray *array,
                                             struct mulle_range range);

MULLE_C_NONNULL_FIRST
static inline void   _mulle__pointerpairarray_add( struct mulle__pointerpairarray *array,
                                                   struct mulle_pointerpair pair,
                                                   struct mulle_allocator *allocator)
{
   if( _mulle__pointerpairarray_is_full( array))
      _mulle__pointerpairarray_grow( array, allocator);

   *array->_curr++ = pair;
}


// if you guarantee that there is enough space for the add beforehand
// you can use this faster version
MULLE_C_NONNULL_FIRST
static inline
void   _mulle__pointerpairarray_add_guaranteed( struct mulle__pointerpairarray *array,
                                                struct mulle_pointerpair pair)
{
   assert( array->_curr < array->_sentinel);
   *array->_curr++ = pair;
}


//
// this removes notakey from the back, until it finds a pointer
// then remove this
//
MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair
   _mulle__pointerpairarray_pop( struct mulle__pointerpairarray *array)
{
   if( array->_curr > array->_storage)
      return( *--array->_curr);
   return( mulle_pointerpair_make_invalid());
}


static inline struct mulle_pointerpair
   mulle__pointerpairarray_pop( struct mulle__pointerpairarray *array)
{
   if( array)
      return( _mulle__pointerpairarray_pop( array));
   return( mulle_pointerpair_make_invalid());
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair
   _mulle__pointerpairarray_get_last( struct mulle__pointerpairarray *array)
{
   if( array->_curr > array->_storage)
      return( array->_curr[ -1]);
   return( mulle_pointerpair_make_invalid());
}

static inline struct mulle_pointerpair
   mulle__pointerpairarray_get_last( struct mulle__pointerpairarray *array)
{
   if( array)
      return( _mulle__pointerpairarray_get_last( array));
   return( mulle_pointerpair_make_invalid());
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair
   _mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                 unsigned int i)
{
   assert( array);
   assert( &array->_storage[ i] >= array->_storage);
   assert( &array->_storage[ i] < array->_curr);

   return( array->_storage[ i]);
}


static inline struct mulle_pointerpair
   mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                unsigned int i)
{
   if( ! array)
      return( mulle_pointerpair_make_invalid());
   return( _mulle__pointerpairarray_get( array, i));
}


MULLE_C_NONNULL_FIRST_THIRD
unsigned int
   _mulle__pointerpairarray_get_in_range( struct mulle__pointerpairarray *array,
                                          struct mulle_range range,
                                          struct mulle_pointerpair *buf);

static inline unsigned int
   mulle__pointerpairarray_get_in_range( struct mulle__pointerpairarray *array,
                                         struct mulle_range range,
                                         struct mulle_pointerpair *buf)
{
   if( ! array || ! buf)
      return( 0);
   return( _mulle__pointerpairarray_get_in_range( array, range, buf));
}




MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair
   _mulle__pointerpairarray_set( struct mulle__pointerpairarray *array,
                                 unsigned int i,
                                 struct mulle_pointerpair pair)
{
   struct mulle_pointerpair   old;

   assert( array);
   assert( &array->_storage[ i] < array->_curr);

   old = array->_storage[ i];
   array->_storage[ i] = pair;
   return( old);
}



#pragma mark - sort and search

MULLE__CONTAINER_GLOBAL
MULLE_C_NONNULL_FIRST
uintptr_t
   _mulle__pointerpairarray_find_in_range( struct mulle__pointerpairarray *array,
                                           struct mulle_pointerpair search,
                                           struct mulle_range range);


static inline uintptr_t
   _mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
                                  struct mulle_pointerpair search)
{
   return( _mulle__pointerpairarray_find_in_range( array,
                                                   search,
                                                   mulle_range_make( 0, array->_curr - array->_storage)));
}



static inline void   mulle__pointerpairarray_qsort_r( struct mulle__pointerpairarray *array,
                                                      mulle_pointerpair_compare_t *compare,
                                                      void *userinfo)
{
   if( array)
      mulle_pointerpair_qsort_r( array->_storage,
                                 mulle__pointerpairarray_get_count( array),
                                 compare,
                                 userinfo);
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__pointerpairarray_find_compare( struct mulle__pointerpairarray *array,
                                            struct mulle_pointerpair search,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
{
   return( _mulle_pointerpair_find_in_range_compare( array->_storage,
                                                     search,
                                                     mulle_range_make( 0, array->_curr - array->_storage),
                                                     compare,
                                                     userinfo));
}


static inline uintptr_t
   mulle__pointerpairarray_find_compare( struct mulle__pointerpairarray *array,
                                           struct mulle_pointerpair search,
                                            mulle_pointerpair_compare_t *compare,
                                            void *userinfo)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerpairarray_find_compare( array, search, compare, userinfo));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle__pointerpairarray_find_callback( struct mulle__pointerpairarray *array,
                                           struct mulle_pointerpair search,
                                           struct mulle_container_keyvaluecallback *callback)
{
   return( _mulle_pointerpair_find_in_range_callback( array->_storage,
                                                      search,
                                                      mulle_range_make( 0, array->_curr - array->_storage),
                                                      callback));
}


static inline uintptr_t
   mulle__pointerpairarray_find_callback( struct mulle__pointerpairarray *array,
                                          struct mulle_pointerpair search,
                                          struct mulle_container_keyvaluecallback *callback)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerpairarray_find_callback( array, search, callback));
}


#ifndef NDEBUG

MULLE__CONTAINER_GLOBAL
void   mulle__pointerpairarray_assert_no_dupes( struct mulle__pointerpairarray *array);

#else

static inline void
   mulle__pointerpairarray_assert_no_dupes( struct mulle__pointerpairarray *array)
{
}

#endif



#pragma mark - enumerator

#define MULLE__POINTERPAIRARRAYENUMERATOR_BASE   \
   struct mulle_pointerpair   *_curr;            \
   struct mulle_pointerpair   *_sentinel

struct mulle__pointerpairarrayenumerator
{
   MULLE__POINTERPAIRARRAYENUMERATOR_BASE;
};

#define mulle__pointerpairarrayenumerator_empty  \
   ((struct mulle__pointerpairarrayenumerator) { 0 })


MULLE_C_NONNULL_FIRST
static inline struct mulle__pointerpairarrayenumerator
   _mulle__pointerpairarray_enumerate( struct mulle__pointerpairarray *array)
{
   struct mulle__pointerpairarrayenumerator   rover;

   rover._curr     = array->_storage;
   rover._sentinel = array->_curr;
   assert( rover._sentinel >= rover._curr);

   return( rover);
}


static inline struct mulle__pointerpairarrayenumerator
   mulle__pointerpairarray_enumerate( struct mulle__pointerpairarray *array)
{
   if( ! array)
      return( mulle__pointerpairarrayenumerator_empty);
   return( _mulle__pointerpairarray_enumerate( array));
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
   _mulle__pointerpairarrayenumerator_next( struct mulle__pointerpairarrayenumerator *rover,
                                            struct mulle_pointerpair *pair)
{
   while( rover->_curr < rover->_sentinel)
   {
      *pair = *rover->_curr++;
      return( 1);
   }

   *pair = mulle_pointerpair_make( NULL, NULL);
   return( 0);
}


static inline int
   mulle__pointerpairarrayenumerator_next( struct mulle__pointerpairarrayenumerator *rover,
                                           struct mulle_pointerpair *pair)
{
   struct mulle_pointerpair   dummy;

   pair = pair ? pair : &dummy;
   if( rover)
      return( _mulle__pointerpairarrayenumerator_next( rover, pair));
   *pair = mulle_pointerpair_make( NULL, NULL);
   return( 0);
}


MULLE_C_NONNULL_FIRST
static inline void
   _mulle__pointerpairarrayenumerator_done( struct mulle__pointerpairarrayenumerator *rover)
{
}


static inline void
   mulle__pointerpairarrayenumerator_done( struct mulle__pointerpairarrayenumerator *rover)
{
}


// created by make-container-do.sh -v -ls --type struct mulle_pointerpair    mulle--pointerpairarray.c

#define mulle__pointerpairarray_do( name)                              \
   for( struct mulle__pointerpairarray                                 \
           name ## __container = { 0 },                                \
           *name = &name ## __container,                               \
           *name ## __i = NULL;                                        \
        ! name ## __i;                                                 \
        name ## __i =                                                  \
        (                                                              \
           _mulle__pointerpairarray_done( &name ## __container, NULL), \
           (void *) 0x1                                                \
        )                                                              \
      )                                                                \
      for( int  name ## __j = 0;    /* break protection */             \
           name ## __j < 1;                                            \
           name ## __j++)


// TODO: Use standard key, value for this ?
//
#define mulle__pointerpairarray_for( name, pair)                                               \
   for( struct mulle__pointerpairarrayenumerator                                               \
           rover__ ## pair = mulle__pointerpairarray_enumerate( name),                         \
           *rover___  ## pair ## __i = (void *) 0;                                             \
        ! rover___  ## pair ## __i;                                                            \
        rover___ ## pair ## __i = (_mulle__pointerpairarrayenumerator_done( &rover__ ## pair), \
                                   (void *) 1))                                                \
      while( _mulle__pointerpairarrayenumerator_next( &rover__ ## pair, &pair))


#endif
