//  _mulle_array.c
//  mulle-container
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

#include "mulle--array.h"

#include "include-private.h"

#include "mulle-container-operation.h"
#include "mulle-container-callback.h"
#include "mulle-container-math.h"

#include <stdio.h>  // debug


struct mulle__arrayenumerator          mulle__arrayenumerator_empty;
struct mulle__arrayreverseenumerator   mulle__arrayreverseenumerator_empty;

#pragma mark - _mulle_arrayrange internal helper

MULLE_C_NONNULL_FIRST_THIRD
static inline void
   _mulle__array_release( struct mulle__array *array,
                          struct mulle_range range,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   void   **p;
   void   **sentinel;

   p        = &array->storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
      (*callback->release)( callback, *p++, allocator);
}


#pragma mark - creation and destruction

struct mulle__array   *mulle__array_create( struct mulle_allocator *allocator)
{
   struct mulle__array  *array;

   array = mulle_allocator_malloc( allocator, sizeof( struct mulle__array));
   _mulle__array_init( array, 0, allocator);
   return( array);
}

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_done( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   _mulle__array_release( array, mulle_range_make( 0, _mulle__array_get_count( array)), callback, allocator);
   _mulle__pointerarray_done( (struct mulle__pointerarray *) array, allocator);
}

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_destroy( struct mulle__array *array,
                              struct mulle_container_keycallback *callback,
                              struct mulle_allocator *allocator)
{
   _mulle__array_done( array, callback, allocator);
   mulle_allocator_free( allocator, array);
}


#pragma mark - search

//
// range must have been checked already
//
MULLE_C_NONNULL_FIRST_FOURTH
uintptr_t   _mulle__array_find_in_range( struct mulle__array *array,
                                         void *obj,
                                         struct mulle_range range,
                                         struct mulle_container_keycallback *callback)
{
   size_t   i, n;
   void     **p;
   void     **sentinel;

   range = mulle_range_validate_against_length( range, _mulle__array_get_count( array));

   // quick check for first 32 pointers
   n = range.length;
   if( n > 32)
      n = 32;
   n += range.location;

   assert( n >= range.location);

   p        = array->storage[ range.location];
   sentinel = &p[ n];
   while( p < sentinel)
   {
      if( *p == obj)
         return( p - array->storage);
      ++p;
   }

   p        = array->storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
   {
      if( (*callback->is_equal)( callback, obj, *p))
         return( p - array->storage);
      ++p;
   }

   return( mulle_not_found_e);
}


#pragma mark - removal

MULLE_C_NONNULL_FIRST_SECOND
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   if( callback->release != mulle_container_keycallback_nop)
      _mulle__array_release( array, mulle_range_make( 0, _mulle__array_get_count( array)), callback, allocator);

   _mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


MULLE_C_NONNULL_FIRST_THIRD
void   _mulle__array_remove_in_range( struct mulle__array *array,
                                      struct mulle_range range,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   range = mulle_range_validate_against_length( range, _mulle__array_get_count( array));
   _mulle__array_release( array, range, callback, allocator);

   _mulle__pointerarray_remove_in_range( (struct mulle__pointerarray *) array,
                                         range);
}

MULLE_C_NONNULL_FIRST_SECOND_THIRD
int    _mulle__array_is_equal( struct mulle__array *array,
                               struct mulle__array *other,
                               struct mulle_container_keycallback *callback)
{
   size_t   i, n;
   void     **p;
   void     **q;

   n = _mulle__array_get_count( array);
   if( n != _mulle__array_get_count( other))
      return( 0);

   if( callback->is_equal == mulle_container_keycallback_pointer_is_equal)
      return( ! memcmp( array->storage, other->storage, n * sizeof( void *)));

   p = array->storage;
   q = other->storage;
   for( i = 0; i < n; i++)
   {
      if( ! (callback->is_equal)( callback, *p, *q))
         return( 0);
   }

   return( 1);
}


MULLE_C_NONNULL_FIRST_FOURTH
void    _mulle__array_set( struct mulle__array *array,
                           size_t i,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   void   *old;

   p = (*callback->retain)( callback, p, allocator);
   old = _mulle__pointerarray_set( (struct mulle__pointerarray *)  array,
                                    i,
                                    p);
   (*callback->release)( callback, old, allocator);
}
