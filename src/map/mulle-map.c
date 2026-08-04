//
//  mulle-map.c
//  mulle-container
//
//  Copyright (c) 2018 Nat! - Mulle kybernetiK.
//  All rights reserved.
//
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

#include "mulle-map.h"

#include "mulle-container-callback.h"

#include "include-private.h"
#include <string.h>


void   _mulle_map_init( struct mulle_map *map,
                        size_t capacity,
                        struct mulle_container_keyvaluecallback *callback,
                        struct mulle_allocator *allocator)
{
//   if( ! map)
//      return;

   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle__map_init( (struct mulle__map *) map, capacity, callback, allocator);

   map->callback  = callback;
   map->allocator = allocator;
}



struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct mulle_map   *map;

   map = mulle_allocator_malloc( allocator, sizeof( struct mulle_map));
   _mulle_map_init( map, capacity, callback, allocator);
   return( map);
}


void   mulle_map_add_map( struct mulle_map *map, struct mulle_map *other)
{
   struct mulle_mapenumerator    rover;
   void                          *key;
   void                          *value;

   if( ! map || map == other)
      return;

   rover = mulle_map_enumerate( other);
   while( mulle_mapenumerator_next( &rover, &key, &value))
      _mulle_map_set( map, key, value);
   mulle_mapenumerator_done( &rover);
}
