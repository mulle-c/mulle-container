//
//  mulle_map.c
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//
#pragma clang diagnostic ignored "-Wparentheses"

#include "mulle-map.h"

#include "mulle-container-callback.h"

#include "include-private.h"
#include <string.h>


void   _mulle_map_init( struct mulle_map *map,
                        unsigned int capacity,
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



struct mulle_map   *mulle_map_create( unsigned int capacity,
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
