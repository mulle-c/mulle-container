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


#pragma mark - _mulle_arrayrange internal helper

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__array_release( struct mulle__array *array,
                          struct mulle_range range,
                          struct mulle_container_keycallback *callback,
                          struct mulle_allocator *allocator)
{
   void   **p;
   void   **sentinel;

   p        = &array->_storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
      (*callback->release)( callback, *p++, allocator);
}


#pragma mark - creation and destruction

void   _mulle__array_done( struct mulle__array *array,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   _mulle__array_release( array, mulle_range_make( 0, _mulle__array_get_count( array)), callback, allocator);
   _mulle__pointerarray_done( (struct mulle__pointerarray *) array, allocator);
}


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
uintptr_t   _mulle__array_find_in_range( struct mulle__array *array,
                                         void *obj,
                                         struct mulle_range range,
                                         struct mulle_container_keycallback *callback)
{
   void   **p;
   void   **sentinel;

   range = mulle_range_validate_against_length( range, _mulle__array_get_count( array));

   // quick check for first 32 pointers
   p        = &array->_storage[ range.location];
   sentinel = &p[ range.length < 32 ? range.length : 32];
   while( p < sentinel)
   {
      if( *p == obj)
         return( p - array->_storage);
      ++p;
   }

   p        = &array->_storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
   {
      if( (*callback->is_equal)( callback, obj, *p))
         return( p - array->_storage);
      ++p;
   }

   return( mulle_not_found_e);
}


#pragma mark - removal

// function should work for not properly initialized with mulle__array_init
void   _mulle__array_reset( struct mulle__array *array,
                            struct mulle_container_keycallback *callback,
                            struct mulle_allocator *allocator)
{
   if( _mulle_container_keycallback_releases( callback))
      _mulle__array_release( array, mulle_range_make( 0, _mulle__array_get_count( array)), callback, allocator);

   _mulle__pointerarray_reset( (struct mulle__pointerarray *) array);
}


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


void   _mulle__array_remove( struct mulle__array *array,
                             void *p,
                             struct mulle_container_keycallback *callback,
                             struct mulle_allocator *allocator)
{
   unsigned int  i;
   void          *item;

   //
   // Removal back to front is cool, as long as we are the only one
   // modifying the array. We don't use an enumerator here, but an index is
   // safe...
   //
   if( callback->is_equal == mulle_container_keycallback_pointer_is_equal)
   {
      for( i = mulle__array_get_count( array); i;)
      {
         item = mulle__array_get( array, --i);
         if( p == item)
            _mulle__array_remove_in_range( array,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
   else
   {
      for( i = mulle__array_get_count( array); i;)
      {
         item = mulle__array_get( array, --i);
         if( (callback->is_equal)( callback, p, item))
            _mulle__array_remove_in_range( array,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
}


int    _mulle__array_is_equal( struct mulle__array *array,
                               struct mulle__array *other,
                               struct mulle_container_keycallback *callback)
{
   unsigned int   i, n;
   void     **p;
   void     **q;

   n = _mulle__array_get_count( array);
   if( n != _mulle__array_get_count( other))
      return( 0);

   if( callback->is_equal == mulle_container_keycallback_pointer_is_equal)
      return( ! memcmp( array->_storage, other->_storage, n * sizeof( void *)));

   p = array->_storage;
   q = other->_storage;
   for( i = 0; i < n; i++)
   {
      if( ! (callback->is_equal)( callback, *p, *q))
         return( 0);
      p++;
      q++;
   }

   return( 1);
}


void    _mulle__array_add( struct mulle__array *array,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   assert( callback);
   assert( p != callback->notakey);

   p = (*callback->retain)( callback, p, allocator);
   _mulle__pointerarray_add( (struct mulle__pointerarray *)  array,
                             p,
                             allocator);
}


void    _mulle__array_set( struct mulle__array *array,
                           unsigned int i,
                           void  *p,
                           struct mulle_container_keycallback *callback,
                           struct mulle_allocator *allocator)
{
   void   *old;

   assert( callback);
   assert( p != callback->notakey);

   p = (*callback->retain)( callback, p, allocator);
   old = _mulle__pointerarray_set( (struct mulle__pointerarray *)  array,
                                    i,
                                    p);
   (*callback->release)( callback, old, allocator);
}



void   _mulle__array_add_array( struct mulle__array *array,
                                struct mulle__array *other,
                                struct mulle_range range,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator)
{
   unsigned int   count;
   void     **q;
   void     **sentinel;

   assert( callback);
   assert( array);

   count = mulle__array_get_count( other);
   range = mulle_range_validate_against_length( range, count);
   if( ! range.length)
      return;

   _mulle__pointerarray_guarantee( (struct mulle__pointerarray *) array, range.length, allocator);

   q        = &other->_storage[ range.location];
   sentinel = &q[ range.length];
   while( q < sentinel)
      _mulle__array_add_guaranteed( array, *q++, callback, allocator);
}


void _mulle__array_copy_items( struct mulle__array *dst,
                               struct mulle__array *src,
                               struct mulle_container_keycallback *callback,
                               struct mulle_allocator *allocator)
{
   unsigned int   n;
   void           **p;
   void           **sentinel;

   n = mulle__array_get_count( src);
   if( ! n)
      return;

   assert( dst != src);

   _mulle__array_guarantee( dst, n, allocator);

   p        = dst->_curr;
   sentinel = dst->_curr + n;

   memcpy( p, src->_storage, n * sizeof( void *));

   if( _mulle_container_keycallback_retains( callback))
      while( p < sentinel)
      {
         *p = (*callback->retain)( callback, *p, allocator);
         ++p;
      }

   dst->_curr = sentinel;
}


// we don't have mulle-buffer here
// use this only for debugging
char   *_mulle__array_describe( struct mulle__array *array,
                                struct mulle_container_keycallback *callback,
                                struct mulle_allocator *allocator)
{
   char                     *result;
   char                     *key;
   int                      separate;
   size_t                   len;
   size_t                   key_len;
   void                     *item;
   struct mulle_allocator   *key_allocator;

   result = NULL;
   len    = 0;
   mulle__array_for( array, callback, item)
   {
      key_allocator  = allocator ? allocator : &mulle_default_allocator;

      // key_allocator will be overwritten if returned key needs to be
      // freed
      key        = (*callback->describe)( callback, item, &key_allocator);
      key_len    = strlen( key);
      separate   = result != NULL;

      result     = mulle_allocator_realloc( allocator, result, len + (separate * 2) + key_len + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len   += 2;
      }

      memcpy( &result[ len], key, key_len);
      len += key_len;

      if( allocator)
         mulle_allocator_free( key_allocator, key);
   }

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


int   mulle__array_member( struct mulle__array *array,
                           void *p,
                           struct mulle_container_keycallback *callback)
{
   void   *q;
   int    rval;

   rval  = 0;
   mulle__array_for( array, callback, q)
   {
      if( callback->is_equal( callback, q, p))
      {
         rval = 1;
         break;
      }
   }
   return( rval);
}
