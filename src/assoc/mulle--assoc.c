//  _mulle_assoc.c
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

#include "mulle--assoc.h"

#include "include-private.h"

#include "mulle-container-operation.h"
#include "mulle-container-callback.h"
#include "mulle-container-math.h"

#include <stdio.h>  // debug


#pragma mark - _mulle_assocrange internal helper

MULLE_C_NONNULL_FIRST
static inline void
   _mulle__assoc_release( struct mulle__assoc *assoc,
                          struct mulle_range range,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! callback || ! mulle_container_keyvaluecallback_releases( callback))
      return;

   p        = &assoc->_storage[ range.location];
   sentinel = &p[ range.length];
   while( p < sentinel)
      mulle_pointerpair_release( *p++, callback, allocator);
}


#pragma mark - creation and destruction


void   _mulle__assoc_done( struct mulle__assoc *assoc,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   _mulle__assoc_release( assoc, mulle_range_make( 0, _mulle__assoc_get_count( assoc)), callback, allocator);
   _mulle__pointerpairarray_done( (struct mulle__pointerpairarray *) assoc, allocator);
}



void   _mulle__assoc_destroy( struct mulle__assoc *assoc,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator)
{
   _mulle__assoc_done( assoc, callback, allocator);
   mulle_allocator_free( allocator, assoc);
}



#pragma mark - removal

void   _mulle__assoc_reset( struct mulle__assoc *assoc,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle__assoc_release( assoc, mulle_range_make( 0, _mulle__assoc_get_count( assoc)), callback, allocator);

   mulle__pointerpairarray_reset( (struct mulle__pointerpairarray *) assoc);
}


void   _mulle__assoc_remove_in_range( struct mulle__assoc *assoc,
                                      struct mulle_range range,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   range = mulle_range_validate_against_length( range, _mulle__assoc_get_count( assoc));
   _mulle__assoc_release( assoc, range, callback, allocator);

   _mulle__pointerpairarray_remove_in_range( (struct mulle__pointerpairarray *) assoc,
                                             range);
}


void   _mulle__assoc_remove( struct mulle__assoc *assoc,
                             void *key,
                             struct mulle_container_keyvaluecallback *callback,
                             struct mulle_allocator *allocator)
{
   unsigned int               i;
   struct mulle_pointerpair   item;

   //
   // Removal back to front is cool, as long as we are the only one
   // modifying the assoc. We don't use an enumerator here, but an index is
   // safe...
   //
   if( _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      for( i = mulle__assoc_get_count( assoc); i;)
      {
         item = mulle__assoc_get_at_index( assoc, --i);
         if( key == item.key)
            _mulle__assoc_remove_in_range( assoc,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
   else
   {
      for( i = mulle__assoc_get_count( assoc); i;)
      {
         item = mulle__assoc_get_at_index( assoc, --i);
         if( (callback->keycallback.is_equal)( &callback->keycallback, key, item.key))
            _mulle__assoc_remove_in_range( assoc,
                                           mulle_range_make( i, 1),
                                           callback,
                                           allocator);
      }
   }
}


int    _mulle__assoc_is_equal( struct mulle__assoc *assoc,
                               struct mulle__assoc *other,
                               struct mulle_container_keyvaluecallback *callback)
{
   unsigned int               i, n;
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *q;

   n = _mulle__assoc_get_count( assoc);
   if( n != _mulle__assoc_get_count( other))
      return( 0);

   if( _mulle_container_keycallback_isbitequals( &callback->keycallback))
      return( ! memcmp( assoc->_storage, other->_storage, n * sizeof( struct mulle_pointerpair)));

   p = assoc->_storage;
   q = other->_storage;
   for( i = 0; i < n; i++)
   {
      if( ! (callback->keycallback.is_equal)( &callback->keycallback, p->key, q->key))
         return( 0);
   }

   return( 1);
}


void    _mulle__assoc_add( struct mulle__assoc *assoc,
                           void *key,
                           void *value,
                           struct mulle_container_keyvaluecallback *callback,
                           struct mulle_allocator *allocator)
{
   struct mulle_pointerpair  pair;
   struct mulle_pointerpair  retained;

   pair     = mulle_pointerpair_make( key, value);
   retained = mulle_pointerpair_retain( pair, callback, allocator);
   _mulle__pointerpairarray_add( (struct mulle__pointerpairarray *)  assoc,
                                  retained,
                                  allocator);
}


void    _mulle__assoc_set_at_index( struct mulle__assoc *assoc,
                                    unsigned int i,
                                    void *key,
                                    void *value,
                                    struct mulle_container_keyvaluecallback *callback,
                                    struct mulle_allocator *allocator)
{
   struct mulle_pointerpair  retained;
   struct mulle_pointerpair  pair;
   struct mulle_pointerpair  old;

   assert( key != callback->keycallback.notakey);

   pair     = mulle_pointerpair_make( key, value);
   retained = mulle_pointerpair_retain( pair, callback, allocator);
   old      = _mulle__pointerpairarray_set( (struct mulle__pointerpairarray *)  assoc,
                                            i,
                                            retained);
   mulle_pointerpair_release( old, callback, allocator);
}


void
   mulle__assoc_add_assoc_range( struct mulle__assoc *assoc,
                                 struct mulle__assoc *other,
                                 struct mulle_range range,
                                 struct mulle_container_keyvaluecallback *callback,
                                 struct mulle_allocator *allocator)
{
   unsigned int               count;
   struct mulle_pointerpair   *q;
   struct mulle_pointerpair   *sentinel;
   uintptr_t                  i;
   struct mulle_range         all;

   assert( callback);
   if( ! assoc || ! callback)
      return;

   count = mulle__assoc_get_count( other);
   range = mulle_range_validate_against_length( range, count);
   if( ! range.length)
      return;

   all      = mulle_range_make( 0, mulle__assoc_get_count( assoc));
   q        = &other->_storage[ range.location];
   sentinel = &q[ range.length];
   while( q < sentinel)
   {
      i = _mulle_pointerpair_find_in_range_callback( assoc->_storage,
                                                     *q,
                                                     all,
                                                     callback);      
      if( i == mulle_not_found_e)
      {
         _mulle__assoc_add( assoc, q->key, q->value, callback, allocator);
         all.length++;
      }
      ++q;
   }
}


void _mulle__assoc_copy_items( struct mulle__assoc *dst,
                               struct mulle__assoc *src,
                               struct mulle_container_keyvaluecallback *callback,
                               struct mulle_allocator *allocator)
{
   unsigned int              n;
   struct mulle_pointerpair  *p;
   struct mulle_pointerpair  *sentinel;

   n = mulle__assoc_get_count( src);
   if( ! n)
      return;

   assert( dst != src);

   _mulle__pointerpairarray_guarantee( (struct mulle__pointerpairarray *) dst, n, allocator);

   p        = dst->_curr;
   sentinel = dst->_curr + n;

   memcpy( p, src->_storage, n * sizeof( struct mulle_pointerpair));

   if( mulle_container_keyvaluecallback_retains( callback))
   {  
      while( p < sentinel)
      {
         *p = mulle_pointerpair_retain( *p, callback, allocator);
         ++p;
      }
   }

   dst->_curr = sentinel;
}



// use this only for debugging
char   *_mulle__assoc_describe( struct mulle__assoc *set,
                                struct mulle_container_keyvaluecallback *callback,
                                struct mulle_allocator *allocator)
{
   char                            *result;
   char                            *key;
   char                            *value;
   int                             separate;
   size_t                          len;
   size_t                          key_len;
   size_t                          value_len;
   struct mulle__assocenumerator   rover;
   struct mulle_pointerpair        item;
   struct mulle_allocator          *key_allocator;
   struct mulle_allocator          *value_allocator;

   result = NULL;
   len    = 0;
   rover = mulle__assoc_enumerate( set, callback);
   while( _mulle__assocenumerator_next( &rover, &item.key, &item.value))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key       = (*callback->keycallback.describe)( &callback->keycallback,
                                                     item.key,
                                                     &key_allocator);
      key_len   = strlen( key);

      value     = (*callback->valuecallback.describe)( &callback->valuecallback,
                                                       item.value,
                                                       &value_allocator);
      value_len = strlen( value);

      separate  = result != NULL;
      result    = mulle_allocator_realloc( allocator,
                                           result,
                                           len + (separate * 2) + 2 + key_len + 1 \
                                            + 2 + value_len + 2 + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len   += 2;
      }

      memcpy( &result[ len], "{ ", 2);
      len   += 2;

      memcpy( &result[ len], key, key_len);
      len += key_len;

      memcpy( &result[ len], ", ", 2);
      len   += 2;

      memcpy( &result[ len], value, value_len);
      len += value_len;

      memcpy( &result[ len], " }", 2);
      len   += 2;

      if( value_allocator)
         mulle_allocator_free( value_allocator, value);

      if( key_allocator)
         mulle_allocator_free( key_allocator, key);
   }
   mulle__assocenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


uintptr_t
   mulle__assoc_find_in_range( struct mulle__assoc *assoc,
                               void *key,
                               struct mulle_range range,
                               struct mulle_container_keyvaluecallback *callback)
{
   struct mulle_pointerpair   *p;
   struct mulle_pointerpair   *sentinel;

   if( ! assoc)
      return( mulle_not_found_e);

   range = mulle_range_validate_against_length( range,
                                                _mulle__assoc_get_count( assoc));

   assert( key != mulle_pointerpair_notakey);

   p        = &assoc->_storage[ range.location];
   sentinel = &p[ range.length];

   // NULL check needed for remap
   if( ! callback || _mulle_container_keycallback_isbitequals( &callback->keycallback))
   {
      for( ; p < sentinel; p++)
         if( p->key == key)
            return( p - assoc->_storage);
   }
   else
   {
      for( ; p < sentinel; p++)
         if( (*callback->keycallback.is_equal)( &callback->keycallback, p->key, key))
            return( p - assoc->_storage);
   }
   return( mulle_not_found_e);
}


int   mulle__assoc_member( struct mulle__assoc *assoc,
                           void *key,
                           struct mulle_container_keyvaluecallback *callback)
{
   uintptr_t   result;

   result = mulle__assoc_find_callback( assoc, key, callback);
   return( result != mulle_not_found_e);
}
