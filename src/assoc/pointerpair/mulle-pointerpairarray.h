//
//  mulle-pointerpairarray.h
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
#ifndef mulle_pointerpairarray_h__
#define mulle_pointerpairarray_h__

#include "mulle--pointerpairarray.h"

#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

//
// mulle_pointerpairarray, simple growing array of pointer pairs
// (kind of like a associative array)
// TODO: hide mulle_pointerpair from API and use key/value ?
// You can also use it as stack
//
struct mulle_pointerpairarray
{
   MULLE__POINTERPAIRARRAY_BASE;
   struct mulle_allocator      *allocator;
};


static inline struct mulle_pointerpairarray  *
	mulle_pointerpairarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerpairarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle_pointerpairarray));
   return( array);
}


static inline void   _mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                                   size_t  capacity,
                                                   struct mulle_allocator *allocator)
{
   _mulle__pointerpairarray_init( (struct mulle__pointerpairarray *) array,
                                   capacity,
                                   allocator);

   array->allocator = allocator;
}


static inline void  _mulle_pointerpairarray_done( struct mulle_pointerpairarray *array)
{
   _mulle__pointerpairarray_done( (struct mulle__pointerpairarray *) array, array->allocator);
}


static inline struct mulle_pointerpairarray *
   mulle_pointerpairarray_create( struct mulle_allocator *allocator)
{
   struct mulle_pointerpairarray   *array;

   array = mulle_pointerpairarray_alloc( allocator);
   _mulle_pointerpairarray_init( array, 0, allocator);
   return( array);
}


static inline void  mulle_pointerpairarray_destroy( struct mulle_pointerpairarray *array)
{
   if( array)
      _mulle__pointerpairarray_destroy( (struct mulle__pointerpairarray *) array, array->allocator);
}


static inline void
   mulle_pointerpairarray_reset( struct mulle_pointerpairarray *array)
{
   mulle__pointerpairarray_reset( (struct mulle__pointerpairarray *) array);
}


# pragma mark - petty accessors


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_pointerpairarray_get_size( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_get_size( (struct mulle__pointerpairarray *) array));
}


static inline size_t
   mulle_pointerpairarray_get_size( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_get_size( (struct mulle__pointerpairarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_pointerpairarray_get_count( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_get_count( (struct mulle__pointerpairarray *) array));
}


static inline size_t
   mulle_pointerpairarray_get_count( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_get_count( (struct mulle__pointerpairarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle_pointerpairarray_get_guaranteed_size( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_get_guaranteed_size( (struct mulle__pointerpairarray *) array));
}


static inline size_t
	mulle_pointerpairarray_get_guaranteed_size( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_get_guaranteed_size( (struct mulle__pointerpairarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline size_t
   _mulle_pointerpairarray_is_full( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_is_full( (struct mulle__pointerpairarray *) array));
}


static inline int
   mulle_pointerpairarray_is_full( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_is_full( (struct mulle__pointerpairarray *) array));
}


# pragma mark - operations

MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                                  struct mulle_pointerpair pair)
{
   _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) array,
                                 pair,
                                 array->allocator);
}


// here the caller guarantees that there is enough space left to add
// also he has to guarantee that p->key is not callback->keycallback.notakey,
// which will only be asserted
MULLE_C_NONNULL_FIRST
static inline
void    _mulle_pointerpairarray_add_guaranteed( struct mulle_pointerpairarray *array,
                                                struct mulle_pointerpair pair)
{
   _mulle__pointerpairarray_add_guaranteed( (struct mulle__pointerpairarray *) array,
                                             pair);
}




static inline void
   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                               struct mulle_pointerpair pair)
{
   if( array)
      _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) array,
                                    pair,
                                    array->allocator);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                                     void *key,
                                                     void *value)
{
   _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) array,
                                 (struct mulle_pointerpair) { key, value },
                                 array->allocator);
}



static inline void
   mulle_pointerpairarray_add_kv( struct mulle_pointerpairarray *array,
                                  void *key,
                                  void *value)
{
   if( array)
      _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) array,
                                    (struct mulle_pointerpair) { key, value },
                                    array->allocator);
}



static inline struct mulle_pointerpair
	mulle_pointerpairarray_pop( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_pop( (struct mulle__pointerpairarray *) array));
}


static inline struct mulle_pointerpair
	mulle_pointerpairarray_get_last( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_get_last( (struct mulle__pointerpairarray *) array));
}


MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpair
	_mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                                size_t i)
{
   return( _mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) array,
                                         i));
}


static inline struct mulle_pointerpair
   mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                               size_t i)
{
   if( ! array )
      return( mulle_pointerpair_make_invalid());

   return( _mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) array,
                                         i));
}


MULLE_C_NONNULL_FIRST_THIRD
static inline size_t
   _mulle_pointerpairarray_get_in_range( struct mulle_pointerpairarray *array,
                                         struct mulle_range range,
                                         void *buf)
{
   return( _mulle__pointerpairarray_get_in_range( (struct mulle__pointerpairarray *) array,
                                                  range,
                                                  buf));
}

static inline size_t
   mulle_pointerpairarray_get_in_range( struct mulle_pointerpairarray *array,
                                        struct mulle_range range,
                                        void *buf)
{
   return( mulle__pointerpairarray_get_in_range( (struct mulle__pointerpairarray *) array,
                                                 range,
                                                 buf));
}


static inline void
	mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                               size_t i,
                               struct mulle_pointerpair pair)
{
   _mulle__pointerpairarray_set( (struct mulle__pointerpairarray *) array,
                                 i,
                                 pair);
}


#pragma mark - sort and find


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle_pointerpairarray_find_in_range( struct mulle_pointerpairarray *array,
                                          struct mulle_pointerpair search,
                                          struct mulle_range range)
{
   return( _mulle__pointerpairarray_find_in_range( (struct mulle__pointerpairarray *) array,
                                                   search,
                                                   range));
}


static inline uintptr_t
   mulle_pointerpairarray_find_in_range( struct mulle_pointerpairarray *array,
                                         struct mulle_pointerpair search,
                                         struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle_pointerpairarray_find_in_range( array, search, range));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle_pointerpairarray_find( struct mulle_pointerpairarray *array,
                                 struct mulle_pointerpair search)
{
   return( _mulle__pointerpairarray_find( (struct mulle__pointerpairarray *) array,
                                          search));
}


static inline uintptr_t
   mulle_pointerpairarray_find( struct mulle_pointerpairarray *array,
                                struct mulle_pointerpair search)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle_pointerpairarray_find( array, search));
}


MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle_pointerpairarray_find_callback( struct mulle_pointerpairarray *array,
                                          struct mulle_pointerpair search,
                                          struct mulle_container_keyvaluecallback *callback)
{
   return( _mulle__pointerpairarray_find_callback( (struct mulle__pointerpairarray *) array,
                                                   search,
                                                   callback));
}


static inline uintptr_t
   mulle_pointerpairarray_find_callback( struct mulle_pointerpairarray *array,
                                         struct mulle_pointerpair search,
                                         struct mulle_container_keyvaluecallback *callback)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle_pointerpairarray_find_callback( array, search, callback));
}



MULLE_C_NONNULL_FIRST
static inline uintptr_t
   _mulle_pointerpairarray_find_compare( struct mulle_pointerpairarray *array,
                                         struct mulle_pointerpair search,
                                         mulle_pointerpair_compare_t *compare,
                                         void *userinfo)
{
   return( _mulle__pointerpairarray_find_compare( (struct mulle__pointerpairarray *) array,
                                                  search,
                                                  compare,
                                                  userinfo));
}


static inline uintptr_t
   mulle_pointerpairarray_find_compare( struct mulle_pointerpairarray *array,
                                        struct mulle_pointerpair search,
                                        mulle_pointerpair_compare_t *compare,
                                        void *userinfo)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle_pointerpairarray_find_compare( array,
                                                 search,
                                                 compare,
                                                 userinfo));
}


static inline void   mulle_pointerpairarray_qsort_r( struct mulle_pointerpairarray *array,
                                                     mulle_pointerpair_compare_t *compare,
                                                     void *userinfo)
{
   mulle__pointerpairarray_qsort_r( (struct mulle__pointerpairarray *) array,
                                    compare,
                                    userinfo);
}

static inline void   mulle_pointerpairarray_qsort( struct mulle_pointerpairarray *array,
                                                   mulle_pointerpair_compare_t *compare)
{
   mulle__pointerpairarray_qsort_r( (struct mulle__pointerpairarray *) array,
                                    compare,
                                    array);
}




#pragma mark - enumerator

#define _MULLE_POINTERPAIRARRAYENUMERATOR_BASE   \
   struct mulle_pointerpair   *_curr;            \
   struct mulle_pointerpair   *_sentinel

#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
struct mulle_pointerpairarrayenumerator
{
   _MULLE_POINTERPAIRARRAYENUMERATOR_BASE;
   struct mulle__pointerpairarray *_array;
   uintptr_t  _n_mutations;
};
#else
struct mulle_pointerpairarrayenumerator
{
   _MULLE_POINTERPAIRARRAYENUMERATOR_BASE;
};
#endif

MULLE_C_NONNULL_FIRST
static inline struct mulle_pointerpairarrayenumerator
   _mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpairarrayenumerator    rover;
   struct mulle__pointerpairarrayenumerator   tmp;

   tmp = _mulle__pointerpairarray_enumerate( (struct mulle__pointerpairarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__pointerpairarrayenumerator));
   return( rover);
}


static inline struct mulle_pointerpairarrayenumerator
   mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpairarrayenumerator    rover;
   struct mulle__pointerpairarrayenumerator   tmp;

   tmp = mulle__pointerpairarray_enumerate( (struct mulle__pointerpairarray *) array);
   memcpy( &rover, &tmp, sizeof( struct mulle__pointerpairarrayenumerator));
   return( rover);
}


MULLE_C_NONNULL_FIRST_SECOND
static inline int
	_mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover,
                                           struct mulle_pointerpair *pair)
{
   return( _mulle__pointerpairarrayenumerator_next( (struct mulle__pointerpairarrayenumerator *) rover,
                                                    pair));
}


static inline int
	mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover,
                                          struct mulle_pointerpair *pair)
{
   return( mulle__pointerpairarrayenumerator_next( (struct mulle__pointerpairarrayenumerator *) rover,
                                                   pair));
}


MULLE_C_NONNULL_FIRST
static inline void
	_mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
{
   _mulle__pointerpairarrayenumerator_done( (struct mulle__pointerpairarrayenumerator *) rover);
}


static inline void
	mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
{
   mulle__pointerpairarrayenumerator_done( (struct mulle__pointerpairarrayenumerator *) rover);
}



// created by make-container-do.sh -v -ls --type struct mulle_pointerpair    mulle-pointerpairarray.c

#define mulle_pointerpairarray_do( name)                        \
   for( struct mulle_pointerpairarray                           \
           name ## __container = { 0 },                         \
           *name = &name ## __container,                        \
           *name ## __i = NULL;                                 \
        ! name ## __i;                                          \
        name ## __i =                                           \
        (                                                       \
           _mulle_pointerpairarray_done( &name ## __container), \
           (void *) 0x1                                         \
        )                                                       \
      )                                                         \
      for( int  name ## __j = 0;    /* break protection */      \
           name ## __j < 1;                                     \
           name ## __j++)


//
// TODO: Use standard key, value for this ?
//
#define mulle_pointerpairarray_for( name, pair)                                               \
   for( struct mulle_pointerpairarrayenumerator                                               \
           rover__ ## pair = mulle_pointerpairarray_enumerate( name),                         \
           *rover__  ## pair ## __i = (void *) 0;                                            \
        ! rover__  ## pair ## __i;                                                           \
        rover__ ## pair ## __i = (_mulle_pointerpairarrayenumerator_done( &rover__ ## pair), \
                                   (void *) 1))                                               \
      while( _mulle_pointerpairarrayenumerator_next( &rover__ ## pair, &pair))




#endif
