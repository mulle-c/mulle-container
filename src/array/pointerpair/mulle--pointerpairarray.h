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
// Can overwrite pointers too (and keep count)
// You can also use it as stack
//
#define MULLE__POINTERPAIRARRAY_BASE         \
   struct mulle_pointerpair    *storage;      \
   size_t                used;        \
   size_t                size;        \
   size_t                count  // # pointers that are notakey


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
      array->size  = capacity;
      array->storage = mulle_allocator_calloc( allocator,
      													 capacity,
      													 sizeof( struct mulle_pointerpair));
   }
}


static inline void  _mulle__pointerpairarray_done( struct mulle__pointerpairarray *array,
                                                   struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->storage);
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

static inline size_t
	_mulle__pointerpairarray_get_count( struct mulle__pointerpairarray *array)
{
   return( array->count);
}


# pragma mark - petty functions

static inline int
   _mulle__pointerpairarray_is_full( struct mulle__pointerpairarray *array)
{
   return( array->count == array->used);
}


static inline int
   _mulle__pointerpairarray_can_compact( struct mulle__pointerpairarray *array)
{
   return( array->count != array->used);
}


static inline int
   _mulle__pointerpairarray_needs_compaction( struct mulle__pointerpairarray *array)
{
   return( array->count < array->used - array->used / 4);
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

   array->storage[ array->used++] = pair;
   array->count++;
}


//
// this removes notakey from the back, until it finds a pointer
// then remove this
//
static inline struct mulle_pointerpair
	mulle__pointerpairarray_remove_last( struct mulle__pointerpairarray *array,
                                        void *notakey)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   sentinel = array->storage;
   p        = &sentinel[ array->used];

   while( p > sentinel)
   {
      --p;
      if( p->_key != notakey)
      {
         array->used = (size_t) (p - array->storage);
         --array->count;
         return( *p);
      }
   }

   return( mulle_pointerpair_make( notakey, NULL));
}


static inline struct mulle_pointerpair
	mulle__pointerpairarray_find_last( struct mulle__pointerpairarray *array,
                                      void *notakey)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   sentinel = array->storage;
   p        = &sentinel[ array->used];

   while( p > sentinel)
   {
      --p;
      if( p->_key != notakey)
         return( *p);
   }

   return( mulle_pointerpair_make( notakey, NULL));
}

void   _mulle__pointerpairarray_compact( struct mulle__pointerpairarray *array,
                                         void *notakey);


static inline struct mulle_pointerpair
	_mulle__pointerpairarray_get( struct mulle__pointerpairarray *array,
                                 size_t i,
                                 void *notakey)
{
   assert( array);
   assert( i < array->used);

   if( _mulle__pointerpairarray_can_compact( array))
      _mulle__pointerpairarray_compact( array, notakey);

   return( array->storage[ i]);
}


static inline uintptr_t
	_mulle__pointerpairarray_find( struct mulle__pointerpairarray *array,
	 									    void  *key,
                                  void  *notakey);


static inline void
	_mulle__pointerpairarray_set( struct mulle__pointerpairarray *array,
                                 size_t i,
                                 struct mulle_pointerpair pair,
                                 void *notakey)
{
   struct mulle_pointerpair   *old;

   assert( array);
   assert( i < array->used);

   old = &array->storage[ i];
   if( pair._key != notakey)
      array->count += (old->_key == notakey);
   else
      array->count -= (old->_key != notakey);

   array->storage[ i] = pair;
}


#pragma mark - enumerator

#define MULLE__POINTERPAIRARRAYENUMERATOR_BASE  \
   struct mulle_pointerpair   *curr;            \
   struct mulle_pointerpair   *sentinel;        \
   void                       *notakey

struct mulle__pointerpairarrayenumerator
{
   MULLE__POINTERPAIRARRAYENUMERATOR_BASE;
};


static inline void
   mulle__pointerpairarrayenumerator_zero( struct mulle__pointerpairarrayenumerator *rover)
{
   memset( rover, 0, sizeof( struct mulle__pointerpairarrayenumerator));
}



static inline struct mulle__pointerpairarrayenumerator
	mulle__pointerpairarray_enumerate( struct mulle__pointerpairarray *array,
                                      void *notakey)
{
   struct mulle__pointerpairarrayenumerator   rover;

   rover.curr     = &array->storage[ 0];
   rover.sentinel = &rover.curr[ array->used];
   rover.notakey  = notakey;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}


static inline struct  mulle__pointerpairarrayenumerator
   mulle__pointerpairarray_enumerate_nil( struct mulle__pointerpairarray *array)
{
   struct mulle__pointerpairarrayenumerator   rover;

   if( ! array)
   {
      mulle__pointerpairarrayenumerator_zero( &rover);
      return( rover);
   }

   return( mulle__pointerpairarray_enumerate( array, NULL));
}



static inline struct mulle_pointerpair
	mulle__pointerpairarrayenumerator_next( struct mulle__pointerpairarrayenumerator *rover)
{
   struct mulle_pointerpair   *pair;

   while( rover->curr < rover->sentinel)
   {
      pair = rover->curr++;
      if( pair->_key != rover->notakey)
         return( *pair);
   }

   return( mulle_pointerpair_make( rover->notakey, NULL));
}


static inline void
	mulle__pointerpairarrayenumerator_done( struct mulle__pointerpairarrayenumerator *rover)
{
}

#endif
