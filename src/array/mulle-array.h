//
//  mulle-array.h
//  mulle-container
//
//  Created by Nat! on 04/11/15.
//  Copyright (c) 2015 Nat! - Mulle kybernetiK.
//  Copyright (c) 2015 Codeon GmbH.
//  All rights reserved.
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

#ifndef mulle_array__h__
#define mulle_array__h__

#include "mulle--array.h"
#include "mulle-container-operation.h"


// mulle-array is a mulle--array with the allocator and callback embedded

#define MULLE_ARRAY_BASE                            \
   MULLE__ARRAY_BASE;                               \
   struct mulle_container_keycallback   *callback; \
   struct mulle_allocator               *allocator


struct mulle_array
{
   MULLE_ARRAY_BASE;
};


struct mulle_array    *mulle_array_create( struct mulle_container_keycallback *callback,
                                           struct mulle_allocator *allocator);


static inline void   mulle_array_destroy( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_destroy( (struct mulle__array *) array,
                          array->callback,
                          array->allocator);
}


static inline void   mulle_array_done( struct mulle_array *array)
{
   if( ! array)
      return;
   _mulle__array_done( (struct mulle__array *) array,
                       array->callback,
                       array->allocator);
}


# pragma mark - petty accessors

MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_array_get_count( struct mulle_array *array)
{
   return( _mulle__array_get_count( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   mulle_array_get_count( struct mulle_array *array)
{
   return( mulle__array_get_count( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   _mulle_array_get_used( struct mulle_array *array)
{
   return( _mulle__array_get_used( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline unsigned int
   mulle_array_get_used( struct mulle_array *array)
{
   return( mulle__array_get_used( (struct mulle__array *) array));
}


MULLE_C_NONNULL_FIRST
static inline int
   _mulle_array_needs_compaction( struct mulle_array *array)
{
   return( _mulle__array_needs_compaction( (struct mulle__array *) array));
}




// TODO: update this to use mulle-range
static inline void   mulle_array_remove_in_range( struct mulle_array *array,
                                                  struct mulle_range range)
{
   if( ! array)
      return;

   _mulle__array_remove_in_range( (struct mulle__array *) array,
                                  range,
                                  array->callback,
                                  array->allocator);
}


static inline void   mulle_array_compact( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_compact( (struct mulle__array *) array, array->callback);
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_array_reset( struct mulle_array *array)
{
   _mulle__array_reset( (struct mulle__array *) array,
                        array->callback,
                        array->allocator);
}


static inline void   mulle_array_reset( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle_array_reset( array);
}


static inline void    mulle_array_init( struct mulle_array *array,
                                        unsigned int length,
                                        struct mulle_container_keycallback *callback,
                                        struct mulle_allocator *allocator)
{
   if( ! array || ! callback)
      return;

   _mulle__array_init( (struct mulle__array *) array, length, allocator);

   array->callback  = callback;
   array->allocator = allocator;
}

// will use callbacks of array to determine equality
static inline int   mulle_array_is_equal( struct mulle_array *array,
                                          struct mulle_array *other)
{
   if( ! array || ! other)
      return( array == other);

   assert( array->callback->is_equal == other->callback->is_equal);
   return( _mulle__array_is_equal( (struct mulle__array *) array,
                                   (struct mulle__array *) other,
                                   array->callback));
}


static inline void    mulle_array_add( struct mulle_array *array,
                                       void  *p)
{
   if( ! array)
      return;

   _mulle__array_add( (struct mulle__array *) array,
                      p,
                      array->callback,
                      array->allocator);
}


static inline void   *mulle_array_get( struct mulle_array *array, unsigned int index)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_get( (struct mulle__array *) array, index));
}


static inline void   *mulle_array_find_last( struct mulle_array *array)
{
   if( ! array)
      return( NULL);

   return( _mulle__array_find_last( (struct mulle__array *) array, array->callback));
}


static inline void   mulle_array_remove_last( struct mulle_array *array)
{
   if( ! array)
      return;

   _mulle__array_remove_last( (struct mulle__array *) array,
                              array->callback);
}


#pragma mark - search

static inline uintptr_t
   mulle_array_find_in_range_identical( struct mulle_array *array,
                                        void *obj,
                                        struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range_identical( (struct mulle__array *) array,
                                                   obj,
                                                   range));
}


static inline uintptr_t
   mulle_array_find_in_range( struct mulle_array *array,
                              void *obj,
                              struct mulle_range range)
{
   if( ! array)
      return( mulle_not_found_e);
   return( _mulle__array_find_in_range( (struct mulle__array *) array,
                                        obj,
                                        range,
                                        array->callback));
}


#pragma mark - enumeration

#define MULLE_ARRAYENUMERATOR_BASE   MULLE__ARRAYENUMERATOR_BASE


struct mulle_arrayenumerator
{
   MULLE_ARRAYENUMERATOR_BASE;
};



static inline struct mulle_arrayenumerator
   mulle_array_enumerate( struct mulle_array *array)
{
   struct mulle_arrayenumerator    rover;
   struct mulle__arrayenumerator   tmp;

   if( ! array)
   {
      memset( &rover, 0, sizeof( rover));
      return( rover);
   }
   tmp = _mulle__array_enumerate( (struct mulle__array *) array, array->callback);
   return( *(struct mulle_arrayenumerator *) &tmp);
}


static inline struct mulle_arrayenumerator
   _mulle_array_reverseenumerate( struct mulle_array *array,
                                  struct mulle_container_keycallback *callback)
{
   struct mulle__arrayenumerator   rover;

   rover = _mulle__array_reverseenumerate( (struct mulle__array *) array, callback);
   return( *(struct mulle_arrayenumerator *) &rover);
}


MULLE_C_NONNULL_FIRST
static inline void   *
   _mulle_arrayenumerator_next( struct mulle__arrayenumerator *rover)
{
   return( _mulle__arrayenumerator_next( (struct mulle__arrayenumerator *) rover));
}


MULLE_C_NONNULL_FIRST
static inline void   _mulle_arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}

static inline void   mulle_arrayenumerator_done( struct mulle__arrayenumerator *rover)
{
}


#endif /* mulle_array_h */


