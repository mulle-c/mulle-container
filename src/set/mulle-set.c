//
//  mulle_set.c
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "mulle-set.h"

#include "mulle-container-callback.h"


void   mulle_set_init( struct mulle_set *set,
                       unsigned int capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator)
{
   _mulle_set_init( (struct _mulle_set *) set, capacity, callback, allocator);

   set->_callback  = callback;
   set->_allocator = allocator;
}


struct mulle_set   *mulle_set_create( unsigned int capacity,
                                      struct mulle_container_keycallback *callback,
                                      struct mulle_allocator *allocator)
{
   struct mulle_set *set;

   if( ! allocator)
      allocator = &mulle_default_allocator;

   set = mulle_allocator_malloc( allocator, sizeof( struct mulle_set));
   mulle_set_init( set, capacity, callback, allocator);
   return( set);
}
