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
// Can overwrite pointers too (and keep count)
// You can also use it as stack
//
struct mulle_pointerpairarray
{
   MULLE__POINTERPAIRARRAY_BASE;
   void                        *notakey;
   struct mulle_allocator      *allocator;
};


static inline struct mulle_pointerpairarray  *
	mulle_pointerpairarray_alloc( void *notakey,
											struct mulle_allocator *allocator)
{
   struct mulle_pointerpairarray   *array;

   array = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle_pointerpairarray));
   return( array);
}


static inline void   _mulle_pointerpairarray_init( struct mulle_pointerpairarray *array,
                                                   size_t  capacity,
                                                   void *notakey,
                                                   struct mulle_allocator *allocator)
{
   _mulle__pointerpairarray_init( (struct mulle__pointerpairarray *) array, capacity, allocator);

   array->notakey   = notakey;
   array->allocator = allocator;
}


static inline void  _mulle_pointerpairarray_done( struct mulle_pointerpairarray *array)
{
   _mulle__pointerpairarray_done( (struct mulle__pointerpairarray *) array, array->allocator);
}


static inline void  mulle_pointerpairarray_destroy( struct mulle_pointerpairarray *array)
{
   if( array)
      mulle__pointerpairarray_destroy( (struct mulle__pointerpairarray *) array, array->allocator);
}


# pragma mark - petty accessors

static inline size_t
	_mulle_pointerpairarray_get_count( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_get_count( (struct mulle__pointerpairarray *) array));
}


static inline struct mulle_allocator  *
	_mulle_pointerpairarray_get_allocator( struct mulle_pointerpairarray *array)
{
   return( array->allocator);
}


static inline void  *
	_mulle_pointerpairarray_get_notakey( struct mulle_pointerpairarray *array)
{
   return( array->notakey);
}


# pragma mark - petty functions

static inline int
   _mulle_pointerpairarray_is_full( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_is_full( (struct mulle__pointerpairarray *) array));
}


static inline int
   _mulle_pointerpairarray_can_compact( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_can_compact( (struct mulle__pointerpairarray *) array));
}


static inline int
   _mulle_pointerpairarray_needs_compaction( struct mulle_pointerpairarray *array)
{
   return( _mulle__pointerpairarray_needs_compaction( (struct mulle__pointerpairarray *) array));
}


# pragma mark - operations

static inline void   _mulle_pointerpairarray_add( struct mulle_pointerpairarray *array,
                                                  struct mulle_pointerpair pair)
{
   _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *) array,
                                 pair,
                                 array->allocator);
}


//
// this removes notakey from the back, until it finds a pointer
// then remove this
//
static inline struct mulle_pointerpair
	mulle_pointerpairarray_remove_last( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_remove_last( (struct mulle__pointerpairarray *) array,
                                                array->notakey));
}


static inline struct mulle_pointerpair
	mulle_pointerpairarray_find_last( struct mulle_pointerpairarray *array)
{
   return( mulle__pointerpairarray_remove_last( (struct mulle__pointerpairarray *) array,
                                                array->notakey));
}


static inline struct mulle_pointerpair
	_mulle_pointerpairarray_get( struct mulle_pointerpairarray *array,
                                size_t i)
{
   return( _mulle__pointerpairarray_get( (struct mulle__pointerpairarray *) array,
                                         i,
                                         array->notakey));
}


static inline void
	mulle_pointerpairarray_set( struct mulle_pointerpairarray *array,
                               size_t i,
                               struct mulle_pointerpair pair)
{
   _mulle__pointerpairarray_set( (struct mulle__pointerpairarray *) array,
                                 i,
                                 pair,
                                 array->notakey);
}


#pragma mark - enumerator

#define MULLE_POINTERPAIRARRAYENUMERATOR_BASE   MULLE__POINTERPAIRARRAYENUMERATOR_BASE

struct mulle_pointerpairarrayenumerator
{
   MULLE_POINTERPAIRARRAYENUMERATOR_BASE;
};


static inline void
   mulle_pointerpairarrayenumerator_zero( struct mulle_pointerpairarrayenumerator *rover)
{
   mulle__pointerpairarrayenumerator_zero( (struct mulle__pointerpairarrayenumerator *) rover);
}


static inline struct mulle_pointerpairarrayenumerator
	mulle_pointerpairarray_enumerate( struct mulle_pointerpairarray *array)
{
   struct mulle__pointerpairarrayenumerator   rover;

   mulle__pointerpairarrayenumerator_zero( (struct mulle__pointerpairarrayenumerator *) &rover);

   rover = mulle__pointerpairarray_enumerate( (struct mulle__pointerpairarray *) array,
                                              array->notakey);
   return( *(struct mulle_pointerpairarrayenumerator *) &rover);
}


static inline struct  mulle_pointerpairarrayenumerator
   mulle_pointerpairarray_enumerate_nil( struct mulle_pointerpairarray *array)
{
   struct mulle_pointerpairarrayenumerator   rover;

   if( ! array)
   {
      mulle_pointerpairarrayenumerator_zero( &rover);
      return( rover);
   }

   assert( ! array->notakey);
   return( mulle_pointerpairarray_enumerate( array));
}


static inline struct mulle_pointerpair
	mulle_pointerpairarrayenumerator_next( struct mulle_pointerpairarrayenumerator *rover)
{
   return( mulle__pointerpairarrayenumerator_next( (struct mulle__pointerpairarrayenumerator *) rover));
}


static inline void
	mulle_pointerpairarrayenumerator_done( struct mulle_pointerpairarrayenumerator *rover)
{
   mulle__pointerpairarrayenumerator_done( (struct mulle__pointerpairarrayenumerator *) rover);
}

#endif
