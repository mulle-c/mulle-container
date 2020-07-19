//
//  mulle-pointers.h
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
#ifndef mulle__pointers_h__
#define mulle__pointers_h__

#include "include.h"

#include "mulle-container-operation.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

//
// mulle__pointers is a growing array of pointers. Basically a void *[]
// with a realloc. Nothing more.
// You can not remove individual elements, though you can remove all with reset.
// It's quite more limited than mulle__pointerarray.
//
#define MULLE__POINTERS_BASE   \
   void           **storage;   \
   size_t         used;        \
   size_t         size


struct mulle__pointers
{
   MULLE__POINTERS_BASE;
};


static inline struct mulle__pointers  *
   mulle__pointers_alloc( struct mulle_allocator *allocator)
{
   struct mulle__pointers   *pointers;

   pointers = mulle_allocator_malloc( allocator, sizeof( struct mulle__pointers));
   return( pointers);
}


static inline struct mulle__pointers  *
   mulle__pointers_create( struct mulle_allocator *allocator)
{
   struct mulle__pointers   *pointers;

   pointers = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointers));
   return( pointers);
}


static inline void   _mulle__pointers_init( struct mulle__pointers *pointers,
                                            size_t  capacity,
                                            struct mulle_allocator *allocator)
{
   memset( pointers, 0, sizeof( *pointers));

   if( capacity)
   {
      pointers->storage = mulle_allocator_malloc( allocator, capacity * sizeof( void *));
      pointers->size    = capacity;
   }
}


static inline void  _mulle__pointers_done( struct mulle__pointers *pointers,
                                           struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, pointers->storage);
}



static inline void  _mulle__pointers_destroy( struct mulle__pointers *pointers,
                                              struct mulle_allocator *allocator)
{
   _mulle__pointers_done( pointers, allocator);
   mulle_allocator_free( allocator, pointers);
}


# pragma mark - petty accessors

static inline size_t
   _mulle__pointers_get_count( struct mulle__pointers *pointers)
{
   return( pointers->used);
}


static inline void  **
   _mulle__pointers_get_pointers( struct mulle__pointers *pointers)
{
   return( pointers->storage);
}


# pragma mark - operations

static inline void
   _mulle__pointers_add( struct mulle__pointers *pointers,
                         void  *pointer,
                         struct mulle_allocator *allocator)
{
   void   _mulle__pointers_grow( struct mulle__pointers *pointers,
                                     struct mulle_allocator *allocator);

   if( pointers->used == pointers->size)
      _mulle__pointers_grow( pointers, allocator);

   pointers->storage[ pointers->used++] = pointer;
}


static inline void *
   _mulle__pointers_advance( struct mulle__pointers *pointers,
                             size_t length,
                             struct mulle_allocator *allocator)
{
   void   _mulle__pointers_guarantee( struct mulle__pointers *pointers,
                                      size_t length,
                                      struct mulle_allocator *allocator);
   void   **reserved;

   _mulle__pointers_guarantee( pointers, length, allocator);

   reserved      = &pointers->storage[ pointers->used];
   pointers->used += length;

   return( reserved);
}


static inline void  *
   _mulle__pointers_get( struct mulle__pointers *pointers, uintptr_t i)
{
   assert( pointers);
   assert( i < pointers->used);

   return( pointers->storage[ i]);
}


static inline void  **
   _mulle__pointers_extract( struct mulle__pointers *pointers,
                             struct mulle_allocator *allocator)
{
   void   **block;

   // reduce size to minimum
   block = mulle_allocator_realloc( allocator,
                                    pointers->storage,
                                    sizeof( void *) * pointers->used);
   // wipe original
   memset( pointers, 0, sizeof( *pointers));

   return( block);
}


static void   _mulle__pointers_reset( struct mulle__pointers *pointers)
{
   pointers->used = 0;
}


static inline uintptr_t
   _mulle__pointers_find( struct mulle__pointers *pointers, void *p)
{
   void   **curr;
   void   **sentinel;

   curr     = pointers->storage;
   sentinel = &curr[ pointers->used];
   while( curr < sentinel)
   {
      if( *curr == p)
         return( (int) (intptr_t) (curr - pointers->storage));
      curr++;
   }
   return( mulle_not_found_e);
}


#pragma mark - enumerator

#define MULLE__POINTERSENUMERATOR_BASE  \
   void   **curr;                       \
   void   **sentinel;                   \
   int    direction

struct mulle__pointersenumerator
{
   MULLE__POINTERSENUMERATOR_BASE;
};


static inline struct mulle__pointersenumerator
   mulle__pointers_enumerate( struct mulle__pointers *pointers)
{
   struct mulle__pointersenumerator   rover;

   if( ! pointers)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   rover.curr      = &pointers->storage[ 0];
   rover.sentinel  = &rover.curr[ pointers ? pointers->used : 0];
   rover.direction = +1;
   assert( rover.sentinel >= rover.curr);

   return( rover);
}



static inline void *
   _mulle__pointersenumerator_next( struct mulle__pointersenumerator *rover)
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
   _mulle__pointersenumerator_done( struct mulle__pointersenumerator *rover)
{
}


static inline void
   mulle__pointersenumerator_done( struct mulle__pointersenumerator *rover)
{
}


#define MULLE__POINTERSREVERSEENUMERATOR_BASE  MULLE__POINTERSENUMERATOR_BASE

struct mulle__pointersreverseenumerator
{
   MULLE__POINTERSREVERSEENUMERATOR_BASE;
};


static inline struct  mulle__pointersreverseenumerator
   mulle__pointers_reverseenumerate( struct mulle__pointers *pointers)
{
   struct mulle__pointersreverseenumerator   rover;

   if( ! pointers)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }
   // can't deal with pointers=nil here because we don't know notakey

   rover.sentinel  = &pointers->storage[ -1];
   rover.curr      = &rover.sentinel[ pointers ? pointers->used : 0];
   rover.direction = -1;
   assert( rover.sentinel <= rover.curr);

   return( rover);
}


static inline void *
   _mulle__pointersreverseenumerator_next( struct mulle__pointersreverseenumerator *rover)
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
   _mulle__pointersreverseenumerator_done( struct mulle__pointersreverseenumerator *rover)
{
}


static inline void
   mulle__pointersreverseenumerator_done( struct mulle__pointersreverseenumerator *rover)
{
}


#pragma mark - enumerator convenience

static inline int   mulle__pointers_member( struct mulle__pointers *pointers, void *p)
{
   struct  mulle__pointersenumerator   rover;
   void                                    *q;

   rover = mulle__pointers_enumerate( pointers);
   {
      while( (q = _mulle__pointersenumerator_next( &rover)))
         if( q == p)
            break;
   }
   mulle__pointersenumerator_done( &rover);

   return( q == p);
}

#endif
