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

#include "mulle--pointerarray.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>


//
// mulle_pointerarray adds storage of notakey and allocator to
// mulle__pointerarray, otherwise its the same
//
#define MULLE_POINTERARRAY_BASE          \
   MULLE__POINTERARRAY_BASE;             \
   void                     *notakey;   \
   struct mulle_allocator   *allocator


struct mulle_pointerarray
{
   MULLE_POINTERARRAY_BASE;
};


static inline struct mulle_pointerarray  *
	mulle_pointerarray_alloc( struct mulle_allocator *allocator)
{
   struct mulle_pointerarray   *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle_pointerarray));
   return( array);
}


static inline void   _mulle_pointerarray_init( struct mulle_pointerarray *array,
                                               size_t  capacity,
                                               void *notakey,
                                               struct mulle_allocator *allocator)
{
   _mulle__pointerarray_init( (struct mulle__pointerarray *) array,
                              capacity,
                              allocator);

   array->allocator = allocator;
   array->notakey   = notakey;
}


// notakey = NULL
// capacity = 0
struct mulle_pointerarray *
      mulle_pointerarray_create_nil( struct mulle_allocator *allocator);


static inline void  _mulle_pointerarray_done( struct mulle_pointerarray *array)
{
   _mulle__pointerarray_done( (struct mulle__pointerarray *) array,
                              array->allocator);
}


static inline void  mulle_pointerarray_done( struct mulle_pointerarray *array)
{
   if( array)
      _mulle__pointerarray_done( (struct mulle__pointerarray *) array,
                                 array->allocator);
}


static inline void  mulle_pointerarray_destroy( struct mulle_pointerarray *array)
{
   if( array)
      mulle__pointerarray_destroy( (struct mulle__pointerarray *) array,
                                   array->allocator);
}


# pragma mark - petty accessors

static inline size_t
	_mulle_pointerarray_get_count( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle_pointerarray_get_count( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_get_count( (struct mulle__pointerarray *) array));
}


static inline size_t
   _mulle_pointerarray_get_used( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_get_used( (struct mulle__pointerarray *) array));
}


static inline size_t
   mulle_pointerarray_get_used( struct mulle_pointerarray *array)
{
   return( mulle__pointerarray_get_used( (struct mulle__pointerarray *) array));
}



static inline int
   _mulle_pointerarray_needs_compaction( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_needs_compaction( (struct mulle__pointerarray *) array));
}


static inline struct mulle_allocator  *
	_mulle_pointerarray_get_allocator( struct mulle_pointerarray *array)
{
   return( array->allocator);
}


static inline void  *
	_mulle_pointerarray_get_notakey( struct mulle_pointerarray *array)
{
   return( array->notakey);
}


# pragma mark - operations

static inline void
	_mulle_pointerarray_add( struct mulle_pointerarray *array, void *p)
{
   _mulle__pointerarray_add( (struct mulle__pointerarray *) array,
                              p,
                              array->notakey,
                              array->allocator);
}


static inline void
   mulle_pointerarray_add( struct mulle_pointerarray *array, void  *p)
{
   if( ! array)
      return;
   _mulle_pointerarray_add( array, p);
}


static inline void  *
	_mulle_pointerarray_remove_last( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_remove_last( (struct mulle__pointerarray *) array,
                                             array->notakey));
}


static inline void  *
	_mulle_pointerarray_find_last( struct mulle_pointerarray *array)
{
   return( _mulle__pointerarray_find_last( (struct mulle__pointerarray *) array,
                                           array->notakey));
}


static inline void  *
	_mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}


static inline void  *
   mulle_pointerarray_get( struct mulle_pointerarray *array, size_t i)
{
   return( _mulle__pointerarray_get( (struct mulle__pointerarray *) array, i));
}



static inline uintptr_t
	_mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
{
   return( _mulle__pointerarray_find( (struct mulle__pointerarray *) array,
                                      p));
}


static inline uintptr_t
   mulle_pointerarray_find( struct mulle_pointerarray *array, void *p)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle_pointerarray_find( array, p));
}



static inline void   *_mulle_pointerarray_set( struct mulle_pointerarray *array,
                                               size_t i,
                                               void *p)
{
   return( _mulle__pointerarray_set( (struct mulle__pointerarray *) array,
                                     i,
                                     p,
                                     array->notakey));

}


static inline void   *mulle_pointerarray_set( struct mulle_pointerarray *array,
                                              size_t i,
                                              void *p)
{
   if( ! array || i >= array->used)
      return( array->notakey);
   return( _mulle_pointerarray_set( array, i, p));
}



static inline void
   _mulle_pointerarray_reset( struct mulle_pointerarray *array)
{
   _mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


static inline void
   mulle_pointerarray_reset( struct mulle_pointerarray *array)
{
   mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


#pragma mark - enumerator

#define MULLE_POINTERARRAYENUMERATOR_BASE MULLE__POINTERARRAYENUMERATOR_BASE

struct mulle_pointerarrayenumerator
{
   MULLE_POINTERARRAYENUMERATOR_BASE;
};

//
// still worth inlining especially if you check before! that
// array must be != nil
// should prefix mulle_pointerarray_enumerate with an underscore
//
static inline struct mulle_pointerarrayenumerator
   _mulle_pointerarray_enumerate( struct mulle_pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_enumerate( (struct mulle__pointerarray *) array,
                                           array->notakey);
   return( *(struct mulle_pointerarrayenumerator *) &rover);
}


static inline struct  mulle_pointerarrayenumerator
   mulle_pointerarray_enumerate_nil( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }

   return( _mulle_pointerarray_enumerate( array));
}


static inline struct  mulle_pointerarrayenumerator
   _mulle_pointerarray_reverseenumerate( struct mulle_pointerarray *array)
{
   struct mulle__pointerarrayenumerator   rover;

   rover = _mulle__pointerarray_reverseenumerate( (struct mulle__pointerarray *) array,
                                                  array->notakey);
   return( *(struct mulle_pointerarrayenumerator *) &rover);
}


// use this if array->not_a_key is known to be null
static inline struct  mulle_pointerarrayenumerator
   mulle_pointerarray_reverseenumerate_nil( struct mulle_pointerarray *array)
{
   struct mulle_pointerarrayenumerator   rover;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }
   assert( ! array->notakey);
   return( _mulle_pointerarray_reverseenumerate( array));
}


static inline void   *
	_mulle_pointerarrayenumerator_next( struct mulle_pointerarrayenumerator *rover)
{
   return( _mulle__pointerarrayenumerator_next( (struct mulle__pointerarrayenumerator *) rover));
}


static inline void
	_mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
{
}


static inline void
   mulle_pointerarrayenumerator_done( struct mulle_pointerarrayenumerator *rover)
{
}



#pragma mark - enumerator convenience

static inline int   mulle_pointerarray_member( struct mulle_pointerarray *array, void *p)
{
   return( mulle__pointerarray_member( (struct mulle__pointerarray *) array, p, array->notakey));
}

#endif
