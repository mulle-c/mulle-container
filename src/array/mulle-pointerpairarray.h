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


#include "mulle-pointerpair.h"

#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle_pointerpairarray, simple growing array of pointer pairs
// (kind of like a associative array)
// Can overwrite pointers too (and keep count)
// You can also use it as stack
//
struct mulle_pointerpairarray
{
   unsigned int                _count;  // # pointers that are notakey
   unsigned int                _used;
   unsigned int                _size;
   struct mulle_pointerpair    *_pairs;
   struct mulle_allocator      *_allocator;
   void                        *_notakey;
};


static inline struct mulle_pointerpairarray  *mulle_pointerpairarray_alloc( void *notakey, struct mulle_allocator *allocator)
{
   struct mulle_pointerpairarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle_pointerpairarray));
   return( array);
}


static inline void   mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                                  unsigned int  capacity,
                                                  void *notakey,
                                                  struct mulle_allocator *allocator)
{
   array->_size      = 0;
   array->_used      = 0;
   array->_count     = 0;
   array->_pairs     = NULL;
   array->_notakey   = notakey;
   array->_allocator = allocator;

   if( capacity)
   {
      if( capacity < 2)
         capacity = 2;

      array->_pairs = mulle_allocator_calloc( array->_allocator, capacity, sizeof( struct mulle_pointerpair));
      array->_size = capacity;
   }
}


static inline void  mulle_pointerpairarray_done( struct mulle_pointerpairarray *array)
{
   mulle_allocator_free( array->_allocator, array->_pairs);
}


static inline void  mulle_pointerpairarray_destroy( struct mulle_pointerpairarray *array)
{
   mulle_pointerpairarray_done( array);
   mulle_allocator_free( array->_allocator, array);
}


# pragma mark -
# pragma mark petty accessors

static inline unsigned int  mulle_pointerpairarray_get_count( struct mulle_pointerpairarray *array)
{
   return( array->_count);
}


static inline struct mulle_allocator  *mulle_pointerpairarray_get_allocator( struct mulle_pointerpairarray *array)
{
   return( array->_allocator);
}


static inline void  *mulle_pointerpairarray_get_notakey( struct mulle_pointerpairarray *array)
{
   return( array->_notakey);
}


# pragma mark -
# pragma mark operations

// intentionally not static inline
int   mulle_pointerpairarray_grow( struct mulle_pointerpairarray *array);

static inline int   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                                struct mulle_pointerpair pair)
{
   assert( pair._key != array->_notakey);

   if( array->_used == array->_size)
      if( mulle_pointerpairarray_grow( array))
      {
         assert( 0);
         return( -1);
      }

   array->_pairs[ array->_used++] = pair;
   array->_count++;

   return( 0);
}


//
// this removes _notakey from the back, until it finds a pointer
// then remove this
//
static inline struct mulle_pointerpair  mulle_pointerpairarray_remove_last( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   sentinel = array->_pairs;
   p        = &sentinel[ array->_used];

   while( p > sentinel)
   {
      --p;
      if( p->_key != array->_notakey)
      {
         array->_used = (unsigned int) (p - array->_pairs);
         --array->_count;
         return( *p);
      }
   }

   return( mulle_pointerpair_create( array->_notakey, NULL));
}


static inline struct mulle_pointerpair   *mulle_pointerpairarray_find_last( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   sentinel = array->_pairs;
   p        = &sentinel[ array->_used];

   while( p > sentinel)
   {
      --p;
      if( p->_key != array->_notakey)
         return( p);
   }

   return( array->_notakey);
}


static inline struct mulle_pointerpair  mulle_pointerpairarray_get( struct mulle_pointerpairarray *array, unsigned int i)
{
   assert( array);
   assert( i < array->_used);
   return( array->_pairs[ i]);
}


static inline intptr_t   mulle_pointerpairarray_index( struct mulle_pointerpairarray *array, struct mulle_pointerpair pair)
{
   struct mulle_pointerpair   *curr;
   struct mulle_pointerpair   *sentinel;

   curr     = array->_pairs;
   sentinel = &curr[ array->_used];
   while( curr < sentinel)
   {
      if( curr->_key == pair._key)
         return( (intptr_t) (curr - array->_pairs));
      curr++;
   }
   return( (intptr_t) -1);
}


static inline void   mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                                                 unsigned int i,
                                                 struct mulle_pointerpair pair)
{
   struct mulle_pointerpair   *old;

   assert( array);
   assert( i < array->_used);

   old = &array->_pairs[ i];
   if( pair._key != array->_notakey)
      array->_count += (old->_key == array->_notakey);
   else
      array->_count -= (old->_key != array->_notakey);

   array->_pairs[ i] = pair;
}


#pragma mark -
#pragma mark enumerator

struct mulle_pointerpairarray_enumerator
{
   struct mulle_pointerpair   *curr;
   struct mulle_pointerpair   *sentinel;
   void                       *notakey;
};


static inline struct  mulle_pointerpairarray_enumerator   mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpairarray_enumerator   rover;

   rover.curr           = &array->_pairs[ 0];
   rover.sentinel       = &rover.curr[ array->_used];
   rover.notakey = array->_notakey;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}


static inline struct mulle_pointerpair   *mulle_pointerpairarray_enumerator_next( struct mulle_pointerpairarray_enumerator *rover)
{
   struct mulle_pointerpair   *pair;

   while( rover->curr < rover->sentinel)
   {
      pair = rover->curr++;
      if( pair->_key != rover->notakey)
         return( pair);
   }
   return( NULL);
}


static inline void  mulle_pointerpairarray_enumerator_done( struct mulle_pointerpairarray_enumerator *rover)
{
}

#endif
