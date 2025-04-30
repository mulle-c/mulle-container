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
                                size_t capacity,
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


struct mulle__pointermap   *mulle__pointermap_create( size_t capacity,
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
   // doesn't hurt, can help
#if MULLE__CONTAINER_HAVE_MUTATION_COUNT
   map->_n_mutations++;
#endif 
   mulle_allocator_free( allocator, map->_storage);
}


void   _mulle__pointermap_done( struct mulle__pointermap *map,
                                struct mulle_allocator *allocator)
{
   _mulle__pointermap_free_storage( map, allocator);
#ifdef DEBUG   
   mulle_memset_uint32( map, 0xDEADDEAD,sizeof( struct mulle__pointermap));
#endif 
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
   MULLE_C_UNUSED( allocator);

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
   size_t      i;
   size_t      size;
   size_t      mask;
   uintptr_t   hash;
   void        *found;
   void        **storage;

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


#pragma mark - copying

struct mulle__pointermap   *_mulle__pointermap_copy( struct mulle__pointermap *map,
                                                     struct mulle_allocator *allocator)
{
   struct mulle__pointermap   *other;

   other = mulle__pointermap_create( _mulle__pointermap_get_count( map), 0, allocator);
   _mulle__pointermap_copy_items_generic( other,
                                          map,
                                          &mulle__pointermap_keyvaluecallback,
                                          allocator);
   return( other);
}

