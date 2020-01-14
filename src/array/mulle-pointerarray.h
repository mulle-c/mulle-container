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

#include "include.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle_pointerarray, simple growing array of pointers
// it's simple, because it doesn't chain linked lists of buckets
// but just reallocs.
// You can overwrite pointers too and remove the last item (and keep count)
// Useful as a stack
//
struct mulle_pointerarray
{
   unsigned int             _count;  // # pointers that are notakey
   unsigned int             _used;
   unsigned int             _size;
   void                     **_pointers;
   struct mulle_allocator   *_allocator;
   void                     *_notakey;
};


static inline struct mulle_pointerarray  *
	mulle_pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_pointerarray));
   return( array);
}


static inline void   mulle_pointerarray_init( struct mulle_pointerarray *array,
                                              unsigned int  capacity,
                                              void *notakey,
                                              struct mulle_allocator *allocator)
{
   array->_size        = 0;
   array->_used        = 0;
   array->_count       = 0;
   array->_pointers    = NULL;
   array->_allocator   = allocator;
   array->_notakey     = notakey;

   if( capacity)
   {
      array->_pointers = mulle_allocator_calloc( array->_allocator,
      														 capacity,
      														 sizeof( void *));
      array->_size     = capacity;
   }
}


// notakey = NULL
// capacity = 0
struct mulle_pointerarray *
      mulle_pointerarray_create_nil( struct mulle_allocator *allocator);


static inline void  mulle_pointerarray_done( struct mulle_pointerarray *array)
{
   mulle_allocator_free( array->_allocator, array->_pointers);
}


static inline void  mulle_pointerarray_destroy( struct mulle_pointerarray *array)
{
   mulle_pointerarray_done( array);
   mulle_allocator_free( array->_allocator, array);
}

# pragma mark -
# pragma mark petty accessors

static inline unsigned int
	mulle_pointerarray_get_count( struct mulle_pointerarray *array)
{
   return( array->_count);
}


static inline struct mulle_allocator  *
	mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
{
   return( array->_allocator);
}


static inline void  *
	mulle_pointerarray_get_notakey( struct mulle_pointerarray *array)
{
   return( array->_notakey);
}



# pragma mark -
# pragma mark operations
int   mulle_pointerarray_grow( struct mulle_pointerarray *array);

static inline int
	mulle_pointerarray_add( struct mulle_pointerarray *array, void  *pointer)
{
   assert( pointer != array->_notakey);

   if( array->_used == array->_size)
      if( mulle_pointerarray_grow( array))
      {
         assert( 0);
         return( -1);
      }


   array->_pointers[ array->_used++] = pointer;
   array->_count++;

   return( 0);
}


//
// this removes _notakey from the back, until it finds a pointer
// then remove this. A random remove is not coded. You generally want to
// use mulle_pointerarray_set with `notakey` for this.
//
static inline void  *
	mulle_pointerarray_remove_last( struct mulle_pointerarray *array)
{
   void   **p;
   void   **sentinel;
   void   *pointer;

   sentinel = array->_pointers;
   p        = &sentinel[ array->_used];

   while( p > sentinel)
   {
      pointer = *--p;
      if( pointer != array->_notakey)
      {
         array->_used = (unsigned int) (p - array->_pointers);
         --array->_count;
         return( pointer);
      }
   }

   return( array->_notakey);
}


static inline void  *
	mulle_pointerarray_find_last( struct mulle_pointerarray *array)
{
   void   **p;
   void   **sentinel;
   void   *pointer;

   pointer  = array->_notakey;
   sentinel = array->_pointers;
   p        = &sentinel[ array->_used];

   while( p > sentinel)
   {
      pointer = *--p;
      if( pointer != array->_notakey)
         break;
   }

   return( pointer);
}


static inline void  *
	mulle_pointerarray_get( struct mulle_pointerarray *array, unsigned int i)
{
   assert( array);
   assert( i < array->_used);

   return( array->_pointers[ i]);
}


static inline intptr_t
	mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
{
   void   **curr;
   void   **sentinel;

   curr     = array->_pointers;
   sentinel = &curr[ array->_used];
   while( curr < sentinel)
   {
      if( *curr == p)
         return( (intptr_t) (curr - array->_pointers));
      curr++;
   }
   return( (intptr_t) -1);
}


static inline void   mulle_pointerarray_set( struct mulle_pointerarray *array,
                                             unsigned int i,
                                             void *p)
{
   void   *old;

   assert( array);
   assert( i < array->_used);

   old = array->_pointers[ i];
   if( p)
      array->_count += (old == array->_notakey);
   else
      array->_count -= (old != array->_notakey);

   array->_pointers[ i] = p;
}



#pragma mark -
#pragma mark enumerator

struct mulle_pointerarrayenumerator
{
   void   **curr;
   void   **sentinel;
   void   *notakey;
   int    direction;
};


static inline void
   mulle_pointerarrayenumerator_zero( struct mulle_pointerarrayenumerator *rover)
{
   memset( rover, 0, sizeof( struct mulle_pointerarrayenumerator));
}

//
// still worth inlining especially if you check before! that
// array must be != nil
// should prefix mulle_pointerarray_enumerate with an underscore
//
static inline struct mulle_pointerarrayenumerator
   mulle_pointerarray_enumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   assert( array);

   // can't deal with array=nil here because we don't know notakey

   rover.curr        = &array->_pointers[ 0];
   rover.sentinel    = &rover.curr[ array->_used];
   rover.notakey     = array->_notakey;
   rover.direction   = +1;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}


static inline struct  mulle_pointerarrayenumerator
   mulle_pointerarray_enumerate_nil( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   if( ! array)
   {
      mulle_pointerarrayenumerator_zero( &rover);
      return( rover);
   }

   return( mulle_pointerarray_enumerate( array));
}


static inline struct  mulle_pointerarrayenumerator
   mulle_pointerarray_reverseenumerate( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   assert( array);

   // can't deal with array=nil here because we don't know notakey

   rover.sentinel  = &array->_pointers[ -1];
   rover.curr      = &rover.sentinel[ array->_used];
   rover.notakey   = array->_notakey;
   rover.direction = -1;
   assert( rover.sentinel <= rover.curr);

   return( rover);
}


// use this if array->not_a_key is known to be null
static inline struct  mulle_pointerarrayenumerator
   mulle_pointerarray_reverseenumerate_nil( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   if( ! array)
   {
      mulle_pointerarrayenumerator_zero( &rover);
      return( rover);
   }
   assert( ! array->_notakey);
   return( mulle_pointerarray_reverseenumerate( array));
}


static inline void   *
	mulle_pointerarrayenumerator_next( struct mulle_pointerarrayenumerator *rover)
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
	mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
{
}



#pragma mark -
#pragma mark enumerator convenience

static inline int   mulle_pointerarray_member( struct mulle_pointerarray *array, void *p)
{
   struct  mulle_pointerarrayenumerator   rover;
   void                                   *q;

   rover = mulle_pointerarray_enumerate( array);
   {
      while( (q = mulle_pointerarrayenumerator_next( &rover)) != rover.notakey)
         if( q == p)
            break;
   }
   mulle_pointerarrayenumerator_done( &rover);

   return( q == p);
}

#endif
