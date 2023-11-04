//
//  mulle-container
//
//  Created by Nat! on 03/11/15.
//  Copyright (C) 2011 Nat!, Mulle kybernetiK. All rights reserved.
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
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle--map.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


#define _MULLE_MAP_INITIAL_SIZE  4

// static const void   *dummy_notakey_storage[ _MULLE_MAP_INITIAL_SIZE * 2];

void   _mulle__map_init( struct mulle__map *p,
                         unsigned int capacity,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   assert_mulle_container_keyvaluecallback( callback);

   memset( p, 0, sizeof( *p));

   //
   // our map requires zeroes to find an end so give it ~25% holes
   //
   if( capacity)
   {
      p->_size    = capacity >= MULLE__POINTERMAP_MIN_SIZE
                        ? mulle_pow2round( capacity + (capacity >> MULLE__POINTERMAP_FILL_SHIFT))
                        : MULLE__POINTERMAP_INITIAL_SIZE;
      p->_storage = mulle__pointermap_allocate_storage_generic( p->_size,
                                                               callback->keycallback.notakey,
                                                               allocator);
   }
}


struct mulle__map   *_mulle__map_create( unsigned int capacity,
                                         size_t extra,
                                         struct mulle_container_keyvaluecallback *callback,
                                         struct mulle_allocator *allocator)
{
   struct mulle__map   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__map) + extra);
   _mulle__map_init( p, capacity, callback, allocator);
   return( p);
}


static inline void _mulle__map_free_storage( struct mulle__map *map,
                                             struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, map->_storage);
}


void   _mulle__map_release_all( struct mulle__map *map,
                                struct mulle_container_keyvaluecallback *callback,
                                struct mulle_allocator *allocator)
{
   struct mulle__mapenumerator   rover;
   struct mulle_pointerpair      *pair;

   //
   // if keycallback and valuecallback are nop, we can just skip this
   //
   if( callback && mulle_container_keyvaluecallback_releases( callback))
   {
      rover = mulle__map_enumerate( map, callback);
      while( pair = _mulle__mapenumerator_next_pair( &rover))
      {
         (callback->keycallback.release)( &callback->keycallback, pair->key, allocator);
         (callback->valuecallback.release)( &callback->valuecallback, pair->value, allocator);
      }
      mulle__mapenumerator_done( &rover);
   }
}


void   _mulle__map_done( struct mulle__map *map,
                         struct mulle_container_keyvaluecallback *callback,
                         struct mulle_allocator *allocator)
{
   _mulle__map_release_all( map, callback, allocator);
   _mulle__map_free_storage( map, allocator);
#ifdef DEBUG   
   memset( map, 0xFD, sizeof( struct mulle__map));
#endif    
}


void   _mulle__map_destroy( struct mulle__map *map,
                            struct mulle_container_keyvaluecallback *callback,
                            struct mulle_allocator *allocator)
{
   _mulle__map_done( map, callback, allocator);
   mulle_allocator_free( allocator, map);
}


void   _mulle__map_reset( struct mulle__map *map,
                          struct mulle_container_keyvaluecallback *callback,
                          struct mulle_allocator *allocator)
{
   // this is good because it allows reset of empty uninitialized maps and
   // avoids superflouse free/dealloc pair
   if( map->_count)
   {
      _mulle__map_done( map, callback, allocator);
      _mulle__map_init( map, 0, callback, allocator);
   }
}



void   _mulle__map_insert_key_valuesv( struct mulle__map *map,
                                       void *firstkey,
                                       va_list args,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.key = firstkey;
   while( pair.key != callback->keycallback.notakey)
   {
      pair.value = va_arg( args, void *);
      _mulle__map_insert_pair( map, &pair, callback, allocator);

      pair.key   = va_arg( args, void *);
   }
}


#pragma mark - copying


struct mulle__map   *_mulle__map_copy( struct mulle__map *set,
                                       struct mulle_container_keyvaluecallback *callback,
                                       struct mulle_allocator *allocator)
{
   struct mulle__map   *other;

   other = _mulle__map_create( _mulle__map_get_count( set), 0, callback, allocator);
   _mulle__map_copy_items( other, set, callback, allocator);
   return( other);
}


//
// use this only for debugging
//
char   *_mulle__map_describe( struct mulle__map *set,
                              struct mulle_container_keyvaluecallback *callback,
                              struct mulle_allocator *allocator)
{
   char                          *result;
   char                          *key;
   char                          *value;
   int                           separate;
   unsigned int                  len;
   size_t                        key_len;
   size_t                        value_len;
   struct mulle__mapenumerator   rover;
   struct mulle_pointerpair      *item;
   struct mulle_allocator        *key_allocator;
   struct mulle_allocator        *value_allocator;

   result = NULL;
   len    = 0;
   rover = mulle__map_enumerate( set, callback);
   while( item = _mulle__mapenumerator_next_pair( &rover))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key        = (*callback->keycallback.describe)( &callback->keycallback, item->key, &key_allocator);
      key_len    = strlen( key);
      value      = (*callback->valuecallback.describe)( &callback->valuecallback, item->value, &value_allocator);
      value_len  = strlen( value);

      separate = result != NULL;

      result = mulle_allocator_realloc( allocator, result, len + (separate * 2) + key_len + 3 + value_len + 1);

      if( separate)
      {
         memcpy( &result[ len], ", ", 2);
         len   += 2;
      }

      memcpy( &result[ len], key, key_len);
      len += key_len;

      memcpy( &result[ len], " = ", 3);
      len += 3;

      memcpy( &result[ len], value, value_len);
      len += value_len;

      if( key_allocator)
         mulle_allocator_free( key_allocator, key);
      if( value_allocator)
         mulle_allocator_free( value_allocator, value);
   }
   mulle__mapenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}


