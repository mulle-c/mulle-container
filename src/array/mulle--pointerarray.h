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
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle__pointerarray is a growing array of pointers. Basically a void *[]
// with a realloc. Nothing more.
// It doesn't shrink nor can you remove elements. It's quite more limited than
// mulle_pointerarray which tries to be more convenient.
//
#define MULLE__POINTERARRAY_BASE          \
   void                     **_pointers;  \
   unsigned int             _used;        \
   unsigned int             _size

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

static inline struct mulle__pointerarray  *
   mulle__pointerarray_create( struct mulle_allocator *allocator)
{
   struct mulle__pointerarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointerarray));
   return( array);
}


static inline void   _mulle__pointerarray_init( struct mulle__pointerarray *array,
                                                unsigned int  capacity,
                                                struct mulle_allocator *allocator)
{
   array->_size        = 0;
   array->_used        = 0;
   array->_pointers    = NULL;

   if( capacity)
   {
      array->_pointers = mulle_allocator_calloc( allocator,
                                                 capacity,
                                                 sizeof( void *));
      array->_size     = capacity;
   }
}


static inline void  _mulle__pointerarray_done( struct mulle__pointerarray *array,
                                               struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, array->_pointers);
}



static inline void  _mulle__pointerarray_destroy( struct mulle__pointerarray *array,
                                                  struct mulle_allocator *allocator)
{
   _mulle__pointerarray_done( array, allocator);
   mulle_allocator_free( allocator, array);
}

# pragma mark -
# pragma mark petty accessors

static inline unsigned int
   _mulle__pointerarray_get_count( struct mulle__pointerarray *array)
{
   return( array->_used);
}



# pragma mark -
# pragma mark operations
int   _mulle__pointerarray_grow( struct mulle__pointerarray *array,
                                 struct mulle_allocator *allocator);

static inline int
   _mulle__pointerarray_add( struct mulle__pointerarray *array,
                             void  *pointer,
                             struct mulle_allocator *allocator)
{
   if( array->_used == array->_size)
      if( _mulle__pointerarray_grow( array, allocator))
      {
         assert( 0);
         return( -1);
      }

   array->_pointers[ array->_used++] = pointer;
   return( 0);
}


static inline void  *
   _mulle__pointerarray_get( struct mulle__pointerarray *array, unsigned int i)
{
   assert( array);
   assert( i < array->_used);

   return( array->_pointers[ i]);
}


static inline int
   _mulle__pointerarray_find( struct mulle__pointerarray *array, void *p)
{
   void   **curr;
   void   **sentinel;

   curr     = array->_pointers;
   sentinel = &curr[ array->_used];
   while( curr < sentinel)
   {
      if( *curr == p)
         return( (int) (intptr_t) (curr - array->_pointers));
      curr++;
   }
   return( -1);
}


#pragma mark - enumerator

struct mulle__pointerarrayenumerator
{
   void   **curr;
   void   **sentinel;
   int    direction;
};


static inline void
   mulle__pointerarrayenumerator_zero( struct mulle__pointerarrayenumerator *rover)
{
   memset( rover, 0, sizeof( struct mulle__pointerarrayenumerator));
}

//
// still worth inlining especially if you check before! that
// array must be != nil
//
static inline struct mulle__pointerarrayenumerator
   mulle__pointerarray_enumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   rover.curr        = &array->_pointers[ 0];
   rover.sentinel    = &rover.curr[ array ? array->_used : 0];
   rover.direction   = +1;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}


static inline struct  mulle__pointerarrayenumerator
   mulle__pointerarray_reverseenumerate( struct mulle__pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   // can't deal with array=nil here because we don't know notakey

   rover.sentinel  = &array->_pointers[ -1];
   rover.curr      = &rover.sentinel[ array ? array->_used : 0];
   rover.direction = -1;
   assert( rover.sentinel <= rover.curr);

   return( rover);
}


static inline void   *
   _mulle__pointerarrayenumerator_next( struct mulle__pointerarrayenumerator *rover)
{
   void   *p;

   while( rover->curr != rover->sentinel)
   {
      p            = *rover->curr;
      rover->curr += rover->direction;
      return( p);
   }
   return( NULL);
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

static inline int   mulle__pointerarray_member( struct mulle__pointerarray *array, void *p)
{
   struct  mulle__pointerarrayenumerator   rover;
   void                                    *q;

   rover = mulle__pointerarray_enumerate( array);
   {
      while( (q = _mulle__pointerarrayenumerator_next( &rover)))
         if( q == p)
            break;
   }
   mulle__pointerarrayenumerator_done( &rover);

   return( q == p);
}

#endif
