//
//  mulle_set.c
//  mulle-container
//
//  Created by Nat! on 17.04.16.
//  Copyright © 2016 Mulle kybernetiK. All rights reserved.
//

#include "mulle-set.h"

#include "mulle-container-callback.h"

#include <assert.h>


void   mulle_set_init( struct mulle_set *set,
                       size_t capacity,
                       struct mulle_container_keycallback *callback,
                       struct mulle_allocator *allocator)
{
   if( ! set)
      return;

   _mulle__set_init( (struct mulle__set *) set, capacity, callback, allocator);

   assert( callback);

   set->callback  = callback;
   set->allocator = allocator;
}


struct mulle_set   *mulle_set_create( size_t capacity,
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



void   mulle_set_add_set( struct mulle_set *set, struct mulle_set *other)
{
   struct mulle_setenumerator    rover;
   void                          *key;

   if( ! set || set == other)
      return;

   rover = mulle_set_enumerate( other);
   while( mulle_setenumerator_next( &rover, &key))
      _mulle_set_set( set, key);
   mulle_setenumerator_done( &rover);
}
