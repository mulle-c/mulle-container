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

#include "mulle--pointermap-struct.h"
#include "mulle--pointermap-generic.h"

#include "mulle-container-operation.h"
#include "mulle-container-math.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>


// both nonowned-pointer
struct mulle_container_keyvaluecallback  mulle__pointermap_keyvaluecallback =
{
   {
      .hash     = mulle_container_keycallback_pointer_hash,
      .is_equal = mulle_container_keycallback_pointer_is_equal,
      .retain   = mulle_container_keycallback_self,
      .release  = mulle_container_keycallback_nop,
      .describe = (mulle_container_keycallback_describe_t *) mulle_container_callback_pointer_describe,
      .notakey  = mulle_not_a_pointer,
      .userinfo = NULL
   },
   {
      .retain   = mulle_container_valuecallback_self,
      .release  = mulle_container_valuecallback_nop,
      .describe = mulle_container_callback_pointer_describe,
      .userinfo = NULL
   },
};

/*
 *
 */

#pragma mark - setup and teardown


// if capacity, this is just a memset, so if you know memory is zeroed
// you can actually forego this
void   _mulle__pointermap_init( struct mulle__pointermap *p,
                                unsigned int capacity,
                                struct mulle_allocator *allocator)
{
   memset( p, 0, sizeof( *p));

   //
   // our map requires zeroes to find an end so give it ~25% holes
   //
   if( capacity)
   {
      p->_size    = capacity >= MULLE__POINTERMAP_MIN_SIZE
                        ? mulle_pow2round( capacity + (capacity >> MULLE__POINTERMAP_FILL_SHIFT))
                        : MULLE__POINTERMAP_INITIAL_SIZE;
      p->_storage = mulle__pointermap_allocate_storage_generic( p->_size, mulle_not_a_pointer, allocator);
   }
}


struct mulle__pointermap   *mulle__pointermap_create( unsigned int capacity,
                                                      size_t extra,
                                                      struct mulle_allocator *allocator)
{
   struct mulle__pointermap   *p;

   p = mulle_allocator_calloc( allocator, 1, sizeof( struct mulle__pointermap) + extra);
   _mulle__pointermap_init( p, capacity, allocator);
   return( p);
}


static inline void _mulle__pointermap_free_storage( struct mulle__pointermap *map,
                                                    struct mulle_allocator *allocator)
{
   mulle_allocator_free( allocator, map->_storage);
}


void   _mulle__pointermap_done( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator)
{
   _mulle__pointermap_free_storage( map, allocator);
}


void   _mulle__pointermap_destroy( struct mulle__pointermap *map,
                                   struct mulle_allocator *allocator)
{
   _mulle__pointermap_done( map, allocator);
   mulle_allocator_free( allocator, map);
}


void   _mulle__pointermap_reset( struct mulle__pointermap *map,
                                 struct mulle_allocator *allocator)
{
   _mulle__pointermap_reset_generic( map, mulle_not_a_pointer);
}



#pragma mark - operations

//
// returns NULL if nothing found. There is no way to distinguish with
// get, if a key/value pair exists, if NULL is a valid value!
//
void   *_mulle__pointermap_get( struct mulle__pointermap *map,
                                void *key)
{
   unsigned int   i;
   unsigned int   size;
   unsigned int   mask;
   uintptr_t      hash;
   void           *found;
   void           **storage;

   // important to not hit a NULL storage later
   // _size must be > 2 for the hash to work, otherwise we could get
   if( map->_count == 0)
      return( NULL);

   hash     = mulle_pointer_hash( key);

   storage  = map->_storage;
   size     = map->_size;
   i        = mulle__pointermap_hash_for_size( hash, size);
   mask     = size - 1;

   for(;;)
   {
      found = storage[ i];
      if( key == found)
         break;
      if( found == mulle_not_a_pointer)
         return( NULL);
      i = (i + 1) & mask;
   }

   return( storage[ i + size]);
}


void   _mulle__pointermap_shrink_if_needed( struct mulle__pointermap *map,
                                            struct mulle_allocator *allocator)
{
   if( _mulle__pointermap_is_sparse( map))
      _mulle__pointermap_shrink_generic( map, &mulle__pointermap_keyvaluecallback, allocator);
}


void   _mulle__pointermap_insert_values_for_keysv( struct mulle__pointermap *map,
                                                  void *firstvalue,
                                                  void *firstkey,
                                                  va_list args,
                                                  struct mulle_allocator *allocator)
{
   struct mulle_pointerpair   pair;

   pair.value = firstvalue;
   pair.key   = firstkey;

   while( pair.key != mulle__pointermap_keyvaluecallback.keycallback.notakey)
   {
      _mulle__pointermap_insert_pair_generic( map, &pair, &mulle__pointermap_keyvaluecallback, allocator);

      pair.value = va_arg( args, void *);
      pair.key   = va_arg( args, void *);
   }
}


#pragma mark - copying

struct mulle__pointermap   *_mulle__pointermap_copy( struct mulle__pointermap *set,
                                                     struct mulle_allocator *allocator)
{
   struct mulle__pointermap   *other;

   other = mulle__pointermap_create( _mulle__pointermap_get_count( set), 0, allocator);
   if( _mulle__pointermap_copy_items_generic( other,
                                              set,
                                              &mulle__pointermap_keyvaluecallback,
                                              allocator))
   {
      _mulle__pointermap_destroy( other, allocator);
      other = NULL;
   }
   return( other);
}


// use this only for debugging
char   *_mulle__pointermap_describe( struct mulle__pointermap *set,
                                    struct mulle_allocator *allocator)
{
   char                                *result;
   char                                *key;
   char                                *value;
   int                                 separate;
   unsigned int                        len;
   size_t                              key_len;
   size_t                              value_len;
   struct mulle__pointermapenumerator  rover;
   struct mulle_pointerpair            *item;
   struct mulle_allocator              *key_allocator;
   struct mulle_allocator              *value_allocator;

   result = NULL;
   len    = 0;
   rover  = mulle__pointermap_enumerate( set);
   while( item = _mulle__pointermapenumerator_next_pair( &rover))
   {
      key_allocator   = allocator ? allocator : &mulle_default_allocator;
      value_allocator = key_allocator;

      key        = item->key;
      key_len    = strlen( key);
      value      = item->value;
      value_len  = strlen( value);

      separate = result != NULL;

      result = mulle_allocator_realloc( allocator,
                                        result,
                                        len + (separate * 2) + key_len + 3 + value_len + 1);

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
   mulle__pointermapenumerator_done( &rover);

   if( ! result)
      return( mulle_allocator_strdup( allocator, "*empty*"));

   result[ len] = 0;
   return( result);
}

