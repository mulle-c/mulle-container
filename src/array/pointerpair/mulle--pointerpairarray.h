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
//
#define MULLE__POINTERPAIRARRAY_BASE         \
   struct mulle_pointerpair    *_storage;    \
   struct mulle_pointerpair    *_curr;       \
   struct mulle_pointerpair    *_sentinel


struct mulle__pointerpairarray
{
   MULLE__POINTERPAIRARRAY_BASE;
};


static inline struct mulle__pointerpairarray  *
	mulle__pointerpairarray_alloc( void *notakey,
											 struct mulle_allocator *allocator)
{
   struct mulle__pointerpairarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointerpairarray));
   return( array);
}


static inline void   _mulle__pointerpairarray_init( struct mulle__pointerpairarray *array,
                                                    size_t  capacity,
                                                    struct mulle_allocator *allocator)
{
   memset( array, 0, sizeof( *array));

   if( capacity)
   {
      array->_storage  = mulle_allocator_calloc( allocator,
      													    capacity,
      													    sizeof( struct mulle_pointerpair));
      array->_curr     = array->_storage;
      array->_sentinel = &array->_curr[ capacity];
   }
}


static inline void  _mulle__pointerpairarray_done( struct mulle__pointerpairarray *array,
                                                   struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->_storage);
}


static inline void  mulle__pointerpairarray_destroy( struct mulle__pointerpairarray *array,
                                                     struct mulle_allocator *allocator)
{
   if( array)
   {
      _mulle__pointerpairarray_done( array, allocator);
      mulle_allocator_free( allocator, array);
   }
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle__pointerpairarray_get_size( struct mulle__pointerpairarray *array)
{
   return( (size_t) (array->_sentinel - array->_storage));
}


static inline size_t
   mulle__pointerpairarray_get_size( struct mulle__pointerpairarray *array)
{
   return(  array ? _mulle__pointerpairarray_get_size( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
{
   return( (size_t) (array->_curr - array->_storage));
}


static inline size_t
   mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
{
   return( array ? _mulle__pointerpairarray_get_count( array) : 0);
}


MULLE_C_NONNULL_FIRST
static inline size_t
	_mulle__pointerpairarray_get_guaranteed_size( struct mulle__pointerpairarray *array)
{
   return( (size_t) (array->_sentinel - array->_curr));
}


static inline size_t
	mulle__pointerpairarray_get_guaranteed_size( struct mulle__pointerpairarray *array)
{
   return( array ? (size_t) (array->_sentinel - array->_curr) : 0);
}


MULLE_C_NONNULL_FIRST
static inline size_t
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
void   _mulle__pointerpairarray_grow( struct mulle__pointerpairarray *array,
                                      struct mulle_allocator *allocator);

static inline void   _mulle__pointerpairarray_add( struct mulle__pointerpairarray *array,
                                                   struct mulle_pointerpair pair,
                                                   struct mulle_allocator *allocator)
{
   if( _mulle__pointerpairarray_is_full( array))
      _mulle__pointerpairarray_grow( array, allocator);

   *array->_curr++ = pair;
}


//
// this removes notakey from the back, until it finds a pointer
// then remove this
//
static inline struct mulle_pointerpair
	mulle__pointerpairarray_remove_last( struct mulle__pointerpairarray *array)
{
   if( array->_curr > array->_storage)
      return( *--array->_curr);
   return( mulle_pointerpair_make_invalid());
}



static inline struct mulle_pointerpair
	mulle__pointerpairarray_get_last( struct mulle__pointerpairarray *array)
{
   if( array->_curr > array->_storage)
      return( array->_curr[ -1]);
   return( mulle_pointerpair_make_invalid());
}


MULLE_C_NONNULL_FIRST
static inline
struct mulle_pointerpair
	_mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                 size_t i)
{
   assert( array);
   assert( &array->_storage[ i] < array->_curr);

   return( array->_storage[ i]);
}

static inline
struct mulle_pointerpair
   mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                size_t i)
{
   if( ! array)
      return( mulle_pointerpair_make_invalid());
   return( _mulle__pointerpairarray_get( array, i));
}


static inline uintptr_t
	_mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
	 									    void  *key);


static inline struct mulle_pointerpair
	_mulle__pointerpairarray_set( struct mulle__pointerpairarray *array,
                                 size_t i,
                                 struct mulle_pointerpair pair)
{
   struct mulle_pointerpair   old;

   assert( array);
   assert( &array->_storage[ i] < array->_curr);

   old = array->_storage[ i];
   array->_storage[ i] = pair;
   return( old);
}


#pragma mark - enumerator

#define MULLE__POINTERPAIRARRAYENUMERATOR_BASE   \
   struct mulle_pointerpair   *_curr;            \
   struct mulle_pointerpair   *_sentinel

struct mulle__pointerpairarrayenumerator
{
   MULLE__POINTERPAIRARRAYENUMERATOR_BASE;
};

extern struct mulle__pointerpairarrayenumerator   mulle__pointerpairarrayenumerator_empty;

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

   return( 0);
}


static inline int
	mulle__pointerpairarrayenumerator_next( struct mulle__pointerpairarrayenumerator *rover,
                                           struct mulle_pointerpair *pair)
{
   if( rover)
      while( rover->_curr < rover->_sentinel)
      {
         *pair = *rover->_curr++;
         return( 1);
      }

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

#endif
