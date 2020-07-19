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
#include "mulle-range.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle__pointerarray is a growing array of pointers, it just reallocs.
// You can overwrite pointers. You can remove the last item (and keep count)
// so it is useful as a stack.
// It has a notion of notakey and keeps track of the actual count of notakey
// pointers. But counting them is all it does. Specifially random access with
// get may yield a notakey!
// "count" is the number of valid pointers
// "used" is the number of pointers used (needs this for get indexing)
// "size" is number of pointers space in storage
//
#define MULLE__POINTERARRAY_BASE  \
   void      **storage;           \
   size_t   used;                 \
   size_t   size;                 \
   size_t   count

struct mulle__pointerarray
{
   MULLE__POINTERARRAY_BASE;
};


static inline struct mulle__pointerarray  *
	mulle__pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__pointerarray));
   return( array);
}


static inline void   _mulle__pointerarray_init( struct mulle__pointerarray *array,
                                                size_t  capacity,
                                                struct mulle_allocator *allocator)
{
   memset( array, 0, sizeof( *array));

   if( capacity)
   {
      array->size     = capacity;
      array->storage = mulle_allocator_calloc( allocator,
                                               capacity,
                                               sizeof( void *));
   }
}


static inline void  _mulle__pointerarray_done( struct mulle__pointerarray *array,
                                               struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->storage);
}



static inline void  mulle__pointerarray_done( struct mulle__pointerarray *array,
                                              struct mulle_allocator *allocator)
{
   if( array)
      _mulle__pointerarray_done( array, allocator);
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


# pragma mark - petty accessors

static inline size_t
	_mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( array->count);
}


static inline size_t
   mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( array ? array->count : 0);
}

static inline size_t
   _mulle__pointerarray_get_used( struct mulle__pointerarray *array)
{
   return( array->used);
}


static inline size_t
   mulle__pointerarray_get_used( struct mulle__pointerarray *array)
{
   return( array ? array->used : 0);
}


static inline int
   _mulle__pointerarray_is_compact( struct mulle__pointerarray *array)
{
   return( array->count == array->used);
}


static inline int
   _mulle__pointerarray_can_compact( struct mulle__pointerarray *array)
{
   return( array->count != array->used);
}


static inline int
   _mulle__pointerarray_needs_compaction( struct mulle__pointerarray *array)
{
   return( array->count < array->used - array->used / 4);
}



# pragma mark - operations
void   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                  struct mulle_allocator *allocator);

void   _mulle__pointerarray_compact( struct mulle__pointerarray *array,
                                     void *notakey);

static inline void
	_mulle__pointerarray_add( struct mulle__pointerarray *array,
                             void *pointer,
                             void *notakey,
                             struct mulle_allocator *allocator)
{
   if( pointer == notakey)
      return;

   if( array->used == array->size)
      _mulle__pointerarray_grow( array, allocator);

   array->storage[ array->used++] = pointer;
   array->count                    += pointer != notakey;
}


static inline void
   mulle__pointerarray_add( struct mulle__pointerarray *array,
                            void  *pointer,
                            void *notakey,
                            struct mulle_allocator *allocator)
{
   if( array)
      return( _mulle__pointerarray_add( array, pointer, notakey, allocator));
}


void  *
	_mulle__pointerarray_remove_last( struct mulle__pointerarray *array,
                                     void *notakey);

static inline void
   _mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   array->used  = 0;
   array->count = 0;
}


static inline void
   mulle__pointerarray_reset( struct mulle__pointerarray *array)
{
   if( array)
      _mulle__pointerarray_reset( array);
}


// array must be compact before calling this!
void
   _mulle__pointerarray_remove_in_range( struct mulle__pointerarray *array,
                                         struct mulle_range range);


static inline void  *
	_mulle__pointerarray_get( struct mulle__pointerarray *array, size_t i)
{
   assert( array);
   assert( i < array->used);

   return( array->storage[ i]);
}


static inline void  *
   mulle__pointerarray_get( struct mulle__pointerarray *array,
                            size_t i)
{
   if( ! array)
      return( NULL);
   return( _mulle__pointerarray_get( array, i));
}


//
// return value is the index of a compacted array, but it doesn't compact
// yet
//
uintptr_t
   _mulle__pointerarray_find_in_range( struct mulle__pointerarray *array,
                                       struct mulle_range range,
                                       void *p);

uintptr_t
   _mulle__pointerarray_find( struct mulle__pointerarray *array,
                              void *p);


static inline uintptr_t
   mulle__pointerarray_find( struct mulle__pointerarray *array,
                             void *p)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__pointerarray_find( array, p));
}


static inline void  *
	_mulle__pointerarray_find_last( struct mulle__pointerarray *array,
                                   void *notakey)
{
   void   **p;
   void   **sentinel;
   void   *pointer;

   pointer  = notakey;
   sentinel = array->storage;
   p        = &sentinel[ array->used];

   while( p > sentinel)
   {
      pointer = *--p;
      if( pointer != notakey)
         break;
   }

   return( pointer);
}


static inline void   *_mulle__pointerarray_set( struct mulle__pointerarray *array,
                                                size_t i,
                                                void *p,
                                                void *notakey)
{
   void   *old;

   assert( array);
   assert( i < array->used);

   old = array->storage[ i];
   if( p != notakey)
      array->count += (old == notakey);
   else
      array->count -= (old != notakey);
   array->storage[ i] = p;

   return( old);
}


static inline void   *mulle__pointerarray_set( struct mulle__pointerarray *array,
                                               size_t i,
                                               void *p,
                                               void *notakey)
{
   if( ! array || i >= array->used)
      return( notakey);
   return( _mulle__pointerarray_set( array, i, p, notakey));
}


#pragma mark - enumerator

#define MULLE__POINTERARRAYENUMERATOR_BASE   \
   void   **curr;                            \
   void   **sentinel;                        \
   void   *notakey;                          \
   int    direction


struct mulle__pointerarrayenumerator
{
   MULLE__POINTERARRAYENUMERATOR_BASE;
};


//
// still worth inlining especially if you check before! that
// array must be != nil
// should prefix mulle__pointerarray_enumerate with an underscore
//
static inline struct mulle__pointerarrayenumerator
   _mulle__pointerarray_enumerate( struct mulle__pointerarray *array,
                                   void *notakey)
{
   struct mulle__pointerarrayenumerator   rover;

   assert( array);

   // can't deal with array=nil here because we don't know notakey

   rover.curr      = &array->storage[ 0];
   rover.sentinel  = &rover.curr[ array->used];
   rover.notakey   = notakey;
   rover.direction = +1;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}


static inline struct  mulle__pointerarrayenumerator
   mulle__pointerarray_enumerate_nil( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   return( _mulle__pointerarray_enumerate( array, NULL));
}


static inline struct  mulle__pointerarrayenumerator
   _mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array,
                                          void *notakey)
{
   struct mulle__pointerarrayenumerator   rover;

   assert( array);

   // can't deal with array=nil here because we don't know notakey

   rover.sentinel  = &array->storage[ -1];
   rover.curr      = &rover.sentinel[ array->used];
   rover.notakey   = notakey;
   rover.direction = -1;
   assert( rover.sentinel <= rover.curr);

   return( rover);
}


// use this if array->not_a_key is known to be null
static inline struct  mulle__pointerarrayenumerator
   mulle__pointerarray_reverseenumerate_nil( struct mulle__pointerarray *array,
                                             void *notakey)
{
   struct mulle__pointerarrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }
   assert( ! notakey);
   return( _mulle__pointerarray_reverseenumerate( array, notakey));
}


static inline void   *
	_mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover)
{
   void   *p;

   while( rover->curr != rover->sentinel)
   {
      p            = *rover->curr;
      rover->curr += rover->direction;
      if( p != rover->notakey)
         return( p);
   }
   return( rover->notakey);
}


static inline void
	_mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
}


static inline void
   mulle__pointerarrayenumerator_done( struct mulle__pointerarrayenumerator *rover)
{
}



#pragma mark - enumerator convenience

static inline int   mulle__pointerarray_member( struct mulle__pointerarray *array,
                                                void *p,
                                                void *notakey)
{
   struct  mulle__pointerarrayenumerator   rover;
   void                                    *q;

   rover = _mulle__pointerarray_enumerate( array, notakey);
   {
      while( (q = _mulle__pointerarrayenumerator_next( &rover)) != notakey)
         if( q == p)
            break;
   }
   mulle__pointerarrayenumerator_done( &rover);

   return( q == p);
}

#endif
