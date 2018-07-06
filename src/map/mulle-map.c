//
//  mulle_map.c
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "mulle-map.h"

#include "mulle-container-callback.h"

#include "include-private.h"
#include <string.h>


void   mulle_map_init( struct mulle_map *map,
                       size_t capacity,
                       struct mulle_container_keyvaluecallback *callback,
                       struct mulle_allocator *allocator)
{
   if( ! allocator)
      allocator = &mulle_default_allocator;

   _mulle_map_init( (struct _mulle_map *) map, capacity, callback, allocator);

   map->_callback  = callback;
   map->_allocator = allocator;
}



struct mulle_map   *mulle_map_create( size_t capacity,
                                      struct mulle_container_keyvaluecallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct mulle_map   *map;

   map = mulle_allocator_malloc( allocator, sizeof( struct mulle_map));
   mulle_map_init( map, capacity, callback, allocator);
   return( map);
}

