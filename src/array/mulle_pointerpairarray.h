//
//  mulle_pointerpairarray.h
//  mulle-aba-test
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


#include <string.h>
#include <mulle_allocator/mulle_allocator.h>
#include "mulle_pointerpair.h"

//
// mulle_pointerpairarray, simple growing array of pointers
// can overwrite pointers too (and keep count)
// you can also use it as stack
//
struct mulle_pointerpairarray
{
   unsigned int                _count;  // # pointers that are notapointer
   unsigned int                _used;
   unsigned int                _size;
   struct mulle_pointerpair   *_pairs;
   struct mulle_allocator      *_allocator;
   void                        *_notakeypointer;
};


static inline struct mulle_pointerpairarray  *mulle_pointerpairarray_alloc( void *notakeypointer, struct mulle_allocator *allocator)
{
   struct mulle_pointerpairarray   *array;
   
   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle_pointerpairarray));
   return( array);
}


static inline void   mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                                  unsigned int  capacity,
                                                  void *notakeypointer,
                                                  struct mulle_allocator *allocator)
{
   array->_size           = 0;
   array->_used           = 0;
   array->_count          = 0;
   array->_pairs          = NULL;
   array->_notakeypointer = notakeypointer;
   array->_allocator      = allocator;
   
   if( capacity)
   {
      if( capacity < 2)
         capacity = 2;
   
      array->_pairs = mulle_allocator_calloc( array->_allocator, capacity, sizeof( struct mulle_pointerpair));
      if( array->_pairs)
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


static inline void  *mulle_pointerpairarray_get_notakeypointer( struct mulle_pointerpairarray *array)
{
   return( array->_notakeypointer);
}


# pragma mark -
# pragma mark mechanisms

static int   mulle_pointerpairarray_grow( struct mulle_pointerpairarray *array)
{
   unsigned int   new_size;

   new_size = array->_size * 2;
   if( new_size < 2)
      new_size = 2;
   
   array->_pairs = mulle_allocator_realloc( array->_allocator, array->_pairs, sizeof( struct mulle_pointerpair) * new_size);
   
   if( ! array->_pairs)
   {
      array->_size = 0;
      assert( 0);
      return( -1);
   }

   memset( &array->_pairs[ array->_size], 0, sizeof( struct mulle_pointerpair) * (new_size - array->_size));
   array->_size = new_size;

   return( 0);
}


# pragma mark -
# pragma mark operations

static inline int   mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                                struct mulle_pointerpair pair)
{
   assert( pair._key != array->_notakeypointer);
   
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
// this removes _notakeypointer from the back, until it finds a pointer
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
      if( p->_key != array->_notakeypointer)
      {
         array->_used = (unsigned int) (p - array->_pairs);
         --array->_count;
         return( *p);
      }
   }
   
   return( mulle_pointerpair_create( array->_notakeypointer, NULL));
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
      if( p->_key != array->_notakeypointer)
         return( p);
   }
   
   return( NULL);
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
   if( pair._key != array->_notakeypointer)
      array->_count += (old->_key == array->_notakeypointer);
   else
      array->_count -= (old->_key != array->_notakeypointer);
   
   array->_pairs[ i] = pair;
}


#pragma mark -
#pragma mark enumerator

struct mulle_pointerpairarray_enumerator
{
   struct mulle_pointerpair   *curr;
   struct mulle_pointerpair   *sentinel;
   void                        *notakeypointer;
};


static inline struct  mulle_pointerpairarray_enumerator   mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpairarray_enumerator   rover;
   
   rover.curr           = &array->_pairs[ 0];
   rover.sentinel       = &rover.curr[ array->_used];
   rover.notakeypointer = array->_notakeypointer;
   assert( rover.sentinel >= rover.curr);
   
   return( rover);
}


static inline struct mulle_pointerpair   *mulle_pointerpairarray_enumerator_next( struct mulle_pointerpairarray_enumerator *rover)
{
   struct mulle_pointerpair   *pair;
   
   while( rover->curr < rover->sentinel)
   {
      pair = rover->curr++;
      if( pair->_key != rover->notakeypointer)
         return( pair);
   }
   return( NULL);
}


static inline void  mulle_pointerpairarray_enumerator_done( struct mulle_pointerpairarray_enumerator *rover)
{
}

#endif
